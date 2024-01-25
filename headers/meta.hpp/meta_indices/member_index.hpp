/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2024, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../meta_base.hpp"
#include "../meta_indices.hpp"
#include "../meta_types.hpp"

namespace meta_hpp
{
    inline member_index::member_index(member_type type, std::string name)
    : type_{type}
    , name_{std::move(name)} {}

    inline member_type member_index::get_type() const noexcept {
        return type_;
    }

    inline std::string member_index::get_name() && noexcept {
        return std::move(name_);
    }

    inline const std::string& member_index::get_name() const& noexcept {
        return name_;
    }

    inline void member_index::swap(member_index& other) noexcept {
        std::swap(type_, other.type_);
        std::swap(name_, other.name_);
    }

    inline std::size_t member_index::get_hash() const noexcept {
        return detail::hash_combiner{}(detail::hash_combiner{}(type_), name_);
    }
}
