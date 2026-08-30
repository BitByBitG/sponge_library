#ifndef SPONGE_MODINT_HPP
#define SPONGE_MODINT_HPP
#include<sponge/core.hpp>
namespace sponge
{
	template<typename T=int64_t,typename U=__int128_t>
	class barrett64
	{
		T i,p;
	public:
		using value_type=T;
		using promote_type=U;
		barrett64()
		{
			set_mod(998244353);
		}
		barrett64(T _p)
		{
			set_mod(_p);
		}
		void set_mod(T _p)
		{
			p=_p;
			i=(U(1)<<64)/p;
		}
		inline T mod()const
		{
			return p;
		}
		T operator()(T x)const
		{
			T r=x-((U(x)*i)>>64)*p;
			if(r>=p)r-=p;
			return r;
		}
	};
	namespace detail
	{
		template<typename T>
		T exgcd_base(T a,T b,T& x,T& y)
		{
			if(!b)return x=1,y=0,a;
			T d=exgcd_base(b,a%b,y,x);
			y-=a/b*x;
			return d;
		}
	}
	template<typename T>
	INLINE array<T,3> exgcd(T a,T b)
	{
		T d,x,y;
		d=detail::exgcd_base(a,b,x,y);
		return {d,x,y};
	}
	template<typename T>
	T exgcd_inv(T x,const T p)
	{
		return (exgcd<T>(x,p)[1]+p)%p;
	}
	template<typename T>
	class combinatorics
	{
	private:
		inline static vector<T> inv_v,fact_v,ifact_v;
	public:
		static void initialize(int n)
		{
			inv_v.resize(n+1);
			fact_v.resize(n+1);
			ifact_v.resize(n+1);
			inv_v[1]=1;
			fact_v[0]=fact_v[1]=1;
			ifact_v[0]=ifact_v[1]=1;
			for(int i=2;i<=n;i++)
			{
				auto d=div(T::mod(),i);
				inv_v[i]=inv_v[d.rem]*(T::mod()-d.quot);
				fact_v[i]=fact_v[i-1]*i;
				ifact_v[i]=ifact_v[i-1]*inv_v[i];
			}
		}
		static inline T inv(int x)
		{
			return inv_v[x];
		}
		static inline T fact(int x)
		{
			return fact_v[x];
		}
		static inline T ifact(int x)
		{
			return ifact_v[x];
		}
		static inline T comb(int x,int y)
		{
			if(y<0||y>x)return 0;
			return fact_v[x]*ifact_v[y]*ifact_v[x-y];
		}
		static inline T perm(int x,int y)
		{
			if(y<0||y>x)return 0;
			return fact_v[x]*ifact_v[x-y];
		}
	};
	template<typename T=uint32_t,T m=998244353,bool is_prime=1>
	class static_modint:public combinatorics<static_modint<T,m,is_prime>>
	{
	public:
		T x;
		using _c=combinatorics<static_modint<T,m,is_prime>>;
		using _c::initialize,_c::inv,_c::fact,_c::ifact,_c::comb,_c::perm;
		using value_type=T;
		using signed_type=make_signed_t<T>;
		static constexpr T mod()
		{
			return m;
		}
		static constexpr signed_type smod()
		{
			return m;
		}
		constexpr static_modint():x(0){}
		template<typename U>
		constexpr static_modint(U _x)
		{
			_x%=smod();
			if(_x<0)_x+=smod();
			x=_x;
		}
		T val()const
		{
			return x;
		}
		signed_type sval()const
		{
			return static_cast<signed_type>(x);
		}
		T& raw()
		{
			return x;
		}
		static_modint& operator++()
		{
			x++;
			if (x==mod())x=0;
			return *this;
		}
		static_modint& operator--()
		{
			if(x==0)x=mod();
			x--;
			return *this;
		}
		static_modint operator++(int)
		{
			static_modint res=*this;
			++*this;
			return res;
		}
		static_modint operator--(int)
		{
			static_modint res=*this;
			--*this;
			return res;
		}
		static_modint& operator+=(const static_modint y)
		{
			x+=y.x;
			if(x>=mod())x-=mod();
			return *this;
		}
		static_modint& operator-=(const static_modint y)
		{
			if(x<y.x)x+=mod();
			x-=y.x;
			return *this;
		}
		static_modint& operator*=(const static_modint y)
		{
			if constexpr(sizeof(T)<=4)x=(uint64_t)x*y.x%mod();
			else x=(__uint128_t)x*y.x%mod();
			return *this;
		}
		template<typename U>
		static_modint pow(U x)const
		{
			static_modint y=*this,z=1;
			for(;x;x>>=1,y*=y)if(x&1)z*=y;
			return z;
		}
		static_modint inv()const
		{
			if constexpr(is_prime)return pow(mod()-2);
			else return exgcd_inv<signed_type>(x,mod());
		}
		static_modint& operator/=(const static_modint y)
		{
			return *this=*this*y.inv();
		}
		static_modint operator+()const
		{
			return *this;
		}
		static_modint operator-()const
		{
			return static_modint()-*this;
		}
		friend static_modint operator+(static_modint x,const static_modint y)
		{
			return x+=y;
		}
		friend static_modint operator-(static_modint x,const static_modint y)
		{
			return x-=y;
		}
		friend static_modint operator*(static_modint x,const static_modint y)
		{
			return x*=y;
		}
		friend static_modint operator/(static_modint x,const static_modint y)
		{
			return x/=y;
		}
		friend bool operator==(const static_modint x,const static_modint y)
		{
			return x.x==y.x;
		}
		friend bool operator!=(const static_modint x,const static_modint y)
		{
			return x.x!=y.x;
		}
		#if __cplusplus>=202002L
		friend auto operator<=>(const static_modint x,const static_modint y)=default;
		#endif
		template<typename Istream>
		friend Istream& operator>>(Istream& is,static_modint& x)
		{
			int64_t y;
			is>>y;
			x=static_modint(y);
			return is;
		}
		template<typename Ostream>
		friend Ostream& operator<<(Ostream& os,const static_modint& x)
		{
			return os<<x.val();
		}
	};
	template<typename T=uint32_t,int id=1,bool is_prime=1>
	class dynamic_modint:public combinatorics<static_modint<T,id,is_prime>>
	{
	public:
		T x;
		using _c=combinatorics<static_modint<T,id,is_prime>>;
		inline static barrett64<> reduction{};
		using _c::initialize,_c::inv,_c::fact,_c::ifact,_c::comb,_c::perm;
		using value_type=T;
		using signed_type=make_signed_t<T>;
		static inline T mod()
		{
			return reduction.mod();
		}
		static inline signed_type smod()
		{
			return reduction.mod();
		}
		constexpr dynamic_modint():x(0){}
		template<typename U>
		dynamic_modint(U _x):x(reduction(_x)){}
		T val()const
		{
			return x;
		}
		signed_type sval()const
		{
			return static_cast<signed_type>(x);
		}
		T& raw()
		{
			return x;
		}
		static void set_mod(T m)
		{
			reduction.set_mod(m);
		}
		dynamic_modint& operator++()
		{
			x++;
			if (x==mod())x=0;
			return *this;
		}
		dynamic_modint& operator--()
		{
			if(x==0)x=mod();
			x--;
			return *this;
		}
		dynamic_modint operator++(int)
		{
			dynamic_modint res=*this;
			++*this;
			return res;
		}
		dynamic_modint operator--(int)
		{
			dynamic_modint res=*this;
			--*this;
			return res;
		}
		dynamic_modint& operator+=(const dynamic_modint y)
		{
			x+=y.x;
			if(x>=mod())x-=mod();
			return *this;
		}
		dynamic_modint& operator-=(const dynamic_modint y)
		{
			if(x<y.x)x+=mod();
			x-=y.x;
			return *this;
		}
		dynamic_modint& operator*=(const dynamic_modint y)
		{
			if constexpr(sizeof(T)<=4)x=reduction((uint64_t)x*y.x);
			else x=(__uint128_t)x*y.x%mod();
			return *this;
		}
		template<typename U>
		dynamic_modint pow(U x)const
		{
			dynamic_modint y=*this,z=1;
			for(;x;x>>=1,y*=y)if(x&1)z*=y;
			return z;
		}
		dynamic_modint inv()const
		{
			if constexpr(is_prime)return pow(mod()-2);
			else return exgcd_inv<signed_type>(x,mod());
		}
		dynamic_modint& operator/=(const dynamic_modint y)
		{
			return *this=*this*y.inv();
		}
		dynamic_modint operator+()const
		{
			return *this;
		}
		dynamic_modint operator-()const
		{
			return dynamic_modint()-*this;
		}
		friend dynamic_modint operator+(dynamic_modint x,const dynamic_modint y)
		{
			return x+=y;
		}
		friend dynamic_modint operator-(dynamic_modint x,const dynamic_modint y)
		{
			return x-=y;
		}
		friend dynamic_modint operator*(dynamic_modint x,const dynamic_modint y)
		{
			return x*=y;
		}
		friend dynamic_modint operator/(dynamic_modint x,const dynamic_modint y)
		{
			return x/=y;
		}
		friend bool operator==(const dynamic_modint x,const dynamic_modint y)
		{
			return x.x==y.x;
		}
		friend bool operator!=(const dynamic_modint x,const dynamic_modint y)
		{
			return x.x!=y.x;
		}
		#if __cplusplus>=202002L
		friend auto operator<=>(const dynamic_modint x,const dynamic_modint y)=default;
		#endif
		template<typename Istream>
		friend Istream& operator>>(Istream& is,dynamic_modint& x)
		{
			int64_t y;
			is>>y;
			x=dynamic_modint(y);
			return is;
		}
		template<typename Ostream>
		friend Ostream& operator<<(Ostream& os,const dynamic_modint& x)
		{
			return os<<x.val();
		}
	};
}
#endif