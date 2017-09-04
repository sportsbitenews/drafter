//
//  refract/data/NamedElements.h
//  librefract
//
//  Created by Thomas Jandecka on 21/08/2017
//  Copyright (c) 2017 Apiary Inc. All rights reserved.
//

#include "NamedElements.h"

#include <cassert>
#include <algorithm>
#include "Element.h"
#include "ElementData.h"
#include "TypeQueryVisitor2.h"

namespace refract
{
    namespace experimental
    {
        NamedElements& NamedElements::operator=(const NamedElements& rhs)
        {
            NamedElements n(rhs);
            std::swap(n, *this);
            return *this;
        }

        void NamedElements::erase(iterator it)
        {
            elements.erase(it);
        }

        void NamedElements::clear()
        {
            elements.clear();
        }

        bool NamedElements::empty() const noexcept
        {
            return elements.empty();
        }

        NamedElements::Container::size_type NamedElements::size() const noexcept
        {
            return elements.size();
        }

        NamedElements::NamedElements(const NamedElements& other) : elements()
        {
            elements.reserve(other.size());
            std::transform(other.begin(), other.end(), std::inserter(elements, elements.end()), [](const auto& el) {
                assert(el.second);
                return std::make_pair(el.first, refract::experimental::clone(*el.second));
            });
        }

        void NamedElements::clone(const NamedElements& other)
        {
            std::transform(other.begin(), other.end(), std::inserter(elements, elements.end()), [](const auto& el) {
                assert(el.second);
                return std::make_pair(el.first, refract::experimental::clone(*el.second));
            });
        }

        void NamedElements::erase(const std::string& key)
        {
            // TODO use erase-remove
            auto it = find(key);
            if (it != elements.end()) {
                elements.erase(it);
            }
        }

        IElement& NamedElements::set(const std::string& key, std::unique_ptr<IElement> value)
        {
            auto& valueRef = *value;
            elements[key] = std::move(value);
            return valueRef;
        }

        IElement& NamedElements::set(const std::string& key, const IElement& value)
        {
            return set(key, refract::experimental::clone(value));
        }

        std::unique_ptr<IElement> NamedElements::claim(const std::string& key)
        {
            auto member = find(key);
            if (member != elements.end()) {
                return claim(member);
            }
            return nullptr;
        }

        std::unique_ptr<IElement> NamedElements::claim(iterator it)
        {
            std::unique_ptr<IElement> result(it->second.release());
            elements.erase(it);

            return std::move(result);
        }
    }
}
