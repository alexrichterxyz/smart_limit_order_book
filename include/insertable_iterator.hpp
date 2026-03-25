#ifndef INSERTABLE_ITERATOR_HPP
#define INSERTABLE_ITERATOR_HPP
#include <list>
#include <map>
#include <cstdint>

namespace slob {
class book;
template <class Cmp, class Lim, class Ins> class insertable_iterator {
	private:
	std::map<std::int64_t, Lim, Cmp> &m_side;
	typename std::map<std::int64_t, Lim, Cmp>::iterator m_level_it;
	typename std::list<Ins>::iterator m_insertable_it;

	insertable_iterator(std::map<std::int64_t, Lim, Cmp> &t_side,
	    const typename std::map<std::int64_t, Lim, Cmp>::iterator &t_level_it,
	    const typename std::list<Ins>::iterator &t_insertable_it);

	insertable_iterator(std::map<std::int64_t, Lim, Cmp> &t_side,
	    const typename std::map<std::int64_t, Lim, Cmp>::iterator &t_level_it);

	public:
	insertable_iterator(const insertable_iterator &t_other);

	bool operator==(const insertable_iterator &t_other);

	bool operator!=(const insertable_iterator &t_other);

	insertable_iterator<Cmp, Lim, Ins> operator++();
	insertable_iterator<Cmp, Lim, Ins> operator++(int);

	typename std::list<Ins>::iterator operator->();
	Ins &operator*();

	friend book;
};

} // namespace slob

#include <functional>
#include <iostream>

template <class Cmp, class Lim, class Ins>
slob::insertable_iterator<Cmp, Lim, Ins>::insertable_iterator(
    std::map<std::int64_t, Lim, Cmp> &t_side,
    const typename std::map<std::int64_t, Lim, Cmp>::iterator &t_level_it,
    const typename std::list<Ins>::iterator &t_insertable_it)
    : m_side(t_side), m_level_it(t_level_it), m_insertable_it(t_insertable_it) {}

template <class Cmp, class Lim, class Ins>
slob::insertable_iterator<Cmp, Lim, Ins>::insertable_iterator(
    std::map<std::int64_t, Lim, Cmp> &t_side,
    const typename std::map<std::int64_t, Lim, Cmp>::iterator &t_level_it)
    : m_side(t_side), m_level_it(t_level_it) {}

template <class Cmp, class Lim, class Ins>
slob::insertable_iterator<Cmp, Lim, Ins>::insertable_iterator(
    const insertable_iterator<Cmp, Lim, Ins> &t_other)
    : m_side(t_other.m_side), m_level_it(t_other.m_level_it),
      m_insertable_it(t_other.m_insertable_it) {}

template <class Cmp, class Lim, class Ins>
bool slob::insertable_iterator<Cmp, Lim, Ins>::operator==(
    const insertable_iterator<Cmp, Lim, Ins> &t_other) {

	return &m_side == &t_other.m_side
    && m_level_it == t_other.m_level_it
    && (m_level_it == m_side.end() || m_insertable_it == t_other.m_insertable_it);
}

template <class Cmp, class Lim, class Ins>
bool slob::insertable_iterator<Cmp, Lim, Ins>::operator!=(
    const insertable_iterator<Cmp, Lim, Ins> &t_other) {

	return !(*this == t_other);
}

template <class Cmp, class Lim, class Ins>
slob::insertable_iterator<Cmp, Lim, Ins>
slob::insertable_iterator<Cmp, Lim, Ins>::operator++() {
	if (m_level_it != m_side.end()) {
		if (m_insertable_it != std::prev(m_level_it->second.end())) {
			++m_insertable_it;
		} else if (++m_level_it != m_side.end()) {
			m_insertable_it = m_level_it->second.begin();
		}
	}

	return *this;
}

template <class Cmp, class Lim, class Ins>
slob::insertable_iterator<Cmp, Lim, Ins>
slob::insertable_iterator<Cmp, Lim, Ins>::operator++(int) {
	auto pre_increment_copy = *this;

	if (m_level_it != m_side.end()) {
		if (m_insertable_it != std::prev(m_level_it->second.end())) {
			++m_insertable_it;
		} else if (++m_level_it != m_side.end()) {
			m_insertable_it = m_level_it->second.begin();
		}
	}

	return pre_increment_copy;
}

template <class Cmp, class Lim, class Ins>
typename std::list<Ins>::iterator
slob::insertable_iterator<Cmp, Lim, Ins>::operator->() {
	return m_insertable_it;
}

template <class Cmp, class Lim, class Ins>
Ins &slob::insertable_iterator<Cmp, Lim, Ins>::operator*() {
	return *m_insertable_it;
}

#endif // #ifndef INSERTABLE_ITERATOR_HPP