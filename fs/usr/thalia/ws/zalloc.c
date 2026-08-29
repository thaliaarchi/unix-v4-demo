#
/*
 * Arbitrary-precision integers for Whitespace
 * by Thalia Archibald, 2026
 *
 * Adapted from K&R C, Second Edition, 8.7 A Storage Allocator
 */

#include "z.h"

/* The multiple of bytes to allocate at once.
 * Should be divisible by 64. */
#define NALLOC 512

struct freehdr {
	/* Next block on free list. */
	struct freehdr *f_next;
	/* Size of this block. Keep at the same offset as z_alloc. */
	int	f_alloc;
};

struct freehdr *base { base, 0 };	/* Initial empty list */
struct freehdr *freep base;

struct freehdr *zgrow();

struct zhdr *
zalloc(limbs)
{
	register nbytes;
	register char *p, *prevp;

	nbytes = ZSIZE + limbs * 2;
	for (prevp = p = freep; ; prevp = p, p = p->f_next) {
		if (p->f_alloc >= nbytes) {	/* Large enough */
			if (p->f_alloc == nbytes)	/* Fits exactly */
				prevp->f_next = p->f_next;
			else {	/* Allocate tail */
				p->f_alloc =- nbytes;
				p =+ p->f_alloc;
				p->f_alloc = nbytes;
			}
			freep = prevp;
			return p;
		}
		/* Wrapped around the free list */
		if (p == freep) {
			p = zgrow(nbytes);
		}
	}
}

struct freehdr *
zgrow(nbytes)
{
	register struct freehdr *p;

	if (nbytes < NALLOC)
		nbytes = NALLOC;
	p = sbrk(nbytes);
	if (p == -1) {
		write(2, "ws: Out of memory\n", 18);
		exit(1);
	}
	p->f_alloc = nbytes;
	zfree(p);
	return freep;
}

zfree(z)
struct freehdr *z;
{
	register char *zp, *fp;

	zp = z;
	for (fp = freep; ; fp = fp->f_next) {
		if (fp < zp && zp < fp->f_next)
			break;	/* Between free blocks */
		if (fp >= fp->f_next && (fp < zp || zp < fp->f_next))
			break;	/* At start or end of arena */
	}

	if (zp + zp->f_alloc == fp->f_next) {	/* Join above */
		zp->f_alloc =+ fp->f_next->f_alloc;
		zp->f_next = fp->f_next->f_next;
	} else
		zp->f_next = fp->f_next;
	if (fp + fp->f_alloc == zp) {	/* Join below */
		fp->f_alloc =+ zp->f_alloc;
		fp->f_next = zp->f_next; /* zp->f_next already assigned */
	} else
		fp->f_next = zp;
	freep = fp;
}

zdecref(z)
struct zhdr *z;
{
	if (--z->f_refs == 0)
		zfree(z);
}
