#ifndef HASHTABLE_H
#define HASHTABLE_H

// Project Identifier: 2C4A3C53CD5AD45A7BEA3AE5356A3B1622C9D04B

// INSTRUCTIONS:
// fill out the methods in the class below.

// You may assume that the key and value types can be copied and have default
// constructors.

// You can also assume that the key type has (==) and (!=) methods.

// You may assume that Hasher is a functor type with a
// size_t operator()(const Key&) overload.

// The key/value types aren't guaranteed to support any other operations.

// Do not add, remove, or change any of the class's member variables.
// The num_deleted counter is *optional*. You might find it helpful, but it
// is not required to pass the lab assignment.

// Do not change the Bucket type.

// SUBMISSION INSTRUCTIONS:
// Submit this file, by itself, in a .tar.gz.
// Other files will be ignored.
//#include <iostream>
#include <cstdint>
#include <functional> // where std::hash lives
#include <vector>
#include <cassert> // useful for debugging!
//using namespace std;
// A bucket's status tells you whether it's empty, occupied,
// or contains a deleted element.
enum class Status : uint8_t {
    Empty,
    Occupied,
    Deleted
};

template<typename K, typename V, typename Hasher = std::hash<K>>
class HashTable {
    // used by the autograder; do not change/remove.
    friend class Verifier;
public:
    // A bucket has a status, a key, and a value.
    struct Bucket {
        // Do not modify Bucket.
        Status status = Status::Empty;
        K key;
        V val;
    };

    HashTable() {
        // TODO: a default constructor (possibly empty).

        // You can use the following to avoid implementing rehash_and_grow().
        // However, you will only pass the AG test cases ending in _C.
        // To pass the rest, start with at most 20 buckets and implement rehash_and_grow().
            buckets.resize(20);
    }

    size_t size() const {
        return num_elements;
    }

    // returns a reference to the value in the bucket with the key, if it
    // already exists. Otherwise, insert the value with a default value, and return
    // a reference to the resulting bucket.
    V& operator[](const K& key) {
        Hasher hasher;
        if((num_elements*2 + 1) >= buckets.size()){
            rehash_and_grow();
        }
        size_t target;
        size_t firstDelete;
        bool metDeleted = false;
        for(size_t i = 0; i < buckets.size(); i++){
            target = (hasher(key)+i)%buckets.size();//should wrap around!
            if(buckets[target].status == Status::Occupied && buckets[target].key == key){
                return buckets[target].val;
            }
            if(buckets[target].status == Status::Deleted && !metDeleted){
                firstDelete = target;
                metDeleted = true;
            }
            if(buckets[target].status == Status::Empty){
                if(metDeleted){
                    target = firstDelete;
                }
                buckets[target].key = key;
                buckets[target].val = V();
                buckets[target].status = Status::Occupied;
                num_elements++;
                return buckets[target].val;
            }
        }
        if(metDeleted){ //everything was deleted or not the right key
            buckets[firstDelete].key = key;
            buckets[firstDelete].val = V();
            buckets[firstDelete].status = Status::Occupied;
            num_elements++;
            return buckets[firstDelete].val;
        }
        return buckets[target].val; //this will literally never be called
    }

    // insert returns whether inserted successfully
    // (if the key already exists in the table, do nothing and return false).
    bool insert(const K& key, const V& val) {
        Hasher hasher;
        if((num_elements*2 + 1) >= buckets.size()){
            rehash_and_grow();
        }
        size_t firstDelete;
        bool metDeleted = false;
        for(size_t i = 0; i < buckets.size(); i++){
            size_t target = (hasher(key)+i)%buckets.size();//should wrap around!
            if(buckets[target].status == Status::Occupied && buckets[target].key == key){
                return false;
            }
            if(buckets[target].status == Status::Deleted && !metDeleted){
                firstDelete = target;
                metDeleted = true;
            }
            if(buckets[target].status == Status::Empty){
                if(metDeleted){
                    target = firstDelete;
                }
                buckets[target].key = key;
                buckets[target].val = val;
                buckets[target].status = Status::Occupied;
                num_elements++;
                return true;
            }
        }
        if(metDeleted){ //everything was deleted or not the right key
            buckets[firstDelete].key = key;
            buckets[firstDelete].val = val;
            buckets[firstDelete].status = Status::Occupied;
            num_elements++;
            return true;
        }
        return false;
    }

    size_t erase(const K& key) {
        Hasher hasher;
        for(size_t i=0; i<buckets.size(); i++){
            size_t target = (hasher(key)+i)%buckets.size();
            if(buckets[target].status == Status::Empty)
                return 0;
            if(buckets[target].status == Status::Occupied && buckets[target].key == key){
                buckets[target].status = Status::Deleted;
                num_elements--;
                return 1;
            }
        }// as soon as empty, deletus
        return 0;
    }

private:
    size_t num_elements = 0;
    size_t num_deleted = 0; // OPTIONAL: you don't need to use num_deleted to pass
    std::vector<Bucket> buckets;

    void rehash_and_grow() {
        std::vector<Bucket> temp = buckets;
        buckets.clear();
        buckets.resize(temp.size()*2);
        num_elements = 0;
        
        for(size_t i=0; i<temp.size(); i++){
            if(temp[i].status == Status::Occupied){
                insert(temp[i].key,temp[i].val);
            }
        }
        // You can avoid implementing rehash_and_grow() by calling
        //    buckets.resize(10000);
        // in the constructor. However, you will only pass the AG test cases ending in _C.
        // To pass the rest, start with at most 20 buckets and implement rehash_and_grow().
    }
    // You can add methods here if you like.
    // TODO
};

#endif // HASHTABLE_H
