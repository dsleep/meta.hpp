/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2023, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../meta_base.hpp"
#include "../meta_states.hpp"

#include "../meta_detail/type_registry.hpp"
#include "../meta_detail/value_utilities/uarg.hpp"
#include "../meta_types/pointer_type.hpp"

namespace meta_hpp::detail
{
    template < variable_policy_kind Policy, pointer_kind Pointer >
    uvalue raw_variable_getter(type_registry&, Pointer variable_ptr) {
        using pt = pointer_traits<Pointer>;
        using data_type = typename pt::data_type;

        constexpr bool as_copy                                    //
            = std::is_copy_constructible_v<data_type>             //
           && std::is_same_v<Policy, variable_policy::as_copy_t>; //

        constexpr bool as_ptr                                        //
            = std::is_same_v<Policy, variable_policy::as_pointer_t>; //

        constexpr bool as_ref_wrap                                             //
            = std::is_same_v<Policy, variable_policy::as_reference_wrapper_t>; //

        static_assert(as_copy || as_ptr || as_ref_wrap);

        auto&& return_value = *variable_ptr;

        if constexpr ( as_copy ) {
            return uvalue{META_HPP_FWD(return_value)};
        }

        if constexpr ( as_ptr ) {
            return uvalue{std::addressof(return_value)};
        }

        if constexpr ( as_ref_wrap ) {
            return uvalue{std::ref(return_value)};
        }
    }

    template < pointer_kind Pointer >
    void raw_variable_setter(type_registry& registry, Pointer variable_ptr, const uarg& arg) {
        using pt = pointer_traits<Pointer>;
        using data_type = typename pt::data_type;

        if constexpr ( std::is_const_v<data_type> ) {
            (void)registry;
            (void)variable_ptr;
            (void)arg;
            META_HPP_ASSERT(false && "an attempt to set a constant variable");
        } else {
            META_HPP_ASSERT(                         //
                arg.can_cast_to<data_type>(registry) //
                && "an attempt to set a variable with an incorrect argument type"
            );

            *variable_ptr = arg.cast<data_type>(registry);
        }
    }

    template < pointer_kind Pointer >
    bool raw_variable_is_settable_with(type_registry& registry, const uarg_base& arg) {
        using pt = pointer_traits<Pointer>;
        using data_type = typename pt::data_type;

        if constexpr ( std::is_const_v<data_type> ) {
            (void)registry;
            (void)arg;
            return false;
        } else {
            return arg.can_cast_to<data_type>(registry);
        }
    }
}

namespace meta_hpp::detail
{
    template < variable_policy_kind Policy, pointer_kind Pointer >
    variable_state::getter_impl make_variable_getter(type_registry& registry, Pointer variable_ptr) {
        return [&registry, variable_ptr]() { //
            return raw_variable_getter<Policy>(registry, variable_ptr);
        };
    }

    template < pointer_kind Pointer >
    variable_state::setter_impl make_variable_setter(type_registry& registry, Pointer variable_ptr) {
        return [&registry, variable_ptr](const uarg& arg) { //
            return raw_variable_setter(registry, variable_ptr, arg);
        };
    }

    template < pointer_kind Pointer >
    variable_state::is_settable_with_impl make_variable_is_settable_with(type_registry& registry) {
        return [&registry](const uarg_base& arg) { //
            return raw_variable_is_settable_with<Pointer>(registry, arg);
        };
    }
}

namespace meta_hpp::detail
{
    inline variable_state::variable_state(variable_index nindex, metadata_map nmetadata)
    : index{std::move(nindex)}
    , metadata{std::move(nmetadata)} {}

    template < variable_policy_kind Policy, pointer_kind Pointer >
    variable_state_ptr variable_state::make(std::string name, Pointer variable_ptr, metadata_map metadata) {
        type_registry& registry{type_registry::instance()};
        variable_state state{variable_index{registry.resolve_type<Pointer>(), std::move(name)}, std::move(metadata)};
        state.getter = make_variable_getter<Policy>(registry, variable_ptr);
        state.setter = make_variable_setter(registry, variable_ptr);
        state.is_settable_with = make_variable_is_settable_with<Pointer>(registry);
        return make_intrusive<variable_state>(std::move(state));
    }
}

namespace meta_hpp
{
    inline pointer_type variable::get_type() const noexcept {
        return state_->index.get_type();
    }

    inline const std::string& variable::get_name() const noexcept {
        return state_->index.get_name();
    }

    inline uvalue variable::get() const {
        return state_->getter();
    }

    inline std::optional<uvalue> variable::safe_get() const {
        return state_->getter();
    }

    template < typename T >
    T variable::get_as() const {
        return get().template get_as<T>();
    }

    template < typename T >
    std::optional<T> variable::safe_get_as() const {
        return safe_get().value_or(uvalue{}).template safe_get_as<T>();
    }

    template < typename Value >
    void variable::set(Value&& value) const {
        using namespace detail;
        type_registry& registry{type_registry::instance()};
        const uarg vvalue{registry, std::forward<Value>(value)};
        state_->setter(vvalue);
    }

    template < typename Value >
    bool variable::safe_set(Value&& value) const {
        if ( is_settable_with(std::forward<Value>(value)) ) {
            set(std::forward<Value>(value));
            return true;
        }
        return false;
    }

    inline uvalue variable::operator()() const {
        return get();
    }

    template < typename Value >
    void variable::operator()(Value&& value) const {
        set(std::forward<Value>(value));
    }

    template < typename Value >
    bool variable::is_settable_with() const noexcept {
        using namespace detail;
        type_registry& registry{type_registry::instance()};
        const uarg_base vvalue{registry, type_list<Value>{}};
        return state_->is_settable_with(vvalue);
    }

    template < typename Value >
    bool variable::is_settable_with(Value&& value) const noexcept {
        using namespace detail;
        type_registry& registry{type_registry::instance()};
        const uarg_base vvalue{registry, std::forward<Value>(value)};
        return state_->is_settable_with(vvalue);
    }
}
