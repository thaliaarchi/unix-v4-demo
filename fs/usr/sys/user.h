struct user {
	int	u_rsav[2];		/* must be first */
	int	u_fsav[25];		/* must be second */
	char	u_segflg;
	char	u_error;
	char	u_uid;
	char	u_gid;
	char	u_ruid;
	char	u_rgid;
	int	u_procp;
	char	*u_base;
	char	*u_count;
	char	*u_offset[2];
	int	*u_cdir;
	char	u_dbuf[DIRSIZ];
	char	*u_dirp;
	struct	{
		int	u_ino;
		char	u_name[DIRSIZ];
	} u_dent;
	int	*u_pdir;
	int	u_uisa[8];
	int	u_uisd[8];
	int	u_ofile[NOFILE];
	int	u_arg[5];
	int	u_tsize;
	int	u_dsize;
	int	u_ssize;
	int	u_qsav[2];
	int	u_ssav[2];
	int	u_signal[NSIG];
	int	u_utime;
	int	u_stime;
	int	u_cutime[2];
	int	u_cstime[2];
	int	*u_ar0;
	int	u_prof[4];
	char	u_nice;
	char	u_dsleep;
} u;	/* u = 140000 */
