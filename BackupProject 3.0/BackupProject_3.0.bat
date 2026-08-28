@echo off
chcp 65001 > nul
setlocal enabledelayedexpansion

:: Загрузка расписания из файла
set "scheduleFile=scheduler.txt"
if not exist "%scheduleFile%" (
    echo File sheduller %scheduleFile% не найден!
    pause
    exit /b
)

:: Отображение расписания
echo Time to do next backups:
for /f "tokens=*" %%t in (%scheduleFile%) do (
    echo %%t
)

:: Начальный бекап при запуске
echo Start first backup on start...
goto :run_backup

:: Основной цикл для отслеживания расписания
:schedule_loop

set "nextBackupTime="
set "firstScheduledTime="

:: Получаем текущее время с точностью до секунд
for /f "tokens=1-3 delims=: " %%h in ('powershell -command "Get-Date -Format HH:mm:ss"') do (
    set "currentHour=%%h"
    set "currentMinute=%%i"
    set "currentSecond=%%j"
)
set "currentTime=!currentHour!:!currentMinute!:!currentSecond!"

:: Рассчитываем время следующего бекапа
for /f "tokens=*" %%t in (%scheduleFile%) do (
    if "!firstScheduledTime!"=="" set "firstScheduledTime=%%t"
    if "!nextBackupTime!"=="" (
        set "scheduledTime=%%t"
        if "!scheduledTime!" GTR "!currentTime!" (
            set "nextBackupTime=!scheduledTime!"
        )
    )
)

:: Если не найдено ближайшего времени для бекапа, планируем на следующий день
if "!nextBackupTime!"=="" (
    echo Today all backup are finished.
    echo Next backup will tomorrow in: !firstScheduledTime!
    set "nextBackupTime=!firstScheduledTime!"
    set "waitingForTomorrow=1"
) else (
    set "waitingForTomorrow="
    echo Nex backup will be in: !nextBackupTime!
)

:: Ожидание времени следующего бекапа
:wait_for_next_backup
for /f "tokens=1-3 delims=: " %%h in ('powershell -command "Get-Date -Format HH:mm:ss"') do (
    set "currentHour=%%h"
    set "currentMinute=%%i"
    set "currentSecond=%%j"
)
set "currentTime=!currentHour!:!currentMinute!:!currentSecond!"

:: Проверка условия выполнения бекапа
if defined waitingForTomorrow (
    if "!currentTime!"=="00:00:00" (
        goto :schedule_loop
    )
)

if "!currentTime!" GEQ "!nextBackupTime!" (
    if not defined waitingForTomorrow (
        echo Start copy in planned time for that: !nextBackupTime!
        goto :run_backup
    )
)

:: Ожидание перед повторной проверкой
timeout /t 1 /nobreak > nul
goto :wait_for_next_backup

:run_backup
for /f "tokens=2 delims==" %%i in ('"wmic os get localdatetime /value"') do set datetime=%%i
set "date=!datetime:~6,2!-!datetime:~4,2!-!datetime:~0,4!"
set "time=!datetime:~8,2!-!datetime:~10,2!"
set "timestamp=!date!_!time!"

echo Backup copy start...

:: Архивирование папок из paths.txt
echo Start archiving from file paths.txt...
for /f "usebackq tokens=1,2 delims=|" %%A in (`findstr /v "^=" paths.txt`) do (
    set "source=%%A"
    set "destinationFolder=%%B\!timestamp!"
    for %%F in ("%%A") do set "folderName=%%~nxF"
    set "destination=!destinationFolder!\!folderName!.zip"

    if not exist "!destinationFolder!" (
        mkdir "!destinationFolder!"
        echo Folder !destinationFolder! is created.
    )

    powershell -Command "Compress-Archive -Path '!source!' -DestinationPath '!destination!' -Force"
    echo Archive createt for folder: !source! в файле !destination!
)

:: Копирование файлов из filenames.txt
echo Copy files from из filenames.txt...
for /f "usebackq tokens=1,2 delims=|" %%A in (`findstr /v "^=" filenames.txt`) do (
    set "sourceFile=%%A"
    set "destinationFolder=%%B\!timestamp!"

    if not exist "!destinationFolder!" (
        mkdir "!destinationFolder!"
        echo Папка !destinationFolder! создана.
    )

    copy "!sourceFile!" "!destinationFolder!" > nul
    echo File !sourceFile! copied to folder !destinationFolder!.
)

echo Backup finished in !time!.

goto :schedule_loop
