//
//  refract/MemberElementCollection.h
//  librefract
//
//  Created by Thomas Jandecka on 21/08/2016
//  Copyright (c) 2017 Apiary Inc. All rights reserved.
//

#ifndef REFRACT_MEMBERELEMENTCOLLECTION_H
#define REFRACT_MEMBERELEMENTCOLLECTION_H

#include <vector>
#include <memory>
#include "ElementFwd.h"

namespace refract
{
    class MemberElementCollection final
    {
        using Container = std::vector<std::unique_ptr<MemberElement> >;
        Container elements;

    public:
        using iterator = typename Container::iterator;
        using const_iterator = typename Container::const_iterator;

    public:
        MemberElementCollection() = default;
        ~MemberElementCollection() = default;

        MemberElementCollection(const MemberElementCollection& other);

        MemberElementCollection(MemberElementCollection&&) = default;

        MemberElementCollection& operator=(const MemberElementCollection& rhs);
        MemberElementCollection& operator=(MemberElementCollection&&) = default;

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

        const_iterator find(const std::string& name) const;
        iterator find(const std::string& name);

        MemberElement& set(const std::string& key, IElement* value);
        MemberElement& set(const IElement& key, const IElement& value);
        MemberElement& set(const MemberElement&);

        /// clone elements from `other` to `this`
        void clone(const MemberElementCollection& other);

        void erase(const std::string& key);
        void erase(iterator it);

        std::unique_ptr<MemberElement> claim(const std::string& key);
        std::unique_ptr<MemberElement> claim(iterator it);

        void clear();

        void push_back(std::unique_ptr<MemberElement> e);

        // FIXME TODO [[deprecated]]
        void push_back(MemberElement* e);

        bool empty() const noexcept;
        Container::size_type size() const noexcept;
    };
}

#endif
