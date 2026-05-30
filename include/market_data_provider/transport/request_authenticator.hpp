#pragma once

#include <cstdlib>
#include <string_view>

#include "market_data_provider/util/status.hpp"

namespace mdp::transport {

class RequestAuthenticator final {
 public:
  static util::Status Authenticate(const std::string_view provided_password) {
    const char* configured = std::getenv("MDP_ADMIN_PASSWORD");
    if (configured == nullptr || configured[0] == '\0') {
      return util::Status::Error(util::ErrorCategory::Config,
                                 "MDP_ADMIN_PASSWORD environment variable is not configured");
    }
    if (!ConstantTimeEquals(provided_password, std::string_view(configured))) {
      return util::Status::Error(util::ErrorCategory::Unauthorized, "invalid admin password");
    }
    return util::Status::Ok();
  }

 private:
  static bool ConstantTimeEquals(const std::string_view lhs, const std::string_view rhs) {
    if (lhs.size() != rhs.size()) {
      return false;
    }
    unsigned char diff = 0;
    for (std::size_t i = 0; i < lhs.size(); ++i) {
      diff |= static_cast<unsigned char>(lhs[i]) ^ static_cast<unsigned char>(rhs[i]);
    }
    return diff == 0;
  }
};

}  // namespace mdp::transport
