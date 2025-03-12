#include <iostream>

#include "Utils/Logging.hpp"
#include "Utils/Time.hpp"

int main()
{
    Logging::Init();
    Logging::Debug("This is just a test. {} {:B}", "0xFF40:", 0x0F40);
    
    system("pause");
    return 0;
    Logging::Init();
    Logging::Debug("This is just a test. {} {:B}", "0xFF40:", 0x0F40);
}
