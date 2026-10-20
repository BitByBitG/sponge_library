#ifndef SPONGE_RBT_HPP
#define SPONGE_RBT_HPP
#include<sponge/core.hpp>
#include<sponge/utility.hpp>
#include<iterator>
namespace sponge
{
	enum rbt_color:bool
	{
		red=1,
		black=0
	};
	template<typename Key,typename Compare=less<Key>,typename Alloc=allocator<Key>>
	class rbt
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
		template<typename... Args>
		static ptr_t alloc_node(Args&&... args)
		{
			ptr_t p=traits::allocate(alloc,1);
			try
			{
				::new((void*)to_address(p)) node_t(forward<Args>(args)...);
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
			to_address(p)->~node_t();
			traits::deallocate(alloc,p,1);
		}
		struct node_t
		{
			ptr_t fa;
			array<ptr_t,2> son;
			int size;
			rbt_color color;
			Key key;
			constexpr node_t():fa(),son{},size(1),color(black),key(){}
			constexpr node_t(const Key& v):fa(),son{},size(1),color(black),key(v){}
		};
		bool cmpeq(const Key& x,const Key& y)
		{
			return !cmp(x,y)&&!cmp(y,x);
		}
		inline static bool is_guard(ptr_t x)
		{
			return x->size==-1;
		}
		inline static bool is_root(ptr_t x)
		{
			return is_guard(x->fa);
		}
		static void delete_tree(ptr_t& x)
		{
			if(x->son[0])delete_tree(x->son[0]);
			if(x->son[1])delete_tree(x->son[1]);
			dealloc_node(x);
			x=nullptr;
		}
		inline static int size(ptr_t x)
		{
			return x?x->size:0;
		}
		inline static bool is_red(ptr_t x)
		{
			return x?x->color==red:0;
		}
		inline static bool is_black(ptr_t x)
		{
			return !is_red(x);
		}
		static bool dir(ptr_t x)
		{
			return x==x->fa->son[1];
		}
		static ptr_t leftest(ptr_t x)
		{
			while(x->son[0])x=x->son[0];
			return x;
		}
		static ptr_t rightest(ptr_t x)
		{
			while(x->son[1])x=x->son[1];
			return x;
		}
		static ptr_t prev_node(ptr_t x)
		{
			if(is_guard(x))return rightest(x->son[0]);
			if(x->son[0])return rightest(x->son[0]);
			while(!is_root(x)&&x==x->fa->son[0])x=x->fa;
			return x->fa;
		}
		static ptr_t next_node(ptr_t x)
		{
			if(is_guard(x))return leftest(x->son[0]);
			if(x->son[1])return leftest(x->son[1]);
			while(!is_root(x)&&x==x->fa->son[1])x=x->fa;
			return x->fa;
		}
		static int rank(ptr_t x)
		{
			if(is_guard(x))return x->son[0]?x->son[0]->size+1:1;
			int ans=size(x->son[0]);
			while(!is_root(x))
			{
				if(x==x->fa->son[1])ans+=size(x->fa->son[0])+1;
				x=x->fa;
			}
			return ++ans;
		}
		static ptr_t nth(ptr_t x,int n)
		{
			while(size(x)>1)
				if(size(x->son[0])>=n)x=x->son[0];
				else if(size(x->son[0])==n-1)return x;
				else n-=size(x->son[0])+1,x=x->son[1];
			return x;
		}
		ptr_t find_key(ptr_t now,const Key& v)
		{
			ptr_t ans=nullptr;
			while(now)
			{
				if(!cmp(now->key,v))ans=now,now=now->son[0];
				else now=now->son[1];
			}
			if(ans&&cmpeq(ans->key,v))return ans;
			else return nullptr;
		}
	public:
		class iterator
		{
		public:
			ptr_t p;
			using iterator_category=bidirectional_iterator_tag;
			using value_type=Key;
			using difference_type=ptrdiff_t;
			using pointer=Key*;
			using reference=Key&;
			constexpr iterator():p(nullptr){}
			constexpr iterator(ptr_t _p):p(_p){}
			ptr_t& raw()
			{
				return p;
			}
			iterator& operator++()
			{
				return p=next_node(p),*this;
			}
			iterator operator++(int)
			{
				iterator it=*this;
				++*this;
				return it;
			}
			iterator& operator--()
			{
				return p=prev_node(p),*this;
			}
			iterator operator--(int)
			{
				iterator it=*this;
				--*this;
				return it;
			}
			bool operator==(const iterator& x)const
			{
				return p==x.p;
			}
			bool operator!=(const iterator& x)const
			{
				return p!=x.p;
			}
			reference operator*()const
			{
				return p->key;
			}
			pointer operator->()const
			{
				return &p->key;
			}
		};
		using reverse_iterator=std::reverse_iterator<iterator>;
	private:
		ptr_t root,guard;
	public:
		rbt():rbt(Compare{}){}
		explicit rbt(Compare c):cmp(move(c)),root(nullptr),guard(alloc_node())
		{
			guard->size=-1;
			maintain_guard();
		}
		rbt(const rbt& t):rbt(t.cmp)
		{
			root=copy_tree(t.root);
			maintain_guard();
		}
		rbt& operator=(const rbt& t)
		{
			if(this!=&t)
			{
				rbt tmp(t);
				swap(tmp);
			}
			return *this;
		}
		rbt(rbt&& t):rbt(t.cmp)
		{
			swap(t);
		}
		rbt& operator=(rbt&& t)
		{
			if(this!=&t)
			{
				rbt tmp(move(t));
				swap(tmp);
			}
			return *this;
		}
		template<typename Compare2,typename Alloc2,enable_if_t<!is_same_v<Compare,Compare2>||!is_same_v<Alloc,Alloc2>,int> =0>
		rbt(rbt<Key,Compare2,Alloc2>& t):rbt()
		{
			insert(t.begin(),t.end());
		}
		template<typename Iter>
		rbt(Iter first,Iter last):rbt()
		{
			insert(first,last);
		}
		rbt(initializer_list<Key> il):rbt(il.begin(),il.end()){}
		~rbt()
		{
			clear();
			dealloc_node(guard);
		}
	private:
		static ptr_t copy_tree(ptr_t x)
		{
			if(!x)return nullptr;
			ptr_t y=alloc_node(x->key);
			y->size=x->size;
			y->color=x->color;
			try
			{
				for(int i=0;i<2;i++)
				{
					y->son[i]=copy_tree(x->son[i]);
					if(y->son[i])y->son[i]->fa=y;
				}
			}
			catch(...)
			{
				delete_tree(y);
				throw;
			}
			return y;
		}
		inline void maintain_guard()
		{
			guard->son[0]=root;
			if(root)root->fa=guard;
		}
	public:
		int size()const
		{
			return root?root->size:0;
		}
		bool empty()const
		{
			return !root;
		}
		void clear()
		{
			if(root)delete_tree(root);
			maintain_guard();
		}
		iterator begin()
		{
			return root?leftest(root):guard;
		}
		iterator end()
		{
			return guard;
		}
		reverse_iterator rbegin()
		{
			return reverse_iterator(end());
		}
		reverse_iterator rend()
		{
			return reverse_iterator(begin());
		}
		iterator root_iterator()
		{
			return root;
		}
	private:
		ptr_t rotate(ptr_t p,bool r)
		{
			ptr_t g=p->fa,s=p->son[!r];
			bool dir_p=dir(p);
			s->size=p->size;
			p->size=size(p->son[r])+size(s->son[r])+1;
			ptr_t c=s->son[r];
			if(c)c->fa=p;
			p->son[!r]=c;
			s->son[r]=p;
			p->fa=s;
			s->fa=g;
			g->son[dir_p]=s;
			if(root==p)root=s;
			return s;
		}
	public:
		pair<iterator,bool> insert(const Key& v)
		{
			ptr_t now=root,p=nullptr;
			bool r=0;
			while(now)
			{
				p=now;
				r=cmp(now->key,v);
				now=now->son[r];
			}
			ptr_t n=alloc_node(v);
			insert_rebalance(p,n,r);
			maintain_guard();
			return {iterator(n),true};
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
	private:
		void insert_leaf(ptr_t p,ptr_t n,bool r)
		{
			if(!p)return root=n,maintain_guard(),[]{}();
			p->son[r]=n;
			n->fa=p;
			for(ptr_t now=p;!is_guard(now);now=now->fa)now->size++;
		}
		void insert_rebalance(ptr_t p,ptr_t n,bool r)
		{
			n->color=p!=nullptr;
			insert_leaf(p,n,r);
			while(is_red(p=n->fa))
			{
				bool dir_p=dir(p);
				ptr_t g=p->fa,u=g->son[!dir_p];
				if(is_red(u))
				{
					p->color=u->color=black;
					g->color=red;
					n=g;
					continue;
				}
				if(dir(n)!=dir_p)rotate(p,dir_p),std::swap(n,p);
				p->color=black;
				g->color=red;
				rotate(g,!dir_p);
			}
			root->color=black;
		}
	public:
		bool erase(const Key& v)
		{
			ptr_t p=find_key(root,v);
			if(!p)return 0;
			erase((iterator)p);
			return 1;
		}
		iterator erase(iterator it)
		{
			ptr_t p=it.p;
			if(!p||is_guard(p))return guard;
			ptr_t ans;
			if(p->son[0]&&p->son[1])
			{
				ptr_t s=leftest(p->son[1]);
				bool dir_p=dir(p),dir_s=dir(s),f=(s->fa==p),g=(p==root);
				std::swap(p->fa,s->fa);
				std::swap(p->son,s->son);
				std::swap(p->size,s->size);
				std::swap(p->color,s->color);
				if(f)p->fa=s,s->son[dir_s]=p;
				if(g)root=s;
				p->fa->son[dir_s]=p;
				if(p->son[1])p->son[1]->fa=p;
				s->fa->son[dir_p]=s;
				s->son[0]->fa=s;
				s->son[1]->fa=s;
				ans=s;
			}
			else ans=next_node(p);
			erase_rebalance(p);
			dealloc_node(p);
			maintain_guard();
			return ans;
		}
		void erase(iterator l,iterator r)
		{
			for(iterator it=l;it!=r;it=erase(it));
		}
		template<typename it_t>
		void erase(it_t bg,it_t ed)
		{
			for(it_t it=bg;it!=ed;++it)erase(*it);
		}
		void erase(initializer_list<Key>l)
		{
			erase(l.begin(),l.end());
		}
	private:
		void erase_node(ptr_t n)
		{
			ptr_t p=n->fa,s=n->son[0]?n->son[0]:n->son[1];
			if(s)s->fa=p;
			if(is_guard(p))return root=s,[]{}();
			p->son[dir(n)]=s;
			for(ptr_t now=p;!is_guard(now);now=now->fa)now->size--;
		}
		void erase_rebalance(ptr_t n)
		{
			bool dir_n=(n==root?0:dir(n));
			erase_node(n);
			ptr_t p=n->fa;
			if(is_guard(p))
			{
				if(root)root->color=black;
				return;
			}
			else
			{
				ptr_t s=p->son[dir_n];
				if(s)return s->color=black,[]{}();
			}
			while(!is_guard(p)&&is_black(n))
			{
				ptr_t s=p->son[!dir_n];
				if(is_red(s))
				{
					s->color=black,p->color=red;
					rotate(p,dir_n);
					s=p->son[!dir_n];
				}
				auto c=s->son[dir_n],d=s->son[!dir_n];
				if(is_black(c)&&is_black(d))
				{
					s->color=red;
					n=p;
					goto end_rebalance;
				}
				if(is_black(d))
				{
					c->color=black,s->color=red;
					rotate(s,!dir_n);
					s=p->son[!dir_n],c=s->son[dir_n],d=s->son[!dir_n];
				}
				s->color=p->color;
				p->color=d->color=black;
				rotate(p,dir_n);
				n=root;
			end_rebalance:
				p=n->fa;
				if(is_guard(p))break;
				dir_n=dir(n);
			}
			n->color=black;
		}
	public:
		iterator find(const Key& v)
		{
			ptr_t ans=find_key(root,v);
			return ans?ans:guard;
		}
		iterator lower_bound(const Key& v)
		{
			ptr_t now=root,ans=nullptr;
			while(now)
				if(!cmp(now->key,v))ans=now,now=now->son[0];
				else now=now->son[1];
			return ans?ans:guard;
		}
		iterator upper_bound(const Key& v)
		{
			ptr_t now=root,ans=nullptr;
			while(now)
				if(cmp(v,now->key))ans=now,now=now->son[0];
				else now=now->son[1];
			return ans?ans:guard;
		}
		iterator predecessor(const Key& v)
		{
			return find_by_order(order_of_key(v)-1);
		}
		iterator successor(const Key& v)
		{
			return upper_bound(v);
		}
		int order_of_key(const Key& v)
		{
			int ans=0;
			ptr_t now=root;
			while(now)
				if(!cmp(now->key,v))now=now->son[0];
				else ans+=size(now->son[0])+1,now=now->son[1];
			return ans;
		}
		iterator find_by_order(int n)
		{
			if(n<0||n>=size())return guard;
			++n;
			ptr_t ans=nth(root,n);
			return ans?ans:guard;
		}
		int count(const Key& v)
		{
			return rank(upper_bound(v).p)-1-order_of_key(v);
		}
		int count(const Key& l,const Key& r)
		{
			if(cmp(r,l))return 0;
			return rank(upper_bound(r).p)-1-order_of_key(l);
		}
		void swap(rbt& t)
		{
			std::swap(cmp,t.cmp);
			std::swap(root,t.root);
			std::swap(guard,t.guard);
		}
		friend void swap(rbt& x,rbt& y)
		{
			x.swap(y);
		}
		void heuristic_merge(rbt& t)
		{
			if(this==&t)return;
			if(size()<t.size())swap(t);
			insert(t.begin(),t.end());
			t.clear();
		}
	};
}
#endif
