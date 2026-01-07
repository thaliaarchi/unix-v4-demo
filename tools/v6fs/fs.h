#define FUSE_USE_VERSION 31
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdint.h>
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <dirent.h>
#include <time.h>
#include <sys/sysmacros.h>


#define nil NULL
typedef int8_t int8;
typedef uint8_t uint8;
typedef uint16_t uint16;
typedef int16_t int16;
typedef uint32_t uint32;
typedef int32_t int32;
typedef unsigned int uint;

#define USED(x) ((void)x)

/* the data containing our file system */
extern uint8 *fsdata;
extern int fslen;

void panic(char *fmt, ...);
void *emalloc(int size);
FILE *mustopen(const char *name, const char *mode);

typedef struct Dirbuf Dirbuf;
struct Dirbuf
{
	char *p;
	size_t size;
};

typedef struct DInode DInode;
struct DInode;

typedef struct Inode Inode;
struct Inode
{
	int count;
	int ino;	/* not really needed */
	DInode *i;
};

void fs_init(void);
Inode *fs_iget(uint ino);
void fs_iput(Inode *ip);
int fs_open(uint ino, int flags);
int fs_stat(uint ino, struct stat *stbuf);
int fs_read(uint ino, void *vdst, int offset, int len);
int fs_write(uint ino, void *vsrc, int offset, int len);
struct dirent *fs_readdir(uint ino);
int fs_mknod(uint ino, const char *name, mode_t mode, dev_t rdev, uint *newino);
int fs_mkdir(uint ino, const char *name, mode_t mode);
int fs_unlink(uint parent, const char *name);
int fs_link(uint ino, uint parent, const char *name);
int fs_atime(uint ino, int32 time);
int fs_mtime(uint ino, int32 time);
int fs_uid(uint ino, int32 uid);
int fs_gid(uint ino, int32 gid);

void dcheck(void);
