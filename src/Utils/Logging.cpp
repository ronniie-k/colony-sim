#include "Logging.hpp"

std::weak_ptr<Console> Logging::m_Console;

std::string Logging::GetCurrentTime()
{
    const time_t now = std::time(nullptr);
    std::tm localTime;
    (void)localtime_s(&localTime, &now);

    const auto format = "%H:%M:%S";

    char buffer[80];
    (void)std::strftime(buffer, sizeof(buffer), format, &localTime);

    return buffer;
}

void Logging::Init()
{
    m_Console = Console::Get();
    if (m_Console.expired()) Error("Failed to get Console instance!");
    Info("Starting Logging...");
}