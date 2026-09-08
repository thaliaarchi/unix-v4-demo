#
/*
 * cp oldfile newfile
 */

#include "/usr/sys/stat.h"

main(argc,argv)
char **argv;
{
	int buf[256];
	struct stat stbuf;
	char name[100];
	int fold, fnew, n, ct, tell, preserve;
	char *p1, *p2, *bp;

	tell = preserve = 0;
	while(argc > 1 && argv[1][0] == '-' && argv[1][2] == 0) {
		switch(argv[1][1]) {
		case 't':
			tell = 1;
			goto bump;
		case 'p':
			preserve = 1;
bump:
			argc--;
			argv++;
			continue;
		default:
			break;
		}
		break;
	}
	if(argc != 3) {
		write(2, "Usage: cp oldfile newfile\n", 26);
		exit(1);
	}
	if((fold = open(argv[1], 0)) < 0) {
		write(2, "Cannot open old file.\n", 22);
		exit(1);
	}
	p2 = argv[2];
	bp = name;
	while(*bp++ = *p2++);
	fstat(fold, &stbuf);
	if((fnew = creat(argv[2], stbuf.st_mode)) < 0){
		stat(argv[2], buf);
		if((buf->st_mode & 060000) == 040000) {
			p1 = argv[1];
			bp[-1] = '/';
			p2 = bp;
			while(*bp = *p1++)
				if(*bp++ == '/')
					bp = p2;
			if((fnew = creat(name, stbuf.st_mode)) < 0) {
				write(2, "Cannot creat new file.\n", 23);
				exit(1);
			}
		} else {
			write(2, "Cannot creat new file.\n", 23);
			exit(1);
		}
	}
	ct = 0;
	while(n = read(fold, buf, 512)) {
		if(n < 0) {
			write(2, "Read error.\n", 12);
			exit(1);
		} else if(write(fnew, buf, n) != n) {
			write(2, "Write error.\n", 13);
			exit(1);
		}
		ct++;
	}
	if(preserve && mdate(name, stbuf.st_mtime) < 0) {
		write(2, "Cannot preserve date.\n", 22);
		exit(1);
	}
	if(tell) {
		conf(ct, 6, buf);
		buf[3] = '\n';
		write(1, buf, 7);
	}
	exit(0);
}

conf(n,width,buf) 
	char	*buf;
{
	auto	i,a;

	i = width;
	while(i--)
		buf[i] = ' ';

	buf[(a = n/10)?conf(a,--width,buf):--width] = n%10 + '0';

	return(++width);
}
