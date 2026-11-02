@echo off
setlocal enabledelayedexpansion

rem Инициализируем переменную для списка модов
set "modList="

rem Читаем моды из файла и создаём строку с модами
for /f "tokens=*" %%i in (modlist_10server.txt) do (
    rem Убираем пробелы в начале и конце строки
    set "line=%%i"
    for /f "tokens=*" %%b in ("!line!") do (
        set "line=%%b"
    )
    rem Пропускаем строки, которые начинаются с #
    if not "!line!"=="" (
        if not "!line:~0,1!"=="#" (
            rem Если строка не пустая и начинается с @, добавляем её в список модов
            if "!line:~0,1!"=="@" (
			
				REM Оригинальный путь
                set "modList=!modList!C:\D\MODS\!line!;"
				
				REM Путь на обновлениях
				REM set "modList=!modList!F:\Games\DayZavr_129\!line!;"
            )
        )
    )
)

rem Удаляем последний символ ";"
set "modList=%modList:~0,-1%"


REM ============================================================================================================================================================================================================================================================================
														REM Начало региона: работа с удалением и обновлением SERVERMOD
REM ============================================================================================================================================================================================================================================================================

set "SCRIPT_DIR=%~dp0"
set "DELETE_DIR=%SCRIPT_DIR%@_Delete_Servermod_On_Restart"
set "UPDATE_DIR=%SCRIPT_DIR%@_Update_Servermod_On_Restart"
set "SERVERMOD_ADDONS=%SCRIPT_DIR%@Servermod\Addons"


REM УДАЛЕНИЕ ФАЙЛОВ SERVERMOD

if exist "%DELETE_DIR%\" (
    if exist "%SERVERMOD_ADDONS%\" (

        echo.
        echo Проверяем файлы SERVERMOD для удаления...

        REM Получаем имена всех файлов из папки удаления
        for /f "delims=" %%F in ('dir /b /a-d "%DELETE_DIR%" 2^>nul') do (

            echo Обрабатываем файл: %%F

            REM Если такой файл существует в @Servermod\Addons — удаляем его
            if exist "%SERVERMOD_ADDONS%\%%F" (

                del /F /Q "%SERVERMOD_ADDONS%\%%F" >nul 2>&1

                REM Проверяем, действительно ли файл был удалён
                if exist "%SERVERMOD_ADDONS%\%%F" (
                    echo ОШИБКА: не удалось удалить файл из SERVERMOD: %%F
                    echo Файл останется в папке удаления до следующего запуска.
                ) else (
                    echo Удалён из SERVERMOD: %%F

                    REM Удаляем файл-задание из папки удаления
                    del /F /Q "%DELETE_DIR%\%%F" >nul 2>&1
                )

            ) else (

                REM Файла в SERVERMOD уже нет — удаляем его из папки-задания
                echo Файл отсутствует в SERVERMOD: %%F
                echo Удаляем его из папки заданий.

                del /F /Q "%DELETE_DIR%\%%F" >nul 2>&1
            )
        )

        echo Проверка файлов для удаления завершена.

    ) else (
        echo.
        echo ВНИМАНИЕ: папка "%SERVERMOD_ADDONS%" не найдена.
        echo Удаление файлов SERVERMOD пропущено.
    )
) else (
    echo.
    echo Папка "%DELETE_DIR%" не найдена.
    echo Удаление файлов SERVERMOD пропущено.
)


REM ОБНОВЛЕНИЕ ФАЙЛОВ SERVERMOD

if exist "%UPDATE_DIR%\" (
    if exist "%SERVERMOD_ADDONS%\" (

        REM Проверяем, есть ли PBO-файлы для обновления
        if exist "%UPDATE_DIR%\*.pbo" (

            echo.
            echo Перемещаем PBO-файлы из:
            echo "%UPDATE_DIR%"
            echo в:
            echo "%SERVERMOD_ADDONS%"

            move /Y "%UPDATE_DIR%\*.pbo" "%SERVERMOD_ADDONS%\" >nul 2>&1

            if errorlevel 1 (
                echo ОШИБКА: не удалось переместить некоторые PBO-файлы.
            ) else (
                echo Обновление SERVERMOD успешно завершено.
            )

        ) else (
            echo.
            echo В папке обновлений нет PBO-файлов.
        )

    ) else (
        echo.
        echo ВНИМАНИЕ: папка "%SERVERMOD_ADDONS%" не найдена.
        echo Обновление SERVERMOD пропущено.
    )
) else (
    echo.
    echo Папка "%UPDATE_DIR%" не найдена.
    echo Обновление SERVERMOD пропущено.
)


REM ============================================================================================================================================================================================================================================================================
													REM Конец региона: работа с удалением и обновлением SERVERMOD
REM ============================================================================================================================================================================================================================================================================


REM ОРИГИНАЛЬНЫЙ БАТНИК
start DayZServer_x6410.exe -port=2402 "-mod=%modList%" -serverMod=@Servermod -config=serverDZ.cfg -cfg=Instance_1\basic.cfg -profiles=Instance_1 -name=server -noLogs -adminlog -freezecheck -limitFPS=100 -noFilePatching


REM ТЕСТОВЫЙ БАТНИК
REM start DayZServer_x6410.exe -port=3202 "-mod=%modList%" -serverMod=@Servermod -config=serverDZ.cfg -cfg=Instance_1\basic.cfg -profiles=Instance_1 -name=server -noLogs -adminlog -freezecheck -limitFPS=80 -noFilePatching

REM Правильный порт сервера: 2402


REM =====================================================================ПЕРЕМЕЩЕНИЕ ЛОГОВ==================================================

REM Определение диска, на котором находится этот .bat файл
set CURRENT_DRIVE=%~d0

REM Определение полного пути к текущему .bat файлу
set FULL_PATH=%~dp0

REM Извлечение имени папки, в которой находится .bat файл
for %%* in ("%FULL_PATH:~0,-1%") do set SERVER_FOLDER=%%~nx*

REM Определение пути к папке назначения для логов
set DESTINATION_PATH=%CURRENT_DRIVE%\GLOBAL_SERVERS_LOGS\Logs_%SERVER_FOLDER%

REM Создание подпапки для логов, если она не существует
if not exist "%DESTINATION_PATH%" mkdir "%DESTINATION_PATH%"

REM Создание папки с временной меткой внутри подпапки
set FOLDER_NAME=%DATE:~0,2%.%DATE:~3,2%.%DATE:~6,4%__%TIME:~0,2%.%TIME:~3,2%
set FOLDER_NAME=%FOLDER_NAME: =0%
echo %FOLDER_NAME%

mkdir "%DESTINATION_PATH%\%FOLDER_NAME%"

REM Определение пути к папке, куда будут перемещены файлы
set TO="%DESTINATION_PATH%\%FOLDER_NAME%\" 
set TO_Access="%DESTINATION_PATH%\%FOLDER_NAME%\Codelock\Access\" 
set TO_Admin="%DESTINATION_PATH%\%FOLDER_NAME%\Codelock\Admin\" 
set TO_Attach="%DESTINATION_PATH%\%FOLDER_NAME%\Codelock\Attach\" 
set TO_Raid="%DESTINATION_PATH%\%FOLDER_NAME%\Codelock\Raid\" 
set TO_Synch="%DESTINATION_PATH%\%FOLDER_NAME%\Codelock\Synch\"
set TO_VPP="%DESTINATION_PATH%\%FOLDER_NAME%\VPPAdminTools\"

REM Создание подпапок для логов, если они не существуют
if not exist "%TO_Access%" mkdir "%TO_Access%"
if not exist "%TO_Admin%" mkdir "%TO_Admin%"
if not exist "%TO_Attach%" mkdir "%TO_Attach%"
if not exist "%TO_Raid%" mkdir "%TO_Raid%"
if not exist "%TO_Synch%" mkdir "%TO_Synch%"
if not exist "%TO_VPP%" mkdir "%TO_VPP%"

REM Перемещение логов из подпапок текущей папки сервера

REM Перемещение из папки Instance_1
move "%FULL_PATH%Instance_1\*.log" %TO%
move "%FULL_PATH%Instance_1\*.RPT" %TO%
move "%FULL_PATH%Instance_1\*.ADM" %TO%
move "%FULL_PATH%Instance_1\*.mdmp" %TO%
move "%FULL_PATH%Instance_1\*.ljson" %TO%

REM Перемещение из папки Airdrop
if exist "%FULL_PATH%Instance_1\Airdrop\Logs\*.log" (
    mkdir "%DESTINATION_PATH%\%FOLDER_NAME%\Airdrop" 2>nul
    move /Y "%FULL_PATH%Instance_1\Airdrop\Logs\*.log" "%DESTINATION_PATH%\%FOLDER_NAME%\Airdrop\"
)


REM Перемещение из папки Codelock
move "%FULL_PATH%Instance_1\CodeLock\Logs\Access\*.log" %TO_Access%
move "%FULL_PATH%Instance_1\CodeLock\Logs\Admin\*.log" %TO_Admin%
move "%FULL_PATH%Instance_1\CodeLock\Logs\Attach\*.log" %TO_Attach%
move "%FULL_PATH%Instance_1\CodeLock\Logs\Raid\*.log" %TO_Raid%
move "%FULL_PATH%Instance_1\CodeLock\Logs\Synch\*.log" %TO_Synch%

REM Перемещение из подпапки Depositary_System
move "%FULL_PATH%Instance_1\Depositary_System\Logs\*.log" %TO%

REM Перемещение из подпапки DayZCasinoV2
move "%FULL_PATH%Instance_1\DayZCasinoV2\*.csv" %TO%

REM Перемещение из подпапки DNA_Logging
move "%FULL_PATH%Instance_1\DNA_Logging\*.log" %TO%

REM Перемещение из подпапки VPPAdminTools
move "%FULL_PATH%Instance_1\VPPAdminTools\Logging\*.txt" %TO_VPP%

REM Перемещение из подпапки WheelLog
move "%FULL_PATH%Instance_1\WheelLog\*.log" %TO%


REM Перемещение из подпапки KGB/DupeCheckSystem
set TO_DUPE_CHECK_LOGS="%DESTINATION_PATH%\%FOLDER_NAME%\DUPE_CHECK_REPORTS\"
if not exist "%TO_DUPE_CHECK_LOGS%" mkdir "%TO_DUPE_CHECK_LOGS%"
move "%FULL_PATH%Instance_1\KGB_MODS\DupeCheckSystem\REPORTS\*.log" %TO_DUPE_CHECK_LOGS%

REM Перемещение из подпапки BRDK_MODS\DeathBox
set TO_DEATH_BOX="%DESTINATION_PATH%\%FOLDER_NAME%\DEATH_BOX\"
if not exist "%TO_DEATH_BOX%" mkdir "%TO_DEATH_BOX%"
move "%FULL_PATH%Instance_1\BRDK_MODS\DeathBox\*.log" %TO_Death_BOX%


REM Перемещение из подпапки RF\LOGGER\
move "%FULL_PATH%Instance_1\RF\LOGGER\logs\*.csv" %TO%

REM Перемещение Winch Logs
set TO_WINCHLOGS="%DESTINATION_PATH%\%FOLDER_NAME%\Winch_Logs\"
if not exist "%TO_WINCHLOGS%" mkdir "%TO_WINCHLOGS%"
REM Перемещение из подпапки \ArzzzMODS\WinchLogger
move "%FULL_PATH%Instance_1\ArzzzMODS\WinchLogger\*.log" %TO_WINCHLOGS%

REM Перемещение логов MBSL
set TO_MBSL="%DESTINATION_PATH%\%FOLDER_NAME%\MBSL\Logs\"
if not exist "%TO_MBSL%" mkdir "%TO_MBSL%"
move "%FULL_PATH%Instance_1\MBSL\Logs\*.txt" %TO_MBSL%

REM Перемещение WebApiLog дейзовский мусор
set TO_WebApiLog="%DESTINATION_PATH%\%FOLDER_NAME%\WebApiLog\"
if not exist "%TO_WebApiLog%" mkdir "%TO_WebApiLog%"
move "%FULL_PATH%Instance_1\WebApiLog\*.log" %TO_WebApiLog%

REM =====================================================================ЛОГИ INT_Mods И КОРНЯ СЕРВЕРА==================================================
REM Каждая папка переносится, только если в ней есть файлы, поэтому пустые подпапки не создаются

set "INT_SRC=%FULL_PATH%Instance_1\INT_Mods"
set "INT_TO=%DESTINATION_PATH%\%FOLDER_NAME%"

REM Перемещение всех .log из корня папки сервера (error_*.log и прочие)
if exist "%FULL_PATH%*.log" (
    mkdir "%DESTINATION_PATH%\%FOLDER_NAME%\Server_Root" 2>nul
    move /Y "%FULL_PATH%*.log" "%DESTINATION_PATH%\%FOLDER_NAME%\Server_Root\"
)

REM Перемещение логов INT_CarImmobilizer
if exist "%INT_SRC%\INT_CarImmobilizer\INT_CarImmobilizerLogs\*.log" (
    mkdir "%INT_TO%\INT_CarImmobilizer" 2>nul
    move /Y "%INT_SRC%\INT_CarImmobilizer\INT_CarImmobilizerLogs\*.log" "%INT_TO%\INT_CarImmobilizer\"
)

REM Перемещение логов INT_Banking
if exist "%INT_SRC%\INT_Banking\INT_BankingLogs\*.log" (
    mkdir "%INT_TO%\INT_Banking" 2>nul
    move /Y "%INT_SRC%\INT_Banking\INT_BankingLogs\*.log" "%INT_TO%\INT_Banking\"
)

REM Перемещение логов INT_Garage
if exist "%INT_SRC%\INT_Garage\INT_GarageLogs\*.log" (
    mkdir "%INT_TO%\INT_Garage" 2>nul
    move /Y "%INT_SRC%\INT_Garage\INT_GarageLogs\*.log" "%INT_TO%\INT_Garage\"
)

REM Перемещение логов INT_P2P_Trading
if exist "%INT_SRC%\INT_P2P_Trading\Logs_P2P_Trading\*.log" (
    mkdir "%INT_TO%\INT_P2P_Trading" 2>nul
    move /Y "%INT_SRC%\INT_P2P_Trading\Logs_P2P_Trading\*.log" "%INT_TO%\INT_P2P_Trading\"
)

REM Перемещение логов INT_PartyMode
if exist "%INT_SRC%\INT_PartyMode\PartyHistory\*.log" (
    mkdir "%INT_TO%\INT_PartyMode" 2>nul
    move /Y "%INT_SRC%\INT_PartyMode\PartyHistory\*.log" "%INT_TO%\INT_PartyMode\"
)

REM Перемещение логов INT_QUESTS
if exist "%INT_SRC%\INT_QUESTS\Logs\*.log" (
    mkdir "%INT_TO%\INT_QUESTS" 2>nul
    move /Y "%INT_SRC%\INT_QUESTS\Logs\*.log" "%INT_TO%\INT_QUESTS\"
)

REM Перемещение логов INT_BaseProtection
if exist "%INT_SRC%\INT_BaseProtection\Logs\*.log" (
    mkdir "%INT_TO%\INT_BaseProtection" 2>nul
    move /Y "%INT_SRC%\INT_BaseProtection\Logs\*.log" "%INT_TO%\INT_BaseProtection\"
)

REM Перемещение логов INT_AdminGameLogger
if exist "%INT_SRC%\INT_AdminGameLogger\Logs\*.log" (
    mkdir "%INT_TO%\INT_AdminGameLogger" 2>nul
    move /Y "%INT_SRC%\INT_AdminGameLogger\Logs\*.log" "%INT_TO%\INT_AdminGameLogger\"
)

REM Перемещение логов INT_BaseBoxAuthorization
if exist "%INT_SRC%\INT_BaseBoxAuthorization\INT_BaseBoxAuthorization_Logs\*.log" (
    mkdir "%INT_TO%\INT_BaseBoxAuthorization" 2>nul
    move /Y "%INT_SRC%\INT_BaseBoxAuthorization\INT_BaseBoxAuthorization_Logs\*.log" "%INT_TO%\INT_BaseBoxAuthorization\"
)


REM =====================================================================ОЧИСТКА СТАРЫХ ЛОГОВ==================================================

REM Удаление файлов старше 30 дней
forfiles /p "%DESTINATION_PATH%" /s /m *.* /d -30 /c "cmd /c if @isdir==FALSE del /q @path" 2>nul

REM Удаление пустых папок (цикл для удаления всех вложенных пустых папок)
:delete_empty_dirs
for /f "delims=" %%D in ('dir "%DESTINATION_PATH%" /ad /s /b ^| sort /r') do (
    if exist "%%D" rd "%%D"
)