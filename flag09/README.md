# SnowCrash - Level 09

## Files available on the machine

- `level09` : a compiled binary (no source code provided)
- `token` : a file containing an encrypted password

## Step 1 - Understanding the encryption

Since no source code was provided, I started by observing the behavior of the binary with controlled inputs.

```bash
$ echo "bbb" > bbb && ./level09 bbb
bcd

$ echo "aaa" > bbb && ./level09 bbb
bcd
```

A clear pattern emerged: each character of the **filename passed as argument** is shifted by its position index.

- character at index 0 is unchanged (`+ 0`)
- character at index 1 is shifted by 1 (`+ 1`)
- character at index 2 is shifted by 2 (`+ 2`)
- and so on...

I confirmed this with the token file:

```bash
$ ./level09 token
tpmhr
```

`t+0=t`, `o+1=p`, `k+2=m`, `e+3=h`, `n+4=r` — consistent.

## Step 2 - Decompiling the binary with Ghidra

To confirm the encryption logic and understand the full binary, I decompiled `level09` using Ghidra (a reverse engineering tool).

![Ghidra decompiled source](ghidra_screenshot.png)

The key line in the decompiled output is:

```c
putchar((int)*(char *)(local_120 + *(int *)(param_2 + 4)) + local_120);
```

Breaking it down:
- `param_2 + 4` is `argv[1]`, the filename passed as argument
- `local_120` is the current character index (starting at 0)
- The operation is: **output = character + index**

This confirmed exactly what was observed experimentally.

## Step 3 - Writing the decryption script

The reverse operation is simply: **character - index**

I applied this to each byte of the `token` file (which contains the already-encrypted password) and wrote the following C script:

```c
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int main()
{
    char    password[1024] = "";
    int     fd = open("token", O_RDONLY);
    int     i = 0;
    char    decrypted[1024] = "";

    if (fd < 0)
        return (1);
    if (read(fd, password, 1024) < 0)
        return (2);
    while (password[i] != '\0' && password[i] != '\n')
    {
        decrypted[i] = password[i] - i;
        i++;
    }
    decrypted[i] = 0;
    printf("your token is : %s\n", decrypted);
    close(fd);
    return (0);
}
```

> [!NOTE]
> the `&& password[i] != '\n'` condition prevents the trailing newline byte from being processed, which would otherwise produce a garbage character at the end of the output.

## Result

```bash
$ cc decrypt.c && ./a.out
your token is : f3iji1ju5yuevaus41q1afiuq
```

This token is used to connect as `flag09` and retrieve the flag.
