#ifndef TEST_DEFERRALS_HPP
#define TEST_DEFERRALS_HPP
#include "../include/slob.hpp"
#include "test.hpp"

class event_count_order : public slob::order {
	private:
	std::size_t m_accepted_id = -1;
	std::size_t m_on_before_match_id = -1;
	std::size_t m_on_resting_id = -1;
	std::size_t m_on_filled_id = -1;
	std::size_t m_on_canceled_id = -1;
	std::size_t m_on_deferred_cancel_id = -1;
	std::size_t m_on_deferred_set_price_id = -1;
	std::size_t m_on_deferred_set_quantity_id = -1;
	static std::size_t m_callback_count;

	public:
	virtual void on_accepted() override {
		m_accepted_id = m_callback_count++;
	};

	virtual void on_resting() override {
		m_on_resting_id = m_callback_count++;
	};

	virtual void on_before_match(slob::c_corder_ptr &t_order) override {
		m_on_before_match_id = m_callback_count++;
	};

	virtual void on_filled() override {
		m_on_filled_id = m_callback_count++;
	};

	virtual void on_canceled() override {
		m_on_canceled_id = m_callback_count++;
	};

	virtual bool on_deferred_cancel() override {
		m_on_deferred_cancel_id = m_callback_count++;
		return true;
	};

	virtual bool on_deferred_set_price(
	    const std::int64_t quantity) override {
		m_on_deferred_set_price_id = m_callback_count++;
		return true;
	};

	virtual bool on_deferred_set_quantity(
	    const std::int64_t quantity) override {
		m_on_deferred_set_quantity_id = m_callback_count++;
		return true;
	};

	event_count_order(const slob::side t_side, const std::int64_t t_price,
	    const std::int64_t t_quantity)
	    : order(t_side, t_price, t_quantity) {}

	std::size_t get_on_accepted_id() const { return m_accepted_id; }

	std::size_t get_on_before_match_id() const {
		return m_on_before_match_id;
	}

	std::size_t get_on_resting_id() const { return m_on_resting_id; }

	std::size_t get_on_filled_id() const { return m_on_filled_id; }

	std::size_t get_on_canceled_id() const { return m_on_canceled_id; }

	std::size_t get_on_deferred_cancel_id() const {
		return m_on_deferred_cancel_id;
	}

	std::size_t get_on_deferred_set_price_id() const {
		return m_on_deferred_set_price_id;
	}

	std::size_t get_on_deferred_set_quantity_id() const {
		return m_on_deferred_set_quantity_id;
	}
};

std::size_t event_count_order::m_callback_count = 0;

enum event_handler {
	on_accepted,
	on_before_match,
	on_resting,
	on_filled,
	on_canceled,
	on_deferred_cancel,
	on_deferred_set_price,
	on_deferred_set_quantity
};

class deferral_test_order : public event_count_order {
	slob::deferrable m_deferrable; // determines which operation is
				       // performed to which order;
	event_handler m_event_handler; // determines when operation is performed

	void perform_operation(event_handler t_event_handler) {
		if (m_event_handler != t_event_handler) {
			return;
		}

		switch (m_deferrable.type()) {
		case slob::deferrable_type::cancel_order: {
			slob::cancel_order_deferrable deferrable =
			    m_deferrable.get_cancel_order_deferrable();
			deferrable.order->cancel();
			break;
		}
		case slob::deferrable_type::set_price: {
			slob::set_price_deferrable deferrable =
			    m_deferrable.get_set_price_deferrable();
			deferrable.order->set_price(deferrable.price);
			break;
		}
		case slob::deferrable_type::set_quantity: {
			slob::set_quantity_deferrable deferrable =
			    m_deferrable.get_set_quantity_deferrable();
			deferrable.order->set_quantity(deferrable.quantity);
			break;
		}
		case slob::deferrable_type::insert_order: {
			slob::insert_order_deferrable deferrable =
			    m_deferrable.get_insert_order_deferrable();
			get_book()->insert(deferrable.order);
			break;
		}
		}
	}

	public:
	deferral_test_order(const slob::side t_side, const std::int64_t t_price,
	    const std::int64_t t_quantity, slob::deferrable t_deferrable,
	    event_handler t_event_handler)
	    : event_count_order(t_side, t_price, t_quantity),
	      m_deferrable(t_deferrable), m_event_handler(t_event_handler) {}

	virtual void on_accepted() {
		event_count_order::on_accepted();
		perform_operation(event_handler::on_accepted);
	};

	virtual void on_resting() {
		event_count_order::on_resting();
		perform_operation(event_handler::on_resting);
	};

	virtual void on_before_match(slob::c_corder_ptr &t_order) {
		event_count_order::on_before_match(t_order);
		perform_operation(event_handler::on_before_match);
	};

	virtual void on_filled() {
		event_count_order::on_filled();
		perform_operation(event_handler::on_filled);
	};

	virtual void on_canceled() {
		event_count_order::on_canceled();
		perform_operation(event_handler::on_canceled);
	};

	virtual bool on_deferred_cancel() {
		event_count_order::on_deferred_cancel();
		perform_operation(event_handler::on_deferred_cancel);
		return true;
	};

	virtual bool on_deferred_set_price(const std::int64_t quantity) {
		event_count_order::on_deferred_set_price(quantity);
		perform_operation(event_handler::on_deferred_set_price);
		return true;
	};

	virtual bool on_deferred_set_quantity(const std::int64_t quantity) {
		event_count_order::on_deferred_set_quantity(quantity);
		perform_operation(event_handler::on_deferred_set_quantity);
		return true;
	};
};

class test_deferrals : public test {
	inline static bool on_accepted_no_deferral();
	inline static bool two_levels_deferral();
	inline static bool not_deferrable_operation();
	inline static bool repeated_insert();
	inline static bool resting_on_before_match_first();

	public:
	test_deferrals();
};

test_deferrals::test_deferrals() : test("test_deferrals") {
	add("on_accepted_no_deferral", on_accepted_no_deferral);
	add("two_levels_deferral", two_levels_deferral);
	add("not_deferrable_operation", not_deferrable_operation);
	add("repeated_insert", repeated_insert);
	add("resting_on_before_match_first", resting_on_before_match_first);
}

bool test_deferrals::on_accepted_no_deferral() {

	auto accepted_second_order =
	    std::make_shared<event_count_order>(slob::side::bid, 10, 100);
	slob::insert_order_deferrable deferrable(accepted_second_order);
	auto accepted_first_order = std::make_shared<deferral_test_order>(
	    slob::side::bid, 10, 100, deferrable, event_handler::on_accepted);

	slob::book book_obj;
	book_obj.insert(accepted_first_order);

	if (accepted_second_order->get_on_accepted_id() <=
	    accepted_first_order->get_on_accepted_id()) {
		throw std::runtime_error("on_accepted should not defer");
	}

	if (accepted_second_order->get_on_resting_id() >=
	    accepted_first_order->get_on_resting_id()) {
		throw std::runtime_error(
		    "Second accepted order should have been inserted first");
	}

	return true;
}

bool test_deferrals::two_levels_deferral() {
	auto third_inserted_order =
	    std::make_shared<event_count_order>(slob::side::bid, 10, 100);
	slob::insert_order_deferrable third_insert_deferrable(
	    third_inserted_order);
	auto second_inserted_order =
	    std::make_shared<deferral_test_order>(slob::side::bid, 10, 100,
		third_insert_deferrable, event_handler::on_before_match);
	slob::insert_order_deferrable second_insert_deferrable(
	    second_inserted_order);
	auto first_inserted_order =
	    std::make_shared<deferral_test_order>(slob::side::bid, 10, 100,
		second_insert_deferrable, event_handler::on_before_match);

	slob::book book_obj;
	book_obj.insert(
	    std::make_shared<slob::order>(slob::side::ask, 0, 1000));
	book_obj.insert(first_inserted_order);

	if (first_inserted_order->get_state() != slob::order_state::filled ||
	    second_inserted_order->get_state() != slob::order_state::filled ||
	    third_inserted_order->get_state() != slob::order_state::filled) {
		throw std::runtime_error("All orders should be resting");
	}

	if (first_inserted_order->get_on_accepted_id() >=
	    second_inserted_order->get_on_accepted_id()) {
		throw std::runtime_error(
		    "First inserted order should have been accepted first");
	}

	if (second_inserted_order->get_on_accepted_id() >=
	    third_inserted_order->get_on_accepted_id()) {
		throw std::runtime_error(
		    "Second inserted order should have been accepted second");
	}

	if (first_inserted_order->get_on_filled_id() >=
	    second_inserted_order->get_on_filled_id()) {
		throw std::runtime_error(
		    "First inserted order should be resting first");
	}

	if (second_inserted_order->get_on_filled_id() >=
	    third_inserted_order->get_on_filled_id()) {
		throw std::runtime_error(
		    "Second inserted order should be resting second");
	}

	return true;
}

bool test_deferrals::not_deferrable_operation() {
	auto pending_order =
	    std::make_shared<event_count_order>(slob::side::bid, 10, 100);
	slob::set_quantity_deferrable pending_order_set_quantity(
	    pending_order, 101);
	auto inserted_order =
	    std::make_shared<deferral_test_order>(slob::side::bid, 10, 100,
		pending_order_set_quantity, event_handler::on_before_match);

	slob::book book_obj;
	book_obj.insert(std::make_shared<slob::order>(slob::side::ask, 0, 50));
	book_obj.insert(inserted_order);

	if (pending_order->get_quantity() != 101) {
		throw std::runtime_error("Quantity did not change");
	}

	if (pending_order->get_on_deferred_set_quantity_id() != -1) {
		throw std::runtime_error("Quantity changes on pending orders "
					 "should not be deferred");
	}

	return true;
}

bool test_deferrals::repeated_insert() {
	auto second_order =
	    std::make_shared<event_count_order>(slob::side::bid, 10, 100);
	slob::insert_order_deferrable insert_deferrable(second_order);
	auto first_order =
	    std::make_shared<deferral_test_order>(slob::side::bid, 10, 20,
		insert_deferrable, event_handler::on_before_match);

	slob::book book_obj;
	book_obj.insert(std::make_shared<slob::order>(slob::side::ask, 9, 10));
	book_obj.insert(std::make_shared<slob::order>(slob::side::ask, 9, 10));
	book_obj.insert(first_order);

	if (book_obj.get_bid_quantity() != 100) {
		throw std::runtime_error(
		    "Order should have been inserted only once");
	}

	return true;
}

bool test_deferrals::resting_on_before_match_first() {
	auto resting_order =
	    std::make_shared<event_count_order>(slob::side::ask, 10, 100);
	auto inserted_order =
	    std::make_shared<event_count_order>(slob::side::bid, 10, 100);

	slob::book book_obj;
	book_obj.insert(resting_order);
	book_obj.insert(inserted_order);

	if (inserted_order->get_on_before_match_id() <=
	    resting_order->get_on_before_match_id()) {
		throw std::runtime_error("on_before_match should have been "
					 "called on resting order first");
	}

	return true;
}

#endif // #ifndef TEST_DEFERRALS_HPP