#pragma once

#include <map>
#include <mutex>
#include <utility>
#include <vector>

template<class Derived, class Key, class Resource>
class ResourceManager {
protected:

    template<class... Args>
    Resource *Load(const Key &key, Args &&...args) {
        auto pair = m_cache.find(key);

        if (pair == m_cache.end()) {
            pair = m_cache.emplace(key, static_cast<Derived *>(this)->CreateResource(key, std::forward<Args>(args)...)).first;
        }

        return &pair->second;
    }

    // template<class State, class... Args>
    // void PreloadAll(const std::vector<Key> keys, State &&state, Args &&...args) {
    //
    // }
    //
    // template<class... Args>
    // void Preload(const Key &key, Args &&...args) {
    //     Resource resource = static_cast<Derived *>(this)->CreateResource(key, std::forward<Args>(args)...);
    //
    //     {
    //         std::scoped_lock<std::mutex> lk(m_cacheMutex);
    //         m_cache.emplace(key, std::move(resource));
    //     }
    // }

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