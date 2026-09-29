@echo off
title Git Push Tool (Clean Flow)
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
REM commit
REM ==============================
git status --short
echo.

set /p MSG="Commit 訊息: "
if "%MSG%"=="" (
    echo Commit 不可為空
    pause
    exit /b
)

git add .

REM 先檢查是否真的有東西可以 commit
git diff --cached --quiet
if %errorlevel%==0 (
    echo 沒有新變更，跳過 commit
    goto skip_commit
)

git commit -m "%MSG%"
if errorlevel 1 (
    echo Commit 失敗
    pause
    exit /b
)

:skip_commit

REM ======================================================
REM PUSH 模式選擇（所有邏輯集中在這裡）
REM ======================================================
echo.
echo ==========================================
echo PUSH 模式選擇
echo ==========================================
echo [1] 推單一 remote
echo [2] 推全部 remote
echo ==========================================
set /p PUSH_MODE="請選擇 [1/2]: "
if not defined PUSH_MODE set PUSH_MODE=1

echo.

REM ==============================
REM mode 1：單一 remote（這裡才選）
REM ==============================
if "%PUSH_MODE%"=="1" (

    set /a idx=0
    set "REMOTE_NAME="

    echo ===== Remote 清單 =====
    for /f %%r in ('git remote') do (
        set /a idx+=1
       rem set "remote!idx!=%%r"
        echo [!idx!] %%r
    )

    echo =======================

    set /p sel="選擇 remote 編號: "
    set /a idx=0

    rem set "REMOTE_NAME=!remote%sel%!"

   rem if not defined REMOTE_NAME (
    rem    echo 錯誤：未選擇 remote
    rem    pause
    rem   exit /b
    rem )
    
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

    git push -u !REMOTE_NAME! %BRANCH_NAME%

    if errorlevel 1 (
        echo Push 失敗
        pause
        exit /b
    )
    goto done
)

REM ==============================
REM mode 2：全部 remote
REM ==============================
echo 推送全部 remote...

for /f %%r in ('git remote') do (
    echo ------------------------
    echo push -- %%r
    git push -u %%r %BRANCH_NAME%
)

:done
echo.
echo ==========================
echo 完成
echo ==========================
pause
exit /b