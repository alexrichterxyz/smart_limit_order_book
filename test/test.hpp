#ifndef TEST_HPP
#define TEST_HPP
#include <functional>
#include <string>
#include <unordered_map>

using test_function = std::function<bool(void)>;
using named_tests = std::unordered_map<std::string, test_function>;

class test {
	private:
	const std::string prefix;
	named_tests m_tests;

	public:
	test(const std::string &prefix);
	inline void add(const std::string &name, const test_function &function);
	inline std::size_t run();
	inline std::size_t test_count() const;
};

#include <iomanip>
#include <iostream>
#include <string>

test::test(const std::string &prefix) : prefix(prefix) {}

void test::add(const std::string &id, const test_function &function) {
	m_tests[id] = function;
}

std::size_t test::run() {
	std::size_t passed_count = 0;

	for (const auto &[id, function] : m_tests) {
		bool passed_test = false;
		std::string message;

		try {
			passed_test = function();
		} catch (const std::exception &e) {
			passed_test = false;
			message = e.what();
		}

		if (passed_test) {
			++passed_count;
		}

		const char fill_before = std::cout.fill();

		(passed_test ? std::cout : std::cerr)
		    << std::setfill('.') << std::setw(60) << std::left
		    << (prefix + "::" + id) << std::setw(20) << std::right
		    << (passed_test ? "PASSED" : "FAILED") << '\n'
			<< (message != "" ? "\t(" + message + ")\n" : "")
		    << std::setfill(fill_before);
	}

	std::cout << "Passed: " << passed_count << '/' << m_tests.size()
		  << "\n\n";

	return passed_count;
}

std::size_t test::test_count() const { return m_tests.size(); }

#endif // #ifndef TEST_HPP