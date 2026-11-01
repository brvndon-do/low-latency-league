#include "engine.hpp"

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <random>
#include <vector>

// ---------------------------------------------------------------------------
// Workload description
// ---------------------------------------------------------------------------

enum class OpType : uint8_t { MATCH, MODIFY, VOLUME };

// One pre-generated operation. Not every field is used by every OpType.
struct Op {
    OpType type;
    Order order;           // MATCH: the incoming order
    IdType target_id;      // MODIFY: which order to change
    QuantityType new_qty;  // MODIFY: 0 means cancel
    Side query_side;       // VOLUME
    PriceType query_price; // VOLUME
};

struct WorkloadConfig {
    size_t num_ops = 1'000'000;
    uint64_t seed = 42;

    // Operation mix (should sum to 1.0)
    double p_match = 0.60;
    double p_modify = 0.30;
    double p_volume = 0.10;

    // Price model
    double start_mid = 10'000.0;
    double price_stddev = 20.0; // how tightly prices cluster around mid
    double drift_stddev = 0.5;  // how much mid wanders per op
};

// ---------------------------------------------------------------------------
// Workload generation (NOT timed)
// ---------------------------------------------------------------------------

std::vector<Op> generate_workload(const WorkloadConfig& cfg) {
    std::vector<Op> ops;
    ops.reserve(cfg.num_ops);

    std::mt19937_64 rng(cfg.seed);

    // TODO: distributions you'll need
    //  - one to pick the op type from the mix
    //  - one for price offsets around the mid (hint: clustering)
    //  - one for quantity
    //  - one for side

    // TODO: track IDs that are (probably) resting so MODIFY targets are valid.
    //   Q: does the generator need to know exactly which orders got filled?
    //      Or is "sometimes targets a dead ID" realistic enough? Decide and
    //      be aware of the tradeoff.

    IdType next_id = 1;
    double mid = cfg.start_mid;

    for (size_t i = 0; i < cfg.num_ops; ++i) {
        // TODO: drift the mid
        // TODO: choose op type, fill in the Op, push_back
        //   - clamp prices to a valid PriceType range
        //   - cancels (new_qty == 0) should be a decent fraction of modifies
        (void)next_id;
        (void)mid;
    }

    return ops;
}

// ---------------------------------------------------------------------------
// Execution (timed)
// ---------------------------------------------------------------------------

struct RunResult {
    double ns_per_op;
    uint64_t checksum; // keeps the optimizer from deleting work
};

RunResult run_once(const std::vector<Op>& ops) {
    Orderbook* ob = create_orderbook();
    uint64_t checksum = 0;

    auto start = std::chrono::steady_clock::now();

    for (const Op& op : ops) {
        switch (op.type) {
        case OpType::MATCH:
            // TODO
            break;
        case OpType::MODIFY:
            // TODO
            break;
        case OpType::VOLUME:
            // TODO
            break;
        }
    }

    auto end = std::chrono::steady_clock::now();

    delete ob;

    // TODO: compute ns_per_op from (end - start)
    (void)start;
    (void)end;
    return {0.0, checksum};
}

// ---------------------------------------------------------------------------
// Driver
// ---------------------------------------------------------------------------

int main(int argc, char** argv) {
    WorkloadConfig cfg;
    if (argc > 1)
        cfg.num_ops = std::strtoull(argv[1], nullptr, 10);

    int trials = 7;

    std::vector<Op> ops = generate_workload(cfg);

    // TODO: one untimed warmup run (why might the first run be slower?)

    std::vector<double> samples;
    uint64_t checksum = 0;
    for (int t = 0; t < trials; ++t) {
        RunResult r = run_once(ops);
        samples.push_back(r.ns_per_op);
        checksum ^= r.checksum;
    }

    // TODO: report median, min, max
    //   Q: why median rather than mean?

    std::printf("ops=%zu trials=%d checksum=%llu\n", cfg.num_ops, trials,
                (unsigned long long)checksum);
    return 0;
}
