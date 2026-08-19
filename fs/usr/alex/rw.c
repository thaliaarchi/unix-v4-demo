int main() {
	char buf[64];
	int n;
	n = read(0, buf, 64);
	write(1, buf, n);
}
