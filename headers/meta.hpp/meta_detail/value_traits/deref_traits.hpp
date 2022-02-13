/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2022, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../../meta_base.hpp"
#include "../../meta_value.hpp"

namespace meta_hpp::detail
{
    template < typename T >
    struct deref_traits;

    template < typename T >
    concept has_deref_traits = requires(const T& v) {
        { deref_traits<T>{}(v) } -> stdex::convertible_to<uvalue>;
    };
}

namespace meta_hpp::detail
{
    template < stdex::copy_constructible T >
    struct deref_traits<T*> {
        uvalue operator()(T* v) const {
            return uvalue{*v};
        }
    };

    template < stdex::copy_constructible T >
    struct deref_traits<const T*> {
        uvalue operator()(const T* v) const {
            return uvalue{*v};
        }
    };

    template < stdex::copy_constructible T >
    struct deref_traits<std::shared_ptr<T>> {
        uvalue operator()(const std::shared_ptr<T>& v) const {
            return uvalue{*v};
        }
    };

    template < stdex::copy_constructible T >
    struct deref_traits<std::unique_ptr<T>> {
        uvalue operator()(const std::unique_ptr<T>& v) const {
            return uvalue{*v};
        }
    };
}