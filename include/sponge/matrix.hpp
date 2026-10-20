#ifndef SPONGE_MATRIX_HPP
#define SPONGE_MATRIX_HPP
#include<sponge/core.hpp>
#include<sponge/functor.hpp>
#include<sponge/type_traits.hpp>
namespace sponge
{
	template<
		typename T,int _n,int _m,
		typename Zero=zero_t<T>,typename One=one_t<T>,
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
		typename Zero=zero_t<T>,typename One=one_t<T>,
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
			ASSERT(m==b.n);
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
		T,n,m,constant_t<T,-inf<T>>,
		zero_t<T>,max_t<T>,plus<T>
	>;
	template<typename T,int n,int m>
	using s_min_plus_matrix=static_matrix<
		T,n,m,constant_t<T,inf<T>>,
		zero_t<T>,min_t<T>,plus<T>
	>;
	template<typename T>
	using d_max_plus_matrix=dynamic_matrix<
		T,constant_t<T,-inf<T>>,
		zero_t<T>,max_t<T>,plus<T>
	>;
	template<typename T>
	using d_min_plus_matrix=dynamic_matrix<
		T,constant_t<T,inf<T>>,
		zero_t<T>,min_t<T>,plus<T>
	>;
	template<typename Mat>
	bool gauss(Mat& a,vector<typename Mat::value_type>* b=nullptr)
	{
		using T=typename Mat::value_type;
		using std::abs;
		int n=a.n;
		ASSERT(a.m==n);
		ASSERT(!b||int(b->size())==n);
		int r=0;
		for(int i=0;i<n;i++)
		{
			int pivot=r;
			if constexpr(std::is_floating_point_v<T>)
			{
				for(int j=r+1;j<n;j++)
					if(std::abs(a[j][i])>std::abs(a[pivot][i]))
						pivot=j;
				if(std::abs(a[pivot][i])<eps<T>)
				{
					for(int j=r;j<n;j++)a[j][i]=0;
					continue;
				}
			}
			else
			{
				while(pivot<n&&a[pivot][i]==T(0))pivot++;
				if(pivot==n)continue;
			}
			swap(a[r],a[pivot]);
			if(b)swap((*b)[r],(*b)[pivot]);
			T div=a[r][i];
			a[r][i]=1;
			for(int j=i+1;j<n;j++)a[r][j]/=div;
			if(b)(*b)[r]/=div;
			for(int j=r+1;j<n;j++)
			{
				div=a[j][i];
				a[j][i]=0;
				for(int k=i+1;k<n;k++)
					a[j][k]-=a[r][k]*div;
				if(b)(*b)[j]-=(*b)[r]*div;
			}
			r++;
		}
		return r==n;
	}
	template<typename Mat>
	pair<bool,vector<typename Mat::value_type>> gauss_solve(Mat a,vector<typename Mat::value_type> b)
	{
		if(!gauss(a,&b))return {false,{}};
		int n=a.n;
		for(int i=n-1;i>=0;i--)
			for(int j=i+1;j<n;j++)
				b[i]-=a[i][j]*b[j];
		return {true,std::move(b)};
	}
	template<typename Mat>
	typename Mat::value_type det(Mat a)
	{
		using T=typename Mat::value_type;
		using std::abs;
		int n=a.n;
		ASSERT(a.m==n);
		T ans=1;
		for(int i=0;i<n;i++)
		{
			int pivot=i;
			if constexpr(std::is_floating_point_v<T>)
			{
				for(int j=i+1;j<n;j++)
					if(std::abs(a[j][i])>std::abs(a[pivot][i]))
						pivot=j;
				if(std::abs(a[pivot][i])<eps<T>)return T(0);
			}
			else
			{
				while(pivot<n&&a[pivot][i]==T(0))pivot++;
				if(pivot==n)return T(0);
			}
			if(pivot!=i)
			{
				swap(a[i],a[pivot]);
				ans=-ans;
			}
			ans*=a[i][i];
			for(int j=i+1;j<n;j++)
			{
				T div=a[j][i]/a[i][i];
				a[j][i]=0;
				for(int k=i+1;k<n;k++)
					a[j][k]-=a[i][k]*div;
			}
		}
		return ans;
	}
}
#endif