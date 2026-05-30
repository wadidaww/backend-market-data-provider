#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <variant>
#include <vector>

namespace mdp::normalization {

using TimestampNs = std::int64_t;

enum class DataType {
  Trade,
  Quote,
  OrderBookSnapshot,
  OrderBookDelta,
  InstrumentDefinition,
};

enum class Side { Bid, Ask };

struct Instrument {
  std::string instrument_id;
  std::string venue;
  std::string symbol;
  std::string asset_class;
};

struct Trade {
  Instrument instrument;
  double price{0.0};
  double quantity{0.0};
  std::int64_t sequence_number{0};
  TimestampNs exchange_ts_ns{0};
  TimestampNs receive_ts_ns{0};
  TimestampNs process_ts_ns{0};
  std::string source;
};

struct Quote {
  Instrument instrument;
  double bid_price{0.0};
  double bid_quantity{0.0};
  double ask_price{0.0};
  double ask_quantity{0.0};
  std::int64_t sequence_number{0};
  TimestampNs exchange_ts_ns{0};
  TimestampNs receive_ts_ns{0};
  TimestampNs process_ts_ns{0};
  std::string source;
};

struct OrderBookLevel {
  Side side{Side::Bid};
  double price{0.0};
  double quantity{0.0};
};

struct OrderBookSnapshot {
  Instrument instrument;
  std::vector<OrderBookLevel> bids;
  std::vector<OrderBookLevel> asks;
  std::int64_t sequence_number{0};
  TimestampNs exchange_ts_ns{0};
  TimestampNs receive_ts_ns{0};
  TimestampNs process_ts_ns{0};
  std::string source;
};

struct OrderBookDelta {
  Instrument instrument;
  std::vector<OrderBookLevel> updates;
  std::int64_t sequence_number{0};
  TimestampNs exchange_ts_ns{0};
  TimestampNs receive_ts_ns{0};
  TimestampNs process_ts_ns{0};
  std::string source;
};

struct SubscriptionRequest {
  std::optional<std::string> instrument_id;
  std::optional<std::string> venue;
  std::optional<std::string> asset_class;
  std::optional<DataType> data_type;
};

using MarketEventPayload =
    std::variant<Trade, Quote, OrderBookSnapshot, OrderBookDelta, Instrument>;

struct MarketEvent {
  DataType type{DataType::Trade};
  MarketEventPayload payload;
  std::string source;
  std::int64_t sequence_number{0};
};

}  // namespace mdp::normalization
