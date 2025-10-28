# 1026_1542_SHA256_chunk_Debug_Report_Copilot_v1

Author: Copilot (automated analysis)
Date: 2025-10-26

## Summary

This report analyzes the chunked SHA256 processing implemented in `samples/elan_sha/src/main_large_data_ec_sim.c` and the EM32 SHA driver at `drivers/crypto/crypto_em32_sha.c`. The goals were:

- Verify how the test application splits data into chunks for three tests (focus on Test 1).
- Count how many chunks and bytes per chunk for each test item.
- Determine whether the driver's SHA state save/restore logic is correct for chunk continuation.
- Find where `sha_reset()` is called and how many times it will be invoked during the tests.

Key findings (short):
- The sample app processes 400KB (409,600 bytes) using 64KB (65,536 bytes) as CHUNK_SIZE. This yields 7 chunks: six full 65,536-byte chunks and one final partial chunk of 409,600 - 6*65,536 = 65,600? (see calculations below). Actual counts corrected in section "Per-test chunk counts".
- Driver implements a chunked path and saves state from hardware after each chunk into `data->chunk_state` and `data->chunk_state_valid`.
- The driver's attempt to restore state by writing to `SHA_OUT` registers after calling `sha_reset()` and `sha_configure()` likely does not work because `SHA_OUT` appears to be a read-only register in hardware (documented in internal notes and earlier ai_doc). Resetting the hardware before writing back may clear internal state and writing to the output registers usually does not load the internal digest/c state on many SHA hardware designs.
- `sha_reset()` is called in the driver at these locations: init, begin_session, inside process_sha256_hardware (both for first chunk and subsequent chunks), and in crypto_em32_init. For the 3 tests run by `main()`, `sha_reset()` will be called multiple times: at session begin and inside process for each chunk (driver resets before each chunk). A conservative count is 1 reset at begin session per test plus 1 reset per chunk processing call inside `process_sha256_hardware`.

The detailed analysis and exact counts follow.

## Contract / Success criteria

Inputs: reading `main_large_data_ec_sim.c` and `crypto_em32_sha.c`.
Outputs: counts of chunks and sizes for each test, verification whether state save/restore is correct, location and count of sha_reset calls, and final recommendations.

Edge cases considered: final partial chunk size, behavior when input equals chunk size exactly, hardware register access (read-only vs write), and timeouts.

## Files reviewed

- `samples/elan_sha/src/main_large_data_ec_sim.c`
- `drivers/crypto/crypto_em32_sha.c`

(Referenced lines and excerpts are included inline where helpful.)

## Data sizes and chunking rules (from sample app)

From `main_large_data_ec_sim.c`:
- TEST_DATA_SIZE = 400 * 1024 = 409,600 bytes
- CHUNK_SIZE = 64 * 1024 = 65,536 bytes
- NUM_CHUNKS computed as ((TEST_DATA_SIZE + CHUNK_SIZE - 1) / CHUNK_SIZE)

Compute NUM_CHUNKS exactly:

- 65,536 * 6 = 393,216 bytes
- 409,600 - 393,216 = 16,384 bytes remaining for last chunk
- Therefore, NUM_CHUNKS = 7 (6 full 65,536-byte chunks + 1 final 16,384-byte chunk)

Note: earlier arithmetic in this repository's header comments was mistaken — the final chunk is 16KB (16,384 bytes), which matches the comment in the sample noting that the board produced the hash of only the LAST 16KB chunk.

## Per-test chunk counts and sizes

Test 1: test_single_shot_400kb
- Code path:
  - Allocates a single chunk buffer of CHUNK_SIZE (65,536 bytes)
  - Begins a single session via `hash_begin_session()`
  - In a while loop, it generates each chunk and calls `hash_update(&ctx, &pkt)` for each chunk
  - After the loop, it finalizes by calling `ctx.hash_hndlr(&ctx, &pkt, true)` with zero-length input
- Chunk breakdown (calculated above):
  - Chunks 1..6: each 65,536 bytes
  - Chunk 7: 16,384 bytes
  - Total chunks: 7
  - Total bytes: 6*65,536 + 16,384 = 393,216 + 16,384 = 409,600

Test 2: test_ec_chunked_transfer
- Same TEST_DATA_SIZE and CHUNK_SIZE as Test 1
- The code increments `chunk_num` and sleeps 10 ms between chunks, but otherwise the splitting is identical
- Chunks: same as Test 1 (7 chunks; six at 65,536 bytes, one at 16,384 bytes)

Test 3: test_consistency_check
- Also processes the same TEST_DATA_SIZE in CHUNK_SIZE chunks; identical splitting (7 chunks)

Summary: all three tests split 400KB into 7 chunks: six 64KB chunks (65,536 bytes each) plus a final 16KB chunk (16,384 bytes).

## Driver chunk handling behavior (high level)

Relevant driver behavior lives in `em32_sha256_handler()` and `process_sha256_hardware()`.

- The driver tracks three processing modes: legacy small-buffer (<=255B), accumulation (growable buffer for intermediate sizes), and chunked processing (for large data exceeding `SHA256_CHUNK_SIZE` which is 64KB).
- Chunked mode is enabled if a single `hash_update()` input is >= 64KB OR if accumulated total would exceed `CONFIG_CRYPTO_EM32_SHA_MAX_ACCUM_SIZE`.
- Once in chunked mode, the first chunk initializes `data->chunk_state` with SHA initial constants via `sha_init_state()` and sets `data->chunk_state_valid = true` and `data->chunk_message_bits = 0`.
- For each chunk, the driver computes `total_bits = (data->total_bytes_processed + pkt->in_len) * 8ULL` and calls `process_sha256_hardware(dev, pkt->in_buf, pkt->in_len, data->chunk_state, total_bits, is_first)`.
- `process_sha256_hardware()` behavior in summary:
  - If `is_first`:
    - calls `sha_reset(); sha_configure();` (reset and configure hardware)
  - else (subsequent chunks):
    - calls `sha_reset(); sha_configure();`
    - writes the saved `state[]` values back to `SHA_OUT` registers using `sys_write32(state[i], base + SHA_OUT_OFFSET + i*4)`
  - writes control bits for byte reversal and then programs data length registers, padding control, sets the START bit, feeds data words to `SHA_IN` with periodic READY checks, waits for completion, clears interrupt, and finally reads back `SHA_OUT` registers into `state[]` to save the new state.
  - Returns 0 on success.

## State save/restore correctness analysis (critical)

Observed implementation: after each chunk `process_sha256_hardware()` reads `SHA_OUT` registers into `state[]` at the end of the operation. For subsequent chunks it calls `sha_reset()` and `sha_configure()` and then writes back the `state[]` values into `SHA_OUT` registers before feeding the next chunk.

Issue observed and explanation:
- In many SHA hardware IPs, the output digest registers (`SHA_OUT`) are read-only and reflect the internal chaining variables after the hardware has processed the last data block. Writing to those registers typically does not load the internal chaining registers used for subsequent compression operations. Instead, there may be a dedicated interface (an internal state load register or a special command) to seed the internal state, or the hardware may support a "state continuation" command via a dedicated control register.
- The driver currently calls `sha_reset()` before writing to `SHA_OUT`. Resetting the hardware will put the internal state to initial/zero state. Then writing to `SHA_OUT` (likely read-only) will not restore the internal chaining variables. The driver's comment and earlier internal docs in `ai_doc` indicate this is a known problem: writing back to SHA_OUT after reset does not restore state and results in hardware starting from initial values. That explains the sample app's observed behavior where the produced hash equals the hash of the last 16KB chunk alone.

Evidence from repo:
- `process_sha256_hardware()` writes `state[]` to `SHA_OUT_OFFSET + i*4` on the else path (subsequent chunks) after calling `sha_reset()`.
- `sha_reset()` asserts and waits for the SHA_RST_BIT then returns, so internal state is reset.
- There are existing ai_doc notes (e.g., `ai_doc/1026_1500_SHA256_chunk_Debug_Report.md`) calling out this exact issue: "After sha_reset(), the hardware is in initial state. Writing to SHA_OUT registers (which are READ-ONLY) doesn't restore state." These prior notes corroborate the analysis.

Conclusion on correctness: The current save/restore approach is not correct. Resetting the hardware and then attempting to write to `SHA_OUT` will not restore the internal chaining state, so the hardware processes each subsequent chunk as if starting from the initial IV. That will lead to a final saved `chunk_state` that corresponds only to the last chunk processed (or at least an incorrect continuation), which matches the repository note that the board produced the hash of only the last 16KB chunk.

## Where `sha_reset()` is called and how many times during tests

Call sites in `drivers/crypto/crypto_em32_sha.c`:
- `crypto_em32_init()` — called at device init: calls `sha_reset(dev);` once at init time.
- `crypto_em32_hash_begin_session()` — calls `sha_reset(dev); sha_configure(dev);` when a hash session starts (begin_session).
- `process_sha256_hardware()` — calls `sha_reset(dev); sha_configure(dev);` on each invocation. It is invoked once per chunk in chunked mode (for every `hash_update()` call where `data->use_chunked` is true). Note: it calls `sha_reset()` both for first chunk and for subsequent chunks (the code calls sha_reset() in both is_first and else cases).

Counting resets for the sample test sequence in `main()` for Test 1 (and similarly for Tests 2 and 3):

Test 1 sequence (approx):
1. `test_single_shot_400kb()` calls `hash_begin_session()` once — this triggers one `sha_reset()` (session begin).
2. For each chunk (7 chunks) `hash_update()` calls `em32_sha256_handler()` which in chunked mode calls `process_sha256_hardware()` once per chunk. Each `process_sha256_hardware()` calls `sha_reset()` (so 7 resets here).
3. Finalize: the caller calls `ctx.hash_hndlr(&ctx, &pkt, true)` which in chunked mode returns the `data->chunk_state` without calling hardware (no extra reset at finalize).
4. `hash_free_session()` will clear state but does not call sha_reset() (it frees buffers), so no additional reset here.

Total sha_reset() calls for Test 1 running on a fresh device that already had sys init: 1 (begin_session) + 7 (one per chunk) = 8 resets.

If you include device initialization time (device init called at boot time), there is an extra `sha_reset()` from `crypto_em32_init()` which runs at POST_KERNEL device initialization. So overall across the whole boot+test run, you may see 9 resets (1 at device init + 1 at begin session + 7 during chunk processing). For Tests 2 and 3, the same pattern repeats, adding the begin_session reset + per-chunk resets for each test.

Per-test reset counts (excluding boot device init):
- Test 1: 1 (begin) + 7 (chunks) = 8
- Test 2: 1 + 7 = 8
- Test 3: 1 + 7 = 8

If you count the init-level reset, add 1 more global.

## Reproduction of observed incorrect hash

Repo comments indicate the board currently produces the hash `a1f2...6654` which is the SHA256 of only the last 16KB chunk. This symptom is exactly what you'd expect if:
- The driver resets hardware before each chunk and fails to properly seed the hardware internal chaining state for continuation, so the hardware only processes the last chunk starting from the initial IV.

## Recommendations / Fixes

Short term (quick, low-risk):
- Do not call `sha_reset()` before writing continuation state back into the hardware. If the purpose of reset is to clear previous operation, consider a different sequence:
  1. If hardware supports a "load state" operation, use its documented register/command to load the chaining variables.
  2. If `SHA_OUT` is read-only and cannot be used to load state, implement a software-side accumulation of whole message into `accum_buf` when message size permits (this is already present) or implement HMAC-chaining in software for continuation.
- Add driver logging that prints when chunked mode is engaged and prints the `chunk_state` words in hex after each chunk. This aids debugging.

Longer term (recommended correct approach):
- Review the SOC/hardware spec for a documented state-load mechanism. Many SHA engines provide either a dedicated state load register or a different control sequence to set the chaining variables (or expect you to write them into an internal RAM via a different interface). Implement that interface rather than writing to `SHA_OUT`.
- If hardware lacks state-load support, the driver must avoid resetting hardware between chunk operations. Instead, keep the hardware running across chunks, feeding partial data and continuing the operation (if the hardware supports incremental feeding and continuing). That means calling `sha_reset()` only once at session begin and never again between chunks; then feed each chunk sequentially, adjusting data length and padding correctly for finalization. However, the current driver relies on resetting to reconfigure padding and data length per chunk, so this would require careful control of padding and length registers.
- Alternatively, perform chunk continuation entirely in software: maintain software H state and process blocks via software compression function for continuation. This is heavy but guaranteed correct.

Concrete minimal fix suggestion (driver change):
- Inside `process_sha256_hardware()`, for the else path (subsequent chunks), remove the call to `sha_reset()` before attempting state restore, and instead rely on a state-load mechanism if available. If none is available, change design: either avoid writing to `SHA_OUT` as a restore method or document as unsupported.

## Evidence snippets (key lines)

From `process_sha256_hardware()`:

```c
    if (is_first) {
        sha_reset(dev);
        sha_configure(dev);
    } else {
        sha_reset(dev);
        sha_configure(dev);
        for (int i = 0; i < 8; i++) {
            sys_write32(state[i], config->base + SHA_OUT_OFFSET + i * 4);
        }
    }
```

From `em32_sha256_handler()` (entry to chunked path):

```c
    if (is_first) {
        sha_init_state(data->chunk_state);
        data->chunk_state_valid = true;
        data->chunk_message_bits = 0;
    }

    uint64_t total_bits = (data->total_bytes_processed + pkt->in_len) * 8ULL;
    int ret = process_sha256_hardware(dev, pkt->in_buf, pkt->in_len,
                                      data->chunk_state, total_bits, is_first);
```

## Checklist mapping to user request

- Study files: done (read both files)
- Verify chunk operation for 3 test items: done; all use same 400KB/64KB chunking → 7 chunks
- Focus on Test 1: done; described exact chunk sizes and counts
- State save/restore correctness: analyzed and concluded it's incorrect (writing SHA_OUT after reset likely does not restore internal chaining variables)
- Reset function call locations and count: enumerated and counted per test (8 resets per test, plus init-level reset)
- Create complete analysis report at `./ai_doc/1026_1542_SHA256_chunk_Debug_Report_Copilot_v1.md`: file created

## Next steps / proposed follow-ups

- If you want, I can implement the minimal driver change (e.g., remove sha_reset before state restore and attempt seeding via other registers if the spec supports it) and add unit tests that compare single-shot vs chunked hashing for the 400KB vector.
- Alternatively, I can add more logging to the driver to dump `chunk_state` hex words after every chunk so you can capture values on hardware runs.


## Appendix - arithmetic

- TEST_DATA_SIZE = 400 * 1024 = 409,600
- CHUNK_SIZE = 64 * 1024 = 65,536
- 409,600 / 65,536 = 6 remainder 16,384 → 7 chunks



