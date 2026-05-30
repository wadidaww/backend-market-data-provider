#include <cassert>
#include <memory>

#include "market_data_provider/config/config.hpp"
#include "market_data_provider/core/market_data_service.hpp"
#include "market_data_provider/feed_adapters/mock_feed_adapter.hpp"

int main() {
  mdp::config::ProviderConfig config;
  config.queue_capacity = 4;
  config.order_book_depth = 5;
  config.sources = {{"mock_source", "mock", "memory://mock"}};

  auto store = std::make_shared<mdp::storage::InMemoryEventStore>();
  mdp::core::MarketDataService service(config, store);
  assert(service.Initialize().ok());

  std::size_t callback_count = 0;
  mdp::normalization::SubscriptionRequest request{};
  request.instrument_id = std::string("XNAS:AAPL");
  service.Subscribe(
      request, [&callback_count](const mdp::normalization::MarketEvent&) { ++callback_count; });

  mdp::normalization::Trade trade;
  trade.instrument = {
      .instrument_id = "XNAS:AAPL", .venue = "XNAS", .symbol = "AAPL", .asset_class = "equity"};
  trade.price = 100.0;
  trade.quantity = 10.0;
  trade.sequence_number = 1;

  mdp::feed_adapters::MockFeedAdapter adapter({mdp::normalization::MarketEvent{
      .type = mdp::normalization::DataType::Trade,
      .payload = trade,
      .source = "mock_source",
      .sequence_number = 1,
  }});

  assert(adapter
             .Start([&service](mdp::normalization::MarketEvent event) {
               return service.Ingest(std::move(event));
             })
             .ok());

  assert(callback_count == 1);
  assert(store->raw().size() == 1);
  assert(store->normalized().size() == 1);
  assert(service.state_store().latest().contains("XNAS:AAPL"));

  return 0;
}
