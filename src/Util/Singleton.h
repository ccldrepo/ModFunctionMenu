#pragma once

#include <memory>

// The singleton base class.
template <class T>
class Singleton
{
public:
    [[nodiscard]] static T* GetSingleton()
    {
        static T singleton;
        return std::addressof(singleton);
    }

    Singleton(const Singleton&) = delete;
    Singleton(Singleton&&) = delete;
    Singleton& operator=(const Singleton&) = delete;
    Singleton& operator=(Singleton&&) = delete;

protected:
    Singleton() = default;

    ~Singleton() = default;
};

// The extended singleton base class.
template <class T>
class SingletonEx
{
public:
    [[nodiscard]] static T* GetSingleton() { return _singleton.get(); }

    SingletonEx(const SingletonEx&) = delete;
    SingletonEx(SingletonEx&&) = delete;
    SingletonEx& operator=(const SingletonEx&) = delete;
    SingletonEx& operator=(SingletonEx&&) = delete;

protected:
    SingletonEx() = default;

    ~SingletonEx() = default;

    class Deleter
    {
    public:
        void operator()(T* a_ptr) const { delete a_ptr; }
    };

    static inline std::unique_ptr<T, Deleter> _singleton;
};
