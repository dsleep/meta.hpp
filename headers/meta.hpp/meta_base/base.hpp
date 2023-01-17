/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2023, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#if !defined(META_HPP_NO_EXCEPTIONS) && !defined(__cpp_exceptions)
#  define META_HPP_NO_EXCEPTIONS
#endif

#if !defined(META_HPP_NO_RTTI) && !defined(__cpp_rtti)
#  define META_HPP_NO_RTTI
#endif

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdlib>

#include <algorithm>
#include <array>
#include <atomic>
#include <concepts>
#include <deque>
#include <functional>
#include <initializer_list>
#include <map>
#include <memory>
#include <mutex>
#include <set>
#include <span>
#include <string_view>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>
#include <variant>
#include <vector>

#if !defined(META_HPP_NO_EXCEPTIONS)
#  include <stdexcept>
#endif

#if !defined(META_HPP_NO_RTTI)
#  include <typeindex>
#  include <typeinfo>
#endif
