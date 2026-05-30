#pragma once

#include <functional>

#include "market_data_provider/normalization/models.hpp"
#include "market_data_provider/util/status.hpp"

namespace mdp::feed_adapters {

using EventSink = std::function<util::Status(normalization::MarketEvent)>;

class Adapter {
 public:
  virtual ~Adapter() = default;
  virtual util::Status Start(EventSink sink) = 0;
};

}  // namespace mdp::feed_adapters
