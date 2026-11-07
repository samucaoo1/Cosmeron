#pragma once
#include "../Connection/Connection.h"

#define DATABASE_TRANSACTION_BEGIN_PROTOTYPE \
  static inline OPSTATUS DATABASE_FUNC(Transaction_Begin)( \
      DATABASE_TYPE(TDatabase) *database)
#define DATABASE_TRANSACTION_COMMIT_PROTOTYPE \
  static inline OPSTATUS DATABASE_FUNC(Transaction_Commit)( \
      DATABASE_TYPE(TDatabase) *database)
#define DATABASE_TRANSACTION_ROLLBACK_PROTOTYPE \
  static inline OPSTATUS DATABASE_FUNC(Transaction_Rollback)( \
      DATABASE_TYPE(TDatabase) *database)
#define DATABASE_TRANSACTION_SAVEPOINT_PROTOTYPE \
  static inline OPSTATUS DATABASE_FUNC(Transaction_Savepoint)( \
      DATABASE_TYPE(TDatabase) *database, \
      const char *name)
#define DATABASE_TRANSACTION_ROLLBACK_TO_PROTOTYPE \
  static inline OPSTATUS DATABASE_FUNC(Transaction_RollbackTo)( \
      DATABASE_TYPE(TDatabase) *database, \
      const char *name)
#define DATABASE_TRANSACTION_RELEASE_PROTOTYPE \
  static inline OPSTATUS DATABASE_FUNC(Transaction_Release)( \
      DATABASE_TYPE(TDatabase) *database, \
      const char *name)
#define DATABASE_TRANSACTION_IS_ACTIVE_PROTOTYPE \
  static inline bool DATABASE_FUNC(Transaction_IsActive)( \
      const DATABASE_TYPE(TDatabase) *database)

DATABASE_TRANSACTION_BEGIN_PROTOTYPE;
DATABASE_TRANSACTION_COMMIT_PROTOTYPE;
DATABASE_TRANSACTION_ROLLBACK_PROTOTYPE;
DATABASE_TRANSACTION_SAVEPOINT_PROTOTYPE;
DATABASE_TRANSACTION_ROLLBACK_TO_PROTOTYPE;
DATABASE_TRANSACTION_RELEASE_PROTOTYPE;
DATABASE_TRANSACTION_IS_ACTIVE_PROTOTYPE;

#include "Impl/Transaction.impl"
