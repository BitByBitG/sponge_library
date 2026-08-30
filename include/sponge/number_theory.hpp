#ifndef SPONGE_NUMBER_THEORY_HPP
#define SPONGE_NUMBER_THEORY_HPP
#include<ext/pb_ds/assoc_container.hpp>
#include<ext/pb_ds/hash_policy.hpp>
#include<sponge/core.hpp>
#include<sponge/modint.hpp>
#include<sponge/math.hpp>
namespace sponge
{
	template<typename T>
	T lucas(int n,int m)
	{
		if(n<m)return 0;
		const int p=T::mod();
		if(n<p&&m<p)return combinatorics<T>::comb(n,m);
		return lucas<T>(n/p,m/p)*lucas<T>(n%p,m%p);
	}
	namespace detail
	{
		ll exlucas_fact(ll n,ll p,ll pk)
		{
			if(!n)return 1;
			ll res=1;
			for(ll i=2;i<=pk;i++)if(i%p)res=res*i%pk;
			res=power(res,n/pk,pk);
			for(ll i=2;i<=n%pk;i++)if(i%p)res=res*i%pk;
			return res*exlucas_fact(n/p,p,pk)%pk;
		}
		ll exlucas_comb(ll n,ll m,ll p,ll pk)
		{
			ll nf=exlucas_fact(n,p,pk),mf=exlucas_fact(m,p,pk),df=exlucas_fact(n-m,p,pk);
			ll k=0;
			for(ll i=n;i;i/=p)k+=i/p;
			for(ll i=m;i;i/=p)k-=i/p;
			for(ll i=n-m;i;i/=p)k-=i/p;
			return nf*exgcd_inv(mf,pk)%pk*exgcd_inv(df,pk)%pk*power(p,k,pk)%pk;
		}
	}
	ll exlucas(ll n,ll m,ll p)
	{
		auto crt=[&](ll b,ll mod)
		{
			return b*exgcd_inv(p/mod,mod)%p*(p/mod)%p;
		};
		ll res=0,tmp=p,pk;
		int lim=sqrt(p)+5;
		for(int i=2;i<=lim;i++)
		{
			if(tmp%i==0)
			{
				pk=1;
				while(tmp%i==0)pk*=i,tmp/=i;
				res=(res+crt(detail::exlucas_comb(n,m,i,pk),pk))%p;
			}
		}
		if(tmp>1)res=(res+crt(detail::exlucas_comb(n,m,tmp,tmp),tmp))%p;
		return res;
	}
	namespace detail
	{
		ll bsgs_base(ll a,ll b,ll p,ll d)
		{
			ll c=ceil(sqrt(p));
			unordered_map<ll,ll>h;
			for(ll y=0,z=1;y<c;y++,z=z*a%p)h[b*z%p]=y;
			ll k=1;
			for(ll i=1;i<=c;i++)k=k*a%p;
			for(ll x=1,z=d*k%p;x<=c;x++,z=z*k%p)
				if(h.find(z)!=h.end())
				{
					ll y=h[z];
					return x*c-y;
				}
			return -1;
		}
	}
	ll bsgs(ll a,ll b,ll p)
	{
		a%=p,b%=p;
		if(b==1||p==1)return 0;
		return detail::bsgs_base(a,b,p,1);
	}
	ll exbsgs(ll a,ll b,ll p)
	{
		a%=p,b%=p;
		if(b==1||p==1)return 0;
		ll c=0,d=1;
		for(ll g;(g=__gcd(a,p))!=1;)
		{
			if(b%g)return -1;
			c++;
			b/=g;
			p/=g;
			d=d*a/g%p;
			if(d==b)return c;
		}
		ll ans=detail::bsgs_base(a,b,p,d);
		if(ans==-1)return -1;
		return ans+c;
	}
	template<typename T,typename FPrimePower>
	class euler_sieve
	{
	public:
		inline static constexpr FPrimePower fpp{};
		vector<int> prime,factor,cnt,g;
		vector<T> f;
		void sieve(int n)
		{
			prime.clear();
			factor.assign(n+1,0);
			cnt.assign(n+1,0);
			f.assign(n+1,0);
			g.assign(n+1,0);
			f[1]=1;
			g[1]=1;
			for(int i=2;i<=n;i++)
			{
				if(!factor[i])
				{
					factor[i]=i;
					prime.push_back(i);
					g[i]=i;
					cnt[i]=1;
					f[i]=fpp(i,1);
				}
				for(int p:prime)
				{
					if(p>factor[i]||(ll)i*p>n)break;
					int x=i*p;
					factor[x]=p;
					if(i%p==0)g[x]=g[i]*p,cnt[x]=cnt[i]+1;
					else g[x]=p,cnt[x]=1;
					if(g[x]==x)f[x]=fpp(p,cnt[x]);
					else f[x]=f[x/g[x]]*f[g[x]];
				}
			}
		}
		T& operator[](int x)
		{
			return f[x];
		}
	};
	namespace detail
	{
		struct math2_phi_sieve_functor
		{
			constexpr int operator()(int p,int k)const
			{
				return power(p,k-1)*(p-1);
			}
		};
		struct math2_mu_sieve_functor
		{
			constexpr int operator()(int,int k)const
			{
				return k==1?-1:0;
			}
		};
	}
	using phi_sieve_=euler_sieve<int,detail::math2_phi_sieve_functor>;
	using mu_sieve_=euler_sieve<int,detail::math2_mu_sieve_functor>;
	template<typename T,typename ConvolutionPrefix,typename GPrefix>
	class du_sieve
	{
	public:
		inline static constexpr ConvolutionPrefix convolution_prefix{};
		inline static constexpr GPrefix g_prefix{};
		vector<T>initial;
		__gnu_pbds::gp_hash_table<ll,T>f_prefix;
		ll initial_size(ll n)
		{
			return pow(n,2.0/3);
		}
		void initialize()
		{
			partial_sum(initial.begin(),initial.end(),initial.begin());
		}
		T operator()(ll n)
		{
			ll m=initial.size()-1;
			T g1=g_prefix(1);
			auto recurse=[&](auto&& recurse,ll x)->T
			{
				if(x<=m)return initial[x];
				if(f_prefix.find(x)!=f_prefix.end())return f_prefix[x];
				T ans=convolution_prefix(x);
				for(ll l=2,r;l<=x;l=r+1)
				{
					r=min(x/(x/l),x);
					ans-=(g_prefix(r)-g_prefix(l-1))*recurse(recurse,x/l);
				}
				return f_prefix[x]=ans/g1;
			};
			return recurse(recurse,n);
		}
	};
}
#endif