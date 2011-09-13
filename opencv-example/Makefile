all: loadimg

loadimg: loadimg.c
	$(CC) -g -o $@ $^ `pkg-config opencv --cflags --libs`

clean: 
	rm -f *.o
	rm -f *~
	rm -f loadimg
