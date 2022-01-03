/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include "../meta_tests.hpp"

namespace
{
    enum class color : unsigned {
        red = 0xFF0000,
        green = 0x00FF00,
        blue = 0x0000FF,
        white = red | green | blue,
    };

    enum ecolor : int {
        ecolor_red = 0xFF0000,
        ecolor_green = 0x00FF00,
        ecolor_blue = 0x0000FF,
        ecolor_white = ecolor_red | ecolor_green | ecolor_blue,
    };
}

TEST_CASE("meta/meta_types/enum_type") {
    namespace meta = meta_hpp;

    meta::enum_<color>()
        .evalue_("red", color::red)
        .evalue_("green", color::green)
        .evalue_("blue", color::blue)
        .evalue_("white", color::white);

    meta::enum_<ecolor>()
        .evalue_("red", ecolor_red)
        .evalue_("green", ecolor_green)
        .evalue_("blue", ecolor_blue)
        .evalue_("white", ecolor_white);

    SUBCASE("color") {
        const meta::enum_type color_type = meta::resolve_type<color>();
        REQUIRE(color_type);

        CHECK(color_type.get_id() == meta::resolve_type(color{}).get_id());
        CHECK(color_type.get_flags() == meta::enum_flags::is_scoped);
        CHECK(color_type.get_underlying_type() == meta::resolve_type<unsigned>());

        CHECK(color_type.get_evalues().size() == 4);
    }

    SUBCASE("ecolor") {
        const meta::enum_type ecolor_type = meta::resolve_type<ecolor>();
        REQUIRE(ecolor_type);

        CHECK(ecolor_type.get_id() == meta::resolve_type(ecolor{}).get_id());
        CHECK(ecolor_type.get_flags() == meta::enum_flags{});
        CHECK(ecolor_type.get_underlying_type() == meta::resolve_type<int>());

        CHECK(ecolor_type.get_evalues().size() == 4);
    }

    SUBCASE("const color") {
        const meta::enum_type color_type = meta::resolve_type<const color>();
        REQUIRE(color_type);

        CHECK(color_type.get_id() == meta::resolve_type(color{}).get_id());
        CHECK(color_type.get_flags() == meta::enum_flags::is_scoped);
        CHECK(color_type.get_underlying_type() == meta::resolve_type<unsigned>());

        CHECK(color_type.get_evalues().size() == 4);
    }

    SUBCASE("const ecolor") {
        const meta::enum_type ecolor_type = meta::resolve_type<const ecolor>();
        REQUIRE(ecolor_type);

        CHECK(ecolor_type.get_id() == meta::resolve_type(ecolor{}).get_id());
        CHECK(ecolor_type.get_flags() == meta::enum_flags{});
        CHECK(ecolor_type.get_underlying_type() == meta::resolve_type<int>());

        CHECK(ecolor_type.get_evalues().size() == 4);
    }

    SUBCASE("color/get_evalue") {
        const meta::enum_type color_type = meta::resolve_type<color>();
        REQUIRE(color_type);

        {
            const meta::evalue green_value = color_type.get_evalue("green");
            REQUIRE(green_value);
            CHECK(green_value.get_value() == color::green);
        }

        {
            const meta::evalue yellow_value = color_type.get_evalue("yellow");
            CHECK_FALSE(yellow_value);
        }
    }

    SUBCASE("ecolor/get_evalue") {
        const meta::enum_type ecolor_type = meta::resolve_type<ecolor>();
        REQUIRE(ecolor_type);

        {
            const meta::evalue green_value = ecolor_type.get_evalue("green");
            REQUIRE(green_value);
            CHECK(green_value.get_value() == ecolor_green);
        }

        {
            const meta::evalue yellow_value = ecolor_type.get_evalue("yellow");
            CHECK_FALSE(yellow_value);
        }
    }

    SUBCASE("color/value_to_name") {
        const meta::enum_type color_type = meta::resolve_type<color>();
        REQUIRE(color_type);

        {
            REQUIRE(color_type.value_to_name(color::red));
            CHECK(color_type.value_to_name(color::red) == "red");
        }

        {

            REQUIRE(color_type.value_to_name(meta::value{color::blue}));
            CHECK(color_type.value_to_name(color::blue) == "blue");
        }

        {
            REQUIRE_FALSE(color_type.value_to_name(100500));
            REQUIRE_FALSE(color_type.value_to_name(color{100500}));
        }
    }

    SUBCASE("ecolor/value_to_name") {
        const meta::enum_type ecolor_type = meta::resolve_type<ecolor>();
        REQUIRE(ecolor_type);

        {
            REQUIRE(ecolor_type.value_to_name(ecolor_red));
            CHECK(ecolor_type.value_to_name(ecolor_red) == "red");
        }

        {

            REQUIRE(ecolor_type.value_to_name(meta::value{ecolor_blue}));
            CHECK(ecolor_type.value_to_name(ecolor_blue) == "blue");
        }

        {
            REQUIRE_FALSE(ecolor_type.value_to_name(100500));
            REQUIRE_FALSE(ecolor_type.value_to_name(ecolor{100500}));
        }
    }

    SUBCASE("color/name_to_value") {
        const meta::enum_type color_type = meta::resolve_type<color>();
        REQUIRE(color_type);

        {
            REQUIRE(color_type.name_to_value("blue"));
            CHECK(color_type.name_to_value("blue") == color::blue);
        }

        {
            REQUIRE_FALSE(color_type.name_to_value("yellow"));
        }
    }

    SUBCASE("ecolor/name_to_value") {
        const meta::enum_type ecolor_type = meta::resolve_type<ecolor>();
        REQUIRE(ecolor_type);

        {
            REQUIRE(ecolor_type.name_to_value("blue"));
            CHECK(ecolor_type.name_to_value("blue") == ecolor_blue);
        }

        {
            REQUIRE_FALSE(ecolor_type.name_to_value("yellow"));
        }
    }
}
