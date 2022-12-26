/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2022, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../meta_base.hpp"
#include "../meta_states.hpp"

namespace meta_hpp::detail
{
    class state_registry final {
    public:
        class locker final {
        public:
            explicit locker() : lock_{instance().mutex_} {}
            ~locker() = default;

            locker(locker&&) = default;
            locker& operator=(locker&&) = default;

            locker(const locker&) = delete;
            locker& operator=(const locker&) = delete;
        private:
            std::unique_lock<std::recursive_mutex> lock_;
        };

        [[nodiscard]] static state_registry& instance() {
            static state_registry instance;
            return instance;
        }
    public:
        [[nodiscard]] scope get_scope_by_name(std::string_view name) const noexcept {
            const locker lock;

            if ( auto iter = scopes_.find(name); iter != scopes_.end() ) {
                return iter->second;
            }

            return scope{};
        }

        [[nodiscard]] scope resolve_scope(std::string_view name) {
            const locker lock;

            if ( auto iter = scopes_.find(name); iter != scopes_.end() ) {
                return iter->second;
            }

            auto state = scope_state::make(std::string{name}, metadata_map{});
            return scopes_.insert_or_assign(std::string{name}, std::move(state)).first->second;
        }
    private:
        state_registry() = default;
    private:
        std::recursive_mutex mutex_;
        std::map<std::string, scope, std::less<>> scopes_;
    };
}
