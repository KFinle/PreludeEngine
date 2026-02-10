@echo off
setlocal

set "CONFIG=Debug"
if /I "%~1"=="release" (
  set "CONFIG=Release"
  shift
)

where cmake >nul 2>nul
if errorlevel 1 (
  echo cmake not found in PATH
  exit /b 1
)

cmake --preset windows-vs2022 %*
if errorlevel 1 exit /b 1

if /I "%CONFIG%"=="Release" (
  cmake --build --preset windows-release
) else (
  cmake --build --preset windows-debug
)

endlocal
