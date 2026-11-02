#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include "SwissMap.h"

TEST_CASE("SwissMap inserts and finds correctly") {
    ki::swiss_map<int, int> map;
    
    SUBCASE("Inserting a new key returns true") {
        auto pair = map.Emplace(42, 100);
        CHECK(pair != nullptr);
        CHECK(map.Size() == 1);
    }
    
    SUBCASE("Finding an existing key returns the value") {
        map.Emplace(42, 100);
        auto* slot = map.Find(42);
        REQUIRE(slot != nullptr);
        CHECK(*slot == 100);
    }
    
    SUBCASE("Finding a missing key returns nullptr") {
        auto* slot = map.Find(999);
        CHECK(slot == nullptr);
    }
}

TEST_CASE("SwissMap handles rehash gracefully") {
    ki::swiss_map<int, int> map;
    for (int i = 0; i < 100000; ++i) {
        auto* Pair = map.Emplace(i, i * 2);
        CHECK(Pair != nullptr);
    }
    // Ensure all are still findable after rehash.
    for (int i = 0; i < 100000; ++i) {
        auto* slot = map.Find(i);
        REQUIRE(slot != nullptr);
        CHECK(*slot == i * 2);
    }
}

TEST_CASE("SwissMap handles erase correctly") {
    ki::swiss_map<int, int> map;
    for (int i = 0; i < 100000; ++i) {
        auto* Pair = map.Emplace(i, i * 2);
        CHECK(Pair != nullptr);
    }
    // Ensure erasing elements doesn't lead to false finds
    for (int i = 0; i < 100000; ++i) {
        map.Erase(i);
        auto* Pair = map.Find(i);
        REQUIRE(Pair == nullptr);
    }
}

TEST_CASE("SwissMap handles duplicate key insertion") {
    ki::swiss_map<int, int> map;
    auto* slot1 = map.Emplace(42, 100);
    REQUIRE(slot1 != nullptr);
    CHECK(slot1->second == 100);

    auto* slot2 = map.Emplace(42, 200);
    // Should return existing slot
    CHECK(slot2 != nullptr);
    // Value should change
    CHECK(slot2->second == 200);
    CHECK(map.Size() == 1);
}

TEST_CASE("SwissMap erase on empty map does nothing") {
    ki::swiss_map<int, int> map;
    map.Erase(42);
    CHECK(map.Size() == 0);
    CHECK(map.Find(42) == nullptr);
}

TEST_CASE("SwissMap erase missing key does nothing") {
    ki::swiss_map<int, int> map;
    map.Emplace(1, 10);
    map.Erase(999);
    CHECK(map.Size() == 1);
    CHECK(map.Find(1) != nullptr);
}

TEST_CASE("SwissMap rehash stability under high load") {
    ki::swiss_map<int, int> map;
    const int N = 10000;
    for (int i = 0; i < N; ++i) {
        auto* slot = map.Emplace(i, i * 3);
        REQUIRE(slot != nullptr);
        CHECK(slot->second == i * 3);
    }
    // Verify all elements are still findable
    for (int i = 0; i < N; ++i) {
        auto* slot = map.Find(i);
        REQUIRE(slot != nullptr);
        CHECK(*slot == i * 3);
    }
    // Erase half and check again
    for (int i = 0; i < N / 2; ++i) {
        map.Erase(i);
    }
    for (int i = 0; i < N / 2; ++i) {
        CHECK(map.Find(i) == nullptr);
    }
    for (int i = N / 2; i < N; ++i) {
        auto* slot = map.Find(i);
        REQUIRE(slot != nullptr);
        CHECK(*slot == i * 3);
    }
}

TEST_CASE("SwissMap move constructor") {
    ki::swiss_map<int, int> map1;
    map1.Emplace(1, 10);
    map1.Emplace(2, 20);

    ki::swiss_map<int, int> map2(std::move(map1));
    // Source is empty
    CHECK(map1.Size() == 0);
    CHECK(map2.Size() == 2);
    CHECK(map2.Find(1) != nullptr);
    CHECK(map2.Find(2) != nullptr);
}

TEST_CASE("SwissMap move assignment") {
    ki::swiss_map<int, int> map1;
    map1.Emplace(1, 10);
    map1.Emplace(2, 20);

    ki::swiss_map<int, int> map2;
    map2.Emplace(3, 30);

    map2 = std::move(map1);
    CHECK(map1.Size() == 0);
    CHECK(map2.Size() == 2);
    CHECK(map2.Find(1) != nullptr);
    CHECK(map2.Find(2) != nullptr);
    // old element gone
    CHECK(map2.Find(3) == nullptr); 
}

// TODO: Add copy constructor
/*TEST_CASE("SwissMap copy constructor") {
    ki::swiss_map<int, int> map1;
    map1.Emplace(1, 10);

    ki::swiss_map<int, int> map2(map1);
    CHECK(map2.Size() == 1);
    CHECK(map2.Find(1) != nullptr);
    // Modify map1, ensure map2 unchanged
    map1.Emplace(2, 20);
    CHECK(map1.Size() == 2);
    CHECK(map2.Size() == 1);
}*/

TEST_CASE("SwissMap const find") {
    ki::swiss_map<int, int> map;
    map.Emplace(1, 10);
    const auto& constMap = map;
    const auto* slot = constMap.Find(1);
    CHECK(slot != nullptr);
    CHECK(*slot == 10);
}

#include <random>
#include <unordered_set>

TEST_CASE("SwissMap random stress test") {
    ki::swiss_map<int, int> map;
    std::unordered_set<int> keys;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(1, 10000);

    const int NUM_OPS = 100000;
    for (int i = 0; i < NUM_OPS; ++i) {
        int key = dist(gen);
        int val = dist(gen);
        // Randomly decide insert, find, or erase
        int op = dist(gen) % 3;
        if (op == 0) { // Insert
            auto* slot = map.Emplace(key, val);
            keys.insert(key);
            CHECK(slot != nullptr);
        } else if (op == 1) { // Find
            auto* slot = map.Find(key);
            // Did we add this key before?
            if (keys.find(key) != keys.end()) {
                // We did add it. 
                // Crash means FHM didn't add it or couldn't find it
                CHECK(slot != nullptr);
            } else {
                // We didn't add it. 
                // Crash means FHM falsely added/found it
                CHECK(slot == nullptr);
            }
        } else { // Erase
            map.Erase(key);
            keys.erase(key);
        }
    }
    // Verify final state
    for (int k : keys) {
        CHECK(map.Find(k) != nullptr);
    }
    // Ensure no extra keys
    CHECK(map.Size() == keys.size());
}

TEST_CASE("SwissMap with complex value type") {
    ki::swiss_map<int, std::string> map;
    map.Emplace(1, "hello");
    auto* slot = map.Find(1);
    REQUIRE(slot != nullptr);
    CHECK(*slot == "hello");
}

TEST_CASE("SwissMap iteration over elements") {
    ki::swiss_map<int, int> map;
    map.Emplace(1, 10);
    map.Emplace(2, 20);
    int count = 0;
    for (const auto& [key, val] : map) {
        ++count;
        // Check that key exists
        CHECK(map.Find(key) != nullptr);
    }
    CHECK(count == 2);
}

TEST_CASE("SwissMap clear removes all elements") {
    ki::swiss_map<int, int> map;
    map.Emplace(1, 10);
    map.Emplace(2, 20);
    map.Clear();
    CHECK(map.Size() == 0);
    CHECK(map.Find(1) == nullptr);
    CHECK(map.Find(2) == nullptr);
    // Ensure you can still insert after clear
    map.Emplace(3, 30);
    CHECK(map.Size() == 1);
    CHECK(map.Find(3) != nullptr);
}

TEST_CASE("SwissMap with move-only value type") {
    ki::swiss_map<int, std::unique_ptr<int>> map;
    auto ptr = std::make_unique<int>(42);
    map.Emplace(1, std::move(ptr));
    auto* slot = map.Find(1);
    REQUIRE(slot != nullptr);
    CHECK(**slot == 42);
    // ptr should be null now (moved)
    CHECK(ptr == nullptr);
}