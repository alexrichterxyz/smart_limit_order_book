#ifndef STOP_TEST_HPP
#define STOP_TEST_HPP
#include "test.hpp"
#include "../include/slob.hpp"

class stop_test : public test {
	inline static bool test_up();
	inline static bool test_down();

	public:
	stop_test();
};

stop_test::stop_test() : test("stop_test") {
	add("test_up", test_up);
    add("test_down", test_down);
}

bool stop_test::test_up() {
	slob::book book;
    const auto order = std::make_shared<slob::order>(slob::side::bid, slob::max_price, 1);
    const auto stop_up = std::make_shared<slob::stop>(slob::direction::up, 1005, order);

    book.insert(std::make_shared<slob::order>(slob::side::ask, 1000, 1));
    book.insert(std::make_shared<slob::order>(slob::side::bid, 1000, 1));
    // market price is now 1000

    book.insert(stop_up);

    if(order->get_state() != slob::order_state::pending) {
        throw std::runtime_error("Order should be pending");
    }

    book.insert(std::make_shared<slob::order>(slob::side::ask, 1004, 1));
    book.insert(std::make_shared<slob::order>(slob::side::bid, 1004, 1));

    // market price is now 2000
    // trigger should trigger at 1995

    if(order->get_state() != slob::order_state::pending) {
        throw std::runtime_error("Order should be pending");
    }

    book.insert(std::make_shared<slob::order>(slob::side::ask, 1005, 1));
    book.insert(std::make_shared<slob::order>(slob::side::bid, 1005, 1));
    
    // error
    if(order->get_state() == slob::order_state::pending) {
        throw std::runtime_error("Order should not be pending");
    }

	return true;
}


bool stop_test::test_down() {
	slob::book book;
    const auto order = std::make_shared<slob::order>(slob::side::bid, slob::max_price, 1);
    const auto stop_down = std::make_shared<slob::stop>(slob::direction::down, 995, order);

    book.insert(std::make_shared<slob::order>(slob::side::ask, 1000, 1));
    book.insert(std::make_shared<slob::order>(slob::side::bid, 1000, 1));
    // market price is now 1000

    book.insert(stop_down);

    if(order->get_state() != slob::order_state::pending) {
        throw std::runtime_error("Order should be pending");
    }

    book.insert(std::make_shared<slob::order>(slob::side::ask, 996, 1));
    book.insert(std::make_shared<slob::order>(slob::side::bid, 996, 1));

    // market price is now 996

    if(order->get_state() != slob::order_state::pending) {
        throw std::runtime_error("Order should be pending");
    }

    book.insert(std::make_shared<slob::order>(slob::side::ask, 995, 1));
    book.insert(std::make_shared<slob::order>(slob::side::bid, 995, 1));
    
    if(order->get_state() == slob::order_state::pending) {
        throw std::runtime_error("Order should not be pending");
    }

	return true;
}

#endif // #ifndef STOP_TEST_HPP