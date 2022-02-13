/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2022, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../meta_base.hpp"
#include "../meta_types.hpp"

#include "../meta_states/evalue.hpp"

#include "../meta_detail/type_registry.hpp"
#include "../meta_detail/type_traits/enum_traits.hpp"
#include "../meta_detail/value_utilities/uarg.hpp"

namespace meta_hpp::detail
{
    template < enum_kind Enum >
    struct enum_tag {};

    template < enum_kind Enum >
    // NOLINTNEXTLINE(readability-named-parameter)
    enum_type_data::enum_type_data(type_list<Enum>)
    : type_data_base{type_id{type_list<enum_tag<Enum>>{}}, type_kind::enum_}
    , flags{enum_traits<Enum>::make_flags()}
    , underlying_type{resolve_type<typename enum_traits<Enum>::underlying_type>()} {}
}

namespace meta_hpp
{
    inline enum_type::enum_type(detail::enum_type_data_ptr data)
    : data_{std::move(data)} {}

    inline bool enum_type::is_valid() const noexcept {
        return !!data_;
    }

    inline enum_type::operator bool() const noexcept {
        return is_valid();
    }

    inline type_id enum_type::get_id() const noexcept {
        return data_->id;
    }

    inline enum_bitflags enum_type::get_flags() const noexcept {
        return data_->flags;
    }

    inline const metadata_map& enum_type::get_metadata() const noexcept {
        return data_->metadata;
    }

    inline number_type enum_type::get_underlying_type() const noexcept {
        return data_->underlying_type;
    }

    inline const evalue_map& enum_type::get_evalues() const noexcept {
        return data_->evalues;
    }

    inline evalue enum_type::get_evalue(std::string_view name) const noexcept {
        for ( auto&& [index, evalue] : data_->evalues ) {
            if ( index.get_name() == name ) {
                return evalue;
            }
        }
        return evalue{};
    }

    template < typename Value >
    std::string_view enum_type::value_to_name(Value&& value) const noexcept {
        const detail::uarg value_arg{std::forward<Value>(value)};

        if ( value_arg.get_raw_type() != *this ) {
            return std::string_view{};
        }

        for ( auto&& evalue : data_->evalues ) {
            if ( evalue.second.get_value() == value ) {
                return evalue.second.get_index().get_name();
            }
        }

        return std::string_view{};
    }

    inline uvalue enum_type::name_to_value(std::string_view name) const noexcept {
        if ( const evalue value = get_evalue(name); value ) {
            return value.get_value();
        }

        return uvalue{};
    }
}