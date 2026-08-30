#ifndef SPONGE_UTILITY_HPP
#define SPONGE_UTILITY_HPP
#include<sponge/core.hpp>
namespace sponge
{
	template<typename T>
	class discretizer:public vector<T>
	{
	public:
		using vector<T>::vector;
		void discretize()
		{
			sort(this->begin(),this->end());
			this->erase(unique(this->begin(),this->end()),this->end());
		}
		int index(const T& x)
		{
			return lower_bound(this->begin(),this->end(),x)-this->begin()+1;
		}
		T value(int x)
		{
			return this->operator[](x-1);
		}
	};
    template<typename T,int n,typename Cmp=less<T>>
    class priority_array:public array<T,n>
    {
    public:
        static constexpr Cmp cmp{};
        using array<T,n>::array;
        void insert(const T& v)
        {
            if(cmp(v,this->operator[](n-1)))
            {
                this->operator[](n-1)=v;
                int k=n-1;
                while(k>0&&cmp(this->operator[](k),this->operator[](k-1)))
                {
                    swap(this->operator[](k),this->operator[](k-1));
                    k--;
                }
            }
        }
    };
	template<typename T>
	void chkmin(T& x,const T& y)
	{
		x=min(x,y);
	}
	template<typename T>
	void chkmax(T& x,const T& y)
	{
		x=max(x,y);
	}
	template<typename FIte,typename Stream=istream>
	void read_each(FIte first,FIte last,Stream& s=cin)
	{
		for(;first!=last;++first)s>>*first;
	}
	template<typename FIte,typename Separator=char,typename Stream=ostream>
	void write_each(FIte first,FIte last,const Separator& sep=' ',Stream& s=cout)
	{
		for(;first!=last;++first)s<<*first<<sep;
	}
}
#endif