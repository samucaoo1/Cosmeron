#pragma once

#include "Status.space"
#include "../Module/Package.h"

PACKAGE(LIB_PREFIX(STATUS_MOD),
    PREDICATE_INLINE(IsSuccess, (OPSTATUS status), {
      return status == STATUS_CONST(SUCCESS);
    }),
    PREDICATE_INLINE(IsFailure, (OPSTATUS status), {
      return status != STATUS_CONST(SUCCESS);
    }),
    PREDICATE_INLINE(IsSpecial, (OPSTATUS status), {
      return status == STATUS_CONST(WOULD_BLOCK) ||
             status == STATUS_CONST(INCOMPLETE_SEQUENCE);
    }),
    FUNCTION(STATUS_TYPE(Category), CategoryOf, (OPSTATUS status)),
    PREDICATE_INLINE(IsCategory,
                     (OPSTATUS status, STATUS_TYPE(Category) category), {
                       return GNS2(LIB_PREFIX(STATUS_MOD), CategoryOf)(status) ==
                              category;
                     }),
    FUNCTION(const char *, CategoryName, (STATUS_TYPE(Category) category)),
    FUNCTION(const char *, Name, (OPSTATUS status)),
    FUNCTION(const char *, Message, (OPSTATUS status))
);

#include "Impl/Status.impl"
/* EOF */
