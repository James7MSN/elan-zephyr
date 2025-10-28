# Zephyr Power Management – Study & Driver Index (EM32F967)

Date: 2025-10-27  
Scope: This covers drivers in this repository (elan-zephyr) and how to apply Zephyr Power Management (PM) on EM32F967.

---

## 1) Executive Summary

- Zephyr provides two complementary PM layers:
  - System PM (SoC sleep/deep sleep states managed from the idle thread via a policy)
  - Device PM (per‑device suspend/resume; plus runtime PM via reference counting)
- In this repo, several drivers already touch SoC power/clock domains (clock gating, backup/power regs, USB suspend ISR). We can integrate them with Zephyr PM for coordinated, low‑power behavior.
- The EM32F967 SoC provides power domain and standby controls (PD_REG, WAKEUP_REG, SIP‑PD gating, etc.) captured in pm_spec CSVs. These map cleanly to Zephyr System PM states with minimal glue.

---

## 2) Driver Index (elan-zephyr/drivers)

Top-level categories and key files:

- bbram
  - bbram_em32.c – BBRAM/backup domain; accesses EM32 PWR regs (backup enable, voltage detect, wakeup pin)
- clock_control
  - clock_control_em32_ahb.c / clock_control_em32_apb.c – AHB/APB clock gating and frequency setup
- crypto
  - crypto_em32_sha.c – EM32 SHA256 accelerator
- entropy
  - entropy_em32_trng.c – TRNG driver
- flash
  - flash_em32.c – Flash driver
- gpio
  - gpio_em32.c – GPIO driver (wake sources likely configured here)
- hwinfo
  - hwinfo_em32.c – Device ID, etc.
- pinctrl
  - pinctrl_em32.c – Pin mux/config; relevant to low power pin states
- serial
  - uart_elandev.c – UART driver
- spi
  - spi_em32.c / spi_elandev_spi2.c – SPI controllers and helper context (runtime PM hooks observed)
- usb
  - udc/udc_elan_elandev.c – USB device controller; has suspend ISR and events
- watchdog
  - wdt_em32.c – Watchdog; should be coordinated on suspend/deep sleep

PM‑relevant touch points found in this repo:
- drivers/spi/spi_context.h – uses `pm_device_runtime_get/put()` for CS GPIO ports (device runtime PM)
- drivers/bbram/bbram_em32.c – manipulates PWR base registers (backup domain enable)
- drivers/usb/udc/udc_elan_elandev.c – suspend ISR and device suspend state
- Spec resources under em32f967_spec/pm_spec – CSVs documenting PD_REG/WAKEUP regs
- Spec examples in EM32F967_Complete_Specification_v3.0.md – entering standby/deep sleep and wakeup

---

## 3) Zephyr Power Management Basics

- System PM
  - Kernel calls PM policy from idle to select a state (`pm_policy_next_state`), then enters it (`pm_state_set`).
  - States (typical): runtime idle (WFI), suspend to idle/light sleep, standby/deep sleep, soft off.
  - Exit hooks restore clocks/IO/SoC context.
- Device PM
  - Drivers implement suspend/resume actions; framework coordinates device order when entering a system state.
  - Runtime PM lets devices clock‑gate/power‑gate when not in use, with `pm_device_runtime_get/put`.
- Kconfig
  - CONFIG_PM=y
  - CONFIG_PM_DEVICE=y (device PM)
  - CONFIG_PM_DEVICE_RUNTIME=y (runtime PM)
  - Optional: CONFIG_PM_POLICY_RESIDENCY=y (default Zephyr policy based on next timeout)

---

## 4) What exists here already (signals of PM integration)

- SPI CS GPIO runtime PM: `pm_device_runtime_get/put()` integration indicates this repo uses Zephyr’s runtime PM API in common helpers.
- USB UDC suspend ISR: receives USB suspend, sets internal suspended state and raises events; can be a wakeup source and can coordinate with PM system.
- BBRAM driver accesses PWR control registers, indicates SoC power/backup integration code exists.
- Clock control drivers: AHB/APB gating are natural building blocks to reduce clocks across sleep states.
- SoC PM spec CSVs: comprehensive mapping of PD_REG, WAKEUP_REG, SIP‑PD gating bits, LDO control, warmup counters, etc.

---

## 5) How to enable Zephyr PM in this repo

- prj.conf (example):
  - CONFIG_PM=y
  - CONFIG_PM_DEVICE=y
  - CONFIG_PM_DEVICE_RUNTIME=y
  - Optional: CONFIG_PM_POLICY_RESIDENCY=y (or custom policy)
- Device Tree:
  - Mark capable wakeup GPIOs; ensure clock/power domains are properly described if bindings are used.
- Build system:
  - Ensure SoC PM glue (`soc/.../pm.c`) is compiled when CONFIG_PM.

---

## 6) SoC PM glue for EM32F967 (System PM)

Implement SoC integration in `soc/elan/em32f967/pm.c`:

- pm_state_set(): map Zephyr state → EM32 power switch entries (`PowerDownSwitch(Sx)`), program warmup counts/LDO/cache/clock paths, configure wakeup pins.
- pm_state_exit_post_ops(): after wake, re‑establish clocks, caches, pinctrl default states if needed.
- Optionally, custom policy: implement `pm_policy_next_state()` or rely on residency policy (chooses deepest eligible state based on next timer deadline and minimum residencies).

Minimal skeletons:

<augment_code_snippet mode="EXCERPT">
````c
/* Called by kernel to enter a power state */
int pm_state_set(enum pm_state state, uint8_t substate_id)
{
    switch (state) {
    case PM_STATE_SUSPEND_TO_IDLE: /* WFI/light sleep */
        __WFI();
        return 0;
    case PM_STATE_STANDBY: /* EM32 Standby1/2 */
        PowerDownSwitch(PDSW1);
        return 0;
    default:
        return -ENOTSUP;
    }
}
````
</augment_code_snippet>

<augment_code_snippet mode="EXCERPT">
````c
void pm_state_exit_post_ops(enum pm_state state, uint8_t substate_id)
{
    /* Reinit clocks/pinctrl if deep state */
    /* e.g., SetMainFreq2(...); pinctrl_apply_state(...); */
}
````
</augment_code_snippet>

Map Standby1/2 using EM32 registers from pm_spec CSVs (PD_REG, SIP‑PD gating, LDO Idle, HIRC/SIRC PD, RAM retention masks, WAKEUP source config).

---

## 7) Device PM patterns for drivers

- If a device can be turned off or suspended when idle, implement runtime PM and device actions.
- Typical runtime PM usage in clients (already present in spi_context.h):

<augment_code_snippet mode="EXCERPT" path="drivers/spi/spi_context.h">
````c
/* Power-manage CS GPIO ports */
ret = pm_device_runtime_get(cs_gpio->port);
// ... use GPIO ...
ret = pm_device_runtime_put(cs_gpio->port);
````
</augment_code_snippet>

- Typical driver action handler (newer API pattern):

<augment_code_snippet mode="EXCERPT">
````c
static int mydev_pm_action(const struct device *dev,
                           enum pm_device_action action)
{
    switch (action) {
    case PM_DEVICE_ACTION_SUSPEND: /* gate clocks, save context */
        return 0;
    case PM_DEVICE_ACTION_RESUME:  /* ungate, restore */
        return 0;
    default:
        return -ENOTSUP;
    }
}
````
</augment_code_snippet>

- Register PM with device init macro if using device PM helpers.

---

## 8) USB and Wakeup integration

- `drivers/usb/udc/udc_elan_elandev.c` provides a suspend ISR that sets suspended state and submits `UDC_EVT_SUSPEND`.
- Tie this into System PM by:
  - Allowing USB SUSPEND to request deeper system state if no active transfers.
  - Mark USB as a wakeup source (remote wakeup or resume signaling); ensure WAKEUP_REG config.

---

## 9) Clocks, Pinctrl, and Retention

- Clock control drivers can lower clocks before deep entry and restore after.
- Pinctrl: ensure low‑power pin states (pulls, drive) are applied before deep sleep and re‑applied on resume.
- RAM retention: pm_spec CSVs include SRAM save masks; choose minimal retention to reduce leakage.

---

## 10) Recommended next steps

1. Add `soc/elan/em32f967/pm.c` implementing `pm_state_set()` and `pm_state_exit_post_ops()` with EM32 register programming from pm_spec.
2. Enable PM in project prj.conf: `CONFIG_PM=y`, `CONFIG_PM_DEVICE=y`, `CONFIG_PM_DEVICE_RUNTIME=y` (and optionally `CONFIG_PM_POLICY_RESIDENCY=y`).
3. Audit drivers for runtime PM opportunities:
   - UART/SPI/USB: gate clocks when idle; resume on demand.
   - TRNG/Flash: ensure safe suspend/resume sequences.
   - Watchdog: define behavior during deep sleep.
4. Define wakeup sources and policies (GPIO, USB resume, RTC) using WAKEUP_REG.
5. Measure current across states; tune residency thresholds and warmup counters.

---

## 11) Appendix – EM32 Power/PM references in repo

- em32f967_spec/pm_spec/*.csv – PD_REG/WAKEUP registers and fields (Standby1/2, SIP‑PD gating, LDO controls, SRAM retention)
- em32f967_spec/EM32F967_Complete_Specification_v3.0.md – power examples (sleep/deep sleep), wakeup flow
- em32f967_spec/SHA256_test_code_20251014/testcodes/encrypt/system.c – `PowerDownSwitch()` sequence illustrating SoC power switching
- drivers/bbram/bbram_em32.c – PWR base offsets used in backup control
- drivers/spi/spi_context.h – runtime PM calls example

---

## 12) Notes on broader Zephyr tree

This report indexes the drivers present in this repository (elan-zephyr). If you want me to index the entire Zephyr workspace at /home/james/zephyrproject (including the upstream Zephyr tree and modules), please confirm and I will extend the scan accordingly.

