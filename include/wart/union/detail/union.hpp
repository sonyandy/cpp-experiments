#ifndef WART_UNION_DETAIL_HPP
#define WART_UNION_DETAIL_HPP

#include "../../all.hpp"
#include "../../enable_if_move_constructible.hpp"

#include <type_traits>
#include <utility>

namespace wart { namespace detail { namespace union_t {

template <bool, typename...>
union union_t;

template <typename, typename...>
struct union_cast;

template <typename T>
union union_t<true, T> {
private:
	T head_;

public:
	constexpr union_t():
		head_() {}

	constexpr union_t(T const& head):
		head_(head) {}

	constexpr union_t(T&& head):
		head_(std::move(head)) {}

	friend
	struct union_cast<T, T>;
};

template <typename T>
union union_t<false, T> {
private:
	T head_;

public:
	constexpr union_t():
		head_() {}

	constexpr union_t(T const& head):
		head_(head) {}

	constexpr union_t(T&& head):
		head_(std::move(head)) {}

	~union_t() {}

	friend
	struct union_cast<T, T>;
};

template <typename Head, typename... Tail>
union union_t<true, Head, Tail...> {
private:
	Head head_;
	union_t<true, Tail...> tail_;

public:
	constexpr union_t():
		head_() {}

	constexpr union_t(Head const& head):
		head_(head) {}

	constexpr union_t(Head&& head):
		head_(std::move(head)) {}

	template <typename T>
	constexpr union_t(T const& tail):
		tail_(tail) {}

	template <typename T>
	constexpr union_t(T&& tail,
	                  typename enable_if_move_constructible<T>::type* = nullptr):
		tail_(std::move(tail)) {}

	template <typename Elem, typename... T>
	friend
	struct union_cast;
};


template <typename Head, typename... Tail>
union union_t<false, Head, Tail...> {
private:
	Head head_;
	union_t<
		all<std::is_trivially_destructible<Tail>::value...>::value,
		Tail...> tail_;

public:
	constexpr union_t():
		head_() {}

	constexpr union_t(Head const& head):
		head_(head) {}

	constexpr union_t(Head&& head):
		head_(std::move(head)) {}

	template <typename T>
	constexpr union_t(T const& tail):
		tail_(tail) {}

	template <typename T>
	constexpr union_t(T&& tail,
	                  typename enable_if_move_constructible<T>::type* = nullptr):
		tail_(std::move(tail)) {}

	~union_t() {}

	template <typename Elem, typename... T>
	friend
	struct union_cast;
};

template <typename Head, typename... Tail>
struct union_cast<Head, Head, Tail...> {
	template <bool B>
	static constexpr Head& call(union_t<B, Head, Tail...>& value) {
		return value.head_;
	}

	template <bool B>
	static constexpr Head const& call(union_t<B, Head, Tail...> const& value) {
		return value.head_;
	}

	template <bool B>
	static constexpr Head&& call(union_t<B, Head, Tail...>&& value) {
		return std::move(value).head_;
	}
};

template <typename Elem, typename Head, typename... Tail>
struct union_cast<Elem, Head, Tail...> {
	template <bool B>
	static constexpr Elem& call(union_t<B, Head, Tail...>& value) {
		return union_cast<Elem, Tail...>::call(value.tail_);
	}

	template <bool B>
	static constexpr Elem const& call(union_t<B, Head, Tail...> const& value) {
		return union_cast<Elem, Tail...>::call(value.tail_);
	}

	template <bool B>
	static constexpr Elem&& call(union_t<B, Head, Tail...>&& value) {
		return union_cast<Elem, Tail...>::call(std::move(value).tail_);
	}
};

}}}

#endif
