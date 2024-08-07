#pragma once

namespace CLib
{
    [[nodiscard]] inline std::uint32_t ParseKey(std::uint32_t a_key, RE::INPUT_DEVICE a_device)
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

    namespace Internal
    {
        template <std::same_as<std::uint32_t>... T>
        [[nodiscard]] inline std::uint16_t ResolveModifierKeyCount(std::uint32_t a_key, T... a_keys)
        {
            if constexpr (sizeof...(T) == 0) {
                return a_key == INVALID_KEY ? 0 : 1;
            } else {
                return ResolveModifierKeyCount(a_key) + ResolveModifierKeyCount(a_keys...);
            }
        }
    }

    template <std::same_as<std::uint32_t>... T>
    [[nodiscard]] inline std::uint16_t ResolveKeyCount(std::uint32_t a_key, T... a_keys)
    {
        if constexpr (sizeof...(T) == 0) {
            return a_key == INVALID_KEY ? 0 : 1;
        } else {
            return a_key == INVALID_KEY ? 0 : 1 + Internal::ResolveModifierKeyCount(a_keys...);
        }
    }

    class Key
    {
    public:
        Key() = default;

        explicit Key(std::uint32_t a_targetHotkey) noexcept : _targetHotkey(a_targetHotkey) {}

        void Load(std::uint32_t a_targetHotkey) noexcept { _targetHotkey = a_targetHotkey; }

        [[nodiscard]] bool IsActive() const noexcept { return _hasHotkey; }

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
            _count(ResolveKeyCount(a_targetHotkey, a_targetModifier))
        {}

        [[nodiscard]] std::uint16_t Count() const noexcept { return _count; }

        void Load(std::uint32_t a_targetHotkey, std::uint32_t a_targetModifier) noexcept
        {
            _targetHotkey = a_targetHotkey;
            _targetModifier = a_targetModifier;
            _count = ResolveKeyCount(a_targetHotkey, a_targetModifier);
        }

        [[nodiscard]] bool IsActive() const noexcept
        {
            return _hasHotkey && (_targetModifier == INVALID_KEY || _hasModifier);
        }

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
        std::uint32_t _targetHotkey{ INVALID_KEY };
        std::uint32_t _targetModifier{ INVALID_KEY };

        bool _hasHotkey{ false };
        bool _hasModifier{ false };

        std::uint16_t _count{ 0 };
    };
}
