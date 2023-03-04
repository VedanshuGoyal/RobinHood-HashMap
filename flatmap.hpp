#pragma once
// #include <functional>
// #include <cstddef>
#include <bits/stdc++.h>

struct power_of_two{
	static constexpr size_t start_size = 4;
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

template <typename K, typename V>
struct Bucket
{
	K _key;
	V _value;
	int8_t distance = -1;

	Bucket() {}
	Bucket(int8_t _distance) : distance(_distance) {}
	// Bucket(const K &key, const V &value) : _key(key), _value(value) {}

	bool has_value() const{
		return distance > -1;
	}

	template<typename Key, typename Value>
	void emplace(int8_t _distance, Key && k, Value && v){
		distance = _distance;
		new(std::addressof(_key)) K(std::forward<Key>(k));
		new(std::addressof(_value)) V(std::forward<Value>(v));
	}

	void mswap(int8_t & _distance, K & k, V & v){
		std::swap(distance, _distance);
		std::swap(k, _key);
		std::swap(v, _value);
	}

	bool is_empty() const{
		return distance < 0;
	}
};

template<typename H>
struct MyHasher : private H{

};

template<typename E>
struct MyEqual : private E{

};


struct Iterator{

};

template<typename K, typename V, typename H = std::hash<K>, typename E = std::equal_to<K>, typename A = std::allocator<std::pair<K, V>>, typename Hashpolicy = power_of_two>
class MyHashMap : private H, private E, private Hp{
	using Allocator = typename std::allocator_traits<A>::rebind_alloc<Bucket>;
	using AllocTraits = std::allocator_traits<Allocator>;
	using Entry = typename Bucket<K, V>;
	using EntryPointer = AllocTraits::pointer;
	using cEntryPointer = AllocTraits::const_pointer;

public:
	MyHashMap() {
		ReAlloc(Hp::start_size);
	}

private:
	static int constexpr min_lookup = 4;

	Allocator Alloc;
	Bucket* _Blk;
	size_t num_slots_minus_one = 0;
	size_t num_elements = 0;
	int8_t max_lookup = min_lookup;
	float loadfactor = 0.5f;

	constexpr size_t log2(size_t x) {return x == 0 ? 0 : 61-__builtin_clzll(x);} // floor(log2(x))
	static constexpr int8_t compute_max_lookup(size_t num_buckets){
		return std::max(min_lookup, log2(num_buckets));
	}

	template<typename Key, typename Value>
	bool emplace(Key && k, Value && v){
		size_t index = hash_index(hash_key(k), num_slots_minus_one);
		int8_t distance = 0;

		EntryPointer it {_Blk};

		for(; distance < it->distance; ++it, ++distance){
			if(compares_equal(it->_key, k)){
				return false;
			}
		}

		if(distance == max_lookup || num_slots_minus_one*loadfactor < num_elements){
			grow();
			emplace(std::forward<Key>(k), std::forward<Value>(v));
		}

		if(it->is_empty()){
			it->emplace(distance, k, v);
			++num_elements;
			return true;
		}

		it->mswap(distance, k, v);
		auto _it = it;

		for(++distance, ++it ; ; ++distance, ++it){
			if(distance == max_lookup){
				_it->mswap(distance, k, v);
				grow();
				return emplace(std::move(k), std::move(v));
			}else if(it->is_empty()){
				it->emplace(distance, k, v);
				++num_elements;
				return 1;
			}else if(it->distance < distance){
				it->mswap(distance, k, v);
			}
		}

		
	}

	void grow(){
		ReAlloc(next_size(num_slots_minus_one));
	}

	void ReAlloc(size_t new_capacity) {
		// Not Work for decrease in size

		if(new_capacity < num_slots_minus_one) return;

		int8_t old_lookup = max_lookup;
		max_lookup = compute_max_lookup(num_slots_minus_one);

		Bucket* Nw_Blk = Alloc.allocate(new_capacity);
		std::swap(num_slots_minus_one, new_capacity - 1);
		std::swap(_Blk, Nw_Blk);

		for(size_t i = 0; i < new_capacity; ++i){
			Nw_Blk[i].distance = -1;
		}
		for(EntryPointer it{Nw_Blk}, end{it + static_cast<ptrdiff_t>(new_capacity + old_lookup)}; it != end; ++it){
			if(it->has_value()){
				emplace(std::move(it->_key), std::move(it->_value));
			}
		}
	}

	size_t hash_key(K const & key) const{
		return static_cast<H&>(*this) (key);
	}

	bool compares_equal(K const & a, K const & b) const {
		return static_cast<E&>(*this) (a, b);
	}
};