#ifndef SPONGE_SPARSE_TABLE_HPP
#define SPONGE_SPARSE_TABLE_HPP
#include<sponge/core.hpp>
#include<sponge/container.hpp>
#include<sponge/functor.hpp>
#include<sponge/monoid.hpp>
namespace sponge
{
	template<typename S,typename SId,typename SOpSS>
	class sparse_table:monoid<S,SId,SOpSS>
	{
	public:
		using M=monoid<S,SId,SOpSS>;
		using M::s_id,M::s_op_s_s;
		using s_cr=typename M::s_cr;
		int n,lg;
		multidimension<S,2> st;
		sparse_table():sparse_table(0){}
		sparse_table(int n):sparse_table(n,[](int){return s_id();}){}
		template<typename F>
		sparse_table(int _n,F&& f)
		{
			resize(_n);
			for(int i=1;i<=n;i++)st[0][i]=f(i);
			for(int i=1;i<=lg;i++)
				for(int j=1;j<=n-(1<<i)+1;j++)
					st[i][j]=s_op_s_s(st[i-1][j],st[i-1][j+(1<<(i-1))]);
		}
		void resize(int _n)
		{
			n=_n;
			lg=__lg(n);
			st.resize(lg+1,n+1);
		}
		S query(int l,int r)
		{
			int k=__lg(r-l+1);
			return s_op_s_s(st[k][l],st[k][r-(1<<k)+1]);
		}
	};
	template<
		typename S,typename T,typename SId,typename TId,typename IsTId,
		typename SOpSS,typename SOpST,typename TOpTT
	>
	using st_template=sparse_table<S,SId,SOpSS>;
}
#endif