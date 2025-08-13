#pragma once

template<typename T>
class Singleton {
public:
    // Disallow copy and move constructor
    Singleton(const Singleton &) = delete;
    Singleton(Singleton &&)      = delete;

    // Disallow copy and move operator
    Singleton &operator=(const Singleton &) = delete;
    Singleton &operator=(Singleton &&)      = delete;

    static T &GetInstance() {
        static InstanceT instance;
        return instance;
    }


protected:
    Singleton()  = default;
    ~Singleton() = default;

private:
    struct InstanceT : T {};
};