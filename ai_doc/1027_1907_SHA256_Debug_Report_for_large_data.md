# 1027_1907 — SHA-256 Debug Report for Large Data

## Executive Summary
- Test 1 fails during finalization (timeout). Root cause: the SHA engine is not in an active streaming state when finalization is triggered. The controller register shows STA=0 (0x...308) through the non‑first chunks, so the additional chunks are not being processed by the hardware before pad/finalize, which leads to a timeout.
- Tests 2 and 3 complete the transfer and produce the same digest, but the digest does not match the expected value printed by the test. Root cause: the “expected” hash corresponds to a different message than the one actually hashed (likely a size/pattern mismatch). Evidence: both flows produce an identical hash value (deterministic), while the log header mislabels 262,144 bytes as “400KB,” indicating the expected value may have been computed for 400KB (409,600 bytes) or for a different data pattern.
- Solutions:
  - Test 1: Use the same sessioned/EC‑style streaming sequence used in Tests 2/3, or ensure the hardware is explicitly started/continued for every chunk (re‑assert START or set the streaming/CONT bit so CTR shows STA=1 during updates). Then finalize. This prevents the finalization timeout.
  - Tests 2/3: Recompute and update the expected SHA‑256 for the actual 262,144‑byte test data (or, if you truly intended 400KB, fix the test data size to 409,600 bytes and keep the current expected). Also tighten the test pass/fail criteria so a hash mismatch fails the test.

## Context (from log)
- Zephyr: v4.2.0-3464-g3476212f72f7
- Data size: 262,144 bytes (printed as “400KB” but 262,144 bytes is 256KB)
- Chunk size: 65,536 bytes (64KB), number of chunks: 4

## Detailed Observations
### Test 1 — Chunked 400KB Hash (EC Communication Pattern)
- Driver path: “Switching to chunked processing for large input (input=65536 bytes >= 65536 bytes)” — first chunk explicitly started.
- First chunk:
  - DATALEN set to 16,384 words (65,536 bytes = 524,288 bits)
  - CTR before/after wait: 0x00000309 → 0x00000309 (READY=1, STA=0)
- Subsequent chunks (2/3/4):
  - For each, CTR stays 0x00000308 (READY=1, STA=0) before/after waits.
  - This shows the engine is not in an active/streaming state while writing non‑first chunks; no evidence of START/CONT state being active.
- Finalization:
  - total_bits=2,097,152 (256KB × 8)
  - PAD_CTR written; dummy word written
  - Timeout waiting for completion; final CTR=0x00000308 (READY=1, STA=0)
- Conclusion: The hardware never entered/maintained streaming (STA=1) for subsequent chunks. As a result, the last chunks were not processed before requesting finalization, causing the timeout.

### Test 2 — EC‑style Chunked Transfer
- Each chunk write shows CTR=0x00000318 (READY=1, STA=1) before/after waits.
- Finalization: “STA_BIT set after 0 iterations” — completes immediately.
- Hash produced: 7daca2095d0438260fa849183dfc67faa459fdf4936e1bc91eec6b281b27e4c2
- Test prints warning: expected=2312394bd99545d9de131c24efb781e765ac1aec243f2ed9347597a793a415e9, but still marks “Test 2 PASSED”.

### Test 3 — Chunked Processing Verification
- Same healthy behavior as Test 2 (CTR=0x00000318, STA=1; immediate finalization).
- Hash produced is identical to Test 2: 7daca2095d0438260fa849183dfc67faa459fdf4936e1bc91eec6b281b27e4c2
- Again “VERIFICATION FAILED” against the same expected value, but the test is marked “PASSED”.

## Root Cause Analysis
### Test 1 Finalization Timeout
- Symptom: CTR shows STA=0 for non‑first chunks and at finalization (0x...308), unlike Tests 2/3 which show STA=1 (0x...318).
- Interpretation: The engine is not in streaming/continue mode and/or is not explicitly restarted for later chunks. Additional chunk data is written when the engine is idle (STA=0), so it is not being processed before finalization, leading to no progress and a timeout.
- Likely driver sequencing issue for the “single-call chunked” path: only the first chunk asserts START; subsequent chunks rely on a streaming/CONT state that is not active (or DATALEN/START isn’t re-applied when needed).

### Tests 2/3 Hash Mismatch
- The hardware path is good (STA=1 throughout, immediate finalization) and both tests produce the same digest. This strongly indicates the mismatch is in the “expected” side, not the hardware computation.
- The header prints 262,144 bytes as “400KB” (likely an oversight). If the expected digest was calculated for 400KB (409,600 bytes) or for a different data pattern/header, a mismatch is guaranteed.
- Therefore, the expected value (2312394b...) does not correspond to the actual data hashed in Tests 2/3. The computed digest (7daca20...) is consistent and repeatable across two independently implemented flows.

## Solutions and Next Steps
### Fix Test 1 (finalization timeout)
- Preferred fix: Use the sessioned/EC‑style streaming flow for long messages (the exact flow Tests 2/3 already use):
  1) sha256_init/start (EC init phase)
  2) For each 64KB chunk: sha256_update while the engine is active (ensure CTR shows STA=1)
  3) sha256_final (EC final phase)
- Alternative (driver-level) fix: In the large‑input chunked path, either:
  - Re-assert START for each new chunk when CTR.STA=0, and reprogram DATALEN accordingly; or
  - Enable the engine’s streaming/CONT mode before the first chunk so STA remains 1 across chunk writes; ensure the engine expects the total length across chunks or is otherwise advanced per chunk.
- Success criteria: During chunk writes CTR should show STA=1 (like 0x...318), and finalization should set STA promptly without timeout.

### Fix Tests 2/3 (hash mismatch)
- Decide which message is intended:
  - If the intended data size is 256KB: recompute the expected SHA‑256 for the exact 262,144‑byte test data you are hashing and update the expected string accordingly.
  - If the intended data size is 400KB: change the test data size to 409,600 bytes and keep/recompute the expected for that message.
  - Also confirm whether any EC protocol headers/trailers are supposed to be included in the hashed message. If yes, ensure both the test computation and the “expected” were generated with the exact same byte stream (data + headers in the correct order and endianness).
- Tighten pass/fail: Treat a hash mismatch as a failed test (right now it prints a warning yet marks PASS).

### Quick Validation Checklist (no code changes to run)
- For Test 1 after applying the sessioned flow: check logs show CTR=0x...318 during updates and “STA_BIT set” on finalization, no timeout.
- For Tests 2/3 after updating the expected:
  - The printed “Got” digest should match the recomputed “Expected”.
  - Summary should read “3 passed, 0 failed,” and no “VERIFICATION FAILED” warnings.

## Evidence (key log excerpts)
- Test 1 non‑first chunks: “Before READY wait: CTR=0x00000308 (READY=1, STA=0)”
- Test 1 finalization: “Timeout waiting for SHA256 completion at finalization … Final CTR=0x00000308 (READY=1, STA=0)”
- Tests 2/3 chunks: “Before/After READY wait: CTR=0x00000318 (READY=1, STA=1)”
- Tests 2/3 finalization: “STA_BIT set after 0 iterations”
- Tests 2/3 digest (both): 7daca2095d0438260fa849183dfc67faa459fdf4936e1bc91eec6b281b27e4c2 vs expected 2312394bd99545d9de131c24efb781e765ac1aec243f2ed9347597a793a415e9

## Closing
- Test 1 fails because the engine is not kept in an active streaming state when additional chunks are written; use the sessioned flow or update the large‑input path to assert START/CONT appropriately.
- Tests 2/3 hardware behavior looks good; the digest mismatch is due to an incorrect expected value for the message actually hashed. Align the expected with the true input (size and content) or align the input with the intended size.
- No driver or test code was modified for this analysis; conclusions are based solely on the provided console log.
