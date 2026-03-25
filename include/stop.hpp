#ifndef STOP_HPP
#define STOP_HPP
#include "order.hpp"
#include "trigger.hpp"
#include <cstdint>

namespace slob {

class stop : virtual public trigger {
	private:
	std::shared_ptr<slob::order> m_order;

	void on_triggered() override;

	public:
	stop(const direction t_follow_direction, const int64_t t_price, const std::shared_ptr<order> &t_order);

	inline const std::shared_ptr<order> &get_order() const;
};

} // namespace slob

#include "book.hpp"

slob::stop::stop(const slob::direction t_follow_direction, const int64_t t_price, const std::shared_ptr<order> &t_order)
    : slob::trigger(t_follow_direction == slob::direction::up ? trigger_type::market_up : trigger_type::market_down, t_price), m_order(t_order) {}

void slob::stop::on_triggered() {
	get_book()->insert(m_order);
}

const std::shared_ptr<slob::order> &slob::stop::get_order() const {
	return m_order;
};

#endif // #ifndef STOP_HPP