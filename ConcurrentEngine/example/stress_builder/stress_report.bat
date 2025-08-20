@echo off
setlocal enabledelayedexpansion

REM ===============================
REM Build and run stress_test
REM ===============================

REM === 產生 timestamp ===
for /f "tokens=2 delims==" %%I in ('"wmic os get localdatetime /value"') do set ldt=%%I
set timestamp=!ldt:~0,8!_!ldt:~8,4!

echo [Build] Compiling stress_test.cpp ...

REM Set include path to ConcurrentEngine headers
set INCLUDE_PATH=..\..\include

REM Add ConcurrentEngine implementation files required
set ENGINE_SRC=..\..\src\threadPool.cpp ..\..\src\logger\threadlogger.cpp ^
    ..\..\src\core\thread.cpp ..\..\src\core\thread_meta.cpp ^
    ..\..\src\scheduler\DAGschedule.cpp ..\..\src\scheduler\FIFO_schedule.cpp ..\..\src\scheduler\priorityScheduler.cpp

REM Compile with C++23 standard
g++ -std=c++23 -I%INCLUDE_PATH% stress_test.cpp %ENGINE_SRC% -o stress_test.exe -pthread

IF %ERRORLEVEL% NEQ 0 (
    echo [Error] Build failed!
    pause
    exit /b %ERRORLEVEL%
)

echo [Run] Executing stress test ...
stress_test.exe

IF %ERRORLEVEL% NEQ 0 (
    echo [Error] Stress test execution failed!
    pause
    exit /b %ERRORLEVEL%
)

REM 重新命名 CSV
if exist stress_report.csv (
    set REPORT_NAME=stress_report_%timestamp%.csv
    rename stress_report.csv %REPORT_NAME%
    python stress_analyze.py %REPORT_NAME%
) ELSE (
    echo [Warning] stress_report.csv not found
)

REM ==== Python analysis with module check ====
python -c "import pandas, matplotlib" 2>nul
IF %ERRORLEVEL% NEQ 0 (
    echo [Warning] pandas or matplotlib not installed, skipping Python analysis
) ELSE (
    echo [Python] Generating charts ...
    python stress_analyze.py
    IF %ERRORLEVEL% NEQ 0 (
        echo [Error] Python analysis failed!
    )
)

echo [Done] All results generated successfully. (timestamp=%timestamp%)
pause
