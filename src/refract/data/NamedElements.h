//
//  refract/data/NamedElements.h
//  librefract
//
//  Created by Thomas Jandecka on 21/08/2017
//  Copyright (c) 2017 Apiary Inc. All rights reserved.
//

#ifndef REFRACT_EXPERIMENTAL_NAMED_ELEMENTS_H
#define REFRACT_EXPERIMENTAL_NAMED_ELEMENTS_H

#include <memory>
#include <string>
#include <unordered_map>

#include "ElementIfc.h"

namespace refract
{
    namespace experimental
    {
        class NamedElements final
        {
            using Container = std::unordered_map<std::string, std::unique_ptr<IElement> >;
            Container elements;

        public:
            using iterator = typename Container::iterator;
            using const_iterator = typename Container::const_iterator;

        public:
            NamedElements() = default;
            ~NamedElements() = default;

            NamedElements(const NamedElements& other);

            NamedElements(NamedElements&&) = default;

            NamedElements& operator=(const NamedElements& rhs);
            NamedElements& operator=(NamedElements&&) = default;

        public:
            const_iterator begin() const noexcept
            {
                return elements.begin();
            }

            // FIXME potential untracked modification
            iterator begin() noexcept
            {
                return elements.begin();
            }

            const_iterator end() const noexcept
            {
                return elements.end();
            }

            // FIXME potential untracked modification
            iterator end() noexcept
            {
                return elements.end();
            }

            const_iterator find(const std::string& name) const
            {
                return elements.find(name);
            }
            iterator find(const std::string& name)
            {
                return elements.find(name);
            }

            IElement& set(const std::string& key, std::unique_ptr<IElement> value);
            IElement& set(const std::string& key, const IElement& value);

            /// clone elements from `other` to `this`
            void clone(const NamedElements& other);

            void erase(const std::string& key);
            void erase(iterator it);

            std::unique_ptr<IElement> claim(const std::string& key);
            std::unique_ptr<IElement> claim(iterator it);

            void clear();

            bool empty() const noexcept;

            Container::size_type size() const noexcept;
        };
    }
}

#endif
