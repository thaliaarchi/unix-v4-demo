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
