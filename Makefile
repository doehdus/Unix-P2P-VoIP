
CC = cc

CFLAGS = -Wall -Wextra -O2
PA_CFLAGS = $(shell pkg-config --cflags portaudio-2.0)
PA_LIBS = $(shell pkg-config --libs portaudio-2.0)

TARGET = voicechat

OBJS = main.o capture.o playback.o ipc.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(PA_LIBS)

main.o: main.c config.h capture.h playback.h ipc.h
	$(CC) $(CFLAGS) -c main.c

capture.o: capture.c capture.h config.h
	$(CC) $(CFLAGS) $(PA_CFLAGS) -c capture.c

playback.o: playback.c playback.h config.h
	$(CC) $(CFLAGS) $(PA_CFLAGS) -c playback.c

ipc.o: ipc.c ipc.h config.h
	$(CC) $(CFLAGS) -c ipc.c

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all run clean
