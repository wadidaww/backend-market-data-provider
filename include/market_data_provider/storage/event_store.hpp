#pragma once

#include <vector>

#include "market_data_provider/normalization/models.hpp"

namespace mdp::storage {

class EventStore {
 public:
  virtual ~EventStore() = default;
  virtual void AppendRaw(const normalization::MarketEvent& event) = 0;
  virtual void AppendNormalized(const normalization::MarketEvent& event) = 0;
};

class InMemoryEventStore final : public EventStore {
 public:
  void AppendRaw(const normalization::MarketEvent& event) override { raw_.push_back(event); }
  void AppendNormalized(const normalization::MarketEvent& event) override { normalized_.push_back(event); }

  [[nodiscard]] const std::vector<normalization::MarketEvent>& raw() const { return raw_; }
  [[nodiscard]] const std::vector<normalization::MarketEvent>& normalized() const { return normalized_; }

 private:
  std::vector<normalization::MarketEvent> raw_;
  std::vector<normalization::MarketEvent> normalized_;
};

}  // namespace mdp::storage
