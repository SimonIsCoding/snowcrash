# SnowCrash — Level 00

## Vulnerability

A file owned by `flag00` is readable by all users on the system.
It contains a password obfuscated with a simple ROT11 cipher.

---

## Walkthrough

### 1. Find files owned by `flag00`

```bash
find / -user flag00 2>/dev/null
```

```
/usr/sbin/john
/rofs/usr/sbin/john
```

### 2. Read the file

```bash
cat /usr/sbin/john
```

```
cdiiddwpgswtgt
```

### 3. Decrypt the password

The string `cdiiddwpgswtgt` is encoded with **ROT11**: each letter is shifted 11 positions forward in the alphabet.

```
c d i i d d w p g s w t g t   (encoded)
↓ ↓ ↓ ↓ ↓ ↓ ↓ ↓ ↓ ↓ ↓ ↓ ↓ ↓  (+11)
n o t t o o h a r d h e r e   (decoded)
```

> Password: **`nottoohardhere`**

To brute-force all possible shifts from the command line:

```bash
for i in $(seq 1 25); do
  echo -n "ROT$i: "
  echo "cdiiddwpgswtgt" | tr 'a-z' "$(echo {a..z} | tr -d ' ' | tail -c +$((i+1)))$(echo {a..z} | tr -d ' ' | head -c $i)"
done
```

Or use [rot13.com](https://rot13.com) and adjust the shift slider interactively.

### 4. Retrieve the token

```bash
su flag00
# Password: nottoohardhere
getflag
```

```
Check flag.Here is your token : x24ti5gi3x0ol2eh4esiuxias
```

---

## What is ROT encryption?

**ROT (Rotation)** is a substitution cipher that shifts each letter of the alphabet by N positions.
There are 25 possible variants (ROT1 through ROT25).

| Algorithm | Shift | Example (`hello`) | Note |
|-----------|-------|-------------------|------|
| ROT3      | +3    | `khoor`           | Original Caesar cipher |
| ROT11     | +11   | `spwwz`           | Used in this level |
| ROT13     | +13   | `uryyb`           | Self-inverse: ROT13(ROT13(x)) = x |

**ROT13** is the most well-known variant because a shift of 13 is exactly half of the 26-letter alphabet, making it symmetric: applying it twice returns the original text.

> ⚠️ ROT provides no real security. With only 25 possible shifts, a brute-force attack is instantaneous. In CTF challenges, it is used as intentionally weak obfuscation.
