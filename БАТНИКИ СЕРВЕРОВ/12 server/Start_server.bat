@echo off


setlocal enabledelayedexpansion

rem Определяем пути к файлам
set file1=C:\D\DayZServer12\Instance_1\DZSM\Z_ALL_SPAWN_POINTS_DataBase\CampEagle\Z_ALL_SPAWN_POINTS.json
set file2=C:\D\DayZServer12\Instance_1\DZSM\Z_ALL_SPAWN_POINTS_DataBase\OldTown\Z_ALL_SPAWN_POINTS.json
set file3=C:\D\DayZServer12\Instance_1\DZSM\Z_ALL_SPAWN_POINTS_DataBase\Duskar\Z_ALL_SPAWN_POINTS.json
set file4=C:\D\DayZServer12\Instance_1\DZSM\Z_ALL_SPAWN_POINTS_DataBase\Portland\Z_ALL_SPAWN_POINTS.json
set destination=C:\D\DayZServer12\Instance_1\DZSM\sets

rem Выбор случайного файла (от 0 до 3)
set /a "randomchoice=%random% %% 4"

echo Selected Variant: !randomchoice!

if !randomchoice!==0 (
    copy /Y "!file1!" "!destination!\Z_ALL_SPAWN_POINTS.json"
) else if !randomchoice!==1 (
    copy /Y "!file2!" "!destination!\Z_ALL_SPAWN_POINTS.json"
) else if !randomchoice!==2 (
    copy /Y "!file3!" "!destination!\Z_ALL_SPAWN_POINTS.json"
) else if !randomchoice!==3 (
    copy /Y "!file4!" "!destination!\Z_ALL_SPAWN_POINTS.json"
)

endlocal

setlocal enabledelayedexpansion


rem Инициализируем переменную для списка модов
set "modList="

rem Читаем моды из файла и создаём строку с модами
for /f "tokens=*" %%i in (modlist_12server.txt) do (
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
			
				REM оригинальный путь
                set "modList=!modList!C:\D\MODS\!line!;"
				
				REM путь на обновлениях
				REM set "modList=!modList!F:\Games\DayZavr_129\!line!;"
				
            )
        )
    )
)

rem Удаляем последний символ ";"
set "modList=%modList:~0,-1%"


REM ===================================================================
REM Начало региона перемещение обновлений Servermod из папки @_Update_Servermod_On_Restart
REM ===================================================================

set "SCRIPT_DIR=%~dp0"
set "UPDATE_DIR=%SCRIPT_DIR%@_Update_Servermod_On_Restart"
set "SERVERMOD_ADDONS=%SCRIPT_DIR%@Servermod\Addons"

if exist "%UPDATE_DIR%" (
    if exist "%SERVERMOD_ADDONS%" (
        echo Перемещаем PBO-файлы из %UPDATE_DIR% в %SERVERMOD_ADDONS%...
        move /Y "%UPDATE_DIR%\*.pbo" "%SERVERMOD_ADDONS%\" >nul 2>&1
        if errorlevel 1 (
            echo Ошибка при перемещении некоторых файлов.
        ) else (
            echo Перемещение успешно завершено.
        )
    ) else (
        echo ВНИМАНИЕ: %SERVERMOD_ADDONS% не найдена. Перемещение пропущено.
    )
) else (
    echo Папка обновлений не найдена, пропускаем перемещение.
)


REM ===================================================================
REM Конец региона Перемещение обновлений Servermod из папки @_Update_Servermod_On_Restart
REM ===================================================================


REM ОСНОВНОЙ БАТНИК
start DayZServer_x6412.exe -port=2428 "-mod=%modList%" -serverMod=@Servermod -config=serverDZ.cfg -cfg=Instance_1\basic.cfg -profiles=Instance_1 -name=server -noLogs -adminlog -freezecheck -noFilePatching

REM ТЕСТОВЫЙ БАТНИК
REM start DayZServer_x6412.exe -port=3202 "-mod=%modList%" -serverMod=@Servermod -config=serverDZ.cfg -cfg=Instance_1\basic.cfg -profiles=Instance_1 -name=server -noLogs -adminlog -freezecheck -noFilePatching


REM оригинальный порт 2428

endlocal


REM =====================================================================ПЕРЕМЕЩЕНИЕ ЛОГОВ==================================================

REM Определение полного пути к текущему .bat файлу
set FULL_PATH=%~dp0

REM Извлечение имени папки, в которой находится .bat файл
for %%* in ("%FULL_PATH:~0,-1%") do set SERVER_FOLDER=%%~nx*

REM Определение пути к папке назначения для логов
set DESTINATION_PATH=C:\D\GLOBAL_SERVERS_LOGS\Logs_%SERVER_FOLDER%

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
move "%FULL_PATH%Instance_1\Airdrop\Logs\*.log" %TO%


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


REM Џеремещение логов MBSL
set TO_MBSL="%DESTINATION_PATH%\%FOLDER_NAME%\MBSL\Logs\"
if not exist "%TO_MBSL%" mkdir "%TO_MBSL%"
move "%FULL_PATH%Instance_1\MBSL\Logs\*.txt" %TO_MBSL%

REM Џеремещение WebApiLog дейзовский мусор
set TO_WebApiLog="%DESTINATION_PATH%\%FOLDER_NAME%\WebApiLog\"
if not exist "%TO_WebApiLog%" mkdir "%TO_WebApiLog%"
move "%FULL_PATH%Instance_1\WebApiLog\*.log" %TO_WebApiLog%

REM =====================================================================ЛОГИ INT_Mods И КОРНЯ СЕРВЕРА==================================================
REM Каждая папка переносится, только если в ней есть файлы, поэтому пустые подпапки не создаются

set "INT_SRC=%FULL_PATH%Instance_1\INT_Mods"
set "INT_TO=%DESTINATION_PATH%\%FOLDER_NAME%\INT_Mods"

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