#include "/usr/sys/tty.h"

struct {
	char	ispeed, ospeed;
	char	erase, kill;
	int	flags;
} ttyb, rawb;

main()
{
	char c;

	gtty(0, &ttyb);
	printf("gtty:\n");
	printf("ispeed = %d\n", ttyb.ispeed);
	printf("ospeed = %d\n", ttyb.ospeed);
	printf("erase  = %c\n", ttyb.erase);
	printf("kill   = %c\n", ttyb.kill);
	printf("flags  =");
	if(ttyb.flags&NODELAY) printf(" NODELAY");
	if(ttyb.flags&XTABS) printf(" XTABS");
	if(ttyb.flags&LCASE) printf(" LCASE");
	if(ttyb.flags&ECHO) printf(" ECHO");
	if(ttyb.flags&CRMOD) printf(" CRMOD");
	if(ttyb.flags&RAW) printf(" RAW");
	if(ttyb.flags&ODDP) printf(" ODDP");
	if(ttyb.flags&EVENP) printf(" EVENP");
	if(ttyb.flags&M2741) printf(" M2741");
	if(ttyb.flags&OCORR) printf(" OCORR");
	if(ttyb.flags&ICORR) printf(" ICORR");
	if(ttyb.flags&NTDELAY) printf(" NTDELAY");
	printf("\n");
	rawb.ispeed = ttyb.ispeed;
	rawb.ospeed = ttyb.ospeed;
	rawb.erase = ttyb.erase;
	rawb.kill = ttyb.kill;
	rawb.flags =& ~ECHO;
	rawb.flags =| RAW;
	stty(0, &rawb);
	for(;;) {
		c = getchar();
		printf("'%c' = %d\r\n", c, c);
		if(c == 0 || c == 004)
			break;
	}
	stty(0, &ttyb);
}
