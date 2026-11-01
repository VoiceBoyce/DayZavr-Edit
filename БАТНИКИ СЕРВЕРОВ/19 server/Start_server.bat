@echo off

setlocal enabledelayedexpansion

set "modList="

for /f "tokens=*" %%i in (modlist_19server.txt) do (
    set "line=%%i"
    for /f "tokens=*" %%b in ("!line!") do (
        set "line=%%b"
    )
    if not "!line!"=="" (
        if not "!line:~0,1!"=="#" (
            if "!line:~0,1!"=="@" (
			
				REM Оригинальный путь
                set "modList=!modList!C:\MODS\!line!;"
				
				REM ПУТЬ НА ВРЕМЯ АПДЕЙТОВ
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
start DayZServer_x6419.exe -port=2718 "-mod=%modList%" -serverMod=@Servermod -config=serverDZ.cfg -cfg=Instance_1\basic.cfg -profiles=Instance_1 -name=server -noLogs -adminlog -freezecheck -noFilePatching

REM ТЕСТОВЫЙ БАТНИК
REM start DayZServer_x6419.exe -port=3202 "-mod=%modList%" -serverMod=@Servermod -config=serverDZ.cfg -cfg=Instance_1\basic.cfg -profiles=Instance_1 -name=server -noLogs -adminlog -freezecheck -noFilePatching


REM Оригинальный порт: 2718





REM =====================================================================ПЕРЕМЕЩЕНИЕ ЛОГОВ==================================================


REM Определение полного пути к текущему .bat файлу
set FULL_PATH=%~dp0

REM Извлечение имени папки, в которой находится .bat файл
for %%* in ("%FULL_PATH:~0,-1%") do set SERVER_FOLDER=%%~nx*

REM Определение пути к папке назначения для логов
set DESTINATION_PATH=C:\GLOBAL_SERVERS_LOGS\Logs_%SERVER_FOLDER%

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

REM Перемещение из папки BodyDeleter
move "%FULL_PATH%Instance_1\BodyDeleter\Logs\*.txt" %TO%

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

REM Перемещение из подпапки LogArchives(VehicleLogs)
move "%FULL_PATH%Instance_1\LogArchives\VehicleActivity\*.log" %TO%

REM Перемещение из подпапки KGB/DupeCheckSystem
set TO_DUPE_CHECK_LOGS="%DESTINATION_PATH%\%FOLDER_NAME%\DUPE_CHECK_REPORTS\"
if not exist "%TO_DUPE_CHECK_LOGS%" mkdir "%TO_DUPE_CHECK_LOGS%"
move "%FULL_PATH%Instance_1\KGB_MODS\DupeCheckSystem\REPORTS\*.log" %TO_DUPE_CHECK_LOGS%

REM Перемещение из подпапки BRDK_MODS\DeathBox
set TO_DEATH_BOX="%DESTINATION_PATH%\%FOLDER_NAME%\DEATH_BOX\"
if not exist "%TO_DEATH_BOX%" mkdir "%TO_DEATH_BOX%"
move "%FULL_PATH%Instance_1\BRDK_MODS\DeathBox\*.log" %TO_Death_BOX%

REM Перемещение из подпапки RF\KILLFEED\PLAYERS
set TO_KILLFEED="%DESTINATION_PATH%\%FOLDER_NAME%\KILLFEED\"
if not exist "%TO_KILLFEED%" mkdir "%TO_KILLFEED%"
move "%FULL_PATH%Instance_1\RF\KILLFEED\PLAYERS\*.json" %TO_KILLFEED%

REM Перемещение из KTPrivateTerritoryLogs
set TO_KTPrivateTerrytoryLogs="%DESTINATION_PATH%\%FOLDER_NAME%\KTPrivateTerrytoryLogs\"
if not exist "%TO_KTPrivateTerrytoryLogs%" mkdir "%TO_KTPrivateTerrytoryLogs%"
move "%FULL_PATH%Instance_1\KTPrivateTerritoryLogs\*.log" %TO_KTPrivateTerrytoryLogs%

REM Перемещение из подпапки RF\LOGGER\
move "%FULL_PATH%Instance_1\RF\LOGGER\logs\*.csv" %TO%

REM Перемещение Winch Logs
set TO_WINCHLOGS="%DESTINATION_PATH%\%FOLDER_NAME%\Winch_Logs\"
if not exist "%TO_WINCHLOGS%" mkdir "%TO_WINCHLOGS%"
REM Перемещение из подпапки \ArzzzMODS\WinchLogger
move "%FULL_PATH%Instance_1\ArzzzMODS\WinchLogger\*.log" %TO_WINCHLOGS%


REM Перемещение BDSM_Configs\Stash\logs
set TO_STASHLOGS="%DESTINATION_PATH%\%FOLDER_NAME%\BDSM_Configs_Stash_logs\"
if not exist "%TO_STASHLOGS%" mkdir "%TO_STASHLOGS%"
move "%FULL_PATH%Instance_1\BDSM_Configs\Stash\logs\*.log" %TO_STASHLOGS%


REM Перемещение логов MBSL
set TO_MBSL="%DESTINATION_PATH%\%FOLDER_NAME%\MBSL\Logs\"
if not exist "%TO_MBSL%" mkdir "%TO_MBSL%"
move "%FULL_PATH%Instance_1\MBSL\Logs\*.txt" %TO_MBSL%

REM Перемещение WebApiLog дейзовский мусор
set TO_WebApiLog="%DESTINATION_PATH%\%FOLDER_NAME%\WebApiLog\"
if not exist "%TO_WebApiLog%" mkdir "%TO_WebApiLog%"
move "%FULL_PATH%Instance_1\WebApiLog\*.log" %TO_WebApiLog%


REM Перемещение MF_Banking логов
set TO_MF_Banking="%DESTINATION_PATH%\%FOLDER_NAME%\MF_Banking\"
if not exist "%TO_MF_Banking%" mkdir "%TO_MF_Banking%"
move "%FULL_PATH%Instance_1\MF_Mods\MF_Banking\logs\*.log" %TO_MF_Banking%

REM Перемещение MF_Lib логов
set TO_MF_Lib="%DESTINATION_PATH%\%FOLDER_NAME%\MF_Lib\"
if not exist "%TO_MF_Lib%" mkdir "%TO_MF_Lib%"
move "%FULL_PATH%Instance_1\MF_Lib\logs\*.log" %TO_MF_Lib%


REM =====================================================================ОЧИСТКА СТАРЫХ ЛОГОВ==================================================

REM Удаление файлов старше 30 дней
forfiles /p "%DESTINATION_PATH%" /s /m *.* /d -30 /c "cmd /c if @isdir==FALSE del /q @path"

REM Удаление пустых папок (цикл для удаления всех вложенных пустых папок)
:delete_empty_dirs
for /f "delims=" %%D in ('dir "%DESTINATION_PATH%" /ad /s /b ^| sort /r') do (
    if exist "%%D" rd "%%D"
)

