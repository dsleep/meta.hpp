/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2022, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include <cassert>
#include <cstddef>
#include <functional>
#include <new>
#include <type_traits>
#include <utility>

namespace meta_hpp::detail
{
    template < typename Function >
    class fixed_function;

    template < typename R, typename... Args >
    class fixed_function<R(Args...)> final {
    public:
        using result_type = R;

        fixed_function() = default;
        ~fixed_function() { reset(); }

        fixed_function(const fixed_function& other) = delete;
        fixed_function& operator=(const fixed_function& other) = delete;

        fixed_function(fixed_function&& other) noexcept {
            if ( other.vtable_ ) {
                other.vtable_->move(other, *this);
            }
        }

        fixed_function& operator=(fixed_function&& other) noexcept {
            if ( this != &other ) {
                fixed_function{std::move(other)}.swap(*this);
            }
            return *this;
        }

        template < typename Functor >
        fixed_function(Functor&& functor) {
            vtable_t::construct(*this, std::forward<Functor>(functor));
        }

        template < typename Functor >
        fixed_function& operator=(Functor&& functor) {
            fixed_function{std::forward<Functor>(functor)}.swap(*this);
            return *this;
        }

        [[nodiscard]] bool is_valid() const noexcept {
            return !!vtable_;
        }

        [[nodiscard]] explicit operator bool() const noexcept {
            return is_valid();
        }

        R operator()(Args... args) const {
            assert(vtable_ && "bad function call");
            return vtable_->call(*this, std::forward<Args>(args)...);
        }

        void reset() noexcept {
            if ( vtable_ ) {
                vtable_->destroy(*this);
            }
        }

        void swap(fixed_function& other) noexcept {
            vtable_t::swap(*this, other);
        }
    private:
        struct vtable_t;
        vtable_t* vtable_{};
    private:
        struct buffer_t final {
            // NOLINTNEXTLINE(*-avoid-c-arrays)
            alignas(std::max_align_t) std::byte data[sizeof(void*) * 2];
        } buffer_{};
    };

    template < typename Function >
    inline void swap(fixed_function<Function>& l, fixed_function<Function>& r) noexcept {
        l.swap(r);
    }
}

namespace meta_hpp::detail
{
    template < typename R, typename... Args >
    struct fixed_function<R(Args...)>::vtable_t final {
        R (*const call)(const fixed_function& self, Args... args);
        void (*const move)(fixed_function& from, fixed_function& to) noexcept;
        void (*const destroy)(fixed_function& self);

        template < typename T >
        static T* buffer_cast(buffer_t& buffer) noexcept {
            // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
            return std::launder(reinterpret_cast<T*>(buffer.data));
        }

        template < typename T >
        static const T* buffer_cast(const buffer_t& buffer) noexcept {
            // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
            return std::launder(reinterpret_cast<const T*>(buffer.data));
        }

        template < typename Fp >
        static vtable_t* get() {
            static vtable_t table{
                .call = +[](const fixed_function& self, Args... args) -> R {
                    assert(self);

                    const Fp& src = *buffer_cast<Fp>(self.buffer_);
                    return std::invoke(src, std::forward<Args>(args)...);
                },
                .move = +[](fixed_function& from, fixed_function& to) noexcept {
                    assert(from && !to);

                    Fp& src = *buffer_cast<Fp>(from.buffer_);
                    ::new (buffer_cast<Fp>(to.buffer_)) Fp(std::move(src));
                    src.~Fp();

                    to.vtable_ = from.vtable_;
                    from.vtable_ = nullptr;
                },
                .destroy = +[](fixed_function& self){
                    assert(self);

                    Fp& src = *buffer_cast<Fp>(self.buffer_);
                    src.~Fp();

                    self.vtable_ = nullptr;
                },
            };
            return &table;
        }

        template < typename Functor >
        static void construct(fixed_function& dst, Functor&& functor) {
            using Fp = std::decay_t<Functor>;

            static_assert(sizeof(Fp) <= sizeof(buffer_t));
            static_assert(alignof(Fp) <= alignof(buffer_t));
            static_assert(std::is_invocable_r_v<R, Fp, Args...>);
            static_assert(std::is_nothrow_move_constructible_v<Fp>);

            ::new (buffer_cast<Fp>(dst.buffer_)) Fp(std::forward<Functor>(functor));

            dst.vtable_ = vtable_t::get<Fp>();
        }

        static void swap(fixed_function& l, fixed_function& r) noexcept {
            if ( (&l == &r) || (!l && !r) ) {
                return;
            }

            if ( l && r ) {
                fixed_function temp;
                r.vtable_->move(r, temp);
                l.vtable_->move(l, r);
                temp.vtable_->move(temp, l);
            } else {
                if ( l ) {
                    l.vtable_->move(l, r);
                } else {
                    r.vtable_->move(r, l);
                }
            }
        }
    };
}

namespace meta_hpp::detail
{
    namespace impl
    {
        template < typename F >
        struct strip_signature_impl;

        template < typename R, typename C, typename... Args >
        struct strip_signature_impl<R(C::*)(Args...)> { using type = R(Args...); };
        template < typename R, typename C, typename... Args >
        struct strip_signature_impl<R(C::*)(Args...) const> { using type = R(Args...); };
        template < typename R, typename C, typename... Args >
        struct strip_signature_impl<R(C::*)(Args...) &> { using type = R(Args...); };
        template < typename R, typename C, typename... Args >
        struct strip_signature_impl<R(C::*)(Args...) const &> { using type = R(Args...); };

        template < typename R, typename C, typename... Args >
        struct strip_signature_impl<R(C::*)(Args...) noexcept> { using type = R(Args...); };
        template < typename R, typename C, typename... Args >
        struct strip_signature_impl<R(C::*)(Args...) const noexcept> { using type = R(Args...); };
        template < typename R, typename C, typename... Args >
        struct strip_signature_impl<R(C::*)(Args...) & noexcept> { using type = R(Args...); };
        template < typename R, typename C, typename... Args >
        struct strip_signature_impl<R(C::*)(Args...) const & noexcept> { using type = R(Args...); };

        template < typename F >
        using strip_signature_impl_t = typename strip_signature_impl<F>::type;
    }

    template < typename R, typename... Args >
    fixed_function(R(*)(Args...)) -> fixed_function<R(Args...)>;

    template < typename Functor
             , typename Signature = impl::strip_signature_impl_t<decltype(&Functor::operator())> >
    fixed_function(Functor) -> fixed_function<Signature>;
}
