#ifndef SPONGE_SEGTREE_HPP
#define SPONGE_SEGTREE_HPP
#include<sponge/core.hpp>
#include<sponge/monoid.hpp>
#include<sponge/functors.hpp>
namespace sponge
{
	template<typename S,typename SId,typename SOpSS>
	class segtree:
		public monoid<S,SId,SOpSS>
	{
	public:
		using M=monoid<S,SId,SOpSS>;
		using M::s_id,M::s_op_s_s;
		int n;
		vector<S> val;
		segtree():n(0){}
		segtree(int n)
		{
			resize(n);
			build([](int){return s_id();});
		}
		template<typename F>
		segtree(int n,F&& f)
		{
			resize(n);
			build(f);
		}
		template<typename RAIte>
		segtree(RAIte first,RAIte last)
		{
			int n=last-first;
			resize(n);
			build([&](int x){return *(first+x-1);});
		}
		INLINE int ls(int x)const
		{
			return x<<1;
		}
		INLINE int rs(int x)const
		{
			return x<<1|1;
		}
		void push_up(const int &x)
		{
			val[x]=s_op_s_s(val[ls(x)],val[rs(x)]);
		}
		template<typename F>
		void build(F&& f)
		{
			auto dfs=[&](auto&& dfs,int x,int l,int r)
			{
				if(l==r)return val[x]=f(l),[]{}();
				int mid=(l+r)>>1;
				dfs(dfs,ls(x),l,mid);
				dfs(dfs,rs(x),mid+1,r);
				push_up(x);
			};
			dfs(dfs,1,1,n);
		}
		void update(int _x,const S& v)
		{
			auto dfs=[&](auto&& dfs,int x,int l,int r)
			{
				if(l==r)return val[x]=v,[]{}();
				int mid=(l+r)>>1;
				if(_x<=mid)dfs(dfs,ls(x),l,mid);
				else dfs(dfs,rs(x),mid+1,r);
				push_up(x);
			};
			dfs(dfs,1,1,n);
		}
		S query(int _l,int _r)const
		{
			if(_l>_r)return s_id();
			auto dfs=[&](auto&& dfs,int x,int l,int r)
			{
				if(_l<=l&&r<=_r)return val[x];
				int mid=(l+r)>>1;
				if(_l<=mid&&mid<_r)return s_op_s_s(dfs(dfs,ls(x),l,mid),dfs(dfs,rs(x),mid+1,r));
				else if(_l<=mid)return dfs(dfs,ls(x),l,mid);
				else return dfs(dfs,rs(x),mid+1,r);
			};
			return dfs(dfs,1,1,n);
		}
		int size()const
		{
			return n;
		}
		void resize(int _n)
		{
			n=_n;
			val.assign((n<<2)+1,s_id());
		}
		void clear()
		{
			n=0;
			val.clear();
		}
	};
	template<
		typename S,typename T,typename SId,typename TId,typename IsTId,
		typename SOpSS,typename SOpST,typename TOpTT
	>
	class lazy_segtree:
		public double_monoid<S,T,SId,TId,IsTId,SOpSS,SOpST,TOpTT>
	{
	public:
		using M=double_monoid<S,T,SId,TId,IsTId,SOpSS,SOpST,TOpTT>;
		using M::s_id,M::t_id,M::is_t_id,M::s_op_s_s,M::s_op_s_t,M::t_op_t_t;
		int n;
		vector<S> val;
		vector<T> tag;
		lazy_segtree():n(0){}
		lazy_segtree(int n)
		{
			resize(n);
			build([](int){return s_id();});
		}
		template<typename F>
		lazy_segtree(int n,F&& f)
		{
			resize(n);
			build(f);
		}
		template<typename RAIte>
		lazy_segtree(RAIte first,RAIte last)
		{
			int n=last-first;
			resize(n);
			build([&](int x){return *(first+x-1);});
		}
		INLINE int ls(int x)const
		{
			return x<<1;
		}
		INLINE int rs(int x)const
		{
			return x<<1|1;
		}
		void push_up(const int &x)
		{
			val[x]=s_op_s_s(val[ls(x)],val[rs(x)]);
		}
		void apply_tag(int x,int l,int r,const T& t)
		{
			val[x]=s_op_s_t(val[x],t,r-l+1);
			tag[x]=t_op_t_t(tag[x],t);
		}
		void push_down(int x,int l,int r)
		{
			if(!is_t_id(tag[x]))
			{
				int mid=(l+r)>>1;
				apply_tag(ls(x),l,mid,tag[x]);
				apply_tag(rs(x),mid+1,r,tag[x]);
				tag[x]=t_id();
			}
		}
		template<typename F>
		void build(F&& f)
		{
			auto dfs=[&](auto&& dfs,int x,int l,int r)
			{
				tag[x]=t_id();
				if(l==r)return val[x]=f(l),[]{}();
				int mid=(l+r)>>1;
				dfs(dfs,ls(x),l,mid);
				dfs(dfs,rs(x),mid+1,r);
				push_up(x);
			};
			dfs(dfs,1,1,n);
		}
		void update(int _l,int _r,const T& t)
		{
			if(_l>_r)return;
			auto dfs=[&](auto&& dfs,int x,int l,int r)
			{
				if(_l<=l&&r<=_r)return apply_tag(x,l,r,t);
				push_down(x,l,r);
				int mid=(l+r)>>1;
				if(_l<=mid)dfs(dfs,ls(x),l,mid);
				if(mid<_r)dfs(dfs,rs(x),mid+1,r);
				push_up(x);
			};
			dfs(dfs,1,1,n);
		}
		S query(int _l,int _r)
		{
			if(_l>_r)return s_id();
			auto dfs=[&](auto&& dfs,int x,int l,int r)
			{
				if(_l<=l&&r<=_r)return val[x];
				push_down(x,l,r);
				int mid=(l+r)>>1;
				if(_l<=mid&&mid<_r)return s_op_s_s(dfs(dfs,ls(x),l,mid),dfs(dfs,rs(x),mid+1,r));
				else if(_l<=mid)return dfs(dfs,ls(x),l,mid);
				else return dfs(dfs,rs(x),mid+1,r);
			};
			return dfs(dfs,1,1,n);
		}
		int size()const
		{
			return n;
		}
		void resize(int _n)
		{
			n=_n;
			val.assign((n<<2)+1,s_id());
			tag.assign((n<<2)+1,t_id());
		}
		void clear()
		{
			n=0;
			val.clear();
			tag.clear();
		}
	};
}
#endif