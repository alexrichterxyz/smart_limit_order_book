#ifndef ORDER_LEVEL_HPP
#define ORDER_LEVEL_HPP
#include <list>
#include <memory>
#include <cstdint>

namespace slob {

class order;
class book;

class order_level {
	private:
	std::int64_t m_quantity = 0;
	/* orders are stored in a doubly-linked list to
		allow for O(1) cancellation.*/
	std::list<order_ptr> m_orders;

	void rest_order(c_order_ptr &t_order);

	// todo: add documentation
	/**
	 * @brief Execute an inbound order.
	 *
	 * @param t_order, the inbound order
	 * @return the traded quantity.
	 */
	void match(slob::c_order_ptr &t_order);
	inline bool is_empty() const { return m_orders.empty(); }

	public:
	/**
	 * @brief Get the non-all-or-none quantity at this price level.
	 * This quantity can be filled partially.
	 *
	 * @return the non-all-or-none quantity at this price level.
	 */
	inline std::int64_t get_quantity() const;

	/**
	 * @brief Get the total number of orders (all-or-nothing
	 * included) at this price level.
	 *
	 * @return std::size_t, the number of orders (all-or-nothing
	 * included) at this price level.
	 */
	inline std::size_t get_order_count() const;

	/**
	 * @brief Get an iterator to the first order in the queue.
	 *
	 * @return std::list<slob::order_ptr>::iterator, iterator
	 * to first order in the queue.
	 */
	inline std::list<order_ptr>::iterator begin();

	/**
	 * @brief Get an iterator to the end of the order queue.
	 *
	 * @return std::list<slob::order_ptr>::iterator, iterator
	 * to the end of the order queue.
	 */
	inline std::list<order_ptr>::iterator end();

	/**
	 * @brief Get the number of orders (including all-or-nothing) at
	 * this price level.
	 *
	 * @return the number of orders (including all-or-nothing) at
	 * this price level.
	 */
	inline std::size_t order_count() const;


	friend book;
	friend order;

	~order_level();
};

} // namespace slob

#include "order.hpp"
#include <algorithm>

void slob::order_level::rest_order(
    slob::c_order_ptr &t_order) {
	m_orders.push_back(t_order);
	const auto order_it = std::prev(m_orders.end());
	m_quantity += t_order->m_quantity;
	t_order->m_order_it = order_it;
}

void slob::order_level::match(slob::c_order_ptr &t_order) {
	auto resting_order_it = m_orders.begin();
	std::int64_t &book_side_quantity = t_order->m_side == side::ask ? t_order->m_book->m_bid_quantity : t_order->m_book->m_ask_quantity;

	while (!m_orders.empty()) {
		const auto resting_order = *resting_order_it;
		const std::int64_t resting_order_quantity = resting_order->m_quantity;

		/* on_before_trade is first called on the resting order because:
		1. the resting order came first
		2. to encourage liquidity provision over consumption
		*/
		resting_order->on_before_match(std::const_pointer_cast<const order>(t_order));
		t_order->on_before_match(std::const_pointer_cast<const order>(resting_order));

		if(t_order->m_quantity > resting_order_quantity) {
			// incoming order is not filled
			// resting order is filled
			const std::int64_t volume = resting_order_quantity;
			book_side_quantity -= volume;
			m_quantity -= volume;
			t_order->m_quantity -= volume;
			resting_order->m_quantity = 0;
			m_orders.erase(resting_order_it++);
			resting_order->m_state = order_state::filled;
			resting_order->on_filled();

			// on_filled may change order state
			if(resting_order->m_state == order_state::filled) {
				resting_order->m_book = nullptr;
			}
		} else {
			// incoming order is filled
			// resting is may be filled	
			const std::int64_t volume = t_order->m_quantity;
			book_side_quantity -= volume;
			m_quantity -= volume;
			t_order->m_quantity = 0;
			resting_order->m_quantity -= volume;
			t_order->m_state = order_state::filled;

			if(resting_order->m_quantity == 0) {
				m_orders.erase(resting_order_it++);
				resting_order->m_state = order_state::filled;
				resting_order->on_filled();
				
				// on_filled may change order state
				if(resting_order->m_state == order_state::filled) {
					resting_order->m_book = nullptr;
				}
			}

			t_order->on_filled();
			
			// on_filled may reset order
			if(t_order->m_state == order_state::filled) {
				t_order->m_book = nullptr;
			}

			break;
		}

	}
}

std::int64_t slob::order_level::get_quantity() const { return m_quantity; }

std::size_t slob::order_level::get_order_count() const {
	return m_orders.size();
}

std::list<slob::order_ptr>::iterator slob::order_level::begin() {
	return m_orders.begin();
}

std::list<slob::order_ptr>::iterator slob::order_level::end() {
	return m_orders.end();
}

std::size_t slob::order_level::order_count() const { return m_orders.size(); }

slob::order_level::~order_level() {
	for (auto &order : m_orders) {
		order->m_book = nullptr;

		// when book goes out of scope and price levels are destroyed, orders to become invalidated
		order->m_state = order_state::canceled; 
	}
}

#endif // #ifndef ORDER_LEVEL_HPP