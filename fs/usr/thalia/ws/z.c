#
/*
 * Arbitrary-precision integers for Whitespace
 * by Thalia Archibald, 2026
 */

#define NULL 0

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
	struct freehdr *next;	/* Next block on free list */
	int	size;		/* Size of this block */
};

struct freehdr *freep NULL;

struct zhdr *
zalloc(len)
{
