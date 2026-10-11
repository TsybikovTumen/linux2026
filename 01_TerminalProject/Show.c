#include <errno.h>
#include <locale.h>
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MARGIN_Y 2
#define MARGIN_X 3

typedef struct {
	char **items;
	size_t count;
	size_t cap;
	size_t max_len;
} Text;

static void text_free(Text *text)
{
	size_t i;

	for (i = 0; i < text->count; i++)
		free(text->items[i]);
	free(text->items);
	text->items = NULL;
	text->count = 0;
	text->cap = 0;
	text->max_len = 0;
}

static int text_add(Text *text, char *line)
{
	size_t len;

	if (text->count == text->cap) {
		size_t cap = text->cap == 0 ? 64 : text->cap * 2;
		char **items = realloc(text->items, cap * sizeof(*items));

		if (items == NULL)
			return -1;
		text->items = items;
		text->cap = cap;
	}

	text->items[text->count++] = line;
	len = strlen(line);
	if (len > text->max_len)
		text->max_len = len;
	return 0;
}

static int load_text(Text *text, const char *path)
{
	FILE *fp;
	char *buf = NULL;
	size_t bufcap = 0;

	fp = fopen(path, "r");
	if (fp == NULL) {
		fprintf(stderr, "Show: %s: %s\n", path, strerror(errno));
		return -1;
	}

	for (;;) {
		ssize_t nread = getline(&buf, &bufcap, fp);
		char *line;

		if (nread < 0)
			break;
		while (nread > 0 &&
		       (buf[nread - 1] == '\n' || buf[nread - 1] == '\r')) {
			buf[--nread] = '\0';
		}

		line = malloc((size_t)nread + 1);
		if (line == NULL) {
			free(buf);
			fclose(fp);
			return -1;
		}
		memcpy(line, buf, (size_t)nread + 1);
		if (text_add(text, line) != 0) {
			free(line);
			free(buf);
			fclose(fp);
			return -1;
		}
	}

	free(buf);
	fclose(fp);
	return 0;
}

static const char *file_name(const char *path)
{
	const char *slash = strrchr(path, '/');

	if (slash != NULL && slash[1] != '\0')
		return slash + 1;
	return path;
}

static void draw_frame(WINDOW *frame, const char *name)
{
	int cols = getmaxx(frame);

	werase(frame);
	box(frame, 0, 0);
	if (cols > 4 && name[0] != '\0') {
		size_t len = strlen(name);
		int max_title = cols - 4;

		if (len > (size_t)max_title)
			len = (size_t)max_title;
		mvwaddnstr(frame, 0, 2, name, (int)len);
	}
	wrefresh(frame);
}

static void draw_text(WINDOW *win, const Text *text, size_t top, size_t col)
{
	int rows, cols;
	size_t width;
	int row;

	getmaxyx(win, rows, cols);
	werase(win);
	width = cols > 1 ? (size_t)(cols - 1) : 0;
	if (width == 0 || rows <= 0) {
		wrefresh(win);
		return;
	}

	for (row = 0; row < rows; row++) {
		size_t index = top + (size_t)row;
		const char *line;
		size_t len;
		size_t avail;
		int n;

		if (index >= text->count)
			break;
		line = text->items[index];
		len = strlen(line);
		if (col >= len)
			continue;
		avail = len - col;
		n = (int)(avail > width ? width : avail);
		mvwaddnstr(win, row, 0, line + col, n);
	}
	wrefresh(win);
}

static size_t page_limit(const Text *text, int rows)
{
	size_t page = rows > 0 ? (size_t)rows : 1;

	if (text->count > page)
		return text->count - page;
	return 0;
}

int main(int argc, char **argv)
{
	Text text = {NULL, 0, 0, 0};
	WINDOW *frame;
	WINDOW *view;
	const char *name;
	size_t top = 0;
	size_t col = 0;
	int running = 1;

	if (argc != 2) {
		fprintf(stderr, "Usage: %s file\n", argv[0]);
		return 1;
	}
	if (load_text(&text, argv[1]) != 0) {
		text_free(&text);
		return 1;
	}

	setlocale(LC_ALL, "");
	initscr();
	cbreak();
	noecho();
	curs_set(0);
	set_escdelay(25);
	refresh();

	frame = newwin(LINES - 2 * MARGIN_Y, COLS - 2 * MARGIN_X,
		       MARGIN_Y, MARGIN_X);
	view = newwin(LINES - 2 * MARGIN_Y - 2, COLS - 2 * MARGIN_X - 2,
		      MARGIN_Y + 1, MARGIN_X + 1);

	keypad(view, TRUE);
	name = file_name(argv[1]);

	while (running) {
		int rows = getmaxy(view);
		size_t limit = page_limit(&text, rows);
		size_t page = rows > 0 ? (size_t)rows : 1;
		int ch;

		if (top > limit)
			top = limit;

		draw_frame(frame, name);
		draw_text(view, &text, top, col);

		ch = wgetch(view);
		switch (ch) {
		case ' ':
		case KEY_DOWN:
			if (top < limit)
				top++;
			break;
		case KEY_UP:
			if (top > 0)
				top--;
			break;
		case KEY_NPAGE:
			if (top < limit) {
				size_t step = page;

				if (step > limit - top)
					step = limit - top;
				top += step;
			}
			break;
		case KEY_PPAGE:
			if (top > page)
				top -= page;
			else
				top = 0;
			break;
		case KEY_RIGHT:
			if (col + 1 < text.max_len)
				col++;
			break;
		case KEY_LEFT:
			if (col > 0)
				col--;
			break;
		case 27:
		case ERR:
			running = 0;
			break;
		default:
			break;
		}
	}

	delwin(view);
	delwin(frame);
	endwin();
	text_free(&text);
	return 0;
}
