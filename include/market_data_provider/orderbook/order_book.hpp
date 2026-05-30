#pragma once

#include <cstddef>
#include <map>
#include <optional>
#include <vector>

#include "market_data_provider/normalization/models.hpp"
#include "market_data_provider/util/status.hpp"

namespace mdp::orderbook {

class OrderBook final {
 public:
  explicit OrderBook(std::size_t depth) : depth_(depth) {}

  util::Status ApplySnapshot(const normalization::OrderBookSnapshot& snapshot);
  util::Status ApplyDelta(const normalization::OrderBookDelta& delta);

  [[nodiscard]] std::optional<std::int64_t> last_sequence() const { return last_sequence_; }

  [[nodiscard]] normalization::OrderBookSnapshot ToSnapshot(normalization::Instrument instrument,
                                                            std::string source) const;

 private:
  static util::Status ValidateLevel(const normalization::OrderBookLevel& level);
  static void UpsertLevel(std::map<double, double, std::greater<>>& levels, double price,
                          double quantity);
  static void UpsertLevel(std::map<double, double>& levels, double price, double quantity);

  std::size_t depth_{0};
  std::optional<std::int64_t> last_sequence_;
  std::map<double, double, std::greater<>> bids_;
  std::map<double, double> asks_;
};

}  // namespace mdp::orderbook
