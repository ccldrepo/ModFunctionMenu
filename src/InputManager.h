#pragma once

class InputManager
{
public:
    enum class BlockState : std::uint32_t
    {
        kNotBlocked = 0,
        kBlocked = 1,
        kWantUnblock = 2,
    };

    [[nodiscard]] static bool IsNotBlocked() noexcept { return _state.load() == BlockState::kNotBlocked; }

    static void SetBlocked() noexcept { _state.store(BlockState::kBlocked); }

    static void SetWantUnblock() noexcept
    {
        auto expected = BlockState::kBlocked;
        _state.compare_exchange_strong(expected, BlockState::kWantUnblock);
    }

    static void HandleWantUnblock() noexcept
    {
        auto currentState = _state.load();
        if (currentState == BlockState::kWantUnblock) {
            _state.compare_exchange_strong(currentState, BlockState::kNotBlocked);
        }
    }

    static void Process(RE::InputEvent* const* a_event);

private:
    static inline std::atomic<BlockState> _state{ BlockState ::kNotBlocked };
};
