int log_a(void);
int log_b(void);

int main(void) {
  return (log_a() == 1 && log_b() == 2) ? 0 : 1;
}
