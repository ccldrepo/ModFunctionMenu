#include <spdlog/sinks/basic_file_sink.h>

#include <XSEPlugin/Base/Configuration.h>
#include <XSEPlugin/Base/Translation.h>
#include <XSEPlugin/Core.h>
#include <XSEPlugin/Hooks.h>
#include <XSEPlugin/ImGui/Renderer.h>
#include <XSEPlugin/Util/Win.h>

namespace
{
    void InitLogger()
    {
        spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] %v");
        ReconfigureLogger(""sv);

        auto path = SKSE::log::log_directory();
        if (!path) {
            SKSE::stl::report_and_fail("Failed to find SKSE logging directory."sv);
        }
        *path /= SKSE::PluginDeclaration::GetSingleton()->GetName();
        *path += L".log"sv;

        auto logger = spdlog::basic_logger_mt("Global", PathToStr(*path), true);
        spdlog::set_default_logger(std::move(logger));
    }

    void OnMessage(SKSE::MessagingInterface::Message* a_message)
    {
        switch (a_message->type) {
        case SKSE::MessagingInterface::kPostLoad:
            {
                std::thread t{ []() { (void)Datastore::GetSingleton(); } };
                t.detach();
            }
            break;
        case SKSE::MessagingInterface::kPostPostLoad:
            ProcessInputQueueHook::Install();
            break;
        default:
            break;
        }
    }
}

SKSEPluginLoad(const SKSE::LoadInterface* a_skse)
{
    InitLogger();

    if (auto osVersion = Win::OsVersion::Get()) {
        SKSE::log::info("OS Version: {}", osVersion->string("."sv));
    } else {
        SKSE::log::info("OS Version: Unknown");
    }
    SKSE::log::info("Game Version: {}", a_skse->RuntimeVersion().string("."sv));
#ifdef _DEBUG
    SKSE::log::debug("Base Address: 0x{:016X}", REL::Module::get().base());
#endif

    auto plugin = SKSE::PluginDeclaration::GetSingleton();
    SKSE::log::info("{} {} is loading...", plugin->GetName(), plugin->GetVersion().string("."sv));

    SKSE::Init(a_skse);
    {
        std::scoped_lock lock{ Configuration::Mutex(), Translation::Mutex() };

        Configuration::Init();
        Translation::Init();

        ReconfigureLogger(Configuration::GetSingleton()->general.sLogLevel);

        Configuration::IncrementVersion();
        Translation::IncrementVersion();
    }
    ImGui::Renderer::Install();

    SKSE::GetMessagingInterface()->RegisterListener(OnMessage);

    SKSE::log::info("{} has finished loading.", plugin->GetName());
    return true;
}
