#pragma once

#include <string>

namespace mdp::util {

enum class ErrorCategory {
  None,
  Config,
  Unauthorized,
  Parse,
  SequenceGap,
  QueueFull,
  NotFound,
  Internal,
};

class Status {
 public:
  static Status Ok() { return Status{}; }

  static Status Error(ErrorCategory category, std::string message) {
    return Status(category, std::move(message));
  }

  [[nodiscard]] bool ok() const { return category_ == ErrorCategory::None; }
  [[nodiscard]] ErrorCategory category() const { return category_; }
  [[nodiscard]] const std::string& message() const { return message_; }

 private:
  Status() = default;

  Status(ErrorCategory category, std::string message)
      : category_(category), message_(std::move(message)) {}

  ErrorCategory category_{ErrorCategory::None};
  std::string message_;
};

template <typename T>
class StatusOr {
 public:
  StatusOr(const T& value) : value_(value), status_(Status::Ok()), has_value_(true) {}
  StatusOr(T&& value) : value_(std::move(value)), status_(Status::Ok()), has_value_(true) {}
  StatusOr(Status status) : status_(std::move(status)), has_value_(false) {}

  [[nodiscard]] bool ok() const { return has_value_ && status_.ok(); }
  [[nodiscard]] const Status& status() const { return status_; }
  [[nodiscard]] const T& value() const { return value_; }
  [[nodiscard]] T& value() { return value_; }

 private:
  T value_{};
  Status status_;
  bool has_value_{false};
};

}  // namespace mdp::util
