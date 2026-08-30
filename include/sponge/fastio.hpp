#ifndef SPONGE_FASTIO_HPP
#define SPONGE_FASTIO_HPP
#include<sponge/core.hpp>
namespace sponge
{
	template<int ibufsiz,int obufsiz,int stksiz,int strbufsiz>
	class fastio_base
	{
		using ptr_t=char*;
		char ibuf[ibufsiz],obuf[obufsiz],stk[stksiz],strbuf[strbufsiz];
		ptr_t p1=ibuf,p2=ibuf,p3=obuf;
		int top=0;
	public:
		~fastio_base()
		{
			flush();
		}
		void flush()
		{
			fwrite(obuf,1,p3-obuf,stdout);
			p3=obuf;
		}
		INLINE char gc()
		{
			return (p1==p2&&(p2=(p1=ibuf)+fread(ibuf,1,ibufsiz,stdin),p1==p2))?EOF:*p1++;
		}
		INLINE void pc(char ch)
		{
			if(p3-obuf==obufsiz)fwrite(obuf,1,obufsiz,stdout),p3=obuf;
			*p3++=ch;
		}
		INLINE bool iss(char ch)
		{
			return ch==EOF||isspace(ch);
		}
		void getstk()
		{
			char ch;
			for(ch=gc();iss(ch);ch=gc());
			top=0;
			for(;!iss(ch);ch=gc())stk[top++]=ch;
		}
		void rputstk()
		{
			while(top)pc(stk[--top]);
		}
		void putstk()
		{
			for(int i=0;i<top;i++)pc(stk[i]);
			top=0;
		}
		inline fastio_base& operator>>(char& x)
		{
			char ch;
			for(ch=gc();iss(ch);ch=gc());
			x=ch;
			return *this;
		}
		inline fastio_base& operator>>(string& x)
		{
			char ch;
			for(ch=gc();iss(ch);ch=gc());
			x.clear();
			int p=0;
			for(;!iss(ch);ch=gc())
			{
				strbuf[p++]=ch;
				if(p==strbufsiz)
				{
					x.append(strbuf,p);
					p=0;
				}
			}
			x.append(strbuf,p);
			return *this;
		}
		string getline()
		{
			string x;
			char ch;
			for(ch=gc();ch!='\n';ch=gc())x+=ch;
			return x;
		}
		template<typename T>
		inline enable_if_t<is_integral_v<T>||is_same_v<T,__int128_t>||is_same_v<T,__uint128_t>,fastio_base&> operator>>(T& x)
		{
			char ch;
			int y=1;
			for(ch=gc();!isdigit(ch);ch=gc())if(ch=='-')y=-y;
			x=0;
			for(;isdigit(ch);ch=gc())x=x*10+ch-48;
			x*=y;
			return *this;
		}
		template<typename T>
		inline enable_if_t<is_floating_point_v<T>,fastio_base&> operator>>(T& x)
		{
			getstk();
			from_chars(stk,stk+top,x);
			return *this;
		}
		inline fastio_base& operator<<(char x)
		{
			pc(x);
			return *this;
		}
		inline fastio_base& operator<<(const string& x)
		{
			for(auto ch:x)pc(ch);
			return *this;
		}
		template<typename T>
		inline enable_if_t<is_integral_v<T>||is_same_v<T,__int128_t>||is_same_v<T,__uint128_t>,fastio_base&> operator<<(T x)
		{
			if(!x)return pc('0'),*this;
			if(x<0)pc('-'),x=-x;
			top=0;
			while(x)stk[top++]=x%10+48,x/=10;
			rputstk();
			return *this;
		}
		template<typename T>
		inline enable_if_t<is_floating_point_v<T>,fastio_base&> operator<<(const T& x)
		{
			auto[ptr,ec]=to_chars(stk,stk+64,x);
			top=ptr-stk;
			putstk();
			return *this;
		}
		struct dummy{ void sync_with_stdio(bool){} };
		dummy* tie(nullptr_t){ return new dummy; }
	};
	using fastio=fastio_base<1<<20,1<<20,1<<6,1<<10>;
	using str_fastio=fastio_base<1<<20,1<<20,1<<6,1<<20>;
}
#endif