#pragma once
// #include <functional>
// #include <cstddef>
#include <bits/stdc++.h>

/*Hash Policy*/
struct poweroftwo{
	size_t start_size = 2;
	size_t next_size() = 0;
	
};

struct Iterator{

};

template<typename K, typename V, typename H = std::hash<K>, typename E = std::equal_to<K>, typename A = std::allocator<std::pair<K, V>>, typename Hp = poweroftwo>
class MyHashMap{

	struct Bucket
	{
		int8_t distance = -1;
		K _key;
		V _value;
		Bucket(const K &key, const V &value) : _key(key), _value(value) {}
	};

	using Alloc = typename std::allocator_traits<A>::rebind_alloc<Bucket>;

public:
	MyHashMap() {
		
	}

};