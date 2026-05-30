#include <cassert>
#include <string>
#include <vector>

#include "market_data_provider/normalization/upstream_parser.hpp"

int main() {
  const auto ok = mdp::normalization::UpstreamTradeParser::ParseCsv(
      "XNAS,AAPL,equity,100.5,10,42,1000,1001", "mock");
  assert(ok.ok());
  assert(ok.value().instrument.instrument_id == "XNAS:AAPL");

  const std::vector<std::string> malformed = {
      "",                                          // empty
      "XNAS,AAPL,equity",                          // short
      "XNAS,AAPL,equity,abc,10,42,1000,1001",     // bad float
      "XNAS,AAPL,equity,100.5,10,seq,1000,1001",  // bad int
      "XNAS,AAPL,equity,100.5,10,42,1000",         // missing field
  };

  for (const auto& line : malformed) {
    const auto parsed = mdp::normalization::UpstreamTradeParser::ParseCsv(line, "mock");
    assert(!parsed.ok());
  }

  return 0;
}
