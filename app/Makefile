CC := gcc
CFLAGS := -Wall -Wextra -Werror
TARGET := ioctl_app
SRC := ioctl_app.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f $(TARGET)

.PHONY: all clean
