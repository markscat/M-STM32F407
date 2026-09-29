@echo off
title GitLab Push
setlocal enabledelayedexpansion

cd..

REM 檢查是否在專案根目錄
echo 檢查是否在專案根目錄
if not exist ".git" (
    echo 錯誤：找不到 .git 目錄
    echo 請先執行 gitlab_set_remote.bat 設定 remote
    pause
    exit /b
)
echo 找到 .git 目錄

REM 偵測 remote 名稱
set REMOTE_NAME=
for /f %%i in ('git remote ^| findstr /i "gitlab origin"') do (
    if not defined REMOTE_NAME set "REMOTE_NAME=%%i"
)

if "%REMOTE_NAME%"=="" (
    echo 找不到 gitlab 或 origin remote
    set /p REMOTE_NAME="請輸入 remote 名稱: "
)

echo 使用 remote: %REMOTE_NAME%
git remote get-url %REMOTE_NAME%
echo.
set /p confirm="網址正確請按 y，要修改請按 n: "

if /i "%confirm%"=="y" (
    echo 網址並未修改，繼續執行...
    goto :fetch
)

echo 請輸入修改的 remote 網址                                                                                              
set /p NEW_URL="新網址: "
echo 暫存使用 %NEW_URL%（尚未存入remote）

:fetch

REM 直接用網址或 remote 名稱 fetch

if "%NEW_URL%"=="" (
    echo NEW_URL 為空，使用預設 Remote: %REMOTE_NAME%
    git fetch %REMOTE_NAME% main
    set "TARGET= %REMOTE_NAME%/main"
) else (
    echo 使用網址 fetch: %NEW_URL%
    git fetch %NEW_URL% main
    set "TARGET=FETCH_HEAD"
)

if errorlevel 1 (
    echo Fetch 失敗，請檢查網址或網路狀況
    pause
    exit /b
)

echo 確定reset?(ctrl+c可跳離)
pause
echo 再次確認是否要reset?(ctrl+c可跳離)
pause

git reset --hard %TARGET%

cd git_bat

echo.
echo ==========================================
echo           檔案已更新
echo ==========================================
echo.
     
REM 最後才問要不要儲存（只有當使用者輸入新網址時才問）
if defined NEW_URL (
    echo.
    echo 是否要將 %REMOTE_NAME% 的網址更新為 %NEW_URL%？
    set /p SAVE="確認請按 Y: "
    if /i "!SAVE!"=="y" (
        git remote set-url %REMOTE_NAME% %NEW_URL%
        echo %REMOTE_NAME% 已更新網址為 %NEW_URL%
    )
)


pause
