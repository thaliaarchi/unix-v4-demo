SRC=unixfs.c v6.c
v6fs: $(SRC) fs.h args.h
	$(CC) `pkg-config fuse3 --cflags --libs` -o $@ $(SRC)

run:
	./v6fs -d disk0.rk mnt
