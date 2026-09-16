#ifndef SPONGE_MATRIX_HPP
#define SPONGE_MATRIX_HPP
#include<sponge/core.hpp>
#include<sponge/functors.hpp>
namespace sponge
{
	template<
		typename T,int _n,int _m,
		typename Zero=zero_fn<T>,typename One=one_fn<T>,
		typename Plus=plus<T>,typename Mul=multiplies<T>
	>
	class static_matrix:
		public array<array<T,_m>,_n>
	{
	public:
		using value_type=T;
		inline static constexpr int n=_n,m=_m;
		inline static constexpr Zero zero{};
		inline static constexpr One one{};
		inline static constexpr Plus plus{};
		inline static constexpr Mul mul{};
		using B=array<array<T,m>,n>;
		using B::array;
		const T& raw(int x,int y)const
		{
			return this->at(x).at(y);
		}
		T& raw(int x,int y)
		{
			return this->at(x).at(y);
		}
		bool operator==(const static_matrix& b)const
		{
			for(int i=0;i<n;i++)
				for(int j=0;j<m;j++)
					if(raw(i,j)!=b.raw(i,j))
						return 0;
			return 1;
		}
		bool operator!=(const static_matrix& b)const
		{
			return !(operator==(b));
		}
		static_matrix& operator+=(const static_matrix& b)
		{
			for(int i=0;i<n;i++)
				for(int j=0;j<m;j++)
					raw(i,j)=plus(raw(i,j),b.raw(i,j));
			return *this;
		}
		friend static_matrix operator+(const static_matrix& a,const static_matrix& b)
		{
			auto c=a;
			c+=b;
			return c;
		}
		template<int p>
		static_matrix<T,n,p,Zero,One,Plus,Mul> operator*(const static_matrix<T,m,p,Zero,One,Plus,Mul>& b)const
		{
			static_matrix<T,n,p,Zero,One,Plus,Mul> c{};
			for(int i=0;i<n;i++)
				for(int k=0;k<m;k++)
					for(int j=0;j<p;j++)
						c.raw(i,j)=plus(c.raw(i,j),mul(raw(i,k),b.raw(k,j)));
			return c;
		}
		static_matrix& operator*=(const static_matrix<T,m,m,Zero,One,Plus,Mul>& b)
		{
			*this=(*this)*b;
			return *this;
		}
		static_matrix& operator*=(T x)
		{
			for(int i=0;i<n;i++)
				for(int j=0;j<m;j++)
					raw(i,j)=mul(raw(i,j),x);
			return *this;
		}
		friend static_matrix operator*(static_matrix x,T y)
		{
			x*=y;
			return x;
		}
		static static_matrix id()
		{
			static_matrix a{};
			for(int i=0;i<n;i++)a.raw(i,i)=one();
			return a;
		}
		static_matrix pow(ll x)const
		{
			static_matrix base=*this,prod=id();
			for(;x;x>>=1,base*=base)if(x&1)prod*=base;
			return prod;
		}
	};
	template<
		typename T,
		typename Zero=zero_fn<T>,typename One=one_fn<T>,
		typename Plus=plus<T>,typename Mul=multiplies<T>
	>
	class dynamic_matrix:
		public vector<vector<T>>
	{
	public:
		using value_type=T;
		inline static constexpr Zero zero{};
		inline static constexpr One one{};
		inline static constexpr Plus plus{};
		inline static constexpr Mul mul{};
		int n,m;
		using B=vector<vector<T>>;
		using B::vector;
		dynamic_matrix(int _n,int _m)
		{
			resize(_n,_m);
		}
		void resize(int _n,int _m)
		{
			n=_n;
			m=_m;
			this->assign(_n,vector<T>(_m));
		}
		const T& raw(int x,int y)const
		{
			return this->at(x).at(y);
		}
		T& raw(int x,int y)
		{
			return this->at(x).at(y);
		}
		bool operator==(const dynamic_matrix& b)const
		{
			for(int i=0;i<n;i++)
				for(int j=0;j<m;j++)
					if(raw(i,j)!=b.raw(i,j))
						return 0;
			return 1;
		}
		bool operator!=(const dynamic_matrix& b)const
		{
			return !(operator==(b));
		}
		dynamic_matrix& operator+=(const dynamic_matrix& b)
		{
			for(int i=0;i<n;i++)
				for(int j=0;j<m;j++)
					raw(i,j)=plus(raw(i,j),b.raw(i,j));
			return *this;
		}
		friend dynamic_matrix operator+(const dynamic_matrix& a,const dynamic_matrix& b)
		{
			auto c=a;
			c+=b;
			return c;
		}
		dynamic_matrix operator*(const dynamic_matrix& b)const
		{
			assert(m==b.n);
			int p=b.m;
			dynamic_matrix c(n,p);
			for(int i=0;i<n;i++)
				for(int k=0;k<m;k++)
					for(int j=0;j<p;j++)
						c.raw(i,j)=plus(c.raw(i,j),mul(raw(i,k),b.raw(k,j)));
			return c;
		}
		dynamic_matrix& operator*=(const dynamic_matrix& b)
		{
			*this=(*this)*b;
			return *this;
		}
		dynamic_matrix& operator*=(T x)
		{
			for(int i=0;i<n;i++)
				for(int j=0;j<m;j++)
					raw(i,j)=mul(raw(i,j),x);
			return *this;
		}
		friend dynamic_matrix operator*(dynamic_matrix x,T y)
		{
			x*=y;
			return x;
		}
		static dynamic_matrix id(int n)
		{
			dynamic_matrix a(n,n);
			for(int i=0;i<n;i++)a.raw(i,i)=one();
			return a;
		}
		dynamic_matrix pow(ll x)const
		{
			dynamic_matrix base=*this,prod=id(n);
			for(;x;x>>=1,base*=base)if(x&1)prod*=base;
			return prod;
		}
	};
	template<typename T,int n,int m>
	using s_max_plus_matrix=static_matrix<
		T,n,m,val_fn<T,-inf<T>>,
		zero_fn<T>,max_fn<T>,plus<T>
	>;
	template<typename T,int n,int m>
	using s_min_plus_matrix=static_matrix<
		T,n,m,val_fn<T,inf<T>>,
		zero_fn<T>,min_fn<T>,plus<T>
	>;
	template<typename T>
	using d_max_plus_matrix=dynamic_matrix<
		T,val_fn<T,-inf<T>>,
		zero_fn<T>,max_fn<T>,plus<T>
	>;
	template<typename T>
	using d_min_plus_matrix=dynamic_matrix<
		T,val_fn<T,inf<T>>,
		zero_fn<T>,min_fn<T>,plus<T>
	>;
	template<typename Mat>
	pair<bool,vector<typename Mat::value_type>> gauss(Mat a)
	{
		bool feasible=1;
		using T=Mat::value_type;
		int n=a.n,m=a.m;
		assert(m==n+1);
		for(int i=0;i<n;i++)
		{
			int pivot=i;
			for(int j=i+1;j<n;j++)
				if(abs(a[j][i])>abs(a[pivot][i]))
					pivot=j;
			if(abs(a[pivot][i])<eps<T>)
			{
				feasible=0;
				continue;
			}
			swap(a[i],a[pivot]);
			ld div=a[i][i];
			for(int j=i;j<n+1;j++)
				a[i][j]/=div;
			for(int j=i+1;j<n;j++)
			{
				div=a[j][i];
				for(int k=i;k<n+1;k++)
					a[j][k]-=a[i][k]*div;
			}
		}
		vector<T> x(n);
		x[n-1]=a[n-1][n];
		for(int i=n-2;i>=0;i--)
		{
			x[i]=a[i][n];
			for(int j=i+1;j<n;j++)
				x[i]-=a[i][j]*x[j];
		}
		return {feasible,x};
	}
}
#endif