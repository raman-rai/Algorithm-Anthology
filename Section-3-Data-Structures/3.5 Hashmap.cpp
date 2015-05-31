/*

3.5 - Hashmap

Description: A hashmap is an alternative to binary search trees. Hashmaps
use more memory than BSTs, but are usually more efficient. In C++11, the
built in hashmap is known as std::unordered_map. This implementation uses
the chaining method to handle collisions. Three integer hash algorithms
and one string hash algorithm are presented here.

Time Complexity: insert(), remove(), find(), are O(1) amortized.
rehash() is O(N).

Space Complexity: O(N) on the number of entries.

*/

#include <list>

template<class key_t, class val_t, class Hash> class hashmap {
  struct entry_t {
    key_t key;
    val_t val;
    entry_t(const key_t & k, const val_t & v): key(k), val(v) {}
  };

  std::list<entry_t> * table;
  int table_size, map_size;

  /**
   * This doubles the table size, then rehashes every entry.
   * Rehashing is expensive; it is strongly suggested for the
   * table to be constructed with a large size to avoid rehashing.
   */
  void rehash() {
    std::list<entry_t> * old = table;
    int old_size = table_size;
    table_size = 2*table_size;
    table = new std::list<entry_t>[table_size];
    map_size = 0;
    typename std::list<entry_t>::iterator it;
    for (int i = 0; i < old_size; i++)
      for (it = old[i].begin(); it != old[i].end(); ++it)
        insert(it->key, it->val);
    delete[] old;
  }

 public:
  hashmap(int size = 1024): table_size(size), map_size(0) {
    table = new std::list<entry_t>[table_size];
  }

  ~hashmap() { delete[] table; }
  int size() const { return map_size; }

  void insert(const key_t & key, const val_t & val) {
    if (find(key) != 0) return;
    if (map_size >= table_size) rehash();
    unsigned int i = Hash()(key) % table_size;
    table[i].push_back(entry_t(key, val));
    map_size++;
  }

  void remove(const key_t & key) {
    unsigned int i = Hash()(key) % table_size;
    typename std::list<entry_t>::iterator it = table[i].begin();
    while (it != table[i].end() && it->key != key) ++it;
    if (it == table[i].end()) return;
    table[i].erase(it);
    map_size--;
  }

  val_t* find(const key_t & key) {
    unsigned int i = Hash()(key) % table_size;
    typename std::list<entry_t>::iterator it = table[i].begin();
    while (it != table[i].end() && it->key != key) ++it;
    if (it == table[i].end()) return 0;
    return &(it->val);
  } 

  inline val_t& operator[] (const key_t & key) {
    val_t * ret = find(key);
    if (ret != 0) return *ret;
    insert(key, val_t());
    return *find(key);
  }
};

/*** Example Usage: ***/

#include <iostream>
using namespace std;

struct class_hash {
  unsigned int operator() (int key) {
    /* Knuth’s multiplicative method (one-to-one) */
    return key * 2654435761u;
  }
  
  unsigned int operator() (unsigned int key) {
    /* Robert Jenkins' 32-bit mix (one-to-one) */
    key = ~key + (key << 15);
    key = key ^ (key >> 12);
    key = key + (key << 2);
    key = (key ^ (key >> 4)) * 2057;
    return key ^ (key >> 16);
  }
  
  unsigned int operator() (unsigned long long key) {
    key = (~key) + (key << 18);
    key = (key ^ (key >> 31)) * 21;
    key = key ^ (key >> 11);
    key = key + (key << 6);
    return key ^ (key >> 22);
  }
  
  unsigned int operator() (const std::string &key) {
    /* Jenkins' one-at-a-time hash */
    unsigned int hash = 0;
    for (unsigned int i = 0; i < key.size(); i++) {
      hash += ((hash += key[i]) << 10);
      hash ^= (hash >> 6);
    }
    hash ^= ((hash += (hash << 3)) >> 11);
    return hash + (hash << 15);
  }
};

int main() {
  hashmap<string, int, class_hash> M;
  M["foo"] = 1;
  M.insert("bar", 2);
  cout << M["foo"] << M["bar"] << endl; //prints 12
  cout << M["baz"] << M["qux"] << endl; //prints 00
  M.remove("foo");
  cout << M.size() << endl;             //prints 3
  cout << M["foo"] << M["bar"] << endl; //prints 02
  return 0;
} 