# Security

The **Security** module provides hashing, checksum, and message digest capabilities across standard algorithms including Adler-32, CRC-32, FNV-1/FNV-1a, MD5, and SHA-256.

```c
#include "Cosmeron/Modules/Security/Security.h"
```

---

# Overview

Security features are organized into three sub-packages:

| Sub-package | Description |
| --- | --- |
| `Checksum` | Fast non-cryptographic checksum algorithms (Adler-32, CRC-32, FNV-1/1a) |
| `Crypto` | Cryptographic hash computations (MD5, SHA-256) |
| `Digest` | Common digest buffer representation, hex encoding, and comparison |

---

# API reference

## Security_Digest_Init

Initializes a digest instance with raw binary bytes.

### Syntax

#### Direct form

```c
OPSTATUS status = Security_Digest_Init(&digest, bytes, length);
```

#### Macro form

```c
OPSTATUS status = SECURITY_DIGEST_FUNC(Init)(&digest, bytes, length);
```

---

### Parameters

| Parameter | Explanation |
| --- | --- |
| `digest` | Pointer to `SECURITY_DIGEST_TYPE(TDigest)` |
| `bytes` | Pointer to input bytes |
| `length` | Number of bytes to copy (up to maximum buffer capacity) |

---

### Return value

Returns `STATUS_CONST(SUCCESS)` on success, `STATUS_CONST(INVALID_ARGUMENT)` if pointers are null, or `STATUS_CONST(INSUFFICIENT_SPACE)` if length exceeds capacity.

---

### Remarks

Copies up to `SECURITY_DIGEST_CONST(MAX_BYTES)` into the digest object and sets its length.

---

### Example

```c
TDigest(myDigest);
const uint8_t raw[4] = {0x12, 0x34, 0x56, 0x78};
Security_Digest_Init(&myDigest, raw, 4U);
```

---

## Security_Digest_ToHex

Formats the digest bytes as a hexadecimal string.

### Syntax

#### Direct form

```c
OPSTATUS status = Security_Digest_ToHex(&digest, dest, capacity, uppercase);
```

#### Macro form

```c
OPSTATUS status = SECURITY_DIGEST_FUNC(ToHex)(&digest, dest, capacity, uppercase);
```

---

### Parameters

| Parameter | Explanation |
| --- | --- |
| `digest` | Pointer to the digest object |
| `dest` | Buffer to receive the null-terminated hex string |
| `capacity` | Byte size of the destination buffer |
| `uppercase` | When `true`, hex digits use uppercase letters `A-F` |

---

### Return value

Returns `STATUS_CONST(SUCCESS)` on success, `STATUS_CONST(INVALID_ARGUMENT)` if arguments are invalid, or `STATUS_CONST(INSUFFICIENT_SPACE)` if the buffer is too small.

---

### Remarks

The required destination capacity is at least `(length * 2) + 1` bytes to include the null terminator.

---

### Example

```c
char hex[16];
Security_Digest_ToHex(&myDigest, hex, sizeof(hex), false);
```
