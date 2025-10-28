# SHA-256 400KB+ Padding Issue — Root Cause and Fix

Date: 2025-10-28 12:03
Owner: crypto_em32_sha driver

## Executive summary
- Issue: Padding failures for non-word-aligned sizes when hashing large inputs in chunked mode (e.g., 400KB+1/2/3).
- Root cause: PAD_CTR was programmed at finalization time and a dummy last word was sent, causing the hardware to treat the dummy as the final word. This produced a one-case-late digest shift. Earlier code also mixed bit- vs byte-based valid-byte derivation.
- Fix: Program PAD_CTR on the first chunk (before STR) using the total message size. Stream all data (including the final partial word content) as normal across chunks. On finalize, do not write PAD_CTR again; simply wait for STA and read the digest.
- Result: All tests pass (6/6). 400KB baseline and 400KB+1/2/3 match Python reference digests.

---

## Background
We simulate Chrome EC firmware transfer by hashing large buffers in 64KB chunks under tight RAM budget. The EM32F967 SHA-256 engine supports:
- CTR: control (WR/ RD byte reverse, STR, READY, STA, INT_CLR)
- DATALEN: total input words for the entire message (set once)
- PAD_CTR: padding control (bits [4:0] = pad_packet words, bits [9:8] = valid_byte)
- IN/OUT: input and digest registers

SHA-256 padding requires appending 0x80, zeros, and a 64-bit big-endian length to reach 512-bit boundaries. The engine uses `valid_byte` to mark how many bytes in the last 32-bit word are meaningful (0→4B, 1→1B, 2→2B, 3→3B), and `pad_packet` to control the number of additional 32-bit padding words.

---

## Symptoms
- 400KB (exact block multiple) passed.
- 400KB+1/2/3 initially failed with a consistent pattern:
  - Test N produced the digest expected for Test N-1 (off-by-one late). This indicated the real last partial word was not the one hardware used for padding.

---

## Root cause analysis
1) PAD_CTR timing
- Previous (buggy): PAD_CTR was set at finalize, then a zero “dummy” word was written. The hardware treated that dummy as the last input word (under the `valid_byte` mask), not the actual data tail.
- Required: PAD_CTR must be programmed before the engine consumes the final word of the message. The working reference implementation did this by setting PAD_CTR prior to STR and then streaming all words (including the partial last word) inside one operation.

2) Valid-byte derivation
- Previous code sometimes derived `valid_enc` from bit counts or chunk-local state, yielding incorrect values for byte-aligned totals in chunked mode.
- Correct approach: drive `valid_enc = total_bytes % 4` from the overall message size (bytes).

---

## Final design and implementation
We aligned chunked mode with the reference single-run sequence while supporting multi-chunk streaming:

Sequence for first chunk (when total length is known):
1. Reset + configure CTR with byte reversal.
2. Program DATALEN = ceil(total_bytes / 4).
3. Compute and program PAD_CTR from total_bytes:
   - `bmod = (total_bytes % 64) * 8`
   - `pad_packet = (bmod < 448) ? ((512 - bmod - 64)/32) : ((512 - bmod + 448)/32)`
   - `valid_enc = total_bytes % 4` (0..3)
4. Set STR (start) BEFORE sending any data.
5. Stream data words for the chunk; hardware will continue across subsequent chunks.

Subsequent chunks:
- Only stream data words and observe READY pacing; no CTR writes.

Finalize:
- Do not re-program PAD_CTR. Wait until STA is asserted, clear INT, and read OUT registers.

Key code changes (drivers/crypto/crypto_em32_sha.c):
- Program PAD_CTR on first chunk if `have_expected_total` is true (same frame where DATALEN is configured) and before setting STR.
- Removed “withhold tail bytes then write after PAD_CTR” logic.
- Finalization in chunked mode now just waits for STA and reads the digest.

---

## Register programming details
- DATALEN (words): `words = (total_bytes + 3) / 4`
  - 409,600 → 102,400 words
  - 409,601/2/3 → 102,401 words
- PAD_CTR fields:
  - valid_byte = total_bytes % 4
  - pad_packet = derived from bmod as above
  - Examples:
    - 409,600: bmod=0,  pad_packet=14, valid=0 → PAD_CTR=0x0000000e
    - 409,601: bmod=8,  pad_packet=13, valid=1 → PAD_CTR=0x0000010d
    - 409,602: bmod=16, pad_packet=13, valid=2 → PAD_CTR=0x0000020d
    - 409,603: bmod=24, pad_packet=13, valid=3 → PAD_CTR=0x0000030d

---

## Console evidence (passing run)
400KB baseline (selected lines):
```
[dbg] First chunk: DATALEN set to 102400 words (3276800 bits)
[dbg] First chunk: PAD_CTR set early (bmod=0, pad_packet=14, pad_ctrl=0x0000000e)
[dbg] First chunk: SHA_STR set, starting operation
...
[inf] Hash: 870130e6ddddd5d74acfa65ae6e060c0bdc135930cc55562c696737c6d046aee
```

409,601 bytes:
```
[dbg] First chunk: DATALEN set to 102401 words (3276808 bits)
[dbg] First chunk: PAD_CTR set early (bmod=8, pad_packet=13, pad_ctrl=0x0000010d)
...
[inf] Hash: 0ae9a6992c813d5cc36e2e6486abe52f66181996eb160677e8f4d86dc5ab2dc5
```

409,602 bytes:
```
[dbg] First chunk: DATALEN set to 102401 words (3276816 bits)
[dbg] First chunk: PAD_CTR set early (bmod=16, pad_packet=13, pad_ctrl=0x0000020d)
...
[inf] Hash: eb1b5c717787585e081a6a49175f7b744db3f072c75413aef3e8f756518c1238
```

409,603 bytes:
```
[dbg] First chunk: DATALEN set to 102401 words (3276824 bits)
[dbg] First chunk: PAD_CTR set early (bmod=24, pad_packet=13, pad_ctrl=0x0000030d)
...
[inf] Hash: 759b9d2304a64cdf25679501b73cbc89ee612a3eb63aa56b85b888630cf313e9
```

All tests summary:
```
[inf] Test Summary: 6 passed, 0 failed
```

---

## Expected hashes (Python reference)
- 409,600: 870130e6ddddd5d74acfa65ae6e060c0bdc135930cc55562c696737c6d046aee
- 409,601: 0ae9a6992c813d5cc36e2e6486abe52f66181996eb160677e8f4d86dc5ab2dc5
- 409,602: eb1b5c717787585e081a6a49175f7b744db3f072c75413aef3e8f756518c1238
- 409,603: 759b9d2304a64cdf25679501b73cbc89ee612a3eb63aa56b85b888630cf313e9

---

## Verification checklist
- [x] DATALEN set once on first chunk to ceil(total_bytes/4)
- [x] PAD_CTR programmed before STR on first chunk, derived from total_bytes
- [x] No special tail-byte withholding or dummy trailing word
- [x] READY pacing observed during streaming
- [x] Finalization only waits for STA and reads digest
- [x] 400KB baseline and +1/+2/+3 match reference digests

---

## Lessons learned
- In this hardware, PAD_CTR must be visible to the engine before it ingests the final input word; late programming reinterprets a dummy word as the last data, corrupting padding.
- Keep padding math byte-driven (total_bytes), not bit-count residue derived from chunk-local progress.
- For multi-chunk flows, mirror the working single-run order as closely as possible: DATALEN → PAD_CTR → STR → stream all words → wait STA.

---

## Appendix: Register fields (for reference)
- CTR:
  - WR_REV, RD_REV: byte-reversal enables
  - STR: start; STA: operation complete; READY: ready for next write; INT_CLR: interrupt clear
- PAD_CTR:
  - bits [9:8] valid_byte = total_bytes % 4 (0→4B, 1→1B, 2→2B, 3→3B)
  - bits [4:0] pad_packet: number of 32-bit padding words between 0x80 and length field

---

## File references
- Driver: drivers/crypto/crypto_em32_sha.c
- Test app: samples/elan_sha/src/main_large_data_ec_sim.c
- Working reference (single-run): em32f967_spec/SHA256_padding_ok/crypto_em32_sha_padding_ok_1020_git.c

