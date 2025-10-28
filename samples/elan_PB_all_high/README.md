# ELAN EM32F967 PB8-PB15 HIGH Test

Simple GPIO test application that sets PB8-PB15 to output HIGH and keeps them high continuously.

## Purpose

This application demonstrates basic GPIO output functionality by:
- Configuring PB8-PB15 as output pins
- Setting all these pins to HIGH (3.3V)
- Keeping them HIGH continuously
- Monitoring their status every 5 seconds

## Pin Configuration

### Output Pins (Always HIGH):
- **PB8**: Output HIGH (3.3V)
- **PB9**: Output HIGH (3.3V)  
- **PB10**: Output HIGH (3.3V)
- **PB11**: **SKIPPED** (Used for SW1 button)
- **PB12**: Output HIGH (3.3V)
- **PB13**: Output HIGH (3.3V)
- **PB14**: Output HIGH (3.3V)
- **PB15**: Output HIGH (3.3V)

**Note**: PB11 is intentionally skipped as it's used for the SW1 button on the development board.

## Features

1. **Simple Setup**: Minimal code for basic GPIO output testing
2. **Status Monitoring**: Displays pin status every 5 seconds
3. **Self-Verification**: Reads back pin values to confirm they're HIGH
4. **Continuous Operation**: Keeps pins HIGH indefinitely
5. **Error Checking**: Reports any configuration or operation failures

## Building and Running

1. Navigate to the project directory:
   ```bash
   cd samples/elan_PB_all_high
   ```

2. Build the project:
   ```bash
   west build -b 32f967_dv
   ```

3. Flash to the board:
   ```bash
   west flash
   ```

4. Connect to UART console to view output

## Expected Behavior

### Console Output
```
ELAN EM32F967 PB8-PB15 HIGH Test
=================================
This program sets PB8-PB15 to HIGH and keeps them high

GPIOB device ready
Configuring PB8-PB15 as outputs and setting HIGH...
PB8 configured as output and set HIGH
PB9 configured as output and set HIGH
PB10 configured as output and set HIGH
PB12 configured as output and set HIGH
PB13 configured as output and set HIGH
PB14 configured as output and set HIGH
PB15 configured as output and set HIGH

PB8-PB15 configuration complete!
All pins should now be outputting HIGH (3.3V)

=== Status Check 1 ===
PB15-PB8: 11101111 (0xEF) - Expected: 11111111 (0xFF)
✅ SUCCESS: All configured pins are HIGH
```

**Note**: The readback shows 0xEF (not 0xFF) because PB11 is not configured as output.

### Hardware Verification

Use a multimeter or oscilloscope to verify:
- **PB8, PB9, PB10, PB12, PB13, PB14, PB15**: Should measure ~3.3V
- **PB11**: Should remain in its default state (not driven by this application)

## Use Cases

- **GPIO Hardware Testing**: Verify GPIO output functionality
- **Pin Voltage Testing**: Check if pins can properly drive HIGH
- **Board Bring-up**: Simple test for new hardware
- **LED Testing**: Connect LEDs to pins to visually verify output
- **Logic Analyzer**: Capture and verify HIGH signals

## Troubleshooting

- **No Output**: Check GPIO driver configuration in `prj.conf`
- **Wrong Voltage**: Verify board power supply and pin connections
- **Console Issues**: Ensure UART is properly connected and configured
- **Build Errors**: Check that all required modules are included

## Comparison with elan_gpio

This application is simpler than `samples/elan_gpio`:
- **Single Function**: Only sets pins HIGH (no testing modes)
- **Fewer Pins**: Only PB8-PB15 (not PA pins)
- **No Button**: No SW0 button interaction
- **Continuous**: Runs continuously without user interaction

Perfect for basic GPIO output verification and hardware testing!
