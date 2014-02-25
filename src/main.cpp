#include <array>
#include <iostream>
#include <tuple>
#include <vector>

#include "wart/for_each.hpp"
#include "wart/for_each/container.hpp"
#include "wart/for_each/tuple.hpp"
#include "wart/math.hpp"
#include "wart/union.hpp"
#include "wart/variant.hpp"

struct person {
	int id;
	std::string name;
};

struct person_tag;

namespace wart {
	namespace extension {
		namespace for_each {
			template <typename T>
			struct tag_of<std::vector<T>> {
				typedef container_tag type;
			};

			template <>
			struct tag_of<person> {
				typedef person_tag type;
			};

			template <>
			struct for_each<person_tag> {
				template <typename>
				struct apply;
			};

			template <>
			struct for_each<person_tag>::apply<person> {
				template <typename F>
				static void call(person const& person, F const& f) {
					f(person.id);
					f(person.name);
				}
			};
		}
	}
}

struct print {
	template <typename T>
	void operator()(T const& x) const {
		std::cout << x << std::endl;
	}
};

struct print_visitor {
	typedef void result_type;
	void operator()(int& x) const {
		std::cout << "int(" << x << ")" << std::endl;
	}
	void operator()(double const& x) const {
		std::cout << "double(" << x << ")" << std::endl;
	}
};

int main() {
	using namespace wart;

	typedef union_t<char, int> test;
	std::cout << sizeof(test) << std::endl;

	variant<int, double> value(1);
	value.accept(print_visitor());

	std::vector<int> xs { 1, 2, 3 };
	for_each(xs, [](int x) {
			std::cout << x << std::endl;
		});
	std::tuple<int, int, int> ys { 1, 2, 3 };
	for_each(ys, [](int y) {
			std::cout << y << std::endl;
		});
	person person { 1, "andy" };
	for_each(person, print());

	std::cout << math::lcm_constant<std::size_t, 28, 7, 42>::value << std::endl;

	return 0;
}
