# Сборка VB_MiningResources целиком: клиент (с бинаризацией моделей), кирка (отдельный PBO в том же @-моде), сервер.
# Кирка VB_MiningResources_Veles собирается в @VB_MiningResources_Veles\Addons и копируется в @VB_MiningResources\Addons:
# игрокам ставится один мод @VB_MiningResources (два PBO), сервер — VB_MiningResources_Server.pbo.
#   powershell -File build_all.ps1
$ErrorActionPreference = "Stop"
$pbo = "D:\DayZavr\DayZavr-Edit\.claude\skills\dayzavr-pbo\scripts\pbo.ps1"
$root = Split-Path -Parent $PSScriptRoot

& $pbo build (Join-Path $root "VB_MiningResources") -Binarize
if ($LASTEXITCODE -ne 0) { throw "сборка VB_MiningResources" }
& $pbo build (Join-Path $root "VB_MiningResources_Veles")
if ($LASTEXITCODE -ne 0) { throw "сборка VB_MiningResources_Veles" }
& $pbo build (Join-Path $root "VB_MiningResources_Server")
if ($LASTEXITCODE -ne 0) { throw "сборка VB_MiningResources_Server" }

$src = Join-Path $root "@VB_MiningResources_Veles\Addons"
$dst = Join-Path $root "@VB_MiningResources\Addons"
Copy-Item (Join-Path $src "VB_MiningResources_Veles.pbo") $dst -Force
Copy-Item (Join-Path $src "VB_MiningResources_Veles.pbo.DayZavrN.bisign") $dst -Force
& $pbo check $dst
Write-Host "Готово: @VB_MiningResources (клиент + кирка), @VB_MiningResources_Server"
