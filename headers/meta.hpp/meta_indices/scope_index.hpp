/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2022, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../meta_base.hpp"
#include "../meta_indices.hpp"
#include "../meta_registry.hpp"
#include "../meta_states.hpp"

namespace meta_hpp
{
    inline scope_index::scope_index(std::string name)
    : name_{std::move(name)} {}

    inline scope_index scope_index::make(std::string name) {
        return scope_index{std::move(name)};
    }

    inline std::size_t scope_index::get_hash() const noexcept {
        return detail::hash_combiner{}(name_);
    }

    inline const std::string& scope_index::get_name() const noexcept {
        return name_;
    }

    inline bool operator<(const scope_index& l, const scope_index& r) noexcept {
        return std::less<>{}(l.name_, r.name_);
    }

    inline bool operator==(const scope_index& l, const scope_index& r) noexcept {
        return std::equal_to<>{}(l.name_, r.name_);
    }
}
