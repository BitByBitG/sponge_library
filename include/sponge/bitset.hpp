#ifndef SPONGE_BITSET_HPP
#define SPONGE_BITSET_HPP
#include<immintrin.h>
#include<sponge/core.hpp>
namespace spongelib
{
	namespace detail
	{
		template<typename T>
		struct aligned_allocator
		{
			using value_type=T;
			using is_always_equal=std::true_type;
			aligned_allocator()=default;
			template<typename U> aligned_allocator(const aligned_allocator<U>&){}
			T* allocate(size_t n)
			{
				if(n>std::numeric_limits<size_t>::max()/sizeof(T))throw std::bad_array_new_length();
				return static_cast<T*>(::operator new(n*sizeof(T),std::align_val_t(32)));
			}
			void deallocate(T* p,size_t)noexcept
			{
				::operator delete(p,std::align_val_t(32));
			}
			template<typename U> bool operator==(const aligned_allocator<U>&)const noexcept
			{
				return true;
			}
			template<typename U> bool operator!=(const aligned_allocator<U>&)const noexcept
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
	template<size_t N> class static_bitset;
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
			E value;
			template<typename T,std::enable_if_t<!std::is_same_v<std::decay_t<T>,bitset_not>,int> =0> explicit bitset_not(T&& x):value(std::forward<T>(x)){}
			size_t size()const
			{
				return value.size();
			}
			__m256i block_at(size_t i)const
			{
				return _mm256_xor_si256(value.block_at(i),_mm256_set1_epi64x(-1));
			}
		};
		template<typename L,typename R,int op>
		struct bitset_binary:bitset_expression<bitset_binary<L,R,op>>
		{
			L lhs;
			R rhs;
			template<typename X,typename Y> bitset_binary(X&& x,Y&& y):lhs(std::forward<X>(x)),rhs(std::forward<Y>(y))
			{
				assert(lhs.size()==rhs.size());
			}
			size_t size()const
			{
				assert(lhs.size()==rhs.size());
				return lhs.size();
			}
			__m256i block_at(size_t i)const
			{
				auto a=lhs.block_at(i),b=rhs.block_at(i);
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
		return ~std::forward<E>(e);
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
		static constexpr size_t m_size=N;
		static constexpr size_t word_count(){return N/64+(N%64!=0);}
		static constexpr size_t block_count(){return N/256+(N%256!=0);}
		alignas(32) std::array<word,block_count()?block_count()*block_size:block_size> m_data{};
		void block_set(size_t i,block value)
		{
			_mm256_store_si256(reinterpret_cast<block*>(m_data.data()+i*4),value);
		}
		void trim()
		{
			if constexpr(N%64)m_data[word_count()-1]&=(word(1)<<(N%64))-1;
			for(size_t i=word_count();i<m_data.size();i++)m_data[i]=0;
		}
	public:
		static_bitset()=default;
		static_bitset(const static_bitset&)=default;
		static_bitset(static_bitset&&)=default;
		static_bitset& operator=(const static_bitset&)=default;
		static_bitset& operator=(static_bitset&&)=default;
		template<typename E> static_bitset(const detail::bitset_expression<E>& e)
		{
			assert(N==e().size());
			for(size_t i=0;i<block_count();i++)block_set(i,e().block_at(i));
			trim();
		}
		template<typename E> static_bitset& operator=(const detail::bitset_expression<E>& e)
		{
			assert(N==e().size());
			for(size_t i=0;i<block_count();i++)block_set(i,e().block_at(i));
			trim();
			return *this;
		}
		word word_at(size_t i)const{return m_data[i];}
		block block_at(size_t i)const
		{
			return _mm256_load_si256(reinterpret_cast<const block*>(m_data.data()+i*4));
		}
		void swap(static_bitset& other)noexcept{m_data.swap(other.m_data);}
		friend void swap(static_bitset& a,static_bitset& b)noexcept{a.swap(b);}
		bool operator[](size_t position) const
		{
			if (position >= m_size) return false;
			return m_data[position / word_size] >> position % word_size & 1;
		}
		bool operator()(size_t position) const
		{
			if (position >= m_size) return false;
			return m_data[position / word_size] >> position % word_size & 1;
		}
		void set(size_t position)
		{
			if (position >= m_size) return;
			m_data[position / word_size] |= 1ULL << position % word_size;
		}
		void unset(size_t position)
		{
			if (position >= m_size) return;
			m_data[position / word_size] &= ~(1ULL << position % word_size);
		}
		void flip(size_t position)
		{
			if (position >= m_size) return;
			m_data[position / word_size] ^= 1ULL << position % word_size;
		}
		size_t size() const
		{
			return m_size;
		}
		size_t length() const
		{
			return m_size;
		}
		size_t count() const
		{
			const block mask = _mm256_set1_epi8(0x0F);
			const block table = _mm256_setr_epi8(
			0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4,
			0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4
			);
			block result = _mm256_setzero_si256();
			for (size_t i = 0; i != block_count(); ++i)
			{
				block value = block_at(i);
				block low = _mm256_shuffle_epi8(table, _mm256_and_si256(value, mask));
				block high = _mm256_shuffle_epi8(table, _mm256_and_si256(_mm256_srli_epi16(value, 4), mask));
				result = _mm256_add_epi64(result, _mm256_sad_epu8(_mm256_add_epi8(low, high), _mm256_setzero_si256()));
			}
			word parts[block_size];
			_mm256_storeu_si256((block*)parts, result);
			return size_t(parts[0] + parts[1] + parts[2] + parts[3]);
		}
		size_t popcnt() const
		{
			return count();
		}
		bool none() const
		{
			const size_t full_blocks = m_size / 256;
			for (size_t i = 0; i != full_blocks; ++i)
			{
				block mask = _mm256_set1_epi8(-1), value = block_at(i);
				if (!_mm256_testz_si256(value, mask)) return false;
			}
			if (size_t remaining = m_size % 256)
			{
				word mask_array[block_size] ={};
				for (size_t i = 0; i != block_size; ++i)
				{
					if (remaining >= word_size)
					{
						mask_array[i] = ~0ULL;
						remaining -= word_size;
					}
					else
					{
						mask_array[i] = (1ULL << remaining) - 1;
						break;
					}
				}
				block mask = _mm256_loadu_si256((const block*)mask_array), value = block_at(full_blocks);
				if (!_mm256_testz_si256(value, mask)) return false;
			}
			return true;
		}
		bool any() const
		{
			const size_t full_blocks = m_size / 256;
			for (size_t i = 0; i != full_blocks; ++i)
			{
				block mask = _mm256_set1_epi8(-1), value = block_at(i);
				if (!_mm256_testz_si256(value, mask)) return true;
			}
			if (size_t remaining = m_size % 256)
			{
				word mask_array[block_size] ={};
				for (size_t i = 0; i != block_size; ++i)
				{
					if (remaining >= word_size)
					{
						mask_array[i] = ~0ULL;
						remaining -= word_size;
					}
					else
					{
						mask_array[i] = (1ULL << remaining) - 1;
						break;
					}
				}
				block mask = _mm256_loadu_si256((const block*)mask_array), value = block_at(full_blocks);
				if (!_mm256_testz_si256(value, mask)) return true;
			}
			return false;
		}
		bool all() const
		{
			const size_t full_blocks = m_size / 256;
			for (size_t i = 0; i != full_blocks; ++i)
			{
				block mask = _mm256_set1_epi8(-1), value = block_at(i);
				if (!_mm256_testc_si256(value, mask)) return false;
			}
			if (size_t remaining = m_size % 256)
			{
				word mask_array[block_size] ={};
				for (size_t i = 0; i != block_size; ++i)
				{
					if (remaining >= word_size)
					{
						mask_array[i] = ~0ULL;
						remaining -= word_size;
					}
					else
					{
						mask_array[i] = (1ULL << remaining) - 1;
						break;
					}
				}
				block mask = _mm256_loadu_si256((const block*)mask_array), value = block_at(full_blocks);
				if (!_mm256_testc_si256(value, mask)) return false;
			}
			return true;
		}
		void set()
		{
			const block value = _mm256_set1_epi64x(-1);
			for (size_t i = 0; i != block_count(); ++i)
				block_set(i, value);
			trim();
		}
		void unset()
		{
			const block value = _mm256_setzero_si256();
			for (size_t i = 0; i != block_count(); ++i)
				block_set(i, value);
			trim();
		}
		void flip()
		{
			const block value = _mm256_set1_epi64x(-1);
			for (size_t i = 0; i != block_count(); ++i)
				block_set(i, _mm256_xor_si256(block_at(i), value));
			trim();
		}
		template <typename E>
		static_bitset& operator&=(const detail::bitset_expression<E>& e)
		{
			assert(m_size == e().size());
			for (size_t i = 0; i != block_count(); ++i)
				block_set(i, _mm256_and_si256(block_at(i), e().block_at(i)));
			trim();
			return *this;
		}
		template <typename E>
		static_bitset& operator|=(const detail::bitset_expression<E>& e)
		{
			assert(m_size == e().size());
			for (size_t i = 0; i != block_count(); ++i)
				block_set(i, _mm256_or_si256(block_at(i), e().block_at(i)));
			trim();
			return *this;
		}
		template <typename E>
		static_bitset& operator^=(const detail::bitset_expression<E>& e)
		{
			assert(m_size == e().size());
			for (size_t i = 0; i != block_count(); ++i)
				block_set(i, _mm256_xor_si256(block_at(i), e().block_at(i)));
			trim();
			return *this;
		}
		template <typename E>
		static_bitset& operator-=(const detail::bitset_expression<E>& e)
		{
			assert(m_size == e().size());
			for (size_t i = 0; i != block_count(); ++i)
				block_set(i, _mm256_andnot_si256(e().block_at(i), block_at(i)));
			trim();
			return *this;
		}
		friend bool operator==(const static_bitset& a_set, const static_bitset& b_set)
		{
			if (a_set.size() != b_set.size()) return false;
			for (size_t i = 0; i != a_set.block_count(); ++i)
			{
				block bxa = _mm256_xor_si256(b_set.block_at(i), a_set.block_at(i));
				if (!_mm256_testz_si256(bxa, bxa)) return false;
			}
			return true;
		}
		friend bool operator!=(const static_bitset& a_set, const static_bitset& b_set)
		{
			if (a_set.size() != b_set.size()) return true;
			for (size_t i = 0; i != a_set.block_count(); ++i)
			{
				block bxa = _mm256_xor_si256(b_set.block_at(i), a_set.block_at(i));
				if (!_mm256_testz_si256(bxa, bxa)) return true;
			}
			return false;
		}
		friend bool operator<=(const static_bitset& a_set, const static_bitset& b_set)
		{
			assert(a_set.size() == b_set.size());
			for (size_t i = 0; i != a_set.block_count(); ++i)
				if (!_mm256_testc_si256(b_set.block_at(i), a_set.block_at(i)))
				return false;
			return true;
		}
		friend bool operator>=(const static_bitset& a_set, const static_bitset& b_set)
		{
			assert(a_set.size() == b_set.size());
			for (size_t i = 0; i != a_set.block_count(); ++i)
				if (!_mm256_testc_si256(a_set.block_at(i), b_set.block_at(i)))
				return false;
			return true;
		}
		friend bool operator<(const static_bitset& a_set, const static_bitset& b_set)
		{
			assert(a_set.size() == b_set.size());
			bool different = false;
			for (size_t i = 0; i != a_set.block_count(); ++i)
			{
				block a = a_set.block_at(i), b = b_set.block_at(i);
				if (!_mm256_testc_si256(b, a)) return false;
				different |= !_mm256_testc_si256(a, b);
			}
			return different;
		}
		friend bool operator>(const static_bitset& a_set, const static_bitset& b_set)
		{
			assert(a_set.size() == b_set.size());
			bool different = false;
			for (size_t i = 0; i != a_set.block_count(); ++i)
			{
				block a = a_set.block_at(i), b = b_set.block_at(i);
				if (!_mm256_testc_si256(a, b)) return false;
				different |= !_mm256_testc_si256(b, a);
			}
			return different;
		}
		size_t find_first_set(size_t position=0) const
		{
			if (position >= m_size) return size_t(-1);
			size_t word_index = position / word_size, bit_in_word = position % word_size;
			size_t block_index = word_index / block_size, word_in_block = word_index % block_size;
			if (word value = m_data[word_index] & ~((1ULL << bit_in_word) - 1))
			{
				size_t result = word_index * word_size + size_t(__builtin_ctzll(value));
				return result < m_size ? result : size_t(-1);
			}
			while (++word_in_block != block_size)
			{
				size_t current = block_index * block_size + word_in_block;
				if (current >= word_count()) break;
				if (word value = m_data[current])
				{
					size_t result = current * word_size + __builtin_ctzll(value);
					return result < m_size ? result : size_t(-1);
				}
			}
			while (++block_index != block_count())
			{
				block mask = _mm256_set1_epi64x(-1), value = block_at(block_index);
				if (_mm256_testz_si256(value, mask)) continue;
				for (size_t i = 0; i != block_size; ++i)
				{
					size_t current = block_index * block_size + i;
					if (current >= word_count()) break;
					if (word value = m_data[current])
					{
						size_t result = current * word_size + __builtin_ctzll(value);
						return result < m_size ? result : size_t(-1);
					}
				}
			}
			return size_t(-1);
		}
		size_t find_first_unset(size_t position=0) const
		{
			if (position >= m_size) return size_t(-1);
			size_t word_index = position / word_size, bit_in_word = position % word_size;
			size_t block_index = word_index / block_size, word_in_block = word_index % block_size;
			if (word value = ~m_data[word_index] & ~((1ULL << bit_in_word) - 1))
			{
				size_t result = word_index * word_size + size_t(__builtin_ctzll(value));
				return result < m_size ? result : size_t(-1);
			}
			while (++word_in_block != block_size)
			{
				size_t current = block_index * block_size + word_in_block;
				if (current >= word_count()) break;
				if (word value = ~m_data[current])
				{
					size_t result = current * word_size + __builtin_ctzll(value);
					return result < m_size ? result : size_t(-1);
				}
			}
			while (++block_index != block_count())
			{
				block mask = _mm256_set1_epi64x(-1), value = block_at(block_index);
				if (_mm256_testc_si256(value, mask)) continue;
				for (size_t i = 0; i != block_size; ++i)
				{
					size_t current = block_index * block_size + i;
					if (current >= word_count()) break;
					if (word value = ~m_data[current])
					{
						size_t result = current * word_size + __builtin_ctzll(value);
						return result < m_size ? result : size_t(-1);
					}
				}
			}
			return size_t(-1);
		}
		void set(size_t position, size_t length)
		{
			if (position > m_size || length > m_size - position || !length) return;
			size_t word_index = position / word_size, bit_in_word = position % word_size;
			if (bit_in_word)
			{
				size_t head_length = word_size - bit_in_word < length ? word_size - bit_in_word : length;
				m_data[word_index++] |= ((1ULL << head_length) - 1) << bit_in_word;
				length -= head_length;
			}
			for (const block value = _mm256_set1_epi64x(-1); length >= 256; )
			{
				_mm256_storeu_si256((block*)&m_data[word_index], value);
				length -= 256;
				word_index += block_size;
			}
			while (length >= word_size)
			{
				m_data[word_index++] = -1ULL;
				length -= word_size;
			}
			if (length) m_data[word_index] |= (1ULL << length) - 1;
		}
		void unset(size_t position, size_t length)
		{
			if (position > m_size || length > m_size - position || !length) return;
			size_t word_index = position / word_size, bit_in_word = position % word_size;
			if (bit_in_word)
			{
				size_t head_length = word_size - bit_in_word < length ? word_size - bit_in_word : length;
				m_data[word_index++] &= ~(((1ULL << head_length) - 1) << bit_in_word);
				length -= head_length;
			}
			for (const block value = _mm256_setzero_si256(); length >= 256; )
			{
				_mm256_storeu_si256((block*)&m_data[word_index], value);
				length -= 256;
				word_index += block_size;
			}
			while (length >= word_size)
			{
				m_data[word_index++] = 0ULL;
				length -= word_size;
			}
			if (length) m_data[word_index] &= ~((1ULL << length) - 1);
		}
		void flip(size_t position, size_t length)
		{
			if (position > m_size || length > m_size - position || !length) return;
			size_t word_index = position / word_size, bit_in_word = position % word_size;
			if (bit_in_word)
			{
				size_t head_length = word_size - bit_in_word < length ? word_size - bit_in_word : length;
				m_data[word_index++] ^= ((1ULL << head_length) - 1) << bit_in_word;
				length -= head_length;
			}
			for (const block value = _mm256_set1_epi64x(-1); length >= 256; )
			{
				_mm256_storeu_si256((block*)&m_data[word_index], _mm256_xor_si256(_mm256_loadu_si256((const block*)&m_data[word_index]), value));
				length -= 256;
				word_index += block_size;
			}
			while (length >= word_size)
			{
				m_data[word_index++] ^= -1ULL;
				length -= word_size;
			}
			if (length) m_data[word_index] ^= (1ULL << length) - 1;
		}
		static_bitset& operator<<=(size_t step)
		{
			if (step >= m_size) return unset(), *this;
			size_t word_shift = step / word_size, bit_shift = step % word_size;
			if (!bit_shift)
			{
				memmove(m_data.data() + word_shift, m_data.data(), (word_count() - word_shift) * sizeof(word));
				memset(m_data.data(), 0, word_shift * sizeof(word));
			}
			else
			{
				size_t remaining = word_count() - word_shift - 1;
				word *destination = m_data.data() + word_count(), *source = destination - word_shift;
				__m128i l_shift = _mm_cvtsi32_si128(int(bit_shift)), r_shift = _mm_cvtsi32_si128(int(word_size - bit_shift));
				while (remaining >= block_size)
				{
					destination -= block_size, source -= block_size;
					block low = _mm256_srl_epi64(_mm256_loadu_si256((const block*)&source[-1]), r_shift);
					block high = _mm256_sll_epi64(_mm256_loadu_si256((const block*)&source[0]), l_shift);
					_mm256_storeu_si256((block*)destination, _mm256_or_si256(low, high));
					remaining -= block_size;
				}
				while (remaining)
				{
					--destination, --source;
					*destination = (source[0] << bit_shift) | (source[-1] >> (word_size - bit_shift));
					--remaining;
				}
				*--destination = *--source << bit_shift;
				memset(m_data.data(), 0, word_shift * sizeof(word));
			}
			trim();
			return *this;
		}
		static_bitset& operator>>=(size_t step)
		{
			if (step >= m_size) return unset(), *this;
			size_t word_shift = step / word_size, bit_shift = step % word_size;
			if (!bit_shift)
			{
				memmove(m_data.data(), m_data.data() + word_shift, (word_count() - word_shift) * sizeof(word));
				memset(m_data.data() + word_count() - word_shift, 0, word_shift * sizeof(word));
			}
			else
			{
				size_t remaining = word_count() - word_shift - 1;
				word *destination = m_data.data(), *source = m_data.data() + word_shift;
				__m128i r_shift = _mm_cvtsi32_si128(int(bit_shift)), l_shift = _mm_cvtsi32_si128(int(word_size - bit_shift));
				while (remaining >= block_size)
				{
					block low = _mm256_srl_epi64(_mm256_loadu_si256((const block*)&source[0]), r_shift);
					block high = _mm256_sll_epi64(_mm256_loadu_si256((const block*)&source[1]), l_shift);
					_mm256_storeu_si256((block*)destination, _mm256_or_si256(low, high));
					destination += block_size, source += block_size;
					remaining -= block_size;
				}
				while (remaining)
				{
					*destination = (source[0] >> bit_shift) | (source[1] << (word_size - bit_shift));
					++destination, ++source;
					--remaining;
				}
				*destination = *source >> bit_shift;
				memset(m_data.data() + word_count() - word_shift, 0, word_shift * sizeof(word));
			}
			trim();
			return *this;
		}
		static_bitset operator<<(size_t step) const
		{
			static_bitset result;
			if (step >= m_size) return result;
			size_t word_shift = step / word_size, bit_shift = step % word_size;
			if (!bit_shift)
			{
				memcpy(result.m_data.data() + word_shift, m_data.data(), (word_count() - word_shift) * sizeof(word));
			}
			else
			{
				size_t remaining = word_count() - word_shift - 1;
				word* destination = result.m_data.data() + word_count();
				const word* source = m_data.data() + word_count() - word_shift;
				__m128i l_shift = _mm_cvtsi32_si128(int(bit_shift)), r_shift = _mm_cvtsi32_si128(int(word_size - bit_shift));
				while (remaining >= block_size)
				{
					destination -= block_size, source -= block_size;
					block low = _mm256_srl_epi64(_mm256_loadu_si256((const block*)&source[-1]), r_shift);
					block high = _mm256_sll_epi64(_mm256_loadu_si256((const block*)&source[0]), l_shift);
					_mm256_storeu_si256((block*)destination, _mm256_or_si256(low, high));
					remaining -= block_size;
				}
				while (remaining)
				{
					--destination, --source;
					*destination = (source[0] << bit_shift) | (source[-1] >> (word_size - bit_shift));
					--remaining;
				}
				*--destination = *--source << bit_shift;
			}
			result.trim();
			return result;
		}
		static_bitset operator>>(size_t step) const
		{
			static_bitset result;
			if (step >= m_size) return result;
			size_t word_shift = step / word_size, bit_shift = step % word_size;
			if (!bit_shift)
			{
				memcpy(result.m_data.data(), m_data.data() + word_shift, (word_count() - word_shift) * sizeof(word));
			}
			else
			{
				size_t remaining = word_count() - word_shift - 1;
				word* destination = result.m_data.data();
				const word* source = m_data.data() + word_shift;
				__m128i r_shift = _mm_cvtsi32_si128(int(bit_shift)), l_shift = _mm_cvtsi32_si128(int(word_size - bit_shift));
				while (remaining >= block_size)
				{
					block low = _mm256_srl_epi64(_mm256_loadu_si256((const block*)&source[0]), r_shift);
					block high = _mm256_sll_epi64(_mm256_loadu_si256((const block*)&source[1]), l_shift);
					_mm256_storeu_si256((block*)destination, _mm256_or_si256(low, high));
					destination += block_size, source += block_size;
					remaining -= block_size;
				}
				while (remaining)
				{
					*destination = (source[0] >> bit_shift) | (source[1] << (word_size - bit_shift));
					++destination, ++source;
					--remaining;
				}
				*destination = *source >> bit_shift;
			}
			result.trim();
			return result;
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
		std::vector<word,detail::aligned_allocator<word>> m_data;
		size_t m_size=0;
		static size_t storage_size(size_t n)
		{
			if(n>std::numeric_limits<size_t>::max()-255)throw std::length_error("dynamic_bitset");
			return ((n+255)/256)*4;
		}
		size_t word_count()const
		{
			return m_size/64+(m_size%64!=0);
		}
		size_t block_count()const
		{
			return m_data.size()/4;
		}
		void block_set(size_t i,block value)
		{
			_mm256_store_si256(reinterpret_cast<block*>(m_data.data()+i*4),value);
		}
		void trim()
		{
			size_t n=word_count();
			if(m_size%64)m_data[n-1]&=(word(1)<<(m_size%64))-1;
			for(size_t i=n;i<m_data.size();i++)m_data[i]=0;
		}
	public:
		dynamic_bitset()=default;
		explicit dynamic_bitset(size_t n,bool value=false):m_data(storage_size(n),value?~word(0):0),m_size(n)
		{
			trim();
		}
		dynamic_bitset(const dynamic_bitset&)=default;
		dynamic_bitset(dynamic_bitset&& other)noexcept:m_data(std::move(other.m_data)),m_size(std::exchange(other.m_size,0))
		{
			other.m_data.clear();
		}
		dynamic_bitset& operator=(const dynamic_bitset& other)
		{
			if(this!=&other)
			{
				m_data=other.m_data;
				m_size=other.m_size;
			}
			return *this;
		}
		dynamic_bitset& operator=(dynamic_bitset&& other)noexcept
		{
			if(this!=&other)
			{
				m_data=std::move(other.m_data);
				m_size=std::exchange(other.m_size,0);
				other.m_data.clear();
			}
			return *this;
		}
		template<typename E> dynamic_bitset(const detail::bitset_expression<E>& e):dynamic_bitset(e().size())
		{
			for(size_t i=0;i<block_count();i++)block_set(i,e().block_at(i));
			trim();
		}
		template<typename E> dynamic_bitset& operator=(const detail::bitset_expression<E>& e)
		{
			if(m_size!=e().size())
			{
				dynamic_bitset tmp(e);
				swap(tmp);
				return *this;
			}
			for(size_t i=0;i<block_count();i++)block_set(i,e().block_at(i));
			trim();
			return *this;
		}
		word word_at(size_t i)const
		{
			return m_data[i];
		}
		block block_at(size_t i)const
		{
			return _mm256_load_si256(reinterpret_cast<const block*>(m_data.data()+i*4));
		}
		void resize(size_t n,bool value=false)
		{
			size_t old=m_size;
			m_data.resize(storage_size(n),0);
			m_size=n;
			if(n>old&&value)set(old,n-old);
			trim();
		}
		void reserve(size_t n)
		{
			m_data.reserve(storage_size(n));
		}
		size_t capacity()const
		{
			return m_data.capacity()*64;
		}
		void shrink_to_fit()
		{
			m_data.shrink_to_fit();
		}
		void clear()noexcept
		{
			m_data.clear();
			m_size=0;
		}
		void swap(dynamic_bitset& other)noexcept
		{
			m_data.swap(other.m_data);
			std::swap(m_size,other.m_size);
		}
		friend void swap(dynamic_bitset& a,dynamic_bitset& b)noexcept
		{
			a.swap(b);
		}
		bool operator[](size_t position) const
		{
			if (position >= m_size) return false;
			return m_data[position / word_size] >> position % word_size & 1;
		}
		bool operator()(size_t position) const
		{
			if (position >= m_size) return false;
			return m_data[position / word_size] >> position % word_size & 1;
		}
		void set(size_t position)
		{
			if (position >= m_size) return;
			m_data[position / word_size] |= 1ULL << position % word_size;
		}
		void unset(size_t position)
		{
			if (position >= m_size) return;
			m_data[position / word_size] &= ~(1ULL << position % word_size);
		}
		void flip(size_t position)
		{
			if (position >= m_size) return;
			m_data[position / word_size] ^= 1ULL << position % word_size;
		}
		size_t size() const
		{
			return m_size;
		}
		size_t length() const
		{
			return m_size;
		}
		size_t count() const
		{
			const block mask = _mm256_set1_epi8(0x0F);
			const block table = _mm256_setr_epi8(
			0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4,
			0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4
			);
			block result = _mm256_setzero_si256();
			for (size_t i = 0; i != block_count(); ++i)
			{
				block value = block_at(i);
				block low = _mm256_shuffle_epi8(table, _mm256_and_si256(value, mask));
				block high = _mm256_shuffle_epi8(table, _mm256_and_si256(_mm256_srli_epi16(value, 4), mask));
				result = _mm256_add_epi64(result, _mm256_sad_epu8(_mm256_add_epi8(low, high), _mm256_setzero_si256()));
			}
			word parts[block_size];
			_mm256_storeu_si256((block*)parts, result);
			return size_t(parts[0] + parts[1] + parts[2] + parts[3]);
		}
		size_t popcnt() const
		{
			return count();
		}
		bool none() const
		{
			const size_t full_blocks = m_size / 256;
			for (size_t i = 0; i != full_blocks; ++i)
			{
				block mask = _mm256_set1_epi8(-1), value = block_at(i);
				if (!_mm256_testz_si256(value, mask)) return false;
			}
			if (size_t remaining = m_size % 256)
			{
				word mask_array[block_size] ={};
				for (size_t i = 0; i != block_size; ++i)
				{
					if (remaining >= word_size)
					{
						mask_array[i] = ~0ULL;
						remaining -= word_size;
					}
					else
					{
						mask_array[i] = (1ULL << remaining) - 1;
						break;
					}
				}
				block mask = _mm256_loadu_si256((const block*)mask_array), value = block_at(full_blocks);
				if (!_mm256_testz_si256(value, mask)) return false;
			}
			return true;
		}
		bool any() const
		{
			const size_t full_blocks = m_size / 256;
			for (size_t i = 0; i != full_blocks; ++i)
			{
				block mask = _mm256_set1_epi8(-1), value = block_at(i);
				if (!_mm256_testz_si256(value, mask)) return true;
			}
			if (size_t remaining = m_size % 256)
			{
				word mask_array[block_size] ={};
				for (size_t i = 0; i != block_size; ++i)
				{
					if (remaining >= word_size)
					{
						mask_array[i] = ~0ULL;
						remaining -= word_size;
					}
					else
					{
						mask_array[i] = (1ULL << remaining) - 1;
						break;
					}
				}
				block mask = _mm256_loadu_si256((const block*)mask_array), value = block_at(full_blocks);
				if (!_mm256_testz_si256(value, mask)) return true;
			}
			return false;
		}
		bool all() const
		{
			const size_t full_blocks = m_size / 256;
			for (size_t i = 0; i != full_blocks; ++i)
			{
				block mask = _mm256_set1_epi8(-1), value = block_at(i);
				if (!_mm256_testc_si256(value, mask)) return false;
			}
			if (size_t remaining = m_size % 256)
			{
				word mask_array[block_size] ={};
				for (size_t i = 0; i != block_size; ++i)
				{
					if (remaining >= word_size)
					{
						mask_array[i] = ~0ULL;
						remaining -= word_size;
					}
					else
					{
						mask_array[i] = (1ULL << remaining) - 1;
						break;
					}
				}
				block mask = _mm256_loadu_si256((const block*)mask_array), value = block_at(full_blocks);
				if (!_mm256_testc_si256(value, mask)) return false;
			}
			return true;
		}
		void set()
		{
			const block value = _mm256_set1_epi64x(-1);
			for (size_t i = 0; i != block_count(); ++i)
				block_set(i, value);
			trim();
		}
		void unset()
		{
			const block value = _mm256_setzero_si256();
			for (size_t i = 0; i != block_count(); ++i)
				block_set(i, value);
			trim();
		}
		void flip()
		{
			const block value = _mm256_set1_epi64x(-1);
			for (size_t i = 0; i != block_count(); ++i)
				block_set(i, _mm256_xor_si256(block_at(i), value));
			trim();
		}
		template <typename E>
		dynamic_bitset& operator&=(const detail::bitset_expression<E>& e)
		{
			assert(m_size == e().size());
			for (size_t i = 0; i != block_count(); ++i)
				block_set(i, _mm256_and_si256(block_at(i), e().block_at(i)));
			trim();
			return *this;
		}
		template <typename E>
		dynamic_bitset& operator|=(const detail::bitset_expression<E>& e)
		{
			assert(m_size == e().size());
			for (size_t i = 0; i != block_count(); ++i)
				block_set(i, _mm256_or_si256(block_at(i), e().block_at(i)));
			trim();
			return *this;
		}
		template <typename E>
		dynamic_bitset& operator^=(const detail::bitset_expression<E>& e)
		{
			assert(m_size == e().size());
			for (size_t i = 0; i != block_count(); ++i)
				block_set(i, _mm256_xor_si256(block_at(i), e().block_at(i)));
			trim();
			return *this;
		}
		template <typename E>
		dynamic_bitset& operator-=(const detail::bitset_expression<E>& e)
		{
			assert(m_size == e().size());
			for (size_t i = 0; i != block_count(); ++i)
				block_set(i, _mm256_andnot_si256(e().block_at(i), block_at(i)));
			trim();
			return *this;
		}
		friend bool operator==(const dynamic_bitset& a_set, const dynamic_bitset& b_set)
		{
			if (a_set.size() != b_set.size()) return false;
			for (size_t i = 0; i != a_set.block_count(); ++i)
			{
				block bxa = _mm256_xor_si256(b_set.block_at(i), a_set.block_at(i));
				if (!_mm256_testz_si256(bxa, bxa)) return false;
			}
			return true;
		}
		friend bool operator!=(const dynamic_bitset& a_set, const dynamic_bitset& b_set)
		{
			if (a_set.size() != b_set.size()) return true;
			for (size_t i = 0; i != a_set.block_count(); ++i)
			{
				block bxa = _mm256_xor_si256(b_set.block_at(i), a_set.block_at(i));
				if (!_mm256_testz_si256(bxa, bxa)) return true;
			}
			return false;
		}
		friend bool operator<=(const dynamic_bitset& a_set, const dynamic_bitset& b_set)
		{
			assert(a_set.size() == b_set.size());
			for (size_t i = 0; i != a_set.block_count(); ++i)
				if (!_mm256_testc_si256(b_set.block_at(i), a_set.block_at(i)))
				return false;
			return true;
		}
		friend bool operator>=(const dynamic_bitset& a_set, const dynamic_bitset& b_set)
		{
			assert(a_set.size() == b_set.size());
			for (size_t i = 0; i != a_set.block_count(); ++i)
				if (!_mm256_testc_si256(a_set.block_at(i), b_set.block_at(i)))
				return false;
			return true;
		}
		friend bool operator<(const dynamic_bitset& a_set, const dynamic_bitset& b_set)
		{
			assert(a_set.size() == b_set.size());
			bool different = false;
			for (size_t i = 0; i != a_set.block_count(); ++i)
			{
				block a = a_set.block_at(i), b = b_set.block_at(i);
				if (!_mm256_testc_si256(b, a)) return false;
				different |= !_mm256_testc_si256(a, b);
			}
			return different;
		}
		friend bool operator>(const dynamic_bitset& a_set, const dynamic_bitset& b_set)
		{
			assert(a_set.size() == b_set.size());
			bool different = false;
			for (size_t i = 0; i != a_set.block_count(); ++i)
			{
				block a = a_set.block_at(i), b = b_set.block_at(i);
				if (!_mm256_testc_si256(a, b)) return false;
				different |= !_mm256_testc_si256(b, a);
			}
			return different;
		}
		size_t find_first_set(size_t position=0) const
		{
			if (position >= m_size) return size_t(-1);
			size_t word_index = position / word_size, bit_in_word = position % word_size;
			size_t block_index = word_index / block_size, word_in_block = word_index % block_size;
			if (word value = m_data[word_index] & ~((1ULL << bit_in_word) - 1))
			{
				size_t result = word_index * word_size + size_t(__builtin_ctzll(value));
				return result < m_size ? result : size_t(-1);
			}
			while (++word_in_block != block_size)
			{
				size_t current = block_index * block_size + word_in_block;
				if (current >= word_count()) break;
				if (word value = m_data[current])
				{
					size_t result = current * word_size + __builtin_ctzll(value);
					return result < m_size ? result : size_t(-1);
				}
			}
			while (++block_index != block_count())
			{
				block mask = _mm256_set1_epi64x(-1), value = block_at(block_index);
				if (_mm256_testz_si256(value, mask)) continue;
				for (size_t i = 0; i != block_size; ++i)
				{
					size_t current = block_index * block_size + i;
					if (current >= word_count()) break;
					if (word value = m_data[current])
					{
						size_t result = current * word_size + __builtin_ctzll(value);
						return result < m_size ? result : size_t(-1);
					}
				}
			}
			return size_t(-1);
		}
		size_t find_first_unset(size_t position=0) const
		{
			if (position >= m_size) return size_t(-1);
			size_t word_index = position / word_size, bit_in_word = position % word_size;
			size_t block_index = word_index / block_size, word_in_block = word_index % block_size;
			if (word value = ~m_data[word_index] & ~((1ULL << bit_in_word) - 1))
			{
				size_t result = word_index * word_size + size_t(__builtin_ctzll(value));
				return result < m_size ? result : size_t(-1);
			}
			while (++word_in_block != block_size)
			{
				size_t current = block_index * block_size + word_in_block;
				if (current >= word_count()) break;
				if (word value = ~m_data[current])
				{
					size_t result = current * word_size + __builtin_ctzll(value);
					return result < m_size ? result : size_t(-1);
				}
			}
			while (++block_index != block_count())
			{
				block mask = _mm256_set1_epi64x(-1), value = block_at(block_index);
				if (_mm256_testc_si256(value, mask)) continue;
				for (size_t i = 0; i != block_size; ++i)
				{
					size_t current = block_index * block_size + i;
					if (current >= word_count()) break;
					if (word value = ~m_data[current])
					{
						size_t result = current * word_size + __builtin_ctzll(value);
						return result < m_size ? result : size_t(-1);
					}
				}
			}
			return size_t(-1);
		}
		void set(size_t position, size_t length)
		{
			if (position > m_size || length > m_size - position || !length) return;
			size_t word_index = position / word_size, bit_in_word = position % word_size;
			if (bit_in_word)
			{
				size_t head_length = word_size - bit_in_word < length ? word_size - bit_in_word : length;
				m_data[word_index++] |= ((1ULL << head_length) - 1) << bit_in_word;
				length -= head_length;
			}
			for (const block value = _mm256_set1_epi64x(-1); length >= 256; )
			{
				_mm256_storeu_si256((block*)&m_data[word_index], value);
				length -= 256;
				word_index += block_size;
			}
			while (length >= word_size)
			{
				m_data[word_index++] = -1ULL;
				length -= word_size;
			}
			if (length) m_data[word_index] |= (1ULL << length) - 1;
		}
		void unset(size_t position, size_t length)
		{
			if (position > m_size || length > m_size - position || !length) return;
			size_t word_index = position / word_size, bit_in_word = position % word_size;
			if (bit_in_word)
			{
				size_t head_length = word_size - bit_in_word < length ? word_size - bit_in_word : length;
				m_data[word_index++] &= ~(((1ULL << head_length) - 1) << bit_in_word);
				length -= head_length;
			}
			for (const block value = _mm256_setzero_si256(); length >= 256; )
			{
				_mm256_storeu_si256((block*)&m_data[word_index], value);
				length -= 256;
				word_index += block_size;
			}
			while (length >= word_size)
			{
				m_data[word_index++] = 0ULL;
				length -= word_size;
			}
			if (length) m_data[word_index] &= ~((1ULL << length) - 1);
		}
		void flip(size_t position, size_t length)
		{
			if (position > m_size || length > m_size - position || !length) return;
			size_t word_index = position / word_size, bit_in_word = position % word_size;
			if (bit_in_word)
			{
				size_t head_length = word_size - bit_in_word < length ? word_size - bit_in_word : length;
				m_data[word_index++] ^= ((1ULL << head_length) - 1) << bit_in_word;
				length -= head_length;
			}
			for (const block value = _mm256_set1_epi64x(-1); length >= 256; )
			{
				_mm256_storeu_si256((block*)&m_data[word_index], _mm256_xor_si256(_mm256_loadu_si256((const block*)&m_data[word_index]), value));
				length -= 256;
				word_index += block_size;
			}
			while (length >= word_size)
			{
				m_data[word_index++] ^= -1ULL;
				length -= word_size;
			}
			if (length) m_data[word_index] ^= (1ULL << length) - 1;
		}
		dynamic_bitset& operator<<=(size_t step)
		{
			if (step >= m_size) return unset(), *this;
			size_t word_shift = step / word_size, bit_shift = step % word_size;
			if (!bit_shift)
			{
				memmove(m_data.data() + word_shift, m_data.data(), (word_count() - word_shift) * sizeof(word));
				memset(m_data.data(), 0, word_shift * sizeof(word));
			}
			else
			{
				size_t remaining = word_count() - word_shift - 1;
				word *destination = m_data.data() + word_count(), *source = destination - word_shift;
				__m128i l_shift = _mm_cvtsi32_si128(int(bit_shift)), r_shift = _mm_cvtsi32_si128(int(word_size - bit_shift));
				while (remaining >= block_size)
				{
					destination -= block_size, source -= block_size;
					block low = _mm256_srl_epi64(_mm256_loadu_si256((const block*)&source[-1]), r_shift);
					block high = _mm256_sll_epi64(_mm256_loadu_si256((const block*)&source[0]), l_shift);
					_mm256_storeu_si256((block*)destination, _mm256_or_si256(low, high));
					remaining -= block_size;
				}
				while (remaining)
				{
					--destination, --source;
					*destination = (source[0] << bit_shift) | (source[-1] >> (word_size - bit_shift));
					--remaining;
				}
				*--destination = *--source << bit_shift;
				memset(m_data.data(), 0, word_shift * sizeof(word));
			}
			trim();
			return *this;
		}
		dynamic_bitset& operator>>=(size_t step)
		{
			if (step >= m_size) return unset(), *this;
			size_t word_shift = step / word_size, bit_shift = step % word_size;
			if (!bit_shift)
			{
				memmove(m_data.data(), m_data.data() + word_shift, (word_count() - word_shift) * sizeof(word));
				memset(m_data.data() + word_count() - word_shift, 0, word_shift * sizeof(word));
			}
			else
			{
				size_t remaining = word_count() - word_shift - 1;
				word *destination = m_data.data(), *source = m_data.data() + word_shift;
				__m128i r_shift = _mm_cvtsi32_si128(int(bit_shift)), l_shift = _mm_cvtsi32_si128(int(word_size - bit_shift));
				while (remaining >= block_size)
				{
					block low = _mm256_srl_epi64(_mm256_loadu_si256((const block*)&source[0]), r_shift);
					block high = _mm256_sll_epi64(_mm256_loadu_si256((const block*)&source[1]), l_shift);
					_mm256_storeu_si256((block*)destination, _mm256_or_si256(low, high));
					destination += block_size, source += block_size;
					remaining -= block_size;
				}
				while (remaining)
				{
					*destination = (source[0] >> bit_shift) | (source[1] << (word_size - bit_shift));
					++destination, ++source;
					--remaining;
				}
				*destination = *source >> bit_shift;
				memset(m_data.data() + word_count() - word_shift, 0, word_shift * sizeof(word));
			}
			trim();
			return *this;
		}
		dynamic_bitset operator<<(size_t step) const
		{
			dynamic_bitset result(m_size);
			if (step >= m_size) return result;
			size_t word_shift = step / word_size, bit_shift = step % word_size;
			if (!bit_shift)
			{
				memcpy(result.m_data.data() + word_shift, m_data.data(), (word_count() - word_shift) * sizeof(word));
			}
			else
			{
				size_t remaining = word_count() - word_shift - 1;
				word* destination = result.m_data.data() + word_count();
				const word* source = m_data.data() + word_count() - word_shift;
				__m128i l_shift = _mm_cvtsi32_si128(int(bit_shift)), r_shift = _mm_cvtsi32_si128(int(word_size - bit_shift));
				while (remaining >= block_size)
				{
					destination -= block_size, source -= block_size;
					block low = _mm256_srl_epi64(_mm256_loadu_si256((const block*)&source[-1]), r_shift);
					block high = _mm256_sll_epi64(_mm256_loadu_si256((const block*)&source[0]), l_shift);
					_mm256_storeu_si256((block*)destination, _mm256_or_si256(low, high));
					remaining -= block_size;
				}
				while (remaining)
				{
					--destination, --source;
					*destination = (source[0] << bit_shift) | (source[-1] >> (word_size - bit_shift));
					--remaining;
				}
				*--destination = *--source << bit_shift;
			}
			result.trim();
			return result;
		}
		dynamic_bitset operator>>(size_t step) const
		{
			dynamic_bitset result(m_size);
			if (step >= m_size) return result;
			size_t word_shift = step / word_size, bit_shift = step % word_size;
			if (!bit_shift)
			{
				memcpy(result.m_data.data(), m_data.data() + word_shift, (word_count() - word_shift) * sizeof(word));
			}
			else
			{
				size_t remaining = word_count() - word_shift - 1;
				word* destination = result.m_data.data();
				const word* source = m_data.data() + word_shift;
				__m128i r_shift = _mm_cvtsi32_si128(int(bit_shift)), l_shift = _mm_cvtsi32_si128(int(word_size - bit_shift));
				while (remaining >= block_size)
				{
					block low = _mm256_srl_epi64(_mm256_loadu_si256((const block*)&source[0]), r_shift);
					block high = _mm256_sll_epi64(_mm256_loadu_si256((const block*)&source[1]), l_shift);
					_mm256_storeu_si256((block*)destination, _mm256_or_si256(low, high));
					destination += block_size, source += block_size;
					remaining -= block_size;
				}
				while (remaining)
				{
					*destination = (source[0] >> bit_shift) | (source[1] << (word_size - bit_shift));
					++destination, ++source;
					--remaining;
				}
				*destination = *source >> bit_shift;
			}
			result.trim();
			return result;
		}
	};
}
#endif