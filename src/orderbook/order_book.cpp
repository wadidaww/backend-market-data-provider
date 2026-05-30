#include "market_data_provider/orderbook/order_book.hpp"

#include <algorithm>

namespace mdp::orderbook {

util::Status OrderBook::ValidateLevel(const normalization::OrderBookLevel& level) {
  if (level.price < 0.0 || level.quantity < 0.0) {
    return util::Status::Error(util::ErrorCategory::Parse, "price and quantity must be non-negative");
  }
  return util::Status::Ok();
}

void OrderBook::UpsertLevel(std::map<double, double, std::greater<>>& levels, const double price,
                            const double quantity) {
  if (quantity == 0.0) {
    levels.erase(price);
  } else {
    levels[price] = quantity;
  }
}

void OrderBook::UpsertLevel(std::map<double, double>& levels, const double price,
                            const double quantity) {
  if (quantity == 0.0) {
    levels.erase(price);
  } else {
    levels[price] = quantity;
  }
}

util::Status OrderBook::ApplySnapshot(const normalization::OrderBookSnapshot& snapshot) {
  bids_.clear();
  asks_.clear();

  for (const auto& level : snapshot.bids) {
    const auto status = ValidateLevel(level);
    if (!status.ok()) {
      return status;
    }
    UpsertLevel(bids_, level.price, level.quantity);
  }
  for (const auto& level : snapshot.asks) {
    const auto status = ValidateLevel(level);
    if (!status.ok()) {
      return status;
    }
    UpsertLevel(asks_, level.price, level.quantity);
  }

  while (bids_.size() > depth_) {
    bids_.erase(std::prev(bids_.end()));
  }
  while (asks_.size() > depth_) {
    asks_.erase(std::prev(asks_.end()));
  }

  last_sequence_ = snapshot.sequence_number;
  return util::Status::Ok();
}

util::Status OrderBook::ApplyDelta(const normalization::OrderBookDelta& delta) {
  if (!last_sequence_.has_value()) {
    return util::Status::Error(util::ErrorCategory::SequenceGap,
                               "delta received before snapshot initialization");
  }
  if (delta.sequence_number <= *last_sequence_) {
    return util::Status::Ok();
  }
  if (delta.sequence_number != *last_sequence_ + 1) {
    return util::Status::Error(util::ErrorCategory::SequenceGap,
                               "order book sequence gap detected");
  }

  for (const auto& level : delta.updates) {
    const auto status = ValidateLevel(level);
    if (!status.ok()) {
      return status;
    }
    if (level.side == normalization::Side::Bid) {
      UpsertLevel(bids_, level.price, level.quantity);
    } else {
      UpsertLevel(asks_, level.price, level.quantity);
    }
  }

  while (bids_.size() > depth_) {
    bids_.erase(std::prev(bids_.end()));
  }
  while (asks_.size() > depth_) {
    asks_.erase(std::prev(asks_.end()));
  }

  last_sequence_ = delta.sequence_number;
  return util::Status::Ok();
}

normalization::OrderBookSnapshot OrderBook::ToSnapshot(normalization::Instrument instrument,
                                                       std::string source) const {
  normalization::OrderBookSnapshot snapshot;
  snapshot.instrument = std::move(instrument);
  snapshot.source = std::move(source);
  snapshot.sequence_number = last_sequence_.value_or(0);

  snapshot.bids.reserve(std::min(depth_, bids_.size()));
  snapshot.asks.reserve(std::min(depth_, asks_.size()));

  for (const auto& [price, quantity] : bids_) {
    if (snapshot.bids.size() >= depth_) {
      break;
    }
    snapshot.bids.push_back({normalization::Side::Bid, price, quantity});
  }
  for (const auto& [price, quantity] : asks_) {
    if (snapshot.asks.size() >= depth_) {
      break;
    }
    snapshot.asks.push_back({normalization::Side::Ask, price, quantity});
  }
  return snapshot;
}

}  // namespace mdp::orderbook
