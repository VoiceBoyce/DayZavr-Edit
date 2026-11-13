# Сборка VB_AnomaliesBosses (клиент) и VB_AnomaliesBosses_Server (сервер) из снимка исходников.
# Незаконченные файлы агентов выкидываются из снимка клиента (-Drop, маски относительно папки мода).
# Клиент копируется в тестовый клиент G:\Games\DayZavrTest\@VB_AnomaliesBosses,
# сервер — в G:\DayZ_Test\env111\servermods\@VB_AnomaliesBosses_Server (стенд: server_mods).
# Пример: build_snapshot.ps1 -Drop "scripts\4_World\VB_AB\VB_AB_Aura*.c"
param([string[]]$Drop = @())
$ErrorActionPreference = "Stop"
$mod = "D:\DayZavr\DayZavr-Edit\modding\mod_vb_AnomaliesBosses"
$src = "$mod\VB_AnomaliesBosses"
$srcS = "$mod\VB_AnomaliesBosses_Server"
$root = "$env:TEMP\vb_ab_build"
$snap = "$root\VB_AnomaliesBosses"
$snapS = "$root\VB_AnomaliesBosses_Server"
if (Test-Path $root) { Remove-Item -Recurse -Force $root }
New-Item -ItemType Directory -Force $snap, $snapS | Out-Null
robocopy $src $snap /MIR /R:2 /W:2 /NFL /NDL /NJH /NJS | Out-Null
robocopy $srcS $snapS /MIR /R:2 /W:2 /NFL /NDL /NJH /NJS | Out-Null
foreach ($mask in $Drop) {
    Get-ChildItem -Path (Join-Path $snap $mask) -File -ErrorAction SilentlyContinue | ForEach-Object {
        Write-Output ("dropped: " + $_.FullName.Substring($snap.Length + 1))
        Remove-Item $_.FullName -Force
    }
}
$env:PYTHONIOENCODING = "utf-8"
python "$mod\tools\ab_checknames.py" "$snap\scripts"
if ($LASTEXITCODE -ne 0) { throw "client: field names clash with vanilla types, see above" }
python "$mod\tools\ab_checknames.py" "$snapS\scripts"
if ($LASTEXITCODE -ne 0) { throw "server: field names clash with vanilla types, see above" }
python "$mod\tools\ab_splitcheck.py" "$snap\scripts" "$snapS\scripts"
if ($LASTEXITCODE -ne 0) { throw "client code uses server-only names, see above" }
$pbo = "D:\DayZavr\DayZavr-Edit\.claude\skills\dayzavr-pbo\scripts\pbo.ps1"
& $pbo build $snap 2>&1 | Select-Object -Last 3
& $pbo build $snapS 2>&1 | Select-Object -Last 3
robocopy "$root\@VB_AnomaliesBosses" "G:\Games\DayZavrTest\@VB_AnomaliesBosses" /MIR /R:2 /W:2 /NFL /NDL /NJH /NJS | Out-Null
if ($LASTEXITCODE -ge 8) { throw "copy to G:\Games\DayZavrTest failed (stand still running? close it first)" }
robocopy "$root\@VB_AnomaliesBosses_Server" "G:\DayZ_Test\env111\servermods\@VB_AnomaliesBosses_Server" /MIR /R:2 /W:2 /NFL /NDL /NJH /NJS | Out-Null
if ($LASTEXITCODE -ge 8) { throw "copy to G:\DayZ_Test\env111\servermods failed (stand still running? close it first)" }
# Only a full build (no -Drop) updates the ready mods in the repo (CLAUDE.md: modding\<folder>\@<Mod>)
if ($Drop.Count -eq 0) {
    robocopy "$root\@VB_AnomaliesBosses" "$mod\@VB_AnomaliesBosses" /MIR /R:2 /W:2 /NFL /NDL /NJH /NJS | Out-Null
    if ($LASTEXITCODE -ge 8) { throw "copy to $mod\@VB_AnomaliesBosses failed" }
    robocopy "$root\@VB_AnomaliesBosses_Server" "$mod\@VB_AnomaliesBosses_Server" /MIR /R:2 /W:2 /NFL /NDL /NJH /NJS | Out-Null
    if ($LASTEXITCODE -ge 8) { throw "copy to $mod\@VB_AnomaliesBosses_Server failed" }
}
Get-ChildItem "G:\Games\DayZavrTest\@VB_AnomaliesBosses\Addons", "G:\DayZ_Test\env111\servermods\@VB_AnomaliesBosses_Server\Addons" | Select-Object Name, Length
