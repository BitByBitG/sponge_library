#ifndef SPONGE_WBLT_HPP
#define SPONGE_WBLT_HPP
#include<sponge/core.hpp>
namespace sponge
{
	template<typename Key,typename Compare=less<Key>,typename Alloc=allocator<Key>>
	class wblt
	{
	public:
		using key_type=Key;
		using key_compare=Compare;
		using value_type=Key;
		using value_compare=Compare;
		using allocator_type=Alloc;
		inline static constexpr Compare cmp{};
		using alloc_traits=allocator_traits<Alloc>;
	private:
		struct node_t;
	public:
		using node_alloc=typename alloc_traits::template rebind_alloc<node_t>;
		using traits=allocator_traits<node_alloc>;
		using ptr_t=typename traits::pointer;
		inline static node_alloc alloc;
	private:
		template<typename... Args>
		static ptr_t alloc_node(Args&&... args)
		{
			ptr_t p=traits::allocate(alloc,1);
			auto raw=to_address(p);
			::new((void*)raw) node_t(forward<Args>(args)...);
			return p;
		}
		static void dealloc_node(ptr_t p)
		{
			auto raw=to_address(p);
			raw->~node_t();
			traits::deallocate(alloc,p,1);
		}
		struct node_t
		{
			ptr_t fa;
			array<ptr_t,2> son;
			int size;
			Key key;
			constexpr node_t():fa(),son{},size(),key(){}
			constexpr node_t(const Key& v):fa(),son{},size(1),key(v){}
		};
		static bool cmpeq(const Key& x,const Key& y)
		{
			return !cmp(x,y)&&!cmp(y,x);
		}
		static bool is_guard(ptr_t x)
		{
			return x->size==-1;
		}
		static bool is_root(ptr_t x)
		{
			return is_guard(x->fa);
		}
		static bool dir(ptr_t x)
		{
			return x->fa->son[1]==x;
		}
		static void set(ptr_t f,bool r,ptr_t s)
		{
			if(f)f->son[r]=s;
			if(s)s->fa=f;
		}
		static int rank(ptr_t x)
		{
			if(is_guard(x))return x->fa?x->fa->size+1:1;
			int ans=0;
			while(!is_root(x))
			{
				if(x==x->fa->son[1])ans+=x->fa->son[0]->size;
				x=x->fa;
			}
			return ++ans;
		}
		static ptr_t nth(ptr_t x,int n)
		{
			while(x->size>1)
				if(x->son[0]->size>=n)x=x->son[0];
				else n-=x->son[0]->size,x=x->son[1];
			return x;
		}
		static ptr_t find_root(ptr_t x)
		{
			while(!is_root(x))x=x->fa;
			return x;
		}
		static ptr_t leftest_leaf(ptr_t x)
		{
			while(x->son[0])x=x->son[0];
			return x;
		}
		static ptr_t rightest_leaf(ptr_t x)
		{
			while(x->son[1])x=x->son[1];
			return x;
		}
		static ptr_t prev_leaf(ptr_t x)
		{
			if(is_guard(x))return rightest_leaf(x->fa);
			while(!is_root(x))
			{
				if(x==x->fa->son[1])return rightest_leaf(x->fa->son[0]);
				x=x->fa;
			}
			return x->fa;
		}
		static ptr_t next_leaf(ptr_t x)
		{
			if(is_guard(x))return leftest_leaf(x->fa);
			while(!is_root(x))
			{
				if(x==x->fa->son[0])return leftest_leaf(x->fa->son[1]);
				x=x->fa;
			}
			return x->fa;
		}
		static ptr_t access_leaf(ptr_t x,int d)
		{
			return d?nth(find_root(x),rank(x)+d):x;
		}
		static void delete_tree(ptr_t& x)
		{
			if(x->son[0])delete_tree(x->son[0]);
			if(x->son[1])delete_tree(x->son[1]);
			dealloc_node(x);
			x=nullptr;
		}
		static ptr_t copy_tree(ptr_t x)
		{
			if(!x)return nullptr;
			ptr_t y=alloc_node(*x);
			(y->son[0]=copy_tree(x->son[0]))->fa=y;
			(y->son[1]=copy_tree(x->son[1]))->fa=y;
		}
		static void push_up(ptr_t x)
		{
			x->size=x->son[0]->size+x->son[1]->size;
			x->key=x->son[1]->key;
		}
		static ptr_t _merge(ptr_t x,ptr_t y)
		{
			ptr_t z=alloc_node();
			set(z,0,x);
			set(z,1,y);
			push_up(z);
			return z;
		}
		static pair<ptr_t,ptr_t> _split(ptr_t& x)
		{
			ptr_t y=x->son[0],z=x->son[1];
			y->fa=z->fa=nullptr;
			dealloc_node(x);
			x=nullptr;
			return {y,z};
		}
#ifndef SPONGE_WBLT_UNBALANCED
		static constexpr double alpha=0.292;
		static void rotate(ptr_t& x,bool r)
		{
			ptr_t f=x->fa;
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
			x->fa=f;
		}
		static bool unbalanced(ptr_t x,ptr_t y)
		{
			return y->size<alpha*(x->size+y->size);
		}
		static bool need_double_rotation(ptr_t x,bool r)
		{
			return x->son[!r]->size>x->size/(2-alpha);
		}
		static void balance(ptr_t& x)
		{
			if(x->size==1)return;
			bool r=x->son[1]->size>x->son[0]->size;
			if(!unbalanced(x->son[r],x->son[!r]))return;
			if(x->son[r]->size>1&&need_double_rotation(x->son[r],r))rotate(x->son[r],!r);
			rotate(x,r);
		}
#endif
		template<typename Fn>
		static ptr_t _build(Fn&& fn,int l,int r)
		{
			if(l==r)return alloc_node(node_t(fn(l)));
			int mid=(l+r)>>1;
			return _merge(_build(fn,l,mid),_build(fn,mid+1,r));
		}
		template<typename Iter>
		static ptr_t _build(Iter l,Iter r)
		{
			if(next(l)==r)return alloc_node(node_t(*l));
			Iter mid=l+((r-l)>>1);
			return _merge(_build(l,mid),_build(mid,r));
		}
		static ptr_t merge(ptr_t x,ptr_t y)
		{
#ifndef SPONGE_WBLT_UNBALANCED
			if(x==nullptr)return y;
			if(y==nullptr)return x;
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
		static pair<ptr_t,ptr_t> split(ptr_t x,int k)
		{
			if(x==nullptr)return {nullptr,nullptr};
			if(!k)return {nullptr,x};
			if(k==x->size)return {x,nullptr};
			auto[a,b]=_split(x);
			if(k<=a->size)
			{
				auto[c,d]=split(a,k);
				return {c,merge(d,b)};
			}
			else
			{
				auto[c,d]=split(b,k-a->size);
				return {merge(a,c),d};
			}
		}
		static ptr_t insert(ptr_t& x,const Key& v)
		{
			if(x==nullptr)return x=alloc_node(v);
			if(x->size==1)
			{
				bool r=!cmp(v,x->key);
				ptr_t tmp=x;
				x=alloc_node();
				x->fa=tmp->fa;
				set(x,r,alloc_node(v));
				set(x,!r,tmp);
				push_up(x);
				return x->son[r];
			}
			bool r=cmp(x->son[0]->key,v);
			ptr_t ans=insert(x->son[r],v);
			push_up(x);
			balance(x);
			return ans;
		}
		static bool erase(ptr_t& x,const Key& v)
		{
			if(x==nullptr)return 0;
			if(x->size==1)
			{
				if(cmpeq(x->key,v))
				{
					dealloc_node(x);
					x=nullptr;
					return 1;
				}
				return 0;
			}
			bool r=cmp(x->son[0]->key,v);
			bool ans=erase(x->son[r],v);
			if(x->son[r]==nullptr)
			{
				ptr_t tmp=x->son[!r];
				tmp->fa=x->fa;
				dealloc_node(x);
				x=tmp;
			}
			else
			{
				push_up(x);
				balance(x);
			}
			return ans;
		}
		static ptr_t erase(ptr_t& root,ptr_t x)
		{
			ptr_t y=next_leaf(x);
			if(x==root)
			{
				dealloc_node(x);
				root=nullptr;
				return y;
			}
			if(x->fa==root)
			{
				bool r=dir(x);
				ptr_t tmp=x->fa->son[!r];
				tmp->fa=root->fa;
				dealloc_node(root);
				dealloc_node(x);
				root=tmp;
				return y;
			}
			ptr_t f=x->fa,g=x->fa->fa;
			bool r=dir(x),rf=dir(f);
			ptr_t tmp=f->son[!r];
			dealloc_node(f);
			dealloc_node(x);
			set(g,rf,tmp);
			for(ptr_t z=g;!is_root(z);z=z->fa)
			{
				push_up(z);
				balance(z);
			}
			push_up(root);
			balance(root);
			return y;
		}
		static ptr_t erase(ptr_t& root,ptr_t l,ptr_t r)
		{
			ptr_t pos=next_leaf(r);
			int rk_l=rank(l),rk_r=rank(r);
			ptr_t ptr_l,ptr_r;
			tie(root,ptr_r)=split(root,rk_r);
			tie(ptr_l,root)=split(root,rk_l-1);
			delete_tree(root);
			root=merge(ptr_l,ptr_r);
			return pos;
		}
	public:
		template<bool is_reverse=0>
		class ite_t
		{
		public:
			ptr_t p;
			using iterator_category=bidirectional_iterator_tag;
			using value_type=Key;
			using difference_type=ptrdiff_t;
			using pointer=Key*;
			using reference=Key&;
			constexpr ite_t():p(nullptr){}
			constexpr ite_t(ptr_t _p):p(_p){}
			ptr_t& raw()
			{
				return p;
			}
			ite_t& operator++()
			{
				if constexpr(is_reverse)return p=prev_leaf(p),*this;
				else return p=next_leaf(p),*this;
			}
			ite_t operator++(int)
			{
				iterator it=*this;
				++*this;
				return it;
			}
			ite_t& operator--()
			{
				if constexpr(is_reverse)return p=next_leaf(p),*this;
				else return p=prev_leaf(p),*this;
			}
			ite_t operator--(int)
			{
				iterator it=*this;
				--*this;
				return it;
			}
			bool operator==(const ite_t& x)const
			{
				return p==x.p;
			}
			bool operator!=(const ite_t& x)const
			{
				return p!=x.p;
			}
			reference operator*()
			{
				return p->key;
			}
			pointer operator->()
			{
				return &p->key;
			}
		};
		using iterator=ite_t<0>;
		using reverse_iterator=ite_t<1>;
	private:
		ptr_t root,guard;
	public:
		wblt():root(nullptr),guard(alloc_node())
		{
			guard->size=-1;
			maintain_guard();
		}
		template<typename Compare2,typename Alloc2>
		wblt(wblt<Key,Compare2,Alloc2>& t):root(nullptr),guard(alloc_node())
		{
			guard->size==-1;
			maintain_guard();
			insert(t.begin(),t.end());
		}
		template<typename Iter>
		wblt(Iter first,Iter last):root(nullptr),guard(alloc_node())
		{
			guard->size=-1;
			maintain_guard();
			insert(first,last);
		}
		wblt(initializer_list<Key> il):root(nullptr),guard(alloc_node())
		{
			guard->size=-1;
			maintain_guard();
			insert(il);
		}
		wblt(const wblt& t)noexcept:root(copy_tree(t.root)),guard(alloc_node())
		{
			guard->size=-1;
			maintain_guard();
		}
		wblt& operator=(const wblt& t)noexcept
		{
			if(this!=&t)
			{
				wblt tmp(t);
				swap(tmp);
			}
			return *this;
		}
		wblt(wblt&& t)noexcept:root(t.root),guard(t.guard)
		{
			t.root=t.guard=nullptr;
		}
		wblt& operator=(wblt&& t)noexcept
		{
			if(this!=&t)
			{
				wblt tmp(t);
				swap(tmp);
			}
			return *this;
		}
		wblt(ptr_t p):root(p),guard(p->fa){}
		wblt(iterator it):root(it.p),guard(it.p->fa){}
		~wblt()
		{
			clear();
			dealloc_node(guard);
		}
	private:
		void maintain_guard()
		{
			if(root)root->fa=guard;
			guard->fa=root;
		}
	public:
		iterator begin()
		{
			return root?leftest_leaf(root):guard;
		}
		iterator end()
		{
			return guard;
		}
		reverse_iterator rbegin()
		{
			return root?rightest_leaf(root):guard;
		}
		reverse_iterator rend()
		{
			return guard;
		}
		iterator root_iterator()
		{
			return root;
		}
		void clear()
		{
			if(root)delete_tree(root);
			maintain_guard();
		}
		bool empty()
		{
			return !root;
		}
		int size()
		{
			return root?root->size:0;
		}
		template<typename Fn>
		void build(Fn&& fn,int n)
		{
			clear();
			root=_build(fn,1,n);
			maintain_guard();
		}
		template<typename Iter>
		void build(Iter l,Iter r)
		{
			clear();
			root=_build(l,r);
			maintain_guard();
		}
		iterator insert(const Key& v)
		{
			iterator pos=insert(root,v);
			maintain_guard();
			return pos;
		}
		template<typename Iter>
		void insert(Iter first,Iter last)
		{
			for(Iter it=first;it!=last;++it)insert(*it);
		}
		void insert(initializer_list<Key> il)
		{
			insert(il.begin(),il.end());
		}
		bool erase(const Key& v)
		{
			bool ans=erase(root,v);
			maintain_guard();
			return ans;
		}
		iterator erase(iterator it)
		{
			iterator pos=erase(root,it.p);
			maintain_guard();
			return pos;
		}
		void erase(iterator l,iterator r)
		{
			erase(root,l.p,r.p);
			maintain_guard();
		}
		template<typename Iter>
		void erase(Iter first,Iter last)
		{
			for(Iter it=first;it!=last;++it)erase(*it);
		}
		void erase(initializer_list<Key> il)
		{
			erase(il.begin(),il.end());
		}
		void swap(wblt& t)
		{
			std::swap(root,t.root);
			std::swap(guard,t.guard);
		}
		friend void swap(wblt& x,wblt& y)
		{
			swap(x.root,y.root);
			swap(x.guard,y.guard);
		}
		void simple_merge(wblt& t)
		{
			if(t.size()>size())swap(t);
			insert(t.begin(),t.end());
			t.clear();
		}
		void disjoint_merge(wblt& t)
		{
			if(t.empty())return;
			if(empty())return swap(t);
			if(!cmp(leftest_leaf(t.root)->key,root->key))root=merge(root,t.root);
			else root=merge(t.root,root);
			t.root=nullptr;
			maintain_guard();
			t.maintain_guard();
		}
	private:
		static pair<ptr_t,ptr_t> split_v(ptr_t x,const Key& k)
		{
			if(!x)return {nullptr,nullptr};
			if(!cmp(k,x->key))return {x,nullptr};
			if(x->size==1)return {nullptr,x};
			auto[a,b]=_split(x);
			if(cmp(k,a->key))
			{
				auto[c,d]=split_v(a,k);
				return {c,merge(d,b)};
			}
			else
			{
				auto[c,d]=split_v(b,k);
				return {merge(a,c),d};
			}
		}
		static pair<ptr_t,ptr_t> hsplit(ptr_t x,const Key& k)
		{
			if(!x)return {nullptr,nullptr};
			if(cmp(x->key,k))return {x,nullptr};
			if(x->size==1)return {nullptr,x};
			auto[a,b]=_split(x);
			if(!cmp(a->key,k))
			{
				auto[c,d]=hsplit(a,k);
				return {c,merge(d,b)};
			}
			else
			{
				auto[c,d]=hsplit(b,k);
				return {merge(a,c),d};
			}
		}
		static ptr_t hmerge(ptr_t x,ptr_t y)
		{
			if(!x)return y;
			if(!y)return x;
			auto[a,b]=hsplit(x,y->key);
			return merge(hmerge(y,a),b);
		}
	public:
		void heuristic_merge(wblt& t)
		{
			ptr_t x=root,y=t.root;
			root=hmerge(x,y);
			maintain_guard();
			t.root=nullptr;
			t.maintain_guard();
		}
		iterator find(const Key& v)
		{
			ptr_t x=root;
			if(x==nullptr)return end();
			while(x->size>1)
				if(cmp(x->son[0]->key,v))x=x->son[1];
				else x=x->son[0];
			if(cmpeq(x->key,v))return x;
			else return end();
		}
		int order_of_key(const Key& v)
		{
			ptr_t x=root;
			if(x==nullptr)return 1;
			int ans=0;
			while(x->size>1)
				if(cmp(x->son[0]->key,v))ans+=x->son[0]->size,x=x->son[1];
				else x=x->son[0];
			if(cmp(x->key,v))ans++;
			return ++ans;
		}
		iterator find_by_order(int n)
		{
			if(!n||n>size())return end();
			return nth(root,n);
		}
		iterator predecessor(const Key& v)
		{
			return find_by_order(order_of_key(v)-1);
		}
		iterator successor(const Key& v)
		{
			ptr_t x=root;
			if(x==nullptr)return end();
			while(x->size>1)
				if(cmp(v,x->son[0]->key))x=x->son[0];
				else x=x->son[1];
			if(!cmp(v,x->key))return end();
			return x;
		}
		iterator lower_bound(const Key& v)
		{
			ptr_t x=root;
			if(x==nullptr)return end();
			while(x->size>1)
				if(cmp(x->son[0]->key,v))x=x->son[1];
				else x=x->son[0];
			if(cmp(x->key,v))return end();
			return x;
		}
		iterator upper_bound(const Key& v)
		{
			return successor(v);
		}
		int count(const Key& v)
		{
			return rank(upper_bound(v).p)-order_of_key(v);
		}
		int count(const Key& l,const Key& r)
		{
			return rank(upper_bound(r).p)-order_of_key(l);
		}
		wblt split(int k)
		{
			auto[a,b]=split(root,k);
			root=a;
			wblt<int>t;
			t.root=b;
			maintain_guard();
			t.maintain_guard();
			return t;
		}
		wblt split_v(const Key& k)
		{
			auto[a,b]=split_v(root,k);
			root=a;
			wblt<int>t;
			t.root=b;
			maintain_guard();
			t.maintain_guard();
			return t;
		}
	};
}
#endif