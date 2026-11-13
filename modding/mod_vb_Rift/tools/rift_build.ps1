# Сборка «Изнанки»: проверка разделения клиент/сервер, клиентский @VB_Rift (VB_Rift.pbo + VB_RiftSky.pbo),
# серверный @VB_Rift_Server (VB_Rift_Server.pbo), копии на стенд (DayZavrTest — клиент, env111\servermods — сервер).
# Пока стенд запущен, копирование не пройдёт (файлы заняты) — сначала остановить стенд.
$ErrorActionPreference = "Stop"
$mod = "D:\DayZavr\DayZavr-Edit\modding\mod_vb_Rift"
$pbo = "D:\DayZavr\DayZavr-Edit\.claude\skills\dayzavr-pbo\scripts\pbo.ps1"
$env:PYTHONIOENCODING = "utf-8"

python "$mod\tools\rift_splitcheck.py"
if ($LASTEXITCODE -ne 0) { throw "client code uses server-only names, see above" }

& $pbo build "$mod\VB_Rift" 2>&1 | Select-Object -Last 3
# VB_RiftSky — после build (build пересобирает папку Addons)
& $pbo pack "$mod\VB_RiftSky" "$mod\@VB_Rift\Addons\VB_RiftSky.pbo" 2>&1 | Select-Object -Last 2
& $pbo build "$mod\VB_Rift_Server" 2>&1 | Select-Object -Last 3

foreach ($dst in @("G:\Games\DayZavrTest\@VB_Rift", "G:\Games\DayZavr\@VB_Rift")) {
    robocopy "$mod\@VB_Rift" $dst /MIR /R:2 /W:2 /NFL /NDL /NJH /NJS | Out-Null
    if ($LASTEXITCODE -ge 8) { throw "copy to $dst failed (stand still running? close it first)" }
}
robocopy "$mod\@VB_Rift_Server" "G:\DayZ_Test\env111\servermods\@VB_Rift_Server" /MIR /R:2 /W:2 /NFL /NDL /NJH /NJS | Out-Null
if ($LASTEXITCODE -ge 8) { throw "copy to env111\servermods failed (stand still running? close it first)" }
Get-ChildItem "$mod\@VB_Rift\Addons", "$mod\@VB_Rift_Server\Addons" | Select-Object Name, Length, LastWriteTime
