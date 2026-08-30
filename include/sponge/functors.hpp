#ifndef SPONGE_FUNCTORS_HPP
#define SPONGE_FUNCTORS_HPP
#include<sponge/core.hpp>
namespace sponge
{
	template<typename T,T val>
	struct val_fn
	{
		constexpr T operator()()const{return val;}
	};
	template<typename T>
	using zero_fn=val_fn<T,(T)(0)>;
	template<typename T>
	using one_fn=val_fn<T,(T)(1)>;
	template<typename T>
	struct max_fn
	{
		constexpr T operator()(const T& x,const T& y)const{return max<T>(x,y);}
	};
	template<typename T>
	struct min_fn
	{
		constexpr T operator()(const T& x,const T& y)const{return min<T>(x,y);}
	};
	using null_id=val_fn<null_t,null_t{}>;
	struct is_null_id
	{
		constexpr bool operator()(null_t)const{ return true; }
	};
	template<typename S>
	struct ordered_op
	{
		constexpr S operator()(S,S y)const{ return y; }
	};
	template<typename S,typename SizeType=int>
	struct ignore_tag
	{
		constexpr S operator()(S x,null_t,SizeType)const{ return x; }
	};
	struct null_op
	{
		constexpr null_t operator()(null_t,null_t)const{ return null_t{}; }
	};
}
#endif