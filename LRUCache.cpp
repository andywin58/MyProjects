// Design a data structure that follows the constraints of a Least Recently Used
// (LRU) cache.

// Implement the LRUCache class:

// LRUCache(int capacity) Initialize the LRU cache with positive size capacity.
// int get(int key) Return the value of the key if the key exists, otherwise
// return -1.
// void put(int key, int value) Update the value of the key if the key exists.
// Otherwise, add the key-value pair to the cache. If the number of keys exceeds
// the capacity from this operation, evict the least recently used key.
// The functions get and put must each run in O(1) average time complexity.

// This implementation passed 20/22 and failed with a time limit exceeded.

// Example 1:
// Input
// ["LRUCache", "put", "put", "get", "put", "get", "put", "get", "get", "get"]
// [[2], [1, 1], [2, 2], [1], [3, 3], [2], [4, 4], [1], [3], [4]]
// Output
// [null, null, null, 1, null, -1, null, -1, 3, 4]

// Explanation
// LRUCache lRUCache = new LRUCache(2);
// lRUCache.put(1, 1); // cache is {1=1}
// lRUCache.put(2, 2); // cache is {1=1, 2=2}
// lRUCache.get(1);    // return 1
// lRUCache.put(3, 3); // LRU key was 2, evicts key 2, cache is {1=1, 3=3}
// lRUCache.get(2);    // returns -1 (not found)
// lRUCache.put(4, 4); // LRU key was 1, evicts key 1, cache is {4=4, 3=3}
// lRUCache.get(1);    // return -1 (not found)
// lRUCache.get(3);    // return 3
// lRUCache.get(4);    // return 4
 
// Constraints:
// 1 <= capacity <= 3000
// 0 <= key <= 104
// 0 <= value <= 105
// At most 2 * 105 calls will be made to get and put.


class LRUCache {
public:
    explicit LRUCache(int capacity) {
        if (capacity > 0) {
             cap = capacity;
             mymap.reserve(cap+1);
        }
    }
    
    int get(int key) {
        int rtn = -1;
        auto mapfind = mymap.find(key);
        if (mapfind != mymap.end()) {
            updateLRU(mapfind->first);
            rtn = mapfind->second;
        }
        return rtn;
    }
    
    void put(int key, int value) {
        // insert or replace
        mymap[key] = value;
        if (mymap.size() > cap) {
        //    int evictkey = getLRU();
            mymap.erase(getLRU()); //evictkey);
        //    updateLRU(key);
        }
        updateLRU(key);
    }

    private:
    int cap = 1;
    // This is probably the problem regarding efficiency,
    // maintaining 2 containers. Try list<pair<key, value> >.
    std::unordered_map<int, int> mymap;
    std::list<int> usedkeys;

    void updateLRU(int akey) {
        usedkeys.remove_if([akey](const int value) {
            return (value == akey); });
        // put it at the head
        usedkeys.push_front(akey);
        /* */
        if (usedkeys.size() > cap) {
            usedkeys.pop_back();
        }
    }

    int getLRU() {
      //  int rtn = usedkeys.back();
      //  usedkeys.pop_back();
        int rtn = -1;
        if (usedkeys.size() > 0) {
            rtn = usedkeys.back();
            usedkeys.pop_back();
        }
        return rtn;
    }
};

/**
 * Your LRUCache object will be instantiated and called as such:
 * LRUCache* obj = new LRUCache(capacity);
 * int param_1 = obj->get(key);
 * obj->put(key,value);
 */
