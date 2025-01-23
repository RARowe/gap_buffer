#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

#define u32 uint32_t
#define i32 int32_t

typedef struct {
	u32 start, end, len, cap;
	char* buffer;
} GapBuffer;

GapBuffer gb_new(void* mem, u32 mem_size) {
	return (GapBuffer) {
		.start = 0,
		.end = mem_size - 1,
		.len = 0,
		.cap = mem_size,
		.buffer = (char*)mem
	};
}

void gb_insert(GapBuffer* gb, u32 col, char c) {
	if (gb->len >= gb->cap ||
			col > gb->len ||
			col < 0) {
		return;
	}

	char *b = gb->buffer;
	if (col <= gb->start) {
		u32 swap_count = gb->start - col;
		for (u32 i = 0; i < swap_count; i++) {
			b[gb->end] = b[gb->start - 1];
			gb->start -= 1;
			gb->end -= 1;
		}
	} else {
		u32 swap_count = col - gb->start;
		for (u32 i = 0; i < swap_count; i++) {
			b[gb->start] = b[gb->end + 1];
			gb->start += 1;
			gb->end += 1;
		}
	}

	b[gb->start] = c;
	gb->start += 1;
	gb->len += 1;
}

void gb_remove(GapBuffer* gb, u32 col) {
	if (gb->len == 0 || col > gb->len || col < 0) {
		return;
	}

	char *b = gb->buffer;
	if (col < gb->start) {
		u32 swap_count = gb->start - col;
		for (u32 i = 1; i < swap_count; i++) {
			b[gb->end] = b[gb->start - 1];
			gb->start -= 1;
			gb->end -= 1;
		}
		gb->start -= 1;
	} else {
		u32 swap_count = col - gb->start;
		for (u32 i = 0; i < swap_count; i++) {
			b[gb->start] = b[gb->end + 1];
			gb->start += 1;
			gb->end += 1;
		}
		gb->end += 1;
	}
}

size_t gb_cpy(GapBuffer* gb, char* dst, size_t n) {
	if (n < gb->cap) {
		return 0;
	}

	size_t dst_i = 0;
	for (size_t i = 0; i < gb->cap; i++) {
		if (i < gb->start || i > gb->end) {
			dst[dst_i] = gb->buffer[i];
			dst_i += 1;
		} else {
			i = gb->end;
		}
	}

	dst[dst_i] = '\0';

	return dst_i;
}

int main() {
	void* mem = malloc(1024);
	GapBuffer b = gb_new(mem, 1024);

	for (size_t i = 0; i < 1024; i++) {
		gb_insert(&b, i, i % 128);
	}
	for (size_t i = 0; i < 1023; i++) {
		gb_remove(&b, 0);
	}

	char* out = (char*)malloc(1024);
	write(1, out, gb_cpy(&b, out, 1024));
	return 0;
}
