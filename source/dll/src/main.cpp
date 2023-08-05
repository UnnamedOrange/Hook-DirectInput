/**
 * @file main.cpp
 * @author UnnamedOrange
 * @brief DLL entry.
 * @version 0.0.0
 * @date 2023-08-04
 *
 * @copyright Copyright (c) UnnamedOrange. Licensed under the MIT License.
 * See the LICENSE file in the repository root for full license text.
 */

#include <memory>
#include <stdexcept>

#include <Windows.h>
#include <dinput.h>
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

#include <detours/detours.h>

#include "MemoryReaderOsu.h"
#include "hook_api.h"
#include "utils/SharedComPtr.hpp"
#include "utils/Vtable.hpp"

namespace orange {
    std::unique_ptr<MemoryReaderOsu> g_memory_reader_osu;

    class DllMain {
    public:
        DllMain(HINSTANCE instance_dll) {
            // Create memory reader.
            g_memory_reader_osu = std::make_unique<MemoryReaderOsu>();

            // Create DirectInput related objects to access vtables.
            SharedComPtr<IDirectInput8W> dinput;
            SharedComPtr<IDirectInputDevice8W> keyboard_device, mouse_device;
            if (FAILED(DirectInput8Create(instance_dll, DIRECTINPUT_VERSION, IID_IDirectInput8W,
                                          reinterpret_cast<void**>(dinput.reset_and_get_address()), nullptr))) {
                throw std::runtime_error("Failed to create DirectInput8.");
            }
            if (FAILED(dinput->CreateDevice(GUID_SysKeyboard, keyboard_device.reset_and_get_address(), nullptr))) {
                throw std::runtime_error("Failed to create keyboard device.");
            }
            if (FAILED(dinput->CreateDevice(GUID_SysMouse, mouse_device.reset_and_get_address(), nullptr))) {
                throw std::runtime_error("Failed to create mouse device.");
            }

            g_original_keyboard_GetDeviceState =
                reinterpret_cast<SGetDeviceState*>(Vtable::get_virtual_method_address(*keyboard_device, 9));
            g_original_mouse_GetDeviceState =
                reinterpret_cast<SGetDeviceState*>(Vtable::get_virtual_method_address(*mouse_device, 9));

            // Hook.
            DetourTransactionBegin();
            DetourUpdateThread(GetCurrentThread());
            DetourAttach(reinterpret_cast<void**>(&g_original_keyboard_GetDeviceState),
                         reinterpret_cast<void*>(hook_keyboard_GetDeviceState));
            DetourAttach(reinterpret_cast<void**>(&g_original_mouse_GetDeviceState),
                         reinterpret_cast<void*>(hook_mouse_GetDeviceState));
            DetourTransactionCommit();
        }
        ~DllMain() {
            // Unhook.
            DetourTransactionBegin();
            DetourUpdateThread(GetCurrentThread());
            DetourDetach(reinterpret_cast<void**>(&g_original_keyboard_GetDeviceState),
                         reinterpret_cast<void*>(hook_keyboard_GetDeviceState));
            DetourDetach(reinterpret_cast<void**>(&g_original_mouse_GetDeviceState),
                         reinterpret_cast<void*>(hook_mouse_GetDeviceState));
            DetourTransactionCommit();

            // Destroy memory reader.
            g_memory_reader_osu.reset();
        }
    };

    std::unique_ptr<DllMain> g_dll_main;
} // namespace orange

using namespace orange;

BOOL APIENTRY DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
    switch (fdwReason) {
    case DLL_PROCESS_ATTACH: {
        g_dll_main = std::make_unique<orange::DllMain>(hinstDLL);
        break;
    }
    case DLL_THREAD_ATTACH: break;
    case DLL_THREAD_DETACH: break;
    case DLL_PROCESS_DETACH: {
        if (lpvReserved != nullptr) {
            break; // Do not do cleanup.
        }
        g_dll_main.reset();
        break;
    }
    }
    return TRUE;
}
