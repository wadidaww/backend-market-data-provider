#pragma once

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

#include "market_data_provider/core/market_data_service.hpp"
#include "market_data_provider/transport/request_authenticator.hpp"
#include "market_data_provider/util/status.hpp"

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

  [[nodiscard]] util::StatusOr<bool> Health(const std::string_view password) const {
    const auto status = Authenticate(password);
    if (!status.ok()) {
      return status;
    }
    return service_.is_ready();
  }

  [[nodiscard]] util::StatusOr<bool> Ready(const std::string_view password) const {
    const auto status = Authenticate(password);
    if (!status.ok()) {
      return status;
    }
    return service_.is_ready();
  }

  [[nodiscard]] util::StatusOr<AdminSnapshot> Snapshot(const std::string_view password) const {
    const auto status = Authenticate(password);
    if (!status.ok()) {
      return status;
    }

    AdminSnapshot result;
    result.ready = service_.is_ready();
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
  [[nodiscard]] static util::Status Authenticate(const std::string_view password) {
    return RequestAuthenticator::Authenticate(password);
  }

  const core::MarketDataService& service_;
};

}  // namespace mdp::transport
