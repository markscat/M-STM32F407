@echo off
title 設定 GitLab Remote
setlocal enabledelayedexpansion

REM 取得批次檔所在目錄的上一層（專案根目錄）
set "script_dir=%~dp0"
cd /d "%script_dir%.."

echo 專案根目錄：%cd%
echo.

REM 從根目錄名稱取得專案名稱
for %%I in ("%cd%") do set "project_name=%%~nxI"
echo 專案名稱：%project_name%
echo.

set /p confirm="確認專案名稱正確？(y/n): "
if /i not "!confirm!"=="y" (
    echo 請確認專案名稱後再執行
    pause
    exit /b
)

REM 檢查是否為 Git 倉庫
if not exist ".git" (
    echo 不是 Git 倉庫，正在初始化...
    git init
    echo.
)

REM 檢查是否已有 gitlab remote
git remote | findstr "^gitlab$" > nul
if not errorlevel 1 (
    echo gitlab remote 已存在：
    git remote -v | findstr "gitlab"
    echo.
    set /p confirm="要覆蓋嗎？ (y/n): "
    if /i not "!confirm!"=="y" (
        echo 取消設定
        pause
        exit /b
    )
    git remote remove gitlab
    echo 已移除舊的 gitlab remote
    echo.
)

REM 設定 GitLab Remote
set /p "custom_url=請輸入 GitLab Remote 的完整網址 (URL): "
if "%custom_url%"=="" (
    echo 網址不能為空，設定失敗！
    pause
    exit /b
)
echo 設定 remote: %custom_url%
git remote add gitlab %custom_url%

if errorlevel 1 (
    echo 設定失敗！
    pause
    exit /b
)

echo.
echo 設定完成！目前的 remote：
git remote -v

REM ===== 修正後的同步邏輯 =====
echo.
echo 正在從 gitlab 拉取並同步...

REM 1. 拉取所有遠端資料
git fetch gitlab
if errorlevel 1 (
    echo 拉取失敗！請檢查網路或遠端倉庫是否存在。
    pause
    exit /b
)

REM 2. 取得當前分支名稱（剛初始化的倉庫可能沒有分支，需處理）
set "current_branch=main"
git rev-parse --abbrev-ref HEAD > nul 2>&1
if not errorlevel 1 (
    for /f %%B in ('git rev-parse --abbrev-ref HEAD') do set "current_branch=%%B"
) else (
    echo 當前不在任何分支，將建立並切換到 main 分支
    git checkout -b main
)

REM 3. 檢查遠端是否有這個分支
git ls-remote --heads gitlab "%current_branch%" | findstr "%current_branch%" > nul
if errorlevel 1 (
    echo 遠端尚未有 %current_branch% 分支，這是一個全新的倉庫。
    echo 請先在本機新增檔案並執行 gitlab_push.bat 上傳。
) else (
    echo 遠端已有 %current_branch% 分支，正在強制同步本地...
    REM 強制將本地分支重置為遠端分支的狀態（解決衝突的最終手段）
    git reset --hard "gitlab/%current_branch%"
    if errorlevel 1 (
        echo 同步失敗！
    ) else (
        echo 同步成功！本地程式碼已與 gitlab/%current_branch% 完全一致。
    )
)

echo.
echo 接下來請執行 gitlab_push.bat 上傳程式碼（如果是新倉庫）

cd /d "%script_dir%"
pause