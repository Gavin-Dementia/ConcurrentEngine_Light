@echo off
REM ===============================
REM Build mini-Redis example only
REM ===============================

REM Set include path to ConcurrentEngine headers
set INCLUDE_PATH=..\..\include

REM Set source files
set SOURCES=CommandExecutor.cpp CommandParser.cpp RedisDatabase.cpp RedisServer.cpp test.cpp

REM Add ConcurrentEngine implementation files required
REM Add ConcurrentEngine implementation files required
set ENGINE_SRC=..\..\src\threadPool.cpp ..\..\src\logger\threadlogger.cpp ^
    ..\..\src\core\thread.cpp ..\..\src\core\thread_meta.cpp ^
    ..\..\src\scheduler\DAGschedule.cpp ..\..\src\scheduler\FIFO_schedule.cpp ..\..\src\scheduler\priorityScheduler.cpp


REM Compile with C++20 standard
g++ -std=c++20 -I%INCLUDE_PATH% %SOURCES% %ENGINE_SRC% -o test.exe

IF %ERRORLEVEL% NEQ 0 (
    echo Compilation failed!
    pause
    exit /b %ERRORLEVEL%
)

REM Run the executable
test.exe

pause
