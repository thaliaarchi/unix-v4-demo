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

## Setup a terminal multiplexer

To use multiple terminals at once with UNIX, we need to setup a terminal
multiplexer. By default, we have only the built-in KL11/DL11 console. SIMH
supports the following terminal devices:

| Device                | SIMH name   | Source               | Lines |
| --------------------- | ----------- | -------------------- | ----- |
| DC11                  | `DCI`/`DCO` | PDP11/pdp11_dc.c     | 1–16  |
| DH11                  | `DH`        | PDP11/pdp11_dh.c     | 16    |
| DZ11                  | `DZ`        | PDP11/pdp11_dz.c     | ×8    |
| KL11/DL11 console     | `TTI`/`TTO` | PDP11/pdp11_stddev.c | 1     |
| extra KL11/DL11 lines | `DLI`/`DLO` | PDP11/pdp11_dl.c     | 0–16  |

The PiDP-11 build I am using is compiled with support for all of these, except
for DH11:

```
PDP-11 simulator V4.0-0 Current  REALCONS build Dec  1 2024

sim> show devices
PDP-11 simulator configuration

CPU     11/70, FPP, RH70, autoconfiguration enabled, idle enabled
...
DLI     disabled
DLO     disabled
DCI     address=17774000-17774077, vector=300-374*, BR4, lines=8
DCO     8 units
DZ      address=17760100-17760137*, vector=400-434*, BR5, lines=32
...
```

Consulting conf.c, we see that the kernel is configured with `tm` (TM tape),
`kl` (KL-11), `rk` (RK disk), `dh` (DH-11), and `pc` (PC-11 paper tape
reader/punch):

```
% cat /usr/sys/conf/conf.c
/*
 *      Copyright 1974 Bell Telephone Laboratories Inc
 */

int     (*bdevsw[])()
{
        &nulldev,       &nulldev,       &rkstrategy,    &rktab,
        &nulldev,       &tcclose,       &tcstrategy,    &tctab,
        &tmopen,        &tmclose,       &tmstrategy,    &tmtab,
        0
};

int     (*cdevsw[])()
{
        &klopen,   &klclose,   &klread,   &klwrite,   &klsgtty,
        &nulldev,  &nulldev,   &rkread,   &rkwrite,   &nodev,
        &tmopen,   &tmclose,   &tmread,   &tmwrite,   &nodev,
        &dhopen,   &dhclose,   &dhread,   &dhwrite,   &dhsgtty,
        &pcopen,   &pcclose,   &pcread,   &pcwrite,   &nodev,
        0
};

int     rootdev {(0<<8)|0};
int     swapdev {(0<<8)|0};
int     swplo   4000;
int     nswap   872;
```

We do not need `dh`, as this SIMH does not support it, but we want `dc` (DC11)
with 8 lines. Regenerate conf.c using mkconf, as explained by
[Berufsakademie Stuttgart](https://www.tuhs.org/Archive/Distributions/Other/OS_Course/script/chapt1.1):

```
login: bin
% chdir /usr/sys/conf
% mkconf
rk
tc
tm
kl
8dc
pc
^D
% diff conf.c c.c
18c18
*       &dhopen,   &dhclose,   &dhread,   &dhwrite,   &dhsgtty,
---
.       &dcopen,   &dcclose,   &dcread,   &dcwrite,   &dcsgtty,
# diff low.s l.s
56a57,70
.       dcin; br5+1.
.       dcou; br5+1.
.       dcin; br5+2.
.       dcou; br5+2.
.       dcin; br5+3.
.       dcou; br5+3.
.       dcin; br5+4.
.       dcou; br5+4.
.       dcin; br5+5.
.       dcou; br5+5.
.       dcin; br5+6.
.       dcou; br5+6.
.       dcin; br5+7.
.       dcou; br5+7.
```

Rebuild the kernel:

```
% mv c.c conf.c
% mv l.s low.s
% chdir ..
% sh run
alloc.c:
clock.c:
fio.c:
iget.c:
main.c:
nami.c:
prf.c:
rdwri.c:
sig.c:
60: Warning: assignment understood
61: Warning: assignment understood
slp.c:
subr.c:
sys1.c:
sys2.c:
sys3.c:
sys4.c:
sysent.c:
text.c:
trap.c:
bio.c:
cat.c:
dc.c:
dh.c:
dhdm.c:
dhfdm.c:
dn.c:
dp.c:
dv.c:
kl.c:
lp.c:
malloc.c:
mem.c:
partab.c:
pc.c:
pipe.c:
rf.c:
rk.c:
rp.c:
tc.c:
tm.c:
tty.c:
vs.c:
vt.c:
% mv a.out /nunix
% 
```

## Setup terminal lines

Add the device files and enable the first three:

```
login: root
# chdir /dev
# /etc/mknod tty0 c 3 0
# /etc/mknod tty1 c 3 1
# /etc/mknod tty2 c 3 2
# /etc/mknod tty3 c 3 3
# /etc/mknod tty4 c 3 4
# /etc/mknod tty5 c 3 5
# /etc/mknod tty6 c 3 6
# /etc/mknod tty7 c 3 7
# ed /etc/ttys
56
1,$p
000
010
020
030
040
050
060
070
180
090
0a0
0b0
0c0
0d0
1,8s/^0/1/
.
w
56
q
# 
```

Enable DCI in the SIMH config:

```
set dci enabled
set dci lines=8
set dco0 dataset
attach dci line=0,4000
attach dci line=1,4001
attach dci line=2,4002
attach dci line=3,4003
attach dci line=4,4004
attach dci line=5,4005
attach dci line=6,4006
attach dci line=7,4007
```

## Enabling all 16 lines

DC11 supports up to 16 lines. Enable the rest in boot.ini:

```
set dci lines=16
...
attach dci line=8,4008
attach dci line=9,4009
attach dci line=10,4010
attach dci line=11,4011
attach dci line=12,4012
attach dci line=13,4013
attach dci line=14,4014
attach dci line=15,4015
```

Since the KL console occupies /dev/tty8, further lines are named by letter:

```
login: root
# chdir /dev
# /etc/mknod ttya c 3 8
# /etc/mknod ttyb c 3 9
# /etc/mknod ttyc c 3 10
# /etc/mknod ttyd c 3 11
# /etc/mknod ttye c 3 12
# /etc/mknod ttyf c 3 13
# /etc/mknod ttyg c 3 14
# /etc/mknod ttyh c 3 15
# ed /etc/ttys
56
1,$p
100
110
120
130
140
150
160
170
180
090
0a0
0b0
0c0
0d0
/a/;$s/^0/1/
a
1e0
1f0
1g0
1h0
.
w
72
q
```

But when we reboot, the kernel panics:

```
# sync
^E
Simulation stopped, PC: 002140 (MOV (SP)+,177776)
sim> b rk
k
unix
mem = 64529
ka6 = 2272
aps = 141636

```

Restore your disk from a backup.

The `dc(IV)` manual reveals the problem. Not all lines are enabled:

```
The special files /dev/tty0, /dev/tty1, ...   refer  to  the
DC11  asynchronous communications interfaces.  At the moment
there are 12 of them, but the number is subject to change.
```

Let's fix the `dc` driver. Evidently, by June 1974, the number of DC11 lines
enabled had grown from 12 to 14. Change this to 16.

```
login: bin
% chdir /usr/sys/dmr
% ed dc.c
3526
/NDC11/
#define NDC11	14
s/14/16/
w
3526
q
% 
```

Rebuild the kernel with 16 lines:

```
login: bin
% chdir /usr/sys/conf
% mkconf
rk
tc
tm
kl
16dc
pc
^D
% diff conf.c c.c
% diff low.s .#l.s
70a71,86
.       dcin; br5+8.
.       dcou; br5+8.
.       dcin; br5+9.
.       dcou; br5+9.
.       dcin; br5+10.
.       dcou; br5+10.
.       dcin; br5+11.
.       dcou; br5+11.
.       dcin; br5+12.
.       dcou; br5+12.
.       dcin; br5+13.
.       dcou; br5+13.
.       dcin; br5+14.
.       dcou; br5+14.
.       dcin; br5+15.
.       dcou; br5+15.
% mv c.c conf.c
% mv l.s low.s
% chdir ..
% sh run
...
% mv a.out /unix
% 
```

Create and enable `tty{a..h}` as above.

And reboot:

```
% ^E
Simulation stopped, PC: 002140 (MOV (SP)+,177776)
sim> b rk
k
unix
mem = 64529

login: 
```

Connecting to line 13 works, but line 14 hangs before login:

```
$ telnet 192.168.0.16 4013
Trying 192.168.0.16...
Connected to 192.168.0.16.
Escape character is '^]'.


Connected to the PDP-11 simulator DCI device, line 13


login: ^]
telnet> q
Connection closed.
$ telnet 192.168.0.16 4014
Trying 192.168.0.16...
Connected to 192.168.0.16.
Escape character is '^]'.


Connected to the PDP-11 simulator DCI device, line 14


```

## Configure Silent 700

`stty -tabs`
