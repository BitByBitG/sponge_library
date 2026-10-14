#ifndef SPONGE_ALLOCATOR_HPP
#define SPONGE_ALLOCATOR_HPP
#include<sponge/core.hpp>
namespace sponge
{
	template<typename T>
	class static_allocator;
	template<typename T>
	struct ptr_base
	{
		using alloc=static_allocator<T>;
		int p;
		ptr_base():p(0){}
		ptr_base(nullptr_t):p(0){}
		explicit ptr_base(int x):p(x){}
		T& operator*()const{ return alloc::mempool[p]; }
		T* operator->()const{ return &alloc::mempool[p]; }
		explicit operator bool()const{ return p; }
		friend bool operator==(ptr_base a,ptr_base b){ return a.p==b.p; }
		friend bool operator!=(ptr_base a,ptr_base b){ return a.p!=b.p; }
		explicit operator T*()const{ return &alloc::mempool[p]; }
	};
	template<typename T>
	class static_allocator
	{
	public:
		using value_type=T;
		using size_type=size_t;
		using difference_type=ptrdiff_t;
		using pointer=ptr_base<T>;
		template<typename U>
		struct rebind
		{
			using other=static_allocator<U>;
		};
		static_allocator()=default;
		~static_allocator()=default;
		template<typename U>
		static_allocator(const static_allocator<U>&){}
		pointer allocate(size_type n)
		{
			if(n!=1)throw bad_alloc();
			return alloc1();
		}
		void deallocate(pointer p,size_type)
		{
			free1(p);
		}
		inline static T* mempool;
		inline static int cap;
		inline static int top;
		inline static int live;
	private:
		struct pool_t
		{
			~pool_t()
			{
				if(mempool)allocator<T>{}.deallocate(mempool,cap);
			}
		};
		inline static pool_t pool;
	public:
		inline static vector<int> stk;
		static void expand(int n)
		{
			if(n<0||n>numeric_limits<int>::max()-10)throw length_error("Invalid capacity");
			if(live)throw logic_error("Pool has live objects");
			(void)pool;
			n+=10;
			T* p=allocator<T>{}.allocate(n);
			if(mempool)allocator<T>{}.deallocate(mempool,cap);
			mempool=p;
			top=0;
			cap=n;
			stk.clear();
		}
        static void expand_mb(int m)
        {
            if(m<0||((ll)m<<20)/sizeof(T)>numeric_limits<int>::max()-10)
				throw length_error("Invalid capacity");
			expand(((ll)m<<20)/sizeof(T));
        }
		static int size()
		{
			return top;
		}
		static int capacity()
		{
			return cap;
		}
		static double mb()
		{
			return sizeof(T)*cap/static_cast<double>(1<<20);
		}
		static pointer alloc1()
		{
			if(!stk.empty())
			{
				int x=stk.back();
				stk.pop_back();
				++live;
				return pointer(x);
			}
			if(top>=cap-1)throw bad_alloc();
			++live;
			return pointer(++top);
		}
		static void free1(pointer p)
		{
			stk.push_back(p.p);
			--live;
		}
	};
}
#endif