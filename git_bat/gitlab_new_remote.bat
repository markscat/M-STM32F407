@echo off
title GitLab Remote 管理工具
setlocal enabledelayedexpansion

REM ==============================
REM 進入專案根目錄（bat 上一層）
REM ==============================
set "script_dir=%~dp0"
cd /d "%script_dir%.."

echo 專案根目錄：%cd%
echo.

REM ==============================
REM 檢查 Git 倉庫
REM ==============================
if not exist ".git" (
    echo [提示] 不是 Git 倉庫，初始化中...
    git init
    echo.
) else (
    echo [狀態] 已存在 Git 倉庫
)

:main
REM ==============================
REM 正確計算 remote 數量
REM ==============================
set "remote_count=0"
for /f %%i in ('git remote') do (
    set /a remote_count+=1
)

echo.
echo ===== Remote 清單 =====
git remote -v
echo =======================
echo.

REM ==============================
REM 沒有 remote
REM ==============================
if !remote_count! equ 0 (
    echo [狀態] 尚未設定 Remote
    echo [N] 新增 Remote
    echo [E] 離開 Exit
    set /p "choice=> "

    if /i "!choice!"=="n" goto add_remote
    if /i "!choice!"=="e" exit /b
    goto main
)

REM ==============================
REM 已有 remote
REM ==============================
echo [M] 修改 Remote
echo [A] 新增 Remote
echo [D] 刪除 Remote
echo [E] 離開 Exit
set /p "choice=> "

if /i "!choice!"=="m" goto modify_remote
if /i "!choice!"=="a" goto add_remote
if /i "!choice!"=="d" goto delete_remote
if /i "!choice!"=="e" exit /b
goto main


REM ==============================
REM 新增 Remote
REM ==============================
:add_remote
echo.
set /p "name=Remote 名稱: "
if "!name!"=="" goto main

git remote | findstr /x "!name!" >nul
if !errorlevel! equ 0 (
    echo [錯誤] Remote 已存在
    pause
    goto main
)

set /p "url=Git URL: "
if "!url!"=="" goto main

git remote add "!name!" "!url!"
echo [完成] 已新增 !name!
pause
goto main


REM ==============================
REM 修改 Remote
REM ==============================
:modify_remote
echo.
set /p "name=要修改的 Remote 名稱: "
if "!name!"=="" goto main

git remote get-url "!name!" >nul 2>nul
if errorlevel 1 (
    echo [錯誤] 找不到 Remote
    pause
    goto main
)

echo 目前 URL：
git remote get-url "!name!"

set /p "url=新 URL: "
if "!url!"=="" goto main

git remote set-url "!name!" "!url!"
echo [完成] 已更新
pause
goto main


REM ==============================
REM 刪除 Remote
REM ==============================
:delete_remote
echo.
set /p "name=要刪除的 Remote 名稱: "
if "!name!"=="" goto main

git remote remove "!name!"
echo [完成] 已刪除 !name!
pause
goto main