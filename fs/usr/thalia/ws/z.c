#
/*
 * Arbitrary-precision integers for Whitespace
 * by Thalia Archibald, 2026
 *
 * Adapted from K&R C, Second Edition, 8.7 A Storage Allocator
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

struct freehdr {
	struct freehdr *f_next;	/* Next block on free list */
	int	f_size;		/* Size of this block */
};

struct freehdr *base { base, 0 };	/* Initial empty list */
struct freehdr *freep base;

struct zhdr *zgrow();

struct zhdr *
zalloc(limbs)
{
	register nbytes;
	register char *p, *prevp;

	nbytes = sizeof *p + limbs * 2;
	for (prevp = p = freep; ; prevp = p, p = p->f_next) {
		if (p->f_size >= nbytes) {	/* Large enough */
			if (p->f_size == nbytes)	/* Fits exactly */
				prevp->f_next = p->f_next;
			else {	/* Allocate tail */
				p->f_size =- nbytes;
				p =+ p->f_size;
				p->f_size = nbytes;
			}
			freep = prevp;
			return p;
		}
		/* Wrapped around the free list */
		if (p == freep) {
			p = zgrow(nbytes);
			if (p == NULL) {
				write(2, "ws: Out of memory\n", 18);
				exit(1);
			}
		}

	}
}

struct zhdr *
zgrow(nbytes)
{
}
