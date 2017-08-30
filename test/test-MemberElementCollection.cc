//
//  test/test-MemberElementCollection.cc
//  test-librefract
//
//  Created by Thomas Jandecka on 26/08/2016
//  Copyright (c) 2017 Apiary Inc. All rights reserved.
//

#include "catch.hpp"

#include "refract/MemberElementCollection.h"
#include "refract/Element.h"

#include "ElementMock.h"

using namespace refract;

using StringElementT = Element<StringElement, StringElementTrait>;

SCENARIO("MemberElementCollections are initialy empty", "[MemberElementCollection]")
{
    GIVEN("A default initialized MemberElementCollection")
    {
        MemberElementCollection collection;

        THEN("it is empty")
        {
            REQUIRE(collection.empty());
        }
        THEN("its size is zero")
        {
            REQUIRE(collection.size() == 0);
        }
        THEN("its begin is equal to its end")
        {
            REQUIRE(collection.begin() == collection.end());
        }
    }
}

SCENARIO("Elements can be added and erased from MemberElementCollection", "[MemberElementCollection]")
{
    GIVEN("An empty MemberElementCollection")
    {
        MemberElementCollection collection;

        WHEN("an ElementMock pointer is added")
        {
            const char* key_ = "id";

            ElementMock* el = new ElementMock();
            el->element_out = "\0 42";
            collection.set(key_, el);

            THEN("its size increments")
            {
                REQUIRE(collection.size() == 1);
            }
            THEN("nothing gets called on the mock")
            {
                REQUIRE(el->_total_ctx == 0);
            }
            THEN("the original mock pointer is still dereferencable")
            {
                REQUIRE(el->element() == "\0 42");
            }
            THEN("the pointer obtained by find equals the original one")
            {
                REQUIRE((*collection.find(key_))->value.second == el);
            }

            WHEN("the member is deleted by erase(const std::string&)")
            {
                REQUIRE(ElementMock::instances().size() == 1);
                collection.erase(key_);

                THEN("its size decrements")
                {
                    REQUIRE(collection.size() == 0);
                }
                THEN("it is empty")
                {
                    REQUIRE(collection.empty());
                }
                THEN("the mock is destructed")
                {
                    REQUIRE(ElementMock::instances().size() == 0);
                }
            }
        }
    }
}

SCENARIO("MemberElementCollections can be copied and moved", "[MemberElementCollection]")
{
    GIVEN("An empty MemberElementCollection")
    {
        MemberElementCollection collection;

        WHEN("from it another MemberElementCollection is copy constructed")
        {
            MemberElementCollection collection2(collection);

            THEN("the latter is also empty")
            {
                REQUIRE(collection.empty());
            }
        }

        WHEN("from it another MemberElementCollection is move constructed")
        {
            MemberElementCollection collection2(std::move(collection));

            THEN("the latter is also empty")
            {
                REQUIRE(collection.empty());
            }
        }
    }

    GIVEN("A MemberElementCollection with three entries")
    {
        std::string key1 = "id";
        std::string key2 = "peterson";
        const char* key3 = "rabbit h\000le";

        auto mock1 = new ElementMock{};
        auto mock2 = new ElementMock{};
        auto mock3 = new ElementMock{};

        auto mock1clone = new ElementMock{};
        auto mock2clone = new ElementMock{};
        auto mock3clone = new ElementMock{};

        mock1->clone_out.reset(mock1clone);
        mock2->clone_out.reset(mock2clone);
        mock3->clone_out.reset(mock3clone);

        MemberElementCollection collection;

        collection.set(key1, mock1);
        collection.set(key2, mock2);
        collection.set(key3, mock3);

        REQUIRE(collection.size() == 3);

        WHEN("from it another MemberElementCollection is copy constructed")
        {
            MemberElementCollection collection2(collection);

            THEN("their sizes equal")
            {
                REQUIRE(collection.size() == collection2.size());
            }
            THEN("the members of the latter are not nullptr")
            {
                for (const auto& m : collection2) {
                    REQUIRE(m);
                }
            }
            THEN("the keys of the latter are not nullptr")
            {
                for (const auto& m : collection2) {
                    REQUIRE(m->value.first);
                }
            }
            THEN("the keys of the latter are StringElement*s")
            {
                for (const auto& m : collection2) {
                    REQUIRE(dynamic_cast<const StringElementT*>(m->value.first));
                }
            }
            THEN("the values of the latter are not nullptr")
            {
                for (const auto& m : collection2) {
                    REQUIRE(m->value.second);
                }
            }
            THEN("the values of the latter were obtained by calling clone() on the original")
            {
                REQUIRE(mock1->_total_ctx == 1);
                REQUIRE(mock1->clone_ctx == 1);
                REQUIRE(mock1->clone_in == IElement::cAll);
                REQUIRE(collection2.begin()[0]->value.second == mock1clone);

                REQUIRE(mock2->_total_ctx == 1);
                REQUIRE(mock2->clone_ctx == 1);
                REQUIRE(mock2->clone_in == IElement::cAll);
                REQUIRE(collection2.begin()[1]->value.second == mock2clone);

                REQUIRE(mock3->_total_ctx == 1);
                REQUIRE(mock3->clone_ctx == 1);
                REQUIRE(mock3->clone_in == IElement::cAll);
                REQUIRE(collection2.begin()[2]->value.second == mock3clone);
            }
            THEN("their keys equal")
            {
                REQUIRE(std::equal(
                    collection.begin(), collection.end(), collection2.begin(), [](const auto& m1, const auto& m2) {

                        auto key1_ = dynamic_cast<const StringElementT*>(m1->value.first);
                        auto key2_ = dynamic_cast<const StringElementT*>(m2->value.first);

                        REQUIRE(key1_);
                        REQUIRE(key2_);

                        return key1_->get() == key2_->get();
                    }));
            }
        }

        WHEN("from it another MemberElementCollection is move constructed")
        {
            MemberElementCollection collection2(std::move(collection));

            THEN("the original is empty")
            {
                REQUIRE(collection.empty());
            }
            THEN("the size of the latter is 3")
            {
                REQUIRE(collection2.size() == 3);
            }
            THEN("the members of the latter are not nullptr")
            {
                for (const auto& m : collection2) {
                    REQUIRE(m);
                }
            }
            THEN("the keys of the latter are not nullptr")
            {
                for (const auto& m : collection2) {
                    REQUIRE(m->value.first);
                }
            }
            THEN("the keys of the latter are StringElement*s")
            {
                for (const auto& m : collection2) {
                    REQUIRE(dynamic_cast<const StringElement*>(m->value.first));
                }
            }
            THEN("the values of the latter are not nullptr")
            {
                for (const auto& m : collection2) {
                    REQUIRE(m->value.first);
                }
            }
            THEN("their keys equal")
            {
                REQUIRE(std::equal(
                    collection.begin(), collection.end(), collection2.begin(), [](const auto& m1, const auto& m2) {

                        auto key1 = dynamic_cast<const StringElement*>(m1->value.first);
                        auto key2 = dynamic_cast<const StringElement*>(m2->value.first);

                        return key1->get() == key2->get();
                    }));
            }
            THEN("the values of the latter are the same as the original values")
            {
                REQUIRE(collection2.begin()[0]->value.second == mock1);
                REQUIRE(collection2.begin()[1]->value.second == mock2);
                REQUIRE(collection2.begin()[2]->value.second == mock3);
            }
        }
    }
}
