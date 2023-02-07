/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2023, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../meta_base.hpp"
#include "../meta_registry.hpp"
#include "../meta_types.hpp"

#include "../meta_detail/type_traits/member_traits.hpp"

namespace meta_hpp::detail
{
    template < member_pointer_kind Member >
    struct member_tag {};

    template < member_pointer_kind Member >
    member_type_data::member_type_data(type_list<Member>)
    : type_data_base{type_id{type_list<member_tag<Member>>{}}, type_kind::member_}
    , flags{member_traits<Member>::make_flags()}
    , owner_type{resolve_type<typename member_traits<Member>::class_type>()}
    , value_type{resolve_type<typename member_traits<Member>::value_type>()} {}
}

namespace meta_hpp
{
    inline member_type::member_type(data_ptr data)
    : data_{data} {}

    inline bool member_type::is_valid() const noexcept {
        return data_ != nullptr;
    }

    inline member_type::operator bool() const noexcept {
        return is_valid();
    }

    inline type_id member_type::get_id() const noexcept {
        return data_->id;
    }

    inline member_bitflags member_type::get_flags() const noexcept {
        return data_->flags;
    }

    inline const metadata_map& member_type::get_metadata() const noexcept {
        return data_->metadata;
    }

    inline class_type member_type::get_owner_type() const noexcept {
        return data_->owner_type;
    }

    inline any_type member_type::get_value_type() const noexcept {
        return data_->value_type;
    }
}
