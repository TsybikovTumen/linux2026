CC = cc
CFLAGS = -Wall -Wextra
TARGET = Show

# ALT packages the wide library as -lncursesw; macOS exports it as -lncurses.
NCURSES_LIB := $(shell \
	tmp=$$(mktemp /tmp/show-ncurses.XXXXXX); \
	if printf 'int main(void){return 0;}\n' | $(CC) -x c - -lncursesw -o "$$tmp" >/dev/null 2>&1; then \
		rm -f "$$tmp"; echo -lncursesw; \
	else \
		rm -f "$$tmp"; echo -lncurses; \
	fi)

.PHONY: all clean

all: $(TARGET)

$(TARGET): Show.c
	$(CC) $(CFLAGS) Show.c -o $(TARGET) $(NCURSES_LIB)

clean:
	rm -f $(TARGET) *.o *~
