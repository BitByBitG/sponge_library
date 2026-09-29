#ifndef SPONGE_FUNCTOR_HPP
#define SPONGE_FUNCTOR_HPP
#include<sponge/core.hpp>
#include<sponge/type_traits.hpp>
namespace sponge
{
	template<typename T,T val>
	struct val_fn
	{
		constexpr T operator()()const{return val;}
	};
	template<typename T>
	struct zero_fn
	{
		constexpr T operator()()const{return (T)(0);}
	};
	template<typename T>
	struct one_fn
	{
		constexpr T operator()()const{return (T)(1);}
	};
	template<typename T,T val>
	struct is_fn
	{
		constexpr bool operator()(const T& x)const{return x==val;}
	};
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
	#define N_DS(B,...) B##_n<__VA_ARGS__>
	#define NN_DS(B,...) B##_nn<__VA_ARGS__>
	#define ASSIGN_ID(T) inf<T>
	template<template<typename...> class Base,typename S=ll,typename T=ll,typename SzTp=int>
	using add_sum_ds=Base<
		S,T,val_fn<S,0>,val_fn<T,0>,is_fn<T,0>,
		plus<S>,range_plus<S,T,SzTp>,plus<T>
	>;
	template<template<typename...> class Base,typename S=ll,typename T=ll,typename SzTp=int>
	using assign_sum_ds=Base<
		S,T,val_fn<S,0>,val_fn<T,ASSIGN_ID(T)>,is_fn<T,ASSIGN_ID(T)>,
		plus<S>,range_assign<S,T,SzTp>,select_second<T>
	>;
	template<template<typename...> class Base,typename S=ll,typename T=ll,typename SzTp=int>
	using add_min_ds=Base<
		S,T,val_fn<S,inf<S>>,val_fn<T,0>,is_fn<T,0>,
		min_fn<S>,m_plus<S,T,SzTp>,plus<T>
	>;
	template<template<typename...> class Base,typename S=ll,typename T=ll,typename SzTp=int>
	using assign_min_ds=Base<
		S,T,val_fn<S,inf<S>>,val_fn<T,ASSIGN_ID(T)>,is_fn<T,ASSIGN_ID(T)>,
		min_fn<S>,m_assign<S,T,SzTp>,select_second<T>
	>;
	template<template<typename...> class Base,typename S=ll,typename T=ll,typename SzTp=int>
	using add_max_ds=Base<
		S,T,val_fn<S,-inf<S>>,val_fn<T,0>,is_fn<T,0>,
		max_fn<S>,m_plus<S,T,SzTp>,plus<T>
	>;
	template<template<typename...> class Base,typename S=ll,typename T=ll,typename SzTp=int>
	using assign_max_ds=Base<
		S,T,val_fn<S,-inf<S>>,val_fn<T,ASSIGN_ID(T)>,is_fn<T,ASSIGN_ID(T)>,
		max_fn<S>,m_assign<S,T,SzTp>,select_second<T>
	>;
}
#endif