#include "market_data_provider/core/market_data_service.hpp"

namespace mdp::core {

MarketDataService::MarketDataService(config::ProviderConfig config,
                                     std::shared_ptr<storage::EventStore> event_store)
    : config_(std::move(config)),
      event_store_(std::move(event_store)),
      state_store_(config_.order_book_depth) {
  queue_.clear();
}

util::Status MarketDataService::Initialize() {
  const auto status = config::ConfigValidator::Validate(config_);
  if (!status.ok()) {
    return status;
  }
  ready_ = true;
  return util::Status::Ok();
}

util::Status MarketDataService::Ingest(normalization::MarketEvent event) {
  if (!ready_) {
    return util::Status::Error(util::ErrorCategory::Internal, "service is not initialized");
  }

  if (queue_.size() >= config_.queue_capacity) {
    metrics_.dropped_events.fetch_add(1, std::memory_order_relaxed);
    return util::Status::Error(util::ErrorCategory::QueueFull, "ingest queue is full");
  }

  metrics_.ingested_events.fetch_add(1, std::memory_order_relaxed);
  event_store_->AppendRaw(event);
  queue_.push_back(std::move(event));
  metrics_.queue_depth.store(queue_.size(), std::memory_order_relaxed);

  DrainQueue();
  metrics_.queue_depth.store(queue_.size(), std::memory_order_relaxed);
  return util::Status::Ok();
}

void MarketDataService::DrainQueue() {
  while (!queue_.empty()) {
    auto event = std::move(queue_.front());
    queue_.pop_front();

    const auto status = state_store_.Apply(event);
    if (!status.ok()) {
      if (status.category() == util::ErrorCategory::SequenceGap ||
          status.category() == util::ErrorCategory::Parse) {
        metrics_.parse_errors.fetch_add(1, std::memory_order_relaxed);
      }
      continue;
    }

    event_store_->AppendNormalized(event);
    subscriptions_.Publish(event);
  }
}

}  // namespace mdp::core
