/**
 * @file hook_api.cpp
 * @author UnnamedOrange
 * @brief Define the APIs to hook.
 * @version 0.0.0
 * @date 2023-08-04
 *
 * @copyright Copyright (c) UnnamedOrange. Licensed under the MIT License.
 * See the LICENSE file in the repository root for full license text.
 */

#include "hook_api.h"

#include <span>

namespace orange {
    SGetDeviceState* g_original_keyboard_GetDeviceState;
    SGetDeviceState* g_original_mouse_GetDeviceState;

    HRESULT hook_keyboard_GetDeviceState(IDirectInputDevice8* self, DWORD cbData, LPVOID lpvData) {
        auto result = g_original_keyboard_GetDeviceState(self, cbData, lpvData);
        if (FAILED(result)) {
            return result;
        }
        return result;
    }
    HRESULT hook_mouse_GetDeviceState(IDirectInputDevice8* self, DWORD cbData, LPVOID lpvData) {
        return g_original_mouse_GetDeviceState(self, cbData, lpvData);
    }
} // namespace orange
