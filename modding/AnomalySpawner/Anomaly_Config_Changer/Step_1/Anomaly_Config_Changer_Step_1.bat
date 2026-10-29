@echo off
setlocal enabledelayedexpansion

for %%F in (Anomaly_*.map) do (
    if exist "%%F" (
        set "output=%%~nF.txt"
        set "first=1"
        set "lastLine="
        > "!output!" (
            for /f "usebackq tokens=2 delims=|" %%A in ("%%F") do (
                for /f "tokens=1-3 delims= " %%X in ("%%A") do (
                    if defined lastLine echo			!lastLine!,
                    set "lastLine=			[%%X, %%Y, %%Z]"
                )
            )
            if defined lastLine echo		!lastLine!
        )
    )
)
