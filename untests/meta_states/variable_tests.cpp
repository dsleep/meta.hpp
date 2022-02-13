/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2022, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include "../meta_untests.hpp"

namespace
{
    struct clazz_1 {
        static int int_variable;
        static const int const_int_variable;

        static int& ref_int_variable;
        static const int& const_ref_int_variable;

        static std::unique_ptr<int> unique_int_variable;
        static const std::unique_ptr<int> const_unique_int_variable;
    };

    int clazz_1::int_variable = 1;
    const int clazz_1::const_int_variable = 2;

    int& clazz_1::ref_int_variable = clazz_1::int_variable;
    const int& clazz_1::const_ref_int_variable = clazz_1::const_int_variable;

    std::unique_ptr<int> clazz_1::unique_int_variable = std::make_unique<int>(42);
    const std::unique_ptr<int> clazz_1::const_unique_int_variable = std::make_unique<int>(42);
}

TEST_CASE("meta/meta_states/variable") {
    namespace meta = meta_hpp;

    meta::class_<clazz_1>()
        .variable_("int_variable", &clazz_1::int_variable)
        .variable_("const_int_variable", &clazz_1::const_int_variable)
        .variable_("ref_int_variable", &clazz_1::ref_int_variable)
        .variable_("const_ref_int_variable", &clazz_1::const_ref_int_variable)

        // .variable_("unique_int_variable", &clazz_1::unique_int_variable)
        .variable_("unique_int_variable_as_ptr", &clazz_1::unique_int_variable, meta::variable_policy::as_pointer{})
        .variable_("unique_int_variable_as_ref", &clazz_1::unique_int_variable, meta::variable_policy::as_reference_wrapper{})

        // .variable_("const_unique_int_variable", &clazz_1::const_unique_int_variable)
        .variable_("const_unique_int_variable_as_ptr", &clazz_1::const_unique_int_variable, meta::variable_policy::as_pointer{})
        .variable_("const_unique_int_variable_as_ref", &clazz_1::const_unique_int_variable, meta::variable_policy::as_reference_wrapper{});

    const meta::class_type clazz_1_type = meta::resolve_type<clazz_1>();
    REQUIRE(clazz_1_type);

    SUBCASE("") {
        const meta::variable variable;
        CHECK_FALSE(variable);
        CHECK_FALSE(variable.is_valid());
        CHECK(variable == clazz_1_type.get_variable("non-existent-variable"));
    }

    SUBCASE("operators") {
        meta::variable int_variable_v = clazz_1_type.get_variable("int_variable");
        meta::variable const_int_variable_v = clazz_1_type.get_variable("const_int_variable");
        CHECK(int_variable_v == int_variable_v);
        CHECK(int_variable_v != const_int_variable_v);
        CHECK((int_variable_v < const_int_variable_v || const_int_variable_v < int_variable_v));
    }

    SUBCASE("int") {
        meta::variable vm = clazz_1_type.get_variable("int_variable");
        REQUIRE(vm);

        CHECK(vm.get_type() == meta::resolve_type(&clazz_1::int_variable));
        CHECK(vm.get_name() == "int_variable");

        CHECK(vm.get() == 1);
        CHECK(vm() == 1);

        CHECK(vm.is_settable_with<int>());
        CHECK(vm.is_settable_with<int&&>());
        CHECK(vm.is_settable_with<const int&>());
        CHECK(vm.is_settable_with(1));

        CHECK_FALSE(vm.is_settable_with<float>());
        CHECK_FALSE(vm.is_settable_with<float&&>());
        CHECK_FALSE(vm.is_settable_with<const float&>());
        CHECK_FALSE(vm.is_settable_with(1.0));

        CHECK_NOTHROW(vm.set(10)); CHECK(vm.get() == 10);
        CHECK_NOTHROW(vm(11)); CHECK(vm() == 11);
    }

    SUBCASE("const int") {
        meta::variable vm = clazz_1_type.get_variable("const_int_variable");
        REQUIRE(vm);

        CHECK(vm.get_type() == meta::resolve_type(&clazz_1::const_int_variable));
        CHECK(vm.get_name() == "const_int_variable");

        CHECK(vm.get() == 2);
        CHECK(vm() == 2);

        CHECK_FALSE(vm.is_settable_with<int>());
        CHECK_FALSE(vm.is_settable_with<int&&>());
        CHECK_FALSE(vm.is_settable_with<const int&>());
        CHECK_FALSE(vm.is_settable_with(1));

        CHECK_FALSE(vm.is_settable_with<float>());
        CHECK_FALSE(vm.is_settable_with<float&&>());
        CHECK_FALSE(vm.is_settable_with<const float&>());
        CHECK_FALSE(vm.is_settable_with(1.0));

        CHECK_THROWS(vm.set(10)); CHECK(vm.get() == 2);
        CHECK_THROWS(vm(10)); CHECK(vm() == 2);
    }

    SUBCASE("ref int") {
        meta::variable vm = clazz_1_type.get_variable("ref_int_variable");
        REQUIRE(vm);

        CHECK(vm.get_type() == meta::resolve_type(&clazz_1::ref_int_variable));
        CHECK(vm.get_name() == "ref_int_variable");

        CHECK(vm.get() == 11);
        CHECK(vm() == 11);

        CHECK(vm.is_settable_with<int>());
        CHECK(vm.is_settable_with<int&&>());
        CHECK(vm.is_settable_with<const int&>());
        CHECK(vm.is_settable_with(1));

        CHECK_FALSE(vm.is_settable_with<float>());
        CHECK_FALSE(vm.is_settable_with<float&&>());
        CHECK_FALSE(vm.is_settable_with<const float&>());
        CHECK_FALSE(vm.is_settable_with(1.0));

        CHECK_NOTHROW(vm.set(20)); CHECK(vm.get() == 20);
        CHECK_NOTHROW(vm(21)); CHECK(vm() == 21);
    }

    SUBCASE("const ref int") {
        meta::variable vm = clazz_1_type.get_variable("const_ref_int_variable");
        REQUIRE(vm);

        CHECK(vm.get_type() == meta::resolve_type(&clazz_1::const_ref_int_variable));
        CHECK(vm.get_name() == "const_ref_int_variable");

        CHECK(vm.get() == 2);
        CHECK(vm() == 2);

        CHECK_FALSE(vm.is_settable_with<int>());
        CHECK_FALSE(vm.is_settable_with<int&&>());
        CHECK_FALSE(vm.is_settable_with<const int&>());
        CHECK_FALSE(vm.is_settable_with(1));

        CHECK_FALSE(vm.is_settable_with<float>());
        CHECK_FALSE(vm.is_settable_with<float&&>());
        CHECK_FALSE(vm.is_settable_with<const float&>());
        CHECK_FALSE(vm.is_settable_with(1.0));

        CHECK_THROWS(vm.set(10)); CHECK(vm.get() == 2);
        CHECK_THROWS(vm(11)); CHECK(vm() == 2);
    }

    SUBCASE("unique_int_variable_as_ptr") {
        meta::variable vm = clazz_1_type.get_variable("unique_int_variable_as_ptr");
        REQUIRE(vm);

        CHECK(vm.get().get_type() == meta::resolve_type<std::unique_ptr<int>*>());
        CHECK(vm.get() == std::addressof(clazz_1::unique_int_variable));

        {
            auto nv = std::make_unique<int>(11);
            CHECK_NOTHROW(vm.set(std::move(nv)));
            CHECK(*clazz_1::unique_int_variable == 11);
        }

        {
            auto nv = std::make_unique<int>(12);
            CHECK_THROWS(vm.set(nv));
            CHECK(*clazz_1::unique_int_variable == 11);
        }
    }

    SUBCASE("unique_int_variable_as_ref") {
        meta::variable vm = clazz_1_type.get_variable("unique_int_variable_as_ref");
        REQUIRE(vm);

        using ref_t = std::reference_wrapper<std::unique_ptr<int>>;
        CHECK(vm.get().get_type() == meta::resolve_type<ref_t>());
        CHECK(vm.get().try_cast<ref_t>()->get() == clazz_1::unique_int_variable);

        {
            auto nv = std::make_unique<int>(13);
            CHECK_NOTHROW(vm.set(std::move(nv)));
            CHECK(*clazz_1::unique_int_variable == 13);
        }

        {
            auto nv = std::make_unique<int>(14);
            CHECK_THROWS(vm.set(nv));
            CHECK(*clazz_1::unique_int_variable == 13);
        }
    }

    SUBCASE("const_unique_int_variable_as_ptr") {
        meta::variable vm = clazz_1_type.get_variable("const_unique_int_variable_as_ptr");
        REQUIRE(vm);

        CHECK(vm.get().get_type() == meta::resolve_type<const std::unique_ptr<int>*>());
        CHECK(vm.get() == std::addressof(clazz_1::const_unique_int_variable));

        {
            auto nv = std::make_unique<int>(11);
            CHECK_THROWS(vm.set(nv));
            CHECK_THROWS(vm.set(std::move(nv)));
            CHECK(*clazz_1::const_unique_int_variable == 42);
        }
    }

    SUBCASE("const_unique_int_variable_as_ref") {
        meta::variable vm = clazz_1_type.get_variable("const_unique_int_variable_as_ref");
        REQUIRE(vm);

        using ref_t = std::reference_wrapper<const std::unique_ptr<int>>;
        CHECK(vm.get().get_type() == meta::resolve_type<ref_t>());
        CHECK(vm.get().try_cast<ref_t>()->get() == clazz_1::const_unique_int_variable);

        {
            auto nv = std::make_unique<int>(12);
            CHECK_THROWS(vm.set(nv));
            CHECK_THROWS(vm.set(std::move(nv)));
            CHECK(*clazz_1::const_unique_int_variable == 42);
        }
    }
}