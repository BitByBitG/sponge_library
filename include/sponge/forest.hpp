#ifndef SPONGE_FOREST_HPP
#define SPONGE_FOREST_HPP
#include<sponge/core.hpp>
#include<sponge/monoid.hpp>
#include<sponge/functors.hpp>
namespace sponge
{
	template<
		typename S,typename T,typename SId,typename TId,typename IsTId,
		typename SOpSS,typename SOpST,typename TOpTT,typename SizeType=int
	>
	class forest:
		public double_monoid<S,T,SId,TId,IsTId,SOpSS,SOpST,TOpTT,SizeType>
	{
	public:
		using Sz=SizeType;
		using size_type=Sz;
		using M=double_monoid<S,T,SId,TId,IsTId,SOpSS,SOpST,TOpTT>;
		using M::s_id,M::t_id,M::is_t_id,M::s_op_s_s,M::s_op_s_t,M::t_op_t_t;
		using Scr=M::Scr;
		using Tcr=M::Tcr;
		struct node_t
		{
#ifdef SPONGE_TREE_FA
			int fa;
#endif
			array<int,2> son;
			Sz size;
#ifndef SPONGE_TREE_NREV
			bool rev;
#endif
			S val;
#ifndef SPONGE_TREE_NTAG
			T tag;
#endif
			int ref;
			node_t():
#ifdef SPONGE_TREE_FA
				fa(),
#endif
				son{},size(),
#ifndef SPONGE_TREE_NREV
				rev(),
#endif
				val(s_id()),
#ifndef SPONGE_TREE_NTAG
				tag(t_id()),
#endif
				ref()
			{}
		};
		int n,cap;
		node_t* tr;
		vector<int> stk;
		forest():n(0),cap(0),tr(nullptr){}
		forest(int _cap):tr(nullptr)
		{
			reserve(_cap);
		}
		~forest()
		{
			delete[] tr;
		}
		node_t& operator[](int x)
		{
			return tr[x];
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
			return tr[x].son[0];
		}
		inline int& rs(int x)
		{
			return tr[x].son[1];
		}
		void reserve(int _cap)
		{
			if(tr!=nullptr)delete[] tr;
			n=0;
			cap=_cap;
			tr=new node_t[cap];
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
				if(x>=cap)throw runtime_error("bad alloc");
			}
			tr[x]=node_t();
			return x;
		}
		void dealloc(int x)
		{
			if(!x||--tr[x].ref)return;
			if(ls(x))dealloc(ls(x));
			if(rs(x))dealloc(rs(x));
			stk.push_back(x);
		}
		int copy(int x)
		{
			int y=alloc();
			tr[y]=tr[x];
			return y;
		}
		int leaf(Scr v)
		{
			int x=alloc();
			tr[x].size=1;
			tr[x].val=v;
			tr[x].ref=1;
			return x;
		}
		void push_up(int x)
		{
			tr[x].size=tr[ls(x)].size+tr[rs(x)].size;
			tr[x].val=s_op_s_s(tr[ls(x)].val,tr[rs(x)].val);
		}
#ifndef SPONGE_TREE_NTAG
		void apply_tag(int& x,Tcr v)
		{
			if(!x)return;
			tr[x].val=s_op_s_t(tr[x].val,v,tr[x].size);
			tr[x].tag=t_op_t_t(tr[x].tag,v);
		}
#endif
		void refresh(int& x)
		{
			if(!x)return;
			if(tr[x].ref==1)return;
			--tr[x].ref;
			if(ls(x))tr[ls(x)].ref++;
			if(rs(x))tr[rs(x)].ref++;
			tr[x=copy(x)].ref=1;
		}
		void push_down(int x[[maybe_unused]])
		{
#ifndef SPONGE_TREE_NTAG
			if(!x||tr[x].size==1)return;
			if(!is_t_id(tr[x].tag)
#ifndef SPONGE_TREE_NREV
				||tr[x].rev
#endif
			)
			{
				refresh(ls(x)),refresh(rs(x));
				if(!is_t_id(tr[x].tag))
				{
					apply_tag(ls(x),tr[x].tag);
					apply_tag(rs(x),tr[x].tag);
					tr[x].tag=t_id();
				}
#ifndef SPONGE_TREE_NREV
				if(tr[x].rev)
				{
					swap(ls(x),rs(x));
					tr[ls(x)].rev^=1;
					tr[rs(x)].rev^=1;
					tr[x].rev=0;
				}
#endif
			}
#endif
		}
		int _merge(int x,int y)
		{
			if(!x||!y)return x|y;
			int z=alloc();
			tr[z].ref=1;
#ifdef SPONGE_TREE_FA
			tr[x].fa=tr[y].fa=z;
#endif
			ls(z)=x,rs(z)=y;
			push_up(z);
			return z;
		}
		pair<int,int>_split(int& x)
		{
			if(!x)return {0,0};
			push_down(x);
			int y=ls(x),z=rs(x);
			++tr[y].ref,++tr[z].ref;
#ifdef SPONGE_TREE_FA
			tr[y].fa=tr[z].fa=0;
#endif
			dealloc(x);
			x=0;
			return {y,z};
		}
#ifndef SPONGE_TREE_UNBALANCED
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
			return tr[y].size<alpha*(tr[x].size+tr[y].size);
		}
		bool need_double_rotation(int x,bool r)
		{
			return tr[tr[x].son[!r]].size>tr[x].size/(2-alpha);
		}
		void balance(int& x)
		{
			if(tr[x].size==1)return;
			bool r=tr[rs(x)].size>tr[ls(x)].size;
			if(!unbalanced(tr[x].son[r],tr[x].son[!r]))return;
			if(tr[tr[x].son[r]].size>1&&need_double_rotation(tr[x].son[r],r))
				rotate(tr[x].son[r],!r);
			rotate(x,r);
		}
#endif
		int merge(int x,int y)
		{
#ifndef SPONGE_TREE_UNBALANCED
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
			else if(k==tr[x].size)return {x,0};
			else
			{
				int a,b,c,d;
				tie(a,b)=_split(x);
				if(k<=tr[a].size)
				{
					tie(c,d)=split(a,k);
					return {c,merge(d,b)};
				}
				else
				{
					tie(c,d)=split(b,k-tr[a].size);
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
#ifndef SPONGE_TREE_NTAG
		void update(int& x,Sz l,Sz r,Tcr t)
		{
			auto[a,y,b]=access(x,l,r);
			refresh(y);
			apply_tag(y,t);
			x=merge(a,merge(y,b));
		}
#endif
		void set_pos(int& x,Sz p,Scr v)
		{
			auto[a,y,b]=access(x,p,p);
			refresh(y);
			deref(y)=v;
			x=merge(a,merge(y,b));
		}
#ifndef SPONGE_TREE_NREV
		void reverse(int& x,Sz l,Sz r)
		{
			auto[a,y,b]=access(x,l,r);
			refresh(y);
			tr[y].rev^=1;
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
			S ans=tr[y].val;
			x=merge(a,merge(y,b));
			return ans;
		}
		S pos(int x,Sz p)
		{
			while(tr[x].size>1)
			{
				push_down(x);
				if(tr[ls(x)].size>=p)x=ls(x);
				else p-=tr[ls(x)].size,x=rs(x);
			}
			return tr[x].val;
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
				if(tr[x].size==1)return ans.push_back(tr[x].val),[]{}();
				push_down(x);
				dfs(dfs,ls(x));
				dfs(dfs,rs(x));
			};
			dfs(dfs,x);
			return ans;
		}
		int insert_after(int& x,Sz p,Scr v)
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
			tr[x].ref++;
			int y=x;
			refresh(y);
			return y;
		}
		int size(int x)
		{
			return tr[x].size;
		}
		S& deref(int x)
		{
			return tr[x].val;
		}
	};
	template<
		typename Compare,
		typename S,typename T,typename SId,typename TId,typename IsTId,
		typename SOpSS,typename SOpST,typename TOpTT,typename SizeType=int
	>
	class ordered_forest:
		public forest<S,T,SId,TId,IsTId,SOpSS,SOpST,TOpTT,SizeType>
	{
	public:
		using Sz=SizeType;
		using size_type=Sz;
		using B=forest<S,T,SId,TId,IsTId,SOpSS,SOpST,TOpTT,SizeType>;
		using Scr=B::Scr;
		using Tcr=B::Tcr;
		inline static constexpr Compare cmp{};
		bool cmpeq(Scr x,Scr y)
		{
			return !cmp(x,y)&&!cmp(y,x);
		}
		int find(int t,Scr v)
		{
			if(!t)return 0;
			int x=t;
			while(B::tr[x].size>1)
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
			if(!t||k<=0||k>B::tr[t].size)return 0;
			int x=t;
			while(B::tr[x].size>1)
			{
				B::push_down(x);
				if(B::tr[B::ls(x)].size>=k)x=B::ls(x);
				else k-=B::tr[B::ls(x)].size,x=B::rs(x);
			}
			return x;
		}
		Sz order_of_key(int t,Scr v)
		{
			if(!t)return 1;
			int x=t;
			Sz ans=0;
			while(B::tr[x].size>1)
			{
				B::push_down(x);
				if(!cmp(B::deref(B::ls(x)),v))x=B::ls(x);
				else ans+=B::tr[B::ls(x)].size,x=B::rs(x);
			}
			if(cmp(B::deref(x),v))ans++;
			return ++ans;
		}
		int lower_bound(int t,const S &v)
		{
			int x=t;
			if(!x)return 0;
			while(B::tr[x].size>1)
			{
				B::push_down(x);
				if(cmp(B::deref(B::tr[B::ls(x)]),v))x=B::rs(x);
				else x=B::ls(x);
			}
			if(cmp(B::deref(x),v))return 0;
			return x;
		}
		int upper_bound(int t,Scr v)
		{
			return successor(t,v);
		}
		int predecessor(int t,Scr v)
		{
			return find_by_order(t,order_of_key(t,v)-1);
		}
		int successor(int t,Scr v)
		{
			if(!t)return 0;
			int x=t;
			while(B::tr[x].size>1)
			{
				B::push_down(x);
				if(cmp(v,B::deref(B::ls(x))))x=B::ls(x);
				else x=B::rs(x);
			}
			if(!cmp(v,B::deref(x)))return 0;
			return x;
		}
		int insert(int& t,Scr v)
		{
			auto[a,b]=vsplit(t,v);
			int ans=B::leaf(v);
			t=B::merge(a,B::merge(ans,b));
			return ans;
		}
		bool erase(int& t,Scr v)
		{
			if(!find(t,v))return 0;
			B::erase_pos(t,order_of_key(t,v));
			return 1;
		}
		pair<int,int> vsplit(int x,Scr k)
		{
			if(!x)return {0,0};
			if(!cmp(k,B::deref(x)))return {x,0};
			if(B::tr[x].size==1)return {0,x};
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
#ifndef SPONGE_TREE_NTAG
		void vupdate(int& x,Scr l,Scr r,Tcr t)
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
		S vquery(int& x,Scr l,Scr r)
		{
			auto[a,y,b]=vaccess(x,l,r);
			S ans=y?B::tr[y].val:S();
			x=B::merge(a,B::merge(y,b));
			return ans;
		}
		pair<int,int> hsplit(int x,Scr k)
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