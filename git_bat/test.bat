@echo off
title Git Pull Tool (Select Remote)
setlocal enabledelayedexpansion

cd..

echo 專案根目錄：%cd%
echo.

REM ==============================
REM 檢查 Git
REM ==============================
if not exist ".git" (
    echo 錯誤：找不到 .git 目錄
    pause
    exit /b
)

echo 已找到 Git 倉庫
echo.

REM ==============================
REM 顯示 remote
REM ==============================
echo ===== Remote 清單 =====
git remote -v
echo =======================
echo.

REM ==============================
REM Branch 選擇
REM ==============================
set /p BRANCH_NAME="請輸入 branch（Enter=目前分支）: "

if "%BRANCH_NAME%"=="" (
    for /f %%i in ('git branch --show-current') do set BRANCH_NAME=%%i
)

echo 使用分支: %BRANCH_NAME%
echo.

REM ==============================
REM 選擇 remote
REM ==============================
set /a idx=0
set "REMOTE_NAME="

echo ===== Remote 清單 =====
for /f %%r in ('git remote') do (
    set /a idx+=1
    echo [!idx!] %%r
)
echo =======================

set /p sel="選擇 remote 編號: "
set /a idx=0

for /f %%r in ('git remote') do (
    set /a idx+=1
    if !idx! equ !sel! set "REMOTE_NAME=%%r"
)

if not defined REMOTE_NAME (
    echo 錯誤：未選擇 remote
    pause
    exit /b
)

echo.
echo 使用 remote: !REMOTE_NAME!
git remote get-url !REMOTE_NAME!
echo.

REM ==============================
REM 執行 pull
REM ==============================
git pull !REMOTE_NAME! %BRANCH_NAME%

if errorlevel 1 (
    echo Pull 失敗
    pause
    exit /b
)

echo.
echo ==========================
echo 完成
echo ==========================
pause
exit /b