#pragma once
#include <format>
#include <mutex>
#include <queue>
#include <string>
#include <memory>
#include <thread>
#include <condition_variable>

struct ConsoleMessage
{
	enum class Type : uint8_t
	{
		Debug,
		Info,
		Warning,
		Error
	};

	Type type;
	std::string message;
	std::string time;
};

enum class ConsoleColor : uint8_t
{
	Default,
	Debug,
	Info,
	Warning,
	Error
};

class Console
{
public:
	Console(const Console&) = delete;
	Console& operator=(const Console&) = delete;
	Console(Console&&) = delete;
	Console& operator=(Console&&) = delete;

	Console();
	~Console();

	void QueueMessage(const std::string& message, ConsoleMessage::Type type, const std::string& time);

	void SetColor(ConsoleMessage::Type type) const;
	void SetColor(ConsoleColor color) const;
	void PrintMessage(const ConsoleMessage& message) const;

	static std::weak_ptr<Console> Get();

private:
	void ThreadLoop();

	FILE* m_Console;

	std::mutex m_QueueMutex;
	std::queue<ConsoleMessage> m_MessageQueue;
	std::condition_variable m_QueueCondition;
	bool m_Running;
	std::thread m_Thread;

	static std::shared_ptr<Console> m_Instance;
};
