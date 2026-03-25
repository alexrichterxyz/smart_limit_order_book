#ifndef ORDER_MODIFICATION_TEST_HPP
#define ORDER_MODIFICATION_TEST_HPP
#include "test.hpp"
#include "../include/slob.hpp"

class order_modification_test : public test {
	inline static bool test_set_quantity_decrease();
    inline static bool test_set_quantity_increase();
    inline static bool test_set_price();

	public:
	order_modification_test();
};

#include "../include/book.hpp"

order_modification_test::order_modification_test() : test("order_modification_test") {
	add("set_quantity_decrease", test_set_quantity_decrease);
    add("test_set_quantity_increase", test_set_quantity_increase);
    add("test_set_price", test_set_price);
}

bool order_modification_test::test_set_quantity_decrease() {
    slob::book book;
    const auto bid_0 = std::make_shared<slob::order>(slob::side::bid, 10, 100);
    const auto bid_1 = std::make_shared<slob::order>(slob::side::bid, 10, 100);
    const auto ask_0 = std::make_shared<slob::order>(slob::side::ask, 11, 100);
    const auto ask_1 = std::make_shared<slob::order>(slob::side::ask, 11, 100);
    
    book.insert(bid_0);
    book.insert(bid_1);
    book.insert(ask_0);
    book.insert(ask_1);
    
    bid_0->set_quantity(99);
    ask_0->set_quantity(98);

    if(book.get_bid_quantity() != 199) {
        throw std::runtime_error("Bid quantity didn't decrease");
    }

    if(book.bid_levels_begin()->second.get_quantity() != 199) {
        throw std::runtime_error("Bid level quantity didn't decrease");
    }

    if(book.get_ask_quantity() != 198) {
        throw std::runtime_error("Ask quantity didn't decrease");
    }

    if(book.ask_levels_begin()->second.get_quantity() != 198) {
        throw std::runtime_error("Ask level quantity didn't decrease");
    }

    if (*(book.ask_levels_begin()->second.begin()) != ask_0) {
        throw std::runtime_error("Quantity decrease should not cause reinsert");
    }

    if (*(book.bid_levels_begin()->second.begin()) != bid_0) {
        throw std::runtime_error("Quantity decrease should not cause reinsert");
    }

	return true;
}

bool order_modification_test::test_set_quantity_increase() {
	
	slob::book book;
    const auto bid_0 = std::make_shared<slob::order>(slob::side::bid, 10, 100);
    const auto bid_1 = std::make_shared<slob::order>(slob::side::bid, 10, 100);
    const auto ask_0 = std::make_shared<slob::order>(slob::side::ask, 11, 100);
    const auto ask_1 = std::make_shared<slob::order>(slob::side::ask, 11, 100);
    
    book.insert(bid_0);
    book.insert(bid_1);
    book.insert(ask_0);
    book.insert(ask_1);

    bid_0->set_quantity(101);
    ask_0->set_quantity(102);

    if(book.get_bid_quantity() != 201) {
        throw std::runtime_error("Bid quantity didn't increase");
    }

    if(book.bid_levels_begin()->second.get_quantity() != 201) {
        throw std::runtime_error("Bid level quantity didn't increase");
    }

    if(book.get_ask_quantity() != 202) {
        throw std::runtime_error("Ask quantity didn't increase");
    }

    if(book.ask_levels_begin()->second.get_quantity() != 202) {
        throw std::runtime_error("Ask level quantity didn't increase");
    }

    if (*std::prev(book.ask_levels_begin()->second.end()) != ask_0) {
        throw std::runtime_error("Quantity increae should cause reinsert");
    }

    if (*std::prev(book.bid_levels_begin()->second.end()) != bid_0) {
        throw std::runtime_error("Quantity increase should cause reinsert");
    }

	return true;
}

bool order_modification_test::test_set_price() {
    slob::book book;

    const auto bid_0 = std::make_shared<slob::order>(slob::side::bid, 10, 100);
    const auto bid_1 = std::make_shared<slob::order>(slob::side::bid, 10, 1000);
    const auto ask_0 = std::make_shared<slob::order>(slob::side::ask, 11, 10);
    const auto ask_1 = std::make_shared<slob::order>(slob::side::ask, 11, 100);
    
    book.insert(bid_0);
    book.insert(bid_1);
    book.insert(ask_0);
    book.insert(ask_1);

    bid_0->set_price(9);
    ask_0->set_price(12);

    if(book.get_bid_quantity() != 1100) {
        throw std::runtime_error("Incorrect bid quantity");
    }

    if(book.get_ask_quantity() != 110) {
        throw std::runtime_error("Incorrect ask quantity");
    }

    if(std::prev(book.bid_levels_end())->first != 9 || *(std::prev(book.bid_levels_end())->second.begin()) != bid_0) {
        throw std::runtime_error("Bid not at correct price level");
    }

    if(std::prev(book.ask_levels_end())->first != 12 || *(std::prev(book.ask_levels_end())->second.begin()) != ask_0) {
        throw std::runtime_error("Ask not at correct price level");
    }

    return true;
}

#endif // #ifndef ORDER_MODIFICATION_TEST_HPP