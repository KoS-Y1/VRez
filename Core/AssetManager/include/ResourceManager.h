#pragma once

#include <map>
#include <utility>

template<class Derived, class Key, class Resource>
class ResourceManager {
protected:
    std::map<Key, Resource *> m_cache;

    template<class... Args>
    Resource *Load(const Key &key, const Args &&...args) {
        auto pair = m_cache.find(key);

        if (pair == m_cache.end()) {
            Resource resource = static_cast<Derived *>(this)->CreateResource(key, std::forward<Args>(args)...);
            pair              = m_cache.emplace(key, std::move(resource)).first;
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