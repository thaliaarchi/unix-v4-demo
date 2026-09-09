#
/*
 * list file or directory
 */

#include "/usr/sys/dirent.h"
#include "/usr/sys/stat.h"

struct {
	int	fdes;
	int	nleft;
	char	*nextc;
	char	buff[512];
} inf;

struct lbuf {
	char	l_name[DIRSIZ];
	int	l_ino;
	int	l_mode;
	char	l_nlink;
	char	l_uid;
	char	l_gid;
	char	l_siz0;
	int	l_siz;
	char	*l_mtime[2];
};

struct lbufx {
	char	*namep;
};

int	aflg, dflg, lflg, sflg, tflg, uflg, iflg, fflg;
int	fout;
int	rflg	1;
int	flags;
int	uidfil	-1;
int	tblocks;
int	statreq;
struct	lbuf	*lastp	&end;
struct	lbuf	*rlastp	&end;
char	*dotp	".";

#define	IFMT	060000
#define	DIR	0100000
#define	CHR	020000
#define	BLK	040000
#define	ISARG	01000
#define	LARGE	010000
#define	SUID	04000
#define	SGID	02000
#define	ROWN	0400
#define	WOWN	0200
#define	XOWN	0100
#define	RGRP	040
#define	WGRP	020
#define	XGRP	010
#define	ROTH	04
#define	WOTH	02
#define	XOTH	01

main(argc, argv)
char **argv;
{
	int i, j;
	extern struct lbuf end;
	register struct lbuf *ep, *ep1;
	register struct lbuf *slastp;
	struct lbuf lb;
	int compar();

	fout = dup(1);
	if (--argc > 0 && *argv[1] == '-') {
		argv++;
		while (*++*argv) switch (**argv) {
		case 'a':
			aflg++;
			continue;

		case 's':
			sflg++;
			statreq++;
			continue;

		case 'd':
			dflg++;
			continue;

		case 'l':
			lflg++;
			statreq++;
			uidfil = open("/etc/passwd", 0);
			continue;

		case 'r':
			rflg = -1;
			continue;

		case 't':
			tflg++;
			statreq++;
			continue;

		case 'u':
			uflg++;
			continue;

		case 'i':
			iflg++;
			continue;

		case 'f':
			fflg++;
			continue;

		default:
			continue;
		}
		argc--;
	}
	if (fflg) {
		aflg++;
		lflg = 0;
		sflg = 0;
		tflg = 0;
		statreq = 0;
	}
	if (argc==0) {
		argc++;
		argv = &dotp - 1;
	}
	for (i=0; i < argc; i++) {
		if ((ep = gstat(*++argv, 1))==0)
			continue;
		ep->namep = *argv;
		ep->l_mode =| ISARG;
	}
	qsort(&end, lastp - &end, 28, compar);
	slastp = lastp;
	for (ep = &end; ep<slastp; ep++) {
		if (ep->l_mode&DIR && dflg==0 || fflg) {
			if (argc>1)
				printf("\n%s:\n", ep->namep);
			lastp = slastp;
			readdir(ep->namep);
			if (fflg==0)
				qsort(slastp, lastp - slastp, 28, compar);
			if (statreq)
				printf("total %d\n", tblocks);
			for (ep1=slastp; ep1<lastp; ep1++)
				pentry(ep1);
		} else 
			pentry(ep);
	}
	flush();
}

pentry(ap)
struct lbuf *ap;
{
	char tbuf[16];
	struct { char dminor, dmajor;};
	register struct lbuf *p;

	p = ap;
	if (p->l_ino == -1)
		return;
	if (iflg)
		printf("%5d ", p->l_ino);
	if (lflg) {
		pmode(p->l_mode);
		printf("%2d ", p->l_nlink);
		if (getname(p->l_uid&0377, tbuf)==0)
			printf("%-6.6s", tbuf);
		else
			printf("%-6d", p->l_uid&0377);
		if (p->l_mode & (BLK|CHR))
			printf("%3d,%3d", p->l_siz.dmajor&0377,
			    p->l_siz.dminor&0377);
		else
			printf("%7s", locv(p->l_siz0, p->l_siz));
		printf(" %-12.12s ", ctime(p->l_mtime)+4);
	} else if (sflg)
		printf("%4d ", nblock(p->l_siz0, p->l_siz));
	if (p->l_mode&ISARG)
		printf("%s\n", p->namep);
	else
		printf("%.14s\n", p->l_name);
}

getname(uid, buf)
int uid;
char buf[];
{
	int j, c, n, i;

	inf.fdes = uidfil;
	seek(inf.fdes, 0, 0);
	inf.nleft = 0;
	do {
		i = 0;
		j = 0;
		n = 0;
		while((c=getc(&inf)) != '\n') {
			if (c<0)
				return(-1);
			if (c==':') {
				j++;
				c = '0';
			}
			if (j==0)
				buf[i++] = c;
			if (j==2)
				n = n*10 + c - '0';
		}
	} while (n != uid);
	buf[i++] = '\0';
	return(0);
}

nblock(size0, size)
char *size0, *size;
{
	register int n;

	n = ldiv(size0, size, 512);
	if (size&0777)
		n++;
	if (n>8)
		n =+ (n+255)/256;
	return(n);
}

int	m0[] { 3, DIR, 'd', BLK, 'b', CHR, 'c', '-'};
int	m1[] { 1, ROWN, 'r', '-' };
int	m2[] { 1, WOWN, 'w', '-' };
int	m3[] { 2, SUID, 's', XOWN, 'x', '-' };
int	m4[] { 1, RGRP, 'r', '-' };
int	m5[] { 1, WGRP, 'w', '-' };
int	m6[] { 2, SGID, 's', XGRP, 'x', '-' };
int	m7[] { 1, ROTH, 'r', '-' };
int	m8[] { 1, WOTH, 'w', '-' };
int	m9[] { 1, XOTH, 'x', '-' };

int	*m[] { m0, m1, m2, m3, m4, m5, m6, m7, m8, m9};

pmode(aflag)
{
	register int **mp;

	flags = aflag;
	for (mp = &m[0]; mp < &m[10];)
		select(*mp++);
}

select(pairp)
int *pairp;
{
	register int n, *ap;

	ap = pairp;
	n = *ap++;
	while (--n>=0 && (flags&*ap++)==0)
		*ap++;
	putchar(*ap);
}

makename(dir, file)
char *dir, *file;
{
	static char dfile[100];
	register char *dp, *fp;
	register int i;

	dp = dfile;
	fp = dir;
	while (*fp)
		*dp++ = *fp++;
	*dp++ = '/';
	fp = file;
	for (i=0; i<DIRSIZ; i++)
		*dp++ = *fp++;
	*dp = 0;
	return(dfile);
}

readdir(dir)
char *dir;
{
	static struct dirent dentry;
	register char *p;
	register int j;
	register struct lbuf *ep;

	if (fopen(dir, &inf) < 0) {
		printf("%s unreadable\n", dir);
		return;
	}
	tblocks = 0;
	for(;;) {
		p = &dentry;
		for (j=0; j<16; j++)
			*p++ = getc(&inf);
		if (dentry.d_ino==0
		 || aflg==0 && dentry.d_name[0]=='.')
			continue;
		if (dentry.d_ino == -1)
			break;
		ep = gstat(makename(dir, dentry.d_name), 0);
		if (ep->l_ino != -1)
			ep->l_ino = dentry.d_ino;
		for (j=0; j<DIRSIZ; j++)
			ep->l_name[j] = dentry.d_name[j];
	}
	close(inf.fdes);
}

gstat(file, argfl)
char *file;
{
	struct stat statb;
	register struct lbuf *rep;

	if (lastp+1 >= rlastp) {
		sbrk(512);
		rlastp.st_dev =+ 512;
	}
	rep = lastp;
	lastp++;
	rep->l_mode = 0;
	rep->l_ino = 0;
	if (argfl || statreq) {
		if (stat(file, &statb)<0) {
			printf("%s not found\n", file);
			statb.st_ino = -1;
			statb.st_siz0 = 0;
			statb.st_siz = 0;
			statb.st_mode = 0;
			if (argfl) {
				lastp--;
				return(0);
			}
		}
		rep->l_ino = statb.st_ino;
		statb.st_mode =& ~(ISARG |  DIR);
		if ((statb.st_mode&IFMT) == 060000) {
			statb.st_mode =& ~020000;
		} else if ((statb.st_mode&IFMT)==040000) {
			statb.st_mode =& ~IFMT;
			statb.st_mode =| DIR;
		}
		rep->l_mode = statb.st_mode;
		rep->l_uid = statb.st_uid;
		rep->l_gid = statb.st_gid;
		rep->l_nlink = statb.st_nlink;
		rep->l_siz0 = statb.st_siz0;
		rep->l_siz = statb.st_siz;
		if (rep->l_mode & (BLK|CHR) && lflg)
			rep->l_siz = statb.st_addr[0];
		rep->l_mtime[0] = statb.st_mtime[0];
		rep->l_mtime[1] = statb.st_mtime[1];
		if(uflg) {
			rep->l_mtime[0] = statb.st_atime[0];
			rep->l_mtime[1] = statb.st_atime[1];
		}
		tblocks =+ nblock(statb.st_siz0, statb.st_siz);
	}
	return(rep);
}

compar(ap1, ap2)
struct lbuf *ap1, *ap2;
{
	register struct lbuf *p1, *p2;
	register int i;
	int j;
	struct { char *charp;};

	p1 = ap1;
	p2 = ap2;
	if (dflg==0) {
		if ((p1->l_mode&(DIR|ISARG)) == (DIR|ISARG)) {
			if ((p2->l_mode&(DIR|ISARG)) != (DIR|ISARG))
				return(1);
		} else {
			if ((p2->l_mode&(DIR|ISARG)) == (DIR|ISARG))
				return(-1);
		}
	}
	if (tflg) {
		i = 0;
		if (p2->l_mtime[0] > p1->l_mtime[0])
			i++;
		else if (p2->l_mtime[0] < p1->l_mtime[0])
			i--;
		else if (p2->l_mtime[1] > p1->l_mtime[1])
			i++;
		else if (p2->l_mtime[1] < p1->l_mtime[1])
			i--;
		return(i*rflg);
	}
	if (p1->l_mode&ISARG)
		p1 = p1->namep;
	else
		p1 = p1->l_name;
	if (p2->l_mode&ISARG)
		p2 = p2->namep;
	else
		p2 = p2->l_name;
	for (i=0; i<DIRSIZ; i++)
		if ((j = *p1.charp++ - *p2.charp++) || p1.charp[-1]==0)
			return(rflg*j);
	return(0);
}
