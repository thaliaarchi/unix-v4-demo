#
/*
 * Arbitrary-precision integers for Whitespace
 * by Thalia Archibald, 2026
 */

#define NULL 0
/* The multiple of bytes to allocate at once.
 * Should be divisible by 64. */
#define BLKSIZ 512

/* Header for an arbitrary-precision integer.
 * The limbs directly follow it, inline. */
struct zhdr {
	/* The absolute value is the number of limbs representing this number.
	 * If negative, this is a negative number. */
	int z_len;
	/* The number of allocated limbs. */
	int z_cap;
	/* The count of references to this number. */
	int z_refs;
};
/* sizeof zhdr. TODO: Replace with sizeof. */
#define ZSIZE 6

struct freehdr {
	struct freehdr *f_next;	/* Next block on free list */
	int	f_size;		/* Size of this block */
};

struct freehdr *freep NULL;

struct zhdr *
zalloc(size)
{
	register struct freehdr *p;

	size =+ ZSIZE;
	for (p = freep; ; p = p->f_next) {
		if (p == NULL) {
		} else if (p->f_size >= size) {
		}
	}
}
