#include "cxxabi.h"

#include <iostream>
#include <memory>
#include <stdexcept>

struct invalid_name: std::invalid_argument {
	using std::invalid_argument::invalid_argument;
};

std::unique_ptr<char, void (*)(void*)> demangle(char const* name) {
	using namespace abi;
	using namespace std;
	int status;
	unique_ptr<char, void (*)(void*)> result(__cxa_demangle(name,
	                                                        nullptr,
	                                                        nullptr,
	                                                        &status),
	                                         free);
	switch (status) {
	case 0: return result;
	case -1: throw bad_alloc();
	case -2: throw invalid_name("not a valid name under the C++ ABI mangling rules");
	case -3: throw invalid_argument("one of the arguments is invalid");
	default: throw logic_error("unexpected status");
	}
}

int main(int argc, char* argv[]) {
	using namespace std;
	if (argc != 2) {
		cerr << argv[0] << " " << "NAME" << endl;
		return 1;
	}
	try {
		cout << demangle(argv[1]).get() << endl;
	} catch (invalid_name const& e) {
		cerr << e.what() << endl;
		return 1;
	}
	return 0;
}
