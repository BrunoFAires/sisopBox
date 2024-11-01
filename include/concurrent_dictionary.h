#include <iostream>
#include <mutex>
#include <unordered_map>
#include <shared_mutex>
#include <array>

using namespace std;

template <typename K, typename V>
class concurrent_dictionary {
private:
    unordered_map<K, V> dictionary;
    mutable shared_mutex mutex;

public:
    void insert_or_update(const K& key, const V& value) {
        unique_lock lock(mutex);
        dictionary[key] = value;
    }

    bool remove(const K& key) {
        unique_lock lock(mutex);
        return dictionary.erase(key) > 0;
    }

    V get(const K& key) const {
        shared_lock lock(mutex);
        if (auto it = dictionary.find(key); it != dictionary.end()) {
            return it->second;
        }
        return V(); // Return default value if key not found
    }

    bool contains(const K& key) const {
        shared_lock lock(mutex);
        return dictionary.count(key) > 0;
    }

    void clear() {
        unique_lock lock(mutex);
        dictionary.clear();
    }
};