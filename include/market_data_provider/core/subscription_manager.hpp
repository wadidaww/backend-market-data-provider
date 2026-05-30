#pragma once

#include <functional>
#include <mutex>
#include <vector>

#include "market_data_provider/normalization/models.hpp"

namespace mdp::core {

class SubscriptionManager final {
 public:
  using Callback = std::function<void(const normalization::MarketEvent&)>;

  void Subscribe(normalization::SubscriptionRequest request, Callback callback) {
    std::scoped_lock lock(mu_);
    subscribers_.push_back({std::move(request), std::move(callback)});
  }

  void Publish(const normalization::MarketEvent& event) const {
    std::scoped_lock lock(mu_);
    for (const auto& subscriber : subscribers_) {
      if (Matches(subscriber.request, event)) {
        subscriber.callback(event);
      }
    }
  }

 private:
  struct Subscriber {
    normalization::SubscriptionRequest request;
    Callback callback;
  };

  static bool Matches(const normalization::SubscriptionRequest& request,
                      const normalization::MarketEvent& event) {
    if (request.data_type && *request.data_type != event.type) {
      return false;
    }

    const auto instrument = std::visit(
        [](const auto& payload) -> const normalization::Instrument* {
          using T = std::decay_t<decltype(payload)>;
          if constexpr (std::is_same_v<T, normalization::Instrument>) {
            return &payload;
          } else {
            return &payload.instrument;
          }
        },
        event.payload);

    if (request.instrument_id && *request.instrument_id != instrument->instrument_id) {
      return false;
    }
    if (request.venue && *request.venue != instrument->venue) {
      return false;
    }
    if (request.asset_class && *request.asset_class != instrument->asset_class) {
      return false;
    }
    return true;
  }

  mutable std::mutex mu_;
  std::vector<Subscriber> subscribers_;
};

}  // namespace mdp::core
