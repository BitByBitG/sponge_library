#ifndef SPONGE_MATRIX_HPP
#define SPONGE_MATRIX_HPP
#include<sponge/core.hpp>
#include<sponge/functors.hpp>
namespace sponge
{
	template<typename T,int _n,int _m,typename Zero=zero_fn<T>,typename One=one_fn<T>,typename Plus=plus<T>,typename Mul=multiplies<T>>
	class static_matrix
	{
	public:
		inline static constexpr int n=_n,m=_m;
		inline static constexpr Zero zero{};
		inline static constexpr One one{};
		inline static constexpr Plus plus{};
		inline static constexpr Mul mul{};
		array<array<T,m>,n> data;
		constexpr static_matrix()
		{
			for(int i=0;i<n;i++)for(int j=0;j<m;j++)data[i][j]=zero();
		}
		constexpr static_matrix(array<array<T,m>,n> a):data(a){}
		constexpr static_matrix(initializer_list<std::initializer_list<T>> il)
		{
			int i=0;
			for(auto& row:il)
			{
				int j=0;
				for(auto& val:row)
				{
					if(i>=n||j>=m)break;
					data[i][j++]=val;
				}
				i++;
			}
		}
		const T& operator()(int x,int y)const
		{
			return data[x][y];
		}
		T& operator()(int x,int y)
		{
			return data[x][y];
		}
		bool operator==(const static_matrix& b)const
		{
			for(int i=0;i<n;i++)
				for(int j=0;j<m;j++)
					if(data[i][j]!=b(i,j))
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
					data[i][j]=plus(data[i][j],b(i,j));
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
			static_matrix<T,n,p,Zero,One,Plus,Mul> c;
			for(int i=0;i<n;i++)
				for(int k=0;k<m;k++)
					for(int j=0;j<p;j++)
						c(i,j)=plus(c(i,j),mul(data[i][k],b(k,j)));
			return c;
		}
		static_matrix operator*=(const static_matrix<T,m,m,Zero,One,Plus,Mul>& b)
		{
			*this=(*this)*b;
			return *this;
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
}
#endif