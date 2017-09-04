//
//  refract/data/TypeQueryVisitor.h
//  librefract
//
//  Created by Thomas Jandecka on 04/09/2017.
//  Copyright (c) 2017 Apiary Inc. All rights reserved.
//
#ifndef REFRACT_EXPERIMENTAL_TYPEQUERYVISITOR_H
#define REFRACT_EXPERIMENTAL_TYPEQUERYVISITOR_H

#include "ElementFwd2.h"
#include "Visitor.h"

namespace refract
{
    namespace experimental
    {
        class TypeQueryVisitor
        {

        public:
            typedef enum {
                Null,
                Holder,

                String,
                Number,
                Boolean,

                Array,
                Member,
                Object,
                Enum,

                Ref,
                Extend,

                Option,
                Select,

                Unknown = 0,
            } ElementType;

        private:
            ElementType typeInfo;

        public:
            TypeQueryVisitor();

            void operator()(const IElement& e);
            void operator()(const HolderElement& e);
            void operator()(const NullElement& e);
            void operator()(const StringElement& e);
            void operator()(const NumberElement& e);
            void operator()(const BooleanElement& e);
            void operator()(const ArrayElement& e);
            void operator()(const MemberElement& e);
            void operator()(const ObjectElement& e);
            void operator()(const EnumElement& e);
            void operator()(const RefElement& e);
            void operator()(const ExtendElement& e);
            void operator()(const OptionElement& e);
            void operator()(const SelectElement& e);

            ElementType get() const;

            template <typename E>
            static E* as(IElement* e)
            {
                if (!e) {
                    return static_cast<E*>(e);
                }

                TypeQueryVisitor tq;
                Visit(tq, *e);

                E type;
                TypeQueryVisitor eq;
                VisitBy(type, eq);

                if (eq.typeInfo != tq.typeInfo) {
                    return 0;
                }

                return static_cast<E*>(e);
            }

            template <typename E>
            static const E* as(const IElement* e)
            {
                if (!e) {
                    return static_cast<const E*>(e);
                }

                TypeQueryVisitor tq;
                Visit(tq, *e);

                E type;
                TypeQueryVisitor eq;
                VisitBy(type, eq);

                if (eq.typeInfo != tq.typeInfo) {
                    return 0;
                }

                return static_cast<const E*>(e);
            }
        };
    }
}

#endif // #ifndef REFRACT_TYPEQUERYVISITOR_H
