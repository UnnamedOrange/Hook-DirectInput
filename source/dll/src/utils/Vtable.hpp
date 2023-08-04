/**
 * @file Vtable.hpp
 * @author UnnamedOrange
 * @brief Utilities for vtable.
 * @version 0.0.0
 * @date 2023-08-04
 *
 * @copyright Copyright (c) UnnamedOrange. Licensed under the MIT License.
 * See the LICENSE file in the repository root for full license text.
 */

#pragma once

#include <cstdint>

namespace orange {
    class Vtable final {
    public:
        template <typename T>
        static void* get_virtual_method_address(const T& obj, size_t index) {
            auto vtable = *reinterpret_cast<const uintptr_t*>(&obj);
            auto method_address_holder = vtable + index * sizeof(uintptr_t);
            return *reinterpret_cast<void**>(method_address_holder);
        }
    };
} // namespace orange
