#pragma once

#include <cstdlib>
#include <string_view>

#include "market_data_provider/util/status.hpp"

namespace mdp::transport {

class RequestAuthenticator final {
 public:
  static constexpr std::size_t kMaxPasswordBytes = 128;

  static util::Status Authenticate(const std::string_view provided_password) {
    const char* configured = std::getenv("MDP_ADMIN_PASSWORD");
    if (configured == nullptr || configured[0] == '\0') {
      return util::Status::Error(util::ErrorCategory::Config,
                                 "MDP_ADMIN_PASSWORD environment variable is not configured");
    }
    const std::string_view configured_password(configured);
    if (configured_password.size() > kMaxPasswordBytes) {
      return util::Status::Error(util::ErrorCategory::Config,
                                 "MDP_ADMIN_PASSWORD exceeds maximum supported length");
    }
    if (!ConstantTimeEquals(provided_password, configured_password)) {
      return util::Status::Error(util::ErrorCategory::Unauthorized, "invalid admin password");
    }
    return util::Status::Ok();
  }

 private:
  static bool ConstantTimeEquals(const std::string_view lhs, const std::string_view rhs) {
    unsigned char diff = 0U;
    for (std::size_t i = 0; i < kMaxPasswordBytes; ++i) {
      const unsigned char lhs_char = i < lhs.size() ? static_cast<unsigned char>(lhs[i]) : 0U;
      const unsigned char rhs_char = i < rhs.size() ? static_cast<unsigned char>(rhs[i]) : 0U;
      diff |= lhs_char ^ rhs_char;
    }
    diff |= static_cast<unsigned char>(lhs.size() ^ rhs.size());
    return diff == 0;
  }
};

}  // namespace mdp::transport
