#ifndef TRIGGER_MODIFICATION_TEST_HPP
#define TRIGGER_MODIFICATION_TEST_HPP
#include "test.hpp"
#include "../include/slob.hpp"

class trigger_modification_test : public test {
	inline static bool set_price_test();
    inline static bool cancel_test();

	public:
	trigger_modification_test();
};

trigger_modification_test::trigger_modification_test() : test("trigger_modification_test") {
	add("set_price_test", set_price_test);
    add("cancel_test", cancel_test);
}

bool trigger_modification_test::set_price_test() {
    slob::book book;

    std::vector<std::shared_ptr<slob::trigger>> triggers = {
        std::make_shared<slob::trigger>(slob::trigger_type::bid_up, 10),
        std::make_shared<slob::trigger>(slob::trigger_type::ask_up, 20),
        std::make_shared<slob::trigger>(slob::trigger_type::ask_down, 10),
        std::make_shared<slob::trigger>(slob::trigger_type::bid_down, 0),
        std::make_shared<slob::trigger>(slob::trigger_type::market_up, 11),
        std::make_shared<slob::trigger>(slob::trigger_type::market_down, 9)
    };

    // set market price to 10
    book.insert(std::make_shared<slob::order>(slob::side::bid, 10, 1));
    book.insert(std::make_shared<slob::order>(slob::side::ask, 10, 1));
    
    book.insert(std::make_shared<slob::order>(slob::side::bid, 5, 100));
    book.insert(std::make_shared<slob::order>(slob::side::ask, 15, 100));

    for(const auto &trigger: triggers) {
        book.insert(trigger);

        if(trigger->get_state() != slob::trigger_state::resting) {
            throw std::runtime_error("Trigger should be resting");
        }

        const int64_t new_price = trigger->get_type() == slob::trigger_type::market_down ?
            trigger->get_price() - 1:
            trigger->get_price() + 1;

        trigger->set_price(new_price);

        if(trigger->get_state() != slob::trigger_state::resting && trigger->get_price() != new_price) {
            throw std::runtime_error("Trigger should be resting at new price");
        }
    }

	return true;
}

bool trigger_modification_test::cancel_test() {
	slob::book book;

    std::vector<std::shared_ptr<slob::trigger>> triggers = {
        std::make_shared<slob::trigger>(slob::trigger_type::bid_up, 10),
        std::make_shared<slob::trigger>(slob::trigger_type::ask_up, 20),
        std::make_shared<slob::trigger>(slob::trigger_type::ask_down, 10),
        std::make_shared<slob::trigger>(slob::trigger_type::bid_down, 0),
        std::make_shared<slob::trigger>(slob::trigger_type::market_up, 11),
        std::make_shared<slob::trigger>(slob::trigger_type::market_down, 9)
    };

    // set market price to 10
    book.insert(std::make_shared<slob::order>(slob::side::bid, 10, 1));
    book.insert(std::make_shared<slob::order>(slob::side::ask, 10, 1));
    
    book.insert(std::make_shared<slob::order>(slob::side::bid, 5, 100));
    book.insert(std::make_shared<slob::order>(slob::side::ask, 15, 100));


    for(const auto &trigger: triggers) {
        book.insert(trigger);

        if(trigger->get_state() != slob::trigger_state::resting) {
            throw std::runtime_error("Trigger should be resting");
        }

        trigger->cancel();

        if(trigger->get_state() != slob::trigger_state::canceled) {
            throw std::runtime_error("Trigger should have been canceled");
        }
    }

	return true;
}

#endif // #ifndef TRIGGER_MODIFICATION_TEST_HPP