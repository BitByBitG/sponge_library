#ifndef SPONGE_SEGTREE_HPP
#define SPONGE_SEGTREE_HPP
#include<sponge/core.hpp>
#include<sponge/monoid.hpp>
#include<sponge/functors.hpp>
namespace sponge
{
	template<
		typename S,typename T,typename SId,typename TId,typename IsTId,
		typename SOpSS,typename SOpST,typename TOpTT
	>
	class segtree:
		public double_monoid<S,T,SId,TId,IsTId,SOpSS,SOpST,TOpTT>
	{
	public:
		using M=double_monoid<S,T,SId,TId,IsTId,SOpSS,SOpST,TOpTT>;
		using M::s_id,M::t_id,M::is_t_id,M::s_op_s_s,M::s_op_s_t,M::t_op_t_t;
		using Scr=M::Scr;
		using Tcr=M::Tcr;
		int n,sz,lg;
		vector<S> val;
		vector<T> tag;
		segtree():segtree(0){}
		segtree(int _n):segtree(_n,[](int){return s_id();}){}
		template<typename F>
		segtree(int _n,F&& f):n(_n)
		{
			sz=bit_ceil<uint32_t>(n);
			lg=countr_zero<uint32_t>(sz);
			val.assign(sz<<1,s_id());
			tag.assign(sz,t_id());
			for(int i=0;i<n;i++)val[sz+i]=f(i+1);
			for(int i=sz-1;i>=1;i--)push_up(i);
		}
		INLINE int ls(int x)const
		{
			return x<<1;
		}
		INLINE int rs(int x)const
		{
			return x<<1|1;
		}
		void push_up(int x)
		{
			val[x]=s_op_s_s(val[ls(x)],val[rs(x)]);
		}
		void apply_tag(int x,Tcr t,int len)
		{
			val[x]=s_op_s_t(val[x],t,len);
			if(x<sz)tag[x]=t_op_t_t(tag[x],t);
		}
		void push_down(int x,int len)
		{
			if(!is_t_id(tag[x]))
			{
				apply_tag(ls(x),tag[x],len);
				apply_tag(rs(x),tag[x],len);
				tag[x]=t_id();
			}
		}
		void set(int p,Scr s)
		{
			p--;
			p+=sz;
			for(int i=lg;i>=1;i--)push_down(p>>i,1<<i);
			val[p]=s;
			for(int i=1;i<=lg;i++)push_up(p>>i);
		}
		S query(int p)
		{
			p--;
			p+=sz;
			for(int i=lg;i>=1;i--)push_down(p>>i,1<<i);
			return val[p];
		}
		S query(int l,int r)
		{
			l--;
			if(l==r)return s_id();
			l+=sz;
			r+=sz;
			for(int i=lg;i>=1;i--)
			{
				int len=1<<i;
				if(((l>>i)<<i)!=l)push_down(l>>i,len);
				if(((r>>i)<<i)!=r)push_down((r-1)>>i,len);
			}
			S vl=s_id(),vr=s_id();
			while(l<r)
			{
				if(l&1)vl=s_op_s_s(vl,val[l++]);
				if(r&1)vr=s_op_s_s(val[--r],vr);
				l>>=1;
				r>>=1;
			}
			return s_op_s_s(vl,vr);
		}
		S query()
		{
			return val[1];
		}
		void update(int p,Tcr t)
		{
			if(is_t_id(t))return;
			p--;
			p+=sz;
			for(int i=lg;i>=1;i--)push_down(p>>i,1<<i);
			val[p]=s_op_s_t(val[p],t,1);
			for(int i=1;i<=lg;i++)push_up(p>>i);
		}
		void update(int l,int r,Tcr t)
		{
			if(is_t_id(t))return;
			l--;
			if(l==r)return;
			l+=sz;
			r+=sz;
			for(int i=lg;i>=1;i--)
			{
				int len=1<<i;
				if(((l>>i)<<i)!=l)push_down(l>>i,len);
				if(((r>>i)<<i)!=r)push_down((r-1)>>i,len);
			}
			int l2=l,r2=r;
			int len=1;
			while(l<r)
			{
				if(l&1)apply_tag(l++,t,len);
				if(r&1)apply_tag(--r,t,len);
				l>>=1;
				r>>=1;
				len<<=1;
			}
			l=l2;
			r=r2;
			for(int i=1;i<=lg;i++)
			{
				if(((l>>i)<<i)!=l)push_up(l>>i);
				if(((r>>i)<<i)!=r)push_up((r-1)>>i);
			}
		}
		template<bool (*f)(S)>
		int max_right(int l)
		{
			return max_right(l,[](S x){return f(x);});
		}
		template<typename F>
		int max_right(int l,F&& f)
		{
			l--;
			if(l==n)return n;
			l+=sz;
			for(int i=lg;i>=1;i--)push_down(l>>i,1<<i);
			S v=s_id();
			do
			{
				while(!(l&1))l>>=1;
				if(!f(s_op_s_s(v,val[l])))
				{
					int len=sz>>(bit_width<uint32_t>(l)-1);
					while(l<sz)
					{
						push_down(l,len);
						l<<=1;
						len>>=1;
						S tmp=s_op_s_s(v,val[l]);
						if(f(tmp))
						{
							v=tmp;
							l++;
						}
					}
					return l-sz;
				}
				v=s_op_s_s(v,val[l]);
				l++;
			}while((l&-l)!=l);
			return n;
		}
		template<bool (*f)(S)>
		int min_left(int r)
		{
			return min_left(r,[](S x){return f(x);});
		}
		template<typename F>
		int min_left(int r,F&& f)
		{
			if(r==0)return 1;
			r+=sz;
			for(int i=lg;i>=1;i--)push_down((r-1)>>i,1<<i);
			S v=s_id();
			do
			{
				r--;
				while(r>1&&(r&1))r>>=1;
				if(!f(s_op_s_s(val[r],v)))
				{
					int len=sz>>(bit_width<uint32_t>(r)-1);
					while(r<sz)
					{
						push_down(r,len);
						r=r<<1|1;
						len>>=1;
						S tmp=s_op_s_s(val[r],v);
						if(f(tmp))
						{
							v=tmp;
							r--;
						}
					}
					return r-sz+2;
				}
				v=s_op_s_s(val[r],v);
			}while((r&-r)!=r);
			return 1;
		}
	};
	template<typename S,typename SId,typename SOpSS>
	using segtree_n=segtree<
		S,null_t,SId,null_id,is_null_id,
		SOpSS,ignore_tag<S>,null_op
	>;
}
#endif