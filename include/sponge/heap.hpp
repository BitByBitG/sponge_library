#ifndef SPONGE_HEAP_HPP
#define SPONGE_HEAP_HPP
#include<sponge/core.hpp>
#include<sponge/utility.hpp>
namespace sponge
{
	template<typename Key,typename Compare=less<Key>,typename Alloc=allocator<Key>>
	class pairing_heap
	{
	public:
		using key_type=Key;
		using key_compare=Compare;
		using value_type=Key;
		using value_compare=Compare;
		using allocator_type=Alloc;
		Compare cmp{};
		using alloc_traits=allocator_traits<Alloc>;
	private:
		struct node_t;
	public:
		using node_alloc=typename alloc_traits::template rebind_alloc<node_t>;
		using traits=allocator_traits<node_alloc>;
		using ptr_t=typename traits::pointer;
		inline static node_alloc alloc;
	private:
		struct node_t
		{
			ptr_t fa,bro,son;
			Key key;
			constexpr node_t():fa(),bro(),son(),key(){}
			constexpr node_t(const Key& v):fa(),bro(),son(),key(v){}
		};
		template<typename... Args>
		static ptr_t alloc_node(Args&&... args)
		{
			ptr_t p=traits::allocate(alloc,1);
			auto raw=to_address(p);
			try
			{
				::new((void*)raw) node_t(forward<Args>(args)...);
			}
			catch(...)
			{
				traits::deallocate(alloc,p,1);
				throw;
			}
			return p;
		}
		static void dealloc_node(ptr_t p)
		{
			if(!p)return;
			auto raw=to_address(p);
			raw->~node_t();
			traits::deallocate(alloc,p,1);
		}
		static void delete_tree(ptr_t& x)
		{
			while(x)
			{
				ptr_t p=x->bro;
				if(x->son)
				{
					p=x->son;
					ptr_t y=p;
					while(y->bro)y=y->bro;
					y->bro=x->bro;
				}
				dealloc_node(x);
				x=p;
			}
		}
		static ptr_t copy_tree(ptr_t x)
		{
			if(!x)return nullptr;
			ptr_t y=alloc_node(x->key);
			try
			{
				vector<pair<ptr_t,ptr_t>> st{{x,y}};
				while(!st.empty())
				{
					auto[a,b]=st.back();
					st.pop_back();
					if(a->bro)
					{
						b->bro=alloc_node(a->bro->key);
						b->bro->fa=b;
						st.push_back({a->bro,b->bro});
					}
					if(a->son)
					{
						b->son=alloc_node(a->son->key);
						b->son->fa=b;
						st.push_back({a->son,b->son});
					}
				}
			}
			catch(...)
			{
				delete_tree(y);
				throw;
			}
			return y;
		}
		ptr_t merge(ptr_t x,ptr_t y)
		{
			if(!x)return y;
			if(!y)return x;
			if(!cmp(x->key,y->key))std::swap(x,y);
			if(x->son)x->son->fa=y;
			y->bro=x->son;
			x->son=y;
			y->fa=x;
			return x;
		}
		ptr_t pairing_merge(ptr_t x)
		{
			ptr_t st=nullptr,ans=nullptr;
			while(x)
			{
				ptr_t a=x,b=x->bro;
				x=b?b->bro:nullptr;
				a->fa=a->bro=nullptr;
				if(b)b->fa=b->bro=nullptr;
				ptr_t p=merge(a,b);
				p->bro=st;
				st=p;
			}
			while(st)
			{
				ptr_t p=st;
				st=st->bro;
				p->bro=nullptr;
				ans=merge(p,ans);
			}
			return ans;
		}
		ptr_t root;
	public:
		struct point_iterator
		{
			using value_type=Key;
			using pointer=Key*;
			using reference=Key&;
			ptr_t p;
			constexpr point_iterator():p(nullptr){}
			constexpr point_iterator(ptr_t _p):p(_p){}
			bool operator==(const point_iterator x)const
			{
				return p==x.p;
			}
			bool operator!=(const point_iterator x)const
			{
				return p!=x.p;
			}
			ptr_t& raw()
			{
				return p;
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
		pairing_heap():root(){}
		explicit pairing_heap(Compare c):cmp(move(c)),root(){}
		template<typename Iter>
		pairing_heap(Iter first,Iter last):pairing_heap()
		{
			insert(first,last);
		}
		template<typename Iter>
		pairing_heap(Iter first,Iter last,Compare c):pairing_heap(move(c))
		{
			insert(first,last);
		}
		pairing_heap(const initializer_list<Key>& il):pairing_heap()
		{
			insert(il);
		}
		pairing_heap(const initializer_list<Key>& il,Compare c):pairing_heap(move(c))
		{
			insert(il);
		}
		pairing_heap(const pairing_heap& h):cmp(h.cmp),root(copy_tree(h.root)){}
		pairing_heap& operator=(const pairing_heap& h)
		{
			if(this!=&h)
			{
				pairing_heap tmp(h);
				swap(tmp);
			}
			return *this;
		}
		pairing_heap(pairing_heap&& h)noexcept(is_nothrow_move_constructible_v<Compare>):cmp(move(h.cmp)),root(h.root)
		{
			h.root=nullptr;
		}
		pairing_heap& operator=(pairing_heap&& h)
		{
			if(this!=&h)
			{
				pairing_heap tmp(move(h));
				swap(tmp);
			}
			return *this;
		}
		~pairing_heap()
		{
			clear();
		}
		bool empty()
		{
			return !root;
		}
		void clear()
		{
			if(root)delete_tree(root);
		}
		point_iterator push(const Key& v)
		{
			ptr_t x=alloc_node(v);
			try
			{
				root=merge(root,x);
			}
			catch(...)
			{
				dealloc_node(x);
				throw;
			}
			return x;
		}
		point_iterator insert(const Key& v)
		{
			return push(v);
		}
		template<typename Iter>
		void insert(Iter first,Iter last)
		{
			for(;first!=last;++first)insert(*first);
		}
		void insert(const initializer_list<Key>& il)
		{
			insert(il.begin(),il.end());
		}
		point_iterator top_iter()
		{
			return root;
		}
		Key top()
		{
			return root->key;
		}
		void pop()
		{
			if(!root)return;
			ptr_t tmp=pairing_merge(root->son);
			dealloc_node(root);
			root=tmp;
		}
		void join(pairing_heap& h)
		{
			if(this==&h)return;
			root=merge(root,h.root);
			h.root=nullptr;
		}
		void swap(pairing_heap& h)
		{
			std::swap(cmp,h.cmp);
			std::swap(root,h.root);
		}
		void extract(point_iterator it)
		{
			ptr_t x=it.p;
			ptr_t tmp=pairing_merge(x->son);
			if(x==root)root=tmp;
			else
			{

				if(!tmp)
				{
					if(x->fa->son==x)x->fa->son=x->bro;
					else x->fa->bro=x->bro;
					if(x->bro)x->bro->fa=x->fa;
				}
				else
				{
					if(x==x->fa->son)x->fa->son=tmp;
					else x->fa->bro=tmp;
					if(x->bro)x->bro->fa=tmp;
					tmp->fa=x->fa;
					tmp->bro=x->bro;
				}
			}
			x->fa=x->bro=x->son=nullptr;
		}
		void erase(point_iterator it)
		{
			extract(it);
			dealloc_node(it.p);
		}
		void decrease_key(point_iterator it,const Key &v)
		{
			ptr_t x=it.p;
			x->key=v;
			if(x==root)return;
			if(x->fa->son==x)x->fa->son=x->bro;
			else x->fa->bro=x->bro;
			if(x->bro)x->bro->fa=x->fa;
			x->bro=nullptr;
			x->fa=nullptr;
			root=merge(root,x);
		}
		void modify(point_iterator it,const Key &v)
		{
			extract(it);
			ptr_t x=it.p;
			x->key=v;
			root=merge(root,x);
		}
	};
}
#endif