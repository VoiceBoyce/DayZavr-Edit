# Сборка «Древнего корня» (сервер 18): проверка разделения клиент/сервер, клиентский @VB_AncientRoot
# (VB_AncientRoot.pbo + VB_AncientRootSky.pbo + VB_AncientRoot_GC.pbo), серверный @VB_AncientRoot_Server,
# копии на стенд (G:\Games\DayZavrTest — клиент, G:\DayZ_Test\env111\servermods — сервер).
# В «Обновлены или готовые моды» не кладёт: туда только готовый мод, по команде пользователя.
# Пока стенд запущен, копирование не пройдёт (файлы заняты) — сначала остановить стенд.
#
# Внутренний префикс клиентского PBO — VB_Rift (имена остались от Изнанки), а бинаризатор ищет текстуры моделей
# по P:\<префикс>. Ссылку P:\VB_Rift на время сборки переводим на исходники «Корня» и потом возвращаем как была
# (у Изнанки она ведёт на mod_vb_Rift\VB_Rift). rmdir удаляет только ссылку, не папку, на которую она ведёт.
param([switch]$NoStand)
$ErrorActionPreference = "Stop"
$mod = "D:\DayZavr\DayZavr-Edit\modding\mod_vb_AncientRoot"
$pbo = "D:\DayZavr\DayZavr-Edit\.claude\skills\dayzavr-pbo\scripts\pbo.ps1"
$env:PYTHONIOENCODING = "utf-8"
$pLink = "P:\VB_Rift"

function Get-LinkTarget([string]$path) {
    if (-not (Test-Path $path)) { return $null }
    $item = Get-Item $path -Force
    if (-not $item.LinkType) { throw "$path — обычная папка, а не ссылка: разобраться вручную" }
    return [string](@($item.Target)[0])
}

function Set-Link([string]$path, [string]$target) {
    if (Get-LinkTarget $path) { cmd /c rmdir "$path" | Out-Null }
    New-Item -ItemType Junction -Path $path -Target $target | Out-Null
}

python "$mod\tools\ar_splitcheck.py"
if ($LASTEXITCODE -ne 0) { throw "client code uses server-only names, see above" }

$prevLink = Get-LinkTarget $pLink
try {
    Set-Link $pLink "$mod\VB_AncientRoot"
    # модели бинаризуются (MLOD -> ODOL)
    & $pbo build "$mod\VB_AncientRoot" -Binarize 2>&1 | Select-Object -Last 4
}
finally {
    if ($prevLink) { Set-Link $pLink $prevLink } else { cmd /c rmdir "$pLink" | Out-Null }
}
# молнии — уже бинарные копии ванильных (gen_ar_lightning.py); аддон /rift в чате — после build клиента
& $pbo pack "$mod\VB_AncientRootSky" "$mod\@VB_AncientRoot\Addons\VB_AncientRootSky.pbo" 2>&1 | Select-Object -Last 2
& $pbo pack "$mod\VB_AncientRoot_GC" "$mod\@VB_AncientRoot\Addons\VB_AncientRoot_GC.pbo" 2>&1 | Select-Object -Last 2
& $pbo build "$mod\VB_AncientRoot_Server" 2>&1 | Select-Object -Last 3

if (-not $NoStand) {
    robocopy "$mod\@VB_AncientRoot" "G:\Games\DayZavrTest\@VB_AncientRoot" /MIR /R:2 /W:2 /NFL /NDL /NJH /NJS | Out-Null
    if ($LASTEXITCODE -ge 8) { throw "copy to DayZavrTest failed (stand still running? close it first)" }
    robocopy "$mod\@VB_AncientRoot_Server" "G:\DayZ_Test\env111\servermods\@VB_AncientRoot_Server" /MIR /R:2 /W:2 /NFL /NDL /NJH /NJS | Out-Null
    if ($LASTEXITCODE -ge 8) { throw "copy to env111\servermods failed (stand still running? close it first)" }
}
Get-ChildItem "$mod\@VB_AncientRoot\Addons", "$mod\@VB_AncientRoot_Server\Addons" | Select-Object Name, Length, LastWriteTime
exit 0
