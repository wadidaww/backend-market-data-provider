#include "market_data_provider/normalization/upstream_parser.hpp"

#include <charconv>
#include <string>
#include <vector>

namespace mdp::normalization {
namespace {

std::vector<std::string_view> Split(std::string_view input, const char delimiter) {
  std::vector<std::string_view> parts;
  std::size_t start = 0;
  while (start <= input.size()) {
    const auto pos = input.find(delimiter, start);
    if (pos == std::string_view::npos) {
      parts.push_back(input.substr(start));
      break;
    }
    parts.push_back(input.substr(start, pos - start));
    start = pos + 1;
  }
  return parts;
}

util::StatusOr<double> ParseDouble(std::string_view text) {
  double value = 0.0;
  const auto* begin = text.data();
  const auto* end = text.data() + text.size();
  const auto result = std::from_chars(begin, end, value);
  if (result.ec != std::errc{} || result.ptr != end) {
    return util::Status::Error(util::ErrorCategory::Parse, "invalid floating point value");
  }
  return value;
}

util::StatusOr<std::int64_t> ParseInt64(std::string_view text) {
  std::int64_t value = 0;
  const auto* begin = text.data();
  const auto* end = text.data() + text.size();
  const auto result = std::from_chars(begin, end, value);
  if (result.ec != std::errc{} || result.ptr != end) {
    return util::Status::Error(util::ErrorCategory::Parse, "invalid int64 value");
  }
  return value;
}

}  // namespace

util::StatusOr<Trade> UpstreamTradeParser::ParseCsv(const std::string_view line, std::string source) {
  const auto fields = Split(line, ',');
  if (fields.size() != 8) {
    return util::Status::Error(util::ErrorCategory::Parse,
                               "trade csv must have 8 columns: venue,symbol,asset_class,price,qty,seq,exchange_ts,receive_ts");
  }

  auto price = ParseDouble(fields[3]);
  if (!price.ok()) {
    return price.status();
  }
  auto qty = ParseDouble(fields[4]);
  if (!qty.ok()) {
    return qty.status();
  }
  auto seq = ParseInt64(fields[5]);
  if (!seq.ok()) {
    return seq.status();
  }
  auto exchange_ts = ParseInt64(fields[6]);
  if (!exchange_ts.ok()) {
    return exchange_ts.status();
  }
  auto receive_ts = ParseInt64(fields[7]);
  if (!receive_ts.ok()) {
    return receive_ts.status();
  }

  Trade trade;
  trade.instrument.venue = std::string(fields[0]);
  trade.instrument.symbol = std::string(fields[1]);
  trade.instrument.asset_class = std::string(fields[2]);
  trade.instrument.instrument_id = trade.instrument.venue + ":" + trade.instrument.symbol;
  trade.price = price.value();
  trade.quantity = qty.value();
  trade.sequence_number = seq.value();
  trade.exchange_ts_ns = exchange_ts.value();
  trade.receive_ts_ns = receive_ts.value();
  trade.process_ts_ns = receive_ts.value();
  trade.source = std::move(source);
  return trade;
}

}  // namespace mdp::normalization
