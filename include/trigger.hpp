#ifndef TRIGGER_HPP
#define TRIGGER_HPP
#include "common.hpp"
#include <list>
#include <map>
#include <memory>
#include <cstdint>

namespace slob {

class trigger_level;
class book;



/**
 * @brief An object of class trigger is essentially an event handler
 * that gets triggered once the market price rises above or falls below
 * the specified price. Triggers inserted on the bid side respond to
 * falling prices whereas triggers inserted on the ask side respond to
 * rising prices. Triggers are essential building blocks for
 * market-price-based order types such as stop orders. Just like order
 * objects, triggers can be inserted into the order book. Two major
 * differences between triggers and order objects are: triggers can move
 * themselves to different price levels, triggers get inserted into the
 * order book immediately whereas orders may get deferred.
 *
 */
class trigger : public std::enable_shared_from_this<trigger> {
	private:
	const trigger_type m_type;
	std::int64_t m_price;
	trigger_state m_state = trigger_state::pending;

	/* pointer to the book into which the order was inserted.
		it's guaranteed to be dereferencable in the virtual
	   event methods. */
	book *m_book = nullptr;

	/* these iterators store the location of the order in the order
		book. They are used to cancel the order in O(1). */
	std::map<std::int64_t, trigger_level>::iterator m_level_it;
	std::list<trigger_ptr>::iterator m_trigger_it;

	inline void erase_trigger_level();

	protected:
	/**
	 * @brief At this stage the trigger has been verified to
	 * be valid and is about to be inserted. This event method can
	 * be used to adapt the price of the trigger to the current
	 * market price of the book.
	 *
	 */
	virtual void on_accepted() {};

	/**
	 * @brief Called once the trigger has been inserted into the book
	 */
	virtual void on_resting() {};

	/**
	 * @brief Triggers on the ask side get triggered if the market
	 * price (price of the last trade) rises above or reaches the
	 * specified price. Triggers inserted on the bid side are
	 * responsive to falling prices.
	 *
	 */
	virtual void on_triggered() {};

	/**
	 * @brief Called once the trigger got canceled.
	 *
	 */
	virtual void on_canceled() {};

	public:
	/**
	 * @brief Get the price of the order.
	 *
	 * @return std::int64_t price of the order.
	 */
	inline std::int64_t get_price() const;

	/**
	 * @brief Update the price of the trigger.
	 *
	 * @param t_price, the new price of the trigger.
	 */
	inline bool set_price(std::int64_t t_price);

	/**
	 * @brief Get the side of the trigger.
	 *
	 * @return either slob::side::bid or slob::side:ask
	 */
	inline trigger_state get_state() const;

	// todo: add documentation
	inline trigger_type get_type() const;

	/**
	 * @brief Construct a new trigger object
	 *
	 * @param t_side, either slob::side::bid or slob::side::ask. Bid
	 * triggers are responsive to falling market prices whereas ask
	 * triggers are responsive to rising market prices.
	 * @param t_price, the market price (price of last trade) at
	 * which the on_triggered method will be triggered.
	 */
	trigger(trigger_type t_type, std::int64_t t_price);

	/**
	 * @brief Get the instance of the book into which the TRIGGER
	 * was inserted. This value is guaranteed to be non-nullptr in
	 * the virtual event methods.
	 *
	 * @return book* pointer to the book object into which the
	 * trigger was inserted or nullptr if it hasn't been inserted
	 * into a book yet or got removed from it.
	 */
	inline book *get_book() const;

	/**
	 * @brief Cancels the trigger, if possible. Currently, only
	 * queued triggers can be canceled.
	 *
	 * @return true successfully cancelled.
	 * @return false could not cancel the trigger because it hasn't
	 * been queued yet.
	 */
	inline bool cancel();

	inline bool reset();

	virtual ~trigger() = default;

	friend book;
	friend trigger_level;
};
} // namespace slob

#include "book.hpp"
#include "trigger_level.hpp"

slob::trigger::trigger(slob::trigger_type t_type, std::int64_t t_price)
    : m_type(t_type), m_price(t_price) {}

void slob::trigger::erase_trigger_level() {
	switch(m_type) {
		case trigger_type::bid_up: {
			m_book->m_bid_up_triggers.erase(m_level_it);
			break;
		}
		case trigger_type::ask_up: {
			m_book->m_ask_up_triggers.erase(m_level_it);
			break;
		}
		case trigger_type::market_up: {
			m_book->m_market_up_triggers.erase(m_level_it);
			break;
		}
		case trigger_type::bid_down: {
			m_book->m_bid_down_triggers.erase(m_level_it);
			break;
		}
		case trigger_type::ask_down: {
			m_book->m_ask_down_triggers.erase(m_level_it);
			break;
		}
		case trigger_type::market_down: {
			m_book->m_market_down_triggers.erase(m_level_it);
			break;
		}
	}
}

bool slob::trigger::cancel() {
	switch(m_state){
		case trigger_state::canceled:
		case trigger_state::triggered:
		case trigger_state::pending: {
			return false;
		}
		
		case trigger_state::accepted: {
			m_state = trigger_state::canceled;
			on_canceled();

			// on_canceled may change the trigger state
			if(m_state == trigger_state::canceled) {
				m_book = nullptr;
			}
			
			return true;
		}
		default:
			break;
	}

	// state is resting

	m_level_it->second.m_triggers.erase(m_trigger_it);
		
	if(m_level_it->second.m_triggers.empty()) {
		erase_trigger_level();
	}

	m_state = trigger_state::canceled;
	on_canceled();

	// on_canceled may change the trigger state
	if(m_state == trigger_state::canceled) {
		m_book = nullptr;
	}
	
	return true;
}


bool slob::trigger::reset() {
	switch(m_state) {
		case slob::trigger_state::canceled:
		case slob::trigger_state::triggered: {
			m_state = slob::trigger_state::pending;
			return true;
		}
		default:
		return false;
	}
}

bool slob::trigger::set_price(std::int64_t t_price) {
	switch(m_state) {
		case trigger_state::canceled:
		case trigger_state::triggered: {
			return false;
		}
		case trigger_state::accepted:
		case trigger_state::pending: {
			m_price = t_price;
			return true;
		}
		default:
		break;
	}

	// state is resting
	m_level_it->second.erase(m_trigger_it);

	if (m_level_it->second.is_empty()) {
		erase_trigger_level();
	}

	m_price = t_price;
	m_state = trigger_state::pending;
	return m_book->insert(shared_from_this());
}

std::int64_t slob::trigger::get_price() const { return m_price; }

slob::trigger_state slob::trigger::get_state() const { return m_state; }

slob::trigger_type slob::trigger::get_type() const { return m_type; }

slob::book *slob::trigger::get_book() const { return m_book; }

#endif // #ifndef TRIGGER_HPP