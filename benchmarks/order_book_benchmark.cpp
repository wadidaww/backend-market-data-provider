#include <chrono>
#include <cstddef>
#include <iostream>

#include "market_data_provider/orderbook/order_book.hpp"

int main() {
  mdp::orderbook::OrderBook book(20);

  mdp::normalization::OrderBookSnapshot snapshot;
  snapshot.instrument = {.instrument_id = "XNAS:AAPL", .venue = "XNAS", .symbol = "AAPL", .asset_class = "equity"};
  snapshot.sequence_number = 1;
  for (int i = 0; i < 20; ++i) {
    snapshot.bids.push_back({mdp::normalization::Side::Bid, 100.0 - i, 1.0});
    snapshot.asks.push_back({mdp::normalization::Side::Ask, 100.5 + i, 1.0});
  }
  if (!book.ApplySnapshot(snapshot).ok()) {
    return 1;
  }

  constexpr std::size_t iterations = 200000;
  auto start = std::chrono::steady_clock::now();
  for (std::size_t i = 0; i < iterations; ++i) {
    mdp::normalization::OrderBookDelta delta;
    delta.instrument = snapshot.instrument;
    delta.sequence_number = static_cast<std::int64_t>(2 + i);
    delta.updates = {{mdp::normalization::Side::Bid, 100.0, 1.0 + static_cast<double>(i % 10)}};
    if (!book.ApplyDelta(delta).ok()) {
      return 1;
    }
  }
  auto end = std::chrono::steady_clock::now();

  const auto elapsed_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
  const double per_update_ns = static_cast<double>(elapsed_ns) / static_cast<double>(iterations);

  std::cout << "iterations=" << iterations << " total_ns=" << elapsed_ns
            << " ns_per_update=" << per_update_ns << '\n';
  return 0;
}
