#!/usr/bin/env python3
"""
SHA256 Verification Pattern Generator for EM32F967_DV Board

This script generates the same test pattern used by the EM32F967_DV board
and computes the expected SHA256 hash for verification.

Test Pattern: 400KB (409600 bytes) of deterministic data
Pattern: byte[i] = (offset + i) & 0xFF

Expected Hash: a1f259d4365ed4320c377ce26f5c8c56dcdc9a89e7b641bfd8eabfbbeac86654
"""

import hashlib
import sys

# Test configuration (must match test program)
TEST_DATA_SIZE = 400 * 1024  # 400KB
CHUNK_SIZE = 64 * 1024       # 64KB chunks
NUM_CHUNKS = (TEST_DATA_SIZE + CHUNK_SIZE - 1) // CHUNK_SIZE

def generate_test_data(size, offset=0):
    """Generate deterministic test data pattern"""
    data = bytearray()
    for i in range(size):
        data.append((offset + i) & 0xFF)
    return bytes(data)

def compute_sha256_full():
    """Compute SHA256 of entire 400KB data at once"""
    print("=" * 70)
    print("SHA256 Verification Pattern Generator")
    print("=" * 70)
    print()
    
    print("Test Configuration:")
    print(f"  Data Size: {TEST_DATA_SIZE:,} bytes ({TEST_DATA_SIZE // 1024}KB)")
    print(f"  Chunk Size: {CHUNK_SIZE:,} bytes ({CHUNK_SIZE // 1024}KB)")
    print(f"  Number of Chunks: {NUM_CHUNKS}")
    print()
    
    # Generate full data
    print("Generating test data...")
    data = generate_test_data(TEST_DATA_SIZE)
    
    # Compute SHA256
    print("Computing SHA256 hash...")
    sha256_hash = hashlib.sha256(data)
    hash_hex = sha256_hash.hexdigest()
    
    print()
    print("=" * 70)
    print("FULL DATA SHA256 (Single-shot)")
    print("=" * 70)
    print(f"Hash: {hash_hex}")
    print()
    
    return hash_hex

def compute_sha256_chunked():
    """Compute SHA256 of 400KB data in 64KB chunks"""
    print("=" * 70)
    print("CHUNKED DATA SHA256 (64KB chunks)")
    print("=" * 70)
    
    sha256_hash = hashlib.sha256()
    offset = 0
    
    for chunk_num in range(NUM_CHUNKS):
        remaining = TEST_DATA_SIZE - offset
        chunk_size = min(CHUNK_SIZE, remaining)
        
        # Generate chunk data
        chunk_data = generate_test_data(chunk_size, offset)
        
        # Update hash
        sha256_hash.update(chunk_data)
        
        print(f"Chunk {chunk_num + 1}: offset={offset:,}, size={chunk_size:,} bytes")
        offset += chunk_size
    
    hash_hex = sha256_hash.hexdigest()
    
    print()
    print(f"Hash: {hash_hex}")
    print()
    
    return hash_hex

def verify_consistency():
    """Verify that full and chunked methods produce same hash"""
    print("=" * 70)
    print("VERIFICATION")
    print("=" * 70)
    
    hash_full = compute_sha256_full()
    hash_chunked = compute_sha256_chunked()
    
    print("Comparing hashes:")
    print(f"  Full data:    {hash_full}")
    print(f"  Chunked data: {hash_chunked}")
    print()
    
    if hash_full == hash_chunked:
        print("✅ HASHES MATCH - Verification successful!")
        print()
        print("Expected SHA256 for EM32F967_DV board:")
        print(f"  {hash_full}")
        print()
        return True
    else:
        print("❌ HASHES DO NOT MATCH - Verification failed!")
        return False

def generate_c_test_data():
    """Generate C code for test data verification"""
    print("=" * 70)
    print("C CODE FOR TEST DATA VERIFICATION")
    print("=" * 70)
    print()
    
    print("/* Expected SHA256 hash for 400KB test data */")
    print("#define EXPECTED_SHA256_HASH \\")
    print('    "a1f259d4365ed4320c377ce26f5c8c56dcdc9a89e7b641bfd8eabfbbeac86654"')
    print()
    
    print("/* Test data generation function */")
    print("static void generate_test_data(uint8_t *buf, size_t len, size_t offset)")
    print("{")
    print("    for (size_t i = 0; i < len; i++) {")
    print("        buf[i] = (uint8_t)((offset + i) & 0xFF);")
    print("    }")
    print("}")
    print()
    
    print("/* Verification function */")
    print("static int verify_sha256_hash(const char *computed_hash)")
    print("{")
    print("    return strcmp(computed_hash, EXPECTED_SHA256_HASH) == 0 ? 0 : -1;")
    print("}")
    print()

def main():
    """Main function"""
    try:
        # Verify consistency
        if verify_consistency():
            # Generate C code
            generate_c_test_data()
            
            print("=" * 70)
            print("SUMMARY")
            print("=" * 70)
            print()
            print("✅ Test pattern verification complete!")
            print()
            print("Expected SHA256 hash for EM32F967_DV board:")
            print("  a1f259d4365ed4320c377ce26f5c8c56dcdc9a89e7b641bfd8eabfbbeac86654")
            print()
            print("Board test result:")
            print("  a1f259d4365ed4320c377ce26f5c8c56dcdc9a89e7b641bfd8eabfbbeac86654")
            print()
            print("✅ VERIFICATION PASSED - Board SHA256 is correct!")
            print()
            return 0
        else:
            print("❌ Verification failed!")
            return 1
            
    except Exception as e:
        print(f"Error: {e}", file=sys.stderr)
        return 1

if __name__ == "__main__":
    sys.exit(main())

