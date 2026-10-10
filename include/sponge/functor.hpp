#ifndef SPONGE_FUNCTOR_HPP
#define SPONGE_FUNCTOR_HPP
#include<sponge/core.hpp>
#include<sponge/type_traits.hpp>
namespace sponge
{
	template<typename T,T val>
	struct constant_t
	{
		constexpr T operator()()const{return val;}
	};
	template<typename T>
	struct zero_t
	{
		constexpr T operator()()const{return (T)(0);}
	};
	template<typename T>
	struct one_t
	{
		constexpr T operator()()const{return (T)(1);}
	};
	template<typename T,T val>
	struct is_fn
	{
		constexpr bool operator()(const T& x)const{return x==val;}
	};
	template<typename T>
	struct max_t
	{
		constexpr T operator()(const T& x,const T& y)const{return max<T>(x,y);}
	};
	template<typename T>
	struct min_t
	{
		constexpr T operator()(const T& x,const T& y)const{return min<T>(x,y);}
	};
	template<typename T>
	struct default_id
	{
		constexpr T operator()()const{return T();}
	};
	using null_id=default_id<null_t>;
	struct is_null_id
	{
		constexpr bool operator()(null_t)const{ return true; }
	};
	template<typename S>
	struct select_first
	{
		constexpr S operator()(S x,S)const{ return x; }
	};
	template<typename S>
	struct select_second
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
	struct null_auto
	{
		template<typename... Args>
		constexpr null_t operator()(Args... args[[maybe_unused]])const
		{
			return null_t{};
		}
	};
	template<typename S,typename T,typename SzTp>
	struct range_plus
	{
		constexpr S operator()(const S& x,const T& y,SzTp z)const
		{
			return x+y*z;
		}
	};
	template<typename S,typename T,typename SzTp>
	struct range_assign
	{
		constexpr S operator()(const S& x,const T& y,SzTp z)const
		{
			return y*z;
		}
	};
	template<typename S,typename T,typename SzTp>
	struct m_plus
	{
		constexpr S operator()(const S& x,const T& y,SzTp)const
		{
			return x+y;
		}
	};
	template<typename S,typename T,typename SzTp>
	struct m_assign
	{
		constexpr S operator()(const S& x,const T& y,SzTp)const
		{
			return y;
		}
	};
	template<typename T>
	constexpr T assign_id=inf<T>+1;
	template<template<typename...> class Base,typename S,typename SId,typename SOpSS>
	using n_ds=Base<S,null_t,SId,null_id,is_null_id,SOpSS,ignore_tag<S>,null_op>;
	template<template<typename...> class Base>
	using nn_ds=n_ds<Base,null_t,null_id,null_op>;
	template<template<typename...> class Base,typename S=ll>
	using sum_ds=n_ds<Base,S,constant_t<S,0>,plus<S>>;
	template<template<typename...> class Base,typename S=ll>
	using min_ds=n_ds<Base,S,constant_t<S,inf<S>>,min_t<S>>;
	template<template<typename...> class Base,typename S=ll>
	using max_ds=n_ds<Base,S,constant_t<S,-inf<S>>,max_t<S>>;
	template<template<typename...> class Base,typename S=ll,typename T=ll,typename SzTp=int>
	using add_sum_ds=Base<
		S,T,constant_t<S,0>,constant_t<T,0>,is_fn<T,0>,
		plus<S>,range_plus<S,T,SzTp>,plus<T>
	>;
	template<template<typename...> class Base,typename S=ll,typename T=ll,typename SzTp=int>
	using assign_sum_ds=Base<
		S,T,constant_t<S,0>,constant_t<T,assign_id<T>>,is_fn<T,assign_id<T>>,
		plus<S>,range_assign<S,T,SzTp>,select_second<T>
	>;
	template<template<typename...> class Base,typename S=ll,typename T=ll,typename SzTp=int>
	using add_min_ds=Base<
		S,T,constant_t<S,inf<S>>,constant_t<T,0>,is_fn<T,0>,
		min_t<S>,m_plus<S,T,SzTp>,plus<T>
	>;
	template<template<typename...> class Base,typename S=ll,typename T=ll,typename SzTp=int>
	using assign_min_ds=Base<
		S,T,constant_t<S,inf<S>>,constant_t<T,assign_id<T>>,is_fn<T,assign_id<T>>,
		min_t<S>,m_assign<S,T,SzTp>,select_second<T>
	>;
	template<template<typename...> class Base,typename S=ll,typename T=ll,typename SzTp=int>
	using add_max_ds=Base<
		S,T,constant_t<S,-inf<S>>,constant_t<T,0>,is_fn<T,0>,
		max_t<S>,m_plus<S,T,SzTp>,plus<T>
	>;
	template<template<typename...> class Base,typename S=ll,typename T=ll,typename SzTp=int>
	using assign_max_ds=Base<
		S,T,constant_t<S,-inf<S>>,constant_t<T,assign_id<T>>,is_fn<T,assign_id<T>>,
		max_t<S>,m_assign<S,T,SzTp>,select_second<T>
	>;
}
#endif