#pragma once

#include <atomic>
#include <cstdint>

namespace mdp::observability {

struct Metrics {
  std::atomic<std::uint64_t> ingested_events{0};
  std::atomic<std::uint64_t> parse_errors{0};
  std::atomic<std::uint64_t> dropped_events{0};
  std::atomic<std::uint64_t> reconnect_count{0};
  std::atomic<std::uint64_t> queue_depth{0};
};

}  // namespace mdp::observability
