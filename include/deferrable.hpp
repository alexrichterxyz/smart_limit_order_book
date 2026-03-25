#ifndef DEFERRABLE_HPP
#define DEFERRABLE_HPP
#include <memory>
#include <variant>
#include <cstdint>

namespace slob {
class order;

struct insert_order_deferrable {
	const std::shared_ptr<slob::order> order;

	insert_order_deferrable(const std::shared_ptr<slob::order> &t_order)
	    : order(t_order) {};
};

struct cancel_order_deferrable {
	const std::shared_ptr<slob::order> order;

	cancel_order_deferrable(const std::shared_ptr<slob::order> &t_order)
	    : order(t_order) {};
};

struct set_price_deferrable {
	const std::shared_ptr<slob::order> order;
	const std::int64_t price;

	set_price_deferrable(const std::shared_ptr<slob::order> &t_order,
	    const std::int64_t t_price)
	    : order(t_order), price(t_price) {};
};

struct set_quantity_deferrable {
	const std::shared_ptr<slob::order> order;
	const std::int64_t quantity;

	set_quantity_deferrable(const std::shared_ptr<slob::order> &t_order,
	    const std::int64_t t_quantity)
	    : order(t_order), quantity(t_quantity) {};
};


enum deferrable_type {
	insert_order = 0,
	cancel_order,
	set_price,
	set_quantity,
};

class deferrable {
	private:
	const std::variant<insert_order_deferrable, cancel_order_deferrable, set_price_deferrable, set_quantity_deferrable>
	    m_obj;

	public:
	template <class T> deferrable(const T &obj);

	const insert_order_deferrable &get_insert_order_deferrable() const;

	const set_price_deferrable &get_set_price_deferrable() const;

	const set_quantity_deferrable &get_set_quantity_deferrable() const;

	const cancel_order_deferrable &get_cancel_order_deferrable() const;

	inline deferrable_type type() const;
};

} // namespace slob

template <class T> slob::deferrable::deferrable(const T &obj) : m_obj(obj){};

slob::deferrable_type slob::deferrable::type() const {
	return static_cast<slob::deferrable_type>(m_obj.index());
}

const slob::insert_order_deferrable &
slob::deferrable::get_insert_order_deferrable() const {
	return std::get<insert_order_deferrable>(m_obj);
}

const slob::cancel_order_deferrable &
slob::deferrable::get_cancel_order_deferrable() const {
	return std::get<cancel_order_deferrable>(m_obj);
}

const slob::set_price_deferrable &
slob::deferrable::get_set_price_deferrable() const {
	return std::get<set_price_deferrable>(m_obj);
}

const slob::set_quantity_deferrable &
slob::deferrable::get_set_quantity_deferrable() const {
	return std::get<set_quantity_deferrable>(m_obj);
}

#endif // #ifndef DEFERRABLE_HPP