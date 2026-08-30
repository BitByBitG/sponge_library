#ifndef SPONGE_MATH_HPP
#define SPONGE_MATH_HPP
#include<sponge/core.hpp>
namespace sponge
{
	random_device seed;
	mt19937 rng(seed());
	mt19937_64 rng64(seed());
	template<typename T>
	T fdivp(const T x,const T y)
	{ return x/y; }
	template<typename T>
	T cdivp(const T x,const T y)
	{ return (x-1)/y+1; }
	template<typename T>
	T fdiv(T x,T y)
	{
		if(y<0)x=-x,y=-y;
		if(x>=0)return x/y;
		return -((-x+y-1)/y);
	}
	template<typename T>
	T cdiv(T x,T y)
	{
		if(y<0)x=-x,y=-y;
		if(x>=0)return (x+y-1)/y;
		else return x / y;
	}
	ll _gcd(ll x,ll y)
	{
		ll _x=__builtin_ctzll(x),_y=__builtin_ctzll(y),z=_x<_y?_x:_y,t;
		y>>=_y;
		while(x)
		{
			x>>=_x;
			t=x-y;
			_x=__builtin_ctzll(t);
			y=min(x,y);
			x=t<0?-t:t;
		}
		return y<<z;
	}
	INLINE ll _lcm(ll x,ll y)
	{
		return x/_gcd(x,y)*y;
	}
	template<typename T,typename U>
	T power(T x,U y)
	{
		T z=1;
		for(;y;y>>=1,x*=x)if(y&1)z*=x;
		return z;
	}
	template<typename T,typename U>
	T power(T x,U y,const T mod)
	{
        using promote_type=conditional_t<sizeof(T)<=4,int64_t,__int128_t>;
		x%=mod;
		T z=1;
		for(;y;y>>=1,x=(promote_type)x*x%mod)if(y&1)z=(promote_type)z*x%mod;
		return z;
	}
	ull splitmix64(ull x)
	{
		x+=0x9e3779b97f4a7c15;
		x=(x^(x>>30))*0xbf58476d1ce4e5b9;
		x=(x^(x>>27))*0x94d049bb133111eb;
		return x^(x>>31);
	}
	class sm64
	{
	public:
		ull state;
		sm64():state(0){}
		sm64(ull seed):state(seed){}
		ull operator()()
		{
			ull x=(state+=0x9e3779b97f4a7c15);
			x=(x^(x>>30))*0xbf58476d1ce4e5b9;
			x=(x^(x>>27))*0x94d049bb133111eb;
			return x^(x>>31);
		}
	};
	bool miller_rabin(ll n)
	{
		if(n<2)return 0;
		for(int p:{2,3,5,7,11,13,17,19,23,29,31,37})if(n%p==0)return n==p;
		auto is_composite=[&](ll a,ll n)->bool
		{
			ll x=n-1,y=0;
			while(!(x&1))x>>=1,y++;
			ll x1=power(a,x,n);
			if(x1==1)return 0;
			for(int i=1;i<=y;i++)
			{
				if(x1==n-1)return 0;
				x1=(__int128)x1*x1%n;
			}
			return 1;
		};
		for(ll a:{2,325,9375,28178,450775,9780504,1795265022})
		{
			if(a%n==0)continue;
			if(is_composite(a,n))return 0;
		}
		return 1;
	}
	ll pollard_rho(ll n)
	{
		if(miller_rabin(n))return -1;
		uniform_int_distribution<ll>uid(1,n);
		ll s=0,t=0,c=uid(rng),v=1,d;
		for(ll i=1;;i<<=1,s=t,v=1)
		{
			for(ll j=1;j<=i;j++)
			{
				t=((__int128)t*t%n+c)%n;
				v=(__int128)v*abs(t-s)%n;
				if(j%127==0)if((d=__gcd(v,n))>1)return d;
			}
			if((d=__gcd(v,n))>1)return d;
		}
	}
	vector<ll> factorize(ll n)
	{
		vector<ll> f;
		auto dfs=[&](auto&& dfs,ll n)->void
		{
			if(n<2)return;
			if(miller_rabin(n))return f.push_back(n);
			ll p=pollard_rho(n);
			n/=p;
			dfs(dfs,n);
			dfs(dfs,p);
		};
		dfs(dfs,n);
		return f;
	}
	vector<ll> divisors(ll n)
	{
		vector<ll> f=factorize(n);
		map<ll,int> mp;
		for(auto x:f) mp[x]++;
		vector<ll> d({1});
		for(auto[p,c]:mp)
		{
			int sz=d.size();
			ll pw=1;
			for(int i=1;i<=c;i++)
			{
				pw*=p;
				for(int j=0;j<sz;j++)d.push_back(d[j]*pw);
			}
		}
		return d;
	}
	class prime_sieve
	{
	public:
		vector<int> prime;
		vector<char> is_prime;
		void sieve(int n)
		{
			prime.clear();
			is_prime.assign(n+1,1);
			is_prime[0]=is_prime[1]=0;
			for(int i=2;i<=n;i++)
			{
				if(is_prime[i])prime.push_back(i);
				for(int j:prime)
				{
					if(i*j>n)break;
					is_prime[i*j]=0;
					if(i%j==0)break;
				}
			}
		}
		int operator[](int x)
		{
			return prime[x];
		}
	};
}
#endif