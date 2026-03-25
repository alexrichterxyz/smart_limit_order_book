#ifndef TRAILING_STOP_HPP
#define TRAILING_STOP_HPP
#include "order.hpp"
#include "trigger.hpp"
#include <cstdint>
#include <memory>

namespace slob {

class trailing_stop;

class trailing_stop_trigger : virtual public trigger {
	private:
	std::shared_ptr<slob::order> m_order;

	void on_triggered() override;

	public:
	trailing_stop_trigger(const direction t_follow_direction,
	    const std::shared_ptr<order> &t_order);

	friend trailing_stop;
};

class trailing_stop : virtual public trigger {
	private:
	std::shared_ptr<slob::trailing_stop_trigger> m_trigger;
	std::int64_t m_offset;

	void on_triggered() override;
	void on_accepted() override;

	public:
	trailing_stop(const direction t_follow_direction,
	    const std::int64_t t_offset, const std::shared_ptr<order> &t_order);

	inline const std::shared_ptr<order> &get_order() const;
};

} // namespace slob

slob::trailing_stop::trailing_stop(const slob::direction t_follow_direction,
    const std::int64_t t_offset, const std::shared_ptr<order> &t_order)
    : trigger(t_follow_direction == direction::up ? trigger_type::market_up
						  : trigger_type::market_down,
	  t_follow_direction == direction::up ? slob::max_price
					      : slob::min_price),
      m_trigger(
	  std::make_shared<trailing_stop_trigger>(t_follow_direction, t_order)),
      m_offset(t_offset) {}

slob::trailing_stop_trigger::trailing_stop_trigger(
    const slob::direction t_follow_direction,
    const std::shared_ptr<order> &t_order)
    : trigger(t_follow_direction == direction::up ? trigger_type::market_down
						  : trigger_type::market_up,
	  t_follow_direction == direction::up ? slob::min_price
					      : slob::max_price),
      m_order(t_order) {}

void slob::trailing_stop::on_accepted() {
	slob::book *book = get_book();
	const std::int64_t market_price = book->get_market_price();
	std::int64_t offset_price = 0;

	if (get_type() == slob::trigger_type::market_up) {
		set_price(market_price + 1);
		offset_price = market_price - m_offset;

	} else {
		set_price(market_price - 1);
		offset_price = market_price + m_offset;
	}

	m_trigger->set_price(offset_price);
	book->insert(m_trigger);
}

void slob::trailing_stop::on_triggered() {
	if (m_trigger->get_state() != trigger_state::resting) {
		return;
	}

	slob::book *book = get_book();
	const std::int64_t market_price = book->get_market_price();
	std::int64_t offset_price = 0;

	if (get_type() == slob::trigger_type::market_up) {
		set_price(market_price + 1);
		offset_price = market_price - m_offset;
        
	} else {
		set_price(market_price - 1);
		offset_price = market_price + m_offset;
	}

	m_trigger->set_price(offset_price);

	book->insert(shared_from_this());
}

const std::shared_ptr<slob::order> &slob::trailing_stop::get_order() const {
	return m_trigger->m_order;
};

void slob::trailing_stop_trigger::on_triggered() { get_book()->insert(m_order); }

#endif // #ifndef TRAILING_STOP_HPP