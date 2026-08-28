main()
{
	char *a, *b, *c;
	a = sbrk(0);
	b = sbrk(1);
	c = sbrk(1);
	printf("%d\n", b-a);
	printf("%d\n", c-a);
}
