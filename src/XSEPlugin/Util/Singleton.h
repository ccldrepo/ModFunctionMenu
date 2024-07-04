#pragma once

#include <memory>
#include <mutex>
#include <shared_mutex>

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

    [[nodiscard]] static std::shared_lock<std::shared_mutex> LockShared() { return std::shared_lock{ _mutex }; }
    [[nodiscard]] static std::unique_lock<std::shared_mutex> LockUnique() { return std::unique_lock{ _mutex }; }

    SingletonEx(const SingletonEx&) = delete;
    SingletonEx(SingletonEx&&) = delete;
    SingletonEx& operator=(const SingletonEx&) = delete;
    SingletonEx& operator=(SingletonEx&&) = delete;

protected:
    SingletonEx() = default;

    ~SingletonEx() = default;

    struct Deleter
    {
        void operator()(T* a_ptr) const { delete a_ptr; }
    };

    static inline std::unique_ptr<T, Deleter> _singleton;

    static inline std::shared_mutex _mutex;
};
