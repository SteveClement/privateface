all: face_detect


cam_display: cam_display.c
	$(CC) $(CFLAGS) -o $@ $^ `pkg-config --cflags --libs libunicap libucil opencv glib-2.0`

face_detect: face_detect.c
	$(CC) $(CFLAGS) -o $@ $^ `pkg-config --cflags --libs libunicap libucil opencv glib-2.0 sdl ` -lSDL_image


clean:
	rm -f *.o
	rm -f *~
	rm -f cam_display
	rm -f face_detect
