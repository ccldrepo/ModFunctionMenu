#pragma once

namespace CLib
{
    inline std::uint32_t ParseKey(std::uint32_t a_key, RE::INPUT_DEVICE a_device)
    {
        switch (a_device) {
        case RE::INPUT_DEVICE::kKeyboard:
            return a_key;
        case RE::INPUT_DEVICE::kMouse:
            return a_key + SKSE::InputMap::kMacro_MouseButtonOffset;
        case RE::INPUT_DEVICE::kGamepad:
            return SKSE::InputMap::GamepadMaskToKeycode(a_key);
        default:
            return a_key;
        }
    }

    constexpr inline std::uint32_t INVALID_KEY = 0;

    class Key
    {
    public:
        Key() = default;

        explicit Key(std::uint32_t a_targetHotkey) noexcept : _targetHotkey(a_targetHotkey) {}

        bool IsActive() const noexcept { return _hasHotkey; }

        void Reset() noexcept { _hasHotkey = false; }

        void Update(std::uint32_t a_key) noexcept
        {
            if (_targetHotkey != INVALID_KEY && a_key == _targetHotkey) {
                _hasHotkey = true;
            }
        }

    private:
        std::uint32_t _targetHotkey{ INVALID_KEY };

        bool _hasHotkey{ false };
    };

    class KeyCombo
    {
    public:
        KeyCombo() = default;

        KeyCombo(std::uint32_t a_targetHotkey, std::uint32_t a_targetModifier) noexcept :
            _targetHotkey(a_targetHotkey), _targetModifier(a_targetModifier),
            _count(CalcCount(a_targetHotkey, a_targetModifier))
        {}

        std::uint32_t Count() const noexcept { return _count; }

        bool IsActive() const noexcept { return _hasHotkey && (_targetModifier == INVALID_KEY || _hasModifier); }

        void Reset() noexcept
        {
            _hasHotkey = false;
            _hasModifier = false;
        }

        void UpdateDown(std::uint32_t a_key) noexcept
        {
            if (_targetHotkey != INVALID_KEY && a_key == _targetHotkey) {
                _hasHotkey = true;
            }
        }

        void UpdatePressed(std::uint32_t a_key) noexcept
        {
            if (_targetModifier != INVALID_KEY && a_key == _targetModifier) {
                _hasModifier = true;
            }
        }

    private:
        static std::uint16_t CalcCount(std::uint32_t a_targetHotkey, std::uint32_t a_targetModifier) noexcept
        {
            if (a_targetHotkey == INVALID_KEY) {
                return 0;
            } else if (a_targetModifier == INVALID_KEY) {
                return 1;
            } else {
                return 2;
            }
        }

        std::uint32_t _targetHotkey{ INVALID_KEY };
        std::uint32_t _targetModifier{ INVALID_KEY };

        bool _hasHotkey{ false };
        bool _hasModifier{ false };

        std::uint16_t _count{ 0 };
    };
}
