#ifndef TRIGGER_TRIGGER_TEST_HPP
#define TRIGGER_TRIGGER_TEST_HPP
#include "test.hpp"
#include "../include/slob.hpp"
#include <vector>


// todo: test market up and down triggers
class id_trigger : public slob::trigger {
    static std::size_t count;
    std::size_t m_trigger_id = -1;

    public:
    id_trigger(const slob::trigger_type t_type, const std::int64_t t_price): trigger(t_type, t_price) {}

    void on_triggered() override {
        m_trigger_id = count++;
    }

    inline std::size_t get_trigger_id() const {
        return m_trigger_id;
    }

};

std::size_t id_trigger::count = 0;


class trigger_trigger_test : public test {
    inline static bool empty_book_trigger_insert_test();
	inline static bool bid_up_test();
    inline static bool ask_down_test();
    inline static bool bid_down_test();
    inline static bool ask_up_test();
    inline static bool bid_insert_test();
    inline static bool ask_insert_test();

	public:
	trigger_trigger_test();
};

#include "../include/book.hpp"

trigger_trigger_test::trigger_trigger_test() : test("trigger_trigger_test") {
    add("empty_book_trigger_insert_test", empty_book_trigger_insert_test);
	add("bid_up_test", bid_up_test);
    add("ask_down_test", ask_down_test);
    add("bid_down_test", bid_down_test);
    add("ask_up_test", ask_up_test);
    add("bid_insert_test", bid_insert_test);
    add("ask_insert_test", ask_insert_test);
}

bool trigger_trigger_test::empty_book_trigger_insert_test() {
    slob::book book;
    
    std::vector<std::shared_ptr<slob::trigger>> resting_triggers = {
        std::make_shared<slob::trigger>(slob::trigger_type::bid_up, 10),
        std::make_shared<slob::trigger>(slob::trigger_type::ask_down, 10),
        std::make_shared<slob::trigger>(slob::trigger_type::market_up, 10),
    };

    std::vector<std::shared_ptr<slob::trigger>> triggered_triggers = {
        std::make_shared<slob::trigger>(slob::trigger_type::bid_down, 10),
        std::make_shared<slob::trigger>(slob::trigger_type::ask_up, 10),
        std::make_shared<slob::trigger>(slob::trigger_type::market_down, 10)
    };
        
    for(const auto &trigger : resting_triggers){
        book.insert(trigger);

        if(trigger->get_state() != slob::trigger_state::resting) {
            throw std::runtime_error("Trigger should be resting");
            return false;
        }
    }

    for(const auto &trigger : triggered_triggers){
        book.insert(trigger);

        if(trigger->get_state() != slob::trigger_state::triggered) {
            throw std::runtime_error("Trigger should have been triggered");
            return false;
        }
    }

    return true;


}


bool trigger_trigger_test::bid_up_test() {
    slob::book book;
    
    const auto trigger_9 = std::make_shared<slob::trigger>(slob::trigger_type::bid_up, 9);
    const auto trigger_10 = std::make_shared<slob::trigger>(slob::trigger_type::bid_up, 10);
    const auto trigger_11 = std::make_shared<slob::trigger>(slob::trigger_type::bid_up, 11);

    book.insert(std::make_shared<slob::order>(slob::side::bid, 9, 1));

    book.insert(trigger_9);
    book.insert(trigger_10);
    book.insert(trigger_11);

    if(trigger_9->get_state() != slob::trigger_state::triggered) {
        throw std::runtime_error("Trigger should have been trigggered");
    }

    if(trigger_10->get_state() != slob::trigger_state::resting || trigger_11->get_state() != slob::trigger_state::resting) {
        throw std::runtime_error("Triggers should be resting");
    }

    book.insert(std::make_shared<slob::order>(slob::side::bid, 10, 1));

    if(trigger_10->get_state() != slob::trigger_state::triggered) {
        throw std::runtime_error("Trigger should have been triggered");
    }

    if(trigger_11->get_state() != slob::trigger_state::resting) {
        throw std::runtime_error("Trigger should be resting");
    }

	return true;
}


bool trigger_trigger_test::ask_down_test() {
    slob::book book;
    
    const auto trigger_9 = std::make_shared<slob::trigger>(slob::trigger_type::ask_down, 9);
    const auto trigger_10 = std::make_shared<slob::trigger>(slob::trigger_type::ask_down, 10);
    const auto trigger_11 = std::make_shared<slob::trigger>(slob::trigger_type::ask_down, 11);

    book.insert(std::make_shared<slob::order>(slob::side::ask, 11, 1));

    book.insert(trigger_9);
    book.insert(trigger_10);
    book.insert(trigger_11);

    if(trigger_11->get_state() != slob::trigger_state::triggered) {
        throw std::runtime_error("Trigger should have been trigggered");
    }

    if(trigger_10->get_state() != slob::trigger_state::resting || trigger_9->get_state() != slob::trigger_state::resting) {
        throw std::runtime_error("Triggers should be resting");
    }

    book.insert(std::make_shared<slob::order>(slob::side::ask, 10, 1));

    if(trigger_10->get_state() != slob::trigger_state::triggered) {
        throw std::runtime_error("Trigger should have been triggered");
    }

    if(trigger_9->get_state() != slob::trigger_state::resting) {
        throw std::runtime_error("Trigger should be resting");
    }

	return true;
}

bool trigger_trigger_test::bid_down_test() {
    slob::book book;

    const auto trigger_11 = std::make_shared<slob::trigger>(slob::trigger_type::bid_down, 11);
    const auto trigger_10 = std::make_shared<slob::trigger>(slob::trigger_type::bid_down, 10);
    const auto trigger_9 = std::make_shared<slob::trigger>(slob::trigger_type::bid_down, 9);
    
    auto order_11 = std::make_shared<slob::order>(slob::side::bid, 11, 1);
    book.insert(order_11);

    book.insert(trigger_11);
    book.insert(trigger_10);
    book.insert(trigger_9);

    if(trigger_11->get_state() != slob::trigger_state::triggered) {
        throw std::runtime_error("Trigger should have been triggered");
    }

    if(trigger_10->get_state() != slob::trigger_state::resting || trigger_9->get_state() != slob::trigger_state::resting) {
        throw std::runtime_error("Triggers should be resting");
    }
    
    auto order_10 = std::make_shared<slob::order>(slob::side::bid, 10, 1);
    book.insert(order_10);
    order_11->cancel();

    if(trigger_10->get_state() != slob::trigger_state::triggered) {
        throw std::runtime_error("Trigger should have been triggered");
    }
    
    if(trigger_9->get_state() != slob::trigger_state::resting) {
        throw std::runtime_error("Trigger should be resting");
    }

    order_10->cancel();

    if(trigger_9->get_state() != slob::trigger_state::triggered) {
        throw std::runtime_error("Trigger should have been triggered");
    }


    return true;
}

bool trigger_trigger_test::ask_up_test() {
    slob::book book;

    const auto trigger_9 = std::make_shared<slob::trigger>(slob::trigger_type::ask_up, 9);
    const auto trigger_10 = std::make_shared<slob::trigger>(slob::trigger_type::ask_up, 10);
    const auto trigger_11 = std::make_shared<slob::trigger>(slob::trigger_type::ask_up, 11);
    
    auto order_9 = std::make_shared<slob::order>(slob::side::ask, 9, 1);
    book.insert(order_9);

    book.insert(trigger_9);
    book.insert(trigger_10);
    book.insert(trigger_11);

    if(trigger_9->get_state() != slob::trigger_state::triggered) {
        throw std::runtime_error("Trigger should have been triggered");
    }

    if(trigger_10->get_state() != slob::trigger_state::resting || trigger_11->get_state() != slob::trigger_state::resting) {
        throw std::runtime_error("Triggers should be resting");
    }
    
    auto order_10 = std::make_shared<slob::order>(slob::side::ask, 10, 1);
    book.insert(order_10);
    order_9->cancel();

    if(trigger_10->get_state() != slob::trigger_state::triggered) {
        throw std::runtime_error("Trigger should have been triggered");
    }
    
    // error
    if(trigger_11->get_state() != slob::trigger_state::resting) {
        throw std::runtime_error("Trigger should be resting");
    }

    order_10->cancel();

    if(trigger_11->get_state() != slob::trigger_state::triggered) {
        throw std::runtime_error("Trigger should have been triggered");
    }


    return true;
}

bool trigger_trigger_test::bid_insert_test() {
    slob::book book;

    std::vector<std::shared_ptr<id_trigger>> triggers = {
        // these should not get triggered
        std::make_shared<id_trigger>(slob::trigger_type::ask_up, 12),
        std::make_shared<id_trigger>(slob::trigger_type::bid_up, 12),
        std::make_shared<id_trigger>(slob::trigger_type::market_up, 12),

        // these should get triggered
        std::make_shared<id_trigger>(slob::trigger_type::ask_up, 10),
        std::make_shared<id_trigger>(slob::trigger_type::ask_up, 10),
        std::make_shared<id_trigger>(slob::trigger_type::bid_up, 10),
        std::make_shared<id_trigger>(slob::trigger_type::bid_up, 10),
        std::make_shared<id_trigger>(slob::trigger_type::market_up, 9),
        std::make_shared<id_trigger>(slob::trigger_type::market_up, 9)
    };

    book.insert(std::make_shared<slob::order>(slob::side::ask, 9, 1));
    book.insert(std::make_shared<slob::order>(slob::side::ask, 11, 1));

    std::vector<std::size_t> insertion_order = {0, 1, 2, 3, 4, 5, 8, 7, 6};

    for(const auto i: insertion_order){
        book.insert(triggers[i]);
    }

    for(const auto &trigger: triggers) {
        if(trigger->get_state() != slob::trigger_state::resting) {
            throw std::runtime_error("Trigger should be resting");
        }
    }

    /* the following order will cause the bid, ask, to rise to 10
    the market price will to rise to 9
    */
    book.insert(std::make_shared<slob::order>(slob::side::bid, 10, 2));

    for(std::size_t i = 0; i < triggers.size(); ++i) {
        const auto &trigger = triggers[i];
        if(i < 3) {
            if(trigger->get_state() != slob::trigger_state::resting) {
                throw std::runtime_error("Trigger should be resting");
            }
        } else {
            if(trigger->get_state() != slob::trigger_state::triggered) {
                throw std::runtime_error("Trigger should have been triggered");
            }
        }
    }

    std::vector<std::size_t> trigger_order = {8, 7, 3, 4, 5, 6};

    for(std::size_t i = 1; i < trigger_order.size(); i++) {
        std::size_t current_trigger_idx = trigger_order[i-1];
        std::size_t next_trigger_idx = trigger_order[i];

        if(triggers[current_trigger_idx]->get_trigger_id() >= triggers[next_trigger_idx]->get_trigger_id()) {
            throw std::runtime_error("Triggered in incorrect order");
        }
    }
    
    return true;
}


bool trigger_trigger_test::ask_insert_test() {
    slob::book book;

    std::vector<std::shared_ptr<id_trigger>> triggers = {
        // these should not get triggered
        std::make_shared<id_trigger>(slob::trigger_type::ask_down, 9),
        std::make_shared<id_trigger>(slob::trigger_type::bid_down, 9),
        std::make_shared<id_trigger>(slob::trigger_type::market_down, 9),

        // these should get triggered
        std::make_shared<id_trigger>(slob::trigger_type::ask_down, 11),
        std::make_shared<id_trigger>(slob::trigger_type::ask_down, 11),
        std::make_shared<id_trigger>(slob::trigger_type::bid_down, 11),
        std::make_shared<id_trigger>(slob::trigger_type::bid_down, 11),
        std::make_shared<id_trigger>(slob::trigger_type::market_down, 12),
        std::make_shared<id_trigger>(slob::trigger_type::market_down, 12)
    };

    // set market price to 15
    book.insert(std::make_shared<slob::order>(slob::side::bid, 15, 1));
    book.insert(std::make_shared<slob::order>(slob::side::ask, 15, 1));

    book.insert(std::make_shared<slob::order>(slob::side::bid, 10, 1));
    book.insert(std::make_shared<slob::order>(slob::side::bid, 12, 1));
    

    std::vector<std::size_t> insertion_order = {0, 1, 2, 3, 4, 5, 8, 7, 6};

    for(const auto i: insertion_order){
        book.insert(triggers[i]);
    }

    for(const auto &trigger: triggers) {
        if(trigger->get_state() != slob::trigger_state::resting) {
            throw std::runtime_error("Trigger should be resting");
        }
    }

    /* the following order will cause the bid, ask, to rise to 10
    the market price will to rise to 9
    */
    book.insert(std::make_shared<slob::order>(slob::side::ask, 11, 2));

    for(std::size_t i = 0; i < triggers.size(); ++i) {
        const auto &trigger = triggers[i];
        if(i < 3) {
            if(trigger->get_state() != slob::trigger_state::resting) {
                throw std::runtime_error("Trigger should be resting");
            }
        } else {
            if(trigger->get_state() != slob::trigger_state::triggered) {
                throw std::runtime_error("Trigger should have been triggered");
            }
        }
    }

    std::vector<std::size_t> trigger_order = {8, 7, 5, 6, 3, 4};

    for(std::size_t i = 1; i < trigger_order.size(); i++) {
        std::size_t current_trigger_idx = trigger_order[i-1];
        std::size_t next_trigger_idx = trigger_order[i];

        if(triggers[current_trigger_idx]->get_trigger_id() >= triggers[next_trigger_idx]->get_trigger_id()) {
            throw std::runtime_error("Triggered in incorrect order");
        }
    }
    
    return true;
}

#endif // #ifndef TRIGGER_TRIGGER_TEST_HPP