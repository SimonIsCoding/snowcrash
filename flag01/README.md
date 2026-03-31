# SnowCrash — Level01 Writeup

## Objective

Find the password of the `flag01` user by cracking its DES hash stored in `/etc/passwd`.

---

## Steps

### 1. Reading `/etc/passwd`

```bash
cat /etc/passwd
```

The following line was found:

```
flag01:42hDRfypTqqnw:3001:3001::/home/flag/flag01:/bin/bash
```

The second field (`42hDRfypTqqnw`) is the **password hash** of the `flag01` user.

> **Note:** On modern systems, passwords are stored in `/etc/shadow` (not readable by regular users). Here, the hash is directly in `/etc/passwd`, which is a legacy and insecure configuration.

---

### 2. Identifying the hash type

The hash `42hDRfypTqqnw` is a **DES** hash (classic Unix crypt(3)).  
It can be identified by its short length (13 characters) and the absence of a `$` prefix.

| Prefix | Algorithm     |
|--------|--------------|
| *(none)* | DES (legacy) |
| `$1$`  | MD5           |
| `$5$`  | SHA-256       |
| `$6$`  | SHA-512       |

---

### 3. Installing John the Ripper (without sudo)

Since we don't have administrator rights, we compile John from source:

```bash
cd ~
mkdir tools && cd tools
git clone https://github.com/openwall/john.git
cd john/src
./configure && make -s clean && make -sj4
```

The binary will be available at `~/tools/john/run/john`.

---

### 4. Preparing the hash file

```bash
echo "flag01:42hDRfypTqqnw" > /tmp/hash.txt
```

> **Important:** Always include the username (`flag01:`) before the hash so John displays the result correctly. Without it, John replaces the username with `?`.

---

### 5. Running John the Ripper

```bash
~/tools/john/run/john /tmp/hash.txt
```

Output:

```
Loaded 1 password hash (descrypt, traditional crypt(3) [DES 256/256 AVX2])
abcdefg          (flag01)
1g 0:00:00:00 DONE
```

Password found: **`abcdefg`**

---

### 6. Logging in and retrieving the flag

```bash
su flag01
# Enter the password: abcdefg
getflag
```

---

## Why is this dangerous?

### ⚠️ DES hash: an obsolete algorithm

The DES algorithm dates back to the 1970s and has several critical weaknesses:

- **Limited key space**: passwords are truncated to **8 characters maximum**.
- **Insufficient salting**: the salt is only 12 bits, meaning only 4096 possible combinations. Two users with the same password will often have the same hash.
- **Extremely fast to crack**: tools like John the Ripper or Hashcat can test **billions of combinations per second** on modern hardware.

### ⚠️ Hash exposed in `/etc/passwd`

The `/etc/passwd` file is **readable by all users** on the system. Storing password hashes there (a legacy Unix practice) allows any authenticated user to retrieve the hashes and crack them offline, without triggering any alert.

On a modern secure system:
- Hashes are stored in **`/etc/shadow`**, readable only by `root`.
- The algorithm used is **SHA-512** (`$6$`) with a random 16-character salt.

### ⚠️ Weak password

The password `abcdefg` is trivial — it is a basic alphabetical sequence found in every common wordlist (such as `rockyou.txt`). John cracked it in under a second without even needing a custom wordlist.

---

## Vulnerability Summary

| Vulnerability | Impact |
|---|---|
| DES hash (obsolete) | Cracked in under a second |
| Hash stored in `/etc/passwd` | Readable by all users |
| Weak password (`abcdefg`) | Found in every wordlist |
| No password policy | No complexity enforced |

---

## Recommendations

- Use **SHA-512** with salting (`$6$`) to store passwords.
- Store hashes in **`/etc/shadow`** with proper permissions.
- Enforce a **strong password policy** (minimum length, complexity requirements).
- Use **SSH key-based authentication** instead of passwords.
