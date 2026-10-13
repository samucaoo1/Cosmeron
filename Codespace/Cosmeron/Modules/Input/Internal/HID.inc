#pragma once
/* USB HID Generic Desktop usages, shared by native controller transports. */
static inline bool INPUT_INS(HIDController)(uint32_t page, uint32_t usage) {
  return page == 1 && (usage == 4 || usage == 5 || usage == 8);
}
static inline bool INPUT_INS(HIDAxis)(uint32_t page, uint32_t usage) {
  return page == 1 && usage >= 0x30 && usage <= 0x38;
}
static inline float INPUT_INS(HIDNormalize)(double value, double minimum, double maximum) {
  double result;
  if (maximum <= minimum) return 0;
  result = 2 * (value - minimum) / (maximum - minimum) - 1;
  return result < -1 ? -1.0f : (result > 1 ? 1.0f : (float)result);
}
static inline INPUT_TYPE(Hat) INPUT_INS(HIDHat)(int64_t value, int64_t minimum, int64_t maximum) {
  static const unsigned directions[8] = {1, 9, 8, 10, 2, 6, 4, 5};
  uint64_t index;
  if (value < minimum || value > maximum) return INPUT_CONST(HAT_CENTERED);
  index = (uint64_t)value - (uint64_t)minimum;
  if (maximum - minimum == 3) index *= 2;
  return index < 8 ? (INPUT_TYPE(Hat))directions[index] : INPUT_CONST(HAT_CENTERED);
}

/* Bounded little-endian bit extraction, including unaligned signed fields. */
static inline bool INPUT_INS(HIDBits)(const unsigned char *bytes, size_t length, uint32_t position, unsigned bits, bool sign, int64_t *value) {
  uint64_t raw = 0;
  unsigned i;
  if (!bytes || !value || !bits || bits > 32 || ((uint64_t)position + bits + 7) / 8 > length) return false;
  for (i = 0; i < bits; ++i) {
    uint64_t bit = (uint64_t)position + i;
    if (bytes[bit / 8] & (1u << (bit % 8))) raw |= UINT64_C(1) << i;
  }
  *value = (int64_t)raw;
  if (sign && (raw & (UINT64_C(1) << (bits - 1)))) *value -= INT64_C(1) << bits;
  return true;
}
