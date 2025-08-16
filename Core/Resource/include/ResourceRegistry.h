#pragma once

#include <map>
#include <string>

using Key = std::string;

template<class Derived, class Resource>
class ResourceRegistry {

protected:
    std::map<Key, Resource> m_cache;
    void Preload(const Key &key) {

    }
};