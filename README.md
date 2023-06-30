# RobinHood-HashMap

[![Last Commit](https://img.shields.io/github/last-commit/VedanshuGoyal/RobinHood-HashMap)](https://github.com/VedanshuGoyal/RobinHood-HashMap/commits/main)

## Project Description

RobinHood-HashMap is a custom C++ library that implements Robin Hood hashing and the SplittableRandom generator to optimize HashMap operations. It provides a high-performance HashMap data structure that surpasses the traditional C++ unordered map by a factor of 4 in most cases. By improving the lookup, insertion, and deletion efficiency, RobinHood-HashMap enhances the overall performance of HashMap operations, resulting in faster data retrieval and enhanced application performance.

## Installation

To use RobinHood-HashMap in your C++ project, follow these steps:

1. Clone the repository:

   ```shell
   $ git clone https://github.com/VedanshuGoyal/RobinHood-HashMap.git
   $ cd RobinHood-HashMap
   
2. Include the necessary header files in your project:
   ```shell
   #include "robinhood.hpp"

4. Create an instance of the HashMap:
   ```shell
   rhmap::Hashmap<KeyType, ValueType> hashmap;

5. To Perform HashMap operations, such as insertion, lookup, and deletion it is the same API as the C++ STL map library.

Leverage the HashMap in your application, taking advantage of its improved performance over the standard unordered map.
