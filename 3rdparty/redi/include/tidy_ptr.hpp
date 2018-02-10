#ifndef REDI_TIDY_PTR_H
#define REDI_TIDY_PTR_H

// Copyright Jonathan Wakely 2012
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <utility>
#include <type_traits>

namespace redi
{
  /// A not-very-smart pointer that is null when moved-from.
  template<typename T>
    struct tidy_ptr
    {
      typedef T element_type;

      explicit constexpr tidy_ptr(T* ptr = nullptr) noexcept : ptr(ptr) { }

      template<typename U>
        constexpr tidy_ptr(const tidy_ptr<U>& u) noexcept : ptr(u.get())
        {
          static_assert( std::is_convertible<U*, T*>::value,
              "No implicit conversion between pointer types." );
        }

      tidy_ptr(const tidy_ptr&) = default;
      tidy_ptr& operator=(const tidy_ptr&) = default;

      /** Move constructor.
       *
       * @post @c o.get()==nullptr
       */
      tidy_ptr(tidy_ptr&& o) noexcept : ptr(o.ptr) { o.ptr = nullptr; }

      /** Move assignment operator.
       *
       * @post @c o.get()==nullptr
       */
      tidy_ptr& operator=(tidy_ptr&& o) noexcept
      {
        ptr = o.ptr;
        o.ptr = nullptr;
        return *this;
      }

      tidy_ptr& operator=(T* p) noexcept
      {
        ptr = p;
        return *this;
      }

      void swap(tidy_ptr& o) noexcept { std::swap(ptr, o.ptr); }

      T& operator*() const noexcept { return *ptr; }
      constexpr T* operator->() const noexcept { return ptr; }

      constexpr T* get() const noexcept { return ptr; }

      explicit constexpr operator bool() const noexcept
      { return ptr != nullptr; }

    private:
      T* ptr;
    };

  template<typename T, typename U>
    constexpr bool
    operator==(const tidy_ptr<T>& lhs, const tidy_ptr<U>& rhs) noexcept
    {
      return lhs.get() == rhs.get();
    }

  template<typename T, typename U>
    constexpr bool
    operator!=(const tidy_ptr<T>& lhs, const tidy_ptr<U>& rhs) noexcept
    { return !(lhs == rhs); }

  template<typename T, typename U>
    constexpr bool
    operator==(const tidy_ptr<T>& lhs, const U* rhs) noexcept
    {
      return lhs.get() == rhs;
    }

  template<typename T, typename U>
    constexpr bool
    operator==(const T* lhs, const tidy_ptr<U>& rhs) noexcept
    {
      return lhs == rhs.get();
    }

  template<typename T, typename U>
    constexpr bool
    operator!=(const tidy_ptr<T>& lhs, const U* rhs) noexcept
    { return !(lhs == rhs); }

  template<typename T, typename U>
    constexpr bool
    operator!=(const T* lhs, const tidy_ptr<U>& rhs) noexcept
    { return !(lhs == rhs); }

  template<typename T>
    constexpr bool
    operator==(const tidy_ptr<T>& lhs, std::nullptr_t) noexcept
    { return lhs.get() == nullptr; }

  template<typename T>
    constexpr bool
    operator==(std::nullptr_t, const tidy_ptr<T>& rhs) noexcept
    { return nullptr == rhs.get(); }

  template<typename T>
    constexpr bool
    operator!=(const tidy_ptr<T>& lhs, std::nullptr_t) noexcept
    { return !(lhs == nullptr); }

  template<typename T>
    constexpr bool
    operator!=(std::nullptr_t, const tidy_ptr<T>& rhs) noexcept
    { return !(nullptr == rhs); }

  /// Swap
  template<typename T>
    constexpr void
    swap(tidy_ptr<T>& lhs, tidy_ptr<T>& rhs) noexcept
    { lhs.swap(rhs); }

}  // namespace redi

#endif // REDI_TIDY_PTR_H

// vi: set ft=cpp sw=2:

