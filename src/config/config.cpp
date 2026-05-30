#include "market_data_provider/config/config.hpp"

#include <string_view>

namespace mdp::config {

util::Status ConfigValidator::Validate(const ProviderConfig& config) {
  if (config.queue_capacity == 0) {
    return util::Status::Error(util::ErrorCategory::Config, "queue_capacity must be > 0");
  }
  if (config.order_book_depth == 0) {
    return util::Status::Error(util::ErrorCategory::Config, "order_book_depth must be > 0");
  }
  for (const auto& source : config.sources) {
    if (source.name.empty()) {
      return util::Status::Error(util::ErrorCategory::Config, "source.name must not be empty");
    }
    if (source.type.empty()) {
      return util::Status::Error(util::ErrorCategory::Config, "source.type must not be empty");
    }
    if (source.endpoint.empty()) {
      return util::Status::Error(util::ErrorCategory::Config, "source.endpoint must not be empty");
    }
    constexpr std::string_view kMockType = "mock";
    constexpr std::string_view kReplayType = "replay";
    const std::string_view type{source.type};
    if (type != kMockType && type != kReplayType) {
      return util::Status::Error(util::ErrorCategory::Config, "source.type must be mock or replay");
    }
  }
  return util::Status::Ok();
}

}  // namespace mdp::config
