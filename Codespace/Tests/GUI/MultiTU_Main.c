#include <stddef.h>
size_t gui_context_size_a(void);
size_t gui_context_size_b(void);
int main(void) { return gui_context_size_a() != gui_context_size_b(); }
