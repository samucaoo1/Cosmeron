#pragma once
#include "../Database.space"

#define DATABASE_CONFIG_PROTOTYPE \
  static inline DATABASE_TYPE(TConfig) DATABASE_FUNC(Config)( \
      const char *path)
#define DATABASE_OPEN_PROTOTYPE \
  static inline OPSTATUS DATABASE_FUNC(Open)( \
      DATABASE_TYPE(TDatabase) *database, \
      const char *path)
#define DATABASE_OPEN_WITH_PROTOTYPE \
  static inline OPSTATUS DATABASE_FUNC(OpenWith)( \
      DATABASE_TYPE(TDatabase) *database, \
      const DATABASE_TYPE(TConfig) *config)
#define DATABASE_OPEN_WITH_DIAGNOSTIC_PROTOTYPE \
  static inline OPSTATUS DATABASE_FUNC(OpenWithDiagnostic)( \
      DATABASE_TYPE(TDatabase) *database, \
      const DATABASE_TYPE(TConfig) *config, \
      DATABASE_TYPE(TDiagnostic) *outDiagnostic)
#define DATABASE_CLOSE_PROTOTYPE \
  static inline OPSTATUS DATABASE_FUNC(Close)( \
      DATABASE_TYPE(TDatabase) *database)
#define DATABASE_EXECUTE_PROTOTYPE \
  static inline OPSTATUS DATABASE_FUNC(Execute)( \
      DATABASE_TYPE(TDatabase) *database, \
      const char *sql)
#define DATABASE_EXECUTE_WITH_DIAGNOSTIC_PROTOTYPE \
  static inline OPSTATUS DATABASE_FUNC(ExecuteWithDiagnostic)( \
      DATABASE_TYPE(TDatabase) *database, \
      const char *sql, \
      DATABASE_TYPE(TDiagnostic) *outDiagnostic)
#define DATABASE_PREPARE_PROTOTYPE \
  static inline OPSTATUS DATABASE_FUNC(Prepare)( \
      DATABASE_TYPE(TDatabase) *database, \
      const char *sql, \
      DATABASE_TYPE(TStatement) *statement)
#define DATABASE_PREPARE_WITH_DIAGNOSTIC_PROTOTYPE \
  static inline OPSTATUS DATABASE_FUNC(PrepareWithDiagnostic)( \
      DATABASE_TYPE(TDatabase) *database, \
      const char *sql, \
      DATABASE_TYPE(TStatement) *statement, \
      DATABASE_TYPE(TDiagnostic) *outDiagnostic)
#define DATABASE_AFFECTED_ROWS_PROTOTYPE \
  static inline OPSTATUS DATABASE_FUNC(AffectedRows)( \
      const DATABASE_TYPE(TDatabase) *database, \
      uint64_t *outCount)
#define DATABASE_LAST_INSERT_ID_PROTOTYPE \
  static inline OPSTATUS DATABASE_FUNC(LastInsertId)( \
      const DATABASE_TYPE(TDatabase) *database, \
      int64_t *outId)
#define DATABASE_SET_TIMEOUT_PROTOTYPE \
  static inline OPSTATUS DATABASE_FUNC(SetTimeout)( \
      DATABASE_TYPE(TDatabase) *database, \
      uint32_t timeoutMs)

DATABASE_CONFIG_PROTOTYPE;
DATABASE_OPEN_PROTOTYPE;
DATABASE_OPEN_WITH_PROTOTYPE;
DATABASE_OPEN_WITH_DIAGNOSTIC_PROTOTYPE;
DATABASE_CLOSE_PROTOTYPE;
DATABASE_EXECUTE_PROTOTYPE;
DATABASE_EXECUTE_WITH_DIAGNOSTIC_PROTOTYPE;
DATABASE_PREPARE_PROTOTYPE;
DATABASE_PREPARE_WITH_DIAGNOSTIC_PROTOTYPE;
DATABASE_AFFECTED_ROWS_PROTOTYPE;
DATABASE_LAST_INSERT_ID_PROTOTYPE;
DATABASE_SET_TIMEOUT_PROTOTYPE;

#include "Impl/Connection.impl"
