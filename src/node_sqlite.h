#ifndef SRC_NODE_SQLITE_H_
#define SRC_NODE_SQLITE_H_

#if defined(NODE_WANT_INTERNALS) && NODE_WANT_INTERNALS

#include "base_object.h"
#include "node_mem.h"
#include "sqlite3.h"
#include "util.h"

#include <map>
#include <unordered_set>

namespace node {
namespace sqlite {

class DatabaseOpenConfiguration {
 public:
  explicit DatabaseOpenConfiguration(std::string&& location)
      : location_(std::move(location)) {}

  inline const std::string& location() const { return location_; }

  inline bool get_read_only() const { return read_only_; }

  inline void set_read_only(bool flag) { read_only_ = flag; }

  inline bool get_enable_foreign_keys() const { return enable_foreign_keys_; }

  inline void set_enable_foreign_keys(bool flag) {
    enable_foreign_keys_ = flag;
  }

  inline bool get_enable_dqs() const { return enable_dqs_; }

  inline void set_enable_dqs(bool flag) { enable_dqs_ = flag; }

  inline void set_timeout(int timeout) { timeout_ = timeout; }

  inline int get_timeout() { return timeout_; }

  inline void set_use_big_ints(bool flag) { use_big_ints_ = flag; }

  inline bool get_use_big_ints() const { return use_big_ints_; }

  inline void set_return_arrays(bool flag) { return_arrays_ = flag; }

  inline bool get_return_arrays() const { return return_arrays_; }

  inline void set_allow_bare_named_params(bool flag) {
    allow_bare_named_params_ = flag;
  }

  inline bool get_allow_bare_named_params() const {
    return allow_bare_named_params_;
  }

  inline void set_allow_unknown_named_params(bool flag) {
    allow_unknown_named_params_ = flag;
  }

  inline bool get_allow_unknown_named_params() const {
    return allow_unknown_named_params_;
  }

 private:
  std::string location_;
  bool read_only_ = false;
  bool enable_foreign_keys_ = true;
  bool enable_dqs_ = false;
  int timeout_ = 0;
  bool use_big_ints_ = false;
  bool return_arrays_ = false;
  bool allow_bare_named_params_ = true;
  bool allow_unknown_named_params_ = false;
};

class StatementSync;
class BackupJob;

class DatabaseSync : public BaseObject {
 public:
  DatabaseSync(Environment* env,
               v8::Local<v8::Object> object,
               DatabaseOpenConfiguration&& open_config,
               bool open,
               bool allow_load_extension);
  void MemoryInfo(MemoryTracker* tracker) const override;
  static void New(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void Open(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void IsOpenGetter(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void IsTransactionGetter(
      const v8::FunctionCallbackInfo<v8::Value>& args);
  static void Close(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void Dispose(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void Prepare(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void Exec(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void Location(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void CustomFunction(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void AggregateFunction(
      const v8::FunctionCallbackInfo<v8::Value>& args);
  static void CreateSession(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void ApplyChangeset(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void EnableLoadExtension(
      const v8::FunctionCallbackInfo<v8::Value>& args);
  static void LoadExtension(const v8::FunctionCallbackInfo<v8::Value>& args);
  void FinalizeStatements();
  void RemoveBackup(BackupJob* backup);
  void AddBackup(BackupJob* backup);
  void FinalizeBackups();
  void UntrackStatement(StatementSync* statement);
  bool IsOpen();
  bool use_big_ints() const { return open_config_.get_use_big_ints(); }
  bool return_arrays() const { return open_config_.get_return_arrays(); }
  bool allow_bare_named_params() const {
    return open_config_.get_allow_bare_named_params();
  }
  bool allow_unknown_named_params() const {
    return open_config_.get_allow_unknown_named_params();
  }
  sqlite3* Connection();

  // In some situations, such as when using custom functions, it is possible
  // that SQLite reports an error while JavaScript already has a pending
  // exception. In this case, the SQLite error should be ignored. These methods
  // enable that use case.
  void SetIgnoreNextSQLiteError(bool ignore);
  bool ShouldIgnoreSQLiteError();

  SET_MEMORY_INFO_NAME(DatabaseSync)
  SET_SELF_SIZE(DatabaseSync)

 private:
  bool Open();
  void DeleteSessions();

  ~DatabaseSync() override;
  DatabaseOpenConfiguration open_config_;
  bool allow_load_extension_;
  bool enable_load_extension_;
  sqlite3* connection_;
  bool ignore_next_sqlite_error_;

  std::set<BackupJob*> backups_;
  std::set<sqlite3_session*> sessions_;
  std::unordered_set<StatementSync*> statements_;

  friend class Session;
};

class StatementSync : public BaseObject {
 public:
  StatementSync(Environment* env,
                v8::Local<v8::Object> object,
                BaseObjectPtr<DatabaseSync> db,
                sqlite3_stmt* stmt);
  void MemoryInfo(MemoryTracker* tracker) const override;
  static v8::Local<v8::FunctionTemplate> GetConstructorTemplate(
      Environment* env);
  static BaseObjectPtr<StatementSync> Create(Environment* env,
                                             BaseObjectPtr<DatabaseSync> db,
                                             sqlite3_stmt* stmt);
  static void All(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void Iterate(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void Get(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void Run(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void Columns(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void SourceSQLGetter(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void ExpandedSQLGetter(
      const v8::FunctionCallbackInfo<v8::Value>& args);
  static void SetAllowBareNamedParameters(
      const v8::FunctionCallbackInfo<v8::Value>& args);
  static void SetAllowUnknownNamedParameters(
      const v8::FunctionCallbackInfo<v8::Value>& args);
  static void SetReadBigInts(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void SetReturnArrays(const v8::FunctionCallbackInfo<v8::Value>& args);
  void Finalize();
  bool IsFinalized();

  SET_MEMORY_INFO_NAME(StatementSync)
  SET_SELF_SIZE(StatementSync)

 private:
  ~StatementSync() override;
  BaseObjectPtr<DatabaseSync> db_;
  sqlite3_stmt* statement_;
  bool return_arrays_ = false;
  bool use_big_ints_;
  bool allow_bare_named_params_;
  bool allow_unknown_named_params_;
  std::optional<std::map<std::string, std::string>> bare_named_params_;
  bool BindParams(const v8::FunctionCallbackInfo<v8::Value>& args);
  bool BindValue(const v8::Local<v8::Value>& value, const int index);
  v8::MaybeLocal<v8::Value> ColumnToValue(const int column);
  v8::MaybeLocal<v8::Name> ColumnNameToName(const int column);

  friend class StatementSyncIterator;
};

class StatementSyncIterator : public BaseObject {
 public:
  StatementSyncIterator(Environment* env,
                        v8::Local<v8::Object> object,
                        BaseObjectPtr<StatementSync> stmt);
  void MemoryInfo(MemoryTracker* tracker) const override;
  static v8::Local<v8::FunctionTemplate> GetConstructorTemplate(
      Environment* env);
  static BaseObjectPtr<StatementSyncIterator> Create(
      Environment* env, BaseObjectPtr<StatementSync> stmt);
  static void Next(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void Return(const v8::FunctionCallbackInfo<v8::Value>& args);

  SET_MEMORY_INFO_NAME(StatementSyncIterator)
  SET_SELF_SIZE(StatementSyncIterator)

 private:
  ~StatementSyncIterator() override;
  BaseObjectPtr<StatementSync> stmt_;
  bool done_;
};

using Sqlite3ChangesetGenFunc = int (*)(sqlite3_session*, int*, void**);

class Session : public BaseObject {
 public:
  Session(Environment* env,
          v8::Local<v8::Object> object,
          BaseObjectWeakPtr<DatabaseSync> database,
          sqlite3_session* session);
  ~Session() override;
  template <Sqlite3ChangesetGenFunc sqliteChangesetFunc>
  static void Changeset(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void Close(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void Dispose(const v8::FunctionCallbackInfo<v8::Value>& args);
  static v8::Local<v8::FunctionTemplate> GetConstructorTemplate(
      Environment* env);
  static BaseObjectPtr<Session> Create(Environment* env,
                                       BaseObjectWeakPtr<DatabaseSync> database,
                                       sqlite3_session* session);

  void MemoryInfo(MemoryTracker* tracker) const override;
  SET_MEMORY_INFO_NAME(Session)
  SET_SELF_SIZE(Session)

 private:
  void Delete();
  sqlite3_session* session_;
  BaseObjectWeakPtr<DatabaseSync> database_;  // The Parent Database
};

class UserDefinedFunction {
 public:
  UserDefinedFunction(Environment* env,
                      v8::Local<v8::Function> fn,
                      DatabaseSync* db,
                      bool use_bigint_args);
  ~UserDefinedFunction();
  static void xFunc(sqlite3_context* ctx, int argc, sqlite3_value** argv);
  static void xDestroy(void* self);

 private:
  Environment* env_;
  v8::Global<v8::Function> fn_;
  DatabaseSync* db_;
  bool use_bigint_args_;
};

}  // namespace sqlite
}  // namespace node

#endif  // defined(NODE_WANT_INTERNALS) && NODE_WANT_INTERNALS
#endif  // SRC_NODE_SQLITE_H_
