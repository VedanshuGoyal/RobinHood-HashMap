#pragma once
// #include <functional>
// #include <cstddef>
#include <bits/stdc++.h>

struct power_of_two{
	static constexpr size_t min_size = 4;

	constexpr size_t next_size(size_t x) const{
		x |= x>>1;
		x |= x>>2;
		x |= x>>4;
		x |= x>>8;
		x |= x>>16;
		x |= x>>32;
		return ++x;
	}

	constexpr size_t hash_index(size_t hash, size_t slots_minus_one) const {
		return hash&slots_minus_one;
	}
};

template <typename T>
struct Bucket
{
	T value;
	int8_t distance = -1;
	static constexpr int8_t special_end_value = 0;

	Bucket() {}
	Bucket(int8_t _distance) : distance(_distance) {}

	template<typename ...Args>
	void emplace(int8_t _distance, Args&&... args){
		distance = _distance;
		new(std::addressof(value)) T(std::forward<Args>(args)...);
	}

	void mswap(int8_t _distance, T& _value){
		std::swap(distance, _distance);
		std::swap(value, _value);
	}

	bool has_value() const{
		return distance > -1;
	}

	bool is_empty() const{
		return distance < 0;
	}

	bool is_at_desired_position() const{
        return distance <= 0;
    }

	void destroy() {
		value.~T();
		distance = -1;
	}
};

template<typename K, typename Hasher>
struct MyHasher : private Hasher{
	size_t operator () (K const & key) const {
		return static_cast<const Hasher&> (*this) (key);
	}

	template <typename F, typename S>
	size_t operator () (std::pair<F, S> const & value) const {
		return static_cast<const Hasher&> (*this) (value.first);
	}
};

template<typename K, typename KeyEqual>
struct MyEqual : private KeyEqual{
	bool operator () (K const & lhs, K const & rhs) const {
		return static_cast<const KeyEqual&> (*this) (lhs, rhs);
	}

	template <typename F, typename S>
	bool operator () (K const & lhs, std::pair<F, S> const & rhs) const {
		return static_cast<const KeyEqual&> (*this) (lhs, rhs.first);
	}

	template <typename F, typename S>
	bool operator () (std::pair<F, S> const & lhs, K const & rhs) const {
		return static_cast<const KeyEqual&> (*this) (lhs.first, rhs);
	}

	template <typename F, typename S>
	bool operator () (std::pair<F, S> const & lhs, std::pair<F, S> const & rhs) const {
		return static_cast<const KeyEqual&> (*this) (lhs.first, rhs.first);
	}
};

template<typename ValueType, typename EntryPointer>
class MapIterator{
public:
	using value_type = ValueType;
	using pointer = ValueType*;
	using reference = ValueType&;

	EntryPointer m_Ptr = EntryPointer();
	MapIterator() = default;
	MapIterator(EntryPointer ptr) : m_Ptr(ptr) {}

	bool operator==(const MapIterator & other) const{
        return m_Ptr == other.m_Ptr;
    }

    bool operator!=(const MapIterator & other) const{
        return !(m_Ptr == other.m_Ptr);
    }

    MapIterator& operator++(){
    	for( ++m_Ptr ; m_Ptr->is_empty() ; ) ++m_Ptr;
        return *this;
    }

    MapIterator operator++(int){
        MapIterator copy(*this);
        ++*this;
        return copy;
    }

    ValueType & operator*() const{
        return m_Ptr->value;
    }

    ValueType * operator->() const{
        return std::addressof(m_Ptr->value);
    }

    operator MapIterator<const value_type, EntryPointer>() const{
        return { m_Ptr };
    }

};

template<typename K, typename V, typename H = std::hash<K>, typename E = std::equal_to<K>, typename A = std::allocator<std::pair<K, V>>, typename Hashpolicy = power_of_two>
class MyHashMap : private MyHasher<K, H>, private MyEqual<K, E>, private Hashpolicy{
public:

	using value_type = typename std::pair<K, V>;
	using Entry = Bucket<value_type>;
	using Allocator = typename std::allocator_traits<A>::rebind_alloc<Entry>;
	using AllocTraits = std::allocator_traits<Allocator>;
	using cEntryPointer = AllocTraits::const_pointer;
	using EntryPointer = AllocTraits::pointer;
	using Hasher = MyHasher<K, H>;
	using Equal_To = MyEqual<K, E>;
	using iterator = MapIterator<value_type, EntryPointer>;
	using const_iterator = MapIterator<const value_type, EntryPointer>;

private:
	static constexpr size_t min_size = Hashpolicy::min_size;
	static int8_t constexpr min_lookup = 4;
	static inline Entry DefTab[min_lookup] = { {}, {}, {}, {Entry::special_end_value} };

	Allocator Alloc;
	EntryPointer _Blk = DefTab;
	size_t num_slots_minus_one = 0;
	size_t num_elements = 0;
	int8_t max_lookup = min_lookup - 1;
	float loadfactor = 0.5f;

	static constexpr int8_t log2(size_t x) {return x == 0 ? 0 : 61-__builtin_clzll(x);} // floor(log2(x))
	static constexpr int8_t compute_max_lookup(size_t num_buckets){
		return std::max(min_lookup, log2(num_buckets));
	}

	template<typename Key> 
	size_t hash_key(Key const & key) const{
		return static_cast<const Hasher&>(*this) (key);
	}


	template<typename L, typename R>
	bool compares_equal(L const & lhs, R const & rhs) const {
		return static_cast<const Equal_To&>(*this) (lhs, rhs);
	}


public:
	MyHashMap() {}
	template<typename Key, typename ...Args>
	std::pair<iterator, bool> emplace(Key && k, Args&&... args){
		size_t index = static_cast<Hashpolicy&>(*this).hash_index(hash_key(k), num_slots_minus_one);
		int8_t distance = 0;

		EntryPointer it {_Blk + static_cast<ptrdiff_t>(index)};


		for(; distance <= it->distance; ++it, ++distance){
			if(compares_equal(k, it->value)){
				return { {it} , false};
			}
		}


		if(num_slots_minus_one == 0 || distance == max_lookup || num_slots_minus_one*loadfactor < num_elements){
			grow();
			return emplace(std::forward<Key>(k), std::forward<Args>(args)...);
		}

		if(it->is_empty()){
			it->emplace(distance, std::forward<Key>(k), std::forward<Args>(args)...);
			++num_elements;
			return { {it} , true};
		}
		value_type value = {std::forward<Key>(k), std::forward<Args>(args)...};
		it->mswap(distance, value);
		auto _it = it;

		for(++distance, ++it ; ; ++distance, ++it){
			if(distance == max_lookup){
				_it->mswap(distance, value);
				grow();
				return emplace(std::move(value));
			}else if(it->is_empty()){
				it->emplace(distance, std::move(value));
				++num_elements;
				return {{it}, true};
			}else if(it->distance < distance){
				it->mswap(distance, value);
			}
		}
	}

	void grow(){
		ReAlloc(std::max(min_size, static_cast<Hashpolicy&>(*this).next_size(num_slots_minus_one + 1)));
	}

	void ReAlloc(size_t new_capacity) {
		// Not Work for decrease in size
		if(new_capacity < num_slots_minus_one) return;

		int8_t old_lookup = max_lookup;
		max_lookup = compute_max_lookup(new_capacity);


		EntryPointer Nw_Blk = Alloc.allocate(new_capacity + max_lookup);
		--new_capacity;
		std::swap(num_slots_minus_one, new_capacity);
		std::swap(_Blk, Nw_Blk);

		EntryPointer mend {_Blk +  static_cast<ptrdiff_t>(num_slots_minus_one + max_lookup)};
		mend->distance = Entry::special_end_value;

		for(EntryPointer it{_Blk}; it != mend; ++it)
			it->distance = -1;

		for(EntryPointer it{Nw_Blk}, end{it + static_cast<ptrdiff_t>(new_capacity + old_lookup)}; it != end; ++it){
			if(it->has_value()) emplace(std::move(it->value));
		}
		
		// deallocate Data...
		deallocate(Nw_Blk, new_capacity + old_lookup + 1);
	}

	inline void deallocate(EntryPointer it, size_t sz){
		if(it != DefTab) Alloc.deallocate(it, sz);
	}

	iterator begin(){
		EntryPointer it = _Blk;
		for( ; it->is_empty(); ++it);
		return {it};
	}

	iterator begin() const{
		EntryPointer it = _Blk;
		for( ; it->is_empty(); ++it);
		return {it};
	}

	const_iterator cbegin() const{
		return begin();
	}

	iterator end(){
		return {_Blk + static_cast<ptrdiff_t>(num_slots_minus_one + max_lookup)};
	}

	iterator end() const{
		return {_Blk + static_cast<ptrdiff_t>(num_slots_minus_one + max_lookup)};
	}

	const_iterator cend() const{
		return end();
	}

	iterator find(K const & key) {
		int8_t distance = 0;
		for(EntryPointer it = {_Blk + static_cast<ptrdiff_t>(hash_index(hash_key(key), num_slots_minus_one))};
			it->distance >= distance ; ++it, ++distance ){
			if(compares_equal(key, it->value)){
				return it;
			}
		}

		return end();
	}

	iterator count(K const &key) {
		return find() == end() ? 0 : 1;
	}

	bool insert(value_type const & value){
		return emplace(value);
	}

	bool insert(value_type && key){
		return emplace(std::move(key));
	}

	template<typename It>
	void insert(It begin, It end){
		for( ; begin != end ; ++begin) emplace(*begin);
	}

	void insert(std::initializer_list<value_type> il){
		insert(il.begin(), il.end());
	}

	size_t erase(const_iterator it1) {
		EntryPointer it = it1->m_Ptr;
		it->destroy();
		--num_elements;

		for( auto next = it + ptrdiff_t(1); !it->is_at_desired_position() ; ++it, ++next){
			it->emplace(it->distance - 1, std::move(next->value));
			next->destroy_value();
		}

		return 1;
	}

	size_t erase(const K& key){
		auto it = find(key);
		if(it == end()) return 0;
		return erase(it);
	}

	V & operator [] (const K & key) {
		return emplace(key, V()).first->second;
	}

	V & operator [] (K && key) {
		return emplace(std::move(key), V()).first->second;
	}

	V & at (const K & key){
		auto it = find(key);
		assert(it != end());
		return it->second;
	}

	~MyHashMap(){
		deallocate(_Blk, num_slots_minus_one + max_lookup + 1);
	}

};