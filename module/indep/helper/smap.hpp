/// @file
#pragma once

#include "indep/macro/declThis.hpp"
#include "indep/macro/or.hpp"
#include "indep/macro/sideFunc.hpp"
#include "indep/macro/unconstFunc.hpp"
#include "indep/macro/when.hpp"

#include <map>
#include <unordered_map>

namespace by {

    /** @ingroup indep
     *  @brief Map that iterates in insertion order
     *  @details A standard map iterates in whatever order its own structure imposes -
     *  alphabetical for @c std::map, arbitrary for the unordered ones. This class keeps
     *  the order the elements were inserted in, by threading a doubly linked list through
     *  the stored values.
     *
     *  @section wrapped Choosing what it wraps
     *  The ordering is the substance here; the container underneath is just storage, and
     *  the caller picks it:
     *  @code
     *      smap<std::string, int, std::unordered_multimap> hashed;  // duplicate keys ok
     *      smap<std::string, int, std::map> sorted;                 // unique keys
     *  @endcode
     *  `wrap` is declared inside this class, so a finished map type cannot be passed in.
     *  MAP is taken as a template template parameter instead, bound as `typename...` so
     *  that it accepts both @c std::map (comparator) and @c std::unordered_multimap
     *  (hash plus key equality), whose trailing parameters differ.
     *
     *  @section duplicates Duplicate keys
     *  A container that allows duplicates keeps every insertion as its own element. A
     *  unique key container replaces the value of the element already there and leaves it
     *  at the position it was first inserted at, which is what a caller overwriting a key
     *  expects.
     *
     *  @section usage Usage
     *  @code
     *      smap<std::string, int, std::map> it;
     *      it.insert("zebra", 1);
     *      it.insert("apple", 2);
     *      for(auto e = it.begin(); e != it.end(); ++e)
     *          BY_I("%s", e.getKey()->c_str());   // zebra, apple
     *  @endcode
     *
     *  @remark Where this came from
     *  the core ideas come from Oliver Schönrock and several great developers at
     *  bit.ly/41CwjLL
     */
    template <typename K, typename V, template <typename...> class MAP> class smap {
        typedef smap<K, V, MAP> __me;
        BY(ME(__me))

    public:
        struct wrap;
        class iterator;
        typedef MAP<K, wrap> stlMap;

        struct wrap {
            wrap() = default;
            wrap(const V& newValue);
            wrap(const wrap& rhs);
            wrap(wrap&& rhs);

        public:
            wrap& operator=(const wrap&);
            wrap& operator=(wrap&&);

        public:
            void clear();

        public:
            const K* key = nullptr;
            V value;
            wrap* prev = this;
            wrap* next = this;
        };

        class iterator {
            typedef me owner; // must precede ME(iterator), which rebinds me to this class.
            BY(ME(iterator))

        public:
            iterator(const owner* owner, const wrap* pair, nbool isReversed);
            iterator(const owner* owner, const wrap* pair, nbool isReversed, const K& key);
            iterator(const owner* owner, const wrap* pair, nbool isReversed, const K* key);
            friend owner;

        public:
            V& operator*();
            V* operator->();

            iterator& operator++();
            iterator operator++(int);
            iterator& operator--();
            iterator operator--(int);
            iterator operator+(ncnt step);

            bool isEnd() const;

            const K* getKey() const;
            const V* getVal() const BY_CONST_FUNC(getVal())
            V* getVal();

            bool operator!=(const iterator& rhs) const;
            bool operator==(const iterator& rhs) const;

        private:
            iterator& _step(ncnt step, nbool isReversed);
            static const K& _getDummyKey();

        private:
            const owner* _owner;
            const wrap* _wrap;
            nbool _isReversed;
            K _key;
        };

    public:
        smap() = default;
        // rebuilt element by element: wrap's copy constructor drops prev/next, so copying
        // the container underneath would hand back a severed chain.
        smap(const me& rhs);

    public:
        me& operator=(const me& rhs);

    public:
        ncnt size() const;

        iterator begin() const;
        iterator end() const;
        iterator begin(const K& key) const;
        iterator begin(const K* key) const BY_SIDE_FUNC(key, begin(*key), begin());

        iterator rbegin() const;
        iterator rend() const;
        iterator rbegin(const K& key) const;
        iterator rbegin(const K* key) const BY_SIDE_FUNC(key, rbegin(*key), rbegin());

        void insert(const K& key, const V& val);

        /**
         *  delete all elements matching given key.
         */
        void erase(const K& key);
        void erase(const K* it) BY_SIDE_FUNC(erase);
        void erase(const iterator& it);
        void erase(const iterator* it) BY_SIDE_FUNC(erase);
        void erase(const iterator& from, const iterator& to);
        void erase(const iterator* from, const iterator& to) BY_SIDE_FUNC(from, erase(*from, to), void());
        void erase(const iterator& from, const iterator* to) BY_SIDE_FUNC(to, erase(from, *to), void());
        void erase(const iterator* from, const iterator* to) BY_SIDE_FUNC(from&& to, erase(*from, *to), void());

        iterator find(const K& key) const;

        void clear();

    private:
        void _assign(const me& rhs);
        typename stlMap::iterator _erase(const typename stlMap::iterator& e);
        void _erase(const iterator& e);
        void _link(wrap& newTail);
        void _unlink(wrap& toDelete);
        iterator _begin(const K& key, nbool isReversed) const;

    private:
        stlMap _map;
        wrap _end;
    };

    /** @ingroup indep
     *  @brief smap over @c std::unordered_multimap, the shape byeol's closure capture uses.
     */
    template <typename K, typename V> using smultimap = smap<K, V, std::unordered_multimap>;
} // namespace by
