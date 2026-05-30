#pragma once

#include <deque>
#include <functional>
#include <memory>

#include "market_data_provider/config/config.hpp"
#include "market_data_provider/core/instrument_state_store.hpp"
#include "market_data_provider/core/subscription_manager.hpp"
#include "market_data_provider/normalization/models.hpp"
#include "market_data_provider/observability/metrics.hpp"
#include "market_data_provider/storage/event_store.hpp"
#include "market_data_provider/util/status.hpp"

namespace mdp::core {

class MarketDataService final {
 public:
  explicit MarketDataService(config::ProviderConfig config,
                             std::shared_ptr<storage::EventStore> event_store =
                                 std::make_shared<storage::InMemoryEventStore>());

  util::Status Initialize();
  util::Status Ingest(normalization::MarketEvent event);

  void Subscribe(normalization::SubscriptionRequest request,
                 SubscriptionManager::Callback callback) {
    subscriptions_.Subscribe(std::move(request), std::move(callback));
  }

  [[nodiscard]] const observability::Metrics& metrics() const { return metrics_; }
  [[nodiscard]] const InstrumentStateStore& state_store() const { return state_store_; }
  [[nodiscard]] bool is_ready() const { return ready_; }

 private:
  void DrainQueue();

  config::ProviderConfig config_;
  std::shared_ptr<storage::EventStore> event_store_;
  observability::Metrics metrics_;
  InstrumentStateStore state_store_;
  SubscriptionManager subscriptions_;
  std::deque<normalization::MarketEvent> queue_;
  bool ready_{false};
};

}  // namespace mdp::core
