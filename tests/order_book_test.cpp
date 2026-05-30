#include <cassert>

#include "market_data_provider/orderbook/order_book.hpp"

int main() {
  mdp::orderbook::OrderBook book(2);

  mdp::normalization::OrderBookSnapshot snapshot;
  snapshot.instrument = {
      .instrument_id = "XNAS:AAPL", .venue = "XNAS", .symbol = "AAPL", .asset_class = "equity"};
  snapshot.sequence_number = 10;
  snapshot.bids = {{mdp::normalization::Side::Bid, 100.0, 5.0},
                   {mdp::normalization::Side::Bid, 99.0, 4.0},
                   {mdp::normalization::Side::Bid, 98.0, 3.0}};
  snapshot.asks = {{mdp::normalization::Side::Ask, 101.0, 2.0},
                   {mdp::normalization::Side::Ask, 102.0, 1.0}};

  assert(book.ApplySnapshot(snapshot).ok());

  mdp::normalization::OrderBookDelta delta;
  delta.instrument = snapshot.instrument;
  delta.sequence_number = 11;
  delta.updates = {{mdp::normalization::Side::Bid, 100.0, 6.0},
                   {mdp::normalization::Side::Ask, 101.0, 0.0}};

  assert(book.ApplyDelta(delta).ok());

  auto latest = book.ToSnapshot(snapshot.instrument, "mock");
  assert(latest.bids.size() == 2);
  assert(latest.asks.size() == 1);
  assert(latest.bids.front().price == 100.0);
  assert(latest.bids.front().quantity == 6.0);

  mdp::normalization::OrderBookDelta gap;
  gap.instrument = snapshot.instrument;
  gap.sequence_number = 13;
  assert(!book.ApplyDelta(gap).ok());

  return 0;
}
