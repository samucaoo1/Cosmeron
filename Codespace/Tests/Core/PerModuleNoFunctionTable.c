#define BIT_DISABLE_FUNCTION_TABLE 1
#define COSMERON_DISABLE_FT_DomainA_SharedMod 1
#include "../../Cosmeron/Core/Bit/Bit.h"
#include "../../Cosmeron/Modules/Math/Math.h"

#include <assert.h>
#include <stddef.h>
#include <stdio.h>

/* Intrinsic NO_FUNCTION_TABLE inside MODULE -> PACKAGE and OBJECT */
MODULE(DOMAIN(Core), TestPerMod,
  PACKAGE(NoFtPkg,
    NO_FUNCTION_TABLE,
    FUNCTION_INLINE(int, AddOne, (int x), { return x + 1; })
  ),
  PACKAGE(WithFtPkg,
    FUNCTION_INLINE(int, AddTwo, (int x), { return x + 2; })
  ),
  OBJECT(NoFtObj,
    FIELDS(int value;),
    NO_FUNCTION_TABLE,
    FUNCTION_INLINE(int, Get, (), { return self->value; })
  ),
  OBJECT(WithFtObj,
    FIELDS(int value;),
    FUNCTION_INLINE(int, Get, (), { return self->value; })
  )
);

/* Standalone PACKAGE_DEF and OBJECT_DEF with NO_FUNCTION_TABLE vs normal */
PACKAGE_DEF(StandaloneNoFtPkg,
  NO_FUNCTION_TABLE,
  FUNCTION_INLINE(int, MulTwo, (int x), { return x * 2; })
);

PACKAGE_DEF(StandaloneWithFtPkg,
  FUNCTION_INLINE(int, MulThree, (int x), { return x * 3; })
);

OBJECT_DEF(StandaloneNoFtObj,
  FIELDS(int value;),
  NO_FUNCTION_TABLE,
  FUNCTION_INLINE(int, Read, (), { return self->value; })
);

OBJECT_DEF(StandaloneWithFtObj,
  FIELDS(int value;),
  FUNCTION_INLINE(int, Read, (), { return self->value; })
);

/* Domain-qualified isolation: DomainA_SharedMod disabled, DomainB_SharedMod enabled */
MODULE(DOMAIN(DomainA), SharedMod,
  PACKAGE(PkgA,
    FUNCTION_INLINE(int, Val, (), { return 1; })
  )
);

MODULE(DOMAIN(DomainB), SharedMod,
  PACKAGE(PkgB,
    FUNCTION_INLINE(int, Val, (), { return 2; })
  )
);

int main(void) {
  assert(SharedMod_PkgA_Val() == 1);
  assert(SharedMod_PkgB_Val() == 2);
  assert(SharedMod_PkgB_GetFunctionTable() != NULL);
  /* 1. Dynamic per-module: BIT_DISABLE_FUNCTION_TABLE did NOT leak to Math */
  const Math_Clamp_Int_FunctionTable *mathFt = Math_Clamp_Int_GetFunctionTable();
  assert(mathFt != NULL);
  assert(Bit_8_IsolateLowestSet(0x06U) == 0x02U);

  /* 2. MODULE-level intrinsic NO_FUNCTION_TABLE vs normal */
  assert(TestPerMod_NoFtPkg_AddOne(41) == 42);
  assert(TestPerMod_WithFtPkg_AddTwo(40) == 42);
  assert(TestPerMod_WithFtPkg_GetFunctionTable() != NULL);

  MODULE_TYPE(TestPerMod, NoFtObj) noFtObj;
  MODULE_TYPE(TestPerMod, WithFtObj) withFtObj;
  assert(TestPerMod_NoFtObj_Init(&noFtObj) == STATUS_CONST(SUCCESS));
  assert(TestPerMod_WithFtObj_Init(&withFtObj) == STATUS_CONST(SUCCESS));
  noFtObj.value = 10;
  withFtObj.value = 20;
  assert(TestPerMod_NoFtObj_Get(&noFtObj) == 10);
  assert(TestPerMod_WithFtObj_Get(&withFtObj) == 20);
  assert(sizeof(noFtObj) == sizeof(int));
  assert(sizeof(withFtObj) > sizeof(noFtObj));

  /* 3. Standalone PACKAGE_DEF / OBJECT_DEF with NO_FUNCTION_TABLE vs normal */
  assert(StandaloneNoFtPkg_MulTwo(21) == 42);
  assert(StandaloneWithFtPkg_MulThree(14) == 42);
  assert(StandaloneWithFtPkg_GetFunctionTable() != NULL);

  StandaloneNoFtObj sNoFt;
  StandaloneWithFtObj sWithFt;
  assert(StandaloneNoFtObj_Init(&sNoFt) == STATUS_CONST(SUCCESS));
  assert(StandaloneWithFtObj_Init(&sWithFt) == STATUS_CONST(SUCCESS));
  sNoFt.value = 7;
  sWithFt.value = 9;
  assert(StandaloneNoFtObj_Read(&sNoFt) == 7);
  assert(StandaloneWithFtObj_Read(&sWithFt) == 9);
  assert(sizeof(sNoFt) == sizeof(int));
  assert(sizeof(sWithFt) > sizeof(sNoFt));

  printf("PerModuleNoFunctionTable tests passed!\n");
  return 0;
}
