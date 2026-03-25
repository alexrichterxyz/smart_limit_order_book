#ifndef BOOK_HPP
#define BOOK_HPP
#include "common.hpp"
#include "deferrable.hpp"
#include "insertable_iterator.hpp"
#include <map>
#include <memory>
#include <queue>
#include <vector>
#include <cstdint>

namespace slob {

class book;
class order_level;
class trigger_level;
class trigger;
class order;
class insertable;
class deferrable;

using bid_order_iterator = slob::insertable_iterator<std::greater<std::int64_t>,
    slob::order_level, std::shared_ptr<slob::order>>;

using ask_order_iterator = slob::insertable_iterator<std::less<std::int64_t>,
    slob::order_level, std::shared_ptr<slob::order>>;

std::ostream &operator<<(std::ostream &t_os, const book &t_book);

/**
 * @brief book implements a price-time-priority matching engine. Orders
 * and triggers can be inserted into book objects.
 *
 */
class book {
	private:

	/* During order execution. event handlers like "on_before_match" are
	 * called which may insert additional orders recursively. These
	 * additional orders will be deferred. Only once
	 * the outer insertion call has completed, the additional orders
	 * are removed from the deferral queue and executed. */
	bool m_deferrals_enabled = false;
	std::queue<deferrable> m_deferred;

	std::int64_t m_bid_quantity = 0;
	std::int64_t m_ask_quantity = 0;

	std::map<std::int64_t, order_level, std::greater<std::int64_t>> m_bids;
	std::map<std::int64_t, order_level, std::less<std::int64_t>> m_asks;

	std::map<std::int64_t, trigger_level, std::less<std::int64_t>> m_bid_up_triggers;
	std::map<std::int64_t, trigger_level, std::less<std::int64_t>> m_ask_up_triggers;
	std::map<std::int64_t, trigger_level, std::less<std::int64_t>>
	    m_market_up_triggers;
	std::map<std::int64_t, trigger_level, std::greater<std::int64_t>>
	    m_bid_down_triggers;
	std::map<std::int64_t, trigger_level, std::greater<std::int64_t>>
	    m_ask_down_triggers;
	std::map<std::int64_t, trigger_level, std::greater<std::int64_t>>
	    m_market_down_triggers;

	// set to INT64_MIN to prevent triggers from being triggered
	// immediately.
	std::int64_t m_market_price = slob::min_price;

	/**
	 * \internal
	 * @brief When called, subsequent orders will be deferred rather
	 * than being queued immediately. This is required to ensure
	 * orders are fully executed before new orders, e.g. those
	 * inserted from within event handlers, are executed.
	 *
	 */
	inline void begin_deferral();

	/**
	 * \internal
	 * @brief Once the outer insertion call has been completed,
	 * orders from the deferral queue are executed.
	 *
	 */
	inline void end_deferral();

	inline bool insert_with_options(
	    c_order_ptr &t_order, const bool t_enable_deferral);
	inline void insert_bid(c_order_ptr &t_order);
	inline void insert_ask(c_order_ptr &t_order);

	/**
	 * \internal
	 * @brief Check if the bid order is executable (can execute against at
	 * least one order and min_quantity can be filled)
	 *
	 * @param t_order the bid order to be executed.
	 * @return true the order is executable
	 * @return false the order is not executable
	 */
	inline bool bid_min_quantity_fillable(c_order_ptr &t_order) const;

	/**
	 * \internal
	 * @brief Check if the ask order is executable (can execute against at
	 * least one order and min_quantity can be filled)
	 *
	 * @param t_order the ask order to be executed.
	 * @return true the order is executable
	 * @return false the order is not executable
	 */
	inline bool ask_min_quantity_fillable(c_order_ptr &t_order) const;

	inline void match_bid(c_order_ptr &t_order);
	inline void match_ask(c_order_ptr &t_order);

	inline void rest_bid(c_order_ptr &t_order);
	inline void rest_ask(c_order_ptr &t_order);

	template <class T>
	inline void rest_trigger(slob::c_trigger_ptr &t_trigger, T &t_triggers);

	template <class T>
	inline void trigger_up(const int64_t t_price, T &t_triggers);

	template <class T>
	inline void trigger_down(const int64_t t_price, T &t_triggers);

	template <class T>
	inline void insert_trigger_up(c_trigger_ptr &t_trigger, const int64_t t_price, T &t_triggers);

	template <class T>
	inline void insert_trigger_down(c_trigger_ptr &t_trigger, const int64_t t_price, T &t_triggers);

	
	public:
	template <class T, class... Args>
	inline std::shared_ptr<T> insert(Args &&...args);

	/**
	 * @brief Inserts an order into the book. Marketable orders will
	 * be executed. Partially filled orders will be queued (or
	 * canelled if marked as immediate-or-cancel). When the function
	 * is called from within another order's event handler (like
	 * on_before_trade), the order will be deferred and only executed once
	 * the other order has been handled.
	 *
	 * @param t_order the order to be inserted
	 */
	inline bool insert(c_order_ptr &t_order);

	/**
	 * @brief Inserts a trigger into the book. Unlike orders,
	 * triggers will cannot be deferred and will instead be queued
	 * immediately.
	 *
	 * @param t_trigger the trigger to be inserted
	 */
	inline bool insert(c_trigger_ptr &t_trigger);

	inline bool insert(const insertable &ins);

	// todo: documentation
	inline bool is_deferring() const;

	// todo: documentation
	inline std::int64_t get_bid_quantity() const;

	// todo: documentation
	inline std::int64_t get_ask_quantity() const;

	/**
	 * @brief Get the best bid price.
	 *
	 * @return std::int64_t the best bid price
	 */
	inline std::int64_t get_bid_price() const;

	/**
	 * @brief Get the best ask price.
	 *
	 * @return std::int64_t the best ask price
	 */
	inline std::int64_t get_ask_price() const;

	// todo: documentation
	inline std::size_t get_bid_level_count() const;

	// todo: documentation
	inline std::size_t get_ask_level_count() const;

	/**
	 * @brief Get the price at which the last trade occured.
	 *
	 * @return std::int64_t the current market price
	 */
	inline std::int64_t get_market_price() const;

	/**
	 * @brief Get an iterator to the first bid price level
	 *
	 * @return std::map<std::int64_t, order_level>::iterator bid begin
	 * iterator
	 */
	inline std::map<std::int64_t, order_level>::iterator bid_levels_begin();

	/**
	 * @brief Get an iterator to the first ask price level
	 *
	 * @return std::map<std::int64_t, order_level>::iterator ask begin
	 * iterator
	 */
	inline std::map<std::int64_t, order_level>::iterator ask_levels_begin();

	/**
	 * @brief Get an iterator to the end of the bids
	 *
	 * @return std::map<std::int64_t, order_level>::iterator bid price
	 * level end iterator
	 */
	inline std::map<std::int64_t, order_level>::iterator bid_levels_end();

	/**
	 * @brief Get an iterator to the end of the asks
	 *
	 * @return std::map<std::int64_t, order_level>::iterator ask price
	 * level end iterator
	 */
	inline std::map<std::int64_t, order_level>::iterator ask_levels_end();

	/**
	 * @brief Get bid price level at specified price
	 *
	 * @param t_price the price of the bid level
	 * @return std::map<std::int64_t, order_level>::iterator the bid price
	 * level. Equals bid_levels_end() if this price level does not
	 * exist.
	 */
	inline std::map<std::int64_t, order_level>::iterator bid_level_at(
	    const std::int64_t t_price);

	/**
	 * @brief Get ask price level at specified price
	 *
	 * @param t_price the price of the ask level
	 * @return std::map<std::int64_t, order_level>::iterator the ask price
	 * level. Equals ask_levels_end() if this price level does not
	 * exist.
	 */
	inline std::map<std::int64_t, order_level>::iterator ask_level_at(
	    const std::int64_t t_price);

	/**
	 * @brief Get iterator to first order pointer at first
	 * price level on bid side
	 *
	 * @return bid_order_iterator to order pointer
	 */
	inline bid_order_iterator bid_orders_begin();

	/**
	 * @brief Get iterator to first order pointer at first
	 * price level on ask side
	 *
	 * @return ask_order_iterator to order pointer
	 */
	inline ask_order_iterator ask_orders_begin();

	/**
	 * @brief Get iterator to past-the-end order pointer on bid side
	 *
	 * @return bid_order_iterator iterator to past-the-end order pointer
	 */
	inline bid_order_iterator bid_orders_end();

	/**
	 * @brief Get iterator to past-the-end order pointer on ask side
	 *
	 * @return ask_order_iterator iterator to past-the-end order pointer
	 */
	inline ask_order_iterator ask_orders_end();

	~book();

	friend std::ostream &operator<<(std::ostream &t_os, const book &t_book);
	friend order;
	friend trigger;
	friend order_level;
};

} // namespace slob

#include "common.hpp"
#include "deferrable.hpp"
#include "insertable.hpp"
#include "insertable_iterator.hpp"
#include "order.hpp"
#include "order_level.hpp"
#include "trigger.hpp"
#include "trigger_level.hpp"
#include <iomanip>

std::ostream &slob::operator<<(std::ostream &t_os, const slob::book &t_book) {
	std::size_t w = 12;
	auto bid_it = t_book.m_bids.begin();
	auto ask_it = t_book.m_asks.begin();
	std::locale loc_before = t_os.getloc();
	std::locale loc("en_US.UTF-8"); // todo: handle possible throw

	t_os.imbue(loc);
	t_os << "┌───────────BIDS───────────"
	     << "┬───────────ASKS───────────┐\n";
	t_os << "│ " << std::setw(w) << "PRC" << std::setw(w) << "QTY"
	     << " │ ";
	t_os << std::setw(w) << "PRC" << std::setw(w) << "QTY"
	     << " │\n";

	while (true) {

		if (bid_it == t_book.m_bids.end() &&
		    ask_it == t_book.m_asks.end()) {
			break;
		}

		if (bid_it != t_book.m_bids.end()) {
			t_os << "│ " << std::setw(w) << bid_it->first
			     << std::setw(w) << bid_it->second.get_quantity()
			     << " │ ";
			bid_it++;
		} else {
			t_os << "│ " << std::setw(24) << ' ' << " │ ";
		}

		if (ask_it != t_book.m_asks.end()) {
			if (ask_it->second.order_count() > 0) {
				t_os << std::setw(w) << ask_it->first
				     << std::setw(w)
				     << ask_it->second.get_quantity() << " │\n";
			}
			ask_it++;
		} else {
			t_os << std::setw(24) << ' ' << " │\n";
		}
	}

	t_os.imbue(loc_before);
	return t_os;
}

template <class T, class... Args>
std::shared_ptr<T> slob::book::insert(Args &&...args) {
	auto ptr = std::make_shared<T>(std::forward<Args>(args)...);
	insert(ptr);
	return ptr;
}

bool slob::book::insert(slob::c_order_ptr &t_order) {
	return insert_with_options(t_order, true);
}

bool slob::book::insert_with_options(
    slob::c_order_ptr &t_order, const bool t_enable_deferral) {

	if (t_order->m_state != order_state::pending ||
	    t_order->m_quantity <= 0 ||
	    t_order->m_quantity < t_order->m_min_quantity) {
		return false;
	}

	t_order->m_book = this;

	if (t_enable_deferral && is_deferring()) {
		m_deferred.push(insert_order_deferrable(t_order));
		return true;
	}

	t_order->m_state = order_state::accepted;
	t_order->on_accepted();

	// on_accepted may cancel the order
	if (t_order->m_state != order_state::accepted) {
		return true;
	}

	begin_deferral();

	if (t_order->m_side == side::bid) {
		insert_bid(t_order);
	} else {
		insert_ask(t_order);
	}

	end_deferral();
	return true;
}

bool slob::book::is_deferring() const {
	// no need to check if deferral queue is empty since 
	// m_deferrals_enabled is only set to false once the m_deferred is empty
	return m_deferrals_enabled;// || !m_deferred.empty();
}

void slob::book::begin_deferral() { m_deferrals_enabled = true;}

void slob::book::end_deferral() {
	// every deferrable modification may still be deferred if the deferral
	// queue is not empty -see is_deferring()

	while (!m_deferred.empty()) {
		auto deferrable_obj = m_deferred.front();
		m_deferred.pop();

		switch (deferrable_obj.type()) {
		case slob::deferrable_type::insert_order: {
			
			const order_ptr &order =
			    deferrable_obj.get_insert_order_deferrable().order;

			/* insert with options checks state and calls
			on_accepted where orders can be canceled
			*/
			insert_with_options(order, false);

			break;
		}
		case slob::deferrable_type::cancel_order: {
			const order_ptr &order =
			    deferrable_obj.get_cancel_order_deferrable().order;

			if (!order->is_closed() &&
			    order->on_deferred_cancel()) {
				order->cancel_with_options(false);
			}

			break;
		}
		case slob::deferrable_type::set_price: {
			const auto &obj =
			    deferrable_obj.get_set_price_deferrable();
			const order_ptr &order = obj.order;
			const std::int64_t price = obj.price;

			if (!order->is_closed() &&
			    order->on_deferred_set_price(price)) {
				order->set_price_with_options(price, false);
			}

			break;
		}
		case slob::deferrable_type::set_quantity: {
			const auto &obj =
			    deferrable_obj.get_set_quantity_deferrable();
			const order_ptr &order = obj.order;
			const std::int64_t quantity = obj.quantity;

			if (!order->is_closed() &&
			    order->on_deferred_set_quantity(quantity)) {
				order->set_quantity_with_options(
				    quantity, false);
			}

			break;
		}
		}
	}

	m_deferrals_enabled = false;
}

template <class T>
void slob::book::rest_trigger(slob::c_trigger_ptr &t_trigger, 
    T &t_triggers) {

	t_trigger->m_state = trigger_state::resting;
	const auto &level_it =
	    t_triggers.emplace(t_trigger->m_price, slob::trigger_level()).first;
	const auto trigger_it = level_it->second.insert(t_trigger);
	t_trigger->m_level_it = level_it;
	t_trigger->m_trigger_it = trigger_it;
	t_trigger->on_resting();
}

template <class T>
void slob::book::trigger_up(const int64_t t_price, 
    T &t_triggers) {
	
	auto trigger_level_it = t_triggers.begin();

	while (trigger_level_it != t_triggers.end() &&
			trigger_level_it->first <= t_price) {
		trigger_level_it->second.trigger_all();
		++trigger_level_it;
	}

	t_triggers.erase(t_triggers.begin(), trigger_level_it);
}

template <class T>
void slob::book::trigger_down(const int64_t t_price, 
    T &t_triggers) {
	
	auto trigger_level_it = t_triggers.begin();

	while (trigger_level_it != t_triggers.end() &&
			trigger_level_it->first >= t_price) {
		trigger_level_it->second.trigger_all();
		++trigger_level_it;
	}

	t_triggers.erase(t_triggers.begin(), trigger_level_it);
}


template <class T>
void slob::book::insert_trigger_down(slob::c_trigger_ptr &t_trigger, const int64_t t_price, T &t_triggers) {
	if (t_trigger->m_price >= t_price) {
		t_trigger->m_state = trigger_state::triggered;
		t_trigger->on_triggered();

		// t_trigger may reinsert the trigger
		if(t_trigger->m_state == trigger_state::triggered) {
			t_trigger->m_book = nullptr;
		}

		return;
	}

	rest_trigger(t_trigger, t_triggers);
}

template <class T>
void slob::book::insert_trigger_up(slob::c_trigger_ptr &t_trigger, const int64_t t_price, T &t_triggers) {
	if (t_trigger->m_price <= t_price) {
		t_trigger->m_state = trigger_state::triggered;
		t_trigger->on_triggered();

		// t_trigger may reinsert the trigger
		if(t_trigger->m_state == trigger_state::triggered) {
			t_trigger->m_book = nullptr;
		}

		return;
	}

	rest_trigger(t_trigger, t_triggers);
}

bool slob::book::insert(slob::c_trigger_ptr &t_trigger) {
	// check if trigger is valid
	if (t_trigger->m_state != trigger_state::pending) {
		return false;
	}
	
	t_trigger->m_book = this;
	t_trigger->m_state = trigger_state::accepted;
	t_trigger->on_accepted();

	// on_accepted may cancel trigger
	if(t_trigger->m_state != trigger_state::accepted) {
		return true;
	}

	switch (t_trigger->m_type) {
	case trigger_type::bid_up: {
		insert_trigger_up(t_trigger, get_bid_price(), m_bid_up_triggers);
		break;
	}
	case trigger_type::ask_up: {
		insert_trigger_up(t_trigger, get_ask_price(), m_ask_up_triggers);
		break;
	}
	case trigger_type::market_up: {
		insert_trigger_up(t_trigger, m_market_price, m_market_up_triggers);
		break;
	}
	case trigger_type::bid_down: {
		insert_trigger_down(t_trigger, get_bid_price(), m_bid_down_triggers);
		break;
	}
	case trigger_type::ask_down: {
		insert_trigger_down(t_trigger, get_ask_price(), m_ask_down_triggers);
		break;
	}
	case trigger_type::market_down: {
		insert_trigger_down(t_trigger, m_market_price, m_market_down_triggers);
		break;
	}
	}

	return true;
}

bool slob::book::insert(const insertable &ins) {

	if (ins.is_order()) {
		return insert(*(ins.get_order()));
	} else {
		return insert(*(ins.get_trigger()));
	}
}

void slob::book::rest_bid(slob::c_order_ptr &t_order) {
	m_bid_quantity += t_order->m_quantity;
	const auto level_it =
	    m_bids.emplace(t_order->m_price, slob::order_level()).first;
	level_it->second.rest_order(t_order);
	t_order->m_level_it = level_it;
	t_order->m_state = order_state::resting;
	t_order->on_resting();

	// trigger bid up triggers
	trigger_up(t_order->m_price, m_bid_up_triggers);
}

void slob::book::rest_ask(slob::c_order_ptr &t_order) {
	m_ask_quantity += t_order->m_quantity;
	const auto level_it =
	    m_asks.emplace(t_order->m_price, slob::order_level()).first;
	level_it->second.rest_order(t_order);
	t_order->m_level_it = level_it;
	t_order->m_state = order_state::resting;
	t_order->on_resting();

	// trigger ask down triggers
	trigger_down(t_order->m_price, m_ask_down_triggers);
}

void slob::book::insert_bid(slob::c_order_ptr &t_order) {

	if (bid_min_quantity_fillable(t_order)) {
		t_order->m_state = order_state::matching;
		match_bid(t_order);
	} else {
		// all orders with positive minimum quantities are IOC
		t_order->m_state = order_state::canceled;
		t_order->on_canceled();

		// on_canceled may change order state
		if(t_order->m_state == order_state::canceled) {
			t_order->m_book = nullptr;
		}
		return;
	}

	if (t_order->m_state != order_state::matching) {
		return;
	}

	if (t_order->m_immediate_or_cancel) {
		t_order->m_state = order_state::canceled;
		t_order->on_canceled();

		// on_canceled may change order state
		if(t_order->m_state == order_state::canceled) {
			t_order->m_book = nullptr;
		}
		return;
	}

	// order is guaranteed to not be filled

	rest_bid(t_order);
}

void slob::book::insert_ask(slob::c_order_ptr &t_order) {

	if (ask_min_quantity_fillable(t_order)) {
		t_order->m_state = order_state::matching;
		match_ask(t_order);
	} else {
		// all orders with positive minimum quantities are IOC
		t_order->m_state = order_state::canceled;
		t_order->on_canceled();

		// on_canceled may change order state
		if(t_order->m_state == order_state::canceled) {
			t_order->m_book = nullptr;
		}
		return;
	}

	if (t_order->m_state != order_state::matching) {
		return;
	}

	if (t_order->m_immediate_or_cancel) {
		t_order->m_state = order_state::canceled;
		t_order->on_canceled();

		// on_canceled may change order state
		if(t_order->m_state == order_state::canceled) {
			t_order->m_book = nullptr;
		}

		return;
	}

	// order is guaranteed to not be filled
	rest_ask(t_order);
}


bool slob::book::bid_min_quantity_fillable(slob::c_order_ptr &t_order) const {
	std::int64_t min_quantity_remaining = t_order->m_min_quantity;

	if (min_quantity_remaining <= 0) {
		return true;
	}

	const std::int64_t order_price = t_order->m_price;

	for (auto level_it = m_asks.begin();
	    level_it != m_asks.end() && level_it->first <= order_price;
	    ++level_it) {
		const std::int64_t level_quantity = level_it->second.m_quantity;
		min_quantity_remaining -= level_quantity;

		if (min_quantity_remaining <= 0) {
			return true;
		}
	}

	return false;
}

bool slob::book::ask_min_quantity_fillable(slob::c_order_ptr &t_order) const {
	std::int64_t min_quantity_remaining = t_order->m_min_quantity;

	if (min_quantity_remaining <= 0) {
		return true;
	}

	const std::int64_t order_price = t_order->m_price;

	for (auto level_it = m_bids.begin();
	    level_it != m_bids.end() && level_it->first >= order_price;
	    ++level_it) {
		const std::int64_t level_quantity = level_it->second.m_quantity;
		min_quantity_remaining -= level_quantity;

		if (min_quantity_remaining <= 0) {
			return true;
		}
	}

	return false;
}

void slob::book::match_bid(slob::c_order_ptr &t_order) {
	const std::int64_t order_price = t_order->m_price;

	auto level_it = m_asks.begin();
	while (level_it != m_asks.end() && level_it->first <= order_price) {
		const std::int64_t prev_market_price = m_market_price;
		m_market_price = level_it->first;
		level_it->second.match(t_order);

		if(m_market_price > prev_market_price) {
			// trigger market up triggers
			trigger_up(m_market_price, m_market_up_triggers);
		} else if(m_market_price < prev_market_price) {
			// trigger market down triggers
			trigger_down(m_market_price, m_market_down_triggers);
		}

		if (level_it->second.is_empty()) {
			m_asks.erase(level_it++);
			// trigger ask up triggers
			trigger_up(get_ask_price(), m_ask_up_triggers); 
		} else {
			// if price level not fully consumed, the order was
			// filled
			break;
		}

		// status may have changed to filled or pending
		if (t_order->m_state != order_state::matching) {
			// level is empty and order is filled (rare)
			break;
		}
	}
}

void slob::book::match_ask(slob::c_order_ptr &t_order) {
	const std::int64_t order_price = t_order->m_price;

	auto level_it = m_bids.begin();
	while (level_it != m_bids.end() && level_it->first >= order_price) {
		const std::int64_t prev_market_price = m_market_price;
		m_market_price = level_it->first;
		level_it->second.match(t_order);

		if(m_market_price > prev_market_price) {
			// trigger market up triggers
			trigger_up(m_market_price, m_market_up_triggers);
		} else if(m_market_price < prev_market_price) {
			// trigger market down triggers
			trigger_down(m_market_price, m_market_down_triggers);
		}

		if (level_it->second.is_empty()) {
			m_bids.erase(level_it++);
			// trigger bid down triggers
			trigger_down(get_bid_price(), m_bid_down_triggers); 
		} else {
			// if price level not fully consumed, the order was
			// filled
			break;
		}

		// status may have changed to filled or pending
		if (t_order->m_state != order_state::matching) {
			// level is empty and order is filled (rare)
			break;
		}
	}
}

std::int64_t slob::book::get_bid_quantity() const { return m_bid_quantity; }

std::int64_t slob::book::get_ask_quantity() const { return m_ask_quantity; }

std::int64_t slob::book::get_bid_price() const {
	const auto it = m_bids.begin();
	return it != m_bids.end() ? it->first : slob::min_price;
}

std::int64_t slob::book::get_ask_price() const {
	const auto it = m_asks.begin();
	return it != m_asks.end() ? it->first : slob::max_price;
}

std::size_t slob::book::get_bid_level_count() const {
	return m_bids.size();
}

std::size_t slob::book::get_ask_level_count() const {
	return m_asks.size();
}

std::int64_t slob::book::get_market_price() const { return m_market_price; }

std::map<std::int64_t, slob::order_level>::iterator slob::book::bid_levels_begin() {
	return m_bids.begin();
}

std::map<std::int64_t, slob::order_level>::iterator slob::book::ask_levels_begin() {
	return m_asks.begin();
}

std::map<std::int64_t, slob::order_level>::iterator slob::book::bid_levels_end() {
	return m_bids.end();
}

std::map<std::int64_t, slob::order_level>::iterator slob::book::ask_levels_end() {
	return m_asks.end();
}

std::map<std::int64_t, slob::order_level>::iterator slob::book::bid_level_at(
    const std::int64_t t_price) {
	return m_bids.find(t_price);
}

std::map<std::int64_t, slob::order_level>::iterator slob::book::ask_level_at(
    const std::int64_t t_price) {
	return m_asks.find(t_price);
}

slob::bid_order_iterator slob::book::bid_orders_begin() {
	return slob::bid_order_iterator(
	    m_bids, m_bids.begin(), m_bids.begin()->second.begin());
}

slob::ask_order_iterator slob::book::ask_orders_begin() {
	return slob::ask_order_iterator(
	    m_asks, m_asks.begin(), m_asks.begin()->second.begin());
}

slob::bid_order_iterator slob::book::bid_orders_end() {
	return slob::bid_order_iterator(m_bids, m_bids.end());
}

slob::ask_order_iterator slob::book::ask_orders_end() {
	return slob::ask_order_iterator(m_asks, m_asks.end());
}

slob::book::~book() {
	m_bids.clear();
	m_asks.clear();

	m_bid_up_triggers.clear();
	m_ask_up_triggers.clear();
	m_market_up_triggers.clear();
	m_bid_down_triggers.clear();
	m_ask_down_triggers.clear();
	m_market_down_triggers.clear();
}

#endif // BOOK_HPP
