/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "meta_base.hpp"

#include "meta_binds.hpp"
#include "meta_binds/class_bind.hpp"
#include "meta_binds/enum_bind.hpp"
#include "meta_binds/scope_bind.hpp"

#include "meta_indices.hpp"
#include "meta_indices/ctor_index.hpp"
#include "meta_indices/dtor_index.hpp"
#include "meta_indices/evalue_index.hpp"
#include "meta_indices/function_index.hpp"
#include "meta_indices/member_index.hpp"
#include "meta_indices/method_index.hpp"
#include "meta_indices/parameter_index.hpp"
#include "meta_indices/scope_index.hpp"
#include "meta_indices/variable_index.hpp"

#include "meta_states.hpp"
#include "meta_states/ctor.hpp"
#include "meta_states/dtor.hpp"
#include "meta_states/evalue.hpp"
#include "meta_states/function.hpp"
#include "meta_states/member.hpp"
#include "meta_states/method.hpp"
#include "meta_states/scope.hpp"
#include "meta_states/variable.hpp"

#include "meta_types.hpp"
#include "meta_types/any_type.hpp"
#include "meta_types/array_type.hpp"
#include "meta_types/class_type.hpp"
#include "meta_types/ctor_type.hpp"
#include "meta_types/dtor_type.hpp"
#include "meta_types/enum_type.hpp"
#include "meta_types/function_type.hpp"
#include "meta_types/member_type.hpp"
#include "meta_types/method_type.hpp"
#include "meta_types/nullptr_type.hpp"
#include "meta_types/number_type.hpp"
#include "meta_types/pointer_type.hpp"
#include "meta_types/reference_type.hpp"
#include "meta_types/void_type.hpp"

#include "meta_value.hpp"
#include "meta_value/value.hpp"
#include "meta_value/vinvoke.hpp"

namespace meta_hpp
{
    using detail::resolve_type;
    using detail::resolve_polymorphic_type;

    using detail::resolve_scope;
}
