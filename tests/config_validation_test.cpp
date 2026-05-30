#include <cassert>

#include "market_data_provider/config/config.hpp"

int main() {
  mdp::config::ProviderConfig invalid;
  invalid.queue_capacity = 0;
  invalid.order_book_depth = 5;
  assert(!mdp::config::ConfigValidator::Validate(invalid).ok());

  mdp::config::ProviderConfig valid;
  valid.queue_capacity = 16;
  valid.order_book_depth = 5;
  valid.sources = {{"s1", "mock", "memory://mock"}};
  assert(mdp::config::ConfigValidator::Validate(valid).ok());

  valid.sources = {{"s1", "unknown", "memory://mock"}};
  assert(!mdp::config::ConfigValidator::Validate(valid).ok());

  return 0;
}
