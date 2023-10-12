/**
 * @file hook_api.cpp
 * @author UnnamedOrange
 * @brief Define the APIs to hook.
 * @version 0.1.0 Adapt to CatTuber 0.5.2.
 * @version 0.0.0
 * @date 2023-08-04
 *
 * @copyright Copyright (c) UnnamedOrange. Licensed under the MIT License.
 * See the LICENSE file in the repository root for full license text.
 */

#include "hook_api.h"

#include <limits>
#include <memory>
#include <span>

#include <Windows.h>

#include "MemoryReaderOsu.h"

namespace orange {
    SGetDeviceState* g_original_keyboard_GetDeviceState;
    SGetDeviceData* g_original_keyboard_GetDeviceData;
    SGetDeviceState* g_original_mouse_GetDeviceState;

    extern std::unique_ptr<MemoryReaderOsu> g_memory_reader_osu;

    HRESULT WINAPI hook_keyboard_GetDeviceState(IDirectInputDevice8W* self, DWORD cbData, LPVOID lpvData) {
        auto result = g_original_keyboard_GetDeviceState(self, cbData, lpvData);
        if (FAILED(result)) {
            return result;
        }

        auto keys = g_memory_reader_osu->get_mania_keys();
        if (!keys) {
            return result;
        }
        for (const auto& key : *keys) {
            int scancode = MapVirtualKeyW(key.first, MAPVK_VK_TO_VSC);
            if (!scancode) {
                continue;
            }
            if (key.second) {
                reinterpret_cast<BYTE*>(lpvData)[scancode] |= 0x80;
            } else {
                reinterpret_cast<BYTE*>(lpvData)[scancode] &= ~0x80;
            }
        }

        return result;
    }
    HRESULT WINAPI hook_keyboard_GetDeviceData(IDirectInputDevice8W* self, DWORD cbObjectData,
                                               LPDIDEVICEOBJECTDATA rgdod, LPDWORD pdwInOut, DWORD dwFlags) {
        if (!rgdod) {
            return g_original_keyboard_GetDeviceData(self, cbObjectData, rgdod, pdwInOut, dwFlags);
        }
        const auto old_buffer_size = *pdwInOut;
        auto result = g_original_keyboard_GetDeviceData(self, cbObjectData, rgdod, pdwInOut, dwFlags);
        if (FAILED(result)) {
            return result;
        }

        auto keys = g_memory_reader_osu->get_mania_keys();
        if (!keys) {
            return result;
        }
        for (const auto& key : *keys) {
            int scancode = MapVirtualKeyW(key.first, MAPVK_VK_TO_VSC);
            if (!scancode) {
                continue;
            }
            auto dod = DIDEVICEOBJECTDATA{};
            dod.dwOfs = scancode;
            dod.dwTimeStamp = 0;                                   // Not used.
            dod.dwSequence = 0;                                    // Not used.
            dod.uAppData = (std::numeric_limits<UINT_PTR>::max)(); // Not used.
            if (key.second) {
                dod.dwData |= 0x80;
            } else {
                dod.dwData &= ~0x80;
            }
            if (*pdwInOut < old_buffer_size) {
                rgdod[(*pdwInOut)++] = dod;
            } else {
                result = DI_BUFFEROVERFLOW;
                break;
            }
        }

        return result;
    }
    HRESULT WINAPI hook_mouse_GetDeviceState(IDirectInputDevice8W* self, DWORD cbData, LPVOID lpvData) {
        return g_original_mouse_GetDeviceState(self, cbData, lpvData);
    }
} // namespace orange
