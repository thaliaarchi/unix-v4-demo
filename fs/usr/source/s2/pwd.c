#include "/usr/sys/stat.h"

char dot[] ".";
char dotdot[] "..";
char root[] "/";
char name[512];
int file, off -1;
struct stat x;
struct entry { int jnum; char name[16];}y;

main() {
	int n;

loop0:
	stat(dot, &x);
	if((file = open(dotdot,0)) < 0) prname();
loop1:
	if((n = read(file,&y,16)) < 16) prname();
	if(y.jnum != x.st_ino)goto loop1;
	close(file);
	if(y.jnum == 1) ckroot();
	cat();
	chdir(dotdot);
	goto loop0;
}
ckroot() {
	int i, n;

	if((n = stat(y.name,&x)) < 0) prname();
	i = x.st_dev;
	if((n = chdir(root)) < 0) prname();
	if((file = open(root,0)) < 0) prname();
loop:
	if((n = read(file,&y,16)) < 16) prname();
	if(y.jnum == 0) goto loop;
	if((n = stat(y.name,&x)) < 0) prname();
	if(x.st_dev != i) goto loop;
	x.st_mode =& 060000;
	if(x.st_mode != 040000) goto loop;
	cat();
	prname();
}
prname() {
	name[off] = '\n';
	write(1,name,off+1);
	exit();
}
cat() {
	int i, j;

	i = -1;
	while(y.name[++i] != 0);
	if((off+i+2) > 511) prname();
	for(j=off+1; j>=0; --j) name[j+i+1] = name[j];
	off=i+off+1;
	name[i] = root[0];
	for(--i; i>=0; --i) name[i] = y.name[i];
}
