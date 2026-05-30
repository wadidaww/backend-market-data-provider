#include "market_data_provider/feed_adapters/replay_feed_adapter.hpp"

namespace mdp::feed_adapters {

util::Status ReplayFeedAdapter::Start(EventSink sink) {
  for (const auto& event : events_) {
    const auto status = sink(event);
    if (!status.ok()) {
      return status;
    }
    if (pacing_.count() > 0) {
      std::this_thread::sleep_for(pacing_);
    }
  }
  return util::Status::Ok();
}

}  // namespace mdp::feed_adapters
