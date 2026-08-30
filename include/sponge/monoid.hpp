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
		inline static constexpr SId s_id{};
		inline static constexpr SOpSS s_op_s_s{};
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
		inline static constexpr SId s_id{};
		inline static constexpr TId t_id{};
		inline static constexpr IsTId is_t_id{};
		inline static constexpr SOpSS s_op_s_s{};
		inline static constexpr SOpST s_op_s_t{};
		inline static constexpr TOpTT t_op_t_t{};
	};
}
#endif