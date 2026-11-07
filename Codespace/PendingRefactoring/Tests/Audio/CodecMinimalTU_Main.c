/*
 * Linking both decoder-only translation units must never require
 * -pthread/-lm/-ldl and must not emit duplicate vendor symbols.
 */
int CodecMinimalTU_Decode(void);
int CodecMinimalTU_Math(void);
int main(void) {
  if (CodecMinimalTU_Decode()) return 1;
  if (CodecMinimalTU_Math()) return 2;
  return 0;
}
