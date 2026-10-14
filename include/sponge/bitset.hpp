#ifndef SPONGE_BITSET_HPP
#define SPONGE_BITSET_HPP
#include<immintrin.h>
#include<sponge/core.hpp>
namespace sponge
{
	namespace detail
	{
		template<typename T>
		struct aligned_allocator
		{
			using value_type=T;
			using is_always_equal=std::true_type;
			aligned_allocator()=default;
			template<typename U>
			aligned_allocator(const aligned_allocator<U>&){}
			T* allocate(size_t n)
			{
				if(n>std::numeric_limits<size_t>::max()/sizeof(T))throw std::bad_array_new_length();
				return static_cast<T*>(::operator new(n*sizeof(T),std::align_val_t(32)));
			}
			void deallocate(T* p,size_t)noexcept
			{
				::operator delete(p,std::align_val_t(32));
			}
			template<typename U>
			bool operator==(const aligned_allocator<U>&)const noexcept
			{
				return true;
			}
			template<typename U>
			bool operator!=(const aligned_allocator<U>&)const noexcept
			{
				return false;
			}
		};
		struct expression_tag{};
		template<typename E>
		struct bitset_expression:expression_tag
		{
			const E& operator()()const
			{
				return static_cast<const E&>(*this);
			}
		};
	}
	class dynamic_bitset;
	template<size_t N>class static_bitset;
	namespace detail
	{
		template<typename T> struct is_bitset:std::false_type{};
		template<> struct is_bitset<dynamic_bitset>:std::true_type{};
		template<size_t N> struct is_bitset<static_bitset<N>>:std::true_type{};
		template<typename E>
		using closure=std::conditional_t<std::is_lvalue_reference_v<E>&&is_bitset<std::decay_t<E>>::value,const std::decay_t<E>&,std::decay_t<E>>;
		template<typename E>
		inline constexpr bool is_expression=std::is_base_of_v<expression_tag,std::decay_t<E>>;
		template<typename E>
		struct bitset_not:bitset_expression<bitset_not<E>>
		{
			E v;
			template<typename T,std::enable_if_t<!std::is_same_v<std::decay_t<T>,bitset_not>,int> =0>
			explicit bitset_not(T&& x):v(std::forward<T>(x)){}
			size_t size()const
			{
				return v.size();
			}
			__m256i block_at(size_t i)const
			{
				return _mm256_xor_si256(v.block_at(i),_mm256_set1_epi64x(-1));
			}
		};
		template<typename L,typename R,int op>
		struct bitset_binary:bitset_expression<bitset_binary<L,R,op>>
		{
			L l;
			R r;
			template<typename X,typename Y>
			bitset_binary(X&& x,Y&& y):l(std::forward<X>(x)),r(std::forward<Y>(y))
			{
				ASSERT(l.size()==r.size());
			}
			size_t size()const
			{
				ASSERT(l.size()==r.size());
				return l.size();
			}
			__m256i block_at(size_t i)const
			{
				auto a=l.block_at(i),b=r.block_at(i);
				if constexpr(op==0)return _mm256_and_si256(a,b);
				else if constexpr(op==1)return _mm256_or_si256(a,b);
				else if constexpr(op==2)return _mm256_xor_si256(a,b);
				else return _mm256_andnot_si256(b,a);
			}
		};
	}
	template<typename E,std::enable_if_t<detail::is_expression<E>,int> =0>
	auto operator~(E&& e)
	{
		return detail::bitset_not<detail::closure<E&&>>(std::forward<E>(e));
	}
	template<typename E,std::enable_if_t<detail::is_expression<E>,int> =0>
	auto operator!(E&& e)
	{
		return~std::forward<E>(e);
	}
	template<typename L,typename R,std::enable_if_t<detail::is_expression<L>&&detail::is_expression<R>,int> =0>
	auto operator&(L&& a,R&& b)
	{
		return detail::bitset_binary<detail::closure<L&&>,detail::closure<R&&>,0>(std::forward<L>(a),std::forward<R>(b));
	}
	template<typename L,typename R,std::enable_if_t<detail::is_expression<L>&&detail::is_expression<R>,int> =0>
	auto operator|(L&& a,R&& b)
	{
		return detail::bitset_binary<detail::closure<L&&>,detail::closure<R&&>,1>(std::forward<L>(a),std::forward<R>(b));
	}
	template<typename L,typename R,std::enable_if_t<detail::is_expression<L>&&detail::is_expression<R>,int> =0>
	auto operator^(L&& a,R&& b)
	{
		return detail::bitset_binary<detail::closure<L&&>,detail::closure<R&&>,2>(std::forward<L>(a),std::forward<R>(b));
	}
	template<typename L,typename R,std::enable_if_t<detail::is_expression<L>&&detail::is_expression<R>,int> =0>
	auto operator-(L&& a,R&& b)
	{
		return detail::bitset_binary<detail::closure<L&&>,detail::closure<R&&>,3>(std::forward<L>(a),std::forward<R>(b));
	}
	template<size_t N>
	class static_bitset:public detail::bitset_expression<static_bitset<N>>
	{
	public:
		using word=uint64_t;
		using block=__m256i;
		static constexpr size_t word_size=64,block_size=4,npos=size_t(-1);
		using detail::bitset_expression<static_bitset<N>>::operator();
	private:
		static constexpr size_t sz=N;
		static constexpr size_t wc(){return N/64+(N%64!=0);}
		static constexpr size_t bc(){return N/256+(N%256!=0);}
		alignas(32)std::array<word,bc()?bc()*block_size:block_size>d{};
		void bs(size_t i,block v)
		{
			_mm256_store_si256(reinterpret_cast<block*>(d.data()+i*4),v);
		}
		void trim()
		{
			if constexpr(N%64)d[wc()-1]&=(word(1)<<(N%64))-1;
			for(size_t i=wc();i<d.size();i++)d[i]=0;
		}
	public:
		static_bitset()=default;
		static_bitset(const static_bitset&)=default;
		static_bitset(static_bitset&&)=default;
		static_bitset& operator=(const static_bitset&)=default;
		static_bitset& operator=(static_bitset&&)=default;
		template<typename E> static_bitset(const detail::bitset_expression<E>& e)
		{
			ASSERT(N==e().size());
			for(size_t i=0;i<bc();i++)bs(i,e().block_at(i));
			trim();
		}
		template<typename E> static_bitset& operator=(const detail::bitset_expression<E>& e)
		{
			ASSERT(N==e().size());
			for(size_t i=0;i<bc();i++)bs(i,e().block_at(i));
			trim();
			return*this;
		}
		word word_at(size_t i)const{return d[i];}
		block block_at(size_t i)const
		{
			return _mm256_load_si256(reinterpret_cast<const block*>(d.data()+i*4));
		}
		void swap(static_bitset& o)noexcept{d.swap(o.d);}
		friend void swap(static_bitset& a,static_bitset& b)noexcept{a.swap(b);}
		bool operator[](size_t p)const
		{
			if(p>=sz)return false;
			return d[p/word_size]>>p%word_size&1;
		}
		bool operator()(size_t p)const
		{
			if(p>=sz)return false;
			return d[p/word_size]>>p%word_size&1;
		}
		void set(size_t p)
		{
			if(p>=sz)return;
			d[p/word_size]|=1ULL<<p%word_size;
		}
		void reset(size_t p)
		{
			if(p>=sz)return;
			d[p/word_size]&=~(1ULL<<p%word_size);
		}
		void flip(size_t p)
		{
			if(p>=sz)return;
			d[p/word_size]^=1ULL<<p%word_size;
		}
		size_t size()const
		{
			return sz;
		}
		size_t length()const
		{
			return sz;
		}
		size_t count()const
		{
			const block mk=_mm256_set1_epi8(0x0F);
			const block tab=_mm256_setr_epi8(
			0,1,1,2,1,2,2,3,1,2,2,3,2,3,3,4,
			0,1,1,2,1,2,2,3,1,2,2,3,2,3,3,4
			);
			block res=_mm256_setzero_si256();
			for(size_t i=0;i!=bc();++i)
			{
				block v=block_at(i);
				block lo=_mm256_shuffle_epi8(tab,_mm256_and_si256(v,mk));
				block hi=_mm256_shuffle_epi8(tab,_mm256_and_si256(_mm256_srli_epi16(v,4),mk));
				res=_mm256_add_epi64(res,_mm256_sad_epu8(_mm256_add_epi8(lo,hi),_mm256_setzero_si256()));
			}
			word part[block_size];
			_mm256_storeu_si256((block*)part,res);
			return size_t(part[0]+part[1]+part[2]+part[3]);
		}
		size_t popcnt()const
		{
			return count();
		}
		bool none()const
		{
			const size_t fb=sz/256;
			for(size_t i=0;i!=fb;++i)
			{
				block mk=_mm256_set1_epi8(-1),v=block_at(i);
				if(!_mm256_testz_si256(v,mk))return false;
			}
			if(size_t rem=sz%256)
			{
				word ma[block_size]={};
				for(size_t i=0;i!=block_size;++i)
				{
					if(rem>=word_size)
					{
						ma[i]=~0ULL;
						rem-=word_size;
					}
					else
					{
						ma[i]=(1ULL<<rem)-1;
						break;
					}
				}
				block mk=_mm256_loadu_si256((const block*)ma),v=block_at(fb);
				if(!_mm256_testz_si256(v,mk))return false;
			}
			return true;
		}
		bool any()const
		{
			const size_t fb=sz/256;
			for(size_t i=0;i!=fb;++i)
			{
				block mk=_mm256_set1_epi8(-1),v=block_at(i);
				if(!_mm256_testz_si256(v,mk))return true;
			}
			if(size_t rem=sz%256)
			{
				word ma[block_size]={};
				for(size_t i=0;i!=block_size;++i)
				{
					if(rem>=word_size)
					{
						ma[i]=~0ULL;
						rem-=word_size;
					}
					else
					{
						ma[i]=(1ULL<<rem)-1;
						break;
					}
				}
				block mk=_mm256_loadu_si256((const block*)ma),v=block_at(fb);
				if(!_mm256_testz_si256(v,mk))return true;
			}
			return false;
		}
		bool all()const
		{
			const size_t fb=sz/256;
			for(size_t i=0;i!=fb;++i)
			{
				block mk=_mm256_set1_epi8(-1),v=block_at(i);
				if(!_mm256_testc_si256(v,mk))return false;
			}
			if(size_t rem=sz%256)
			{
				word ma[block_size]={};
				for(size_t i=0;i!=block_size;++i)
				{
					if(rem>=word_size)
					{
						ma[i]=~0ULL;
						rem-=word_size;
					}
					else
					{
						ma[i]=(1ULL<<rem)-1;
						break;
					}
				}
				block mk=_mm256_loadu_si256((const block*)ma),v=block_at(fb);
				if(!_mm256_testc_si256(v,mk))return false;
			}
			return true;
		}
		void set()
		{
			const block v=_mm256_set1_epi64x(-1);
			for(size_t i=0;i!=bc();++i)
				bs(i,v);
			trim();
		}
		void reset()
		{
			const block v=_mm256_setzero_si256();
			for(size_t i=0;i!=bc();++i)
				bs(i,v);
			trim();
		}
		void flip()
		{
			const block v=_mm256_set1_epi64x(-1);
			for(size_t i=0;i!=bc();++i)
				bs(i,_mm256_xor_si256(block_at(i),v));
			trim();
		}
		template<typename E>
		static_bitset& operator&=(const detail::bitset_expression<E>& e)
		{
			ASSERT(sz==e().size());
			for(size_t i=0;i!=bc();++i)
				bs(i,_mm256_and_si256(block_at(i),e().block_at(i)));
			trim();
			return*this;
		}
		template<typename E>
		static_bitset& operator|=(const detail::bitset_expression<E>& e)
		{
			ASSERT(sz==e().size());
			for(size_t i=0;i!=bc();++i)
				bs(i,_mm256_or_si256(block_at(i),e().block_at(i)));
			trim();
			return*this;
		}
		template<typename E>
		static_bitset& operator^=(const detail::bitset_expression<E>& e)
		{
			ASSERT(sz==e().size());
			for(size_t i=0;i!=bc();++i)
				bs(i,_mm256_xor_si256(block_at(i),e().block_at(i)));
			trim();
			return*this;
		}
		template<typename E>
		static_bitset& operator-=(const detail::bitset_expression<E>& e)
		{
			ASSERT(sz==e().size());
			for(size_t i=0;i!=bc();++i)
				bs(i,_mm256_andnot_si256(e().block_at(i),block_at(i)));
			trim();
			return*this;
		}
		friend bool operator==(const static_bitset& x,const static_bitset& y)
		{
			if(x.size()!=y.size())return false;
			for(size_t i=0;i!=x.bc();++i)
			{
				block bxa=_mm256_xor_si256(y.block_at(i),x.block_at(i));
				if(!_mm256_testz_si256(bxa,bxa))return false;
			}
			return true;
		}
		friend bool operator!=(const static_bitset& x,const static_bitset& y)
		{
			if(x.size()!=y.size())return true;
			for(size_t i=0;i!=x.bc();++i)
			{
				block bxa=_mm256_xor_si256(y.block_at(i),x.block_at(i));
				if(!_mm256_testz_si256(bxa,bxa))return true;
			}
			return false;
		}
		friend bool operator<=(const static_bitset& x,const static_bitset& y)
		{
			ASSERT(x.size()==y.size());
			for(size_t i=0;i!=x.bc();++i)
				if(!_mm256_testc_si256(y.block_at(i),x.block_at(i)))
				return false;
			return true;
		}
		friend bool operator>=(const static_bitset& x,const static_bitset& y)
		{
			ASSERT(x.size()==y.size());
			for(size_t i=0;i!=x.bc();++i)
				if(!_mm256_testc_si256(x.block_at(i),y.block_at(i)))
				return false;
			return true;
		}
		friend bool operator<(const static_bitset& x,const static_bitset& y)
		{
			ASSERT(x.size()==y.size());
			bool dif=false;
			for(size_t i=0;i!=x.bc();++i)
			{
				block a=x.block_at(i),b=y.block_at(i);
				if(!_mm256_testc_si256(b,a))return false;
				dif|=!_mm256_testc_si256(a,b);
			}
			return dif;
		}
		friend bool operator>(const static_bitset& x,const static_bitset& y)
		{
			ASSERT(x.size()==y.size());
			bool dif=false;
			for(size_t i=0;i!=x.bc();++i)
			{
				block a=x.block_at(i),b=y.block_at(i);
				if(!_mm256_testc_si256(a,b))return false;
				dif|=!_mm256_testc_si256(b,a);
			}
			return dif;
		}
		size_t find_first_set(size_t p=0)const
		{
			if(p>=sz)return size_t(-1);
			size_t wi=p/word_size,bi=p%word_size;
			size_t bj=wi/block_size,wj=wi%block_size;
			if(word v=d[wi]&~((1ULL<<bi)-1))
			{
				size_t res=wi*word_size+size_t(__builtin_ctzll(v));
				return res<sz?res:size_t(-1);
			}
			while(++wj!=block_size)
			{
				size_t cur=bj*block_size+wj;
				if(cur>=wc())break;
				if(word v=d[cur])
				{
					size_t res=cur*word_size+__builtin_ctzll(v);
					return res<sz?res:size_t(-1);
				}
			}
			while(++bj!=bc())
			{
				block mk=_mm256_set1_epi64x(-1),v=block_at(bj);
				if(_mm256_testz_si256(v,mk))continue;
				for(size_t i=0;i!=block_size;++i)
				{
					size_t cur=bj*block_size+i;
					if(cur>=wc())break;
					if(word v=d[cur])
					{
						size_t res=cur*word_size+__builtin_ctzll(v);
						return res<sz?res:size_t(-1);
					}
				}
			}
			return size_t(-1);
		}
		size_t find_first_unset(size_t p=0)const
		{
			if(p>=sz)return size_t(-1);
			size_t wi=p/word_size,bi=p%word_size;
			size_t bj=wi/block_size,wj=wi%block_size;
			if(word v=~d[wi]&~((1ULL<<bi)-1))
			{
				size_t res=wi*word_size+size_t(__builtin_ctzll(v));
				return res<sz?res:size_t(-1);
			}
			while(++wj!=block_size)
			{
				size_t cur=bj*block_size+wj;
				if(cur>=wc())break;
				if(word v=~d[cur])
				{
					size_t res=cur*word_size+__builtin_ctzll(v);
					return res<sz?res:size_t(-1);
				}
			}
			while(++bj!=bc())
			{
				block mk=_mm256_set1_epi64x(-1),v=block_at(bj);
				if(_mm256_testc_si256(v,mk))continue;
				for(size_t i=0;i!=block_size;++i)
				{
					size_t cur=bj*block_size+i;
					if(cur>=wc())break;
					if(word v=~d[cur])
					{
						size_t res=cur*word_size+__builtin_ctzll(v);
						return res<sz?res:size_t(-1);
					}
				}
			}
			return size_t(-1);
		}
		void set(size_t p,size_t len)
		{
			if(p>sz||len>sz-p||!len)return;
			size_t wi=p/word_size,bi=p%word_size;
			if(bi)
			{
				size_t hl=word_size-bi<len?word_size-bi:len;
				d[wi++]|=((1ULL<<hl)-1)<<bi;
				len-=hl;
			}
			for(const block v=_mm256_set1_epi64x(-1);len>=256;)
			{
				_mm256_storeu_si256((block*)&d[wi],v);
				len-=256;
				wi+=block_size;
			}
			while(len>=word_size)
			{
				d[wi++]=-1ULL;
				len-=word_size;
			}
			if(len)d[wi]|=(1ULL<<len)-1;
		}
		void reset(size_t p,size_t len)
		{
			if(p>sz||len>sz-p||!len)return;
			size_t wi=p/word_size,bi=p%word_size;
			if(bi)
			{
				size_t hl=word_size-bi<len?word_size-bi:len;
				d[wi++]&=~(((1ULL<<hl)-1)<<bi);
				len-=hl;
			}
			for(const block v=_mm256_setzero_si256();len>=256;)
			{
				_mm256_storeu_si256((block*)&d[wi],v);
				len-=256;
				wi+=block_size;
			}
			while(len>=word_size)
			{
				d[wi++]=0ULL;
				len-=word_size;
			}
			if(len)d[wi]&=~((1ULL<<len)-1);
		}
		void flip(size_t p,size_t len)
		{
			if(p>sz||len>sz-p||!len)return;
			size_t wi=p/word_size,bi=p%word_size;
			if(bi)
			{
				size_t hl=word_size-bi<len?word_size-bi:len;
				d[wi++]^=((1ULL<<hl)-1)<<bi;
				len-=hl;
			}
			for(const block v=_mm256_set1_epi64x(-1);len>=256;)
			{
				_mm256_storeu_si256((block*)&d[wi],_mm256_xor_si256(_mm256_loadu_si256((const block*)&d[wi]),v));
				len-=256;
				wi+=block_size;
			}
			while(len>=word_size)
			{
				d[wi++]^=-1ULL;
				len-=word_size;
			}
			if(len)d[wi]^=(1ULL<<len)-1;
		}
		static_bitset& operator<<=(size_t s)
		{
			if(s>=sz)return reset(),*this;
			size_t ws=s/word_size,bt=s%word_size;
			if(!bt)
			{
				memmove(d.data()+ws,d.data(),(wc()-ws)*sizeof(word));
				memset(d.data(),0,ws*sizeof(word));
			}
			else
			{
				size_t rem=wc()-ws-1;
				word* dst=d.data()+wc(),* src=dst-ws;
				__m128i ls=_mm_cvtsi32_si128(int(bt)),rs=_mm_cvtsi32_si128(int(word_size-bt));
				while(rem>=block_size)
				{
					dst-=block_size,src-=block_size;
					block lo=_mm256_srl_epi64(_mm256_loadu_si256((const block*)&src[-1]),rs);
					block hi=_mm256_sll_epi64(_mm256_loadu_si256((const block*)&src[0]),ls);
					_mm256_storeu_si256((block*)dst,_mm256_or_si256(lo,hi));
					rem-=block_size;
				}
				while(rem)
				{
					--dst,--src;
					*dst=(src[0]<<bt)|(src[-1]>>(word_size-bt));
					--rem;
				}
				*--dst=*--src<<bt;
				memset(d.data(),0,ws*sizeof(word));
			}
			trim();
			return*this;
		}
		static_bitset& operator>>=(size_t s)
		{
			if(s>=sz)return reset(),*this;
			size_t ws=s/word_size,bt=s%word_size;
			if(!bt)
			{
				memmove(d.data(),d.data()+ws,(wc()-ws)*sizeof(word));
				memset(d.data()+wc()-ws,0,ws*sizeof(word));
			}
			else
			{
				size_t rem=wc()-ws-1;
				word* dst=d.data(),* src=d.data()+ws;
				__m128i rs=_mm_cvtsi32_si128(int(bt)),ls=_mm_cvtsi32_si128(int(word_size-bt));
				while(rem>=block_size)
				{
					block lo=_mm256_srl_epi64(_mm256_loadu_si256((const block*)&src[0]),rs);
					block hi=_mm256_sll_epi64(_mm256_loadu_si256((const block*)&src[1]),ls);
					_mm256_storeu_si256((block*)dst,_mm256_or_si256(lo,hi));
					dst+=block_size,src+=block_size;
					rem-=block_size;
				}
				while(rem)
				{
					*dst=(src[0]>>bt)|(src[1]<<(word_size-bt));
					++dst,++src;
					--rem;
				}
				*dst=*src>>bt;
				memset(d.data()+wc()-ws,0,ws*sizeof(word));
			}
			trim();
			return*this;
		}
		static_bitset rotl(int s)const
		{
			if(!sz)return*this;
			size_t k=size_t(s<0?-int64_t(s):s)%sz;
			if(!k)return*this;
			if(s<0)k=sz-k;
			return(*this<<k)|(*this>>(sz-k));
		}
		static_bitset rotr(int s)const
		{
			if(!sz)return*this;
			size_t k=size_t(s<0?-int64_t(s):s)%sz;
			if(!k)return*this;
			if(s<0)k=sz-k;
			return(*this>>k)|(*this<<(sz-k));
		}
		static_bitset operator<<(size_t s)const
		{
			static_bitset res;
			if(s>=sz)return res;
			size_t ws=s/word_size,bt=s%word_size;
			if(!bt)
			{
				memcpy(res.d.data()+ws,d.data(),(wc()-ws)*sizeof(word));
			}
			else
			{
				size_t rem=wc()-ws-1;
				word* dst=res.d.data()+wc();
				const word* src=d.data()+wc()-ws;
				__m128i ls=_mm_cvtsi32_si128(int(bt)),rs=_mm_cvtsi32_si128(int(word_size-bt));
				while(rem>=block_size)
				{
					dst-=block_size,src-=block_size;
					block lo=_mm256_srl_epi64(_mm256_loadu_si256((const block*)&src[-1]),rs);
					block hi=_mm256_sll_epi64(_mm256_loadu_si256((const block*)&src[0]),ls);
					_mm256_storeu_si256((block*)dst,_mm256_or_si256(lo,hi));
					rem-=block_size;
				}
				while(rem)
				{
					--dst,--src;
					*dst=(src[0]<<bt)|(src[-1]>>(word_size-bt));
					--rem;
				}
				*--dst=*--src<<bt;
			}
			res.trim();
			return res;
		}
		static_bitset operator>>(size_t s)const
		{
			static_bitset res;
			if(s>=sz)return res;
			size_t ws=s/word_size,bt=s%word_size;
			if(!bt)
			{
				memcpy(res.d.data(),d.data()+ws,(wc()-ws)*sizeof(word));
			}
			else
			{
				size_t rem=wc()-ws-1;
				word* dst=res.d.data();
				const word* src=d.data()+ws;
				__m128i rs=_mm_cvtsi32_si128(int(bt)),ls=_mm_cvtsi32_si128(int(word_size-bt));
				while(rem>=block_size)
				{
					block lo=_mm256_srl_epi64(_mm256_loadu_si256((const block*)&src[0]),rs);
					block hi=_mm256_sll_epi64(_mm256_loadu_si256((const block*)&src[1]),ls);
					_mm256_storeu_si256((block*)dst,_mm256_or_si256(lo,hi));
					dst+=block_size,src+=block_size;
					rem-=block_size;
				}
				while(rem)
				{
					*dst=(src[0]>>bt)|(src[1]<<(word_size-bt));
					++dst,++src;
					--rem;
				}
				*dst=*src>>bt;
			}
			res.trim();
			return res;
		}
	};
	class dynamic_bitset:public detail::bitset_expression<dynamic_bitset>
	{
	public:
		using word=uint64_t;
		using block=__m256i;
		static constexpr size_t word_size=64,block_size=4,npos=size_t(-1);
		using detail::bitset_expression<dynamic_bitset>::operator();
	private:
		std::vector<word,detail::aligned_allocator<word>>d;
		size_t sz=0;
		static size_t ss(size_t n)
		{
			if(n>std::numeric_limits<size_t>::max()-255)throw std::length_error("dynamic_bitset");
			return((n+255)/256)*4;
		}
		size_t wc()const
		{
			return sz/64+(sz%64!=0);
		}
		size_t bc()const
		{
			return d.size()/4;
		}
		void bs(size_t i,block v)
		{
			_mm256_store_si256(reinterpret_cast<block*>(d.data()+i*4),v);
		}
		void trim()
		{
			size_t n=wc();
			if(sz%64)d[n-1]&=(word(1)<<(sz%64))-1;
			for(size_t i=n;i<d.size();i++)d[i]=0;
		}
	public:
		dynamic_bitset()=default;
		explicit dynamic_bitset(size_t n,bool v=false):d(ss(n),v?~word(0):0),sz(n)
		{
			trim();
		}
		dynamic_bitset(const dynamic_bitset&)=default;
		dynamic_bitset(dynamic_bitset&& o)noexcept:d(std::move(o.d)),sz(std::exchange(o.sz,0))
		{
			o.d.clear();
		}
		dynamic_bitset& operator=(const dynamic_bitset& o)
		{
			if(this!=&o)
			{
				d=o.d;
				sz=o.sz;
			}
			return*this;
		}
		dynamic_bitset& operator=(dynamic_bitset&& o)noexcept
		{
			if(this!=&o)
			{
				d=std::move(o.d);
				sz=std::exchange(o.sz,0);
				o.d.clear();
			}
			return*this;
		}
		template<typename E> dynamic_bitset(const detail::bitset_expression<E>& e):dynamic_bitset(e().size())
		{
			for(size_t i=0;i<bc();i++)bs(i,e().block_at(i));
			trim();
		}
		template<typename E> dynamic_bitset& operator=(const detail::bitset_expression<E>& e)
		{
			if(sz!=e().size())
			{
				dynamic_bitset tmp(e);
				swap(tmp);
				return*this;
			}
			for(size_t i=0;i<bc();i++)bs(i,e().block_at(i));
			trim();
			return*this;
		}
		word word_at(size_t i)const
		{
			return d[i];
		}
		block block_at(size_t i)const
		{
			return _mm256_load_si256(reinterpret_cast<const block*>(d.data()+i*4));
		}
		void resize(size_t n,bool v=false)
		{
			size_t old=sz;
			d.resize(ss(n),0);
			sz=n;
			if(n>old&&v)set(old,n-old);
			trim();
		}
		void reserve(size_t n)
		{
			d.reserve(ss(n));
		}
		size_t capacity()const
		{
			return d.capacity()*64;
		}
		void shrink_to_fit()
		{
			d.shrink_to_fit();
		}
		void clear()noexcept
		{
			d.clear();
			sz=0;
		}
		void swap(dynamic_bitset& o)noexcept
		{
			d.swap(o.d);
			std::swap(sz,o.sz);
		}
		friend void swap(dynamic_bitset& a,dynamic_bitset& b)noexcept
		{
			a.swap(b);
		}
		bool operator[](size_t p)const
		{
			if(p>=sz)return false;
			return d[p/word_size]>>p%word_size&1;
		}
		bool operator()(size_t p)const
		{
			if(p>=sz)return false;
			return d[p/word_size]>>p%word_size&1;
		}
		void set(size_t p)
		{
			if(p>=sz)return;
			d[p/word_size]|=1ULL<<p%word_size;
		}
		void reset(size_t p)
		{
			if(p>=sz)return;
			d[p/word_size]&=~(1ULL<<p%word_size);
		}
		void flip(size_t p)
		{
			if(p>=sz)return;
			d[p/word_size]^=1ULL<<p%word_size;
		}
		size_t size()const
		{
			return sz;
		}
		size_t length()const
		{
			return sz;
		}
		size_t count()const
		{
			const block mk=_mm256_set1_epi8(0x0F);
			const block tab=_mm256_setr_epi8(
			0,1,1,2,1,2,2,3,1,2,2,3,2,3,3,4,
			0,1,1,2,1,2,2,3,1,2,2,3,2,3,3,4
			);
			block res=_mm256_setzero_si256();
			for(size_t i=0;i!=bc();++i)
			{
				block v=block_at(i);
				block lo=_mm256_shuffle_epi8(tab,_mm256_and_si256(v,mk));
				block hi=_mm256_shuffle_epi8(tab,_mm256_and_si256(_mm256_srli_epi16(v,4),mk));
				res=_mm256_add_epi64(res,_mm256_sad_epu8(_mm256_add_epi8(lo,hi),_mm256_setzero_si256()));
			}
			word part[block_size];
			_mm256_storeu_si256((block*)part,res);
			return size_t(part[0]+part[1]+part[2]+part[3]);
		}
		size_t popcnt()const
		{
			return count();
		}
		bool none()const
		{
			const size_t fb=sz/256;
			for(size_t i=0;i!=fb;++i)
			{
				block mk=_mm256_set1_epi8(-1),v=block_at(i);
				if(!_mm256_testz_si256(v,mk))return false;
			}
			if(size_t rem=sz%256)
			{
				word ma[block_size]={};
				for(size_t i=0;i!=block_size;++i)
				{
					if(rem>=word_size)
					{
						ma[i]=~0ULL;
						rem-=word_size;
					}
					else
					{
						ma[i]=(1ULL<<rem)-1;
						break;
					}
				}
				block mk=_mm256_loadu_si256((const block*)ma),v=block_at(fb);
				if(!_mm256_testz_si256(v,mk))return false;
			}
			return true;
		}
		bool any()const
		{
			const size_t fb=sz/256;
			for(size_t i=0;i!=fb;++i)
			{
				block mk=_mm256_set1_epi8(-1),v=block_at(i);
				if(!_mm256_testz_si256(v,mk))return true;
			}
			if(size_t rem=sz%256)
			{
				word ma[block_size]={};
				for(size_t i=0;i!=block_size;++i)
				{
					if(rem>=word_size)
					{
						ma[i]=~0ULL;
						rem-=word_size;
					}
					else
					{
						ma[i]=(1ULL<<rem)-1;
						break;
					}
				}
				block mk=_mm256_loadu_si256((const block*)ma),v=block_at(fb);
				if(!_mm256_testz_si256(v,mk))return true;
			}
			return false;
		}
		bool all()const
		{
			const size_t fb=sz/256;
			for(size_t i=0;i!=fb;++i)
			{
				block mk=_mm256_set1_epi8(-1),v=block_at(i);
				if(!_mm256_testc_si256(v,mk))return false;
			}
			if(size_t rem=sz%256)
			{
				word ma[block_size]={};
				for(size_t i=0;i!=block_size;++i)
				{
					if(rem>=word_size)
					{
						ma[i]=~0ULL;
						rem-=word_size;
					}
					else
					{
						ma[i]=(1ULL<<rem)-1;
						break;
					}
				}
				block mk=_mm256_loadu_si256((const block*)ma),v=block_at(fb);
				if(!_mm256_testc_si256(v,mk))return false;
			}
			return true;
		}
		void set()
		{
			const block v=_mm256_set1_epi64x(-1);
			for(size_t i=0;i!=bc();++i)
				bs(i,v);
			trim();
		}
		void reset()
		{
			const block v=_mm256_setzero_si256();
			for(size_t i=0;i!=bc();++i)
				bs(i,v);
			trim();
		}
		void flip()
		{
			const block v=_mm256_set1_epi64x(-1);
			for(size_t i=0;i!=bc();++i)
				bs(i,_mm256_xor_si256(block_at(i),v));
			trim();
		}
		template<typename E>
		dynamic_bitset& operator&=(const detail::bitset_expression<E>& e)
		{
			ASSERT(sz==e().size());
			for(size_t i=0;i!=bc();++i)
				bs(i,_mm256_and_si256(block_at(i),e().block_at(i)));
			trim();
			return*this;
		}
		template<typename E>
		dynamic_bitset& operator|=(const detail::bitset_expression<E>& e)
		{
			ASSERT(sz==e().size());
			for(size_t i=0;i!=bc();++i)
				bs(i,_mm256_or_si256(block_at(i),e().block_at(i)));
			trim();
			return*this;
		}
		template<typename E>
		dynamic_bitset& operator^=(const detail::bitset_expression<E>& e)
		{
			ASSERT(sz==e().size());
			for(size_t i=0;i!=bc();++i)
				bs(i,_mm256_xor_si256(block_at(i),e().block_at(i)));
			trim();
			return*this;
		}
		template<typename E>
		dynamic_bitset& operator-=(const detail::bitset_expression<E>& e)
		{
			ASSERT(sz==e().size());
			for(size_t i=0;i!=bc();++i)
				bs(i,_mm256_andnot_si256(e().block_at(i),block_at(i)));
			trim();
			return*this;
		}
		friend bool operator==(const dynamic_bitset& x,const dynamic_bitset& y)
		{
			if(x.size()!=y.size())return false;
			for(size_t i=0;i!=x.bc();++i)
			{
				block bxa=_mm256_xor_si256(y.block_at(i),x.block_at(i));
				if(!_mm256_testz_si256(bxa,bxa))return false;
			}
			return true;
		}
		friend bool operator!=(const dynamic_bitset& x,const dynamic_bitset& y)
		{
			if(x.size()!=y.size())return true;
			for(size_t i=0;i!=x.bc();++i)
			{
				block bxa=_mm256_xor_si256(y.block_at(i),x.block_at(i));
				if(!_mm256_testz_si256(bxa,bxa))return true;
			}
			return false;
		}
		friend bool operator<=(const dynamic_bitset& x,const dynamic_bitset& y)
		{
			ASSERT(x.size()==y.size());
			for(size_t i=0;i!=x.bc();++i)
				if(!_mm256_testc_si256(y.block_at(i),x.block_at(i)))
				return false;
			return true;
		}
		friend bool operator>=(const dynamic_bitset& x,const dynamic_bitset& y)
		{
			ASSERT(x.size()==y.size());
			for(size_t i=0;i!=x.bc();++i)
				if(!_mm256_testc_si256(x.block_at(i),y.block_at(i)))
				return false;
			return true;
		}
		friend bool operator<(const dynamic_bitset& x,const dynamic_bitset& y)
		{
			ASSERT(x.size()==y.size());
			bool dif=false;
			for(size_t i=0;i!=x.bc();++i)
			{
				block a=x.block_at(i),b=y.block_at(i);
				if(!_mm256_testc_si256(b,a))return false;
				dif|=!_mm256_testc_si256(a,b);
			}
			return dif;
		}
		friend bool operator>(const dynamic_bitset& x,const dynamic_bitset& y)
		{
			ASSERT(x.size()==y.size());
			bool dif=false;
			for(size_t i=0;i!=x.bc();++i)
			{
				block a=x.block_at(i),b=y.block_at(i);
				if(!_mm256_testc_si256(a,b))return false;
				dif|=!_mm256_testc_si256(b,a);
			}
			return dif;
		}
		size_t find_first_set(size_t p=0)const
		{
			if(p>=sz)return size_t(-1);
			size_t wi=p/word_size,bi=p%word_size;
			size_t bj=wi/block_size,wj=wi%block_size;
			if(word v=d[wi]&~((1ULL<<bi)-1))
			{
				size_t res=wi*word_size+size_t(__builtin_ctzll(v));
				return res<sz?res:size_t(-1);
			}
			while(++wj!=block_size)
			{
				size_t cur=bj*block_size+wj;
				if(cur>=wc())break;
				if(word v=d[cur])
				{
					size_t res=cur*word_size+__builtin_ctzll(v);
					return res<sz?res:size_t(-1);
				}
			}
			while(++bj!=bc())
			{
				block mk=_mm256_set1_epi64x(-1),v=block_at(bj);
				if(_mm256_testz_si256(v,mk))continue;
				for(size_t i=0;i!=block_size;++i)
				{
					size_t cur=bj*block_size+i;
					if(cur>=wc())break;
					if(word v=d[cur])
					{
						size_t res=cur*word_size+__builtin_ctzll(v);
						return res<sz?res:size_t(-1);
					}
				}
			}
			return size_t(-1);
		}
		size_t find_first_unset(size_t p=0)const
		{
			if(p>=sz)return size_t(-1);
			size_t wi=p/word_size,bi=p%word_size;
			size_t bj=wi/block_size,wj=wi%block_size;
			if(word v=~d[wi]&~((1ULL<<bi)-1))
			{
				size_t res=wi*word_size+size_t(__builtin_ctzll(v));
				return res<sz?res:size_t(-1);
			}
			while(++wj!=block_size)
			{
				size_t cur=bj*block_size+wj;
				if(cur>=wc())break;
				if(word v=~d[cur])
				{
					size_t res=cur*word_size+__builtin_ctzll(v);
					return res<sz?res:size_t(-1);
				}
			}
			while(++bj!=bc())
			{
				block mk=_mm256_set1_epi64x(-1),v=block_at(bj);
				if(_mm256_testc_si256(v,mk))continue;
				for(size_t i=0;i!=block_size;++i)
				{
					size_t cur=bj*block_size+i;
					if(cur>=wc())break;
					if(word v=~d[cur])
					{
						size_t res=cur*word_size+__builtin_ctzll(v);
						return res<sz?res:size_t(-1);
					}
				}
			}
			return size_t(-1);
		}
		void set(size_t p,size_t len)
		{
			if(p>sz||len>sz-p||!len)return;
			size_t wi=p/word_size,bi=p%word_size;
			if(bi)
			{
				size_t hl=word_size-bi<len?word_size-bi:len;
				d[wi++]|=((1ULL<<hl)-1)<<bi;
				len-=hl;
			}
			for(const block v=_mm256_set1_epi64x(-1);len>=256;)
			{
				_mm256_storeu_si256((block*)&d[wi],v);
				len-=256;
				wi+=block_size;
			}
			while(len>=word_size)
			{
				d[wi++]=-1ULL;
				len-=word_size;
			}
			if(len)d[wi]|=(1ULL<<len)-1;
		}
		void reset(size_t p,size_t len)
		{
			if(p>sz||len>sz-p||!len)return;
			size_t wi=p/word_size,bi=p%word_size;
			if(bi)
			{
				size_t hl=word_size-bi<len?word_size-bi:len;
				d[wi++]&=~(((1ULL<<hl)-1)<<bi);
				len-=hl;
			}
			for(const block v=_mm256_setzero_si256();len>=256;)
			{
				_mm256_storeu_si256((block*)&d[wi],v);
				len-=256;
				wi+=block_size;
			}
			while(len>=word_size)
			{
				d[wi++]=0ULL;
				len-=word_size;
			}
			if(len)d[wi]&=~((1ULL<<len)-1);
		}
		void flip(size_t p,size_t len)
		{
			if(p>sz||len>sz-p||!len)return;
			size_t wi=p/word_size,bi=p%word_size;
			if(bi)
			{
				size_t hl=word_size-bi<len?word_size-bi:len;
				d[wi++]^=((1ULL<<hl)-1)<<bi;
				len-=hl;
			}
			for(const block v=_mm256_set1_epi64x(-1);len>=256;)
			{
				_mm256_storeu_si256((block*)&d[wi],_mm256_xor_si256(_mm256_loadu_si256((const block*)&d[wi]),v));
				len-=256;
				wi+=block_size;
			}
			while(len>=word_size)
			{
				d[wi++]^=-1ULL;
				len-=word_size;
			}
			if(len)d[wi]^=(1ULL<<len)-1;
		}
		dynamic_bitset& operator<<=(size_t s)
		{
			if(s>=sz)return reset(),*this;
			size_t ws=s/word_size,bt=s%word_size;
			if(!bt)
			{
				memmove(d.data()+ws,d.data(),(wc()-ws)*sizeof(word));
				memset(d.data(),0,ws*sizeof(word));
			}
			else
			{
				size_t rem=wc()-ws-1;
				word* dst=d.data()+wc(),* src=dst-ws;
				__m128i ls=_mm_cvtsi32_si128(int(bt)),rs=_mm_cvtsi32_si128(int(word_size-bt));
				while(rem>=block_size)
				{
					dst-=block_size,src-=block_size;
					block lo=_mm256_srl_epi64(_mm256_loadu_si256((const block*)&src[-1]),rs);
					block hi=_mm256_sll_epi64(_mm256_loadu_si256((const block*)&src[0]),ls);
					_mm256_storeu_si256((block*)dst,_mm256_or_si256(lo,hi));
					rem-=block_size;
				}
				while(rem)
				{
					--dst,--src;
					*dst=(src[0]<<bt)|(src[-1]>>(word_size-bt));
					--rem;
				}
				*--dst=*--src<<bt;
				memset(d.data(),0,ws*sizeof(word));
			}
			trim();
			return*this;
		}
		dynamic_bitset& operator>>=(size_t s)
		{
			if(s>=sz)return reset(),*this;
			size_t ws=s/word_size,bt=s%word_size;
			if(!bt)
			{
				memmove(d.data(),d.data()+ws,(wc()-ws)*sizeof(word));
				memset(d.data()+wc()-ws,0,ws*sizeof(word));
			}
			else
			{
				size_t rem=wc()-ws-1;
				word* dst=d.data(),* src=d.data()+ws;
				__m128i rs=_mm_cvtsi32_si128(int(bt)),ls=_mm_cvtsi32_si128(int(word_size-bt));
				while(rem>=block_size)
				{
					block lo=_mm256_srl_epi64(_mm256_loadu_si256((const block*)&src[0]),rs);
					block hi=_mm256_sll_epi64(_mm256_loadu_si256((const block*)&src[1]),ls);
					_mm256_storeu_si256((block*)dst,_mm256_or_si256(lo,hi));
					dst+=block_size,src+=block_size;
					rem-=block_size;
				}
				while(rem)
				{
					*dst=(src[0]>>bt)|(src[1]<<(word_size-bt));
					++dst,++src;
					--rem;
				}
				*dst=*src>>bt;
				memset(d.data()+wc()-ws,0,ws*sizeof(word));
			}
			trim();
			return*this;
		}
		dynamic_bitset rotl(int s)const
		{
			if(!sz)return*this;
			size_t k=size_t(s<0?-int64_t(s):s)%sz;
			if(!k)return*this;
			if(s<0)k=sz-k;
			return(*this<<k)|(*this>>(sz-k));
		}
		dynamic_bitset rotr(int s)const
		{
			if(!sz)return*this;
			size_t k=size_t(s<0?-int64_t(s):s)%sz;
			if(!k)return*this;
			if(s<0)k=sz-k;
			return(*this>>k)|(*this<<(sz-k));
		}
		dynamic_bitset operator<<(size_t s)const
		{
			dynamic_bitset res(sz);
			if(s>=sz)return res;
			size_t ws=s/word_size,bt=s%word_size;
			if(!bt)
			{
				memcpy(res.d.data()+ws,d.data(),(wc()-ws)*sizeof(word));
			}
			else
			{
				size_t rem=wc()-ws-1;
				word* dst=res.d.data()+wc();
				const word* src=d.data()+wc()-ws;
				__m128i ls=_mm_cvtsi32_si128(int(bt)),rs=_mm_cvtsi32_si128(int(word_size-bt));
				while(rem>=block_size)
				{
					dst-=block_size,src-=block_size;
					block lo=_mm256_srl_epi64(_mm256_loadu_si256((const block*)&src[-1]),rs);
					block hi=_mm256_sll_epi64(_mm256_loadu_si256((const block*)&src[0]),ls);
					_mm256_storeu_si256((block*)dst,_mm256_or_si256(lo,hi));
					rem-=block_size;
				}
				while(rem)
				{
					--dst,--src;
					*dst=(src[0]<<bt)|(src[-1]>>(word_size-bt));
					--rem;
				}
				*--dst=*--src<<bt;
			}
			res.trim();
			return res;
		}
		dynamic_bitset operator>>(size_t s)const
		{
			dynamic_bitset res(sz);
			if(s>=sz)return res;
			size_t ws=s/word_size,bt=s%word_size;
			if(!bt)
			{
				memcpy(res.d.data(),d.data()+ws,(wc()-ws)*sizeof(word));
			}
			else
			{
				size_t rem=wc()-ws-1;
				word* dst=res.d.data();
				const word* src=d.data()+ws;
				__m128i rs=_mm_cvtsi32_si128(int(bt)),ls=_mm_cvtsi32_si128(int(word_size-bt));
				while(rem>=block_size)
				{
					block lo=_mm256_srl_epi64(_mm256_loadu_si256((const block*)&src[0]),rs);
					block hi=_mm256_sll_epi64(_mm256_loadu_si256((const block*)&src[1]),ls);
					_mm256_storeu_si256((block*)dst,_mm256_or_si256(lo,hi));
					dst+=block_size,src+=block_size;
					rem-=block_size;
				}
				while(rem)
				{
					*dst=(src[0]>>bt)|(src[1]<<(word_size-bt));
					++dst,++src;
					--rem;
				}
				*dst=*src>>bt;
			}
			res.trim();
			return res;
		}
	};
}
#endif
