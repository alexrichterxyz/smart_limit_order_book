#ifndef INSERTABLE_HPP
#define INSERTABLE_HPP
#include <memory>
#include <variant>

namespace slob {
class order;
class trigger;

class insertable {
	private:
	std::variant<std::shared_ptr<order>, std::shared_ptr<trigger>> m_obj;

	public:
	insertable(const std::shared_ptr<order> &order);
	insertable(const std::shared_ptr<trigger> &trigger);

	inline bool is_order() const;

	inline const std::shared_ptr<order> *get_order() const;

	inline const std::shared_ptr<trigger> *get_trigger() const;
};

} // namespace slob

slob::insertable::insertable(const std::shared_ptr<order> &order)
    : m_obj(order) {}

slob::insertable::insertable(const std::shared_ptr<trigger> &trigger)
    : m_obj(trigger) {}

bool slob::insertable::is_order() const { return m_obj.index() == 0; }

const std::shared_ptr<slob::order> *slob::insertable::get_order() const {
	return std::get_if<std::shared_ptr<slob::order>>(&m_obj);
}

const std::shared_ptr<slob::trigger> *slob::insertable::get_trigger() const {
	return std::get_if<std::shared_ptr<slob::trigger>>(&m_obj);
}

#endif // #ifndef INSERTABLE_HPP