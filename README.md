# system_logger

light weight thread safe / realtime safe logger that uses syslog utility

# example
- use Logger if you don't mind allocating memory
- use RtLogger if you care about allocating memory

```cpp

#include "system_logger/Logger.h"

using namespace system_logger;
LoggingSession::instance().initialize("My Application");
LoggingSession::instance().setLogLevel(LogLevel::Debug);
RtLogger logger("main");
logger.setLogLevel(LogLevel::Warning);
SYSLOG_INFO(logger, "Hello world!", 42, M_PI);

```
