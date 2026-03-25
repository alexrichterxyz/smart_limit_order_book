#include "order_insert_test.hpp"
#include "order_modification_test.hpp"
#include "trigger_trigger_test.hpp"
#include "trigger_modification_test.hpp"
#include "trailing_stop_test.hpp"
#include "stop_test.hpp"
#include "deferral_tests.hpp"

#include <iostream>

int main() {
	std::vector<std::shared_ptr<test>> tests = {
		std::make_shared<order_insert_test>(),
		std::make_shared<order_modification_test>(),
		std::make_shared<trigger_trigger_test>(),
		std::make_shared<trigger_modification_test>(),
		std::make_shared<trailing_stop_test>(),
		std::make_shared<stop_test>(),
		std::make_shared<test_deferrals>()
	};

	std::size_t passed_count = 0;
	std::size_t total_count = 0;
	for(const auto &test_ptr: tests) {
		passed_count += test_ptr->run();
		total_count += test_ptr->test_count();
	}

	if(passed_count == total_count) {
		std::cout << "Passed all\n";
	} else {
		std::cout << "Passed " << passed_count << " of " << total_count << "\n";
	}
	
	
	return 0;
}