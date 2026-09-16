#ifndef SPONGE_BITSET_HPP
#define SPONGE_BITSET_HPP
#include<sponge/core.hpp>
namespace sponge
{
	namespace detail
	{
#include<sponge/ext/Bitset.h>
	}
	template<size_t N>
	using bitset256=detail::Bitset<N>;
}
#endif