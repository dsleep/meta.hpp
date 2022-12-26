/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2022, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../meta_base.hpp"

namespace meta_hpp::detail
{
    template < typename T >
    inline constexpr bool is_index_family_v =
        std::same_as<T, argument_index> ||
        std::same_as<T, constructor_index> ||
        std::same_as<T, destructor_index> ||
        std::same_as<T, evalue_index> ||
        std::same_as<T, function_index> ||
        std::same_as<T, member_index> ||
        std::same_as<T, method_index> ||
        std::same_as<T, scope_index> ||
        std::same_as<T, variable_index>;

    template < typename T >
    concept index_family = is_index_family_v<T>;
}
