#pragma once

#include <string_view>

#include "market_data_provider/normalization/models.hpp"
#include "market_data_provider/util/status.hpp"

namespace mdp::normalization {

class UpstreamTradeParser final {
 public:
  static util::StatusOr<Trade> ParseCsv(std::string_view line, std::string source);
};

}  // namespace mdp::normalization
