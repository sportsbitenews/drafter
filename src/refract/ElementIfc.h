//
//  refract/ElementIfc.h
//  librefract
//
//  Created by Thomas Jandecka on 21/08/2016
//  Copyright (c) 2017 Apiary Inc. All rights reserved.
//

#ifndef REFRACT_ELEMENTIFC_H
#define REFRACT_ELEMENTIFC_H

#include <string>
#include <memory>

namespace refract
{
    struct MemberElementCollection;
    struct Visitor;

    struct IElement {
        typedef enum {
            cMeta = 0x01,
            cAttributes = 0x02,
            cValue = 0x04,
            cElement = 0x08,
            cAll = cMeta | cAttributes | cValue | cElement,

            cNoMetaId = 0x10,
        } cloneFlags;

        virtual MemberElementCollection& meta() noexcept = 0;
        virtual const MemberElementCollection& meta() const noexcept = 0;

        virtual MemberElementCollection& attributes() noexcept = 0;
        virtual const MemberElementCollection& attributes() const noexcept = 0;

        /**
         * return "name" of element
         * usualy injected by "trait", but you can set own
         * via pair method `element(std::string)`
         */
        virtual std::string element() const = 0;
        virtual void element(const std::string&) = 0;

        // NOTE: probably rename to Accept
        virtual void content(Visitor& v) const = 0;

        virtual IElement* clone(int flags = cAll) const = 0;

        virtual bool empty() const = 0;

        virtual ~IElement()
        {
        }
    };

    template <typename ElementT, typename = std::enable_if<std::is_base_of<IElement, ElementT>::value> >
    auto clone(const ElementT& el, int flags = IElement::cAll)
    {
        return std::unique_ptr<ElementT>(static_cast<ElementT*>(el.clone(flags)));
    }
}

#endif
