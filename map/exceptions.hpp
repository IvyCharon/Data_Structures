#ifndef SJTU_EXCEPTIONS_HPP
#define SJTU_EXCEPTIONS_HPP

#include <cstddef>
#include <cstring>
#include <string>

namespace sjtu {

class exception {
protected:
	const std::string variant = "";
	std::string detail = "";
public:
	exception() {}
	exception(const exception &ec) : variant(ec.variant), detail(ec.detail) {}
	virtual std::string what() {
		return variant + " " + detail;
	}
};

/**
 * TODO
 * Please complete them.
 */
class index_out_of_bound : public exception {
	const char * mes;
public:
	index_out_of_bound() : mes("index_out_of_bound") {}
};

class runtime_error : public exception {
	const char * mes;
public:
	runtime_error() : mes("runtime_error") {}
};

class invalid_iterator : public exception {
	const char * mes;
public:
	invalid_iterator() : mes("invalid_iterator") {}
};

class container_is_empty : public exception {
	const char * mes;
public:
	container_is_empty() : mes("container_is_empty") {}
};
}

#endif
