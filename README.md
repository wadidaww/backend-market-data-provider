# backend-market-data-provider

Production-oriented C++20 market data backend scaffold with a low-latency event pipeline, order book handling, mock/replay adapters, observability hooks, tests, and benchmarks.

## Architecture

- `feed_adapters/`: adapter interfaces plus `MockFeedAdapter` and `ReplayFeedAdapter`
- `normalization/`: canonical schema (`Instrument`, `Trade`, `Quote`, `OrderBookSnapshot`, `OrderBookDelta`, `MarketEvent`) and parser robustness entrypoint
- `orderbook/`: sequence-aware snapshot + delta order book with depth bounds and gap detection
- `core/`: ingestion pipeline (`MarketDataService`), bounded queue/backpressure, subscription manager, instrument state store
- `storage/`: event store abstraction and in-memory append-only implementation
- `transport/`: admin snapshot API abstraction for health/readiness/metrics/source status
- `config/`: typed configuration model and validation
- `observability/`: counters for throughput, drops, parse errors, reconnects, queue depth
- `tests/`: unit + integration + parser robustness tests
- `benchmarks/`: order book update benchmark

## Build

```bash
cmake -S . -B build
cmake --build build
```

## Run tests

```bash
ctest --test-dir build --output-on-failure
```

## Run example

```bash
export MDP_ADMIN_PASSWORD=example-admin-password
./build/mdp_example
```

`AdminService` request entrypoints (`Health`, `Ready`, `Snapshot`) require the provided password to match `MDP_ADMIN_PASSWORD`.

## Run benchmark

```bash
./build/order_book_benchmark
```

## Threading model and performance notes

- Ingestion path is currently single-threaded and deterministic (easy to reason about tail behavior).
- Queue is bounded (`queue_capacity`) and enforces backpressure (`QueueFull` status) instead of unbounded growth.
- Order book updates are sequence-aware and idempotent for stale deltas (`sequence <= last_sequence`).
- Hot path avoids exception-heavy control flow and uses status-based error handling.
- Current store/admin implementations are abstractions with lightweight defaults; they can be swapped for persistent/networked versions without changing hot-path contracts.

## Extension guide

1. Implement new `feed_adapters::Adapter` for a venue/protocol.
2. Add parser/normalization logic under `normalization/`.
3. Emit canonical `MarketEvent` objects into `core::MarketDataService::Ingest`.
4. Plug persistent storage by implementing `storage::EventStore`.
5. Replace `transport::AdminService` abstraction with gRPC/REST wrappers using the snapshot interfaces.
