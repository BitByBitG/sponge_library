#ifndef SPONGE_PTREE_HPP
#define SPONGE_PTREE_HPP
#ifndef SPONGE_CORE_HPP
#define SPONGE_CORE_HPP
#define SPONGELIB_VERSION "1.0.0"
#include<bits/stdc++.h>
#include<cassert>
#ifdef __GNUC__
#define INLINE __attribute__((always_inline)) inline
#else
#define INLINE inline
#endif
namespace sponge
{
	using namespace std;
	using uint=unsigned int;
	using ll=long long;
	using ull=unsigned long long;
	using ld=long double;
	class null_t{};
	template<typename T>
	constexpr T inf=[]()->T
	{
		if constexpr(is_floating_point_v<T>)return numeric_limits<T>::infinity();
		else return numeric_limits<T>::max()/2;
	}();
	template<typename T>
	constexpr T eps=[]()->T
	{
		if constexpr(is_same_v<T,ld>)return 1e-15;
		else if constexpr(is_floating_point_v<T>)return 1e-8;
		else return 0;
	}();
	template<typename F>
	void multitest_n(int _n,F&& _f)
	{
		for(int _i=1;_i<=_n;_i++)_f(_i);
	}
	template<typename F,typename Stream=istream>
	void multitest(F&& _f,Stream& _s=cin)
	{
		int _n;
		_s>>_n;
		multitest_n(_n,_f);
	}
}
#endif
#ifndef SPONGE_MONOID_HPP
#define SPONGE_MONOID_HPP
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
		using s_cr=const S&;
		using t_cr=const T&;
		inline static constexpr SId s_id{};
		inline static constexpr TId t_id{};
		inline static constexpr IsTId is_t_id{};
		inline static constexpr SOpSS s_op_s_s{};
		inline static constexpr SOpST s_op_s_t{};
		inline static constexpr TOpTT t_op_t_t{};
	};
}
#endif
#ifndef SPONGE_FUNCTORS_HPP
#define SPONGE_FUNCTORS_HPP
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
#ifndef SPONGE_PFOREST_HPP
#define SPONGE_PFOREST_HPP
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
		using M=double_monoid<S,T,SId,TId,IsTId,SOpSS,SOpST,TOpTT>;
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
			node_t():
#ifdef SPONGE_PTREE_FA
				fa(),
#endif
				son{},size(),
#ifndef SPONGE_PTREE_NREV
				rev(),
#endif
				val(s_id()),
#ifndef SPONGE_PTREE_NTAG
				tag(t_id()),
#endif
				ref()
			{}
		};
		int n,cap;
		node_t* tr;
		vector<int> stk;
		pforest():n(0),cap(0),tr(nullptr){}
		pforest(int _cap):tr(nullptr)
		{
			reserve(_cap);
		}
		~pforest()
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
		int leaf(s_cr v)
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
#ifndef SPONGE_PTREE_NTAG
		void apply_tag(int& x,t_cr v)
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
#ifndef SPONGE_PTREE_NTAG
			if(!x||tr[x].size==1)return;
			if(!is_t_id(tr[x].tag)
#ifndef SPONGE_PTREE_NREV
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
#ifndef SPONGE_PTREE_NREV
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
#ifdef SPONGE_PTREE_FA
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
#ifdef SPONGE_PTREE_FA
			tr[y].fa=tr[z].fa=0;
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
	class ordered_pforest:
		public pforest<S,T,SId,TId,IsTId,SOpSS,SOpST,TOpTT,SizeType>
	{
	public:
		using Sz=SizeType;
		using size_type=Sz;
		using B=pforest<S,T,SId,TId,IsTId,SOpSS,SOpST,TOpTT,SizeType>;
		using s_cr=typename B::s_cr;
		using t_cr=typename B::t_cr;
		inline static constexpr Compare cmp{};
		bool cmpeq(s_cr x,s_cr y)
		{
			return !cmp(x,y)&&!cmp(y,x);
		}
		int find(int t,s_cr v)
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
		Sz order_of_key(int t,s_cr v)
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
			while(B::tr[x].size>1)
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
			S ans=y?B::tr[y].val:S();
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
namespace sponge
{
	template<
		typename S,typename T,typename SId,typename TId,typename IsTId,
		typename SOpSS,typename SOpST,typename TOpTT,typename SizeType=int
	>
	class ptree
	{
	public:
		using Fr=pforest<S,T,SId,TId,IsTId,SOpSS,SOpST,TOpTT,SizeType>;
		using pforest_type=Fr;
		inline static Fr fr;
		using Sz=typename Fr::Sz;
		using size_type=Sz;
		using s_cr=typename Fr::s_cr;
		using t_cr=typename Fr::t_cr;
		int root;
		ptree():root(){}
		ptree(int _root):root(_root){}
		template<typename F>
		void build(int n,F&& f){root=fr.build(n,f);}
		template<typename RAIter>
		void build(RAIter first,RAIter last){root=fr.build(first,last);}
		pair<ptree,ptree> split(int k)
		{
			auto[x,y]=fr.split(root,k);
			return {ptree(x),ptree(y)};
		}
		array<ptree,3> access(Sz l,Sz r)
		{
			auto[x,y,z]=fr.access(root,l,r);
			return {ptree(x),ptree(y),ptree(z)};
		}
#ifndef SPONGE_PTREE_NTAG
		void update(Sz l,Sz r,t_cr t){fr.update(root,l,r,t);}
#endif
		void set_pos(Sz p,s_cr v){fr.set_pos(root,p,v);}
#ifndef SPONGE_PTREE_NREV
		void reverse(Sz l,Sz r){fr.reverse(root,l,r);}
#endif
		S query(Sz p){return pos(p);}
		S query(Sz l,Sz r){return fr.query(root,l,r);}
		S pos(Sz p){return fr.pos(root,p);}
		vector<S> range(Sz l,Sz r){return fr.range(root,l,r);}
		vector<S> seq(){return fr.seq(root);}
		int insert_after(Sz p,s_cr v){return fr.insert_after(root,p,v);}
		void erase_pos(Sz p){fr.erase_pos(root,p);}
		void derive_from(ptree x){root=fr.derive(x.root);}
		static ptree merge(ptree x,ptree y){return ptree(fr.merge(x.root,y.root));}
		static S deref(int x){return fr.deref(x);}
	};
	template<
		typename Compare,
		typename S,typename T,typename SId,typename TId,typename IsTId,
		typename SOpSS,typename SOpST,typename TOpTT,typename SizeType=int
	>
	class ordered_ptree
	{
	public:
		using Fr=ordered_pforest<Compare,S,T,SId,TId,IsTId,SOpSS,SOpST,TOpTT,SizeType>;
		using pforest_type=Fr;
		inline static Fr fr;
		using Sz=typename Fr::Sz;
		using size_type=Sz;
		using s_cr=typename Fr::s_cr;
		using t_cr=typename Fr::t_cr;
		int root;
		ordered_ptree():root(){}
		ordered_ptree(int _root):root(_root){}
		template<typename F>
		void build(int n,F&& f){root=fr.build(n,f);}
		template<typename RAIter>
		void build(RAIter first,RAIter last){root=fr.build(first,last);}
		pair<ordered_ptree,ordered_ptree> split(int k)
		{
			auto[x,y]=fr.split(root,k);
			return {ordered_ptree(x),ordered_ptree(y)};
		}
		array<ordered_ptree,3> access(Sz l,Sz r)
		{
			auto[x,y,z]=fr.access(root,l,r);
			return {ordered_ptree(x),ordered_ptree(y),ordered_ptree(z)};
		}
#ifndef SPONGE_PTREE_NTAG
		void update(Sz l,Sz r,t_cr t){fr.update(root,l,r,t);}
#endif
		void set_pos(Sz p,s_cr v){fr.set_pos(root,p,v);}
#ifndef SPONGE_PTREE_NREV
		void reverse(Sz l,Sz r){fr.reverse(root,l,r);}
#endif
		S query(Sz p){return pos(p);}
		S query(Sz l,Sz r){return fr.query(root,l,r);}
		S pos(Sz p){return fr.pos(root,p);}
		vector<S> range(Sz l,Sz r){return fr.range(root,l,r);}
		vector<S> seq(){return fr.seq(root);}
		int insert_after(Sz p,s_cr v){return fr.insert_after(root,p,v);}
		void erase_pos(Sz p){fr.erase_pos(root,p);}
		void derive_from(ordered_ptree x){root=fr.derive(x.root);}
		static ordered_ptree merge(ordered_ptree x,ordered_ptree y){return ordered_ptree(fr.merge(x.root,y.root));}
		static S deref(int x){return fr.deref(x);}
		int find(s_cr v){return fr.find(root,v);}
		int find_by_order(Sz k){return fr.find_by_order(root,k);}
		Sz order_of_key(s_cr v){return fr.order_of_key(root,v);}
		int lower_bound(s_cr v){return fr.lower_bound(root,v);}
		int upper_bound(s_cr v){return fr.upper_bound(root,v);}
		int predecessor(s_cr v){return fr.predecessor(root,v);}
		int successor(s_cr v){return fr.successor(root,v);}
		int insert(s_cr v){return fr.insert(root,v);}
		bool erase(s_cr v){return fr.erase(root,v);}
		pair<ordered_ptree,ordered_ptree> vsplit(s_cr k)
		{
			auto[x,y]=fr.vsplit(root,k);
			return {ordered_ptree(x),ordered_ptree(y)};
		}
		array<ordered_ptree,3> vaccess(s_cr l,s_cr r)
		{
			auto[x,y,z]=fr.vaccess(root,l,r);
			return {ordered_ptree(x),ordered_ptree(y),ordered_ptree(z)};
		}
#ifndef SPONGE_PTREE_NTAG
		void vupdate(s_cr l,s_cr r,t_cr t){fr.vupdate(l,r,t);}
#endif
		S vquery(s_cr l,s_cr r){return fr.vquery(l,r);}
		static ordered_ptree hmerge(ordered_ptree x,ordered_ptree y){return ordered_ptree(fr.hmerge(x.root,y.root));}
 	};
	template<typename S,typename SId,typename SOpSS,typename SizeType=int>
	using ptree_n=ptree<
		S,null_t,SId,null_id,is_null_id,
		SOpSS,ignore_tag<S,SizeType>,null_op,SizeType
	>;
	template<typename S,typename SId,typename SizeType=int>
	using ptree_nn=ptree_n<S,SId,ordered_op<S>,SizeType>;
	template<typename Compare,typename S,typename SId,typename SOpSS,typename SizeType=int>
	using ordered_ptree_n=ordered_ptree<
		Compare,
		S,null_t,SId,null_id,is_null_id,
		SOpSS,ignore_tag<S,SizeType>,null_op,SizeType
	>;
	template<typename Compare,typename S,typename SId,typename SizeType=int>
	using ordered_ptree_nn=ordered_ptree_n<Compare,S,SId,ordered_op<S>,SizeType>;
	template<typename S,typename SId,typename SOpSS,typename SizeType=int>
	using pforest_n=typename ptree_n<S,SId,SOpSS,SizeType>::Fr;
	template<typename S,typename SId,typename SizeType=int>
	using pforest_nn=typename ptree_nn<S,SId,SizeType>::Fr;
	template<typename Compare,typename S,typename SId,typename SOpSS,typename SizeType=int>
	using ordered_pforest_n=typename ordered_ptree_n<Compare,S,SId,SOpSS,SizeType>::Fr;
	template<typename Compare,typename S,typename SId,typename SizeType=int>
	using ordered_pforest_nn=typename ordered_ptree_nn<Compare,S,SId,SizeType>::Fr;
}
#endif