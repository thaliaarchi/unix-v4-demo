#define	DIRSIZ	14

struct dirent {
	int	d_ino;
	char	d_name[DIRSIZ];
};
