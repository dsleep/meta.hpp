/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2022, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../meta_base.hpp"
#include "../meta_registry.hpp"
#include "../meta_uvalue.hpp"

#include "../meta_detail/value_traits/deref_traits.hpp"
#include "../meta_detail/value_traits/equals_traits.hpp"
#include "../meta_detail/value_traits/index_traits.hpp"
#include "../meta_detail/value_traits/istream_traits.hpp"
#include "../meta_detail/value_traits/less_traits.hpp"
#include "../meta_detail/value_traits/ostream_traits.hpp"

#include "../meta_detail/value_utilities/utraits.hpp"

namespace meta_hpp
{
    struct uvalue::vtable_t final {
        const any_type type;

        void* (*const data)(storage_u& from) noexcept;
        const void* (*const cdata)(const storage_u& from) noexcept;

        void (*const move)(uvalue& from, uvalue& to) noexcept;
        void (*const copy)(const uvalue& from, uvalue& to);
        void (*const destroy)(uvalue& self) noexcept;

        uvalue (*const deref)(const uvalue& from);
        uvalue (*const index)(const uvalue& from, std::size_t);

        bool (*const less)(const uvalue&, const uvalue&);
        bool (*const equals)(const uvalue&, const uvalue&);

        std::istream& (*const istream)(std::istream&, uvalue&);
        std::ostream& (*const ostream)(std::ostream&, const uvalue&);

        template < typename T >
        static T* buffer_cast(buffer_t& buffer) noexcept {
            // NOLINTNEXTLINE(*-reinterpret-cast)
            return std::launder(reinterpret_cast<T*>(buffer.data));
        }

        template < typename T >
        static const T* buffer_cast(const buffer_t& buffer) noexcept {
            // NOLINTNEXTLINE(*-reinterpret-cast)
            return std::launder(reinterpret_cast<const T*>(buffer.data));
        }

        template < typename T >
        static T* storage_cast(storage_u& storage) noexcept {
            return std::visit(detail::overloaded {
                [](void* ptr) { return static_cast<T*>(ptr); },
                [](buffer_t& buffer) { return buffer_cast<T>(buffer); },
                [](...) -> T* { return nullptr; },
            }, storage);
        }

        template < typename T >
        static const T* storage_cast(const storage_u& storage) noexcept {
            return std::visit(detail::overloaded {
                [](const void* ptr) { return static_cast<const T*>(ptr); },
                [](const buffer_t& buffer) { return buffer_cast<T>(buffer); },
                [](...) -> const T* { return nullptr; },
            }, storage);
        }

        template < typename T >
        static void construct(uvalue& dst, T&& val) {
            using Tp = std::decay_t<T>;

            constexpr bool in_buffer =
                sizeof(Tp) <= sizeof(buffer_t) &&
                alignof(Tp) <= alignof(buffer_t) &&
                std::is_nothrow_move_constructible_v<Tp>;

            if constexpr ( in_buffer ) {
                std::construct_at(buffer_cast<Tp>(dst.storage_.emplace<buffer_t>()), std::forward<T>(val));
            } else {
                dst.storage_.emplace<void*>(std::make_unique<Tp>(std::forward<T>(val)).release());
            }

            dst.vtable_ = vtable_t::get<Tp>();
        }

        static void swap(uvalue& l, uvalue& r) noexcept {
            if ( (&l == &r) || (!l && !r) ) {
                return;
            }

            if ( l && r ) {
                if ( std::holds_alternative<buffer_t>(l.storage_) ) {
                    uvalue temp;
                    r.vtable_->move(r, temp);
                    l.vtable_->move(l, r);
                    temp.vtable_->move(temp, l);
                } else {
                    uvalue temp;
                    l.vtable_->move(l, temp);
                    r.vtable_->move(r, l);
                    temp.vtable_->move(temp, r);
                }
            } else {
                if ( l ) {
                    l.vtable_->move(l, r);
                } else {
                    r.vtable_->move(r, l);
                }
            }
        }

        template < typename Tp >
        // NOLINTNEXTLINE(*-cognitive-complexity)
        static vtable_t* get() {
            static vtable_t table{
                .type = resolve_type<Tp>(),

                .data = [](storage_u& from) noexcept -> void* {
                    return storage_cast<Tp>(from);
                },

                .cdata = [](const storage_u& from) noexcept -> const void* {
                    return storage_cast<Tp>(from);
                },

                .move = [](uvalue& from, uvalue& to) noexcept {
                    assert(from && !to); // NOLINT

                    std::visit(detail::overloaded {
                        [&to](void* ptr) {
                            Tp* src = static_cast<Tp*>(ptr);
                            to.storage_.emplace<void*>(src);
                        },
                        [&to](buffer_t& buffer) {
                            Tp& src = *buffer_cast<Tp>(buffer);
                            std::construct_at(buffer_cast<Tp>(to.storage_.emplace<buffer_t>()), std::move(src));
                            std::destroy_at(&src);
                        },
                        [](...){}
                    }, from.storage_);

                    to.vtable_ = from.vtable_;
                    from.vtable_ = nullptr;
                },

                .copy = [](const uvalue& from, uvalue& to){
                    assert(from && !to); // NOLINT

                    std::visit(detail::overloaded {
                        [&to](void* ptr) {
                            const Tp& src = *static_cast<const Tp*>(ptr);
                            to.storage_.emplace<void*>(std::make_unique<Tp>(src).release());
                        },
                        [&to](const buffer_t& buffer) {
                            const Tp& src = *buffer_cast<Tp>(buffer);
                            std::construct_at(buffer_cast<Tp>(to.storage_.emplace<buffer_t>()), src);
                        },
                        [](...){}
                    }, from.storage_);

                    to.vtable_ = from.vtable_;
                },

                .destroy = [](uvalue& self) noexcept {
                    assert(self); // NOLINT

                    std::visit(detail::overloaded {
                        [](void* ptr) {
                            Tp* src = static_cast<Tp*>(ptr);
                            std::unique_ptr<Tp>{src}.reset();
                        },
                        [](buffer_t& buffer) {
                            Tp& src = *buffer_cast<Tp>(buffer);
                            std::destroy_at(&src);
                        },
                        [](...){}
                    }, self.storage_);

                    self.vtable_ = nullptr;
                },

                .deref = +[]([[maybe_unused]] const uvalue& v) -> uvalue {
                    if constexpr ( detail::has_deref_traits<Tp> ) {
                        return detail::deref_traits<Tp>{}(v.get_as<Tp>());
                    } else {
                        detail::throw_exception_with("value type doesn't have value deref traits");
                    }
                },

                .index = +[]([[maybe_unused]] const uvalue& v, [[maybe_unused]] std::size_t i) -> uvalue {
                    if constexpr ( detail::has_index_traits<Tp> ) {
                        return detail::index_traits<Tp>{}(v.get_as<Tp>(), i);
                    } else {
                        detail::throw_exception_with("value type doesn't have value index traits");
                    }
                },

                .less = +[]([[maybe_unused]] const uvalue& l, [[maybe_unused]] const uvalue& r) -> bool {
                    if constexpr ( detail::has_less_traits<Tp> ) {
                        return detail::less_traits<Tp>{}(l.get_as<Tp>(), r.get_as<Tp>());
                    } else {
                        detail::throw_exception_with("value type doesn't have value less traits");
                    }
                },

                .equals = +[]([[maybe_unused]] const uvalue& l, [[maybe_unused]] const uvalue& r) -> bool {
                    if constexpr ( detail::has_equals_traits<Tp> ) {
                        return detail::equals_traits<Tp>{}(l.get_as<Tp>(), r.get_as<Tp>());
                    } else {
                        detail::throw_exception_with("value type doesn't have value equals traits");
                    }
                },

                .istream = +[]([[maybe_unused]] std::istream& is, [[maybe_unused]] uvalue& v) -> std::istream& {
                    if constexpr ( detail::has_istream_traits<Tp> && !detail::pointer_kind<Tp> ) {
                        return detail::istream_traits<Tp>{}(is, v.get_as<Tp>());
                    } else {
                        detail::throw_exception_with("value type doesn't have value istream traits");
                    }
                },

                .ostream = +[]([[maybe_unused]] std::ostream& os, [[maybe_unused]] const uvalue& v) -> std::ostream& {
                    if constexpr ( detail::has_ostream_traits<Tp> && !detail::pointer_kind<Tp> ) {
                        return detail::ostream_traits<Tp>{}(os, v.get_as<Tp>());
                    } else {
                        detail::throw_exception_with("value type doesn't have value ostream traits");
                    }
                },
            };

            return &table;
        }
    };
}

namespace meta_hpp
{
    inline uvalue::~uvalue() {
        reset();
    }

    inline uvalue::uvalue(uvalue&& other) noexcept {
        if ( other.vtable_ != nullptr ) {
            other.vtable_->move(other, *this);
        }
    }

    inline uvalue::uvalue(const uvalue& other) {
        if ( other.vtable_ != nullptr ) {
            other.vtable_->copy(other, *this);
        }
    }

    inline uvalue& uvalue::operator=(uvalue&& other) noexcept {
        if ( this != &other ) {
            uvalue{std::move(other)}.swap(*this);
        }
        return *this;
    }

    inline uvalue& uvalue::operator=(const uvalue& other) {
        if ( this != &other ) {
            uvalue{other}.swap(*this);
        }
        return *this;
    }

    template < detail::decay_non_value_kind T >
        requires std::copy_constructible<std::decay_t<T>>
    // NOLINTNEXTLINE(*-forwarding-reference-overload)
    uvalue::uvalue(T&& val) {
        vtable_t::construct(*this, std::forward<T>(val));
    }

    template < detail::decay_non_value_kind T >
        requires std::copy_constructible<std::decay_t<T>>
    uvalue& uvalue::operator=(T&& val) {
        uvalue{std::forward<T>(val)}.swap(*this);
        return *this;
    }

    inline bool uvalue::is_valid() const noexcept {
        return vtable_ != nullptr;
    }

    inline uvalue::operator bool() const noexcept {
        return is_valid();
    }

    inline void uvalue::reset() {
        if ( vtable_ != nullptr ) {
            vtable_->destroy(*this);
        }
    }

    inline void uvalue::swap(uvalue& other) noexcept {
        vtable_t::swap(*this, other);
    }

    inline const any_type& uvalue::get_type() const noexcept {
        static any_type void_type = resolve_type<void>();
        return vtable_ != nullptr ? vtable_->type : void_type;
    }

    inline void* uvalue::data() noexcept {
        return vtable_ != nullptr ? vtable_->data(storage_) : nullptr;
    }

    inline const void* uvalue::data() const noexcept {
        return vtable_ != nullptr ? vtable_->cdata(storage_) : nullptr;
    }

    inline const void* uvalue::cdata() const noexcept {
        return vtable_ != nullptr ? vtable_->cdata(storage_) : nullptr;
    }

    inline uvalue uvalue::operator*() const {
        return vtable_ != nullptr ? vtable_->deref(*this) : uvalue{};
    }

    inline uvalue uvalue::operator[](std::size_t index) const {
        return vtable_ != nullptr ? vtable_->index(*this, index) : uvalue{};
    }

    template < typename T >
    auto uvalue::get_as() -> std::conditional_t<detail::pointer_kind<T>, T, T&> {
        static_assert(std::same_as<T, std::decay_t<T>>);

        if constexpr ( detail::pointer_kind<T> ) {
            if ( T ptr = try_get_as<T>(); ptr || get_type().is_nullptr() ) {
                return ptr;
            }
        } else {
            if ( T* ptr = try_get_as<T>() ) {
                return *ptr;
            }
        }

        detail::throw_exception_with("bad value cast");
    }

    template < typename T >
    auto uvalue::get_as() const -> std::conditional_t<detail::pointer_kind<T>, T, const T&> {
        static_assert(std::same_as<T, std::decay_t<T>>);

        if constexpr ( detail::pointer_kind<T> ) {
            if ( T ptr = try_get_as<T>(); ptr || get_type().is_nullptr() ) {
                return ptr;
            }
        } else {
            if ( const T* ptr = try_get_as<T>() ) {
                return *ptr;
            }
        }

        detail::throw_exception_with("bad value cast");
    }

    template < typename T >
    // NOLINTNEXTLINE(*-cognitive-complexity)
    auto uvalue::try_get_as() noexcept -> std::conditional_t<detail::pointer_kind<T>, T, T*> {
        static_assert(std::same_as<T, std::decay_t<T>>);

        const any_type& from_type = get_type();
        const any_type& to_type = resolve_type<T>();

        const auto is_a = [](const any_type& base, const any_type& derived){
            return (base == derived)
                || (base.is_class() && derived.is_class() && base.as_class().is_base_of(derived.as_class()));
        };

        if constexpr ( detail::pointer_kind<T> ) {
            if ( to_type.is_pointer() && from_type.is_nullptr() ) {
                return static_cast<T>(nullptr);
            }

            if ( to_type.is_pointer() && from_type.is_pointer() ) {
                const pointer_type& to_type_ptr = to_type.as_pointer();
                const bool to_type_ptr_readonly = to_type_ptr.get_flags().has(pointer_flags::is_readonly);

                const pointer_type& from_type_ptr = from_type.as_pointer();
                const bool from_type_ptr_readonly = from_type_ptr.get_flags().has(pointer_flags::is_readonly);

                const any_type& to_data_type = to_type_ptr.get_data_type();
                const any_type& from_data_type = from_type_ptr.get_data_type();

                if ( to_type_ptr_readonly >= from_type_ptr_readonly ) {
                    void** from_data_ptr = static_cast<void**>(data());

                    if ( to_data_type.is_void() || to_data_type == from_data_type ) {
                        void* to_ptr = *from_data_ptr;
                        return static_cast<T>(to_ptr);
                    }

                    if ( is_a(to_data_type, from_data_type) ) {
                        const class_type& to_data_class = to_data_type.as_class();
                        const class_type& from_data_class = from_data_type.as_class();

                        void* to_ptr = detail::pointer_upcast(*from_data_ptr, from_data_class, to_data_class);
                        return static_cast<T>(to_ptr);
                    }
                }
            }
        }

        if constexpr ( !detail::pointer_kind<T> ) {
            if ( from_type == to_type ) {
                T* to_ptr = static_cast<T*>(data());
                return to_ptr;
            }

            if ( is_a(to_type, from_type) ) {
                const class_type& to_class = to_type.as_class();
                const class_type& from_class = from_type.as_class();

                T* to_ptr = static_cast<T*>(detail::pointer_upcast(data(), from_class, to_class));
                return to_ptr;
            }
        }

        return nullptr;
    }

    template < typename T >
    // NOLINTNEXTLINE(*-cognitive-complexity)
    auto uvalue::try_get_as() const noexcept -> std::conditional_t<detail::pointer_kind<T>, T, const T*> {
        static_assert(std::same_as<T, std::decay_t<T>>);

        const any_type& from_type = get_type();
        const any_type& to_type = resolve_type<T>();

        const auto is_a = [](const any_type& base, const any_type& derived){
            return (base == derived)
                || (base.is_class() && derived.is_class() && base.as_class().is_base_of(derived.as_class()));
        };

        if constexpr ( detail::pointer_kind<T> ) {
            if ( to_type.is_pointer() && from_type.is_nullptr() ) {
                return static_cast<T>(nullptr);
            }

            if ( to_type.is_pointer() && from_type.is_pointer() ) {
                const pointer_type& to_type_ptr = to_type.as_pointer();
                const bool to_type_ptr_readonly = to_type_ptr.get_flags().has(pointer_flags::is_readonly);

                const pointer_type& from_type_ptr = from_type.as_pointer();
                const bool from_type_ptr_readonly = from_type_ptr.get_flags().has(pointer_flags::is_readonly);

                const any_type& to_data_type = to_type_ptr.get_data_type();
                const any_type& from_data_type = from_type_ptr.get_data_type();

                if ( to_type_ptr_readonly >= from_type_ptr_readonly ) {
                    void* const* from_data_ptr = static_cast<void* const*>(data());

                    if ( to_data_type.is_void() || to_data_type == from_data_type ) {
                        void* to_ptr = *from_data_ptr;
                        return static_cast<T>(to_ptr);
                    }

                    if ( is_a(to_data_type, from_data_type) ) {
                        const class_type& to_data_class = to_data_type.as_class();
                        const class_type& from_data_class = from_data_type.as_class();

                        void* to_ptr = detail::pointer_upcast(*from_data_ptr, from_data_class, to_data_class);
                        return static_cast<T>(to_ptr);
                    }
                }
            }
        }

        if constexpr ( !detail::pointer_kind<T> ) {
            if ( from_type == to_type ) {
                const T* to_ptr = static_cast<const T*>(data());
                return to_ptr;
            }

            if ( is_a(to_type, from_type) ) {
                const class_type& to_class = to_type.as_class();
                const class_type& from_class = from_type.as_class();

                const T* to_ptr = static_cast<const T*>(detail::pointer_upcast(data(), from_class, to_class));
                return to_ptr;
            }
        }

        return nullptr;
    }
}

namespace meta_hpp
{
    template < typename T >
    [[nodiscard]] bool operator<(const uvalue& l, const T& r) {
        if ( !static_cast<bool>(l) ) {
            return true;
        }

        const any_type& l_type = l.get_type();
        const any_type& r_type = resolve_type<T>();

        return (l_type < r_type) || (l_type == r_type && l.get_as<T>() < r);
    }

    template < typename T >
    [[nodiscard]] bool operator<(const T& l, const uvalue& r) {
        if ( !static_cast<bool>(r) ) {
            return false;
        }

        const any_type& l_type = resolve_type<T>();
        const any_type& r_type = r.get_type();

        return (l_type < r_type) || (l_type == r_type && l < r.get_as<T>());
    }

    [[nodiscard]] inline bool operator<(const uvalue& l, const uvalue& r) {
        if ( !static_cast<bool>(r) ) {
            return false;
        }

        if ( !static_cast<bool>(l) ) {
            return true;
        }

        const any_type& l_type = l.get_type();
        const any_type& r_type = r.get_type();

        return (l_type < r_type) || (l_type == r_type && l.vtable_->less(l, r));
    }
}

namespace meta_hpp
{
    template < typename T >
    [[nodiscard]] bool operator==(const uvalue& l, const T& r) {
        if ( !static_cast<bool>(l) ) {
            return false;
        }

        const any_type& l_type = l.get_type();
        const any_type& r_type = resolve_type<T>();

        return l_type == r_type && l.get_as<T>() == r;
    }

    template < typename T >
    [[nodiscard]] bool operator==(const T& l, const uvalue& r) {
        if ( !static_cast<bool>(r) ) {
            return false;
        }

        const any_type& l_type = resolve_type<T>();
        const any_type& r_type = r.get_type();

        return l_type == r_type && l == r.get_as<T>();
    }

    [[nodiscard]] inline bool operator==(const uvalue& l, const uvalue& r) {
        if ( static_cast<bool>(l) != static_cast<bool>(r) ) {
            return false;
        }

        if ( !static_cast<bool>(l) ) {
            return true;
        }

        const any_type& l_type = l.get_type();
        const any_type& r_type = r.get_type();

        return l_type == r_type && l.vtable_->equals(l, r);
    }
}

namespace meta_hpp
{
    inline std::istream& operator>>(std::istream& is, uvalue& v) {
        return v.vtable_->istream(is, v);
    }

    inline std::ostream& operator<<(std::ostream& os, const uvalue& v) {
        return v.vtable_->ostream(os, v);
    }
}
