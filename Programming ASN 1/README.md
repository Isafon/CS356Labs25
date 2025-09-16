CS356 - PA1 Tutorial
Welcome to Isa's Read Me file!
This file contains instructions to run the quick tests for the cipher program.
Author's Note: I took ECE456 and most of the content in this class closely relates to that class,
therefore, I am submitting how I did for that class, nobody said overexplaining hurt anyone.
---------------------------------------------------------------------------------
Executable: "cipher"
Standard: C++17
Platforms: Linux/macOS

What this program does:

Encrypts or decrypts files using
- Block Cipher (B) — 16-byte blocks: Pad (0x81) -> XOR with 16-byte key -> Conditional swap
Decrypt: Reverse swap -> XOR -> strip 0x81 padding (only at the end).
- Stream Cipher (S) — XORs each byte of input with the key bytes (key cycles).
Program is silent on success and only writes to the output file.
On errors, it prints an exact message and exits with status 1.

Files in this folder:
- Makefile = builds the cipher executable
- cipher.cpp = implementation (no external libraries)

The assignment allows any layout. I used one .cpp file for simplicity.

Build:
make clean 2>/dev/null || true
make
Result: ./cipher

Usage:
./cipher <Type> <InputFile> <OutputFile> <KeyFile> <Mode>

<Type>: B (Block) or S (Stream)
<Mode>: E (Encrypt) or D (Decrypt)
<InputFile>: plaintext (for E) or ciphertext (for D)
<OutputFile>: file to write (ciphertext for E, plaintext for D)
<KeyFile>:
  - Block: exactly 16 bytes, no newline (example: COMPUTER76543210)
  - Stream: any length, no newline

Required error messages (exit code = 1):
- Invalid Function Type
- Invalid Mode Type
- Input File Does Not Exist
- Key File Does Not Exist
- (Wrong arg count → exit 1, no message)

I made two sample plaintexts + keys (for my testing only):
printf $'HELLO\nWORLD\n' > t1.txt        # includes real newlines (size 12)
printf '123456789ABCDEF01' > t2.txt      # 17 bytes, no newline

Keys:
printf 'COMPUTER76543210' > key_block.txt
printf 'TheKeySizeForStreamCipherMayBeLongerThan16Bytes' > key_stream.txt

Sanity checks (I expect: 12, 17, 16)
wc -c t1.txt t2.txt key_block.txt

Examples:

Stream cipher (S): encrypt -> decrypt -> verify:

./cipher S t1.txt s_e.bin key_stream.txt E
./cipher S s_e.bin t1_roundtrip.txt key_stream.txt D
diff -u t1.txt t1_roundtrip.txt && echo "STREAM OK: t1"

./cipher S t2.txt s2_e.bin key_stream.txt E
./cipher S s2_e.bin t2_roundtrip.txt key_stream.txt D
diff -u t2.txt t2_roundtrip.txt && echo "STREAM OK: t2"

Block cipher (B): encrypt -> decrypt -> verify:
Uses 16-byte key: COMPUTER76543210

./cipher B t1.txt b_e.bin key_block.txt E
./cipher B b_e.bin t1_roundtrip_b.txt key_block.txt D
diff -u t1.txt t1_roundtrip_b.txt && echo "BLOCK OK: t1"

./cipher B t2.txt b2_e.bin key_block.txt E
./cipher B b2_e.bin t2_roundtrip_b.txt key_block.txt D
diff -u t2.txt t2_roundtrip_b.txt && echo "BLOCK OK: t2"

My Quick test plan (ASN1 compliance):
1. Build clean: make clean && make (no console output except compiler lines)
2. Round-trip (S & B): diff original vs. decrypted -> must match byte-for-byte
3. Padding:
   - 12-byte (t1.txt) -> padded to 16 on encrypt; removed on decrypt
   - 17-byte (t2.txt) -> encrypt to 32 bytes; decrypt back to 17
4. Newlines preserved: t1.txt newlines survive round-trip
5. Deterministic: same input+key -> same ciphertext
6. Silence on success: no stdout/stderr when command succeeds
7. Errors print exact text + exit 1

Packaging (what to submit for Isa's memory, she is gonna forget):

Only include source files and Makefile (no test files/keys/binaries).
make clean
tar -czvf ../Last_Name-First_Name-PA1.tar.gz .
tar -tzvf ../Last_Name-First_Name-PA1.tar.gz   # verify: files at archive root, no extras

Troubleshooting?:

- t1.txt size looks wrong (e.g., 14 bytes)
  You likely have literal \n characters or Windows CRLF endings. I know this bc I had this issue.
  Recreate with:
  printf $'HELLO\nWORLD\n' > t1.txt
  hexdump -C t1.txt   # look for 0a (LF), not 5c 6e (\n)

- Block key length != 16
  Recreate with no trailing newline:
  printf 'COMPUTER76543210' > key_block.txt
  wc -c key_block.txt   # must be 16

- Program prints extra text
  Remove/disable any debug prints. Only the specified error messages may appear.

  Congrats, you completed the lab!! Give yourself a pat on the back girl!