#include "fs.h"
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <fuse_lowlevel.h>

#include "args.h"

uint8 *fsdata;
int fslen;

void
panic(char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	fprintf(stderr, "\n");
	va_end(ap);
	exit(1);
}

void*
emalloc(int size)
{
	void *p;
	p = malloc(size);
	if(p == nil)
		panic("error: no memory");
	return p;
}

FILE*
mustopen(const char *name, const char *mode)
{
	FILE *f;
	if(f = fopen(name, mode), f == nil)
		panic("couldn't open file: %s", name);
	return f;
}


typedef struct Options Options;
struct Options
{
	int singlethread;
	int readonly;
	int foreground;
	int debug;
	int nodefault_subtype;
	char *device;
	char *mountpoint;
	int show_version;
	int show_help;
	int clone_fd;
	unsigned int max_idle_threads;
};
static Options options;

#define offsetof(type, field) ((long)&((type*)0)->field)
#define OPTION(t, p) \
	{ t, offsetof(Options, p), 1 }

static const struct fuse_opt myopts[] = {
	OPTION("-h",		show_help),
	OPTION("--help",	show_help),
	OPTION("-V",		show_version),
	OPTION("--version",	show_version),
	OPTION("-d",		debug),
	OPTION("debug",	debug),
	OPTION("-d",		foreground),
	OPTION("debug",	foreground),
	FUSE_OPT_KEY("-d",		FUSE_OPT_KEY_KEEP),
	FUSE_OPT_KEY("debug",		FUSE_OPT_KEY_KEEP),
	OPTION("-f",		foreground),
	OPTION("-s",		singlethread),
	OPTION("-r",		readonly),
	OPTION("clone_fd",	clone_fd),
	OPTION("max_idle_threads=%u", max_idle_threads),
	FUSE_OPT_END
};

static int
opt_proc(void *data, const char *arg, int key, struct fuse_args *outargs)
{
	(void)data; (void)outargs;
	switch(key){
	case FUSE_OPT_KEY_NONOPT:
		if(options.device == nil){
			options.device = strdup(arg);
			return 0;
		}
		if(options.mountpoint == nil){
			options.mountpoint = strdup(arg);
			return 0;
		}
		return -1;
	default:
		panic("invalid option");
	}
	return -1;
}

void
usage(void)
{
	fprintf(stderr,
		" usage: v6fs [options] filesystem mountpoint\n"
		"    -h   --help            print help\n"
		"    -V   --version         print version\n"
		"    -d   -o debug          enable debug output (implies -f)\n"
		"    -f                     foreground operation\n"
		"    -s                     disable multi-threaded operation\n"
		"    -o clone_fd            use separate fuse device fd for each thread\n"
		"                           (may improve performance)\n"
		"    -o max_idle_threads    the maximum number of idle worker threads\n"
		"                           allowed (default: 10)\n");
	exit(1);
}





void
dirbuf_add(fuse_req_t req, Dirbuf *b, const char *name, fuse_ino_t ino)
{
	struct stat stbuf;
	size_t oldsize = b->size;
	b->size += fuse_add_direntry(req, NULL, 0, name, NULL, 0);
	b->p = (char *) realloc(b->p, b->size);
	memset(&stbuf, 0, sizeof(stbuf));
	stbuf.st_ino = ino;
	fuse_add_direntry(req, b->p + oldsize, b->size - oldsize, name, &stbuf,
			  b->size);
}

#define min(x, y) ((x) < (y) ? (x) : (y))

int
reply_buf_limited(fuse_req_t req, const char *buf, size_t bufsize,
	off_t off, size_t maxsize)
{
	if(off < bufsize)
		return fuse_reply_buf(req, buf + off,
				      min(bufsize - off, maxsize));
	else
		return fuse_reply_buf(req, NULL, 0);
}

static void
vfs_getattr(fuse_req_t req, fuse_ino_t ino, struct fuse_file_info *fi)
{
	struct stat stbuf;
	USED(fi);

	memset(&stbuf, 0, sizeof(stbuf));
	if(fs_stat(ino, &stbuf) == -1)
		fuse_reply_err(req, ENOENT);
	else
		fuse_reply_attr(req, &stbuf, 1.0);
}

static void
vfs_setattr(fuse_req_t req, fuse_ino_t ino, struct stat *attr, int to_set, struct fuse_file_info *fi)
{
	int ret = 0;
	USED(fi);

	if(to_set & FUSE_SET_ATTR_ATIME)
		ret = fs_atime(ino, attr->st_atime);
	if(to_set & FUSE_SET_ATTR_MTIME)
		ret = fs_mtime(ino, attr->st_mtime);
	if(to_set & FUSE_SET_ATTR_GID)
		ret = fs_gid(ino, attr->st_gid);
	if(to_set & FUSE_SET_ATTR_UID)
		ret = fs_uid(ino, attr->st_uid);

	if(ret)
		fuse_reply_err(req, ret);
	else
		vfs_getattr(req, ino, fi);
}

void
vfs_open(fuse_req_t req, fuse_ino_t ino, struct fuse_file_info *fi)
{
	int e;

	e = fs_open(ino, fi->flags);
	if(e)
		fuse_reply_err(req, e);
	else
		fuse_reply_open(req, fi);
}

void
vfs_read(fuse_req_t req, fuse_ino_t ino, size_t size,
	off_t off, struct fuse_file_info *fi)
{
	USED(fi);
	char *buf;
	int n;

	buf = malloc(size);
	n = fs_read(ino, buf, off, size);
	fuse_reply_buf(req, buf, n);
	free(buf);
}

void
vfs_write(fuse_req_t req, fuse_ino_t ino, const char *buf,
	size_t size, off_t off, struct fuse_file_info *fi)
{
	int n;
	USED(fi);
	n = fs_write(ino, (void*)buf, off, size);
	fuse_reply_write(req, n);
}

void
vfs_readdir(fuse_req_t req, fuse_ino_t ino, size_t size,
	off_t off, struct fuse_file_info *fi)
{
	struct dirent *dirs, *dp;
	Dirbuf b;

	USED(fi);
	dirs = fs_readdir(ino);
	if(dirs == nil){
		fuse_reply_err(req, ENOTDIR);
		return;
	}
	memset(&b, 0, sizeof(b));
	for(dp = dirs; dp->d_ino; dp++)
		dirbuf_add(req, &b, dp->d_name, dp->d_ino);
	free(dirs);
	reply_buf_limited(req, b.p, b.size, off, size);
	free(b.p);
}

/* returns static pointer! */
struct dirent*
lookup(uint ino, const char *name)
{
	struct dirent *dirs, *dp;
	static struct dirent de;

	dirs = fs_readdir(ino);
	if(dirs == nil)
		return nil;
	for(dp = dirs; dp->d_ino; dp++)
		if(strcmp(dp->d_name, name) == 0){
			de = *dp;
			free(dirs);
			return &de;
		}
	free(dirs);
	return nil;
}

void
vfs_lookup(fuse_req_t req, fuse_ino_t parent, const char *name)
{
	struct fuse_entry_param e;
	struct dirent *de;

	de = lookup(parent, name);
	if(de == nil){
		fuse_reply_err(req, ENOENT);
		return;
	}
	memset(&e, 0, sizeof(e));
	e.ino = de->d_ino;
	e.attr_timeout = 1.0;
	e.entry_timeout = 1.0;
	fs_stat(e.ino, &e.attr);

	/* increment ref count */
	fs_iget(e.ino);
	fuse_reply_entry(req, &e);
}

void
vfs_forget(fuse_req_t req, fuse_ino_t ino, uint64_t nlookup)
{
	Inode *ip;

	ip = fs_iget(ino);
	fs_iput(ip);
	while(nlookup--)
		fs_iput(ip);
	fuse_reply_none(req);
}

void
vfs_mknod(fuse_req_t req, fuse_ino_t parent, const char *name, mode_t mode, dev_t rdev)
{
	int ret;

	ret = fs_mknod(parent, name, mode, rdev, nil);
	if(ret)
		fuse_reply_err(req, ret);
	else
		vfs_lookup(req, parent, name);
}

void
vfs_mkdir(fuse_req_t req, fuse_ino_t parent, const char *name, mode_t mode)
{
	int ret;

	ret = fs_mkdir(parent, name, mode);
	if(ret)
		fuse_reply_err(req, ret);
	else
		vfs_lookup(req, parent, name);
}

void
vfs_unlink(fuse_req_t req, fuse_ino_t parent, const char *name)
{
	int ret;

	ret = fs_unlink(parent, name);
	fuse_reply_err(req, ret);
}

int
isdirempty(struct dirent *de)
{
	for(; de->d_ino; de++)
		if(strcmp(de->d_name, ".") != 0 &&
		   strcmp(de->d_name, "..") != 0)
			return 0;
	return 1;
}

void
vfs_rmdir(fuse_req_t req, fuse_ino_t parent, const char *name)
{
	struct dirent *dir, *de;
	int ret;
	int dino;

	ret = 0;
	de = lookup(parent, name);
	/* this shouldn't happen */
	if(de == nil)
		return;
	dino = de->d_ino;
	dir = fs_readdir(dino);
	assert(dir);
	if(!isdirempty(dir)){
		free(dir);
		fuse_reply_err(req, ENOTEMPTY);
		return;
	}
	for(de = dir; de->d_ino; de++)
		fs_unlink(dino, de->d_name);
	free(dir);
	fs_unlink(parent, name);

	fuse_reply_err(req, ret);
}

void
vfs_link(fuse_req_t req, fuse_ino_t ino,
	fuse_ino_t newparent, const char *newname)
{
	int ret;

	ret = fs_link(ino, newparent, newname);
	if(ret)
		fuse_reply_err(req, ret);
	else
		vfs_lookup(req, newparent, newname);
}

static struct fuse_lowlevel_ops hello_ll_oper = {
	.lookup		= vfs_lookup,
	.getattr	= vfs_getattr,
	.setattr	= vfs_setattr,
	.readdir	= vfs_readdir,
	.open		= vfs_open,
	.read		= vfs_read,
	.write		= vfs_write,
	.mknod		= vfs_mknod,
	.mkdir		= vfs_mkdir,
	.unlink		= vfs_unlink,
	.rmdir		= vfs_rmdir,
	.link		= vfs_link,
	.forget		= vfs_forget,
	// TODO
	// rename
};


void
fsload(const char *filename)
{
	FILE *f = mustopen(filename, "rb");
	fseek(f, 0, SEEK_END);
	fslen = ftell(f);
	fseek(f, 0, SEEK_SET);
	fsdata = emalloc(fslen);
	fread(fsdata, 1, fslen, f);
	fclose(f);
}

void
fsdump(FILE *f)
{
	fwrite(fsdata, 1, fslen, f);
}

int
main(int argc, char *argv[])
{
	struct fuse_args args = FUSE_ARGS_INIT(argc, argv);
	struct fuse_session *se;
	char *m;
	int ret = -1;

	if(fuse_opt_parse(&args, &options, myopts, opt_proc) == -1 ||
	   options.device == nil || options.mountpoint == nil)
		usage();

	m = options.mountpoint;
	options.mountpoint = realpath(m, nil);
	if(options.mountpoint == nil)
		panic("bad mount point %s", m);

	printf("device: %s\n", options.device);
	printf("mountpoint: %s\n", options.mountpoint);

	if(options.show_help){
		usage();
		return 0;
	}
	if(options.show_version){
		printf("FUSE library version %s\n", fuse_pkgversion());
		fuse_lowlevel_version();
		return 0;
	}

	fsload(options.device);
	fs_init();

	se = fuse_session_new(&args, &hello_ll_oper,
			      sizeof(hello_ll_oper), NULL);
	if(se == NULL)
		return 1;

	if(fuse_set_signal_handlers(se) != 0)
		goto err_out2;

	if(fuse_session_mount(se, options.mountpoint) != 0)
		goto err_out3;

	// for some reason we have to open the file before daemonize
	FILE *f = nil;
	if(!options.readonly)
		f = mustopen("dump", "wb");
	fuse_daemonize(options.foreground);

	/* Block until ctrl+c or fusermount -u */
	if(options.singlethread)
		ret = fuse_session_loop(se);
	else
		ret = fuse_session_loop_mt(se, options.clone_fd);

	fuse_session_unmount(se);
	if(f){
		printf("dumping disk\n");
		fsdump(f);
		fclose(f);
	}
err_out3:
	fuse_remove_signal_handlers(se);
err_out2:
	fuse_session_destroy(se);
	free(options.mountpoint);
	fuse_opt_free_args(&args);

	return ret ? 1 : 0;
}
