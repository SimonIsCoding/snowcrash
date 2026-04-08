# Level 14 - SnowCrash

## Reconnaissance

The home directory contains no binary or exploitable file:

```bash
level14@SnowCrash:~$ ls -la
total 12
dr-x------ 1 level14 level14  100 Mar  5  2016 .
d--x--x--x 1 root    users    340 Aug 30  2015 ..
-r-x------ 1 level14 level14  220 Apr  3  2012 .bash_logout
-r-x------ 1 level14 level14 3518 Aug 30  2015 .bashrc
-r-x------ 1 level14 level14  675 Apr  3  2012 .profile
```

The only attack surface is `/bin/getflag` itself.

## Binary Analysis

Disassembling `main` reveals two key protections:

**1. Anti-debug via `ptrace`**

`getflag` calls `ptrace(PTRACE_TRACEME, 0, 0, 0)` on itself at startup. If the call fails (return value < 0), it means a debugger is already attached and the program exits with `You should not reverse this`. This is a standard self-`ptrace` anti-debugging technique.

**2. UID-based flag dispatch**

After the `ptrace` check, `getflag` calls `getuid()` and uses the result in a switch-like comparison to determine which flag to print. The UID of `flag14`, obtained from `/etc/passwd`, is `3014`.

## Exploitation

The goal is to run `getflag` under GDB, bypass the `ptrace` anti-debug check, then spoof the `getuid()` return value.

**Why `ptrace` check fails under GDB:** GDB itself uses `ptrace` to control the traced process. When `getflag` tries to call `ptrace(PTRACE_TRACEME)`, it fails because GDB already holds the trace slot — this is exactly what the check is designed to detect.

**Fix:** Let `ptrace` execute and return, then immediately overwrite `eax` with `0` before the comparison. A return value of `0` signals success, tricking the program into believing no debugger is attached.

**Fix `getuid`:** Place a breakpoint at `0x08048b02` — the instruction immediately after `call getuid` — where `eax` holds the raw return value before it is used. Overwrite it with `3014`.

```bash
gdb /bin/getflag
(gdb) break ptrace
(gdb) break *0x08048b02
(gdb) run

# Breakpoint 1: ptrace returned
(gdb) finish
(gdb) set $eax = 0
(gdb) continue

# Breakpoint 2: getuid just returned
(gdb) set $eax = 3014
(gdb) continue
# Check flag.Here is your token : 7QiHafiNa3HVozsaXkawuYrTstxbpABHD8CPnHJ
```

## Vulnerability Class

- **CWE-388** – Insufficient anti-debugging protection (bypassable via runtime register manipulation)
- **CWE-269** – Improper privilege management (UID-based access control bypassable at runtime)

## Remediation

- Avoid relying solely on `ptrace` as an anti-debug mechanism; it is trivially bypassed with a debugger.
- Privilege checks should be enforced by the kernel, not by user-space UID comparisons that can be manipulated at runtime.
- Flag delivery logic should reside outside the binary entirely (e.g., a privileged daemon over a local socket).
