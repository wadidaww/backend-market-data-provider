#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "market_data_provider/core/market_data_service.hpp"

namespace mdp::transport {

struct SourceStatus {
  std::string source;
  bool connected{true};
  std::uint64_t reconnect_count{0};
};

struct AdminSnapshot {
  bool healthy{true};
  bool ready{false};
  std::uint64_t ingested_events{0};
  std::uint64_t dropped_events{0};
  std::uint64_t parse_errors{0};
  std::uint64_t queue_depth{0};
  std::size_t subscriptions{0};
  std::size_t instruments{0};
  std::vector<SourceStatus> source_status;
};

class AdminService final {
 public:
  explicit AdminService(const core::MarketDataService& service) : service_(service) {}

  [[nodiscard]] bool Health() const { return true; }
  [[nodiscard]] bool Ready() const { return service_.is_ready(); }

  [[nodiscard]] AdminSnapshot Snapshot() const {
    AdminSnapshot result;
    result.ready = Ready();
    result.ingested_events = service_.metrics().ingested_events.load(std::memory_order_relaxed);
    result.dropped_events = service_.metrics().dropped_events.load(std::memory_order_relaxed);
    result.parse_errors = service_.metrics().parse_errors.load(std::memory_order_relaxed);
    result.queue_depth = service_.metrics().queue_depth.load(std::memory_order_relaxed);
    result.instruments = service_.state_store().latest().size();
    result.source_status.push_back({"mock", true,
                                    service_.metrics().reconnect_count.load(std::memory_order_relaxed)});
    return result;
  }

 private:
  const core::MarketDataService& service_;
};

}  // namespace mdp::transport
