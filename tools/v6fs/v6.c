#include "fs.h"

/* NB: not portable! assumes little endian (and probably other things) */

#define NIPB (512/sizeof(DInode))
#define NDPB (512/sizeof(Dirent))

/* v6 superblock */
typedef struct Filsys Filsys;
struct Filsys
{
	int16	s_isize;	/* size in blocks of I list */
	int16	s_fsize;	/* size in blocks of entire volume */
	int16	s_nfree;	/* number of in core free blocks (0-100) */
	int16	s_free[100];	/* in core free blocks */
	int16	s_ninode;	/* number of in core I nodes (0-100) */
	int16	s_inode[100];	/* in core free I nodes */
	uint8	s_flock;	/* lock during free list manipulation */
	uint8	s_ilock;	/* lock during I list manipulation */
	uint8	s_fmod;		/* super block modified flag */
	uint8	s_ronly;	/* mounted read-only flag */
	int16	s_time[2];	/* current date of last update */
	int16	pad[48];
};

/* v6 and PWB/1.0 disk inode */
struct DInode
{
	uint16	i_mode;
	int8	i_nlink;
	uint8	i_uid;
	uint8	i_gid;
	uint8	i_size0;
	uint16	i_size1;
	uint16	i_addr[8];
	uint16	i_atime[2];
	uint16	i_mtime[2];
};

/* modes */
#define	IALLOC	0100000
#define	IFMT	060000
#define	IFDIR	040000
#define	IFCHR	020000
#define	IFBLK	060000
#define	ILARG	010000
#define	ISUID	04000
#define	ISGID	02000
#define ISVTX	01000
#define	IREAD	0400
#define	IWRITE	0200
#define	IEXEC	0100

typedef struct Dirent Dirent;
struct Dirent
{
	uint16 inode;
	char name[14];
};

Filsys *filesys;
DInode *dinodes;
Inode *inodes;

// consistency checks
int *blockmap;
int *usedino;

#define ISIZE(ip) ((ip)->i_size0<<16 | (ip)->i_size1)
int32
pdplong(void *p)
{
	uint8 *up;
	uint32 ui;
	up = p;
	ui = up[1]<<24 | up[0]<<16 | up[3]<<8 | up[2];
	return *(int32*)&ui;
}

void
setpdplong(void *p, int32 i)
{
	uint8 *up;
	uint32 ui;
	up = p;
	ui = *(uint32*)&i;
	up[1] = ui>>24;
	up[0] = ui>>16;
	up[3] = ui>>8;
	up[2] = ui;
}

void
setint24(void *p, int32 i)
{
	uint8 *up;
	uint32 ui;
	up = p;
	ui = *(uint32*)&i;
	up[0] = ui>>16;
	up[2] = ui>>8;
	up[1] = ui;
}

void *bget(int n) { 
	assert(n >= 0);
	assert(filesys == nil || n < filesys->s_fsize);
	return &fsdata[n*512];
}

DInode *diget(uint ino) { 
	assert(ino <= filesys->s_isize*NIPB);
	return &dinodes[ino];
}

void
dumpino(uint ino)
{
	DInode *ip;
	int16 *indb;

	ip = diget(ino);
	printf("  ino%d %o %d %d. ", ino, ip->i_mode, ip->i_nlink, ISIZE(ip));
	for(int i = 0; i < 8; i++)
		printf("b%d ", ip->i_addr[i]);
	printf("\n");
	if(ip->i_mode & ILARG){
		indb = bget(ip->i_addr[0]);
		for(int i = 0; i < 256; i++){
			printf("b%d ", indb[i]);
			if((i&7) == 7)
				printf("\n");
		}
		printf("\n");
	}
}

/* returns a free block or 0 */
int
balloc(void)
{
	int i, bno;
	uint16 *b;

	filesys->s_nfree--;
	if(filesys->s_nfree < 0 || filesys->s_nfree >= 100){
		printf("bad free count %d\n", filesys->s_nfree);
		return 0;
	}
	bno = filesys->s_free[filesys->s_nfree];
	filesys->s_free[filesys->s_nfree] = 0;
	if(bno == 0) {
		filesys->s_nfree++;
		printf("no space left\n");
		return 0;
	}
	if(bno < filesys->s_isize+2 || bno >= filesys->s_fsize){
		printf("bad free block (%d)\n", bno);
		return 0;
	}
	if(filesys->s_nfree <= 0){
		b = bget(bno);
		filesys->s_nfree = b[0];
		for(i = 0; i < 100; i++)
			filesys->s_free[i] = b[i+1];
	}
	return bno;
}

void
bfree(int bno)
{
	int i;
	uint16 *b;

		memset(bget(bno), 0, 512);
	if(filesys->s_nfree >= 100){
		assert(filesys->s_nfree == 100);
		b = bget(bno);
		b[0] = filesys->s_nfree;
		for(i = 0; i < 100; i++)
			b[i+1] = filesys->s_free[i];
		filesys->s_nfree = 0;
	}
	filesys->s_free[filesys->s_nfree] = bno;
	filesys->s_nfree++;
}

int
zalloc(void)
{
	int bn;
	bn = balloc();
	if(bn)
		memset(bget(bn), 0, 512);
	return bn;
}

/* returns a free inode or 0 */
int
ialloc(void)
{
	uint ino;
	DInode *ip;
	int i, j;

	if(filesys->s_ninode <= 0){
		ino = 0;
		for(i = 0; i < filesys->s_isize; i++)
			for(j = 0; j < NIPB; j++){
				ino++;
				ip = diget(ino);
				if(ip->i_mode)
					continue;
				filesys->s_inode[filesys->s_ninode++] = ino;
				if(filesys->s_ninode >= 100)
					goto brk;
			}
	}
brk:
	if(filesys->s_ninode > 0){
		ino = filesys->s_inode[--filesys->s_ninode];
		return ino;
	}
	return 0;
}

void
ifree(uint ino)
{
	if(filesys->s_ninode >= 100)
		return;
	filesys->s_inode[filesys->s_ninode++] = ino;
}

void
itrunc(DInode *ip)
{
	int i;
	uint16 *bp, *cp, *dp, *ep;

	if((ip->i_mode & IFMT) != 0 &&
	   (ip->i_mode & IFMT) != IFDIR)
		return;
	for(i = 7; i >= 0; i--)
	if(ip->i_addr[i]){
		if(ip->i_mode & ILARG){
			bp = bget(ip->i_addr[i]);
			for(cp = &bp[255]; cp >= bp; cp--)
			if(*cp){
				if(i == 7){
					dp = bget(*cp);
					for(ep = &dp[255]; ep >= dp; ep--)
					if(*ep)
						bfree(*ep);
				}
				bfree(*cp);
			}
		}
		bfree(ip->i_addr[i]);
		ip->i_addr[i] = 0;
	}
	ip->i_mode &= ~ILARG;
	setint24(&ip->i_size0, 0);
}

int
bmap(DInode *ip, uint bn)
{
	uint i;
	uint nb;
	uint16 *b;

	if(bn & ~077777)
		return 0;

	if((ip->i_mode & ILARG) == 0){

		/* small file, direct fetch */

		if(bn & ~7){
			/* convert to large */
			nb = zalloc();
			if(nb == 0)
				return 0;
			b = bget(nb);
			for(i = 0; i < 8; i++){
				b[i] = ip->i_addr[i];
				ip->i_addr[i] = 0;
			}
			ip->i_addr[0] = nb;
			ip->i_mode |= ILARG;
			goto large;
		}

		nb = ip->i_addr[bn];
		if(nb == 0){
			nb = zalloc();
			if(nb == 0)
				return 0;
			ip->i_addr[bn] = nb;
		}
		assert(nb < filesys->s_fsize);
		return nb;
	}

	/* large file, 7 indirect blocks */

large:
	i = bn>>8;
	if(i > 7)
		i = 7;
	nb = ip->i_addr[i];
	if(nb == 0){
		nb = zalloc();
		if(nb == 0)
			return 0;
		ip->i_addr[bn] = nb;
	}
	b = bget(nb);

	if(i == 7){

		/* huge file, double indirect last block */

		i = (bn>>8) - 7;
		nb = b[i&0377];
		if(nb == 0){
			nb = zalloc();
			if(nb == 0)
				return 0;
			b[i&0377] = nb;
		}
		b = bget(nb);
	}

	nb = b[bn&0377];
	if(nb == 0){
		nb = zalloc();
		if(nb == 0)
			return 0;
		b[bn&0377] = nb;
	}
	assert(nb < filesys->s_fsize);
	return nb;
};

uint8*
getblock(DInode *ip, uint bn)
{
	bn = bmap(ip, bn);
	if(bn == 0) {
		return nil;
	}
	return bget(bn);
}

void
markb(int bno, uint ino)
{
	if(bno == 0) return;
	if(blockmap[bno])
		printf("warning: block %d used by inodes %d and %d\n",
			bno, blockmap[bno], ino);
	blockmap[bno] = ino;
}

void
markblocks(uint ino)
{
	int16 *indb;
	DInode *ip = diget(ino);

	for(int i = 0; i < 8; i++){
		markb(ip->i_addr[i], ino);
		if((ip->i_mode & ILARG) && ip->i_addr[i]){
			indb = bget(ip->i_addr[i]);
			for(int j = 0; j < 256; j++){
				markb(indb[j], ino);
			}
		}
	}
}

void marki(uint ino);

void
markdir(uint ino)
{
	struct dirent *d, *dir = fs_readdir(ino);
	for(d = dir; d->d_ino; d++)
		marki(d->d_ino);
	free(dir);
}

void
marki(uint ino)
{
	if(usedino[ino]) return;
	usedino[ino] = 1;

	DInode *ip = diget(ino);
	markblocks(ino);
	if((ip->i_mode & IFMT) == IFDIR)
		markdir(ino);
}

void
dumpilist(void)
{
	printf("	ilist\n");
	for(int i = 1; i < filesys->s_isize*NIPB + 1; i++) {
		DInode *ip = diget(i);
		if(usedino[i] == 0) {
			if(ip->i_mode&IALLOC)
				printf("warning: abandoned ino%d still allocated\n", i);
		} else {
			if(!(ip->i_mode&IALLOC))
				printf("warning: used ino%d not allocated\n", i);
			dumpino(i);
		}
	}
	printf("	end ilist\n");
}

/* mark block usage and used inodes */
void
mark(void)
{
	int nb = filesys->s_fsize;
	int ni = filesys->s_isize*NIPB + 1;

	free(blockmap);
	free(usedino);
	blockmap = malloc(nb*sizeof(int));
	usedino = malloc(ni*sizeof(int));
	for(int i = 0; i < nb; i++) blockmap[i] = i < filesys->s_isize+2 ? 2 : 0;
	for(int i = 0; i < ni; i++) usedino[i] = 0;
	marki(1);

	int nbfree=0;
	int nifree=0;
	for(int i = 1; i < nb; i++) if(blockmap[i] == 0) nbfree++;
	for(int i = 1; i < ni; i++) if(usedino[i] == 0) nifree++;
	printf("actually free: %d %d\n", nbfree, nifree);
}

/* check free blocks against actually free ones */
void
fcheck(void)
{
	Filsys fs;
	int i, bn;

	mark();

	fs = *filesys;
	i = 0;
	while(filesys->s_free[filesys->s_nfree-1] && (bn = balloc())){
//	while(bn = balloc()){
		if(blockmap[bn] != 0)
			printf("warning: used b%d on freelist ino%d\n",
				bn, blockmap[bn]);
		blockmap[bn] = -1;
		i++;
	}
	printf("free: %d\n", i);
	*filesys = fs;

	for(i = 1; i < filesys->s_fsize; i++){
		if(blockmap[i] == 0)
			printf("abandoned b%d\n", i);
	}
}

/* fix free list and unused inodes */
void
ffix(void)
{
	int nb = filesys->s_fsize;
	int ni = filesys->s_isize*NIPB + 1;

	mark();

	// init free list
	filesys->s_nfree = 0;
	bfree(0);

	for(int i = 1; i < nb; i++) if(blockmap[i] == 0) bfree(i);
	for(int i = 1; i < ni; i++) {
		if(usedino[i]) continue;
		DInode *ip = diget(i);
		if(ip->i_mode != 0)
			printf("warning: unused ino%d mode %d\n", i, ip->i_mode);
		ip->i_mode = 0;
	}
}

void
fs_init(void)
{
	int i, ni;

	filesys = bget(1);
	dinodes = (DInode*)bget(2) - 1;
	ni = filesys->s_isize*NIPB + 1;
	inodes = malloc(ni*sizeof(Inode));
	for(i = 0; i < ni; i++){
		inodes[i].ino = i;
		inodes[i].count = 0;
		inodes[i].i = &dinodes[i];
	}

printf("fs: %d %d\n", filesys->s_fsize, filesys->s_isize);
//	fcheck();
//	ffix();
//	dumpilist();
}

int
fs_open(uint ino, int flags)
{
	DInode *ip;

	ip = diget(ino);
//printf("	opening ino%d %d\n", ino, ISIZE(ip)) ;
//dumpino(ino);
	if((ip->i_mode & IFMT) == IFDIR)
		return EISDIR;
//	if((flags & 3) != O_RDONLY)
//		return EACCES;
	return 0;
}

int
fs_stat(uint ino, struct stat *stbuf)
{
	DInode *ip;
	ip = diget(ino);

	stbuf->st_ino = ino;
	if((ip->i_mode & IFMT) == IFDIR)
		stbuf->st_mode = S_IFDIR;
	else if((ip->i_mode & IFMT) == IFCHR)
		stbuf->st_mode = S_IFCHR;
	else if((ip->i_mode & IFMT) == IFBLK)
		stbuf->st_mode = S_IFBLK;
	else
		stbuf->st_mode = S_IFREG;
	stbuf->st_mode |= ip->i_mode & 0777;
	stbuf->st_nlink = ip->i_nlink;
	stbuf->st_size = ISIZE(ip);
	if((ip->i_mode & IFMT) == IFCHR ||
	   (ip->i_mode & IFMT) == IFBLK)
		stbuf->st_rdev = makedev(ip->i_addr[0]>>8 & 0xFF, ip->i_addr[0]&0xFF);
	if(fs_no_same_owner){
		stbuf->st_uid = fs_owner_uid;
		stbuf->st_gid = fs_owner_gid;
	} else {
		stbuf->st_uid = ip->i_uid;
		stbuf->st_gid = ip->i_gid;
	}
	stbuf->st_atime = pdplong(ip->i_atime);
	stbuf->st_mtime = pdplong(ip->i_mtime);

	return 0;
}

int
fs_atime(uint ino, int32 time)
{
	DInode *ip;
	ip = diget(ino);
	setpdplong(ip->i_atime, time);
	return 0;
}

int
fs_mtime(uint ino, int32 time)
{
	DInode *ip;
	ip = diget(ino);
	setpdplong(ip->i_mtime, time);
	return 0;
}

int
fs_uid(uint ino, int32 uid)
{
	DInode *ip;
	ip = diget(ino);
	ip->i_uid = uid;
	return 0;
}

int
fs_gid(uint ino, int32 gid)
{
	DInode *ip;
	ip = diget(ino);
	ip->i_gid = gid;
	return 0;
}

int
fs_read(uint ino, void *vdst, int offset, int len)
{
	DInode *ip;
	int bn;
	uint8 *b;
	int isize;
	int n;
	uint8 *dst;

	ip = diget(ino);
	dst = vdst;
	isize = ISIZE(ip);
	if(offset + len > isize)
		len = isize - offset;
	if(len <= 0)
		return 0;

	bn = offset/512;
	offset %= 512;

	b = getblock(ip, bn);
	if(b == nil)
		/* TODO: handle gracefully */
		panic("no block");

	if(len < 512-offset){
		/* smaller than block */
		memcpy(dst, b+offset, len);
		return len;
	}

	/* at least one block */
	memcpy(dst, b+offset, 512-offset);
	n = 512-offset;
	len -= n;
	dst += n;
	while(len > 0){
		bn++;
		b = getblock(ip, bn);
		if(b == nil)
			/* TODO: handle gracefully */
			panic("no block");
		memcpy(dst, b, len > 512 ? 512 : len);
		n += len > 512 ? 512 : len;
		len -= 512;
		dst += 512;
	}
	return n;
}

int
fs_write(uint ino, void *vsrc, int offset, int len)
{
	DInode *ip;
	int bn;
	uint8 *b;
	int n;
	int32 size;
	uint8 *src;

	ip = diget(ino);
	src = vsrc;
	size = ISIZE(ip);
	// TODO: make this better:
	if(offset+len > size)
		setint24(&ip->i_size0, offset+len);
	if(len <= 0)
		return 0;

	bn = offset/512;
	offset %= 512;

	b = getblock(ip, bn);
	if(b == nil)
		/* TODO: handle gracefully */
		panic("no block 1");

	if(len < 512-offset){
		/* smaller than block */
		memcpy(b+offset, src, len);
		return len;
	}

	/* at least one block */
	memcpy(b+offset, src, 512-offset);
	n = 512-offset;
	len -= n;
	src += n;
	while(len > 0){
		bn++;
		b = getblock(ip, bn);
		if(b == nil)
			/* TODO: handle gracefully */
			panic("no block 2");
		memcpy(b, src, len > 512 ? 512 : len);
		n += len > 512 ? 512 : len;
		len -= 512;
		src += 512;
	}
	return n;
}

/* return a malloc'd array of dirents for a given inode */
struct dirent*
fs_readdir(uint ino)
{
	DInode *ip;
	struct dirent *des, *dp;
	int size;
	int ndes;
	Dirent de;
	int offset;

	ip = diget(ino);
	if((ip->i_mode & IFDIR) == 0)
		return nil;

	size = ISIZE(ip);
	ndes = size/sizeof(Dirent);
	des = malloc((ndes+1)*sizeof(struct dirent));

	offset = 0;
	dp = des;
	while(fs_read(ino, &de, offset, sizeof(Dirent)) == sizeof(Dirent)){
		if(de.inode){
			dp->d_ino = de.inode;
			memcpy(dp->d_name, de.name, 14);
			dp->d_name[14] = '\0';
			dp++;
		}
		offset += sizeof(Dirent);
	}
	dp->d_ino = 0;
	dp->d_name[0] = '\0';

	return des;
}

static int
lookup(uint ino, const char *name, Dirent *de)
{
	int offset;

	assert(de);
	offset = 0;
	while(fs_read(ino, de, offset, sizeof(Dirent)) == sizeof(Dirent)){
		if(de->inode)
			if(strcmp(de->name, name) == 0)
				return offset;
		offset += sizeof(Dirent);
	}
	memset(de, 0, sizeof(Dirent));
	return -1;
}

static int
allocdirent(uint ino)
{
	Dirent de;
	int offset;

	offset = 0;
	while(fs_read(ino, &de, offset, sizeof(Dirent)) == sizeof(Dirent)){
		if(de.inode == 0)
			return offset;
		offset += sizeof(Dirent);
	}
	memset(&de, 0, sizeof(Dirent));
	fs_write(ino, &de, offset, sizeof(Dirent));
	return offset;
}

int
fs_link(uint ino, uint parent, const char *name)
{
	Dirent de;
	int offset;

	offset = allocdirent(parent);
	/* TODO: can't happen right now */
	if(offset < 0)
		return ENOSPC;
	memset(&de, 0, sizeof(Dirent));
	de.inode = ino;
	strncpy(de.name, name, 14);
	fs_write(parent, &de, offset, sizeof(de));
	fs_mtime(ino, time(nil));
	fs_atime(ino, time(nil));
	dinodes[ino].i_nlink++;

	return 0;
}

int
fs_mknod(uint parent, const char *name, mode_t mode, dev_t rdev, uint *newino)
{
	uint ino;
	DInode *ip, *ipp;
	int offset;
	Dirent de;

	USED(rdev);
	// TODO: support more files
	if((mode & S_IFMT) != S_IFREG &&
	   (mode & S_IFMT) != S_IFDIR)
		return EACCES;

	ino = ialloc();
	if(ino == 0)
		return ENOSPC;
	ip = diget(ino);
	memset(ip, 0, sizeof(DInode));
	ip->i_mode = (mode&0777) | IALLOC;
	if((mode & S_IFMT) == S_IFDIR)
		ip->i_mode |= IFDIR;
	else if((mode & S_IFMT) == S_IFCHR)
		ip->i_mode |= IFCHR;
	else if((mode & S_IFMT) == S_IFBLK)
		ip->i_mode |= IFBLK;
	if((ip->i_mode & IFMT) == IFCHR ||
	   (ip->i_mode & IFMT) == IFBLK)
		ip->i_addr[0] =
			major(rdev)<<8 & 0xFF00 | minor(rdev) & 0xFF;

	ipp = &dinodes[parent];
	assert((ipp->i_mode & IFMT) == IFDIR);

	offset = allocdirent(parent);
	/* TODO: can't happen right now */
	if(offset < 0){
		ip->i_mode = 0;
		return ENOSPC;
	}

	ip->i_nlink = 1;
	memset(&de, 0, sizeof(Dirent));
	de.inode = ino;
	strncpy(de.name, name, 14);
	fs_write(parent, &de, offset, sizeof(de));
	fs_mtime(ino, time(nil));
	fs_atime(ino, time(nil));

	if(newino)
		*newino = ino;
	printf("making node %s %o\n", name, mode);
	return 0;
}

int
fs_mkdir(uint parent, const char *name, mode_t mode)
{
	int bn;
	int ino;
	int ret;
	DInode *ip;
	Dirent des[2];

	/* Make sure we have space */
	bn = balloc();
	if(bn == 0)
		return ENOSPC;
	ret = fs_mknod(parent, name, mode | S_IFDIR, 0, &ino);
	if(ret){
		bfree(bn);
		return ret;
	}
	ip = diget(ino);
	ip->i_addr[0] = bn;

	memset(des, 0, 2*sizeof(Dirent));
	des[0].inode = ino;
	strcpy(des[0].name, ".");
	ip->i_nlink++;
	des[1].inode = parent;
	strcpy(des[1].name, "..");
	dinodes[parent].i_nlink++;
	fs_write(ino, des, 0, 2*sizeof(Dirent));
	return 0;
}

Inode*
fs_iget(uint ino)
{
	Inode *ip;
	ip = &inodes[ino];
	ip->count++;
	return ip;
}

void
fs_iput(Inode *ip)
{
	assert(ip->count > 0);
	ip->count--;
	if(ip->count == 0)
		if(ip->i->i_nlink <= 0){
			itrunc(ip->i);
			ip->i->i_mode = 0;
			ifree(ip->ino);
		}
}

int
fs_unlink(uint parent, const char *name)
{
	Dirent de;
	int offset;

	offset = lookup(parent, name, &de);
	if(offset < 0)
		/* shouldn't happen */
		return 1;
	dinodes[de.inode].i_nlink--;

	de.inode = 0;
	fs_write(parent, &de, offset, sizeof(Dirent));
	return 0;
}
