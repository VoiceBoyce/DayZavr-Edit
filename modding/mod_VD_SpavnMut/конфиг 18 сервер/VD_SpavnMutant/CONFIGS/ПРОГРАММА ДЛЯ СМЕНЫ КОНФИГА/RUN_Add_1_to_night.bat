@echo off
chcp 65001 > nul
setlocal enabledelayedexpansion

set "input_file=Nochnoy.txt"
set "temp_file=%temp%\Nochnoy_temp.txt"

if not exist "%input_file%" (
    echo Файл "%input_file%" не найден!
    pause
    exit /b 1
)

set "skip=0"
(for /f "tokens=*" %%a in (%input_file%) do (
    if !skip! equ 0 (
        echo %%a
        set /a skip+=1
    ) else if !skip! equ 1 (
        echo %%a
        set /a skip+=1
    ) else (
        for /f "tokens=1-5 delims=|" %%b in ("%%a") do (
            set /a count=%%b+1
            echo !count!^|%%c^|%%d^|%%e^|%%f
        )
        set skip=0
    )
)) > "%temp_file%"

move /y "%temp_file%" "%input_file%" > nul
echo Количество мутантов увеличено на 1 в файле %input_file%