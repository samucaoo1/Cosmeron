#pragma once
#include "../Database.space"

#define DATABASE_STATEMENT_BIND_NULL_PROTOTYPE \
  static inline OPSTATUS DATABASE_FUNC(Statement_BindNull)( \
      DATABASE_TYPE(TStatement) *statement, \
      size_t index)
#define DATABASE_STATEMENT_BIND_INT64_PROTOTYPE \
  static inline OPSTATUS DATABASE_FUNC(Statement_BindInt64)( \
      DATABASE_TYPE(TStatement) *statement, \
      size_t index, \
      int64_t value)
#define DATABASE_STATEMENT_BIND_DOUBLE_PROTOTYPE \
  static inline OPSTATUS DATABASE_FUNC(Statement_BindDouble)( \
      DATABASE_TYPE(TStatement) *statement, \
      size_t index, \
      double value)
#define DATABASE_STATEMENT_BIND_TEXT_PROTOTYPE \
  static inline OPSTATUS DATABASE_FUNC(Statement_BindText)( \
      DATABASE_TYPE(TStatement) *statement, \
      size_t index, \
      const char *text, \
      size_t length)
#define DATABASE_STATEMENT_BIND_BLOB_PROTOTYPE \
  static inline OPSTATUS DATABASE_FUNC(Statement_BindBlob)( \
      DATABASE_TYPE(TStatement) *statement, \
      size_t index, \
      const void *data, \
      size_t size)
#define DATABASE_STATEMENT_NEXT_PROTOTYPE \
  static inline OPSTATUS DATABASE_FUNC(Statement_Next)( \
      DATABASE_TYPE(TStatement) *statement, \
      bool *outHasRow)
#define DATABASE_STATEMENT_NEXT_WITH_DIAGNOSTIC_PROTOTYPE \
  static inline OPSTATUS DATABASE_FUNC(Statement_NextWithDiagnostic)( \
      DATABASE_TYPE(TStatement) *statement, \
      bool *outHasRow, \
      DATABASE_TYPE(TDiagnostic) *outDiagnostic)
#define DATABASE_STATEMENT_EXECUTE_PROTOTYPE \
  static inline OPSTATUS DATABASE_FUNC(Statement_Execute)( \
      DATABASE_TYPE(TStatement) *statement)
#define DATABASE_STATEMENT_EXECUTE_WITH_DIAGNOSTIC_PROTOTYPE \
  static inline OPSTATUS DATABASE_FUNC(Statement_ExecuteWithDiagnostic)( \
      DATABASE_TYPE(TStatement) *statement, \
      DATABASE_TYPE(TDiagnostic) *outDiagnostic)
#define DATABASE_STATEMENT_RESET_PROTOTYPE \
  static inline OPSTATUS DATABASE_FUNC(Statement_Reset)( \
      DATABASE_TYPE(TStatement) *statement)
#define DATABASE_STATEMENT_CLEAR_BINDINGS_PROTOTYPE \
  static inline OPSTATUS DATABASE_FUNC(Statement_ClearBindings)( \
      DATABASE_TYPE(TStatement) *statement)
#define DATABASE_STATEMENT_CLOSE_PROTOTYPE \
  static inline OPSTATUS DATABASE_FUNC(Statement_Close)( \
      DATABASE_TYPE(TStatement) *statement)
#define DATABASE_STATEMENT_PARAMETER_COUNT_PROTOTYPE \
  static inline size_t DATABASE_FUNC(Statement_ParameterCount)( \
      const DATABASE_TYPE(TStatement) *statement)
#define DATABASE_STATEMENT_COLUMN_COUNT_PROTOTYPE \
  static inline size_t DATABASE_FUNC(Statement_ColumnCount)( \
      const DATABASE_TYPE(TStatement) *statement)
#define DATABASE_STATEMENT_COLUMN_NAME_PROTOTYPE \
  static inline OPSTATUS DATABASE_FUNC(Statement_ColumnName)( \
      const DATABASE_TYPE(TStatement) *statement, \
      size_t column, \
      const char **outName)
#define DATABASE_STATEMENT_COLUMN_KIND_PROTOTYPE \
  static inline OPSTATUS DATABASE_FUNC(Statement_ColumnKind)( \
      const DATABASE_TYPE(TStatement) *statement, \
      size_t column, \
      DATABASE_TYPE(ColumnKind) *outKind)
#define DATABASE_STATEMENT_COLUMN_IS_NULL_PROTOTYPE \
  static inline OPSTATUS DATABASE_FUNC(Statement_ColumnIsNull)( \
      const DATABASE_TYPE(TStatement) *statement, \
      size_t column, \
      bool *outIsNull)
#define DATABASE_STATEMENT_COLUMN_INT64_PROTOTYPE \
  static inline OPSTATUS DATABASE_FUNC(Statement_ColumnInt64)( \
      const DATABASE_TYPE(TStatement) *statement, \
      size_t column, \
      int64_t *outValue)
#define DATABASE_STATEMENT_COLUMN_DOUBLE_PROTOTYPE \
  static inline OPSTATUS DATABASE_FUNC(Statement_ColumnDouble)( \
      const DATABASE_TYPE(TStatement) *statement, \
      size_t column, \
      double *outValue)
#define DATABASE_STATEMENT_COLUMN_TEXT_PROTOTYPE \
  static inline OPSTATUS DATABASE_FUNC(Statement_ColumnText)( \
      DATABASE_TYPE(TStatement) *statement, \
      size_t column, \
      const char **outText, \
      size_t *outLength)
#define DATABASE_STATEMENT_COLUMN_BLOB_PROTOTYPE \
  static inline OPSTATUS DATABASE_FUNC(Statement_ColumnBlob)( \
      DATABASE_TYPE(TStatement) *statement, \
      size_t column, \
      const void **outData, \
      size_t *outSize)

DATABASE_STATEMENT_BIND_NULL_PROTOTYPE;
DATABASE_STATEMENT_BIND_INT64_PROTOTYPE;
DATABASE_STATEMENT_BIND_DOUBLE_PROTOTYPE;
DATABASE_STATEMENT_BIND_TEXT_PROTOTYPE;
DATABASE_STATEMENT_BIND_BLOB_PROTOTYPE;
DATABASE_STATEMENT_NEXT_PROTOTYPE;
DATABASE_STATEMENT_NEXT_WITH_DIAGNOSTIC_PROTOTYPE;
DATABASE_STATEMENT_EXECUTE_PROTOTYPE;
DATABASE_STATEMENT_EXECUTE_WITH_DIAGNOSTIC_PROTOTYPE;
DATABASE_STATEMENT_RESET_PROTOTYPE;
DATABASE_STATEMENT_CLEAR_BINDINGS_PROTOTYPE;
DATABASE_STATEMENT_CLOSE_PROTOTYPE;
DATABASE_STATEMENT_PARAMETER_COUNT_PROTOTYPE;
DATABASE_STATEMENT_COLUMN_COUNT_PROTOTYPE;
DATABASE_STATEMENT_COLUMN_NAME_PROTOTYPE;
DATABASE_STATEMENT_COLUMN_KIND_PROTOTYPE;
DATABASE_STATEMENT_COLUMN_IS_NULL_PROTOTYPE;
DATABASE_STATEMENT_COLUMN_INT64_PROTOTYPE;
DATABASE_STATEMENT_COLUMN_DOUBLE_PROTOTYPE;
DATABASE_STATEMENT_COLUMN_TEXT_PROTOTYPE;
DATABASE_STATEMENT_COLUMN_BLOB_PROTOTYPE;

#include "Impl/Statement.impl"
