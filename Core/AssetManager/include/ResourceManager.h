#pragma once

#include <map>
#include <utility>

template<class Derived, class Key, class Resource>
class ResourceManager {
protected:
    std::map<Key, Resource> m_cache;

    template<class... Args>
    Resource *Load(const Key &key, Args &&...args) {
        auto pair = m_cache.find(key);

        if (pair == m_cache.end()) {
            pair = m_cache.emplace(key, static_cast<Derived *>(this)->CreateResource(key, std::forward<Args>(args)...)).first;
        }

        return &pair->second;
    }

    void DestroyAll() {
        for (auto &[key, value]: m_cache) {
            value.Destroy();
        }

        m_cache.clear();
    }

    ResourceManager()  = default;
    ~ResourceManager() = default;
};