#include "Console.hpp"

#include <memory>
#include <print>
#include <iostream>
#include <fstream>

#ifdef WIN32
#include <Windows.h>
#else
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#endif

std::shared_ptr<Console> Console::m_Instance = std::make_shared<Console>();

Console::Console()
{
	if (m_Instance)
	{
		std::println("Console::Console() - Instance already exists");
		return;
	}
	m_Thread = std::thread(&Console::ThreadLoop, this);
	m_Running = true;
}

Console::~Console()
{
	m_Running = false;
	m_QueueCondition.notify_all();
	m_Thread.join();
}

void Console::ThreadLoop()
{
#ifdef WIN32
	auto allocated = AllocConsole();

	if (allocated)
	{
		(void) freopen_s(&m_Console, "CONOUT$", "w", stdout);
		SetConsoleTitle("Debug Console");
	}
	else
	{
		auto error = GetLastError();
		if (error != 5)
		{
			std::println("Console::ThreadLoop() - Failed to Allocate Console");
			LPVOID lpMsgBuf;
			auto flags = FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS;
			auto langId = MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT);
			FormatMessage(flags, nullptr, error, langId, reinterpret_cast<LPSTR>(&lpMsgBuf), 0, nullptr);
			std::println("{}", static_cast<LPCTSTR>(lpMsgBuf));
			return;
		}
	}
#else
/*	const char* logPipe = "/tmp/debug_console_pipe";
	unlink(logPipe);

	if (mkfifo(logPipe, 0666) == -1)
	{
		std::println("Failed to create named pipe\n");
		return;
	}

	system("konsole -e bash"); // 'echo' \"Debug Console\";");

	std::ofstream logFile(logPipe);
	if (!logFile)
	{
		std::cerr << "Failed to open log pipe\n";
		return;
	}

	std::cout.rdbuf(logFile.rdbuf());
	std::cerr.rdbuf(logFile.rdbuf());*/
#endif

	while (m_Running)
	{
		ConsoleMessage message;
		{
			std::unique_lock<std::mutex> lock(m_QueueMutex);
			m_QueueCondition.wait(lock, [this] { return !m_MessageQueue.empty() || !m_Running; });

			if (!m_Running)
				break;

			message = m_MessageQueue.front();
			m_MessageQueue.pop();
		}

		PrintMessage(message);
	}

#ifdef WIN32
	if (allocated)
	{
		FreeConsole();
		(void) fclose(m_Console);
	}
#else

#endif
}

void Console::QueueMessage(const std::string& message, ConsoleMessage::Type type, const std::string& time)
{
	std::lock_guard<std::mutex> lock(m_QueueMutex);
	m_MessageQueue.push({ type, message, time });
	m_QueueCondition.notify_one();
}

void Console::SetColor(ConsoleMessage::Type type) const
{
	switch (type)
	{
	case ConsoleMessage::Type::Debug:
		SetColor(ConsoleColor::Debug);
		break;
	case ConsoleMessage::Type::Info:
		SetColor(ConsoleColor::Info);
		break;
	case ConsoleMessage::Type::Warning:
		SetColor(ConsoleColor::Warning);
		break;
	case ConsoleMessage::Type::Error:
		SetColor(ConsoleColor::Error);
		break;
	}
}


void Console::SetColor(ConsoleColor color) const
{
#ifdef WIN32
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	switch (color)
	{
	case ConsoleColor::Default:
		SetConsoleTextAttribute(hConsole, 7);
		break;
	case ConsoleColor::Debug:
		SetConsoleTextAttribute(hConsole, 10);
		break;
	case ConsoleColor::Info:
		SetConsoleTextAttribute(hConsole, 11);
		break;
	case ConsoleColor::Warning:
		SetConsoleTextAttribute(hConsole, 6);
		break;
	case ConsoleColor::Error:
		SetConsoleTextAttribute(hConsole, 4);
		break;
	}
#else
	switch (color)
	{
	case ConsoleColor::Default:
		std::print("\033[0m");
		break;
	case ConsoleColor::Debug:
		std::print("\033[32m");
		break;
	case ConsoleColor::Info:
		std::print("\033[36m");
		break;
	case ConsoleColor::Warning:
		std::print("\033[33m");
		break;
	case ConsoleColor::Error:
		std::print("\033[31m");
		break;
	}
#endif
}

void Console::PrintMessage(const ConsoleMessage& message) const
{
	SetColor(ConsoleColor::Default);
	std::print("(");
	std::print("{}", message.time);
	std::print(") ");
	std::print("[");


	SetColor(message.type);
	switch (message.type)
	{
	case ConsoleMessage::Type::Debug:
		std::print("Debug");
		break;
	case ConsoleMessage::Type::Info:
		std::print("Info");
		break;
	case ConsoleMessage::Type::Warning:
		std::print("Warning");
		break;
	case ConsoleMessage::Type::Error:
		std::print("Error");
		break;
	}

	SetColor(ConsoleColor::Default);
	std::print("] ");
	std::print("{}", message.message);
	std::print("\n");
}

std::weak_ptr<Console> Console::Get()
{
	return m_Instance;
}
