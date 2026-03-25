#ifndef ORDER_INSERT_TEST_HPP
#define ORDER_INSERT_TEST_HPP
#include "test.hpp"
#include "../include/slob.hpp"

class order_insert_test : public test {
	inline static bool rest_bids();
	inline static bool rest_asks();
	inline static bool insert_marketable_bids();
	inline static bool insert_marketable_asks();
	inline static bool min_quantity_insufficient_bids();
	inline static bool min_quantity_insufficient_asks();

	public:
	order_insert_test();
};

#include "../include/book.hpp"

order_insert_test::order_insert_test() : test("order_insert_test") {
	add("rest_bids", rest_bids);
	add("rest_asks", rest_asks);
	add("insert_marketable_bids", insert_marketable_bids);
	add("insert_marketable_asks", insert_marketable_asks);

	add("min_quantity_insufficient_bids", min_quantity_insufficient_bids);
	add("min_quantity_insufficient_asks", min_quantity_insufficient_asks);
}

bool order_insert_test::rest_bids() {
	slob::book book;
	book.insert(std::make_shared<slob::order>(slob::side::bid, 10, 1));
	book.insert(std::make_shared<slob::order>(slob::side::bid, 10, 2));
	book.insert(std::make_shared<slob::order>(slob::side::bid, 11, 3));
	book.insert(std::make_shared<slob::order>(slob::side::bid, 9, 4));

	if(book.get_bid_quantity() != 10) {
		throw std::runtime_error("Incorrect bid quantity in book");
	}

	if(book.get_bid_level_count() != 3) {
		throw std::runtime_error("Incorrect number of bid levels");
	}

	if(book.get_bid_price() != 11) {
		throw std::runtime_error("Incorrect bid price");
	}

	if(book.bid_level_at(10)->second.get_quantity() != 3) {
		throw std::runtime_error("Incorrect quantity at bid level");
	}

	auto price10_second_order = *(++(book.bid_level_at(10)->second.begin()));

	if(price10_second_order->get_quantity() != 2) {
		throw std::runtime_error("Time priority not respected");
	}

	return true;
}

bool order_insert_test::rest_asks() {
	slob::book book;
	book.insert(std::make_shared<slob::order>(slob::side::ask, 10, 1));
	book.insert(std::make_shared<slob::order>(slob::side::ask, 10, 2));
	book.insert(std::make_shared<slob::order>(slob::side::ask, 9, 4));
	book.insert(std::make_shared<slob::order>(slob::side::ask, 11, 3));

	if(book.get_ask_quantity() != 10) {
		throw std::runtime_error("Incorrect ask quantity in book");
	}

	if(book.get_ask_level_count() != 3) {
		throw std::runtime_error("Incorrect number of ask levels");
	}

	if(book.get_ask_price() != 9) {
		throw std::runtime_error("Incorrect ask price");
	}

	if(book.ask_level_at(10)->second.get_quantity() != 3) {
		throw std::runtime_error("Incorrect quantity at ask level");
	}

	auto price10_second_order = *(++(book.ask_level_at(10)->second.begin()));

	if(price10_second_order->get_quantity() != 2) {
		throw std::runtime_error("Time priority not respected");
	}

	return true;
}

bool order_insert_test::insert_marketable_bids() { 
	slob::book book;
	book.insert(std::make_shared<slob::order>(slob::side::ask, 10, 1));
	book.insert(std::make_shared<slob::order>(slob::side::ask, 10, 2));
	book.insert(std::make_shared<slob::order>(slob::side::ask, 11, 3));
	book.insert(std::make_shared<slob::order>(slob::side::ask, 9, 4));

	book.insert(std::make_shared<slob::order>(slob::side::bid, 10, 8));

	if(book.get_ask_price() != 11) {
		throw std::runtime_error("Incorrect ask price after bid insert");
	}

	if(book.get_ask_quantity() != 3) {
		throw std::runtime_error("Incorrect ask quantity after bid insert");
	}

	if(book.get_bid_price() != 10) {
		throw std::runtime_error("Incorrect bid price after insert");
	}

	if(book.get_bid_quantity() != 1) {
		throw std::runtime_error("Incorrect bid quantity after insert");
	}

	if(book.get_market_price() != 10) {
		throw std::runtime_error("Incorrect market price");
	}
	
	return true;
}

bool order_insert_test::insert_marketable_asks() { 
	slob::book book;
	book.insert(std::make_shared<slob::order>(slob::side::bid, 10, 1));
	book.insert(std::make_shared<slob::order>(slob::side::bid, 10, 2));
	book.insert(std::make_shared<slob::order>(slob::side::bid, 11, 3));
	book.insert(std::make_shared<slob::order>(slob::side::bid, 9, 4));

	book.insert(std::make_shared<slob::order>(slob::side::ask, 10, 7));

	if(book.get_bid_price() != 9) {
		throw std::runtime_error("Incorrect bid price after ask insert");
	}

	if(book.get_bid_quantity() != 4) {
		throw std::runtime_error("Incorrect bid quantity after ask insert");
	}

	if(book.get_ask_price() != 10) {
		throw std::runtime_error("Incorrect ask price after insert");
	}

	if(book.get_ask_quantity() != 1) {
		throw std::runtime_error("Incorrect ask quantity after insert");
	}
	
	if(book.get_market_price() != 10) {
		throw std::runtime_error("Incorrect market price");
	}
	
	
	return true;
}

bool order_insert_test::min_quantity_insufficient_bids() {
	slob::book book;
	book.insert(std::make_shared<slob::order>(slob::side::bid, 10, 1));

    const auto order = std::make_shared<slob::order>(slob::side::ask, 9, 2, 2);
	book.insert(order);

	if(order->get_state() != slob::order_state::canceled) {
        throw std::runtime_error("Ask with unfillable min quantity should have been canceled");
    }

	return true;
}

bool order_insert_test::min_quantity_insufficient_asks() {
	slob::book book;
	book.insert(std::make_shared<slob::order>(slob::side::ask, 9, 1));

    const auto order = std::make_shared<slob::order>(slob::side::bid, 10, 2, 2);
	book.insert(order);

	if(order->get_state() != slob::order_state::canceled) {
        throw std::runtime_error("Bid with unfillable min quantity should have been canceled");
    }

	return true;
}

#endif // #ifndef ORDER_INSERT_TEST_HPP