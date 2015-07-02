all: facedetect-svn

develop: clean edit facedetect-svn run

facedetect-svn: facedetect.c
	#$(CC) -O2 -O2 -Wall -W -D_REENTRANT  -fopenmp -Wl,-O1 -g -o $@ $^ `pkg-config opencv gthread-2.0 --cflags --libs`
	g++ -O2 -O2 -Wall -W -D_REENTRANT  -fopenmp -Wl,-O1 -g -o $@ $^ `pkg-config opencv gthread-2.0 --cflags --libs`
	#g++ -O2 -O2 -Wall -W -D_REENTRANT  -fopenmp -Wl,-O1 -L/usr/local/lib -lpthread -lopencv_core -lopencv_highgui -lopencv_objdetect -I/usr/local/include/opencv -I. -o "facedetect" facedetect.c

facedetect-mac: facedetect.c
	g++ -O2 -O2 -Wall -W -D_REENTRANT -g -o $@ $^ `pkg-config opencv gthread-2.0 --cflags --libs`

# Compile with Intel IPP
facedetect-ipp: facedetect.c
	g++ -c -O2 -O2 -Wall -W -D_REENTRANT -fopenmp -I/opt/intel/ipp/6.1.6.063/ia32/include -I/usr/local/include/opencv -I. -o "facedetect.o" facedetect.c

# OpenCV SVN \w IPP
facedetect-ipp-svn: facedetect.c
	g++ -O2 -O2 -Wall -W -D_REENTRANT  -fopenmp -Wl,-O1 -L/opt/intel/ipp/6.1.6.063/ia32/sharedlib -L/usr/local/lib -lpthread -lippi -lopencv_core -lopencv_highgui -lopencv_objdetect -lippcore -lsvml -limf -lirc -liomp5 -I/usr/local/include/opencv -I. -o "facedetect" facedetect.c

# OpenCV 2.1
facedetect:
	g++ -O2 -O2 -Wall -W -D_REENTRANT -fopenmp -Wl,-O1 -L/usr/local/lib -lpthread -lcv -lcxcore -lhighgui  -I/usr/local/include/opencv -I. -o "facedetect" facedetect.c

clean:
	rm -f *.o
	rm -f *~
	rm -f facedetect

run:
	./facedetect-svn --cascade="data/haarcascade_frontalface_alt.xml" 0

edit:
	vi facedetect.c
