#ifndef ORDER_HPP
#define ORDER_HPP
#include "common.hpp"
#include <list>
#include <map>
#include <memory>
#include <cstdint>

namespace slob {

class order_level;
class book;

/**
 * @brief the order class defines the fundamental properties of orders
 * including side, price, quantity, and whether the order is immediate
 * or cancel, or all or nothing. Market orders are represented as level
 * orders with a price of zero (sell) or infinity (buy). Objects of this
 * class can be inserted into book objects. The behavior of orders can
 * be customized by overriding the virtual event methods: on_resting,
 * on_accepted, on_before_match, on_canceled, on_filled, on_deferred_set_quantity,
 * on_deferred_set_price, on_deferred_cancel.
 *
 */
class order : public std::enable_shared_from_this<order> {
	private:
	side m_side;
	std::int64_t m_price;
	std::int64_t m_quantity = 0;
	std::int64_t m_min_quantity = 0;
	bool m_immediate_or_cancel = false;
	order_state m_state = order_state::pending;

	/* pointer to the book into which the order was inserted.
		it's guaranteed to be dereferencable in the virtual
	   event methods. */
	book *m_book = nullptr;

	/* these iterators store the location of the order in the order
		book. They are used to cancel the order in O(1). */
	std::map<std::int64_t, slob::order_level>::iterator m_level_it;
	std::list<order_ptr>::iterator m_order_it;

	inline bool set_quantity_with_options(const std::int64_t t_quantity, const bool t_enable_deferral);

	inline bool cancel_with_options(const bool t_enable_deferral);

	// todo: add documentation
	inline bool set_price_with_options(const std::int64_t price, const bool t_enable_deferral);

	// returns bool to indicate whether bbo changed
	inline bool remove_from_book();

	protected:
	/**
	 * @brief At this stage the order has been verified to be
	 * valid and is awaiting execution. This event method can be
	 * used to adapt the price of the order to the current market
	 * price of the book.
	 *
	 */
	virtual void on_accepted() {};

	/**
	 * @brief Called once the order has been resting at the specified
	 * price level.
	 *
	 */
	virtual void on_resting() {};

	/**
	 * @brief Called before the order is executed against another one.
	 *
	 * @param t_order the other order
	 */
	virtual void on_before_match(c_corder_ptr &t_order) {};

	// todo: add documentation
	/**
	 * @brief Called after the order was involved in a trade.
	 *
	 * @param t_order the other order
	 */
	virtual void on_filled() {};

	// todo: say that book may be null_ptr
	/**
	 * @brief Called once the order got canceled. This may happen if
	 * the order got canceled manually or if the order is immediate
	 * or cancel and could not get filled immediately.
	 *
	 */
	virtual void on_canceled() {};

	// todo: documentation
	virtual bool on_deferred_cancel() { return true; };

	virtual bool on_deferred_set_price(const std::int64_t t_price) {
		return true;
	};

	// todo: documentation
	virtual bool on_deferred_set_quantity(const std::int64_t t_quantity) {
		return true;
	};

	public:
	/**
	 * @brief Construct a new order object
	 *
	 * @param t_side the side at which the order will be inserted
	 * (either slob::side::bid or slob::side::ask)
	 * @param t_price the price at which the order will be inserted.
	 * For market orders this will be 0 (sell) or DBL_MAX (buy)
	 * @param t_quantity the quantity demanded or provided.
	 * @param t_min_quantity the minimum quantity of the order
	 * @param t_immediate_or_cancel indicator of whether the order
	 * is immediate or cancel.
	 */
	order(const side t_side, const std::int64_t t_price, const std::int64_t t_quantity,
	    const std::int64_t t_min_quantity = 0,
	    const bool t_immediate_or_cancel = false);

	

	/**
	 * @brief Cancels the order, if possible. Currently, only resting
	 * orders can be canceled.
	 *
	 * @return true successfully cancelled.
	 * @return false could not cancel the order because it hasn't
	 * been resting yet.
	 */
	inline bool cancel();

	/**
	 * @brief Get the instance of the book into which the order was
	 * inserted. This value is guaranteed to be non-nullptr in the
	 * virtual event methods.
	 *
	 * @return book* pointer to the book object into which the order
	 * was inserted or nullptr if it hasn't been inserted into a
	 * book yet or got removed from it.
	 */
	inline book *get_book() const;

	/**
	 * @brief Get the side of the order.
	 *
	 * @return either slob::side::bid or slob::side::ask.
	 */
	inline side get_side() const;

	/**
	 * @brief Get the price of the order.
	 *
	 * @return std::int64_t price of the order.
	 */
	inline std::int64_t get_price() const;

	/**
	 * @brief Get the quantity of the order.
	 *
	 * @return the quantity of the order.
	 */
	inline std::int64_t get_quantity() const;

	/**
	 * @brief Get the minimum quantity of the order.
	 *
	 * @return the minimum quantity of the order.
	 */
	inline std::int64_t get_min_quantity() const;

	/**
	 * @brief Update the quantity of the order. This operation is
	 * O(1) in some cases but can be very inefficient if there are
	 * lot of all or nothing orders in the book.
	 *
	 * @param t_quantity
	 * @return true successfully set the quantity
	 * @return false could not set the quantity
	 */
	inline bool set_quantity(const std::int64_t t_quantity);

	// todo: add documentation
	/**
	 * @brief Update the minimum quantity of the order.
	 *
	 * @param t_min_quantity
	 * @return true successfully set the minimum quantity
	 * @return false could not set the minimum quantit
	 */
	inline bool set_min_quantity(const std::int64_t t_min_quantity);

	// todo: add documentation
	inline bool set_immediate_or_cancel(const bool t_ioc);

	// todo: add documentation
	inline bool set_side(const side t_side);

	// todo: add documentation
	inline bool set_price(const std::int64_t price);

	/**
	 * @brief Check if the order is immediate or cancel.
	 *
	 * @return true, is immediate or cancel.
	 * @return false, is not immediate or cancel.
	 */
	inline bool is_immediate_or_cancel() const;

	/**
	 * @brief Check if the order is all or nothing.
	 *
	 * @return true, is all or nothing.
	 * @return false, is not all or nothing.
	 */
	inline bool is_all_or_nothing() const;

	// todo: add documentation
	inline bool is_closed() const;

	// todo: add documentation
	inline order_state get_state() const;

	inline bool reset();

	virtual ~order() = default;

	friend book;
	friend order_level;
};

} // namespace slob

#include "book.hpp"
#include "deferrable.hpp"
#include "order_level.hpp"
#include <algorithm>

slob::order::order(const slob::side t_side, const std::int64_t t_price,
    const std::int64_t t_quantity, const std::int64_t t_min_quantity,
    const bool t_immediate_or_cancel)
    : m_side(t_side), m_price(t_price), m_quantity(t_quantity),
      m_min_quantity(t_min_quantity),
      m_immediate_or_cancel(t_immediate_or_cancel) {}

bool slob::order::cancel_with_options(const bool t_enable_deferral) {
	switch (m_state) {
		case order_state::filled:
		case order_state::canceled:
		case order_state::pending:
			return false;
		case order_state::accepted:
			m_state = order_state::canceled;
			on_canceled();

			// on_canceled may reinsert order
			if(m_state == order_state::canceled) {
				m_book = nullptr;
			}
			
			return true;
		default:
			break;
	}

	// state is matching or resting
	if(t_enable_deferral && m_book->is_deferring()) {
		m_book->m_deferred.push(cancel_order_deferrable(shared_from_this()));
		return true;
	}

	// state is resting

	/* if the bbo changed, we may have to execute some triggers
	however, on_canceled may insert a new order immediately at the original BBO
	so that trigger_bids_down would not detect the temporary change in bids.
	By wrapping this code in begin_deferral and end_deferral, we can ensure that
	any order inserted by on_canceled is deferred until after the triggers have been
	triggered.
	*/ 
	m_book->begin_deferral();

	// remove order from book
	const bool bbo_did_change = remove_from_book();
	m_state = order_state::canceled;
	
	// on_cancaled may reset order and change side or book
	const auto original_side = m_side;
	const auto original_book = m_book;

	on_canceled();

	// on_canceled may reinsert order
	if(m_state == order_state::canceled) {
		m_book = nullptr;
	}
	
	if(bbo_did_change) {
		if(original_side == side::bid) {
			// trigger bid down triggers
			original_book->trigger_down(original_book->get_bid_price(), original_book->m_bid_down_triggers);
		} else {
			// trigger ask up triggers
			original_book->trigger_up(original_book->get_ask_price(), original_book->m_ask_up_triggers);
		}
	}
	
	original_book->end_deferral();
	
	return true;
}

bool slob::order::remove_from_book() {
	bool bbo_did_change = false;

	if(m_side == side::bid) {
		m_book->m_bid_quantity -= m_quantity;
		m_level_it->second.m_orders.erase(m_order_it);
		m_level_it->second.m_quantity -= m_quantity;

		if(m_level_it->second.m_orders.empty()) {
			bbo_did_change = m_level_it == m_book->m_bids.begin();
			m_book->m_bids.erase(m_level_it);
		}
	} else {
		m_book->m_ask_quantity -= m_quantity;
		m_level_it->second.m_orders.erase(m_order_it);
		m_level_it->second.m_quantity -= m_quantity;

		if(m_level_it->second.m_orders.empty()) {
			bbo_did_change = m_level_it == m_book->m_asks.begin();
			m_book->m_asks.erase(m_level_it);
		}
	}

	return bbo_did_change;
}

bool slob::order::cancel() {
	return cancel_with_options(true);
}

bool slob::order::set_quantity(const std::int64_t t_quantity) {
	return set_quantity_with_options(t_quantity, true);
}

bool slob::order::set_quantity_with_options(const std::int64_t t_quantity, const bool t_enable_deferral) {
	if (t_quantity <= 0 || t_quantity < m_min_quantity) {
		return false;
	}

	switch (m_state) {
		case order_state::filled:
		case order_state::canceled:
			return false;
		case order_state::pending:
		case order_state::accepted:
			m_quantity = t_quantity;
			return true;
		default:
			break;
	}

	// state is either inserterting or resting
	
	if(t_enable_deferral && m_book->is_deferring()) {
		m_book->m_deferred.push(set_quantity_deferrable(
			shared_from_this(),
			t_quantity
		));

		return true;
	} 

	// state is resting

	const std::int64_t quantity_change = t_quantity - m_quantity;
	m_quantity = t_quantity;
	m_level_it->second.m_quantity += quantity_change;

	if(m_side == slob::side::bid) {
		m_book->m_bid_quantity += quantity_change;
	} else {
		m_book->m_ask_quantity += quantity_change;
	}
	
	// if quantity is increased, the priority is changed
	// todo: come back here
	if(quantity_change > 0 && std::prev(m_level_it->second.m_orders.end()) != m_order_it) {
		m_level_it->second.m_orders.erase(m_order_it);
		m_level_it->second.m_orders.push_back(shared_from_this());
		m_order_it = std::prev(m_level_it->second.m_orders.end());
	}
		
	return true;
}


bool slob::order::set_min_quantity(const std::int64_t t_min_quantity) {
	switch (m_state) {
	case order_state::matching:
	case order_state::resting:
	case order_state::canceled:
	case order_state::filled:
		return false;
	default:
		break;
	}

	m_min_quantity = t_min_quantity;
	return true;
}

bool slob::order::set_immediate_or_cancel(const bool t_ioc) {
	// unable to set IOC if one of these states
	switch (m_state) {
	case order_state::resting:
	case order_state::matching:
	case order_state::canceled:
	case order_state::filled:
		return false;
	default:
		break;
	}

	if (t_ioc) {
		m_immediate_or_cancel = true;
		return true;
	}

	// disable immediate or cancel

	if (m_min_quantity > 0) {
		return false;
	}

	m_immediate_or_cancel = false;
	return true;
}

bool slob::order::set_side(const slob::side t_side) {
	// unable to set side if one of these states
	switch (m_state) {
	case order_state::matching:
	case order_state::resting:
	case order_state::canceled:
	case order_state::filled:
		return false;
	default:
		break;
	}

	m_side = t_side;
	return true;
}

bool slob::order::set_price(const std::int64_t t_price) {
	return set_price_with_options(t_price, true);
}

bool slob::order::set_price_with_options(const std::int64_t t_price, const bool t_enable_deferral=true) {
	// unable to set side if one of these states

	switch (m_state) {
		case order_state::canceled:
		case order_state::filled: {
			return false;
		}
		case order_state::pending:
		case order_state::accepted: 
			m_price = t_price;
			return true;
		default:
			break;
	}

	// state is inserting or resting
	if(t_enable_deferral && m_book->is_deferring()) {
		m_book->m_deferred.push(set_price_deferrable(
			shared_from_this(),
			t_price
		));
		return true;
	}

	// put this check below deferral so that traders can queue "price resets"
	if(t_price == m_price) {
		return true;
	}

	const bool bbo_did_change = remove_from_book();

	const int64_t original_price = m_price;
	m_price = t_price;
	m_state = order_state::accepted;
	m_book->begin_deferral();
	
	// todo: potentially optimize, the new price tells us if matching is neccessay so can avoid check against BBO
	if (m_side == side::bid) {
		if(bbo_did_change && original_price > t_price) {
			m_book->trigger_down(m_book->get_bid_price(), m_book->m_bid_down_triggers);
		}

		m_book->insert_bid(shared_from_this());
	} else {
		if(bbo_did_change && original_price < t_price) {
			m_book->trigger_up(m_book->get_ask_price(), m_book->m_ask_up_triggers);
		}

		m_book->insert_ask(shared_from_this());
	}

	m_book->end_deferral();

	return true;
}

slob::book *slob::order::get_book() const { return m_book; }

slob::side slob::order::get_side() const { return m_side; }

std::int64_t slob::order::get_price() const { return m_price; }

std::int64_t slob::order::get_quantity() const { return m_quantity; }

std::int64_t slob::order::get_min_quantity() const { return m_min_quantity; }

bool slob::order::is_immediate_or_cancel() const {
	return m_immediate_or_cancel || m_min_quantity > 0;
}

bool slob::order::is_all_or_nothing() const {
	return m_quantity == m_min_quantity;
}

bool slob::order::is_closed() const {
	switch(m_state) {
		case order_state::canceled:
		case order_state::filled:
		return true;
		default:
		break;
	}

	return false;
}

bool slob::order::reset() {
	switch(m_state) {
		case order_state::canceled:
		case order_state::filled:
			m_state = order_state::pending;
			return true;
		default:
			return false;
	}
}

slob::order_state slob::order::get_state() const { return m_state; }

#endif // #ifndef ORDER_HPP