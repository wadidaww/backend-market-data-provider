#pragma once

#include <string>
#include <unordered_map>

#include "market_data_provider/normalization/models.hpp"
#include "market_data_provider/orderbook/order_book.hpp"
#include "market_data_provider/util/status.hpp"

namespace mdp::core {

class InstrumentStateStore final {
 public:
  explicit InstrumentStateStore(std::size_t book_depth) : book_depth_(book_depth) {}

  util::Status Apply(const normalization::MarketEvent& event) {
    const auto instrument_key = std::visit(
        [](const auto& payload) -> std::string {
          using T = std::decay_t<decltype(payload)>;
          if constexpr (std::is_same_v<T, normalization::Instrument>) {
            return payload.instrument_id;
          } else {
            return payload.instrument.instrument_id;
          }
        },
        event.payload);

    latest_by_instrument_[instrument_key] = event;

    if (event.type == normalization::DataType::OrderBookSnapshot) {
      const auto& snapshot = std::get<normalization::OrderBookSnapshot>(event.payload);
      auto [it, _] = books_.try_emplace(instrument_key, orderbook::OrderBook(book_depth_));
      return it->second.ApplySnapshot(snapshot);
    }

    if (event.type == normalization::DataType::OrderBookDelta) {
      const auto& delta = std::get<normalization::OrderBookDelta>(event.payload);
      auto [it, _] = books_.try_emplace(instrument_key, orderbook::OrderBook(book_depth_));
      return it->second.ApplyDelta(delta);
    }

    return util::Status::Ok();
  }

  [[nodiscard]] const std::unordered_map<std::string, normalization::MarketEvent>& latest() const {
    return latest_by_instrument_;
  }

 private:
  std::size_t book_depth_{0};
  std::unordered_map<std::string, normalization::MarketEvent> latest_by_instrument_;
  std::unordered_map<std::string, orderbook::OrderBook> books_;
};

}  // namespace mdp::core
