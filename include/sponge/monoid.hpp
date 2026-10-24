#ifndef SPONGE_MONOID_HPP
#define SPONGE_MONOID_HPP
#include<sponge/core.hpp>
namespace sponge
{
	template<typename S,typename SId,typename SOpSS,typename SizeType=int>
	class monoid
	{
#if __cplusplus>=201703L
		static_assert(is_invocable_r_v<S,SId>);
		static_assert(is_invocable_r_v<S,SOpSS,S,S>);
#endif
	public:
		using value_type=S;
		using size_type=SizeType;
		using s_cr=const S&;
		SId s_id{};
		SOpSS s_op_s_s{};
		monoid()=default;
		monoid(SId f0,SOpSS f1):s_id(move(f0)),s_op_s_s(move(f1)){}
	};
	template<
		typename S,typename T,typename SId,typename TId,typename IsTId,
		typename SOpSS,typename SOpST,typename TOpTT,typename SizeType=int
	>
	class double_monoid
	{
#if __cplusplus>=201703L
		static_assert(is_invocable_r_v<S,SId>);
		static_assert(is_invocable_r_v<T,TId>);
		static_assert(is_invocable_r_v<bool,IsTId,T>);
		static_assert(is_invocable_r_v<S,SOpSS,S,S>);
		static_assert(is_invocable_r_v<S,SOpST,S,T,SizeType>);
		static_assert(is_invocable_r_v<T,TOpTT,T,T>);
#endif
	public:
		using value_type=S;
		using tag_type=T;
		using size_type=SizeType;
		using s_cr=const S&;
		using t_cr=const T&;
		SId s_id{};
		TId t_id{};
		IsTId is_t_id{};
		SOpSS s_op_s_s{};
		SOpST s_op_s_t{};
		TOpTT t_op_t_t{};
		double_monoid()=default;
		double_monoid(SId f0,TId f1,IsTId f2,SOpSS f3,SOpST f4,TOpTT f5):s_id(move(f0)),t_id(move(f1)),is_t_id(move(f2)),s_op_s_s(move(f3)),s_op_s_t(move(f4)),t_op_t_t(move(f5)){}
	};
}
#endif