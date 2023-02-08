/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2023, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../meta_base.hpp"
#include "../meta_indices.hpp"
#include "../meta_registry.hpp"
#include "../meta_types.hpp"

namespace meta_hpp
{
    inline evalue_index::evalue_index(enum_type type, std::string name)
    : type_{type}
    , name_{std::move(name)} {}

    inline enum_type evalue_index::get_type() const noexcept {
        return type_;
    }

    inline const std::string& evalue_index::get_name() const noexcept {
        return name_;
    }

    inline std::size_t evalue_index::get_hash() const noexcept {
        return detail::hash_combiner{}(detail::hash_combiner{}(type_), name_);
    }
}
