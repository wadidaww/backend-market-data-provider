#pragma once

#include <vector>

#include "market_data_provider/feed_adapters/adapter.hpp"

namespace mdp::feed_adapters {

class MockFeedAdapter final : public Adapter {
 public:
  explicit MockFeedAdapter(std::vector<normalization::MarketEvent> scripted_events)
      : events_(std::move(scripted_events)) {}

  util::Status Start(EventSink sink) override;

 private:
  std::vector<normalization::MarketEvent> events_;
};

}  // namespace mdp::feed_adapters
