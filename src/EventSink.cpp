#include "EventSink.h"

#include "Application.h"
#include "HotkeyManager.h"

void InputEventSink::Register()
{
    if (auto manager = RE::BSInputDeviceManager::GetSingleton()) {
        manager->AddEventSink(GetSingleton());
        SKSE::log::info("Successfully registered input event.");
    } else {
        SKSE::log::error("Failed to register input event.");
    }
}

RE::BSEventNotifyControl InputEventSink::ProcessEvent(const Event* a_event, [[maybe_unused]] EventSource*)
{
    if (!a_event) {
        return RE::BSEventNotifyControl::kContinue;
    }

    if (auto player = RE::PlayerCharacter::GetSingleton(); !player || !player->Is3DLoaded()) {
        return RE::BSEventNotifyControl::kContinue;
    }

    HotkeyManager::Process(a_event);

    return RE::BSEventNotifyControl::kContinue;
}
