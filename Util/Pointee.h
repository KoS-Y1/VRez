#pragma once

#include <memory>

template<class T>
struct Pointee {
    using Pointer = T*;

    static Pointer ptr(T& t) { return &t; }
};

template<class T>
struct Pointee<std::unique_ptr<T>> {
    using Pointer = T*;

    static Pointer ptr(std::unique_ptr<T>& t) { return t.get(); }
};
