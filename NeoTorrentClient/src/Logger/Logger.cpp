#include "NTCpch.h"
#include "Logger.h"

#include <spdlog/sinks/basic_file_sink.h>

namespace NTC
{
    Ref<spdlog::logger> Logger::Logger_;

    void Logger::Init()
    {
        NTC_PROFILE_FUNCTION();
        Ref<spdlog::sinks::basic_file_sink_mt> FileSink = CreateRef<spdlog::sinks::basic_file_sink_mt>("..\\NG.log", true);
        FileSink->set_pattern("%^[%T] [%l]: %v%$");

        Logger_ = CreateRef<spdlog::logger>("Logger", FileSink);
        
        spdlog::register_logger(Logger_);
        
        Logger_->set_level(spdlog::level::trace);
        Logger_->flush_on(spdlog::level::trace);

        NTC_TRACE("Logger initialized!");
    }
}