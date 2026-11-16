#include "../../../Cosmeron/Core/Module/Module.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

/* ============================================================
 * 1. Enum Definition with Row-Tuple Table Pattern
 * ============================================================ */

#define SEEK_TABLE(X)                                                          \
  X((SET, 0, "Seek from beginning"))                                           \
  X((CUR, 1, "Seek from current position"))                                    \
  X((END, 2, "Seek from end"))

/* ============================================================
 * 2. Next-Gen Declarative Module Declaration
 * ============================================================ */

MODULE(DOMAIN(Core), StreamBuffer,
    ENUM(SeekOrigin, SEEK_TABLE),

    OBJECT(Buffer,
        FIELDS(
            char data[64];
            size_t size;
            size_t capacity;
        ),
        OPERATION(Clear, ()),
        OPERATION(Write, (const char *src, size_t count)),
        CONST_FUNCTION(size_t, Length, ()),
        CONST_PREDICATE(IsEmpty, ()),
        CONST_COMPARISON(Compare, (const MODULE_TYPE(StreamBuffer, Buffer) *other))
    ),

    OBJECT(AttributedBuffer, EXTENDS(Buffer),
        FIELDS(
            uint32_t flags;
        ),
        OPERATION(SetFlags, (uint32_t flagsMask))
    ),

    PACKAGE(Utils,
        FUNCTION(size_t, StrLen, (const char *str)),
        PREDICATE(IsStrEmpty, (const char *str)),

        VARIANT(Alloc,
            FUNCTION(void *, (size_t size)),
            FUNCTION(void *, AndFill, (size_t size, char fillChar))
        ),

        SPACE(Transform,
            PROCEDURE(ToUpper, (char *dest, const char *src))
        )
    )
);

/* Standalone module without DOMAIN(...) */
MODULE(MathOps,
    PACKAGE(Arithmetic,
        FUNCTION(int, Add, (int a, int b))
    )
);

/* Container Module with Declarative Template Object */
MODULE(DOMAIN(Container), Container,
    TEMPLATE_OBJECT(Vector, (SUFFIX, TYPE),
        FIELDS(
            TYPE items[8];
            size_t count;
        ),
        OPERATION(Push, (TYPE item)),
        OPERATION(Pop, (TYPE *outItem)),
        CONST_FUNCTION(size_t, Count, ()),
        CONST_PREDICATE(IsEmpty, ())
    )
);

/* 1. Template Object Blueprint: Vector */
#define TEMPLATE_Vector(SUFFIX, TYPE)                                          \
  TEMPLATE_OBJECT(Container, Vector, SUFFIX,                                   \
      FIELDS(                                                                  \
          TYPE items[8];                                                       \
          size_t count;                                                        \
      ),                                                                       \
      OPERATION(Push, (TYPE item)),                                            \
      OPERATION(Pop, (TYPE *outItem)),                                         \
      CONST_FUNCTION(size_t, Count, ()),                                       \
      CONST_PREDICATE(IsEmpty, ())                                             \
  )

#define TEMPLATE_Vector_IMPLEMENT(SUFFIX, TYPE)                                \
  OBJECT_OPERATION_IMPLEMENT_EX(                                               \
      LIB_PREFIX(NS(Container, Vector, SUFFIX)), Push, (TYPE item)) {         \
    if (!self)                                                                 \
      return STATUS(INVALID_ARGUMENT);                                         \
    if (self->count >= 8U)                                                     \
      return STATUS(OUT_OF_RANGE);                                             \
    self->items[self->count++] = item;                                         \
    return STATUS(SUCCESS);                                                    \
  }                                                                            \
  OBJECT_OPERATION_IMPLEMENT_EX(                                               \
      LIB_PREFIX(NS(Container, Vector, SUFFIX)), Pop, (TYPE *outItem)) {       \
    if (!self || !outItem)                                                     \
      return STATUS(INVALID_ARGUMENT);                                         \
    if (self->count == 0U)                                                     \
      return STATUS(OUT_OF_RANGE);                                             \
    *outItem = self->items[--self->count];                                     \
    return STATUS(SUCCESS);                                                    \
  }                                                                            \
  OBJECT_FUNCTION_IMPLEMENT_EX(                                                \
      LIB_PREFIX(NS(Container, Vector, SUFFIX)), size_t, Count, ()) {          \
    if (!self)                                                                 \
      return 0U;                                                               \
    return self->count;                                                        \
  }                                                                            \
  OBJECT_PREDICATE_IMPLEMENT_EX(                                               \
      LIB_PREFIX(NS(Container, Vector, SUFFIX)), IsEmpty, ()) {                \
    if (!self)                                                                 \
      return true;                                                             \
    return self->count == 0U;                                                  \
  }

/* Forward declaration and instantiation of Vector_int */
FORWARD_TEMPLATE_OBJECT(Container, Vector, int);
TEMPLATE_INSTANTIATE(Vector, int, int);

/* 2. Template Package Blueprint: BitOps (Inline-Only) */
#define TEMPLATE_BitOps(SUFFIX, TYPE)                                          \
  TEMPLATE_PACKAGE(Bit, BitOps, SUFFIX,                                        \
      OPERATION_INLINE(Set, (TYPE *value, uint8_t bit), {                      \
        if (!value)                                                            \
          return STATUS(INVALID_ARGUMENT);                                     \
        if (bit >= (uint8_t)(sizeof(TYPE) * 8U))                               \
          return STATUS(OUT_OF_RANGE);                                         \
        *value = (TYPE)(*value | (TYPE)((TYPE)1 << bit));                      \
        return STATUS(SUCCESS);                                                \
      }),                                                                      \
      PREDICATE_INLINE(Check, (TYPE value, uint8_t bit), {                     \
        if (bit >= (uint8_t)(sizeof(TYPE) * 8U))                               \
          return false;                                                        \
        return (value & ((TYPE)1 << bit)) != 0;                                \
      })                                                                       \
  )
#define TEMPLATE_BitOps_IMPLEMENT(SUFFIX, TYPE) /* empty / inline-only */

/* Table-Driven Batch Instantiation for BitOps */
#define BIT_TEST_TABLE(X) \
  X((u16, uint16_t)) \
  X((u32, uint32_t))

TEMPLATE_INSTANTIATE_TABLE(BitOps, BIT_TEST_TABLE);

/* ============================================================
 * 3. Scoped Implementations (.impl simulation)
 * ============================================================ */

/* --- Object: Buffer --- */
MODULE_SCOPE(StreamBuffer, Buffer)

OBJECT_OPERATION_IMPLEMENT_EX(LIB_PREFIX(StreamBuffer_Buffer), Clear, ()) {
  if (!self)
    return STATUS(INVALID_ARGUMENT);
  self->size = 0U;
  self->data[0] = '\0';
  return STATUS(SUCCESS);
}

OBJECT_OPERATION_IMPLEMENT_EX(LIB_PREFIX(StreamBuffer_Buffer), Write,
                              (const char *src, size_t count)) {
  if (!self || !src)
    return STATUS(INVALID_ARGUMENT);
  if (count >= sizeof(self->data))
    return STATUS(OUT_OF_RANGE);
  memcpy(self->data, src, count);
  self->data[count] = '\0';
  self->size = count;
  return STATUS(SUCCESS);
}

OBJECT_FUNCTION_IMPLEMENT_EX(LIB_PREFIX(StreamBuffer_Buffer), size_t, Length, ()) {
  if (!self)
    return 0U;
  return self->size;
}

OBJECT_PREDICATE_IMPLEMENT_EX(LIB_PREFIX(StreamBuffer_Buffer), IsEmpty, ()) {
  if (!self)
    return true;
  return self->size == 0U;
}

OBJECT_COMPARISON_IMPLEMENT_EX(
    LIB_PREFIX(StreamBuffer_Buffer), Compare,
    (const MODULE_TYPE(StreamBuffer, Buffer) *other)) {
  if (!self || !other)
    return COMPARISON_CONST(LOWER);
  if (self->size < other->size)
    return COMPARISON_CONST(LOWER);
  if (self->size > other->size)
    return COMPARISON_CONST(HIGHER);
  return COMPARISON_CONST(EQUAL);
}

MODULE_SCOPE_END()

/* --- Object: AttributedBuffer --- */
OBJECT_SCOPE(StreamBuffer, AttributedBuffer)

OBJECT_OPERATION_IMPLEMENT_EX(LIB_PREFIX(StreamBuffer_AttributedBuffer),
                              SetFlags, (uint32_t flagsMask)) {
  if (!self)
    return STATUS(INVALID_ARGUMENT);
  self->flags |= flagsMask;
  return STATUS(SUCCESS);
}

OBJECT_SCOPE(END)

/* --- Package: Utils --- */
PACKAGE_SCOPE(StreamBuffer, Utils)

PACKAGE_FUNCTION_IMPLEMENT_EX(LIB_PREFIX(StreamBuffer_Utils), size_t, StrLen,
                              (const char *str)) {
  if (!str)
    return 0U;
  return strlen(str);
}

PACKAGE_PREDICATE_IMPLEMENT_EX(LIB_PREFIX(StreamBuffer_Utils), IsStrEmpty,
                              (const char *str)) {
  if (!str)
    return true;
  return str[0] == '\0';
}

/* Static buffer for mock allocations */
static char _mock_heap[256];

VARIANT_SCOPE(Alloc)

PACKAGE_FUNCTION_IMPLEMENT_EX(LIB_PREFIX(StreamBuffer_Utils), void *, Alloc,
                              (size_t size)) {
  if (size >= sizeof(_mock_heap))
    return NULL;
  return _mock_heap;
}

PACKAGE_FUNCTION_IMPLEMENT_EX(LIB_PREFIX(StreamBuffer_Utils), void *,
                              AllocAndFill, (size_t size, char fillChar)) {
  if (size >= sizeof(_mock_heap))
    return NULL;
  memset(_mock_heap, (int)fillChar, size);
  return _mock_heap;
}

VARIANT_SCOPE_END()

SPACE_SCOPE(Transform)

PACKAGE_PROCEDURE_IMPLEMENT_EX(LIB_PREFIX(StreamBuffer_Utils_Transform),
                              ToUpper, (char *dest, const char *src)) {
  if (!dest || !src)
    return;
  for (size_t i = 0; src[i] != '\0'; ++i) {
    char c = src[i];
    if (c >= 'a' && c <= 'z')
      c = (char)(c - ('a' - 'A'));
    dest[i] = c;
  }
}

SPACE_SCOPE_END()

PACKAGE_SCOPE_END()

/* --- Package: MathOps_Arithmetic (Standalone Module) --- */
PACKAGE_SCOPE(MathOps, Arithmetic)

PACKAGE_FUNCTION_IMPLEMENT_EX(LIB_PREFIX(MathOps_Arithmetic), int, Add,
                              (int a, int b)) {
  return a + b;
}

PACKAGE_SCOPE_END()

/* ============================================================
 * 4. Test Suite Execution
 * ============================================================ */

int main(void) {
  printf("[Testing Next-Gen Declarative Module Engine]\n");

  /* 1. Enum verification */
  MODULE_ENUM(StreamBuffer, SeekOrigin) origin = MODULE_CONST(StreamBuffer, SET);
  assert(origin == 0);
  assert(MODULE_CONST(StreamBuffer, CUR) == 1);
  assert(MODULE_CONST(StreamBuffer, END) == 2);

  /* 2. Object verification with MODULE_TYPE and MODULE_CALL */
  MODULE_TYPE(StreamBuffer, Buffer) buf;
  OPSTATUS status = MODULE_CALL(StreamBuffer, Buffer, Init, (&buf));
  assert(status == STATUS(SUCCESS));

  status = MODULE_CALL(StreamBuffer, Buffer, Clear, (&buf));
  assert(status == STATUS(SUCCESS));
  assert(MODULE_CALL(StreamBuffer, Buffer, IsEmpty, (&buf)) == true);
  assert(MODULE_CALL(StreamBuffer, Buffer, Length, (&buf)) == 0U);

  status = MODULE_CALL(StreamBuffer, Buffer, Write, (&buf, "Cosmeron", 8));
  assert(status == STATUS(SUCCESS));
  assert(MODULE_CALL(StreamBuffer, Buffer, Length, (&buf)) == 8U);
  assert(MODULE_CALL(StreamBuffer, Buffer, IsEmpty, (&buf)) == false);

  MODULE_TYPE(StreamBuffer, Buffer) otherBuf;
  status = MODULE_CALL(StreamBuffer, Buffer, Init, (&otherBuf));
  assert(status == STATUS(SUCCESS));
  status = MODULE_CALL(StreamBuffer, Buffer, Write, (&otherBuf, "Cos", 3));
  assert(status == STATUS(SUCCESS));

  CMPOUT cmp = MODULE_CALL(StreamBuffer, Buffer, Compare, (&buf, &otherBuf));
  assert(cmp == COMPARISON_CONST(HIGHER));

  /* 3. Inheritance verification */
  MODULE_TYPE(StreamBuffer, AttributedBuffer) attrBuf;
  status = MODULE_CALL(StreamBuffer, AttributedBuffer, Init, (&attrBuf));
  assert(status == STATUS(SUCCESS));
  attrBuf.flags = 0U;

  status = MODULE_CALL(StreamBuffer, AttributedBuffer, SetFlags, (&attrBuf, 0x01U));
  assert(status == STATUS(SUCCESS));
  assert(attrBuf.flags == 0x01U);

  /* 4. Package free functions */
  size_t len = MODULE_CALL(StreamBuffer, Utils, StrLen, ("Architecture"));
  assert(len == 12U);
  assert(MODULE_CALL(StreamBuffer, Utils, IsStrEmpty, ("")) == true);
  assert(MODULE_CALL(StreamBuffer, Utils, IsStrEmpty, ("Text")) == false);

  /* 5. VARIANT flat family invocations */
  void *mem1 = MODULE_CALL(StreamBuffer, Utils, Alloc, (32));
  assert(mem1 != NULL);

  char *mem2 = (char *)MODULE_CALL(StreamBuffer, Utils, AllocAndFill, (16, 'Z'));
  assert(mem2 != NULL);
  assert(mem2[0] == 'Z');
  assert(mem2[15] == 'Z');

  /* 6. SPACE nested sub-namespace invocations (5 levels depth) */
  char dest[16] = {0};
  MODULE_CALL(StreamBuffer, Utils, Transform, ToUpper, (dest, "cosmeron"));
  assert(strcmp(dest, "COSMERON") == 0);

  /* 7. Standalone module without DOMAIN(...) verification */
  int sum = MODULE_CALL(MathOps, Arithmetic, Add, (30, 12));
  assert(sum == 42);

  /* 8. Templated Object (Vector_int) verification */
  MODULE_TYPED(Container, Vector, int) vec = {0};
  status = MODULE_CALL(Container, Vector_int, Init, (&vec));
  assert(status == STATUS(SUCCESS));
  assert(MODULE_CALL(Container, Vector_int, IsEmpty, (&vec)) == true);
  assert(MODULE_CALL(Container, Vector_int, Count, (&vec)) == 0U);

  status = MODULE_CALL(Container, Vector_int, Push, (&vec, 123));
  assert(status == STATUS(SUCCESS));
  assert(MODULE_CALL(Container, Vector, int, Count, (&vec)) == 1U);
  assert(MODULE_CALL(Container, Vector_int, IsEmpty, (&vec)) == false);

  status = MODULE_CALL(Container, Vector_int, Push, (&vec, 456));
  assert(status == STATUS(SUCCESS));
  assert(MODULE_CALL(Container, Vector, int, Count, (&vec)) == 2U);

  int popped = 0;
  status = MODULE_CALL(Container, Vector_int, Pop, (&vec, &popped));
  assert(status == STATUS(SUCCESS));
  assert(popped == 456);
  assert(MODULE_CALL(Container, Vector, int, Count, (&vec)) == 1U);

  status = MODULE_CALL(Container, Vector_int, Pop, (&vec, &popped));
  assert(status == STATUS(SUCCESS));
  assert(popped == 123);
  assert(MODULE_CALL(Container, Vector_int, Count, (&vec)) == 0U);
  assert(MODULE_CALL(Container, Vector_int, IsEmpty, (&vec)) == true);

  /* 9. Templated Package instantiated via Table (BitOps) */
  uint32_t val32 = 0U;
  status = MODULE_CALL(Bit, BitOps_u32, Set, (&val32, 4));
  assert(status == STATUS(SUCCESS));
  assert(MODULE_CALL(Bit, BitOps_u32, Check, (val32, 4)) == true);
  assert(MODULE_CALL(Bit, BitOps, u32, Check, (val32, 5)) == false);

  uint16_t val16 = 0U;
  status = MODULE_CALL(Bit, BitOps_u16, Set, (&val16, 2));
  assert(status == STATUS(SUCCESS));
  assert(MODULE_CALL(Bit, BitOps_u16, Check, (val16, 2)) == true);
  assert(MODULE_CALL(Bit, BitOps, u16, Check, (val16, 3)) == false);

  printf("Next-Gen Declarative Module Engine tests passed successfully!\n");
  return 0;
}
