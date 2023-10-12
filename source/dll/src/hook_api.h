/**
 * @file hook_api.h
 * @author UnnamedOrange
 * @brief Define the APIs to hook.
 * @version 0.1.0 Adapt to CatTuber 0.5.2.
 * @version 0.0.0
 * @date 2023-08-04
 *
 * @copyright Copyright (c) UnnamedOrange. Licensed under the MIT License.
 * See the LICENSE file in the repository root for full license text.
 */

#pragma once

#include <Windows.h>
#include <dinput.h>

namespace orange {
    using SGetDeviceState = HRESULT WINAPI(IDirectInputDevice8W* self, DWORD cbData, LPVOID lpvData);
    using SGetDeviceData = HRESULT WINAPI(IDirectInputDevice8W* self, DWORD cbObjectData, LPDIDEVICEOBJECTDATA rgdod,
                                          LPDWORD pdwInOut, DWORD dwFlags);

    extern SGetDeviceState* g_original_keyboard_GetDeviceState;
    extern SGetDeviceData* g_original_keyboard_GetDeviceData;
    extern SGetDeviceState* g_original_mouse_GetDeviceState;

    HRESULT WINAPI hook_keyboard_GetDeviceState(IDirectInputDevice8W* self, DWORD cbData, LPVOID lpvData);
    HRESULT WINAPI hook_keyboard_GetDeviceData(IDirectInputDevice8W* self, DWORD cbObjectData,
                                               LPDIDEVICEOBJECTDATA rgdod, LPDWORD pdwInOut, DWORD dwFlags);
    HRESULT WINAPI hook_mouse_GetDeviceState(IDirectInputDevice8W* self, DWORD cbData, LPVOID lpvData);
} // namespace orange
