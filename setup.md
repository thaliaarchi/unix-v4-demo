# Setting up UNIX V4

## Create a new user

```
k
unix
mem = 64530

login: root
# ed /etc/passwd
30
1,$p
root::0:1::/:
bin::3:1::/bin:
a
thalia::4:4::/usr/thalia:
.
w
56
q
# mkdir /usr/thalia
# chown thalia /usr/thalia
# ls -al /usr
total 16
drwxr-xr-x16 bin       256 Jun 12 19:51 .
drwxr-xr-x 9 bin       160 Jun 12 19:50 ..
drwxr-xr-x 2 bin        32 Jun 10 09:37 adm
drwxr-xr-x 2 bin       480 Jun 12 17:51 bin
drwxr-xr-x 2 bin       448 Jun 10 09:37 c
drwxr-xr-x 2 bin       160 Jun 10 09:37 fort
drwxr-xr-x 2 bin       144 Jun 10 09:37 games
drwxr-xr-x 2 bin       144 Jun 10 09:37 lib
drwxrwxrwx 2 bin        32 Jun 12 17:51 lpd
drwxr-xr-x 2 bin       192 Jun 10 09:37 mdec
drwxr-xr-x 2 bin        96 Jun 10 09:37 pub
drwxr-xr-x 2 bin       112 Jun 10 09:37 sno
drwxr-xr-x 7 bin       112 Jun 10 09:37 source
drwxr-xr-x 5 bin       384 Jun 12 19:50 sys
drwxrwxrwx 2 thalia     32 Jun 12 19:51 thalia
drwxrwxrwx 2 bin        32 Jun 10 09:37 tmp
# 
login: thalia
% 
```

## Rebuilding the kernel

Follow Angelo's "Rebuilding the kernel (without sys.tp)" instructions, but make
sure to sign in as the `bin` user.

## Set time zone and date

As detailed in ["Setting up UNIX"](https://www.tuhs.org/Archive/Applications/Dennis_Tapes/Gao_Analysis/v4_dist/setup.pdf):

```
login: bin
% ed /usr/source/s3/ctime.c
3992
/int timezone/;.+7p
int timezone    5*60*60;
int tzname[]
{
        "EST",
        "EDT",
};
int     daylight 1;     /* Allow daylight conversion */
int     nixonflg 1;     /* Daylight time all year around */
/5\*/s//6*/p
int timezone    6*60*60;
/EST/s//MST/p
        "MST",
/EDT/s//MDT/p
        "MDT",
/nixonflg/s/1/0/p
int     nixonflg 0;     /* Daylight time all year around */
w
3992
q
% sh /usr/sys/conf/tmrc
Cannot creat new file.
40: Warning: assignment understood
% ^D
login: root
# sh /usr/sys/conf/tmrc
40: Warning: assignment understood
```

Set the date with [`date(I)`](http://squoze.net/UNIX/v4man/man1/date):

```
# date 0811033974
Sun Aug 11 03:39:00 MDT 1974
```

## Add device files

As detailed in ["Setting up UNIX"](https://www.tuhs.org/Archive/Applications/Dennis_Tapes/Gao_Analysis/v4_dist/setup.pdf):

```
% ls /dev
null
tty8
% 
login: root
# chdir /dev
# ls -al /dev
total 2
drwxr-xr-x 2 bin        64 Jun 10 08:37 .
drwxr-xr-x 9 bin       176 Aug 11 03:39 ..
-r--r--r-- 1 bin         0 Jun 10 08:37 null
crw--w--w- 1 root    0,  0 Jun 12 19:15 tty8
# /etc/mknod rp0 b 0 0
# /etc/mknod rp1 b 0 1
# /etc/mknod rk0 b 1 0
# /etc/mknod rk1 b 1 1
# /etc/mknod tap0 b 2 0
# /etc/mknod tap1 b 2 1
# /etc/mknod tap2 b 2 2
# /etc/mknod tap3 b 2 3
# /etc/mknod tap4 b 2 4
# /etc/mknod tap5 b 2 5
# /etc/mknod tap6 b 2 6
# /etc/mknod tap7 b 2 7
# /etc/mknod mt0 b 3 0
# /etc/mknod mt1 b 3 1
# /etc/mknod mt2 b 3 2
# /etc/mknod mt3 b 3 3
# /etc/mknod mt4 b 3 4
# /etc/mknod mt5 b 3 5
# /etc/mknod mt6 b 3 6
# /etc/mknod mt7 b 3 7
# ls -al
total 2
drwxr-xr-x 2 bin       384 Aug 11 03:46 .
drwxr-xr-x 9 bin       176 Aug 11 03:46 ..
brw-rw-rw- 1 root    3,  0 Aug 11 03:45 mt0
brw-rw-rw- 1 root    3,  1 Aug 11 03:45 mt1
brw-rw-rw- 1 root    3,  2 Aug 11 03:45 mt2
brw-rw-rw- 1 root    3,  3 Aug 11 03:45 mt3
brw-rw-rw- 1 root    3,  4 Aug 11 03:45 mt4
brw-rw-rw- 1 root    3,  5 Aug 11 03:46 mt5
brw-rw-rw- 1 root    3,  6 Aug 11 03:46 mt6
brw-rw-rw- 1 root    3,  7 Aug 11 03:46 mt7
-r--r--r-- 1 bin         0 Jun 10 08:37 null
brw-rw-rw- 1 root    1,  0 Aug 11 03:44 rk0
brw-rw-rw- 1 root    1,  1 Aug 11 03:44 rk1
brw-rw-rw- 1 root    0,  0 Aug 11 03:44 rp0
brw-rw-rw- 1 root    0,  1 Aug 11 03:44 rp1
brw-rw-rw- 1 root    2,  0 Aug 11 03:44 tap0
brw-rw-rw- 1 root    2,  1 Aug 11 03:44 tap1
brw-rw-rw- 1 root    2,  2 Aug 11 03:44 tap2
brw-rw-rw- 1 root    2,  3 Aug 11 03:45 tap3
brw-rw-rw- 1 root    2,  4 Aug 11 03:45 tap4
brw-rw-rw- 1 root    2,  5 Aug 11 03:45 tap5
brw-rw-rw- 1 root    2,  6 Aug 11 03:45 tap6
brw-rw-rw- 1 root    2,  7 Aug 11 03:45 tap7
crw--w--w- 1 root    0,  0 Aug 11 03:46 tty8
```
