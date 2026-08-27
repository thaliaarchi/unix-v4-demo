#include "/usr/sys/param.h"

char	*arg;
int	mode[3];

main(argc, argv)
char	*argv[];
{
	gtty(1,mode);
	while(--argc > 0) {

		arg = *++argv;
		if(eq("ek"))
			mode[1] = '#@';
		else if(eq("erase")) {
			arg = *++argv;
			argc--;
			mode[1].lobyte = eq("bs") ? '\b' : *arg;
		} else if(eq("kill")) {
			mode[1].hibyte = **++argv;
			argc--;
		} else if(eq("even"))
			set(0200);
		else if(eq("-even"))
			reset(0200);
		else if(eq("odd"))
			set(0100);
		else if(eq("-odd"))
			reset(0100);
		else if(eq("raw"))
			set(040);
		else if(eq("-raw") || eq("cooked"))
			reset(040);
		else if(eq("-nl"))
			set(020);
		else if(eq("nl"))
			reset(020);
		else if(eq("echo"))
			set(010);
		else if(eq("-echo"))
			reset(010);
		else if(eq("lcase"))
			set(04);
		else if(eq("-lcase"))
			reset(04);
		else if(eq("-tabs"))
			set(02);
		else if(eq("tabs"))
			reset(02);
		else if(eq("-delay"))
			set(01);
		else if(eq("delay"))
			reset(01);
		else if(eq("tdelay"))
			reset(010000);
		else if(eq("-tdelay"))
			set(010000);
		else
			printf("unknown mode: %s\n", arg);
	}
	stty(1,mode);
}

eq(string)
char *string;
{
	int i;

	i = 0;
loop:
	if(arg[i] != string[i])
		return(0);
	if(arg[i++] != '\0')
		goto loop;
	return(1);
}

set(b)
{

	mode[2] =| b;
}

reset(b)
{

	mode[2] =& ~b;
}
