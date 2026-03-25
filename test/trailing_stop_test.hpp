#ifndef TRAILING_STOP_TEST_HPP
#define TRAILING_STOP_TEST_HPP
#include "test.hpp"
#include "../include/slob.hpp"

class trailing_stop_test : public test {
	inline static bool test_up();
	inline static bool test_down();

	public:
	trailing_stop_test();
};

trailing_stop_test::trailing_stop_test() : test("trailing_stop_test") {
	add("test_up", test_up);
    add("test_down", test_down);
}

bool trailing_stop_test::test_up() {
	slob::book book;
    const auto order = std::make_shared<slob::order>(slob::side::bid, slob::max_price, 1);
    const auto stop_up = std::make_shared<slob::trailing_stop>(slob::direction::up, 5, order);

    book.insert(std::make_shared<slob::order>(slob::side::ask, 1000, 1));
    book.insert(std::make_shared<slob::order>(slob::side::bid, 1000, 1));
    // market price is now 1000

    book.insert(stop_up);

    if(order->get_state() != slob::order_state::pending) {
        throw std::runtime_error("Order should be pending");
    }

    book.insert(std::make_shared<slob::order>(slob::side::ask, 2000, 1));
    book.insert(std::make_shared<slob::order>(slob::side::bid, 2000, 1));

    // market price is now 2000
    // trigger should trigger at 1995

    if(order->get_state() != slob::order_state::pending) {
        throw std::runtime_error("Order should be pending");
    }

    book.insert(std::make_shared<slob::order>(slob::side::ask, 1995, 1));
    book.insert(std::make_shared<slob::order>(slob::side::bid, 1995, 1));
    
    // error
    if(order->get_state() == slob::order_state::pending) {
        throw std::runtime_error("Order should not be pending");
    }

	return true;
}


bool trailing_stop_test::test_down() {
	slob::book book;
    const auto order = std::make_shared<slob::order>(slob::side::bid, slob::max_price, 1);
    const auto stop_down = std::make_shared<slob::trailing_stop>(slob::direction::down, 5, order);

    book.insert(std::make_shared<slob::order>(slob::side::ask, 1000, 1));
    book.insert(std::make_shared<slob::order>(slob::side::bid, 1000, 1));
    // market price is now 1000

    book.insert(stop_down);

    if(order->get_state() != slob::order_state::pending) {
        throw std::runtime_error("Order should be pending");
    }

    book.insert(std::make_shared<slob::order>(slob::side::ask, 500, 1));
    book.insert(std::make_shared<slob::order>(slob::side::bid, 500, 1));

    // market price is now 500
    // trigger should trigger at 505

    if(order->get_state() != slob::order_state::pending) {
        throw std::runtime_error("Order should be pending");
    }

    book.insert(std::make_shared<slob::order>(slob::side::ask, 505, 1));
    book.insert(std::make_shared<slob::order>(slob::side::bid, 505, 1));
    
    if(order->get_state() == slob::order_state::pending) {
        throw std::runtime_error("Order should not be pending");
    }

	return true;
}

#endif // #ifndef TRAILING_STOP_TEST_HPP