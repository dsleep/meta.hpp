/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2022, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "meta_base.hpp"
#include "meta_states.hpp"
#include "meta_types.hpp"

#include "meta_detail/state_registry.hpp"
#include "meta_detail/type_registry.hpp"

namespace meta_hpp
{
    template < typename T >
    [[nodiscard]] auto resolve_type() {
        using namespace detail;
        type_registry& registry = type_registry::instance();
        return registry.resolve_type<std::remove_cv_t<T>>();
    }

    template < typename... Ts >
    [[nodiscard]] std::vector<any_type> resolve_types() {
        return { resolve_type<Ts>()... };
    }
}

namespace meta_hpp
{
    template < typename T >
    // NOLINTNEXTLINE(readability-named-parameter)
    [[nodiscard]] auto resolve_type(T&&) {
        return resolve_type<std::remove_reference_t<T>>();
    }

    template < typename... Ts >
    // NOLINTNEXTLINE(readability-named-parameter)
    [[nodiscard]] std::vector<any_type> resolve_types(type_list<Ts...>) {
        return { resolve_type<Ts>()... };
    }
}

namespace meta_hpp
{
    template < detail::class_kind Class, typename... Args >
    constructor_type resolve_constructor_type() {
        using namespace detail;
        type_registry& registry = type_registry::instance();
        return registry.resolve_constructor_type<Class, Args...>();
    }

    template < detail::class_kind Class >
    destructor_type resolve_destructor_type() {
        using namespace detail;
        type_registry& registry = type_registry::instance();
        return registry.resolve_destructor_type<Class>();
    }
}

namespace meta_hpp
{
    template < typename T >
    [[nodiscard]] any_type resolve_polymorphic_type(T&& v) noexcept {
    #ifndef META_HPP_NO_RTTI
        using namespace detail;
        type_registry& registry = type_registry::instance();
        return registry.get_type_by_rtti(typeid(v));
    #else
        (void)v;
        return any_type{};
    #endif
    }
}

namespace meta_hpp
{
    [[nodiscard]] inline scope resolve_scope(std::string_view name) {
        using namespace detail;
        state_registry& registry = state_registry::instance();
        return registry.resolve_scope(name);
    }
}