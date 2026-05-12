@echo off
echo ========== Keil5项目编译脚本 ==========
echo 实测模式编译验证

REM 检查Keil项目文件
if not exist "App_Framework.uvprojx" (
    echo 错误：找不到Keil项目文件 App_Framework.uvprojx
    pause
    exit /b 1
)

echo 找到Keil项目文件：App_Framework.uvprojx

REM 检查Keil5安装路径
set KEIL_PATH=C:\Keil_v5\UV4
if not exist "%KEIL_PATH%\UV4.exe" (
    echo 警告：未找到Keil5在标准路径 %KEIL_PATH%
    echo 请手动打开Keil5并编译项目
    echo 项目文件：%cd%\App_Framework.uvprojx
    pause
    exit /b 1
)

echo 找到Keil5：%KEIL_PATH%\UV4.exe

REM 尝试编译项目
echo 开始编译项目...
"%KEIL_PATH%\UV4.exe" -b "App_Framework.uvprojx" -o build_output_realtest.log

REM 检查编译结果
if exist "build_output_realtest.log" (
    echo.
    echo ========== 编译输出 ==========
    type "build_output_realtest.log"
    echo.
    echo ========== 编译完成 ==========
    
    REM 检查是否编译成功
    findstr /C:"0 Error(s)" "build_output_realtest.log" > nul
    if %errorlevel% equ 0 (
        echo ✓ 编译成功！
        findstr /C:"Warning(s)" "build_output_realtest.log"
        echo.
        echo 实测模式准备完成！
        echo 可以下载程序到开发板进行实测。
    ) else (
        echo ✗ 编译失败，请检查错误信息
    )
) else (
    echo 编译日志文件未生成，可能编译器路径不正确
)

echo.
echo 手动操作指南：
echo 1. 打开Keil5
echo 2. 打开项目：%cd%\App_Framework.uvprojx  
echo 3. 按F7编译或选择Project -> Build Target
echo 4. 检查编译输出窗口的结果

pause 