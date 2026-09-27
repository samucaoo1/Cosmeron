#include "../../Cosmeron/Core/Cast/Cast.space"

typedef struct CAST_TYPE(Value) {
  int value;
} CAST_TYPE(Value);

static int CAST_FUNC(Identity)(int value) { return value; }

static CAST_TYPE(Value) CAST_TYPE_TO_STRUCT(int, Value)(int value) {
  CAST_TYPE(Value) result = {value};
  return result;
}

static int CAST_STRUCT_TO_TYPE(Value, int)(CAST_TYPE(Value) value) {
  return value.value;
}

int main(void) {
  CAST_TYPE(Value) value = CAST_TYPE_TO_STRUCT(int, Value)(7);

  if (CAST_FUNC(Identity)(value.value) != 7)
    return 1;

  return CAST_STRUCT_TO_TYPE(Value, int)(value) == 7 ? 0 : 2;
}
