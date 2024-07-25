#include "Function.h"

#include <spdlog/sinks/ostream_sink.h>

#include <XSEPlugin/Base/Configuration.h>
#include <XSEPlugin/Base/Translation.h>

MFMAPI void ReloadConfig(char* a_msg, std::size_t a_len)
{
    std::ostringstream oss;
    spdlog::create<spdlog::sinks::ostream_sink_mt>("Base", oss);

    try {
        std::scoped_lock lock{ Configuration::Mutex(), Translation::Mutex() };

        Configuration::Init(false);
        Translation::Init(false);

        ReconfigureLogger(Configuration::GetSingleton()->general.sLogLevel);

        Configuration::IncrementVersion();
        Translation::IncrementVersion();
    } catch (...) {
        // Suppress exception.
    }

    if (a_msg) {
        auto msg = oss.str();
        std::memcpy(a_msg, msg.c_str(), std::min(msg.size() + 1, a_len));
    }

    spdlog::drop("Base");
}
