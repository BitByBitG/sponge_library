#ifndef SPONGE_POLY_HPP
#define SPONGE_POLY_HPP
#include<sponge/core.hpp>
#include<sponge/modint.hpp>
namespace sponge
{
	ll Cipolla(ll x,ll p)
	{
		uniform_int_distribution<int>uid(0,p-1);
		if(!x)return 0;
		else if(power(x,(p-1)>>1,p)==p-1)return -1;
		else
		{
			ll r;
			for(r=uid(rng);;r=uid(rng))if(power((r*r-x+p)%p,(p-1)>>1,p)==p-1)break;
			ll v=(r*r-x+p)%p;
			auto multiply=[&](const pair<ll,ll>&x,const pair<ll,ll>&y)->pair<ll,ll>
			{
				return make_pair((x.first*y.first%p+v*x.second%p*y.second%p)%p,(x.first*y.second%p+x.second*y.first%p)%p);
			};
			auto binary_power=[&](pair<ll,ll>x,ll y)->pair<ll,ll>
			{
				pair<ll,ll>z(1,0);
				for(;y;y>>=1,x=multiply(x,x))if(y&1)z=multiply(z,x);
				return z;
			};
			ll a=binary_power(make_pair(r,1ll),(p+1)>>1).first;
			return min(a,p-a);
		}
	}
	template<typename T>
	class polynomial:public vector<T>
	{
	public:
		using vector<T>::vector;
		template<typename Istream>
		friend Istream& operator>>(Istream& is,polynomial& x)
		{
			for(auto& i:x)is>>i;
			return is;
		}
		template<typename Ostream>
		friend Ostream& operator<<(Ostream& os,const polynomial& x)
		{
			for(const auto& i:x)os<<i<<' ';
			return os;
		}
	};
	class fft
	{
	public:
		using value_type=complex<double>;
		using poly_type=polynomial<complex<double>>;
		static constexpr double pi=numbers::pi;
		vector<int> rev;
		int n;
		int size()const
		{
			return n;
		}
		void initialize(int _n)
		{
			int n_=bit_ceil((uint32_t)_n);
			if(n==n_)return;
			n=n_;
			int l=__lg(n);
			rev.resize(n+1);
			for(int i=1;i<=n-1;i++)rev[i]=(rev[i>>1]>>1)|((i&1)<<(l-1));
		}
		template<bool idft=0>
		void operator()(poly_type& a)const
		{
			a.resize(n);
			for(int i=1;i<n;i++)if(i<rev[i])swap(a[i],a[rev[i]]);
			for(int mid=1,len=2;mid<n;mid<<=1,len<<=1)
			{
				complex<double>omega(cos(pi/mid),idft?-sin(pi/mid):sin(pi/mid));
				for(int i=0;i<n;i+=len)
				{
					complex<double>f(1,0),x,y;
					for(int j=0;j<mid;j++,f*=omega)
						x=a[i|j],y=a[i|j|mid]*f,a[i|j]=x+y,a[i|j|mid]=x-y;
				}
			}
			if constexpr(idft)
				for(int i=0;i<n;i++)
					a[i].real(a[i].real()/n);
		}
		void dft(poly_type& a)
		{
			this->operator()(a);
		}
		void idft(poly_type& a)
		{
			this->template operator()<1>(a);
		}
	};
	template<uint32_t mod=998244353,uint32_t _pr=3>
	class ntt
	{
	public:
		using mint=static_modint<uint32_t,mod,1>;
		using value_type=mint;
		using poly_type=polynomial<mint>;
		const mint pr=mint(_pr),ipr=pr.inv();
		vector<mint> pow_pr,pow_ipr;
		vector<int> rev;
		int n;
		ntt():n(0){}
		int size()const
		{
			return n;
		}
		void initialize(int _n)
		{
			int n_=bit_ceil((uint32_t)_n);
			if(n==n_)return;
			n=n_;
			int l=__lg(n);
			rev.resize(n+1);
			for(int i=1;i<=n-1;i++)rev[i]=(rev[i>>1]>>1)|((i&1)<<(l-1));
			if(l>(int)pow_pr.size()-1)
			{
				pow_pr.resize(l+1);
				pow_ipr.resize(l+1);
				for(int i=1,j=2;j<=n;i++,j<<=1)pow_pr[i]=pr.pow((mod-1)/j),pow_ipr[i]=ipr.pow((mod-1)/j);
			}
		}
		template<bool idft=0>
		void operator()(poly_type& a)const
		{
			a.resize(n);
			const vector<mint>& _pow=(idft?pow_ipr:pow_pr);
			for(int i=1;i<n;i++)if(i<rev[i])swap(a[i],a[rev[i]]);
			for(int mid=1,len=2,k=1;mid<n;mid<<=1,len<<=1,k++)
				for(int i=0;i<n;i+=len)
				{
					mint g=_pow[k],f=1,x,y;
					for(int j=0;j<mid;j++,f*=g)
						x=a[i|j],y=a[i|j|mid]*f,a[i|j]=x+y,a[i|j|mid]=x-y;
				}
			if constexpr(idft)
			{
				mint inv=mint(n).inv();
				for(int i=0;i<n;i++)a[i]*=inv;
			}
		}
		void dft(poly_type& a)
		{
			this->operator()(a);
		}
		void idft(poly_type& a)
		{
			this->template operator()<1>(a);
		}
		poly_type resize(poly_type a,int n)
		{
			a.resize(n);
			return a;
		}
		poly_type reverse(poly_type a)
		{
			std::reverse(a.begin(),a.end());
			return a;
		}
		poly_type plus(poly_type a,poly_type b)
		{
			int n=max(a.size(),b.size());
			a.resize(n),b.resize(n);
			for(int i=0;i<n;i++)a[i]+=b[i];
			return a;
		}
		poly_type minus(poly_type a,poly_type b)
		{
			int n=max(a.size(),b.size());
			a.resize(n),b.resize(n);
			for(int i=0;i<n;i++)a[i]-=b[i];
			return a;
		}
		poly_type multiply(poly_type a,mint x)
		{
			for(auto& i:a)i*=x;
			return a;
		}
		poly_type bf_convolution(const poly_type& a,const poly_type& b)
		{
			if(a.empty()||b.empty())return {};
			int n=ssize(a),m=ssize(b);
			poly_type c(n+m-1);
			for(int i=0;i<n;i++)
				for(int j=0;j<m;j++)
					c[i+j]+=a[i]*b[j];
			return c;
		}
		poly_type convolution(poly_type a,poly_type b)
		{
			int n=a.size(),m=b.size();
			initialize(n+m);
			dft(a);
			dft(b);
			for(int i=0;i<size();i++)a[i]*=b[i];
			idft(a);
			a.resize(n+m-1);
			return a;
		}
		void inverse_newton_iteration(const poly_type& a,poly_type& b,poly_type& c,int n)
		{
			if(n==1)return b[0]=a[0].inv(),[]{}();
			inverse_newton_iteration(a,b,c,(n+1)>>1);
			initialize(n<<1);
			c.resize(n);
			for(int i=0;i<n;i++)c[i]=a[i];
			dft(b);
			dft(c);
			for(int i=0;i<size();i++)b[i]*=2-b[i]*c[i];
			idft(b);
			for(int i=n;i<size();i++)b[i]=0;
		}
		poly_type inverse(const poly_type& a)
		{
			int n=a.size();
			poly_type b(n),c;
			inverse_newton_iteration(a,b,c,n);
			b.resize(n);
			return b;
		}
		poly_type differentiate(const poly_type& a)
		{
			int n=a.size();
			poly_type b(n);
			for(int i=1;i<n;i++)b[i-1]=a[i]*i;
			b[n-1]=0;
			return b;
		}
		poly_type integrate(const poly_type& a)
		{
			int n=a.size();
			poly_type b(n);
			b[0]=0;
			for(int i=0;i<n-1;i++)b[i+1]=mint(i+1).inv()*a[i];
			return b;
		}
		poly_type ln(const poly_type& a)
		{
			int n=a.size();
			poly_type b=convolution(inverse(a),differentiate(a));
			b.resize(n);
			return integrate(b);
		}
		void exp_newton_iteration(const poly_type& a,poly_type& b,poly_type& c,int n)
		{
			if(n==1)return b[0]=1,[]{}();
			exp_newton_iteration(a,b,c,(n+1)>>1);
			b.resize(n);
			c=ln(b);
			for(int i=0;i<n;i++)c[i]=a[i]-c[i];
			c[0]++;
			b=convolution(b,c);
		}
		poly_type exp(poly_type a)
		{
			int n=a.size();
			poly_type b(1),c;
			exp_newton_iteration(a,b,c,n);
			b.resize(n);
			return b;
		}
		poly_type power_aux(poly_type a,ll k)
		{
			int n=a.size();
			a=ln(a);
			for(int i=0;i<n;i++)a[i]*=k;
			a=exp(a);
			return a;
		}
		template<uint32_t _mod>
		int string_mod(const string& s)
		{
			ll k=0;
			for(int i=0;i<(int)s.size();i++)k=(k*10+s[i]-48)%_mod;
			return k;
		}
		poly_type power_impl(poly_type a,int k1,int k2,int k3)
		{
			int n=ssize(a);
			if(!a[0].val()&&k3>=n)return poly_type(n,0);
			int d=0;
			while(d<n&&!a[d].val())d++;
			if((ll)d*k1>n)return poly_type(n,0);
			mint inv1=a[d].inv(),inv2=a[d].pow(k2);
			for(int i=d;i<n;i++)a[i-d]=a[i]*inv1;
			for(int i=n-d;i<n;i++)a[i]=0;
			a=power_aux(a,k1);
			d*=k1;
			for(int i=n-1;i>=d;i--)a[i]=a[i-d]*inv2;
			for(int i=0;i<d&&i<n;i++)a[i]=0;
			return a;
		}
		poly_type power(const poly_type& a,ll k)
		{
			int k1=k%mod,k2=k%(mod-1),k3=k;
			return power_impl(a,k1,k2,k3);
		}
		poly_type power(const poly_type& a,const string& k)
		{
			int n=a.size();
			int k1,k2,k3=0;
			for(int i=0;i<min(8,(int)k.size());i++)k3=k3*10+k[i]-48;
			k1=string_mod<mod>(k),k2=string_mod<mod-1>(k);
			return power_impl(a,k1,k2,k3);
		}
		poly_type sqrt(poly_type a)
		{
			int n=a.size();
			mint inv2=mint(2).inv(),a0=a[0],inva0=a0.inv(),sqrta0=cipolla(a0.val(),mod);
			for(int i=0;i<n;i++)a[i]*=inva0;
			a=ln(a);
			for(int i=0;i<n;i++)a[i]*=inv2;
			a=exp(a);
			for(int i=0;i<n;i++)a[i]*=sqrta0;
			return a;
		}
		struct poly_div_t
		{
			poly_type quot,rem;
		};
		poly_div_t divide(const poly_type& a,const poly_type& b)
		{
			int n=a.size(),m=b.size();
			poly_div_t c;
			c.quot=reverse(resize(convolution(reverse(a),inverse(resize(reverse(b),n-m+1))),n-m+1));
			c.rem=resize(minus(a,convolution(b,c.quot)),m-1);
			return c;
		}
		mint bostan_mori(poly_type p,poly_type q,int n)
		{
			while(n)
			{
				poly_type _q=q;
				for(int i=1;i<ssize(q);i+=2)_q[i]=-q[i];
				p=convolution(p,_q);
				q=convolution(q,_q);
				int i=n&1;
				for(;i<ssize(p);i+=2)p[i>>1]=p[i];
				p.resize(i>>1);
				for(i=0;i<ssize(q);i+=2)q[i>>1]=q[i];
				q.resize(i>>1),n>>=1;
			}
			if(!ssize(p))return 0;
			return p[0]*q[0].inv();
		}
		pair<poly_type,poly_type> linear_recursion(const poly_type& f,const poly_type& a)
		{
			int n=ssize(a);
			poly_type q(n+1);
			q[0]=1;
			for(int i=1;i<=n;i++)q[i]=-f[i];
			poly_type p=convolution(q,a);
			p.resize(n);
			return {p,q};
		}
		poly_type product(const vector<poly_type>& v)
		{
			auto dac=[&](auto&& dac,int l,int r)->poly_type
			{
				if(l==r)return v[l-1];
				int mid=(l+r)>>1;
				return convolution(dac(dac,l,mid),dac(dac,mid+1,r));
			};
			return dac(dac,1,ssize(v));
		}
		poly_type semi_online_convolution(poly_type g)
		{
			int n=ssize(g);
			int _n=bit_ceil((uint32_t)n);
			g.resize(_n);
			poly_type f(_n);
			auto dac=[&](auto&& dac,int l,int r)->void
			{
				if(r-l<=1)return;
				int mid=(l+r)>>1;
				dac(dac,l,mid);
				poly_type c=convolution(poly_type(f.begin()+l,f.begin()+mid),poly_type(g.begin(),g.begin()+r-l));
				for(int i=mid;i<r&&i-l<ssize(c);i++)f[i]+=c[i-l];
				dac(dac,mid,r);
			};
			f[0]=1;
			dac(dac,0,_n);
			f.resize(n);
			return f;
		}
		poly_type mul_t(poly_type a,poly_type b)
		{
			int n=ssize(a),m=ssize(b);
			std::reverse(b.begin(),b.end());
			int _n=bit_ceil<uint32_t>(n);
			initialize(_n);
			a.resize(_n),b.resize(_n);
			dft(a);
			dft(b);
			for(int i=0;i<_n;i++)b[i]=a[i]*b[i];
			idft(b);
			for(int i=m-1;i<n;i++)a[i-m+1]=b[i];
			a.resize(max(0,n-m+1));
			return a;
		}
		poly_type evaluation(poly_type f,poly_type q)
		{
			int n=ssize(f),m=ssize(q);
			if(n<m)f.resize(n=m);
			else if(m<n)q.resize(n);
			vector<poly_type> tr(n<<2);
			poly_type ans(n);
			auto ls=[](int x){ return x<<1; };
			auto rs=[](int x){ return x<<1|1; };
			auto build=[&](auto&& build,int x,int l,int r)->void
			{
				if(l==r)return tr[x]={1,-q[l]},[]{}();
				int mid=(l+r)>>1;
				build(build,ls(x),l,mid);
				build(build,rs(x),mid+1,r);
				tr[x]=convolution(tr[ls(x)],tr[rs(x)]);
			};
			auto solve=[&](auto&& solve,int x,int l,int r,poly_type f)->void
			{
				if(l==r)return ans[l]=f[0],[]{}();
				int mid=(l+r)>>1;
				solve(solve,ls(x),l,mid,mul_t(f,tr[rs(x)]));
				solve(solve,rs(x),mid+1,r,mul_t(f,tr[ls(x)]));
			};
			build(build,1,0,n-1);
			f.resize(n*2);
			solve(solve,1,0,n-1,mul_t(f,inverse(tr[1])));
			ans.resize(m);
			return ans;
		}
	};
}
#endif