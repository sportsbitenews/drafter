//
//  refract/data/Traits.h
//  librefract
//
//  Created by Thomas Jandecka on 23/08/17.
//  Copyright (c) 2017 Apiary Inc. All rights reserved.
//
#ifndef REFRACT_DATA_TRAITS_H
#define REFRACT_DATA_TRAITS_H

#include <cassert>
#include <iterator>
#include <type_traits>

namespace refract
{
    namespace data
    {
        template <typename T, typename = decltype(std::declval<T>().begin())>
        std::true_type supports_begin_test(const T&);
        std::false_type supports_begin_test(...);
        template <typename T>
        using supports_begin = decltype(supports_begin_test(std::declval<T>()));

        template <typename T, typename = decltype(std::declval<T>().end())>
        std::true_type supports_end_test(const T&);
        std::false_type supports_end_test(...);
        template <typename T>
        using supports_end = decltype(supports_end_test(std::declval<T>()));

        template <typename T,
            typename = decltype(std::declval<T>().insert(
                std::declval<typename T::const_iterator>(), std::declval<typename T::value_type>()))>
        std::true_type supports_insert_test(const T&);
        std::false_type supports_insert_test(...);
        template <typename T>
        using supports_insert = decltype(supports_insert_test(std::declval<T>()));

        template <typename T,
            typename = decltype(std::declval<T>().erase(
                std::declval<typename T::const_iterator>(), std::declval<typename T::const_iterator>()))>
        std::true_type supports_erase_test(const T&);
        std::false_type supports_erase_test(...);
        template <typename T>
        using supports_erase = decltype(supports_erase_test(std::declval<T>()));

        template <typename T>
        using supports_size = std::integral_constant<bool, supports_begin<T>::value && supports_end<T>::value>;

        template <typename T>
        using supports_empty = std::integral_constant<bool, supports_size<T>::value>;

        template <typename T>
        using supports_clear = std::integral_constant<bool, supports_erase<T>::value>;

        template <typename T>
        using supports_push_back = std::integral_constant<bool, supports_insert<T>::value && supports_end<T>::value>;

        template <typename T, typename = decltype(std::declval<T>().merge())>
        std::true_type supports_merge_test(const T&);
        std::false_type supports_merge_test(...);
        template <typename T>
        using supports_merge = decltype(supports_merge_test(std::declval<T>()));

        template <typename T, typename = decltype(std::declval<T>().key())>
        std::true_type supports_key_test(const T&);
        std::false_type supports_key_test(...);
        template <typename T>
        using supports_key = decltype(supports_key_test(std::declval<T>()));

        template <typename T, typename = decltype(std::declval<T>().value())>
        std::true_type supports_value_test(const T&);
        std::false_type supports_value_test(...);
        template <typename T>
        using supports_value = decltype(supports_value_test(std::declval<T>()));

        template <typename T>
        using is_iterable = std::integral_constant<bool, supports_begin<T>::value && supports_end<T>::value>;

        template <typename T>
        using is_pair = std::integral_constant<bool, supports_key<T>::value && supports_value<T>::value>;

        template <typename T, bool = data::is_iterable<T>::value>
        struct iterable_traits {
        };

        template <typename Container, bool = data::is_iterable<Container>::value>
        struct algorithm_traits {
        };

        template <typename Container>
        struct algorithm_traits<Container, true> {
            using container_type = Container;

            using const_iterator = typename Container::const_iterator;
            using const_pointer = typename Container::const_pointer;
            using const_reference = typename Container::const_reference;
            using difference_type = typename Container::difference_type;
            using iterator = typename Container::iterator;
            using size_type = typename Container::size_type;
            using value_type = typename Container::value_type;
        };

        template <typename Child>
        class container_traits
        {
            Child& self() noexcept
            {
                return *static_cast<Child*>(this);
            }

            const Child& self() const noexcept
            {
                return *static_cast<const Child*>(this);
            }

        public:
            ///
            /// Remove a specific member from collection
            ///
            /// @param data  collection to be modified
            /// @param it    iterator to the member to be removed
            ///
            /// @return iterator following the member removed
            ///
            template <typename It, typename = typename std::enable_if<data::supports_erase<Child>::value> >
            auto erase(It it)
            {
                return self().erase(it, std::next(it));
            }

            ///
            /// Query the size of the collection
            ///
            /// @param data  collection to be modified
            ///
            /// @return size of collection
            ///
            template <typename = typename std::enable_if<data::supports_size<Child>::value> >
            auto size() const noexcept
            {
                return self().end() - self().begin();
            }

            ///
            /// Query whether the collection is empty
            ///
            /// @param data  collection to be modified
            ///
            /// @return true iff the collection doesn't have children
            ///
            template <typename = typename std::enable_if<data::supports_empty<Child>::value> >
            bool empty() const noexcept
            {
                return size() == 0;
            }

            ///
            /// Remove all members from collection
            ///
            /// @param data  collection to be modified
            ///
            template <typename = typename std::enable_if<data::supports_clear<Child>::value> >
            void clear()
            {
                auto e = erase(self().begin(), self().end());
                assert(e == self().end());
            }

            ///
            /// Add a member at before given position
            ///
            /// @param data  collection to be modified
            /// @param it    position after where the value shall be inserted
            /// @param el    value to be consumed
            ///
            /// @return iterator to inserted member
            ///
            template <typename It, typename Value, typename = typename std::enable_if<data::supports_insert<Child>::value> >
            auto insert(It it, Value el)
            {
                return self().insert(it, std::move(el));
            }

            ///
            /// Add member to end of collection
            ///
            /// @param data  collection to be modified
            /// @param el    value to be consumed
            ///
            template <typename Value, typename = typename std::enable_if<data::supports_push_back<Child>::value> >
            void push_back(Value el)
            {
                auto it = self().insert(self().end(), std::move(el));
                assert(std::next(it) == self().end());
            }
        };
    }
}

#endif
