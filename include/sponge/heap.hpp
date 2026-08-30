#ifndef SPONGE_HEAP_HPP
#define SPONGE_HEAP_HPP
#include<sponge/core.hpp>
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
		struct node_t
		{
			ptr_t fa,bro,son;
			Key key;
			constexpr node_t():fa(),bro(),son(),key(){}
		};
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
		static void delete_tree(ptr_t& x)
		{
			if(x->son)delete_tree(x->son);
			if(x->bro)delete_tree(x->bro);
			dealloc_node(x);
			x=nullptr;
		}
		static ptr_t copy_tree(ptr_t x)
		{
			if(!x)return nullptr;
			ptr_t y=alloc_node();
			y->key=x->key;
			if(y->bro=copy_tree(x->bro))y->bro->fa=y;
			if(y->son=copy_tree(x->son))y->son->fa=y;
			return y;
		}
		static ptr_t merge(ptr_t x,ptr_t y)
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
		static ptr_t pairing_merge(ptr_t x)
		{
			if(!x)return nullptr;
			x->fa=nullptr;
			if(!x->bro)return x;
			ptr_t y=x->bro,z=x->bro->bro;
			y->fa=nullptr;
			x->bro=y->bro=nullptr;
			return merge(pairing_merge(z),merge(x,y));
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
		template<typename Iter>
		pairing_heap(Iter first,Iter last):root()
		{
			insert(first,last);
		}
		pairing_heap(const initializer_list<Key>& il):root()
		{
			insert(il);
		}
		pairing_heap(const pairing_heap& h)noexcept:root(copy_tree(h.root)){}
		pairing_heap& operator=(const pairing_heap& h)noexcept
		{
			if(this!=&h)
			{
				pairing_heap tmp(h);
				swap(tmp);
			}
			return *this;
		}
		pairing_heap(pairing_heap&& h)noexcept:root(h.root)
		{
			h.root=nullptr;
		}
		pairing_heap& operator=(pairing_heap&& h)noexcept
		{
			if(this!=&h)
			{
				pairing_heap tmp(h);
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
			ptr_t x=alloc_node();
			x->key=v;
			root=merge(root,x);
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
			ptr_t tmp=pairing_merge(root->son);
			dealloc_node(root);
			root=tmp;
		}
		void join(pairing_heap& h)
		{
			root=merge(root,h.root);
			h.root=nullptr;
		}
		void swap(pairing_heap& h)
		{
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