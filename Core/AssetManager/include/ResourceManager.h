#pragma once

#include <map>
#include <mutex>
#include <utility>

#include <Debug.h>

template<class Derived, class Key, class Resource>
class ResourceManager {
protected:

    template<class... Args>
    Resource *Load(const Key &key, Args &&...args) {
        auto pair = m_cache.find(key);

        DEBUG_ASSERT(pair != m_cache.end());

        return &pair->second;
    }

    template<class... Args>
    void Preload(const Key &key, Args &&...args) {
        Resource resource = static_cast<Derived *>(this)->CreateResource(key, std::forward<Args>(args)...);

        {
            std::scoped_lock<std::mutex> lk(m_cacheMutex);
            m_cache.emplace(key, std::move(resource));
        }
    }

    void DestroyAll() {
        for (auto &[key, value]: m_cache) {
            value.Destroy();
        }

        m_cache.clear();
    }

    ResourceManager()  = default;
    ~ResourceManager() = default;

private:
    std::map<Key, Resource> m_cache;
    std::mutex m_cacheMutex;
};