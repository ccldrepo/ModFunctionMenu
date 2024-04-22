#pragma once

#include <PCH.h>

class InputManager
{
public:
    enum class BlockState : std::uint32_t
    {
        kNotBlocked = 0,
        kBlocked = 1,
        kWantUnblock = 2,
    };

    static bool IsNotBlocked() noexcept { return state.load() == BlockState::kNotBlocked; }

    static void SetBlocked() noexcept { state.store(BlockState::kBlocked); }

    static void SetWantUnblock() noexcept
    {
        auto expected = BlockState::kBlocked;
        state.compare_exchange_strong(expected, BlockState::kWantUnblock);
    }

    static void HandleWantUnblock() noexcept
    {
        auto currentState = state.load();
        if (currentState == BlockState::kWantUnblock) {
            state.compare_exchange_strong(currentState, BlockState::kNotBlocked);
        }
    }

    static void Process(const RE::InputEvent* const* a_event);

private:
    static inline std::atomic<BlockState> state{ BlockState ::kNotBlocked };
};
