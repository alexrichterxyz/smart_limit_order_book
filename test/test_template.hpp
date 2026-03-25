#ifndef TEST_TEMPLATE_HPP
#define TEST_TEMPLATE_HPP
#include "test.hpp"
#include "../include/slob.hpp"

class test_template : public test {
	inline static bool test_1();

	public:
	test_template();
};

test_template::test_template() : test("test_template") {
	add("test_1", test_1);
}

bool test_template::test_1() {
	
	if(false) {
		throw std::runtime_error("Error message");
	}

	return true;
}

#endif // #ifndef TEST_TEMPLATE_HPP