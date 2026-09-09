#pragma once

#include "indep/helper/smap.hpp"

namespace by {

#define TEMPL template <typename K, typename V, template <typename...> class MAP>
#define ME smap<K, V, MAP>

    TEMPL
    ME::wrap::wrap(const V& newValue): value(newValue) {}

    TEMPL
    ME::wrap::wrap(const wrap& rhs): key(rhs.key), value(rhs.value) {}

    TEMPL
    ME::wrap::wrap(wrap&& rhs): key(rhs.key), value(rhs.value) {}

    TEMPL
    void ME::wrap::clear() { prev = next = this; }

    TEMPL
    typename ME::wrap& ME::wrap::operator=(const wrap&) { return *this; }

    TEMPL
    typename ME::wrap& ME::wrap::operator=(wrap&&) { return *this; }

    TEMPL
    const K& ME::iterator::_getDummyKey() {
        static K inner;
        return inner;
    }

    TEMPL
    ME::iterator::iterator(const owner* owner, const wrap* pair, nbool isReversed):
        me(owner, pair, isReversed, nullptr) {}

    TEMPL
    ME::iterator::iterator(const owner* owner, const wrap* pair, nbool isReversed, const K& key):
        _owner(owner), _wrap(pair), _isReversed(isReversed), _key(key) {}

    TEMPL
    ME::iterator::iterator(const owner* owner, const wrap* pair, nbool isReversed, const K* key):
        _owner(owner), _wrap(pair), _isReversed(isReversed), _key(key ? *key : _getDummyKey()) {}

    TEMPL
    V& ME::iterator::operator*() { return *getVal(); }

    TEMPL
    V* ME::iterator::operator->() { return getVal(); }

    TEMPL
    typename ME::iterator& ME::iterator::operator++() {
        WHEN_NUL(_owner).ret(*this);
        return _step(1, false);
    }

    TEMPL
    typename ME::iterator& ME::iterator::_step(ncnt step, nbool isReversed) {
        const K* key = nullptr;
        for(ncnt n = 0; n < step; ++n)
            do {
                _wrap =
                    isReversed ? (_isReversed ? _wrap->next : _wrap->prev) : (_isReversed ? _wrap->prev : _wrap->next);
                if(_key == _getDummyKey()) break;
                key = getKey();
                if(!key) return *this;

            } while(*key != _key);
        return *this;
    }

    TEMPL
    typename ME::iterator ME::iterator::operator++(int) {
        iterator ret = *this;
        operator++();
        return ret;
    }

    TEMPL
    typename ME::iterator& ME::iterator::operator--() {
        WHEN_NUL(_owner).ret(*this);
        return _step(1, true);
    }

    TEMPL
    typename ME::iterator ME::iterator::operator--(int) {
        iterator ret = *this;
        operator--();
        return ret;
    }

    TEMPL
    typename ME::iterator ME::iterator::operator+(ncnt step) {
        iterator ret = *this;
        ret._step(step, false);
        return ret;
    }

    TEMPL
    bool ME::iterator::isEnd() const { return _wrap == &_owner->_end; }

    TEMPL
    const K* ME::iterator::getKey() const { return _wrap->key; }

    TEMPL
    V* ME::iterator::getVal() {
        WHEN_NUL(_wrap).ret(nullptr);
        return (V*) &_wrap->value;
    }

    TEMPL
    bool ME::iterator::operator!=(const iterator& rhs) const { return _wrap != rhs._wrap; }

    TEMPL
    bool ME::iterator::operator==(const iterator& rhs) const { return _wrap == rhs._wrap; }

    TEMPL
    ME::smap(const me& rhs) { _assign(rhs); }

    TEMPL
    ME& ME::operator=(const me& rhs) {
        WHEN(this == &rhs) .ret(*this);

        clear();
        _assign(rhs);
        return *this;
    }

    TEMPL
    void ME::_assign(const me& rhs) {
        for(auto e = rhs.begin(); e != rhs.end(); ++e) {
            const K& key = e.getKey() OR_CONTINUE;
            insert(key, *e);
        }
    }

    TEMPL
    ncnt ME::size() const { return _map.size(); }

    TEMPL
    typename ME::iterator ME::begin() const { return iterator(this, _end.next, false); }

    TEMPL
    typename ME::iterator ME::end() const { return iterator(this, &_end, false); }

    TEMPL
    typename ME::iterator ME::begin(const K& key) const { return _begin(key, false); }

    TEMPL
    typename ME::iterator ME::rbegin() const { return iterator(this, _end.prev, true); }

    TEMPL
    typename ME::iterator ME::rend() const { return iterator(this, &_end, true); }

    TEMPL
    typename ME::iterator ME::rbegin(const K& key) const { return _begin(key, true); }

    TEMPL void ME::insert(const K& key, const V& val) {
        ncnt before = _map.size();
        auto at = _map.insert(_map.end(), typename stlMap::value_type(key, wrap(val)));
        WHEN(at == _map.end()) .ret();

        // the hint overload answers both container shapes with a bare iterator, but drops
        // the bool saying whether it inserted. growing is what says it instead.
        if(_map.size() == before) {
            // a refused duplicate. replacing the value keeps the element where it was,
            // and linking it again would splice it after itself.
            at->second.value = val;
            return;
        }

        at->second.key = &at->first;
        _link(at->second);
    }

    TEMPL
    void ME::erase(const K& key) {
        auto range = _map.equal_range(key);
        for(auto e = range.first; e != range.second;)
            e = _erase(e);
    }

    TEMPL
    void ME::erase(const iterator& it) {
        const auto& key = it.getKey() OR.ret();
        auto range = _map.equal_range(key);

        for(auto e = range.first; e != range.second; ++e)
            if(&(e->second) == it._wrap) return _erase(e), void();
        // not found.
    }

    TEMPL
    void ME::erase(const iterator& from, const iterator& to) {
        for(auto e = from; e != to && e != end();)
            _erase(e++);
    }

    TEMPL
    typename ME::iterator ME::find(const K& key) const { return _begin(key, false); }

    TEMPL
    void ME::clear() {
        _end.clear();
        _map.clear();
    }

    TEMPL
    typename ME::stlMap::iterator ME::_erase(const typename stlMap::iterator& e) {
        WHEN(e == _map.end()) .ret(_map.end()); // not found.

        _unlink(e->second);
        return _map.erase(e);
    }

    TEMPL
    void ME::_erase(const iterator& e) {
        WHEN(e.isEnd()) .ret(); // not found.

        const K& key = e._wrap->key OR.ret();
        auto range = _map.equal_range(key);
        for(auto stlE = range.first; stlE != range.second; ++stlE)
            if(&stlE->second == e._wrap) return _erase(stlE), void();
    }

    TEMPL
    void ME::_link(wrap& newTail) {
        newTail.next = &_end;
        auto*& curTail = _end.prev;
        curTail->next = &newTail;
        newTail.prev = curTail;
        curTail = &newTail;
    }

    TEMPL
    void ME::_unlink(wrap& toDelete) {
        toDelete.prev->next = toDelete.next;
        toDelete.next->prev = toDelete.prev;
    }

    TEMPL
    typename ME::iterator ME::_begin(const K& key, nbool isReversed) const {
        auto ret = iterator(this, &_end, isReversed, key);
        ++ret;
        return ret;
    }

#undef TEMPL
#undef ME
} // namespace by
