#ifndef COMMON_HPP
#define COMMON_HPP
#include <cstdint>
#include <memory>
#include <iomanip>
#include <cstdint>


namespace slob {

enum class side { bid = 0, ask };
enum class direction { up = 0, down };
enum class order_state {pending = 0, accepted, matching, resting, filled, canceled};
enum class trigger_state {pending = 0, accepted, resting, canceled, triggered};
enum class trigger_type {
	market_up = 0,
	bid_up,
	ask_up,
	market_down,
	bid_down,
	ask_down
};

const std::int64_t max_price = INT64_MAX;
const std::int64_t min_price = INT64_MIN;

class order;
using order_ptr = std::shared_ptr<order>;
using corder_ptr = std::shared_ptr<const order>;
using c_order_ptr = const order_ptr;
using c_corder_ptr = const corder_ptr;


class trigger;
using trigger_ptr = std::shared_ptr<trigger>;
using c_trigger_ptr = const trigger_ptr;

} // namespace slob

#endif // #ifndef COMMON_HPP