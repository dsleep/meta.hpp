/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "meta_base.hpp"

#include "meta_detail/type_family.hpp"

#include "meta_detail/type_traits/array_traits.hpp"
#include "meta_detail/type_traits/class_traits.hpp"
#include "meta_detail/type_traits/ctor_traits.hpp"
#include "meta_detail/type_traits/dtor_traits.hpp"
#include "meta_detail/type_traits/enum_traits.hpp"
#include "meta_detail/type_traits/function_traits.hpp"
#include "meta_detail/type_traits/member_traits.hpp"
#include "meta_detail/type_traits/method_traits.hpp"
#include "meta_detail/type_traits/number_traits.hpp"
#include "meta_detail/type_traits/pointer_traits.hpp"
#include "meta_detail/type_traits/reference_traits.hpp"

namespace meta_hpp
{
    using array_flags = detail::array_flags;
    using class_flags = detail::class_flags;
    using ctor_flags = detail::ctor_flags;
    using dtor_flags = detail::dtor_flags;
    using enum_flags = detail::enum_flags;
    using function_flags = detail::function_flags;
    using member_flags = detail::member_flags;
    using method_flags = detail::method_flags;
    using number_flags = detail::number_flags;
    using pointer_flags = detail::pointer_flags;
    using reference_flags = detail::reference_flags;
}

namespace meta_hpp
{
    class any_type final {
    public:
        explicit any_type() = default;

        [[nodiscard]] bool is_valid() const noexcept;
        [[nodiscard]] explicit operator bool() const noexcept;

        [[nodiscard]] type_id get_id() const noexcept;
        [[nodiscard]] type_kind get_kind() const noexcept;

        any_type(const array_type& other) noexcept;
        any_type(const class_type& other) noexcept;
        any_type(const ctor_type& other) noexcept;
        any_type(const dtor_type& other) noexcept;
        any_type(const enum_type& other) noexcept;
        any_type(const function_type& other) noexcept;
        any_type(const member_type& other) noexcept;
        any_type(const method_type& other) noexcept;
        any_type(const nullptr_type& other) noexcept;
        any_type(const number_type& other) noexcept;
        any_type(const pointer_type& other) noexcept;
        any_type(const reference_type& other) noexcept;
        any_type(const void_type& other) noexcept;

        [[nodiscard]] bool is_array() const noexcept;
        [[nodiscard]] bool is_class() const noexcept;
        [[nodiscard]] bool is_ctor() const noexcept;
        [[nodiscard]] bool is_dtor() const noexcept;
        [[nodiscard]] bool is_enum() const noexcept;
        [[nodiscard]] bool is_function() const noexcept;
        [[nodiscard]] bool is_member() const noexcept;
        [[nodiscard]] bool is_method() const noexcept;
        [[nodiscard]] bool is_nullptr() const noexcept;
        [[nodiscard]] bool is_number() const noexcept;
        [[nodiscard]] bool is_pointer() const noexcept;
        [[nodiscard]] bool is_reference() const noexcept;
        [[nodiscard]] bool is_void() const noexcept;

        [[nodiscard]] array_type as_array() const noexcept;
        [[nodiscard]] class_type as_class() const noexcept;
        [[nodiscard]] ctor_type as_ctor() const noexcept;
        [[nodiscard]] dtor_type as_dtor() const noexcept;
        [[nodiscard]] enum_type as_enum() const noexcept;
        [[nodiscard]] function_type as_function() const noexcept;
        [[nodiscard]] member_type as_member() const noexcept;
        [[nodiscard]] method_type as_method() const noexcept;
        [[nodiscard]] nullptr_type as_nullptr() const noexcept;
        [[nodiscard]] number_type as_number() const noexcept;
        [[nodiscard]] pointer_type as_pointer() const noexcept;
        [[nodiscard]] reference_type as_reference() const noexcept;
        [[nodiscard]] void_type as_void() const noexcept;
    private:
        detail::type_data_base_ptr data_;
        friend auto detail::type_access<any_type>(const any_type&);
    };

    class array_type final {
    public:
        array_type() = default;
        array_type(detail::array_type_data_ptr data);

        [[nodiscard]] bool is_valid() const noexcept;
        [[nodiscard]] explicit operator bool() const noexcept;

        [[nodiscard]] type_id get_id() const noexcept;
        [[nodiscard]] bitflags<array_flags> get_flags() const noexcept;

        [[nodiscard]] std::size_t get_extent() const noexcept;
        [[nodiscard]] any_type get_data_type() const noexcept;
    private:
        detail::array_type_data_ptr data_;
        friend auto detail::type_access<array_type>(const array_type&);
    };

    class class_type final {
    public:
        class_type() = default;
        class_type(detail::class_type_data_ptr data);

        [[nodiscard]] bool is_valid() const noexcept;
        [[nodiscard]] explicit operator bool() const noexcept;

        [[nodiscard]] type_id get_id() const noexcept;
        [[nodiscard]] bitflags<class_flags> get_flags() const noexcept;

        [[nodiscard]] std::size_t get_size() const noexcept;

        [[nodiscard]] std::size_t get_arity() const noexcept;
        [[nodiscard]] any_type get_argument_type(std::size_t index) const noexcept;
        [[nodiscard]] const std::vector<any_type>& get_argument_types() const noexcept;

        [[nodiscard]] const ctor_map& get_ctors() const noexcept;
        [[nodiscard]] const dtor_map& get_dtors() const noexcept;
        [[nodiscard]] const class_set& get_bases() const noexcept;
        [[nodiscard]] const function_map& get_functions() const noexcept;
        [[nodiscard]] const member_map& get_members() const noexcept;
        [[nodiscard]] const method_map& get_methods() const noexcept;
        [[nodiscard]] const variable_map& get_variables() const noexcept;

        template < typename... Args >
        [[nodiscard]] value create(Args&&... args) const;

        template < typename... Args >
        [[nodiscard]] value operator()(Args&&... args) const;

        template < typename Arg >
        bool destroy(Arg&& ptr) const;

        template < detail::class_kind Derived >
        [[nodiscard]] bool is_base_of() const noexcept;
        [[nodiscard]] bool is_base_of(const class_type& derived) const noexcept;

        template < detail::class_kind Base >
        [[nodiscard]] bool is_derived_from() const noexcept;
        [[nodiscard]] bool is_derived_from(const class_type& base) const noexcept;

        [[nodiscard]] function get_function(std::string_view name) const noexcept;
        [[nodiscard]] member get_member(std::string_view name) const noexcept;
        [[nodiscard]] method get_method(std::string_view name) const noexcept;
        [[nodiscard]] variable get_variable(std::string_view name) const noexcept;

        template < typename... Args >
        [[nodiscard]] ctor get_ctor_with() const noexcept;
        [[nodiscard]] ctor get_ctor_with(const std::vector<any_type>& args) const noexcept;
        [[nodiscard]] ctor get_ctor_with(std::initializer_list<any_type> args) const noexcept;

        template < typename... Args >
        [[nodiscard]] function get_function_with(std::string_view name) const noexcept;
        [[nodiscard]] function get_function_with(std::string_view name, const std::vector<any_type>& args) const noexcept;
        [[nodiscard]] function get_function_with(std::string_view name, std::initializer_list<any_type> args) const noexcept;

        template < typename... Args >
        [[nodiscard]] method get_method_with(std::string_view name) const noexcept;
        [[nodiscard]] method get_method_with(std::string_view name, const std::vector<any_type>& args) const noexcept;
        [[nodiscard]] method get_method_with(std::string_view name, std::initializer_list<any_type> args) const noexcept;
    private:
        detail::class_type_data_ptr data_;
        friend auto detail::type_access<class_type>(const class_type&);
    };

    class ctor_type final {
    public:
        ctor_type() = default;
        ctor_type(detail::ctor_type_data_ptr data);

        [[nodiscard]] bool is_valid() const noexcept;
        [[nodiscard]] explicit operator bool() const noexcept;

        [[nodiscard]] type_id get_id() const noexcept;
        [[nodiscard]] bitflags<ctor_flags> get_flags() const noexcept;

        [[nodiscard]] std::size_t get_arity() const noexcept;
        [[nodiscard]] any_type get_class_type() const noexcept;
        [[nodiscard]] any_type get_argument_type(std::size_t index) const noexcept;
        [[nodiscard]] const std::vector<any_type>& get_argument_types() const noexcept;
    private:
        detail::ctor_type_data_ptr data_;
        friend auto detail::type_access<ctor_type>(const ctor_type&);
    };

    class dtor_type final {
    public:
        dtor_type() = default;
        dtor_type(detail::dtor_type_data_ptr data);

        [[nodiscard]] bool is_valid() const noexcept;
        [[nodiscard]] explicit operator bool() const noexcept;

        [[nodiscard]] type_id get_id() const noexcept;
        [[nodiscard]] bitflags<dtor_flags> get_flags() const noexcept;

        [[nodiscard]] any_type get_class_type() const noexcept;
    private:
        detail::dtor_type_data_ptr data_;
        friend auto detail::type_access<dtor_type>(const dtor_type&);
    };

    class enum_type final {
    public:
        enum_type() = default;
        enum_type(detail::enum_type_data_ptr data);

        [[nodiscard]] bool is_valid() const noexcept;
        [[nodiscard]] explicit operator bool() const noexcept;

        [[nodiscard]] type_id get_id() const noexcept;
        [[nodiscard]] bitflags<enum_flags> get_flags() const noexcept;

        [[nodiscard]] number_type get_underlying_type() const noexcept;

        [[nodiscard]] const evalue_map& get_evalues() const noexcept;

        [[nodiscard]] evalue get_evalue(std::string_view name) const noexcept;

        template < typename Value >
        [[nodiscard]] std::string_view value_to_name(Value&& value) const noexcept;
        [[nodiscard]] value name_to_value(std::string_view name) const noexcept;
    private:
        detail::enum_type_data_ptr data_;
        friend auto detail::type_access<enum_type>(const enum_type&);
    };

    class function_type final {
    public:
        function_type() = default;
        function_type(detail::function_type_data_ptr data);

        [[nodiscard]] bool is_valid() const noexcept;
        [[nodiscard]] explicit operator bool() const noexcept;

        [[nodiscard]] type_id get_id() const noexcept;
        [[nodiscard]] bitflags<function_flags> get_flags() const noexcept;

        [[nodiscard]] std::size_t get_arity() const noexcept;
        [[nodiscard]] any_type get_return_type() const noexcept;
        [[nodiscard]] any_type get_argument_type(std::size_t index) const noexcept;
        [[nodiscard]] const std::vector<any_type>& get_argument_types() const noexcept;
    private:
        detail::function_type_data_ptr data_;
        friend auto detail::type_access<function_type>(const function_type&);
    };

    class member_type final {
    public:
        member_type() = default;
        member_type(detail::member_type_data_ptr data);

        [[nodiscard]] bool is_valid() const noexcept;
        [[nodiscard]] explicit operator bool() const noexcept;

        [[nodiscard]] type_id get_id() const noexcept;
        [[nodiscard]] bitflags<member_flags> get_flags() const noexcept;

        [[nodiscard]] class_type get_owner_type() const noexcept;
        [[nodiscard]] any_type get_value_type() const noexcept;
    private:
        detail::member_type_data_ptr data_;
        friend auto detail::type_access<member_type>(const member_type&);
    };

    class method_type final {
    public:
        method_type() = default;
        method_type(detail::method_type_data_ptr data);

        [[nodiscard]] bool is_valid() const noexcept;
        [[nodiscard]] explicit operator bool() const noexcept;

        [[nodiscard]] type_id get_id() const noexcept;
        [[nodiscard]] bitflags<method_flags> get_flags() const noexcept;

        [[nodiscard]] std::size_t get_arity() const noexcept;
        [[nodiscard]] class_type get_owner_type() const noexcept;
        [[nodiscard]] any_type get_return_type() const noexcept;
        [[nodiscard]] any_type get_argument_type(std::size_t index) const noexcept;
        [[nodiscard]] const std::vector<any_type>& get_argument_types() const noexcept;
    private:
        detail::method_type_data_ptr data_;
        friend auto detail::type_access<method_type>(const method_type&);
    };

    class nullptr_type final {
    public:
        nullptr_type() = default;
        nullptr_type(detail::nullptr_type_data_ptr data);

        [[nodiscard]] bool is_valid() const noexcept;
        [[nodiscard]] explicit operator bool() const noexcept;

        [[nodiscard]] type_id get_id() const noexcept;
    private:
        detail::nullptr_type_data_ptr data_;
        friend auto detail::type_access<nullptr_type>(const nullptr_type&);
    };

    class number_type final {
    public:
        number_type() = default;
        number_type(detail::number_type_data_ptr data);

        [[nodiscard]] bool is_valid() const noexcept;
        [[nodiscard]] explicit operator bool() const noexcept;

        [[nodiscard]] type_id get_id() const noexcept;
        [[nodiscard]] bitflags<number_flags> get_flags() const noexcept;

        [[nodiscard]] std::size_t get_size() const noexcept;
    private:
        detail::number_type_data_ptr data_;
        friend auto detail::type_access<number_type>(const number_type&);
    };

    class pointer_type final {
    public:
        pointer_type() = default;
        pointer_type(detail::pointer_type_data_ptr data);

        [[nodiscard]] bool is_valid() const noexcept;
        [[nodiscard]] explicit operator bool() const noexcept;

        [[nodiscard]] type_id get_id() const noexcept;
        [[nodiscard]] bitflags<pointer_flags> get_flags() const noexcept;

        [[nodiscard]] any_type get_data_type() const noexcept;
    private:
        detail::pointer_type_data_ptr data_;
        friend auto detail::type_access<pointer_type>(const pointer_type&);
    };

    class reference_type final {
    public:
        reference_type() = default;
        reference_type(detail::reference_type_data_ptr data);

        [[nodiscard]] bool is_valid() const noexcept;
        [[nodiscard]] explicit operator bool() const noexcept;

        [[nodiscard]] type_id get_id() const noexcept;
        [[nodiscard]] bitflags<reference_flags> get_flags() const noexcept;

        [[nodiscard]] any_type get_data_type() const noexcept;
    private:
        detail::reference_type_data_ptr data_;
        friend auto detail::type_access<reference_type>(const reference_type&);
    };

    class void_type final {
    public:
        void_type() = default;
        void_type(detail::void_type_data_ptr data);

        [[nodiscard]] bool is_valid() const noexcept;
        [[nodiscard]] explicit operator bool() const noexcept;

        [[nodiscard]] type_id get_id() const noexcept;
    private:
        detail::void_type_data_ptr data_;
        friend auto detail::type_access<void_type>(const void_type&);
    };
}

namespace meta_hpp
{
    template < detail::type_family T, detail::type_family U >
    [[nodiscard]] bool operator<(const T& l, const U& r) noexcept {
        if ( !static_cast<bool>(r) ) {
            return false;
        }

        if ( !static_cast<bool>(l) ) {
            return true;
        }

        return l.get_id() < r.get_id();
    }

    template < detail::type_family T, detail::type_family U >
    [[nodiscard]] bool operator==(const T& l, const U& r) noexcept {
        if ( static_cast<bool>(l) != static_cast<bool>(r) ) {
            return false;
        }

        if ( !static_cast<bool>(l) ) {
            return true;
        }

        return l.get_id() == r.get_id();
    }

    template < detail::type_family T, detail::type_family U >
    [[nodiscard]] bool operator!=(const T& l, const U& r) noexcept {
        return !(l == r);
    }
}

namespace meta_hpp::detail
{
    struct type_data_base {
        const type_id id;
        const type_kind kind;

        explicit type_data_base(type_id id, type_kind kind)
        : id{id}
        , kind{kind} {}
    };

    struct array_type_data final : type_data_base {
        const bitflags<array_flags> flags;
        const std::size_t extent;
        const any_type data_type;

        template < array_kind Array >
        explicit array_type_data(type_list<Array>);
    };

    struct class_type_data final : type_data_base {
        const bitflags<class_flags> flags;
        const std::size_t size;
        const std::vector<any_type> argument_types;

        ctor_map ctors;
        dtor_map dtors;
        class_set bases;
        function_map functions;
        member_map members;
        method_map methods;
        variable_map variables;

        struct base_info final {
            using upcast_fptr = void*(*)(void*);
            const upcast_fptr upcast;
        };

        using base_info_map = std::map<class_type, base_info, std::less<>>;
        base_info_map bases_info;

        template < class_kind Class >
        explicit class_type_data(type_list<Class>);
    };

    struct ctor_type_data final : type_data_base {
        const bitflags<ctor_flags> flags;
        const any_type class_type;
        const std::vector<any_type> argument_types;

        template < class_kind Class, typename... Args >
        explicit ctor_type_data(type_list<Class>, type_list<Args...>);
    };

    struct dtor_type_data final : type_data_base {
        const bitflags<dtor_flags> flags;
        const any_type class_type;

        template < class_kind Class >
        explicit dtor_type_data(type_list<Class>);
    };

    struct enum_type_data final : type_data_base {
        const bitflags<enum_flags> flags;
        const number_type underlying_type;

        evalue_map evalues;

        template < enum_kind Enum >
        explicit enum_type_data(type_list<Enum>);
    };

    struct function_type_data final : type_data_base {
        const bitflags<function_flags> flags;
        const any_type return_type;
        const std::vector<any_type> argument_types;

        template < function_kind Function >
        explicit function_type_data(type_list<Function>);
    };

    struct member_type_data final : type_data_base {
        const bitflags<member_flags> flags;
        const class_type owner_type;
        const any_type value_type;

        template < member_kind Member >
        explicit member_type_data(type_list<Member>);
    };

    struct method_type_data final : type_data_base {
        const bitflags<method_flags> flags;
        const class_type owner_type;
        const any_type return_type;
        const std::vector<any_type> argument_types;

        template < method_kind Method >
        explicit method_type_data(type_list<Method>);
    };

    struct nullptr_type_data final : type_data_base {
        template < nullptr_kind Nullptr >
        explicit nullptr_type_data(type_list<Nullptr>);
    };

    struct number_type_data final : type_data_base {
        const bitflags<number_flags> flags;
        const std::size_t size;

        template < number_kind Number >
        explicit number_type_data(type_list<Number>);
    };

    struct pointer_type_data final : type_data_base {
        const bitflags<pointer_flags> flags;
        const any_type data_type;

        template < pointer_kind Pointer >
        explicit pointer_type_data(type_list<Pointer>);
    };

    struct reference_type_data final : type_data_base {
        const bitflags<reference_flags> flags;
        const any_type data_type;

        template < reference_kind Reference >
        explicit reference_type_data(type_list<Reference>);
    };

    struct void_type_data final : type_data_base {
        template < void_kind Void >
        explicit void_type_data(type_list<Void>);
    };
}
