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

Let's examine the DCI connections to determine what's wrong. Lines 0 and 1 are
connected to physical terminals, lines 4 and 14 are connected over telnet, and
all other lines have no connection.

```
^E
Simulation stopped, PC: 002240 (MOV (SP)+,177776)
sim> show dci connections
line 0: Connecting to remote port /dev/ttyUSB0;1200-7E1
Connected to serial port /dev/ttyUSB0;1200-7E1
 Connected 00:28:25
line 1: Connecting to remote port /dev/ttyUSB1;9600-8N1
Connected to serial port /dev/ttyUSB1;9600-8N1
 Connected 00:28:25
line 4: Connection from IP address 192.168.0.11
Connection [192.168.0.16]:4004->[192.168.0.11]:57828
Listening on port 4004
 Connected 00:28:14
 Telnet protocol
line 14: Connection from IP address 192.168.0.11
Connection [192.168.0.16]:4014->[192.168.0.11]:57978
Listening on port 4014
 Connected 00:00:12
 Telnet protocol
```

Testing the CSR registers:

```
sim> examine dci csr[0]
CSR[0]: 000125
sim> examine dco csr[0]
CSR[0]: 000723
sim> examine dci csr[1]
CSR[1]: 000125
sim> examine dco csr[1]
CSR[1]: 000721
sim> examine dci csr[4]
CSR[4]: 000125
sim> examine dco csr[4]
CSR[4]: 000723
sim> examine dci csr[5]
CSR[5]: 000111
sim> examine dco csr[5]
CSR[5]: 000711
sim> examine dci csr[14]
CSR[14]:        130200
sim> examine dco csr[14]
CSR[14]:        000200
sim> 
```

SIMH pdp11_dc.c and pdp11_defs.h and V4 dc.c reveal the meanings of some of
these bits:

| SIMH name    | V4 name   | Direction | Bit     | Meaning                   |
| ------------ | --------- | --------- | ------- | ------------------------- |
| `CSR_GO`     |           | DCI/DCO   | 0000001 | go                        |
|              | `SPEED1`  |           | 0000010 | speed select bit 0        |
|              |           |           | 0000020 | speed select bit 1        |
|              | `PARITY`  |           | 0000040 |                           |
| `CSR_IE`     |           | DCI/DCO   | 0000100 | interrupt enable          |
| `CSR_DONE`   |           | DCI/DCO   | 0000200 | done                      |
|              | `STOP1`   |           | 0000400 |                           |
| `CSR_BUSY`   |           | DCI/DCO   | 0004000 | busy                      |
| `CSR_ERR`    | `ERROR`   | DCI/DCO   | 0100000 | error                     |
| `DCICSR_DTR` | `CDLEAD`  | DCI       | 0000001 | data terminal ready (DTR) |
| `DCICSR_XBR` |           | DCI       | 0000002 | transmit break            |
| `DCICSR_CDT` | `CARRIER` | DCI       | 0000004 | carier detect (CD)        |
| `DCICSR_PAR` |           | DCI       | 0000040 | odd parity                |
| `DCICSR_OVR` |           | DCI       | 0010000 | overrun                   |
| `DCICSR_RNG` | `RINGIND` | DCI       | 0020000 | ring indicator (RI)       |
| `DCICSR_CCH` | `CTRANS`  | DCI       | 0040000 | carrier change            |
| `DCICSR_ERR` |           | DCI       | 0100000 | error                     |
| `DCOCSR_RTS` | `RQSEND`  | DCO       | 0000001 | request to send (RTS)     |
| `DCOCSR_CTS` |           | DCO       | 0000002 | clear to send (CTS)       |
| `DCOCSR_MNT` |           | DCO       | 0000004 | maintenance               |

We can see that line 14 does not have DTR/RTS:

```
DCI CSR[0]:  0000125 = DCICSR_DTR | DCICSR_CDT | SPEED2 | CSR_IE
DCO CSR[0]:  0000723 = DCOCSR_RTS | DCOCSR_CTS | SPEED2 | CSR_IE | CSR_DONE | STOP1
DCI CSR[1]:  0000125 = DCICSR_DTR | DCICSR_CDT | SPEED2 | CSR_IE
DCO CSR[1]:  0000721 = DCOCSR_RTS | SPEED2 | CSR_IE | CSR_DONE | STOP1
DCI CSR[4]:  0000125 = DCICSR_DTR | DCICSR_CDT | SPEED2 | CSR_IE
DCO CSR[4]:  0000723 = DCOCSR_RTS | DCOCSR_CTS | SPEED2 | CSR_IE | CSR_DONE | STOP1
DCI CSR[5]:  0000111 = DCICSR_DTR | SPEED1 | CSR_IE
DCO CSR[5]:  0000711 = DCOCSR_RTS | SPEED1 | CSR_IE | CSR_DONE | STOP1
DCI CSR[14]: 0130200 = CSR_DONE | DCICSR_OVR | DCICSR_RNG | DCICSR_ERR
DCO CSR[14]: 0000200 = CSR_DONE
```

Turns out `NDC11` was reverted, probably from missing `sync` before a reboot or
forgetting to reapply it after a disk restore.

```
login: bin
% chdir /usr/sys/dmr
% ed dc.c
3526
/NDC11/
#define NDC11   14
//
struct  tty dc11[NDC11];
//
        if (dev.d_minor >= NDC11) {
a
                printf("minor %d > NDC11 %d\n", dev.d_minor, NDC11);
.
w
3581
q
```

After rebuilding the kernel and rebooting, this error is repeatedly printed, so
this was the problem. Fix `NDC11` and remove the print:

```
login: bin
% chdir /usr/sys/dmr
% ed dc.c
3526
/NDC11/s/14/16/
/printf/d
w
3526
q
% chdir ..
% sh run
...
% mv a.out /nunix
% ^D
Simulation stopped, PC: 002240 (MOV (SP)+,177776)
sim> b rk
k
nunix
mem = 64529

login: root
# mv nunix unix
# sync
# 
```

All 16 lines now work!

## Enable extra KL lines

The KL11/DL11 is what handles the main console, but it also can have extra
terminal lines.

In SIMH, the main console is the `TTI`/`TTO` device and up to 16 extra lines are
supported by the `DLI`/`DLO` device. Enable the lines in the SIMH config:

```
# Extra KL11 terminal lines
set dli enabled
set dli lines=16
attach dli line=0,4016
attach dli line=1,4017
attach dli line=2,4018
attach dli line=3,4019
attach dli line=4,4020
attach dli line=5,4021
attach dli line=6,4022
attach dli line=7,4023
attach dli line=8,4024
attach dli line=9,4025
attach dli line=10,4026
attach dli line=11,4027
attach dli line=12,4028
attach dli line=13,4029
attach dli line=14,4030
attach dli line=15,4031
```

However, this version of UNIX is configured for 4 KL lines (main console and 3
extra). Raise `NKL11` to space for 17 KL lines and reconfigure the system with
`16kl` for 16 extra lines, then rebuild the kernel.

```
login: bin
% chdir /usr/sys/dmr
% ed kl.c
1609
/NKL11/p
#define NKL11   4
s/4/17/
w
1610
q
% chdir ../conf
% mkconf
rk
tc
tm
16kl
16dc
pc
^D
% diff conf.c c.c
% diff low.s l.s
88a89,118
.       klin; br4+1+1.
.       klou; br4+1+1.
.       klin; br4+1+2.
.       klou; br4+1+2.
.       klin; br4+1+3.
.       klou; br4+1+3.
.       klin; br4+1+4.
.       klou; br4+1+4.
.       klin; br4+1+5.
.       klou; br4+1+5.
.       klin; br4+1+6.
.       klou; br4+1+6.
.       klin; br4+1+7.
.       klou; br4+1+7.
.       klin; br4+1+8.
.       klou; br4+1+8.
.       klin; br4+1+9.
.       klou; br4+1+9.
.       klin; br4+1+10.
.       klou; br4+1+10.
.       klin; br4+1+11.
.       klou; br4+1+11.
.       klin; br4+1+12.
.       klou; br4+1+12.
.       klin; br4+1+13.
.       klou; br4+1+13.
.       klin; br4+1+14.
.       klou; br4+1+14.
.       klin; br4+1+15.
.       klou; br4+1+15.
% mv c.c conf.c
% mv l.s low.s
% chdir ..
% sh run
...
% mv a.out /nunix
% sync
% ^E
Simulation stopped, PC: 014110 (BGE 14176)
sim> b rk
k
nunix
mem = 64526

login: 
```

Conventionally the main console is `/dev/tty8` and the first extra terminal is
`/dev/tty9`. I do not know what the further extra lines would traditionally be
called, so I assign numbers after the DL assignments.

```
login: root
# chdir /dev
# /etc/mknod tty9 c 0 1
# /etc/mknod ttyi c 0 2
# /etc/mknod ttyj c 0 3
# /etc/mknod ttyk c 0 4
# /etc/mknod ttyl c 0 5
# /etc/mknod ttym c 0 6
# /etc/mknod ttyn c 0 7
# /etc/mknod ttyo c 0 8
# /etc/mknod ttyp c 0 9
# /etc/mknod ttyq c 0 10
# /etc/mknod ttyr c 0 11
# /etc/mknod ttys c 0 12
# /etc/mknod ttyt c 0 13
# /etc/mknod ttyu c 0 14
# /etc/mknod ttyv c 0 15
# /etc/mknod ttyw c 0 16
# ls -al tty*
crw--w--w- 1 root    3,  0 Aug 11 14:52 tty0
crw--w--w- 1 root    3,  1 Aug 11 14:52 tty1
crw--w--w- 1 root    3,  2 Aug 11 14:52 tty2
crw--w--w- 1 root    3,  3 Aug 11 14:52 tty3
crw--w--w- 1 root    3,  4 Aug 11 14:52 tty4
crw--w--w- 1 root    3,  5 Aug 11 14:52 tty5
crw--w--w- 1 root    3,  6 Aug 11 14:52 tty6
crw--w--w- 1 root    3,  7 Aug 11 14:52 tty7
crw--w--w- 1 root    0,  0 Aug 11 16:30 tty8
crw-rw-rw- 1 root    0,  1 Aug 11 16:29 tty9
crw--w--w- 1 root    3,  8 Aug 11 14:52 ttya
crw--w--w- 1 root    3,  9 Aug 11 14:52 ttyb
crw--w--w- 1 root    3, 10 Aug 11 14:52 ttyc
crw--w--w- 1 root    3, 11 Aug 11 14:52 ttyd
crw--w--w- 1 root    3, 12 Aug 11 14:52 ttye
crw--w--w- 1 root    3, 13 Aug 11 14:52 ttyf
crw--w--w- 1 root    3, 14 Aug 11 14:52 ttyg
crw--w--w- 1 root    3, 15 Aug 11 14:52 ttyh
crw-rw-rw- 1 root    0,  2 Aug 11 16:29 ttyi
crw-rw-rw- 1 root    0,  3 Aug 11 16:29 ttyj
crw-rw-rw- 1 root    0,  4 Aug 11 16:29 ttyk
crw-rw-rw- 1 root    0,  5 Aug 11 16:29 ttyl
crw-rw-rw- 1 root    0,  6 Aug 11 16:30 ttym
crw-rw-rw- 1 root    0,  7 Aug 11 16:30 ttyn
crw-rw-rw- 1 root    0,  8 Aug 11 16:30 ttyo
crw-rw-rw- 1 root    0,  9 Aug 11 16:30 ttyp
crw-rw-rw- 1 root    0, 10 Aug 11 16:30 ttyq
crw-rw-rw- 1 root    0, 11 Aug 11 16:30 ttyr
crw-rw-rw- 1 root    0, 12 Aug 11 16:30 ttys
crw-rw-rw- 1 root    0, 13 Aug 11 16:30 ttyt
crw-rw-rw- 1 root    0, 14 Aug 11 16:30 ttyu
crw-rw-rw- 1 root    0, 15 Aug 11 16:30 ttyv
crw-rw-rw- 1 root    0, 16 Aug 11 16:30 ttyw
```

Enable the terminals:

```
# ed /etc/ttys
72
/9/
090
s/^0/1/
$a
1i0
1j0
1k0
1l0
1m0
1n0
1o0
1p0
1q0
1r0
1s0
1t0
1u0
1v0
1w0
.
w
132
q
# sync
# ^D
Simulation stopped, PC: 002430 (MOV (SP)+,177776)
sim> b rk
k
nunix
mem = 64526

login: root
# mv nunix unix
```

Connecting to ports 4016 to 4018 (tty9, ttyi, and ttyj) does not print the UNIX
login prompt, but is still interactive:

```
$ telnet localhost 4016
Trying ::1...
Connected to localhost.
Escape character is '^]'.


Connected to the PDP-11 simulator DLI device, line 0

ROOT
# 
```

Connecting to ports 4019 to 4031 (ttyk to ttyw) shows the PiDP-11 banner, but
does not print a UNIX login prompt:

```
$ telnet localhost 4019
Trying ::1...
Connected to localhost.
Escape character is '^]'.


Connected to the PDP-11 simulator DLI device, line 3


```

Looking at /etc/ttys, it is the first 20 terminals that work:

```
# cat /etc/ttys
100
110
120
130
140
150
160
170
180
190
1a0
1b0
1c0
1d0
1e0
1f0
1g0
1h0
1i0
1j0
1k0
1l0
1m0
1n0
1o0
1p0
1q0
1r0
1s0
1t0
1u0
1v0
1w0
```

This is due to a limitation of `init`. It only has slots for 20 ttys and any
more entries than that in `/etc/ttys` are silently dropped. We need 33 slots,
but let's double it to 40.

```
login: bin
% chdir /usr/source/s1
% ed init.c
3114
g/20/s//40/p
#define tabsize 40
#define all     p = &itab[0]; p < &itab[40]; p++
w
3114
q
% cc init.c
% mv a.out /etc/init
% sync
```

Kernel panic!

```
% ^E
Simulation stopped, PC: 002430 (MOV (SP)+,177776)
sim> b rk
k
unix
mem = 64526

ka6 = 5753
aps = 141630
login: 
```

Restore from a backup to before overwriting `/etc/init`.

Let's try gradually increasing `tabsize`. Sizes 21 to 32 work:

- 21: up to port 4019
- 22: up to port 4020
- 25: up to port 4023
- 30: up to port 4028
- 32: up to port 4030
- 33: kernel panic (ka6 = 5715, aps = 141630)
- 34: kernel panic (ka6 = 5715, aps = 141630)
- 35: kernel panic (ka6 = 5715, aps = 141630)
- 36: kernel panic (ka6 = 5715, aps = 141630)
- 37: kernel panic (ka6 = 5715, aps = 141630)
- 38: kernel panic (ka6 = 5715, aps = 141630)
- 39: kernel panic (ka6 = 5753, aps = 141630)
- 40: kernel panic (ka6 = 5753, aps = 141630)
- 50: kernel panic (ka6 = 6011, aps = 141630)

The 33rd terminal is ttyw, which has minor number 16. If we set `tabsize` to 32
and disable ttyv, ttyw becomes active and causes a kernel panic. The problem is
probably the minor number, not `tabsize`.

```
login: root
# ed /usr/source/s1/init.c
3114
1
#define tabsize 32
q
# ed /etc/ttys
132
^
1v0
s/1/0/
w
132
q
# sync
# ^E
Simulation stopped, PC: 002430 (MOV (SP)+,177776)
sim> b rk
k
unix
mem = 64526

ka6 = 5607
aps = 141630
login: 
```

If I set `tabsize` to 33 and reconfigure with 17kl instead of 16kl, then the
kernel panics with ka6 = 5716, aps = 141630.

By adding logging to `klopen`, `klrint`, and `klxint`, excluding device minor 0
to avoid cycles, we see that `klopen(16)` is called, but never `klxint(16)`,
while all others call `klxint`.

```
k
unix
mem = 64526

klopen 1
klxint 1
klxint 1
klxint 1
klopen 2
klxint 2
klxint 2
klxint 2
klopen 3
klxint 3
klxint 3
klxint 3
klopen 4
klxint 4
klxint 4
klxint 4
klopen 5
klxint 5
klxint 5
klopen 6
klxint 6
klopen 7
klxint 7
klopen 8
klxint 8
klopen 9
klxint 9
klopen 10
klxint 10
klopen 11
klxint 11
klopen 12
klxint 12
klxint 5
klxint 6
klxint 6
klxint 7
klxint 7
klxint 8
klxint 8
klxint 9
klxint 9
klxint 10
klxint 10
klxint 11
klxint 11
klxint 12
klxint 12
klopen 13
klxint 13
klxint 13
klxint 13
klopen 14
klxint 14
klxint 14
klxint 14
klopen 15
klxint 15
klxint 15
klxint 15
klopen 16
ka6 = 5717
aps = 141630
klxint 1
klxint 1
lklxint 1
oklxint 1
gklxint 1
iklxint 1
nklxint 1
:klxint 1
 klxint 1
klxint 1
klxint 2
klxint 2
klxint 2
klxint 2
klxint 2
klxint 2
klxint 2
klxint 2
klxint 2
klxint 2
klxint 3
klxint 3
klxint 3
klxint 3
klxint 3
klxint 3
klxint 3
klxint 3
klxint 3
klxint 3
klxint 4
klxint 4
klxint 4
klxint 4
klxint 4
klxint 4
klxint 4
klxint 4
klxint 4
klxint 4
klxint 5
klxint 5
klxint 5
klxint 5
klxint 5
klxint 5
klxint 5
klxint 5
klxint 5
klxint 5
klxint 6
klxint 6
klxint 6
klxint 6
klxint 6
klxint 6
klxint 6
klxint 6
klxint 6
klxint 6
klxint 7
klxint 7
klxint 7
klxint 7
klxint 7
klxint 7
klxint 7
klxint 7
klxint 7
klxint 7
klxint 8
klxint 8
klxint 8
klxint 8
klxint 8
klxint 8
klxint 8
klxint 8
klxint 8
klxint 8
klxint 9
klxint 9
klxint 9
klxint 9
klxint 9
klxint 9
klxint 9
klxint 9
klxint 9
klxint 9
klxint 10
klxint 10
klxint 10
klxint 10
klxint 10
klxint 10
klxint 10
klxint 10
klxint 10
klxint 10
klxint 11
klxint 11
klxint 11
klxint 11
klxint 11
klxint 11
klxint 11
klxint 11
klxint 11
klxint 11
klxint 12
klxint 12
klxint 12
klxint 12
klxint 12
klxint 12
klxint 12
klxint 12
klxint 12
klxint 13
klxint 13
klxint 13
klxint 13
klxint 13
klxint 13
klxint 13
klxint 13
klxint 14
klxint 14
klxint 14
klxint 14
klxint 14
klxint 14
klxint 14
klxint 15
klxint 15
klxint 15
klxint 15
klxint 15
klxint 15
ka6 = 5717
aps = 141564
```

Unfortunately, kl minor 16 is unusable on UNIX V4, due to how V4 implements
interrupt vectors.

On the PDP-11, each interrupt vector is two words: new `PC` and the new `PS`.
The `PC` word is the address of the handler to execute when the interrupt is
taken. The `PS` word is the processor status to load for the handler.

In V4, the vector table is generated by `mkconf` as `/usr/sys/conf/low.s`. V4
repurposes the low four bits of the `PS` word as an argument for the interrupt
handler.

However, the hardware still treats this word as a real [`PS`](https://gunkies.org/wiki/PDP-11_architecture#Processor_Status_Word),
with meanings for each bit. The low four bits are the condition codes, which are
benign to repurpose by the kernel, but the fifth bit is the trace bit. After an
instruction sets the trace bit, the hardware invokes the trace trap, causing a
kernel panic. Thus, an argument of 16 or higher overflows the four-bit argument
size. `mkconf` does not check that they are in range.

Although SIMH supports 17 KL11 lines (the main console and 16 extra terminals),
UNIX cannot safely use the 17th line (minor 16) without a significant
rearchitecture.

Also, since the first KL line is the main console, UNIX always enables it and it
is not specified with `mkconf`. Thus, the number of KL lines declared with
`mkconf` is one less than the actual.

Remove KL11 line 17 in SIMH:

```diff
-set dli lines=16
+set dli lines=15
 attach dli line=0,4016
 ...
 attach dli line=14,4030
-attach dli line=15,4031
```

and in UNIX:

```
login: bin
% rm /dev/ttyw
% ed /etc/ttys
132
.
1w0
d
w
128
q
% chdir /usr/source/s1
% ed init.c
3114
1,2s/32/40/
w
3114
q
% cc init.c
% mv a.out /etc/init
% sync
% ^E
Simulation stopped, PC: 002430 (MOV (SP)+,177776)
sim> b rk
k
unix
mem = 64526

login: 
```

## Configure Silent 700

`stty -tabs`

## Mirroring a terminal for a projector

How to connect to a line a second time for read-only projection?
