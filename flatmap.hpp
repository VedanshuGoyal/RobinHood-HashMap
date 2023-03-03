#pragma once
// #include <functional>
// #include <cstddef>
#include <bits/stdc++.h>


struct power_of_two{
	static constexpr size_t start_size = 2;
	constexpr size_t next_size(size_t &x) const{
		--x;
		x |= x>>1;
		x |= x>>2;
		x |= x>>4;
		x |= x>>8;
		x |= x>>16;
		x |= x>>32;
		return (x + 1);
	}

	constexpr size_t hash_index(size_t hash, size_t slots_minus_one) const {
		return hash&slots_minus_one;
	}
};

struct Iterator{

};

template<typename K, typename V, typename H = std::hash<K>, typename E = std::equal_to<K>, typename A = std::allocator<std::pair<K, V>>, typename Hashpolicy = power_of_two>
class MyHashMap : private H, private E, private Hp{

	struct Bucket
	{
		K _key;
		V _value;
		int8_t distance = -1;
		Bucket(const K &key, const V &value) : _key(key), _value(value) {}

		bool has_value() const{
			return distance > -1;
		}

		bool is_empty() const{
			return distance < 0;
		}

	};

	using B_Alloc = typename std::allocator_traits<A>::rebind_alloc<Bucket>;
	using B_Pointer = B_Alloc::

public:
	MyHashMap() {
		ReAlloc(Hp::start_size);
	}

private:

private:
	static int constexpr min_lookup = 4;

	B_Alloc Alloc;
	Bucket* _Blk;
	size_t num_slots_minus_one = 0;
	int8_t max_lookup = min_lookup;

	constexpr size_t log2(size_t x) {return x == 0 ? 0 : 61-__builtin_clzll(x);} // floor(log2(x))
	static constexpr int8_t compute_max_lookup(size_t num_buckets){
		return std::max(min_lookup, log2(num_buckets));
	}

	template<typename Key, typename... Arg>
	void emplace(Key && k, Arg && a){
			
	}

	void ReAlloc(size_t new_capacity) {
		Bucket* Nw_Blk = Alloc.allocate(new_capacity);

		for(size_t i = 0; i < new_capacity; ++i){
			Nw_Blk[i].distance = -1;
		}

		std::swap(num_slots_minus_one, new_capacity - 1);
		std::swap(_Blk, Nw_Blk);
		int8_t old_lookup = max_lookup;


		for(size_t i = 0; i < num_slots_minus_one; ++i){
			if(Nw_Blk[i].has_value()){
				emplace(std::move(Nw_Blk[i]));
			}
		}
	}
};