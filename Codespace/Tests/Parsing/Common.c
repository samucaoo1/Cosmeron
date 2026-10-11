#include "../../Cosmeron/Modules/Parsing/Common/Common.h"
#include <assert.h>
#define SLICE(S) PARSING_FUNC(Slice_Create)(S, sizeof(S)-1)
int main(void) {
  uint64_t u=1; int64_t i=1; double f=1; bool b;
  assert(PARSING_FUNC(Slice_Equal)(SLICE("abc"),SLICE("abc")));
  assert(!PARSING_FUNC(Slice_Equal)(SLICE("abc"),SLICE("ab")));
  assert(PARSING_FUNC(Slice_U64)(SLICE("18446744073709551615"),&u)==STATUS_CONST(SUCCESS) && u==UINT64_MAX);
  assert(PARSING_FUNC(Slice_U64)(SLICE("18446744073709551616"),&u)==STATUS_CONST(OUT_OF_RANGE) && u==UINT64_MAX);
  assert(PARSING_FUNC(Slice_U64)(SLICE("1x"),&u)==STATUS_CONST(INVALID_SEQUENCE));
  assert(PARSING_FUNC(Slice_I64)(SLICE("-9223372036854775808"),&i)==STATUS_CONST(SUCCESS) && i==INT64_MIN);
  assert(PARSING_FUNC(Slice_I64)(SLICE("9223372036854775808"),&i)==STATUS_CONST(OUT_OF_RANGE) && i==INT64_MIN);
  assert(PARSING_FUNC(Slice_I64)(SLICE("+"),&i)==STATUS_CONST(INVALID_SEQUENCE));
  assert(PARSING_FUNC(Slice_F64)(SLICE("-1.25e2"),&f)==STATUS_CONST(SUCCESS) && f==-125);
  assert(PARSING_FUNC(Slice_F64)(SLICE("1e9999"),&f)==STATUS_CONST(OUT_OF_RANGE) && f==-125);
  assert(PARSING_FUNC(Slice_F64)(SLICE("1x"),&f)==STATUS_CONST(INVALID_SEQUENCE));
  assert(PARSING_FUNC(Slice_Bool)(SLICE("true"),&b)==STATUS_CONST(SUCCESS) && b);
  assert(PARSING_FUNC(Slice_Bool)(SLICE("false"),&b)==STATUS_CONST(SUCCESS) && !b);
  assert(PARSING_FUNC(Slice_Bool)(SLICE("yes"),&b)==STATUS_CONST(INVALID_SEQUENCE));
  return 0;
}
