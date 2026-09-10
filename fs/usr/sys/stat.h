struct stat {
	int	st_dev;		/* device where inode resides */
	int	st_ino;		/* inumber */
	int	st_mode;	/* mode flags */
	char	st_nlink;	/* number of links to file */
	char	st_uid;		/* user ID of owner */
	char	st_gid;		/* group ID of owner */
	char	st_siz0;	/* high byte of 24-bit size */
	int	st_siz;		/* low word of 24-bit size */
	int	st_addr[8];	/* block numbers or device number */
	/*unsigned*/int	st_atime[2];	/* time of last access */
	/*unsigned*/int	st_mtime[2];	/* time of last modification */
};
