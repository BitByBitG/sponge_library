#ifndef SPONGE_VEB_TREE_HPP
#define SPONGE_VEB_TREE_HPP
#include<sponge/core.hpp>
#include<sponge/splitmix64.hpp>
namespace sponge
{
	template<typename T,typename Alloc=allocator<T>,int LeafBits=12>
	class veb_tree
	{
		static_assert(is_integral_v<T>&&is_signed_v<T>);
		static_assert(LeafBits>=6&&LeafBits<=24);
	public:
		using value_type=T;
		using allocator_type=Alloc;
		using alloc_traits=allocator_traits<Alloc>;
	private:
		struct node_t;
	public:
		using node_alloc=typename alloc_traits::template rebind_alloc<node_t>;
		using traits=allocator_traits<node_alloc>;
		using ptr_t=typename traits::pointer;
		inline static node_alloc alloc;
	private:
		using map_t=sm64_map<T,ptr_t>;
		struct bitmap_t
		{
			static constexpr int levels=(LeafBits+5)/6;
			array<size_t,levels> off{};
			vector<uint64_t> a;
			int n=0;
			explicit bitmap_t(int w)
			{
				size_t m=size_t(1)<<w,total=0;
				do
				{
					m=(m+63)/64;
					off[n++]=total;
					total+=m;
				}while(m>1);
				a.assign(total,0);
			}
			bool contains(size_t x)const
			{
				return (a[x>>6]>>(x&63))&1;
			}
			void insert(size_t x)
			{
				for(int i=0;i<n;i++)
				{
					size_t j=x>>6;
					uint64_t& v=a[off[i]+j];
					bool nonempty=v!=0;
					v|=uint64_t(1)<<(x&63);
					if(nonempty)break;
					x=j;
				}
			}
			void erase(size_t x)
			{
				for(int i=0;i<n;i++)
				{
					size_t j=x>>6;
					uint64_t& v=a[off[i]+j];
					v&=~(uint64_t(1)<<(x&63));
					if(v)break;
					x=j;
				}
			}
			T successor(size_t x)const
			{
				for(int i=0;i<n;i++)
				{
					size_t j=x>>6;
					uint64_t v=a[off[i]+j]&((~uint64_t(0)<<(x&63))<<1);
					if(v)
					{
						x=(j<<6)|countr_zero(v);
						while(i>0)
						{
							--i;
							x=(x<<6)|countr_zero(a[off[i]+x]);
						}
						return T(x);
					}
					x=j;
				}
				return -1;
			}
			T predecessor(size_t x)const
			{
				for(int i=0;i<n;i++)
				{
					size_t j=x>>6;
					uint64_t v=a[off[i]+j]&((uint64_t(1)<<(x&63))-1);
					if(v)
					{
						x=(j<<6)|(63-countl_zero(v));
						while(i>0)
						{
							--i;
							x=(x<<6)|(63-countl_zero(a[off[i]+x]));
						}
						return T(x);
					}
					x=j;
				}
				return -1;
			}
		};
		struct node_t
		{
			int w,k;
			T mn=-1,mx=-1;
			uint64_t bits=0;
			bitmap_t* bitmap=nullptr;
			ptr_t summary=nullptr;
			map_t* cluster=nullptr;
			explicit node_t(int w):w(w),k(w/2){}
		};
		ptr_t root=nullptr;
		int w;
		static ptr_t alloc_node(int w)
		{
			ptr_t p=traits::allocate(alloc,1);
			try
			{
				::new((void*)to_address(p)) node_t(w);
			}
			catch(...)
			{
				traits::deallocate(alloc,p,1);
				throw;
			}
			return p;
		}
		static void delete_tree(ptr_t p)
		{
			if(!p)return;
			if(p->cluster)
			{
				for(auto& v:*p->cluster)delete_tree(v.second);
				delete p->cluster;
			}
			delete_tree(p->summary);
			delete p->bitmap;
			to_address(p)->~node_t();
			traits::deallocate(alloc,p,1);
		}
		bool valid(T x)const
		{
			return x>=0&&(w==numeric_limits<T>::digits||(x>>w)==0);
		}
		static T high(ptr_t p,T x)
		{
			return x>>p->k;
		}
		static T low(ptr_t p,T x)
		{
			return x&((T(1)<<p->k)-1);
		}
		static T index(ptr_t p,T h,T l)
		{
			return (h<<p->k)|l;
		}
		static ptr_t child(ptr_t p,T h)
		{
			if(!p->cluster)return nullptr;
			auto it=p->cluster->find(h);
			return it==p->cluster->end()?ptr_t(nullptr):it->second;
		}
		static void update_bits(ptr_t p)
		{
			if(!p->bits)p->mn=p->mx=-1;
			else
			{
				p->mn=countr_zero(p->bits);
				p->mx=63-countl_zero(p->bits);
			}
		}
		static bool contains(ptr_t p,T x)
		{
			if(!p||p->mn==-1)return false;
			if(p->w<=6)return (p->bits>>x)&1;
			if(x==p->mn||x==p->mx)return true;
			if(p->w<=LeafBits)return p->bitmap&&p->bitmap->contains(size_t(x));
			return contains(child(p,high(p,x)),low(p,x));
		}
		static void insert_impl(ptr_t p,T x)
		{
			if(p->w<=6)
			{
				p->bits|=uint64_t(1)<<x;
				update_bits(p);
				return;
			}
			if(p->mn==-1)
			{
				p->mn=p->mx=x;
				return;
			}
			if(p->w<=LeafBits)
			{
				if(!p->bitmap)
				{
					bitmap_t* b=new bitmap_t(p->w);
					b->insert(size_t(p->mn));
					b->insert(size_t(x));
					p->bitmap=b;
				}
				else p->bitmap->insert(size_t(x));
				p->mn=std::min(p->mn,x);
				p->mx=std::max(p->mx,x);
				return;
			}
			T v=std::min(x,p->mn);
			x=std::max(x,p->mn);
			T h=high(p,x),l=low(p,x);
			ptr_t q=child(p,h);
			if(q)insert_impl(q,l);
			else
			{
				q=alloc_node(p->k);
				bool new_map=!p->cluster,added=false,new_summary=!p->summary;
				try
				{
					insert_impl(q,l);
					if(new_map)p->cluster=new map_t;
					p->cluster->insert({h,q});
					added=true;
					if(new_summary)p->summary=alloc_node(p->w-p->k);
					insert_impl(p->summary,h);
				}
				catch(...)
				{
					if(added)p->cluster->erase(h);
					delete_tree(q);
					if(new_summary)
					{
						delete_tree(p->summary);
						p->summary=nullptr;
					}
					if(new_map)
					{
						delete p->cluster;
						p->cluster=nullptr;
					}
					throw;
				}
			}
			p->mn=v;
			p->mx=std::max(p->mx,x);
		}
		static void erase_impl(ptr_t p,T x)
		{
			if(p->w<=6)
			{
				p->bits&=~(uint64_t(1)<<x);
				update_bits(p);
				return;
			}
			if(p->mn==p->mx)
			{
				p->mn=p->mx=-1;
				return;
			}
			if(p->w<=LeafBits)
			{
				if(x==p->mn)p->mn=p->bitmap->successor(size_t(x));
				if(x==p->mx)p->mx=p->bitmap->predecessor(size_t(x));
				p->bitmap->erase(size_t(x));
				if(p->mn==p->mx)
				{
					delete p->bitmap;
					p->bitmap=nullptr;
				}
				return;
			}
			if(x==p->mn)
			{
				T h=p->summary->mn;
				x=index(p,h,child(p,h)->mn);
				p->mn=x;
			}
			T h=high(p,x);
			ptr_t q=child(p,h);
			erase_impl(q,low(p,x));
			if(q->mn==-1)
			{
				p->cluster->erase(h);
				delete_tree(q);
				erase_impl(p->summary,h);
				if(p->summary->mn==-1)
				{
					delete_tree(p->summary);
					p->summary=nullptr;
					delete p->cluster;
					p->cluster=nullptr;
				}
			}
			if(x==p->mx)
			{
				if(!p->summary)p->mx=p->mn;
				else
				{
					T j=p->summary->mx;
					p->mx=index(p,j,child(p,j)->mx);
				}
			}
		}
		static T successor(ptr_t p,T x)
		{
			if(!p||p->mn==-1||x>=p->mx)return -1;
			if(x<p->mn)return p->mn;
			if(p->w<=6)
				return countr_zero(p->bits&(~uint64_t(0)<<(x+1)));
			if(p->w<=LeafBits)return p->bitmap->successor(size_t(x));
			T h=high(p,x),l=low(p,x);
			ptr_t q=child(p,h);
			if(q&&l<q->mx)return index(p,h,successor(q,l));
			T j=successor(p->summary,h);
			return j==-1?T(-1):index(p,j,child(p,j)->mn);
		}
		static T predecessor(ptr_t p,T x)
		{
			if(!p||p->mn==-1||x<=p->mn)return -1;
			if(x>p->mx)return p->mx;
			if(p->w<=6)
				return 63-countl_zero(p->bits&((uint64_t(1)<<x)-1));
			if(p->w<=LeafBits)return p->bitmap->predecessor(size_t(x));
			T h=high(p,x),l=low(p,x);
			ptr_t q=child(p,h);
			if(q&&l>q->mn)return index(p,h,predecessor(q,l));
			T j=predecessor(p->summary,h);
			return j==-1?p->mn:index(p,j,child(p,j)->mx);
		}
	public:
		explicit veb_tree(int w):w(w)
		{
			if(w<1||w>numeric_limits<T>::digits)throw invalid_argument("Invalid bit width");
		}
		~veb_tree()
		{
			delete_tree(root);
		}
		veb_tree(const veb_tree&)=delete;
		veb_tree& operator=(const veb_tree&)=delete;
		veb_tree(veb_tree&& t)noexcept:root(t.root),w(t.w){t.root=nullptr;}
		veb_tree& operator=(veb_tree&& t)noexcept
		{
			if(this!=&t)
			{
				delete_tree(root);
				root=t.root;
				w=t.w;
				t.root=nullptr;
			}
			return *this;
		}
		void clear()
		{
			delete_tree(root);
			root=nullptr;
		}
		bool empty()const
		{
			return !root;
		}
		T min()const
		{
			return root?root->mn:T(-1);
		}
		T max()const
		{
			return root?root->mx:T(-1);
		}
		bool contains(T x)const
		{
			return valid(x)&&contains(root,x);
		}
		bool insert(T x)
		{
			if(!valid(x)||contains(root,x))return false;
			if(!root)
			{
				ptr_t p=alloc_node(w);
				try{insert_impl(p,x);}
				catch(...){delete_tree(p);throw;}
				root=p;
			}
			else insert_impl(root,x);
			return true;
		}
		bool erase(T x)
		{
			if(!contains(x))return false;
			erase_impl(root,x);
			if(root->mn==-1)clear();
			return true;
		}
		T successor(T x)const
		{
			return successor(root,x);
		}
		T predecessor(T x)const
		{
			return predecessor(root,x);
		}
	};
}
#endif
