#pragma once
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

namespace NTC
{
    class Logger
    {
    public:
        static void Init();

        static Ref<spdlog::logger>& GetLogger() { return Logger_; }

    private:
        
        static Ref<spdlog::logger> Logger_;
    };
}

// DO NOT USE INSIDE CONSTRUCTORS IT CAUSES CRASH!!!!!!!!

#define NTC_TRACE(...)      ::NTC::Logger::GetLogger()->trace(__VA_ARGS__)
#define NTC_INFO(...)       ::NTC::Logger::GetLogger()->info(__VA_ARGS__)
#define NTC_WARN(...)       ::NTC::Logger::GetLogger()->warn(__VA_ARGS__)
#define NTC_ERROR(...)      ::NTC::Logger::GetLogger()->error(__VA_ARGS__)
#define NTC_CRITICAL(...)   ::NTC::Logger::GetLogger()->critical(__VA_ARGS__)