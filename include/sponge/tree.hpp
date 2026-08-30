#ifndef SPONGE_TREE_HPP
#define SPONGE_TREE_HPP
#include<sponge/core.hpp>
#include<sponge/monoid.hpp>
#include<sponge/functors.hpp>
namespace sponge
{
	using tree_ptr=int;
	template<
		typename S,typename T,typename SId,typename TId,typename IsTId,
		typename SOpSS,typename SOpST,typename TOpTT,typename SizeType=int
	>
	class tree:
		public double_monoid<S,T,SId,TId,IsTId,SOpSS,SOpST,TOpTT,SizeType>
	{
	public:
		using size_type=SizeType;
		using M=double_monoid<S,T,SId,TId,IsTId,SOpSS,SOpST,TOpTT>;
		using M::s_id,M::t_id,M::is_t_id,M::s_op_s_s,M::s_op_s_t,M::t_op_t_t;
		using ptr_t=tree_ptr;
		struct node_t
		{
#ifdef SPONGE_TREE_MAINTAIN_FATHER
			ptr_t fa;
#endif
			array<ptr_t,2> son;
			size_type size;
			bool rev;
			S val;
			T tag;
			int ref;
			node_t():
#ifdef SPONGE_TREE_MAINTAIN_FATHER
				fa(),
#endif
				son{},size(),rev(),val(s_id()),tag(t_id()),ref()
			{}
		};
		int n,cap;
		node_t* tr;
		vector<ptr_t> stk;
		tree():n(0),cap(0),tr(nullptr){}
		tree(int _cap):tr(nullptr)
		{
			reserve(_cap);
		}
		node_t& operator[](tree_ptr x)
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
		inline ptr_t& ls(ptr_t x)
		{
			return tr[x].son[0];
		}
		inline ptr_t& rs(ptr_t x)
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
		ptr_t alloc()
		{
			ptr_t x;
			if(!stk.empty())
			{
				x=stk.back();
				stk.pop_back();
			}
			else
			{
				x=++n;
				if(x>=cap)throw runtime_error("Bad alloc.");
			}
			tr[x]=node_t();
			return x;
		}
		void dealloc(ptr_t x)
		{
			if(!x||--tr[x].ref)return;
			if(ls(x))dealloc(ls(x));
			if(rs(x))dealloc(rs(x));
			stk.push_back(x);
		}
		ptr_t copy(ptr_t x)
		{
			ptr_t y=alloc();
			tr[y]=tr[x];
			return y;
		}
		ptr_t leaf(const S& v)
		{
			ptr_t x=alloc();
			tr[x].size=1;
			tr[x].val=v;
			tr[x].ref=1;
			return x;
		}
		void push_up(ptr_t x)
		{
			tr[x].size=tr[ls(x)].size+tr[rs(x)].size;
			tr[x].val=s_op_s_s(tr[ls(x)].val,tr[rs(x)].val);
		}
		void apply_tag(ptr_t& x,const T& v)
		{
			if(!x)return;
			tr[x].val=s_op_s_t(tr[x].val,v,tr[x].size);
			tr[x].tag=t_op_t_t(tr[x].tag,v);
		}
		void refresh(ptr_t& x)
		{
			if(!x)return;
			if(tr[x].ref==1)return;
			--tr[x].ref;
			if(ls(x))tr[ls(x)].ref++;
			if(rs(x))tr[rs(x)].ref++;
			tr[x=copy(x)].ref=1;
		}
		void push_down(ptr_t x)
		{
			if(!x||tr[x].size==1)return;
			if(!untagged(tr[x].tag)||tr[x].rev)
			{
				refresh(ls(x)),refresh(rs(x));
				if(!untagged(tr[x].tag))
				{
					apply_tag(ls(x),tr[x].tag);
					apply_tag(rs(x),tr[x].tag);
					tr[x].tag=t_id();
				}
				if(tr[x].rev)
				{
					swap(ls(x),rs(x));
					tr[ls(x)].rev^=1;
					tr[rs(x)].rev^=1;
					tr[x].rev=0;
				}
			}
		}
		ptr_t _merge(ptr_t x,ptr_t y)
		{
			if(!x||!y)return x|y;
			ptr_t z=alloc();
			tr[z].ref=1;
#ifdef SPONGE_TREE_MAINTAIN_FATHER
			tr[x].fa=tr[y].fa=z;
#endif
			ls(z)=x,rs(z)=y;
			push_up(z);
			return z;
		}
		pair<ptr_t,ptr_t>_split(ptr_t& x)
		{
			if(!x)return {0,0};
			push_down(x);
			ptr_t y=ls(x),z=rs(x);
			++tr[y].ref,++tr[z].ref;
#ifdef SPONGE_TREE_MAINTAIN_FATHER
			tr[y].fa=tr[z].fa=0;
#endif
			dealloc(x);
			x=0;
			return {y,z};
		}
#ifndef SPONGE_TREE_UNBALANCED
		void rotate(ptr_t& x,bool r)
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
		bool unbalanced(ptr_t x,ptr_t y)
		{
			return tr[y].size<alpha*(tr[x].size+tr[y].size);
		}
		bool need_double_rotation(ptr_t x,bool r)
		{
			return tr[tr[x].son[!r]].size>tr[x].size/(2-alpha);
		}
		void balance(ptr_t& x)
		{
			if(tr[x].size==1)return;
			bool r=tr[rs(x)].size>tr[ls(x)].size;
			if(!unbalanced(tr[x].son[r],tr[x].son[!r]))return;
			if(tr[tr[x].son[r]].size>1&&need_double_rotation(tr[x].son[r],r))
				rotate(tr[x].son[r],!r);
			rotate(x,r);
		}
#endif
		ptr_t merge(ptr_t x,ptr_t y)
		{
#ifndef SPONGE_TREE_UNBALANCED
			if(!x||!y)return x|y;
			if(unbalanced(x,y))
			{
				auto[a,b]=_split(x);
				ptr_t z=_merge(a,merge(b,y));
				balance(z);
				return z;
			}
			else if(unbalanced(y,x))
			{
				auto[a,b]=_split(y);
				ptr_t z=_merge(merge(x,a),b);
				balance(z);
				return z;
			}
			else return _merge(x,y);
#else
			return _merge(x,y);
#endif
		}
		pair<ptr_t,ptr_t>split(ptr_t x,size_type k)
		{
			if(!k)return {0,x};
			else if(k==tr[x].size)return {x,0};
			else
			{
				ptr_t a,b,c,d;
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
		ptr_t build(int n,F&& f)
		{
			if(n==0)return 0;
			auto dfs=[&](auto&& dfs,int l,int r)->ptr_t
			{
				if(l==r)return leaf(f(l));
				int mid=(l+r)>>1;
				return _merge(dfs(dfs,l,mid),dfs(dfs,mid+1,r));
			};
			return dfs(dfs,1,n);
		}
		template<typename RAIter>
		ptr_t build(RAIter first,RAIter last)
		{
			return build(last-first,[&](int x){return *(first+x-1);});
		}
		array<ptr_t,3>access(ptr_t x,size_type l,size_type r)
		{
			ptr_t a,b;
			tie(x,b)=split(x,r);
			tie(a,x)=split(x,l-1);
			return {a,x,b};
		}
		void update(ptr_t& x,size_type l,size_type r,const T& t)
		{
			auto[a,y,b]=access(x,l,r);
			refresh(y);
			apply_tag(y,t);
			x=merge(a,merge(y,b));
		}
		void reverse(ptr_t& x,size_type l,size_type r)
		{
			auto[a,y,b]=access(x,l,r);
			refresh(y);
			tr[y].rev^=1;
			x=merge(a,merge(y,b));
		}
        S query(ptr_t x,size_type p)
        {
            return get_position(x,p);
        }
		S query(ptr_t& x,size_type l,size_type r)
		{
			auto[a,y,b]=access(x,l,r);
			S ans=tr[y].val;
			x=merge(a,merge(y,b));
			return ans;
		}
		S get_position(ptr_t x,size_type p)
		{
			while(tr[x].size>1)
			{
				push_down(x);
				if(tr[ls(x)].size>=p)x=ls(x);
				else p-=tr[ls(x)].size,x=rs(x);
			}
			return tr[x].val;
		}
		vector<S> get_range(ptr_t& x,size_type l,size_type r)
		{
			auto[a,b,c]=access(x,l,r);
			vector<S>ans=get_sequence(b);
			x=merge(a,merge(b,c));
			return ans;
		}
		vector<S> get_sequence(ptr_t x)
		{
			vector<S>ans;
			auto dfs=[&](auto&& dfs,ptr_t x)->void
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
		ptr_t insert_after(ptr_t& x,size_type p,const S& v)
		{
			auto[a,b]=split(x,p);
			ptr_t ans=leaf(v);
			x=merge(a,merge(ans,b));
			return ans;
		}
		void erase_position(ptr_t& x,size_type p)
		{
			auto[a,y,b]=access(x,p,p);
			x=merge(a,b);
			dealloc(y);
		}
		ptr_t derive(ptr_t x)
		{
			if(!x)return 0;
			tr[x].ref++;
			ptr_t y=x;
			refresh(y);
			return y;
		}
		int size(ptr_t x)
		{
			return tr[x].size;
		}
		S& deref(ptr_t x)
		{
			return tr[x].val;
		}
	};
	template<
		typename Compare,
		typename S,typename T,typename SId,typename TId,typename IsTId,
		typename SOpSS,typename SOpST,typename TOpTT,typename SizeType=int
	>
	class ordered_tree:
		public tree<S,T,SId,TId,IsTId,SOpSS,SOpST,TOpTT,SizeType>
	{
	public:
		using size_type=SizeType;
		using ptr_t=tree_ptr;
		inline static constexpr Compare cmp{};
		bool cmpeq(const S& x,const S& y)
		{
			return !cmp(x,y)&&!cmp(y,x);
		}
		ptr_t find(ptr_t t,const S& v)
		{
			if(!t)return 0;
			ptr_t x=t;
			while(this->tr[x].size>1)
			{
				this->push_down(x);
				if(!cmp(this->deref(this->ls(x)),v))x=this->ls(x);
				else x=this->rs(x);
			}
			if(cmpeq(this->deref(x),v))return x;
			else return 0;
		}
		ptr_t find_by_order(ptr_t t,size_type k)
		{
			if(!t||k<=0||k>this->tr[t].size)return 0;
			ptr_t x=t;
			while(this->tr[x].size>1)
			{
				this->push_down(x);
				if(this->tr[this->ls(x)].size>=k)x=this->ls(x);
				else k-=this->tr[this->ls(x)].size,x=this->rs(x);
			}
			return x;
		}
		size_type order_of_key(ptr_t t,const S& v)
		{
			if(!t)return 1;
			ptr_t x=t;
			size_type ans=0;
			while(this->tr[x].size>1)
			{
				this->push_down(x);
				if(!cmp(this->deref(this->ls(x)),v))x=this->ls(x);
				else ans+=this->tr[this->ls(x)].size,x=this->rs(x);
			}
			if(cmp(this->deref(x),v))ans++;
			return ++ans;
		}
		ptr_t lower_bound(ptr_t t,const S &v)
		{
			ptr_t x=t;
			if(!x)return 0;
			while(this->tr[x].size>1)
			{
				this->push_down(x);
				if(cmp(this->deref(this->tr[this->ls(x)]),v))x=this->rs(x);
				else x=this->ls(x);
			}
			if(cmp(this->deref(x),v))return 0;
			return x;
		}
		ptr_t upper_bound(ptr_t t,const S& v)
		{
			return successor(t,v);
		}
		ptr_t predecessor(ptr_t t,const S& v)
		{
			return find_by_order(t,order_of_key(t,v)-1);
		}
		ptr_t successor(ptr_t t,const S& v)
		{
			if(!t)return 0;
			ptr_t x=t;
			while(this->tr[x].size>1)
			{
				this->push_down(x);
				if(cmp(v,this->deref(this->ls(x))))x=this->ls(x);
				else x=this->rs(x);
			}
			if(!cmp(v,this->deref(x)))return 0;
			return x;
		}
		ptr_t insert(ptr_t& t,const S& v)
		{
			auto[a,b]=vsplit(t,v);
			ptr_t ans=this->leaf(v);
			t=this->merge(a,this->merge(ans,b));
			return ans;
		}
		bool erase(ptr_t& t,const S& v)
		{
			if(!find(t,v))return 0;
			this->erase_position(t,order_of_key(t,v));
			return 1;
		}
		pair<ptr_t,ptr_t> vsplit(ptr_t x,const S& k)
		{
			if(!x)return {0,0};
			if(!cmp(k,this->deref(x)))return {x,0};
			if(this->tr[x].size==1)return {0,x};
			auto[a,b]=this->_split(x);
			if(cmp(k,this->deref(a)))
			{
				auto[c,d]=vsplit(a,k);
				return {c,this->merge(d,b)};
			}
			else
			{
				auto[c,d]=vsplit(b,k);
				return {this->merge(a,c),d};
			}
		}
		array<ptr_t,3> vaccess(ptr_t x,S l,S r)
		{
			ptr_t p=predecessor(x,l);
			ptr_t a=0,b=0;
			tie(x,b)=vsplit(x,r);
			if(p)tie(a,x)=vsplit(x,this->deref(p));
			return {a,x,b};
		}
		void vupdate(ptr_t& x,const S& l,const S& r,const T& t)
		{
			auto[a,y,b]=vaccess(x,l,r);
			if(y)
			{
				this->refresh(y);
				this->apply_tag(y,t);
			}
			x=this->merge(a,this->merge(y,b));
		}
		S vquery(ptr_t& x,const S& l,const S& r)
		{
			auto[a,y,b]=vaccess(x,l,r);
			S ans=y?this->tr[y].val:S();
			x=this->merge(a,this->merge(y,b));
			return ans;
		}
		pair<ptr_t,ptr_t> hsplit(ptr_t x,const S& k)
		{
			if(!x)return {0,0};
			if(cmp(this->deref(x),k))return {x,0};
			if(this->size(x)==1)return {0,x};
			auto[a,b]=this->_split(x);
			if(!cmp(this->deref(a),k))
			{
				auto[c,d]=hsplit(a,k);
				return {c,this->merge(d,b)};
			}
			else
			{
				auto[c,d]=hsplit(b,k);
				return {this->merge(a,c),d};
			}
		}
		ptr_t hmerge(ptr_t x,ptr_t y)
		{
			if(!x||!y)return x|y;
			auto[a,b]=hsplit(x,this->deref(y));
			return this->merge(hmerge(y,a),b);
		}
	};
	template<typename S,typename SId,typename SOpSS,typename SizeType=int>
	using tree_n=tree<
		S,
		null_t,
		SId,
		null_id,
		is_null_id,
		SOpSS,
		ignore_tag<S,SizeType>,
		null_op,
		SizeType
	>;
	template<typename S,typename SId,typename SizeType=int>
	using tree_nn=tree_n<
		S,
		SId,
		ordered_op<S>,
		SizeType
	>;
	template<typename Compare,typename S,typename SId,typename SOpSS,typename SizeType=int>
	using ordered_tree_n=ordered_tree<
		Compare,
		S,
		null_t,
		SId,
		null_id,
		is_null_id,
		SOpSS,
		ignore_tag<S,SizeType>,
		null_op
	>;
	template<typename Compare,typename S,typename SId,typename SizeType=int>
	using ordered_tree_nn=ordered_tree_n<
		Compare,
		S,
		SId,
		ordered_op<S>,
		SizeType
	>;
}
#endif