#include "Function.h"

#include <spdlog/sinks/ostream_sink.h>

#include <XSEPlugin/Base/Configuration.h>
#include <XSEPlugin/Base/Translation.h>

MFMAPI void ReloadConfig(char* a_msg, std::size_t a_len)
{
    std::ostringstream oss;
    {
        auto sink = std::make_shared<spdlog::sinks::ostream_sink_mt>(oss);
        sink->set_pattern("[%l] %v");
        spdlog::default_logger_raw()->sinks().push_back(std::move(sink));
    }

    try {
        std::scoped_lock lock{ Configuration::GetMutex(), Translation::GetMutex() };

        Configuration::Init(false);
        Translation::Init(false);
        Configuration::IncrementVersion();
        Translation::IncrementVersion();
    } catch (const std::exception&) {
        // Suppress exception.
    }

    auto msg = oss.str();
    std::memcpy(a_msg, msg.c_str(), std::min(msg.size() + 1, a_len));
    spdlog::default_logger_raw()->sinks().pop_back();
}
