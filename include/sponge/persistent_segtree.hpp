#ifndef SPONGE_PERSISTENT_SEGTREE_HPP
#define SPONGE_PERSISTENT_SEGTREE_HPP
#include<sponge/monoid.hpp>
#include<sponge/functor.hpp>
namespace sponge
{
	template<typename S,typename T,typename SId,typename TId,typename IsTId,
	typename SOpSS,typename SOpST,typename TOpTT>
	class psgt_forest:public double_monoid<S,T,SId,TId,IsTId,SOpSS,SOpST,TOpTT>
	{
		public:
		using M=double_monoid<S,T,SId,TId,IsTId,SOpSS,SOpST,TOpTT>;
		using s_cr=typename M::s_cr;
		using t_cr=typename M::t_cr;
		using M::s_id,M::t_id,M::is_t_id,M::s_op_s_s,M::s_op_s_t,M::t_op_t_t;
		struct node_t
		{
			int left=0,right=0,len=0;
			S val;
			T tag;
		};
		vector<node_t> nd;
		psgt_forest():psgt_forest(M{}) {}
		explicit psgt_forest(const M& m):M(m)
		{
			nd.push_back({0,0,0,s_id(),t_id()});
		}
		explicit psgt_forest(int cap):psgt_forest()
		{
			reserve(cap);
		}
		psgt_forest(int cap,const M& m):psgt_forest(m)
		{
			reserve(cap);
		}
		void reserve(int cap)
		{
			nd.reserve(cap);
		}
		int size()const
		{
			return int(nd.size())-1;
		}
		int capacity()const
		{
			return int(nd.capacity());
		}
		int size(int x)const
		{
			return nd[x].len;
		}
		node_t& operator[](int x)
		{
			return nd[x];
		}
		int derive(int x)const
		{
			return x;
		}
		int copy(int x)
		{
			auto v=nd[x];
			nd.push_back(move(v));
			return int(nd.size())-1;
		}
		template<typename F> int build(int n,F&& f)
		{
			auto dfs=[&](auto&& self,int l,int r)->int
			{
				if(l>r)return 0;
				int x=int(nd.size());
				nd.push_back(
				{
					0,0,r-l+1,s_id(),t_id()
				});
				if(l==r)nd[x].val=f(l);
				else
				{
					int mid=l+(r-l)/2;
					int a=self(self,l,mid),b=self(self,mid+1,r);
					nd[x].left=a;
					nd[x].right=b;
					push_up(x);
				}
				return x;
			};
			return dfs(dfs,1,n);
		}
		template<typename Iter> int build(Iter first,Iter last)
		{
			return build(int(last-first),[&](int i)
			{
				return *(first+i-1);
			});
		}
		void push_up(int x)
		{
			nd[x].val=s_op_s_s(nd[nd[x].left].val,nd[nd[x].right].val);
		}
		void apply_tag(int x,t_cr t)
		{
			nd[x].val=s_op_s_t(nd[x].val,t,nd[x].len);
			nd[x].tag=t_op_t_t(nd[x].tag,t);
		}
		void push_down(int x)
		{
			if(nd[x].len<=1||is_t_id(nd[x].tag))return;
			T t=nd[x].tag;
			int a=copy(nd[x].left),b=copy(nd[x].right);
			nd[x].left=a;
			nd[x].right=b;
			apply_tag(a,t);
			apply_tag(b,t);
			nd[x].tag=t_id();
		}
		int modify(int x,int l,int r,int ql,int qr,t_cr t,const S* value)
		{
			if(!x||qr<l||r<ql)return x;
			x=copy(x);
			if(value&&l==r)
			{
				nd[x].val=*value;
				nd[x].tag=t_id();
				return x;
			}
			if(!value&&ql<=l&&r<=qr)
			{
				apply_tag(x,t);
				return x;
			}
			push_down(x);
			int mid=l+(r-l)/2;
			int a=modify(nd[x].left,l,mid,ql,qr,t,value);
			int b=modify(nd[x].right,mid+1,r,ql,qr,t,value);
			nd[x].left=a;
			nd[x].right=b;
			push_up(x);
			return x;
		}
		void set(int& x,int p,s_cr v)
		{
			S value=v;
			x=modify(x,1,size(x),p,p,t_id(),&value);
		}
		void set_pos(int& x,int p,s_cr v)
		{
			set(x,p,v);
		}
		void update(int& x,int l,int r,t_cr t)
		{
			if(l<=r&&!is_t_id(t))
			{
				T tag=t;
				x=modify(x,1,size(x),l,r,tag,nullptr);
			}
		}
		void update(int& x,int p,t_cr t)
		{
			update(x,p,p,t);
		}
		S query_impl(int x,int l,int r,int ql,int qr,t_cr carry)const
		{
			if(!x||qr<l||r<ql)return s_id();
			if(ql<=l&&r<=qr)return s_op_s_t(nd[x].val,carry,nd[x].len);
			T next=t_op_t_t(nd[x].tag,carry);
			int mid=l+(r-l)/2;
			return s_op_s_s(query_impl(nd[x].left,l,mid,ql,qr,next),query_impl(nd[x].right,mid+1,r,ql,qr,next));
		}
		S query(int x,int l,int r)const
		{
			return query_impl(x,1,size(x),l,r,t_id());
		}
		S query(int x,int p)const
		{
			return query(x,p,p);
		}
		S query(int x)const
		{
			return nd[x].val;
		}
		template<typename F> int search(int x,int l,int r,int bound,bool reverse,F& f,S& acc,t_cr carry)const
		{
			if(!x||(reverse?l>bound:r<bound))return -1;
			S v=s_op_s_t(nd[x].val,carry,nd[x].len);
			if(reverse?r<=bound:l>=bound)
			{
				S next=reverse?s_op_s_s(v,acc):s_op_s_s(acc,v);
				if(f(next))
				{
					acc=move(next);
					return -1;
				}if(l==r)return l;
			}
			T next=t_op_t_t(nd[x].tag,carry);
			int mid=l+(r-l)/2;
			int ans=reverse?search(nd[x].right,mid+1,r,bound,reverse,f,acc,next):search(nd[x].left,l,mid,bound,reverse,f,acc,next);
			if(ans!=-1)return ans;
			return reverse?search(nd[x].left,l,mid,bound,reverse,f,acc,next):search(nd[x].right,mid+1,r,bound,reverse,f,acc,next);
		}
		template<typename F> int max_right(int x,int l,F&& f)const
		{
			S acc=s_id();
			int p=search(x,1,size(x),l,false,f,acc,t_id());
			return p<0?size(x):p-1;
		}
		template<typename F> int min_left(int x,int r,F&& f)const
		{
			S acc=s_id();
			int p=search(x,1,size(x),r,true,f,acc,t_id());
			return p<0?1:p+1;
		}
	};
	template<typename S,typename T,typename SId,typename TId,typename IsTId,
	typename SOpSS,typename SOpST,typename TOpTT>
	class persistent_segtree
	{
		public:
		using Fr=psgt_forest<S,T,SId,TId,IsTId,SOpSS,SOpST,TOpTT>;
		using psgt_forest_type=Fr;
		using M=typename Fr::M;
		using s_cr=typename Fr::s_cr;
		using t_cr=typename Fr::t_cr;
		inline static Fr fr;
		int root=0,n=0;
		persistent_segtree()=default;
		explicit persistent_segtree(int n):persistent_segtree(n,M{}) {}
		explicit persistent_segtree(const M& m)
		{
			static_cast<M&>(fr)=m;
		}
		persistent_segtree(int n,const M& m):persistent_segtree(n,[&](int){return m.s_id();},m) {}
		template<typename F,enable_if_t<!is_same_v<decay_t<F>,M>,int> =0>
		persistent_segtree(int n,F&& f)
		{
			build(n,forward<F>(f));
		}
		template<typename F> persistent_segtree(int n,F&& f,const M& m):persistent_segtree(m)
		{
			build(n,forward<F>(f));
		}
		template<typename F> void build(int count,F&& f)
		{
			root=fr.build(count,forward<F>(f));
			n=count;
		}
		template<typename Iter> void build(Iter first,Iter last)
		{
			n=int(last-first);
			root=fr.build(first,last);
		}
		void derive_from(const persistent_segtree& x)
		{
			root=fr.derive(x.root);
			n=x.n;
		}
		void set(int p,s_cr s)
		{
			fr.set(root,p,s);
		}
		void update(int p,t_cr t)
		{
			fr.update(root,p,t);
		}
		void update(int l,int r,t_cr t)
		{
			fr.update(root,l,r,t);
		}
		S query(int p)const
		{
			return fr.query(root,p);
		}
		S query(int l,int r)const
		{
			return fr.query(root,l,r);
		}
		S query()const
		{
			return fr.query(root);
		}
		template<typename F> int max_right(int l,F&& f)const
		{
			return fr.max_right(root,l,forward<F>(f));
		}
		template<bool (*f)(S)> int max_right(int l)const
		{
			return max_right(l,f);
		}
		template<typename F> int min_left(int r,F&& f)const
		{
			return fr.min_left(root,r,forward<F>(f));
		}
		template<bool (*f)(S)> int min_left(int r)const
		{
			return min_left(r,f);
		}
	};
	template<typename S,typename SId,typename SOpSS>
	using persistent_segtree_n=persistent_segtree<S,null_t,SId,null_id,is_null_id,SOpSS,ignore_tag<S>,null_op>;
	template<typename S,typename SId,typename SOpSS>
	using psgt_forest_n=psgt_forest<S,null_t,SId,null_id,is_null_id,SOpSS,ignore_tag<S>,null_op>;
}
#endif
