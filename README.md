# Flat Hash Map
Performant and tested implementation of a swiss table flat hash map.

## Performance
Benchmark Results (FlatHashMap vs std::unordered_map)

Hardware: Ryzen 5 2600 (L1: 93KB, L2: 512KB, L3: 16MB)

| **Size** | **Container**        | **Insert (ns)** | **Find (ns)** | **Iterate (ns)** | **Delete (ns)** | **Live Memory (bytes)** |
|----------|----------------------|----------------|---------------|------------------|----------------|-------------------------|
| **256**  | `std::unordered_map` | 28,747         | 669           | 312              | 1,250          | 16,416                  |
|          | `FlatHashMap`        | 21,551         | 853           | 319              | 600            | 8,720                   |
|          |                      | **-25%**       | **+27%**      | **+2%** (tie)    | **-52%**       | **−47%**                |
| **2048** | `std::unordered_map` | 193,881        | 6,978         | 6,324            | 17,880         | 98,336                  |
|          | `FlatHashMap`        | 145,560        | 7,347         | 3,624            | 4,898          | 69,648                  |
|          |                      | **-25%**       | **+5%** (tie) | **-43%**         | **-73%**       | **−29%**                |
| **32768**| `std::unordered_map` | 3,055,656      | 270,024       | 255,039          | 434,301        | 1,572,896               |
|          | `FlatHashMap`        | 3,086,654      | 171,614       | 72,039           | 103,444        | 1,114,128               |
|          |                      | **−1%** (tie)  | **-36%**      | **-72%**         | **-76%**       | **−29%**                |

## Usage
#### Include
```c++
#include <FlatHashMap.h>
```

#### Interface
```c++
FlatHashMap<KeyType,
            ValueType,
   OPTIONAL AllocatorType,
   OPTIONAL HashFunction,
   OPTIONAL KeyEqualTo>
```

#### Insert
Insert only inserts new pair if none found and fails if it already exists
```c++
FlatHashMap<uint64_t, std::string> map;

auto* slot1 = map.Insert(42, "Test"); // {key: 42, value: "Test"}

auto* slot2 = map.Insert(42, "Buzz"); // nullptr
```

#### Emplace
Emplace inserts new pair if none found or replaces value if key exists
```c++
FlatHashMap<uint64_t, std::string> map;

map.Emplace(42, "Test");
auto* value1 = map.Find(42); // {key: 42, value: "Test"}
map.Emplace(42, "Buzz");
auto* value2 = map.Find(42); // {key: 42, value: "Buzz"}
```

#### Find
Find key and returns its value if found or nullptr if not found
```c++
FlatHashMap<uint64_t, std::string> map;

map.Emplace(42, "Test");
auto* value1 = map.Find(42); // {key: 42, value: "Test"}
auto* value2 = map.Find(43); // nullptr
```

#### Iterate
FlatHashMap supports iterations and constant iterators
```c++
FlatHashMap<uint64_t, std::string> map;

map.Emplace(42, "Test");
map.Emplace(5, "Fizz");
map.Emplace(9283, "Buzz");

for (auto* it : map) {
  printf("%s\n", it->second);
};
```

#### Erase
FlatHashMap supports erasing keys and iterators
Erasing using an iterator invalidates iterator, but Erase() method returns next valid iterator
```c++
FlatHashMap<uint64_t, std::string> map;

map.Emplace(42, "Test");
map.Emplace(5, "Fizz");
map.Emplace(9283, "Buzz");

map.Erase(42); // Removes "Test"
auto* it = map.begin(); // Points to "Fizz"
it = map.Erase(it); // Removes "Fizz" and returns iterator pointing to "Buzz"
it = map.Erase(it); // Removes "Buzz" and returns iterator pointing to map.end()
```

## Use with CMake
```cmake
# FetchContent or external directory
add_executable(myApplication ...)
# This automatically provides include directory
target_link_libraries(myApplication PRIVATE FlatHashMap)
```

## Run tests
```sh
mkdir build
cd build
cmake -G Ninja -DENABLE_TEST ..
Ninja
./tests/FlatHashMapTests
```
