//
//  refract/Element.h
//  librefract
//
//  Created by Jiri Kratochvil on 18/05/15.
//  Copyright (c) 2015 Apiary Inc. All rights reserved.
//
#ifndef REFRACT_ELEMENT_H
#define REFRACT_ELEMENT_H

#include <string>
#include <vector>
#include <algorithm>
#include <functional>
#include <stdexcept>
#include <iterator>

#include "Exception.h"
#include "Visitor.h"

#include "ElementFwd.h"

namespace refract
{

    class Visitor;

    template <typename T> struct ElementTypeSelector;

    // NOTE: alternative solution:
    // find in Element* for ValueType instead of specialized templates
    template <>
    struct ElementTypeSelector<std::string>
    {
        typedef StringElement ElementType;
    };

    template <>
    struct ElementTypeSelector<char*>
    {
        typedef StringElement ElementType;
    };

    template <>
    struct ElementTypeSelector<double>
    {
        typedef NumberElement ElementType;
    };

    template <>
    struct ElementTypeSelector<int>
    {
        typedef NumberElement ElementType;
    };

    template <>
    struct ElementTypeSelector<size_t>
    {
        typedef NumberElement ElementType;
    };

    template <>
    struct ElementTypeSelector<bool>
    {
        typedef BooleanElement ElementType;
    };

    struct IElement
    {

        struct MemberElementCollection : public std::vector<MemberElement*>
        {
            virtual const_iterator find(const std::string& name) const;
            virtual iterator find(const std::string& name);
            MemberElement& operator[](const std::string& name);
            MemberElement& operator[](const int index);
            virtual void clone(const MemberElementCollection& other); /// < clone elements from `other` to `this`
            virtual void erase(const std::string& key);
            virtual ~MemberElementCollection();
        };

        MemberElementCollection meta;
        MemberElementCollection attributes;

        /**
         * return "name" of element
         * usualy injected by "trait", but you can set own
         * via pair method `element(std::string)`
         */
        virtual std::string element() const = 0;
        virtual void element(const std::string&) = 0;

        /**
         * return base name of element 
         * this allow detect "named elements"
         *
         * `if (e.element() == e.elementName*()) ...`
         *
         * do not use to check element type!
         */
        virtual const std::string elementName() const = 0;

        // NOTE: probably rename to Accept
        virtual void content(Visitor& v) const = 0;

        /**
         * Flags for clone() element - select parts of refract element to be clonned
         * \see Element<T>::clone()
         */
        typedef enum {
            cMeta       = 0x01,
            cAttributes = 0x02,
            cValue      = 0x04,
            cElement    = 0x08,
            cAll = cMeta | cAttributes | cValue | cElement,

            cNoMetaId   = 0x10,
        } cloneFlags;


        virtual IElement* clone(const int flag = cAll) const = 0;

        virtual bool empty() const = 0;

        /**
         * Returns new element with content set as `value`
         * Type of returned element depends on type of `value`
         *
         * In current implementation iis able create just primitive element types: (Bool|Number|String)
         */
        template <typename T>
        static typename ElementTypeSelector<T>::ElementType* Create(const T& value)
        {
            typedef typename ElementTypeSelector<T>::ElementType ElementType;
            return new ElementType(value);
        };

        /**
         * overrided for static function `Create()` with classic c-string
         */
        static StringElement* Create(const char* value);

        virtual ~IElement() {}
    };

    bool isReserved(const std::string& element);

    /**
     * CRTP implementation of RefractElement
     */
    template <typename T, typename Trait>
    class Element : public IElement
    {

    public:

        typedef Element<T, Trait> Type;
        typedef Trait TraitType;
        typedef typename TraitType::ValueType ValueType;

    protected:
        std::string element_;
        bool hasContent; ///< was content of element already set? \see empty()

    public:

        // FIXME: move into protected part, currently still required in ComparableVisitor
        ValueType value;

        virtual std::string element() const
        {
            return element_.empty() ? TraitType::element() : element_;
        }

        virtual void element(const std::string& name)
        {
            element_ = name;
        }

        virtual const std::string elementName() const
        {
            return TraitType::element();
        }

        void set(const ValueType& val)
        {
            hasContent = true;
            value = val;
        }

        const ValueType& get() const
        {
            return value;
        }

        virtual void content(Visitor& v) const
        {
            v.visit(static_cast<const T&>(*this));
        }

        virtual IElement* clone(const int flags = cAll) const {
            const Type* self = static_cast<const T*>(this);
            Type* element =  new Type;

            element->hasContent = self->hasContent;

            if (flags & cElement) {
                element->element_ = self->element_;
            }

            if (flags & cAttributes) {
                element->attributes.clone(self->attributes);
            }

            if (flags & cMeta) {
                element->meta.clone(self->meta);

                if (flags & cNoMetaId) {
                    element->meta.erase("id");
                }
            }

            if (flags & cValue) {
                TraitType::cloneValue(value, element->value);
            }

            return element;
        }

        Element() : hasContent(false), value(TraitType::init())
        {
        }

        virtual bool empty() const
        {
            return !hasContent;
        }

        virtual ~Element()
        {
            TraitType::release(value);
        }
    };

    struct NullElementTrait
    {
        struct null_type {};
        typedef null_type ValueType;

        static ValueType init() { return ValueType(); }
        static const std::string element() { return "null"; }
        static void release(ValueType&) {}
        static void cloneValue(const ValueType&, ValueType&) {}
    };

    struct NullElement : Element<NullElement, NullElementTrait> {};

    struct StringElementTrait
    {
        typedef std::string ValueType;

        static ValueType init() { return ValueType(); }
        static const std::string element() { return "string"; }
        static void release(ValueType&) {}
        static void cloneValue(const ValueType& self, ValueType& other) { other = self; }
    };

    struct StringElement : Element<StringElement, StringElementTrait>
    {
        StringElement() : Type() {}

        StringElement(const TraitType::ValueType& value) : Type()
        {
            set(value);
        }
    };

    struct NumberElementTrait
    {
        typedef double ValueType;

        static ValueType init() { return 0; }
        static const std::string element() { return "number"; }
        static void release(ValueType&) {}
        static void cloneValue(const ValueType& self, ValueType& other) { other = self; }
    };

    struct NumberElement : Element<NumberElement, NumberElementTrait>
    {
        NumberElement() : Type() {}

        NumberElement(const TraitType::ValueType& value) : Type()
        {
            set(value);
        }
    };

    struct BooleanElementTrait
    {
        typedef bool ValueType;

        static ValueType init() { return false; }
        static const std::string element() { return "boolean"; }
        static void release(ValueType&) {}
        static void cloneValue(const ValueType& self, ValueType& other) { other = self; }
    };

    struct BooleanElement : Element<BooleanElement, BooleanElementTrait>
    {
        BooleanElement() : Type() {}

        BooleanElement(const TraitType::ValueType& value) : Type()
        {
            set(value);
        }
    };

    typedef std::vector<IElement*> RefractElements;

    template <typename Type = IElement, typename Collection = std::vector<Type*> >
    struct ElementCollectionTrait
    {
        typedef Collection ValueType;
        typedef ElementCollectionTrait<Type, Collection> SelfType;

        static ValueType init() { return ValueType(); }

        static void release(ValueType& values)
        {
            for (typename ValueType::iterator it = values.begin(); it != values.end(); ++it) {
                delete (*it);
            }

            values.clear();
        }

        /**
         * WARN: possible dangerous method. We trust underlaying type in collection
         * if anyone push casted type, bad things can happen.
         */
        static typename ValueType::value_type typedMemberClone(IElement* element, const IElement::cloneFlags flags) {
            if (!element) {
                return NULL;
            }

            return static_cast<typename ValueType::value_type>(element->clone(flags));
        }

        static void cloneValue(const ValueType& self, ValueType& other) {
            std::transform(self.begin(), self.end(),
                           std::back_inserter(other),
                           std::bind(&SelfType::typedMemberClone, std::placeholders::_1, IElement::cAll));
        }
    };


    struct ArrayElementTrait : public ElementCollectionTrait<>
    {
        static const std::string element() { return "array"; }
    };

    struct ArrayElement : Element<ArrayElement, ArrayElementTrait>
    {
        ArrayElement() : Type()
        {
        }

        ArrayElement(const TraitType::ValueType& value) : Type()
        {
            set(value);
        }

        void push_back(IElement* e)
        {
            // FIXME: warn if MemberElement
            // there is no way to present "key: value" in array
            hasContent = true;
            value.push_back(e);
        }
    };

    struct EnumElementTrait : public ElementCollectionTrait<>
    {
        static const std::string element() { return "enum"; }
    };

    struct EnumElement : Element<EnumElement, EnumElementTrait>
    {
        EnumElement() : Type()
        {
        }

        EnumElement(const TraitType::ValueType& value) : Type()
        {
            set(value);
        }

        void push_back(IElement* e)
        {
            hasContent = true;
            value.push_back(e);
        }
    };

    struct MemberElementTrait
    {
        typedef std::pair<IElement*, IElement*> ValueType;

        static ValueType init() { return ValueType(); }
        static const std::string element() { return "member"; }

        static void release(ValueType& member)
        {
            if (member.first) {
                delete member.first;
                member.first = NULL;
            }

            if (member.second) {
                delete member.second;
                member.second = NULL;
            }
        }

        static void cloneValue(const ValueType& self, ValueType& other) {
            other.first = self.first
                ? self.first->clone()
                : NULL;

            other.second = self.second
                ? self.second->clone()
                : NULL;
        }
    };

    struct MemberElement : Element<MemberElement, MemberElementTrait>
    {

        MemberElement() : Type()
        {
        }

        MemberElement(IElement* key, IElement* value) : Type()
        {
            set(key, value);
        }

        MemberElement(const std::string& key, IElement* value) : Type()
        {
            set(key, value);
        }

        void set(const std::string& key, IElement* element)
        {
            set(IElement::Create(key), element);
        }

        void set(IElement* key, IElement* element)
        {
            if (value.first != NULL) {
                delete value.first;
                value.first = NULL;
            }

            value.first = key;

            if (value.second != NULL) {
                delete value.second;
                value.second = NULL;
            }

            value.second = element;

            hasContent = true;
        }

        MemberElement& operator=(IElement* element)
        {
            if (value.second != NULL) {
                delete value.second;
                value.second = NULL;
            }

            value.second = element;
            return *this;
        }
    };

    struct ObjectElementTrait : public ElementCollectionTrait<>
    {
        // Use inherited ValueType definition instead of specialized std::vector<MemberElement*>
        // because ObjectElement can contain:
        // - (array[Member Element])
        // - (object)
        // - (Extend Element)
        // - (Select Element)
        // - (Ref Element)
        //
        // FIXME: behavioration for content types different than
        // `(array[Member Element])` is not currently implemented

        static const std::string element() { return "object"; }
    };

    struct ObjectElement : Element<ObjectElement, ObjectElementTrait>
    {

        ObjectElement() : Type()
        {
        }

        ObjectElement(const TraitType::ValueType& value) : Type()
        {
            set(value);
        }

        ObjectElement(IElement* e) : Type()
        {
            value.push_back(e);
        }

        void push_back(IElement* e)
        {
            // FIXME:
            // probably add check for allowed type
            hasContent = true;
            value.push_back(e);
        }
    };

    struct RefElementTrait
    {
        typedef std::string ValueType;

        static ValueType init() { return ValueType(); }
        static const std::string element() { return "ref"; }
        static void release(ValueType&) {}
        static void cloneValue(const ValueType& self, ValueType& other) { other = self; }
    };

    struct RefElement : Element<RefElement, RefElementTrait>
    {
        RefElement() : Type() {}

        RefElement(const TraitType::ValueType& value) : Type()
        {
            set(value);
        }
    };

    struct ExtendElementTrait : public ElementCollectionTrait<>
    {
        static const std::string element() { return "extend"; }
    };

    struct ExtendElement : Element<ExtendElement, ExtendElementTrait>
    {
        ExtendElement() : Type()
        {
        }

        ExtendElement(const TraitType::ValueType& value) : Type()
        {
            set(value);
        }

        void push_back(IElement* e);
        void set(const ValueType& val);

        IElement* merge() const;
    };

    struct OptionElementTrait : public ElementCollectionTrait<>
    {
        static const std::string element() { return "option"; }
    };

    struct OptionElement : Element<OptionElement, OptionElementTrait>
    {

        OptionElement() : Type()
        {
        }

        OptionElement(const TraitType::ValueType& value) : Type()
        {
            set(value);
        }

        OptionElement(IElement* e) : Type()
        {
            value.push_back(e);
        }

        void push_back(IElement* e)
        {
            // FIXME:
            // probably add check for allowed type
            hasContent = true;
            value.push_back(e);
        }
    };

    struct SelectElementTrait : public ElementCollectionTrait<OptionElement>
    {
        static const std::string element() { return "select"; }

        static void cloneValue(const ValueType& self, ValueType& other)
        {
            for (std::vector<OptionElement*>::const_iterator it = self.begin(); it != self.end(); ++it) {
                IElement* clonned = (*it)->clone();
                other.push_back(static_cast<OptionElement*>(clonned));
            }
        }
    };

    struct SelectElement : Element<SelectElement, SelectElementTrait>
    {
        SelectElement() : Type()
        {
        }

        SelectElement(const TraitType::ValueType& value) : Type()
        {
            set(value);
        }

        SelectElement(OptionElement* e) : Type()
        {
            value.push_back(e);
        }

        void push_back(OptionElement* e)
        {
            // FIXME:
            // probably add check for allowed type
            hasContent = true;
            value.push_back(e);
        }
    };

};

#endif // #ifndef REFRACT_ELEMENT_H
