main()
{
	int i;
	for (i = 0; i < 100; i++) {
		write(1, "\007", 1);
		sleep(2);
	}
}
