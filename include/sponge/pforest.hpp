#ifndef SPONGE_PFOREST_HPP
#define SPONGE_PFOREST_HPP
#include<sponge/core.hpp>
#include<sponge/monoid.hpp>
#include<sponge/functor.hpp>
namespace sponge
{
	template<
		typename S,typename T,typename SId,typename TId,typename IsTId,
		typename SOpSS,typename SOpST,typename TOpTT,typename SizeType=int
	>
	class pforest:
		public double_monoid<S,T,SId,TId,IsTId,SOpSS,SOpST,TOpTT,SizeType>
	{
	public:
		using Sz=SizeType;
		using size_type=Sz;
		using M=double_monoid<S,T,SId,TId,IsTId,SOpSS,SOpST,TOpTT,SizeType>;
		using M::s_id,M::t_id,M::is_t_id,M::s_op_s_s,M::s_op_s_t,M::t_op_t_t;
		using s_cr=typename M::s_cr;
		using t_cr=typename M::t_cr;
		struct node_t
		{
#ifdef SPONGE_PTREE_FA
			int fa;
#endif
			array<int,2> son;
			Sz size;
#ifndef SPONGE_PTREE_NREV
			bool rev;
#endif
			S val;
#ifndef SPONGE_PTREE_NTAG
			T tag;
#endif
			int ref;
			node_t():node_t(M{}){}
			node_t(const M& m):
#ifdef SPONGE_PTREE_FA
				fa(),
#endif
				son{},size(),
#ifndef SPONGE_PTREE_NREV
				rev(),
#endif
				val(m.s_id()),
#ifndef SPONGE_PTREE_NTAG
				tag(m.t_id()),
#endif
				ref()
			{}
		};
		int n,cap;
		node_t* nd;
		vector<int> stk;
		pforest():pforest(M{}){}
		explicit pforest(const M& m):M(m),n(0),cap(0),nd(nullptr){}
		pforest(int _cap):pforest(_cap,M{}){}
		pforest(int _cap,const M& m):pforest(m)
		{
			reserve(_cap);
		}
		void destroy_nodes()
		{
			if(!nd)return;
			for(int i=0;i<cap;i++)nd[i].~node_t();
			allocator<node_t>{}.deallocate(nd,cap);
			nd=nullptr;
		}
		~pforest()
		{
			destroy_nodes();
		}
		node_t& operator[](int x)
		{
			return nd[x];
		}
		int size()const
		{
			return n;
		}
		int capacity()const
		{
			return cap;
		}
		inline int& ls(int x)
		{
			return nd[x].son[0];
		}
		inline int& rs(int x)
		{
			return nd[x].son[1];
		}
		void reserve(int _cap)
		{
			if(nd!=nullptr)destroy_nodes();
			n=0;
			cap=_cap;
			stk.clear();
			nd=allocator<node_t>{}.allocate(cap);
			int i=0;
			try
			{
				for(;i<cap;i++)::new((void*)(nd+i)) node_t(*this);
			}
			catch(...)
			{
				while(i)nd[--i].~node_t();
				allocator<node_t>{}.deallocate(nd,cap);
				nd=nullptr;
				throw;
			}
		}
		int alloc()
		{
			int x;
			if(!stk.empty())
			{
				x=stk.back();
				stk.pop_back();
			}
			else
			{
				x=++n;
				ASSERT(x<cap);
			}
			nd[x]=node_t(*this);
			return x;
		}
		void dealloc(int x)
		{
			if(!x||--nd[x].ref)return;
			if(ls(x))dealloc(ls(x));
			if(rs(x))dealloc(rs(x));
			stk.push_back(x);
		}
		int copy(int x)
		{
			int y=alloc();
			nd[y]=nd[x];
			return y;
		}
		int leaf(s_cr v)
		{
			int x=alloc();
			nd[x].size=1;
			nd[x].val=v;
			nd[x].ref=1;
			return x;
		}
		void push_up(int x)
		{
			nd[x].size=nd[ls(x)].size+nd[rs(x)].size;
			nd[x].val=s_op_s_s(nd[ls(x)].val,nd[rs(x)].val);
		}
#ifndef SPONGE_PTREE_NTAG
		void apply_tag(int& x,t_cr v)
		{
			if(!x)return;
			nd[x].val=s_op_s_t(nd[x].val,v,nd[x].size);
			nd[x].tag=t_op_t_t(nd[x].tag,v);
		}
#endif
		void refresh(int& x)
		{
			if(!x||nd[x].ref==1)return;
			int old=x;
			int y=copy(old);
			nd[y].ref=1;
			if(ls(y))++nd[ls(y)].ref;
			if(rs(y))++nd[rs(y)].ref;
			dealloc(old);
			x=y;
		}
		void push_down(int x[[maybe_unused]])
		{
#ifndef SPONGE_PTREE_NTAG
			if(!x||nd[x].size==1)return;
			if(!is_t_id(nd[x].tag)
#ifndef SPONGE_PTREE_NREV
				||nd[x].rev
#endif
			)
			{
				refresh(ls(x)),refresh(rs(x));
				if(!is_t_id(nd[x].tag))
				{
					apply_tag(ls(x),nd[x].tag);
					apply_tag(rs(x),nd[x].tag);
					nd[x].tag=t_id();
				}
#ifndef SPONGE_PTREE_NREV
				if(nd[x].rev)
				{
					swap(ls(x),rs(x));
					nd[ls(x)].rev^=1;
					nd[rs(x)].rev^=1;
					nd[x].rev=0;
				}
#endif
			}
#endif
		}
		int _merge(int x,int y)
		{
			if(!x||!y)return x|y;
			int z=alloc();
			nd[z].ref=1;
#ifdef SPONGE_PTREE_FA
			nd[x].fa=nd[y].fa=z;
#endif
			ls(z)=x,rs(z)=y;
			push_up(z);
			return z;
		}
		pair<int,int> _split(int& x)
		{
			if(!x)return {0,0};
			push_down(x);
			int y=ls(x),z=rs(x);
			if(y)++nd[y].ref;
			if(z)++nd[z].ref;
#ifdef SPONGE_PTREE_FA
			nd[y].fa=nd[z].fa=0;
#endif
			dealloc(x);
			x=0;
			return {y,z};
		}
#ifndef SPONGE_PTREE_UNBALANCED
		void rotate(int& x,bool r)
		{
			auto[a,b]=_split(x);
			if(r)
			{
				auto[c,d]=_split(b);
				x=_merge(_merge(a,c),d);
			}
			else
			{
				auto[c,d]=_split(a);
				x=_merge(c,_merge(d,b));
			}
		}
		static constexpr double alpha=0.292;
		bool unbalanced(int x,int y)
		{
			return nd[y].size<alpha*(nd[x].size+nd[y].size);
		}
		bool need_double_rotation(int x,bool r)
		{
			return nd[nd[x].son[!r]].size>nd[x].size/(2-alpha);
		}
		void balance(int& x)
		{
			if(nd[x].size==1)return;
			bool r=nd[rs(x)].size>nd[ls(x)].size;
			if(!unbalanced(nd[x].son[r],nd[x].son[!r]))return;
			if(nd[nd[x].son[r]].size>1&&need_double_rotation(nd[x].son[r],r))
				rotate(nd[x].son[r],!r);
			rotate(x,r);
		}
#endif
		int merge(int x,int y)
		{
#ifndef SPONGE_PTREE_UNBALANCED
			if(!x||!y)return x|y;
			if(unbalanced(x,y))
			{
				auto[a,b]=_split(x);
				int z=_merge(a,merge(b,y));
				balance(z);
				return z;
			}
			else if(unbalanced(y,x))
			{
				auto[a,b]=_split(y);
				int z=_merge(merge(x,a),b);
				balance(z);
				return z;
			}
			else return _merge(x,y);
#else
			return _merge(x,y);
#endif
		}
		pair<int,int>split(int x,Sz k)
		{
			if(!k)return {0,x};
			else if(k==nd[x].size)return {x,0};
			else
			{
				int a,b,c,d;
				tie(a,b)=_split(x);
				if(k<=nd[a].size)
				{
					tie(c,d)=split(a,k);
					return {c,merge(d,b)};
				}
				else
				{
					tie(c,d)=split(b,k-nd[a].size);
					return {merge(a,c),d};
				}
			}
		}
		template<typename F>
		int build(int n,F&& f)
		{
			if(n==0)return 0;
			auto dfs=[&](auto&& dfs,int l,int r)->int
			{
				if(l==r)return leaf(f(l));
				int mid=(l+r)>>1;
				return _merge(dfs(dfs,l,mid),dfs(dfs,mid+1,r));
			};
			return dfs(dfs,1,n);
		}
		template<typename RAIter>
		int build(RAIter first,RAIter last)
		{
			return build(last-first,[&](int x){return *(first+x-1);});
		}
		array<int,3> access(int x,Sz l,Sz r)
		{
			int a,b;
			tie(x,b)=split(x,r);
			tie(a,x)=split(x,l-1);
			return {a,x,b};
		}
#ifndef SPONGE_PTREE_NTAG
		void update(int& x,Sz l,Sz r,t_cr t)
		{
			auto[a,y,b]=access(x,l,r);
			refresh(y);
			apply_tag(y,t);
			x=merge(a,merge(y,b));
		}
#endif
		void set_pos(int& x,Sz p,s_cr v)
		{
			auto[a,y,b]=access(x,p,p);
			refresh(y);
			deref(y)=v;
			x=merge(a,merge(y,b));
		}
#ifndef SPONGE_PTREE_NREV
		void reverse(int& x,Sz l,Sz r)
		{
			auto[a,y,b]=access(x,l,r);
			refresh(y);
			nd[y].rev^=1;
			x=merge(a,merge(y,b));
		}
#endif
		S query(int x,Sz p)
		{
			return pos(x,p);
		}
		S query(int& x,Sz l,Sz r)
		{
			auto[a,y,b]=access(x,l,r);
			S ans=nd[y].val;
			x=merge(a,merge(y,b));
			return ans;
		}
		S pos(int x,Sz p)
		{
			while(nd[x].size>1)
			{
				push_down(x);
				if(nd[ls(x)].size>=p)x=ls(x);
				else p-=nd[ls(x)].size,x=rs(x);
			}
			return nd[x].val;
		}
		vector<S> range(int& x,Sz l,Sz r)
		{
			auto[a,b,c]=access(x,l,r);
			vector<S> ans=seq(b);
			x=merge(a,merge(b,c));
			return ans;
		}
		vector<S> seq(int x)
		{
			vector<S> ans;
			auto dfs=[&](auto&& dfs,int x)->void
			{
				if(!x)return;
				if(nd[x].size==1)return ans.push_back(nd[x].val),[]{}();
				push_down(x);
				dfs(dfs,ls(x));
				dfs(dfs,rs(x));
			};
			dfs(dfs,x);
			return ans;
		}
		int insert_after(int& x,Sz p,s_cr v)
		{
			auto[a,b]=split(x,p);
			int ans=leaf(v);
			x=merge(a,merge(ans,b));
			return ans;
		}
		void erase_pos(int& x,Sz p)
		{
			auto[a,y,b]=access(x,p,p);
			x=merge(a,b);
			dealloc(y);
		}
		int derive(int x)
		{
			if(!x)return 0;
			nd[x].ref++;
			int y=x;
			refresh(y);
			return y;
		}
		int size(int x)
		{
			return nd[x].size;
		}
		S& deref(int x)
		{
			return nd[x].val;
		}
	};
	template<
		typename Compare,
		typename S,typename T,typename SId,typename TId,typename IsTId,
		typename SOpSS,typename SOpST,typename TOpTT,typename SizeType=int
	>
	class ordered_pforest:
		public pforest<S,T,SId,TId,IsTId,SOpSS,SOpST,TOpTT,SizeType>
	{
	public:
		using Sz=SizeType;
		using size_type=Sz;
		using B=pforest<S,T,SId,TId,IsTId,SOpSS,SOpST,TOpTT,SizeType>;
		using s_cr=typename B::s_cr;
		using t_cr=typename B::t_cr;
		Compare cmp{};
		using M=typename B::M;
		ordered_pforest()=default;
		explicit ordered_pforest(Compare c):cmp(move(c)){}
		explicit ordered_pforest(const M& m):B(m){}
		ordered_pforest(Compare c,const M& m):B(m),cmp(move(c)){}
		ordered_pforest(int n,const M& m):B(n,m){}
		ordered_pforest(int n,Compare c):B(n),cmp(move(c)){}
		ordered_pforest(int n,Compare c,const M& m):B(n,m),cmp(move(c)){}
		bool cmpeq(s_cr x,s_cr y)
		{
			return !cmp(x,y)&&!cmp(y,x);
		}
		int find(int t,s_cr v)
		{
			if(!t)return 0;
			int x=t;
			while(B::nd[x].size>1)
			{
				B::push_down(x);
				if(!cmp(B::deref(B::ls(x)),v))x=B::ls(x);
				else x=B::rs(x);
			}
			if(cmpeq(B::deref(x),v))return x;
			else return 0;
		}
		int find_by_order(int t,Sz k)
		{
			if(!t||k<=0||k>B::nd[t].size)return 0;
			int x=t;
			while(B::nd[x].size>1)
			{
				B::push_down(x);
				if(B::nd[B::ls(x)].size>=k)x=B::ls(x);
				else k-=B::nd[B::ls(x)].size,x=B::rs(x);
			}
			return x;
		}
		Sz order_of_key(int t,s_cr v)
		{
			if(!t)return 1;
			int x=t;
			Sz ans=0;
			while(B::nd[x].size>1)
			{
				B::push_down(x);
				if(!cmp(B::deref(B::ls(x)),v))x=B::ls(x);
				else ans+=B::nd[B::ls(x)].size,x=B::rs(x);
			}
			if(cmp(B::deref(x),v))ans++;
			return ++ans;
		}
		int lower_bound(int t,const S &v)
		{
			int x=t;
			if(!x)return 0;
			while(B::nd[x].size>1)
			{
				B::push_down(x);
				if(cmp(B::deref(B::nd[B::ls(x)]),v))x=B::rs(x);
				else x=B::ls(x);
			}
			if(cmp(B::deref(x),v))return 0;
			return x;
		}
		int upper_bound(int t,s_cr v)
		{
			return successor(t,v);
		}
		int predecessor(int t,s_cr v)
		{
			return find_by_order(t,order_of_key(t,v)-1);
		}
		int successor(int t,s_cr v)
		{
			if(!t)return 0;
			int x=t;
			while(B::nd[x].size>1)
			{
				B::push_down(x);
				if(cmp(v,B::deref(B::ls(x))))x=B::ls(x);
				else x=B::rs(x);
			}
			if(!cmp(v,B::deref(x)))return 0;
			return x;
		}
		int insert(int& t,s_cr v)
		{
			auto[a,b]=vsplit(t,v);
			int ans=B::leaf(v);
			t=B::merge(a,B::merge(ans,b));
			return ans;
		}
		bool erase(int& t,s_cr v)
		{
			if(!find(t,v))return 0;
			B::erase_pos(t,order_of_key(t,v));
			return 1;
		}
		pair<int,int> vsplit(int x,s_cr k)
		{
			if(!x)return {0,0};
			if(!cmp(k,B::deref(x)))return {x,0};
			if(B::nd[x].size==1)return {0,x};
			auto[a,b]=B::_split(x);
			if(cmp(k,B::deref(a)))
			{
				auto[c,d]=vsplit(a,k);
				return {c,B::merge(d,b)};
			}
			else
			{
				auto[c,d]=vsplit(b,k);
				return {B::merge(a,c),d};
			}
		}
		array<int,3> vaccess(int x,S l,S r)
		{
			int p=predecessor(x,l);
			int a=0,b=0;
			tie(x,b)=vsplit(x,r);
			if(p)tie(a,x)=vsplit(x,B::deref(p));
			return {a,x,b};
		}
#ifndef SPONGE_PTREE_NTAG
		void vupdate(int& x,s_cr l,s_cr r,t_cr t)
		{
			auto[a,y,b]=vaccess(x,l,r);
			if(y)
			{
				B::refresh(y);
				B::apply_tag(y,t);
			}
			x=B::merge(a,B::merge(y,b));
		}
#endif
		S vquery(int& x,s_cr l,s_cr r)
		{
			auto[a,y,b]=vaccess(x,l,r);
			S ans=y?B::nd[y].val:S();
			x=B::merge(a,B::merge(y,b));
			return ans;
		}
		pair<int,int> hsplit(int x,s_cr k)
		{
			if(!x)return {0,0};
			if(cmp(B::deref(x),k))return {x,0};
			if(B::size(x)==1)return {0,x};
			auto[a,b]=B::_split(x);
			if(!cmp(B::deref(a),k))
			{
				auto[c,d]=hsplit(a,k);
				return {c,B::merge(d,b)};
			}
			else
			{
				auto[c,d]=hsplit(b,k);
				return {B::merge(a,c),d};
			}
		}
		int hmerge(int x,int y)
		{
			if(!x||!y)return x|y;
			auto[a,b]=hsplit(x,B::deref(y));
			return B::merge(hmerge(y,a),b);
		}
	};
}
#endif