#include <cassert>
#include <cstdlib>
#include <memory>

#include "market_data_provider/config/config.hpp"
#include "market_data_provider/core/market_data_service.hpp"
#include "market_data_provider/transport/admin_service.hpp"

int main() {
  mdp::config::ProviderConfig config;
  config.queue_capacity = 64;
  config.order_book_depth = 10;
  config.sources = {{"mock_source", "mock", "memory://mock"}};

  auto store = std::make_shared<mdp::storage::InMemoryEventStore>();
  mdp::core::MarketDataService service(config, store);
  assert(service.Initialize().ok());

  mdp::transport::AdminService admin(service);

  unsetenv("MDP_ADMIN_PASSWORD");
  auto missing_env = admin.Snapshot("secret");
  assert(!missing_env.ok());
  assert(missing_env.status().category() == mdp::util::ErrorCategory::Config);

  setenv("MDP_ADMIN_PASSWORD", "correct-password", 1);
  auto unauthorized = admin.Snapshot("wrong-password");
  assert(!unauthorized.ok());
  assert(unauthorized.status().category() == mdp::util::ErrorCategory::Unauthorized);

  auto authorized = admin.Snapshot("correct-password");
  assert(authorized.ok());
  assert(authorized.value().ready);

  return 0;
}
