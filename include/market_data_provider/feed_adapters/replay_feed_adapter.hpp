#pragma once

#include <chrono>
#include <thread>
#include <vector>

#include "market_data_provider/feed_adapters/adapter.hpp"

namespace mdp::feed_adapters {

class ReplayFeedAdapter final : public Adapter {
 public:
  explicit ReplayFeedAdapter(std::vector<normalization::MarketEvent> recorded_events,
                             std::chrono::milliseconds pacing = std::chrono::milliseconds(0))
      : events_(std::move(recorded_events)), pacing_(pacing) {}

  util::Status Start(EventSink sink) override;

 private:
  std::vector<normalization::MarketEvent> events_;
  std::chrono::milliseconds pacing_;
};

}  // namespace mdp::feed_adapters
