/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2022, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../meta_base.hpp"
#include "../meta_types.hpp"

#include "../meta_detail/type_registry.hpp"
#include "../meta_detail/type_traits/array_traits.hpp"

namespace meta_hpp::detail
{
    template < array_kind Array >
    struct array_tag {};

    template < array_kind Array >
    // NOLINTNEXTLINE(readability-named-parameter)
    array_type_data::array_type_data(type_list<Array>)
    : type_data_base{type_id{type_list<array_tag<Array>>{}}, type_kind::array_}
    , flags{array_traits<Array>::make_flags()}
    , extent{array_traits<Array>::extent}
    , data_type{resolve_type<typename array_traits<Array>::data_type>()} {}
}

namespace meta_hpp
{
    inline array_type::array_type(detail::array_type_data_ptr data)
    : data_{std::move(data)} {}

    inline bool array_type::is_valid() const noexcept {
        return !!data_;
    }

    inline array_type::operator bool() const noexcept {
        return is_valid();
    }

    inline type_id array_type::get_id() const noexcept {
        return data_->id;
    }

    inline array_bitflags array_type::get_flags() const noexcept {
        return data_->flags;
    }

    inline const metadata_map& array_type::get_metadata() const noexcept {
        return data_->metadata;
    }

    inline std::size_t array_type::get_extent() const noexcept {
        return data_->extent;
    }

    inline any_type array_type::get_data_type() const noexcept {
        return data_->data_type;
    }
}