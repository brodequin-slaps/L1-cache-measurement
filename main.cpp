#include <iostream>
#include <chrono>
#include <array>
#include <math.h>
#include <tuple>
#include <utility>
#include <random>

using namespace std;

template<uint64_t base, uint64_t power>
constexpr uint64_t gpow()
{
	if constexpr (power > 1)
	{
		return base * gpow<base, power - 1>();
	}
	else if (power == 1)
	{
		return base;
	}
	else if (power == 0)
	{
		return 1;
	}
	else return 1;
}

template<size_t... I>
constexpr auto get3_impl(index_sequence<I...>)
{
	return make_tuple(I...);
}

template<std::size_t N, typename Indices = make_index_sequence<N>>
constexpr auto get3()
{
	return get3_impl(Indices{});
}

template<typename First>
constexpr auto lastOfArgs(First first)
{
	return first;
}


template<typename First, typename... Last>
constexpr auto lastOfArgs(First first, Last... last)
{
	return lastOfArgs<Last...>(last...);
}

template<bool cond, typename TrueType, typename FalseType>
struct gcond
{};

template<typename TrueType, typename FalseType>
struct gcond<true, typename TrueType, typename FalseType>
{
	using type = typename TrueType;
};

template<typename TrueType, typename FalseType>
struct gcond<false, typename TrueType, typename FalseType>
{
	using type = typename FalseType;
};

template<bool cond, typename TrueType, typename FalseType>
struct gcond_idx
{};

template<typename TrueType, typename FalseType>
struct gcond_idx<true, typename TrueType, typename FalseType>
{
	using type = typename TrueType;
};

template<typename TrueType, typename FalseType>
struct gcond_idx<false, typename TrueType, typename FalseType>
{
	using type = typename FalseType::type;
};

template<typename T, typename ToAppend>
struct append_T{};

template<template<typename...> typename T, typename... Ts, typename ToAppend>
struct append_T<T<Ts...>, ToAppend>
{
	using type = T<Ts..., ToAppend>;
};

template<typename T>
struct remove_first
{};

template<template<auto...> typename T, auto First, auto... Rest>
struct remove_first<T<First, Rest...>>
{
	using type = T<Rest...>;
};

template<template<typename...> typename T, typename First, typename... Rest>
struct remove_first<T<First, Rest...>>
{
	using type = T<Rest...>;
};

template<size_t max, size_t... Args>
struct inner_indexes_2
{
	using type = typename gcond_idx< lastOfArgs(Args...) == max - 1, inner_indexes_2<max, Args...>, inner_indexes_2<max, Args..., lastOfArgs(Args...) + 1>>::type;
};

template<size_t max>
struct inner_indexes_2<max>
{
	using type = typename remove_first<typename inner_indexes_2<max, 0>::type>::type;
};

template<size_t... Args>
struct inner_indexes_2<0, Args...>
{
	using type = inner_indexes_2<0, Args...>;
};



template<size_t... I>
constexpr auto sam_get_impl()
{
	return make_tuple(I...);
}

template<size_t N>
constexpr auto sam_get()
{
	return sam_get_impl();
}

template<int N, typename... Types>
void print_tuple(tuple<Types...> tup)
{
	if constexpr (N < 0)
	{
		return;
	}
	else
	{
		cout << std::get<N>(tup) << endl;
		print_tuple<N - 1, Types...>(tup);
		return;
	}
}

template<typename T>
struct conv
{
	conv(T t) : t(t) { };

	T t;

	friend ostream& operator<<(ostream& os, conv<T> const& conv)
	{
		os << conv.t << endl;
		return os;
	}
};

template<typename Tup, size_t... I>
void print_tuple2_impl(Tup const& tup, index_sequence<I...>)
{
	(cout << ... << conv{ get<I>(tup) });
}

template<typename... Types>
void print_tuple2(tuple<Types...> const& tup)
{
	//(cout << ... << conv{ get< make_index_sequence<sizeof...(Types)>{}>(tup) });
	print_tuple2_impl(tup, make_index_sequence<sizeof...(Types)>{});
}

template<typename Tup, size_t N>
struct gen_tuple_T
{
	using type = typename gen_tuple_T<typename append_T<Tup, array<uint64_t, gpow<2, N>()>>::type, N-1>::type;
};

template<typename Tup>
struct gen_tuple_T<Tup, 0>
{
	using type = Tup;
};

template<typename T, size_t N>
void assignXTimes(array<T, N>& arr, uint64_t x, mt19937_64& engine)
{
	auto start = std::chrono::steady_clock::now();
	for (uint64_t i = 0; i < x; i++)
	{
		uint64_t rand_n = engine();
		uint64_t rand_n_2 = engine();
		uint64_t idx = rand_n % arr.size();
		uint64_t idx2 = rand_n_2 % arr.size();

		if (rand_n % 2)
		{
			arr[idx] = rand_n % 1000;
		}
		else
		{
			arr[idx] = arr[idx2];
		}
	}
	auto end = std::chrono::steady_clock::now();
	std::chrono::duration<double> elapsed_seconds = end - start;
	std::cout << "elapsed time for " << arr.size() << " size: " << elapsed_seconds.count() << "s\n";
}

int main()
{
	//auto xx = getArrs<4>();

	auto xx = get3<10>();
	
	//print_tuple<4>(xx);
	print_tuple2(xx);

	inner_indexes_2<20>::type x;
	remove_first<tuple<int, double, char>>::type x2;

	append_T<tuple<int, double>, char>::type x3;

	gen_tuple_T<tuple<>, 26>::type x4;

	mt19937_64 engine(time(0));
	apply([&engine](auto... arrays)
		{
			(assignXTimes(arrays, 50000000, engine), ...);
		}, x4);

	return 0;
}