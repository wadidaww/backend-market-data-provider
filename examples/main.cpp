#include <iostream>
#include <memory>

#include "market_data_provider/config/config.hpp"
#include "market_data_provider/core/market_data_service.hpp"
#include "market_data_provider/feed_adapters/mock_feed_adapter.hpp"
#include "market_data_provider/normalization/models.hpp"
#include "market_data_provider/transport/admin_service.hpp"

int main() {
  mdp::config::ProviderConfig config;
  config.queue_capacity = 256;
  config.order_book_depth = 10;
  config.sources = {{"mock_source", "mock", "memory://mock"}};

  auto store = std::make_shared<mdp::storage::InMemoryEventStore>();
  mdp::core::MarketDataService service(config, store);
  const auto init_status = service.Initialize();
  if (!init_status.ok()) {
    std::cerr << "Failed to initialize: " << init_status.message() << '\n';
    return 1;
  }

  mdp::normalization::SubscriptionRequest request{};
  request.instrument_id = std::string("XNAS:AAPL");
  service.Subscribe(request,
                    [](const mdp::normalization::MarketEvent& event) {
                      if (event.type == mdp::normalization::DataType::Trade) {
                        const auto& trade = std::get<mdp::normalization::Trade>(event.payload);
                        std::cout << "trade " << trade.instrument.instrument_id << " px=" << trade.price
                                  << " qty=" << trade.quantity << '\n';
                      }
                    });

  mdp::normalization::Trade trade;
  trade.instrument = {.instrument_id = "XNAS:AAPL", .venue = "XNAS", .symbol = "AAPL", .asset_class = "equity"};
  trade.price = 190.25;
  trade.quantity = 100;
  trade.sequence_number = 1;
  trade.exchange_ts_ns = 1;
  trade.receive_ts_ns = 2;
  trade.process_ts_ns = 3;
  trade.source = "mock_source";

  mdp::feed_adapters::MockFeedAdapter adapter({mdp::normalization::MarketEvent{
      .type = mdp::normalization::DataType::Trade,
      .payload = trade,
      .source = "mock_source",
      .sequence_number = trade.sequence_number,
  }});

  const auto adapter_status = adapter.Start([&service](mdp::normalization::MarketEvent event) {
    return service.Ingest(std::move(event));
  });
  if (!adapter_status.ok()) {
    std::cerr << "Adapter failed: " << adapter_status.message() << '\n';
    return 1;
  }

  mdp::transport::AdminService admin(service);
  const auto snapshot = admin.Snapshot();
  std::cout << "ready=" << std::boolalpha << snapshot.ready
            << " ingested=" << snapshot.ingested_events << " instruments=" << snapshot.instruments
            << '\n';

  return 0;
}
