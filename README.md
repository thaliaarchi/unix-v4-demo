# UNIX V4 workshop at Low Resource Computing

At the [Low Resource Computing](https://lrc.cs.dartmouth.edu) 2026 workshop at
Dartmouth, I hosted an interactive session on UNIX V4. This version of UNIX was
recently recovered from a 1974 magnetic tape found at the University of Utah and
is the earliest complete machine-readable snapshot of UNIX. I taught how to do
software development using the tools of the day, particularly the ed text editor
with a teletype, and participants solved a coding challenge, all signed onto the
same machine.

This repository reproduces the [disk image](disk.rk) and [terminal logs](logs/) 
from the workshop, as well as my [setup](setup.sh).

## Terminals

Before the workshop, I modified the kernel to support up to 32 simultaneous
terminal connections, raised from the previous 20. It uses 16 KL terminals and
16 DC terminals. I would have also configured 16 DH terminals, bringing the
total up to 48 terminals, the theoretical limit for this version of UNIX, but
the PiDP-11 fork of the SIMH emulator does not support this multiplexer.

I had two physical terminals connected (a Silent 700 teleprinter and a Heathkit
CRT terminal), 29 telnet connections from participants' laptops, as well the
main console on the PiDP-11.

Find the terminals you used in this list:

<details>
<summary>The logs for each terminal line</summary>

- [KL line 0](logs/kl0.log): main console, `/dev/tty8`
- [DC line 0](logs/dc0.log): TI Silent 700 Model 707/1200, `/dev/tty0`
- [DC line 1](logs/dc1.log): Heathkit H19, `/dev/tty1`
- [DC line 2](logs/dc2.log): port 4002, `/dev/tty2`
- [DC line 3](logs/dc3.log): port 4003, `/dev/tty3`
- [DC line 4](logs/dc4.log): port 4004, `/dev/tty4`
- [DC line 5](logs/dc5.log): port 4005, `/dev/tty5`
- [DC line 6](logs/dc6.log): port 4006, `/dev/tty6`
- [DC line 7](logs/dc7.log): port 4007, `/dev/tty7`
- [DC line 8](logs/dc8.log): port 4008, `/dev/ttya`
- [DC line 9](logs/dc9.log): port 4009, `/dev/ttyb`
- [DC line 10](logs/dc10.log): port 4010, `/dev/ttyc`
- [DC line 11](logs/dc11.log): port 4011, `/dev/ttyd`
- [DC line 12](logs/dc12.log): port 4012, `/dev/ttye`
- [DC line 13](logs/dc13.log): port 4013, `/dev/ttyf`
- [DC line 14](logs/dc14.log): port 4014, `/dev/ttyg`
- [DC line 15](logs/dc15.log): port 4015, `/dev/ttyh`
- [KL line 1](logs/kl1.log): port 4016, `/dev/tty9`
- [KL line 2](logs/kl2.log): port 4017, `/dev/ttyi`
- [KL line 3](logs/kl3.log): port 4018, `/dev/ttyj`
- [KL line 4](logs/kl4.log): port 4019, `/dev/ttyk`
- [KL line 5](logs/kl5.log): port 4020, `/dev/ttyl`
- [KL line 6](logs/kl6.log): port 4021, `/dev/ttym`
- [KL line 7](logs/kl7.log): port 4022, `/dev/ttyn`
- [KL line 8](logs/kl8.log): port 4023, `/dev/ttyo`
- [KL line 9](logs/kl9.log): port 4024, `/dev/ttyp`
- [KL line 10](logs/kl10.log): port 4025, `/dev/ttyq`
- [KL line 11](logs/kl11.log): port 4026, `/dev/ttyr`
- [KL line 12](logs/kl12.log): port 4027, `/dev/ttys`
- [KL line 13](logs/kl13.log): port 4028, `/dev/ttyt`
- [KL line 14](logs/kl14.log): port 4029, `/dev/ttyu`
- [KL line 15](logs/kl15.log): port 4030, `/dev/ttyv`

</details>

I describe my configuration for reproducibility, though the [notes](setup.md)
are mostly intended for myself. I will rework it into a tutorial for configuring
V4–V7 later.

## Workshop

First, we created user accounts for everyone. UNIX did not gain utilities for
adding users until much later, so we edited `/etc/passwd` ourselves.

```
% cat /etc/passwd
root::0:1::/:
bin::3:1::/bin:
thalia::4:4::/usr/thalia:
ben::5:5::/usr/ben:
alex::9:9::/usr/alex:
newt::64:64::/usr/newt:
auberon::18:18::/usr/auberon:
ncb::12:12::/usr/ncb:
joe::88:88::/usr/joe:
cody::25:25::/usr/cody:
hash::31337:31337::/usr/hash:
dnm::55555:55555::/usr/dnm:
ty::1990:1990::/usr/ty:
justin::70:70::/usr/justin:
yang::16::16::/usr/yang:
doug::8:::/usr/doug:
amitb::6:6::/usr/amitb:
voytilla::7:7::/usr/voytilla:
kevin::10:10::/usr/kevin:
michael::30:30::/usr/michael:
bx::42:42::/usr/bx:
hacker::256:256::/usr/hacker:
jordan::69:69::/usr/jordan:
dominic::111:111::/usr/dominic:
```

Erase and kill.

doug

```
ben::5:5::/usr/ben:
ben::65535:65535::/usr/ben:
ben::-1:-1::/usr/ben:
hacker::256:256::/usr/hacker:
```

```
% ed /usr/source/s2/sh.c
10267
/acname =/;/}/p
        acname = "/usr/adm/sh_acct";
        promp = "% ";
        if(((uid = getuid())&0377) == 0) {
                promp = "# ";
                acname = "/usr/adm/su_acct";
        }
```

```
% cd dir
haha
% ed /usr/source/s2/sh.c
10267
/cd/;/}/p
                if(equal(cp1, "cd")) {
                        prs("haha\n");
                        return;
                }
```

Used up a whole roll of paper.

Mailed to each other. `write`. Wrote to `/dev/tty?`

Played games.

Learned ed.

Thompson shell scripting (`/usr/alex/hint3b`).

Wrote some C.

Played with beep.
