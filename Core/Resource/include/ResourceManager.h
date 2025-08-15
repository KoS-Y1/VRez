#pragma once

#include <map>
#include <mutex>
#include <utility>
#include <string>

#include <Debug.h>

using Key = std::string;

template<class Derived, class Resource>
class ResourceManager {
public:
        virtual Resource *Load(const Key &key) {
            auto pair = m_cache.find(key);

            DEBUG_ASSERT_LOG(pair != m_cache.end(), (key + " does not exist").c_str());

            return &(pair->second);
        }

protected:
    std::map<Key, Resource> m_cache;
    std::mutex              m_cacheMutex;

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

};