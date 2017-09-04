//
//  refract/data/Element.h
//  librefract
//
//  Created by Thomas Jandecka on 04/09/2017
//  Copyright (c) 2017 Apiary Inc. All rights reserved.
//

#ifndef REFRACT_EXPERIMENTAL_ELEMENT_H
#define REFRACT_EXPERIMENTAL_ELEMENT_H

#include <string>

#include "ElementData.h"
#include "ElementIfc.h"
#include "NamedElements.h"
#include "Traits.h"
#include "Visitor.h"

namespace refract
{
    namespace experimental
    {
        ///
        /// Refract Element definition
        ///
        /// @tparam DataType    Data structure definition (DSD) of the Refract Element
        ///
        template <typename DataType>
        class Element final : public IElement, public data::iterable_traits<DataType>
        {
            NamedElements meta_ = {};       //< Refract Element meta
            NamedElements attributes_ = {}; //< Refract Element attributes

            bool hasValue_ = false; //< Whether DSD is set
            DataType data_ = {};    //< DSD

            std::string name_ = DataType::name; //< Name of the Element

        public:
            using ValueType = DataType; //< DSD type definition

        public:
            ///
            /// Initialize a Refract Element with empty DSD
            /// @remark sets name of the element to DataType::name
            ///
            Element() = default;

            ///
            /// Initialize a Refract Element from a DSD
            /// @remark sets name of the element to DataType::name
            ///
            explicit Element(DataType data) : hasValue_(true), data_(std::move(data)), name_(DataType::name)
            {
            }

            ///
            /// Initialize a Refract Element from given name and DSD
            ///
            Element(const std::string& name, DataType data) : hasValue_(true), data_(data), name_(name)
            {
            }

            Element(Element&&) = default;
            Element(const Element&) = default;

            Element& operator=(Element&&) = default;
            Element& operator=(const Element&) = default;

        public:
            DataType& get() noexcept
            {
                return data_;
            }

            const DataType& get() const noexcept
            {
                return data_;
            }

            void set(DataType data)
            {
                hasValue_ = true;
                data_ = data;
            }

        public: // IElement
            NamedElements& meta() noexcept override
            {
                return meta_;
            }

            const NamedElements& meta() const noexcept override
            {
                return meta_;
            }

            NamedElements& attributes() noexcept override
            {
                return attributes_;
            }

            const NamedElements& attributes() const noexcept override
            {
                return attributes_;
            }

            std::string element() const override
            {
                return name_;
            }

            void element(const std::string& name) override
            {
                name_ = name;
            }

            void content(Visitor& v) const override
            {
                v.visit(*this);
            }

            std::unique_ptr<IElement> clone(int flags = IElement::cAll) const override
            {
                auto el = std::make_unique<Element>();

                if (flags & IElement::cElement)
                    el->element(name_);
                if (flags & IElement::cAttributes)
                    el->attributes_ = attributes_;
                if (flags & IElement::cMeta) {
                    el->meta_ = meta_; // FIXME use copy_if rather than full copy with remove
                    if (flags & IElement::cNoMetaId)
                        el->meta_.erase("id");
                }
                if (flags & IElement::cValue) {
                    el->hasValue_ = hasValue_;
                    el->data_ = data_;
                }

                return std::move(el);
            }

            bool empty() const override
            {
                return !hasValue_;
            }

        public: // implement common methods from data representation
            template <typename = typename std::enable_if<data::supports_begin<DataType>::value> >
            auto begin() const noexcept
            {
                return data_.begin();
            }

            template <typename = typename std::enable_if<data::supports_begin<DataType>::value> >
            auto begin() noexcept
            {
                return data_.begin();
            }

            template <typename = typename std::enable_if<data::supports_end<DataType>::value> >
            auto end() const noexcept
            {
                return data_.end();
            }

            template <typename = typename std::enable_if<data::supports_end<DataType>::value> >
            auto end() noexcept
            {
                return data_.end();
            }

            ///
            /// Remove a subsequence of members
            ///
            /// @param b    iterator to the first member to be removed
            /// @param e    iterator following the last member to be removed
            ///
            /// @return iterator following the last member removed
            ///
            template <typename It, typename = typename std::enable_if<data::supports_erase<DataType>::value> >
            auto erase(It b, It e)
            {
                return data_.erase(b, e);
            }

            ///
            /// Remove a specific member from collection
            ///
            /// @param data  collection to be modified
            /// @param it    iterator to the member to be removed
            ///
            /// @return iterator following the member removed
            ///
            template <typename It, typename = typename std::enable_if<data::supports_erase<DataType>::value> >
            auto erase(It it)
            {
                return erase(it, std::advance(it));
            }

            ///
            /// Query the size of the collection
            ///
            /// @param data  collection to be modified
            ///
            /// @return size of collection
            ///
            template <typename = typename std::enable_if<data::supports_size<DataType>::value> >
            auto size() const noexcept
            {
                return end() - begin();
            }

            ///
            /// Query whether the collection is empty
            ///
            /// @param data  collection to be modified
            ///
            /// @return true iff the collection doesn't have children
            ///
            template <typename = typename std::enable_if<data::supports_empty<DataType>::value> >
            bool empty() const noexcept
            {
                return size() == 0;
            }

            ///
            /// Remove all members from collection
            ///
            /// @param data  collection to be modified
            ///
            template <typename = typename std::enable_if<data::supports_clear<DataType>::value> >
            void clear()
            {
                auto e = erase(begin(), end());
                assert(e == end());
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
            template <typename It,
                typename T,
                typename = typename std::enable_if<data::supports_insert<DataType>::value> >
            auto insert(It it, T&& el)
            {
                return data_.insert(it, std::forward<T>(el));
            }

            ///
            /// Add member to end of collection
            ///
            /// @param data  collection to be modified
            /// @param el    value to be consumed
            ///
            template <typename T, typename = typename std::enable_if<data::supports_push_back<DataType>::value> >
            void push_back(T&& el)
            {
                auto it = insert(end(), std::forward<T>(el));
                assert(it == end());
            }

            ///
            /// Merge given collection
            ///
            /// @param data  collection to be merged
            ///
            /// @return a new element representing a merge of the given one
            ///
            template <typename = typename std::enable_if<data::supports_merge<DataType>::value> >
            auto merge() const
            {
                return data_.merge();
            }

            ///
            /// Get a reference to the key of a pair
            ///
            /// @param data  pair to be queried
            ///
            /// @return a reference to the key
            ///
            template <typename = typename std::enable_if<data::supports_key<DataType>::value> >
            auto key()
            {
                return data_.key();
            }

            ///
            /// Get a reference to the value of a pair
            ///
            /// @param data  pair to be queried
            ///
            /// @return a reference to the value
            ///
            template <typename = typename std::enable_if<data::supports_value<DataType>::value> >
            auto value()
            {
                return data_.value();
            }
        };

        ///
        /// Create an empty Element of given type
        /// @remark an empty Element has an empty data structure definition (DSD)
        ///
        template <typename ElementT>
        auto make_element()
        {
            return std::make_unique<ElementT>();
        }

        ///
        /// Create an Element of given type forwarding arguments to the constructor of its data structure definition
        /// (DSD)
        ///
        template <typename ElementT, typename Arg, typename... Args>
        auto make_element(Arg&& arg, Args&&... args)
        {
            using StorageT = typename ElementT::ValueType;
            return std::make_unique<ElementT>(StorageT{ std::forward<Arg>(arg), std::forward<Args>(args)... });
        }
    }
}

#endif
