#pragma once

#include <cstddef>
#include <string>
#include <vector>

#include "market_data_provider/util/status.hpp"

namespace mdp::config {

struct SourceConfig {
  std::string name;
  std::string type;
  std::string endpoint;
};

struct ProviderConfig {
  std::size_t queue_capacity{4096};
  std::size_t order_book_depth{50};
  bool replay_mode{false};
  std::vector<SourceConfig> sources;
};

class ConfigValidator final {
 public:
  static util::Status Validate(const ProviderConfig& config);
};

}  // namespace mdp::config
