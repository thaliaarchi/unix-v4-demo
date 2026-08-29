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
	/* The number of allocated limbs. Keep at the same offset as f_alloc. */
	int z_alloc;
	/* The count of references to this number. */
	int z_refs;
};
/* The size of zhdr. */
#define ZSIZE 6
