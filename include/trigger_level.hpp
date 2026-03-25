#ifndef TRIGGER_LEVEL_HPP
#define TRIGGER_LEVEL_HPP
#include <list>
#include <memory>

namespace slob {
class trigger;
class book;

class trigger_level {
	private:
	std::list<trigger_ptr> m_triggers;

	std::list<trigger_ptr>::iterator insert(c_trigger_ptr &t_trigger);

	inline bool is_empty() const { return m_triggers.empty(); }

	void erase(const std::list<trigger_ptr>::iterator &t_trigger_it);
	void trigger_all();

	public:
	/**
	 * @brief Get an iterator to the first trigger in the queue.
	 *
	 * @return std::list<slob::trigger_ptr>::iterator,
	 * iterator to first trigger in the queue.
	 */
	inline std::list<trigger_ptr>::iterator begin();

	/**
	 * @brief Get an iterator to the end of the trigger queue.
	 *
	 * @return std::list<slob::trigger_ptr>::iterator,
	 * iterator to the end of the trigger queue.
	 */
	inline std::list<trigger_ptr>::iterator end();

	/**
	 * @brief Get the number of triggers at this price level.
	 *
	 * @return the number of tiggers at this price level.
	 */
	inline std::size_t trigger_count() const;

	friend book;
	friend trigger;

	~trigger_level();
};

} // namespace slob

std::list<slob::trigger_ptr>::iterator slob::trigger_level::insert(
    slob::c_trigger_ptr &t_trigger) {
	m_triggers.push_back(t_trigger);
	return std::prev(m_triggers.end());
}

void slob::trigger_level::erase(
    const std::list<slob::trigger_ptr>::iterator &t_trigger_it) {
	auto trigger_obj = *t_trigger_it;
	m_triggers.erase(t_trigger_it);
}

void slob::trigger_level::trigger_all() {

	while (!m_triggers.empty()) {
		auto trigger_obj = m_triggers.front();
		m_triggers.pop_front();
		trigger_obj->m_state = trigger_state::triggered;
		trigger_obj->on_triggered();

		 // on_triggered may reinsert the trigger
		if (trigger_obj->m_state == trigger_state::triggered) {
			trigger_obj->m_book = nullptr;
		}
	}
}

slob::trigger_level::~trigger_level() {
	for (const auto &trigger : m_triggers) {
		trigger->m_book = nullptr;
		trigger->m_state = trigger_state::canceled;
	}
}

std::list<slob::trigger_ptr>::iterator slob::trigger_level::begin() {
	return m_triggers.begin();
}

inline std::list<slob::trigger_ptr>::iterator slob::trigger_level::end() {
	return m_triggers.end();
}

inline std::size_t slob::trigger_level::trigger_count() const {
	return m_triggers.size();
}

#endif // #ifndef TRIGGER_LEVEL_HPP