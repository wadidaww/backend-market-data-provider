#include "market_data_provider/feed_adapters/mock_feed_adapter.hpp"

namespace mdp::feed_adapters {

util::Status MockFeedAdapter::Start(EventSink sink) {
  for (const auto& event : events_) {
    const auto status = sink(event);
    if (!status.ok()) {
      return status;
    }
  }
  return util::Status::Ok();
}

}  // namespace mdp::feed_adapters
