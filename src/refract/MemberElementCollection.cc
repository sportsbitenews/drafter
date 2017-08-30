//
//  refract/MemberElementCollection.h
//  librefract
//
//  Created by Thomas Jandecka on 21/08/2016
//  Copyright (c) 2017 Apiary Inc. All rights reserved.
//

#include "MemberElementCollection.h"

#include <cassert>
#include "Element.h"
#include "TypeQueryVisitor.h"

namespace refract
{
    MemberElementCollection::const_iterator MemberElementCollection::find(const std::string& name) const
    {
        return std::find_if(elements.begin(), elements.end(), [&name](const auto& e) {
            auto keyStr = TypeQueryVisitor::as<const StringElement>(e->value.first);
            return keyStr ? keyStr->get() == name : false;
        });
    }

    MemberElementCollection::iterator MemberElementCollection::find(const std::string& name)
    {
        return std::find_if(elements.begin(), elements.end(), [&name](const auto& e) {
            auto keyStr = TypeQueryVisitor::as<const StringElement>(e->value.first);
            return keyStr ? keyStr->get() == name : false;
        });
    }

    MemberElementCollection& MemberElementCollection::operator=(const MemberElementCollection& rhs)
    {
        MemberElementCollection n(rhs);
        std::swap(n, *this);
        return *this;
    }

    void MemberElementCollection::erase(iterator it)
    {
        elements.erase(it);
    }

    void MemberElementCollection::clear()
    {
        elements.clear();
    }

    void MemberElementCollection::push_back(std::unique_ptr<MemberElement> e)
    {
        elements.emplace_back(std::move(e)); // TODO test this
    }

    void MemberElementCollection::push_back(MemberElement* e)
    {
        elements.emplace_back(e); // TODO test this
    }

    bool MemberElementCollection::empty() const noexcept
    {
        return elements.empty();
    }

    MemberElementCollection::Container::size_type MemberElementCollection::size() const noexcept
    {
        return elements.size();
    }

    MemberElementCollection::MemberElementCollection(const MemberElementCollection& other) : elements()
    {
        elements.reserve(other.size());
        std::transform(other.begin(), other.end(), std::back_inserter(elements), [](const auto& el) {
            assert(el);
            return std::unique_ptr<MemberElement>(static_cast<MemberElement*>(el->clone()));
        });
    }

    void MemberElementCollection::clone(const MemberElementCollection& other)
    {
        std::transform(other.begin(), other.end(), std::back_inserter(elements), [](const auto& el) {
            assert(el);
            return std::unique_ptr<MemberElement>(static_cast<MemberElement*>(el->clone()));
        });
    }

    void MemberElementCollection::erase(const std::string& key)
    {
        // TODO use erase-remove
        auto it = find(key);
        if (it != elements.end()) {
            elements.erase(it);
        }
    }

    MemberElement& MemberElementCollection::set(const std::string& key, IElement* value)
    {
        assert(value);
        auto it = find(key);
        if (it != elements.end()) {
            (**it) = value;
            return *(*it);
        } else {
            elements.emplace_back(new MemberElement(key, value));
            return *elements.back();
        }
    }
    MemberElement& MemberElementCollection::set(const IElement& key, const IElement& value)
    {
        if (auto strKey = TypeQueryVisitor::as<const StringElement>(&key)) {
            return set(strKey->get(), value.clone());
        }
        elements.emplace_back(new MemberElement(key.clone(), value.clone()));
        return *elements.back();
    }

    MemberElement& MemberElementCollection::set(const MemberElement& el)
    {
        // if (auto strKey = dynamic_cast<const StringElement*>(el.get().key())) {

        return set(*el.value.first, *el.value.second);
        // push_back(el);
    }

    std::unique_ptr<MemberElement> MemberElementCollection::claim(const std::string& key)
    {
        // TODO naive implementation
        auto member = find(key);
        if (member != elements.end()) {
            return claim(member);
        }
        return nullptr;
    }

    std::unique_ptr<MemberElement> MemberElementCollection::claim(iterator it)
    {
        assert(it >= elements.begin());
        assert(it < elements.end());

        std::unique_ptr<MemberElement> result(it->release());
        elements.erase(it);

        return std::move(result);
    }
}
