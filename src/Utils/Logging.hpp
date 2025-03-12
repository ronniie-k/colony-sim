#pragma once

#include <format>
#include <print>

#include "Console.hpp"

class Logging
{
public:
    static void Init();
        
    template<class ... Args>
    static void Debug(std::format_string<Args...> format, Args&& ... args);

    template<class ... Args>
    static void Info(std::format_string<Args...> format, Args&& ... args);

    template<class ... Args>
    static void Warning(std::format_string<Args...> format, Args&& ... args);

    template<class ... Args>
    static void Error(std::format_string<Args...> format, Args&& ... args);
    
private:
    static std::string GetCurrentTime();
    static std::weak_ptr<Console> m_Console;
};

template<class ... Args>
void Logging::Debug(std::format_string<Args...> format, Args&& ... args)
{
    if (auto console = m_Console.lock())
    {
        console->QueueMessage(std::format(format, std::forward<Args>(args)...), ConsoleMessage::Type::Debug, GetCurrentTime());
    }
}

template<class ... Args>
void Logging::Info(std::format_string<Args...> format, Args&& ... args)
{
    if (auto console = m_Console.lock())
    {
        console->QueueMessage(std::format(format, std::forward<Args>(args)...), ConsoleMessage::Type::Info, GetCurrentTime());
    }
}

template<class ... Args>
void Logging::Warning(std::format_string<Args...> format, Args&& ... args)
{
    if (auto console = m_Console.lock())
    {
        console->QueueMessage(std::format(format, std::forward<Args>(args)...), ConsoleMessage::Type::Warning, GetCurrentTime());
    }
}

template <class... Args>
void Logging::Error(std::format_string<Args...> format, Args&&... args)
{
    if (auto console = m_Console.lock())
    {
        console->QueueMessage(std::format(format, std::forward<Args>(args)...), ConsoleMessage::Type::Error, GetCurrentTime());
    }
}
