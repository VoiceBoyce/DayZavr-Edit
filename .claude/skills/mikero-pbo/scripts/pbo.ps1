<#
.SYNOPSIS
  Обёртка над Mikero's Tools: распаковка, упаковка, листинг и проверка конфига PBO.

.EXAMPLE
  pbo.ps1 list   D:\path\Mod.pbo
  pbo.ps1 unpack D:\path\Mod.pbo D:\work\unpacked          # создаст D:\work\unpacked\<prefix>\
  pbo.ps1 unpack D:\path\Mod.pbo D:\work\unpacked -Raw     # не конвертировать config.bin в config.cpp
  pbo.ps1 pack   D:\work\unpacked\Mod D:\out\Mod.pbo
  pbo.ps1 lint   D:\work\unpacked\Mod\config.cpp
#>
param(
    [Parameter(Mandatory)][ValidateSet('list', 'unpack', 'pack', 'lint')][string]$Action,
    [Parameter(Mandatory)][string]$Source,
    [string]$Destination,
    [switch]$Raw
)

$Bin = 'C:\Program Files (x86)\Mikero\DePboTools\bin'
if (-not (Test-Path "$Bin\DePbo64.dll")) { throw "Mikero не найден в $Bin" }
if (-not (Test-Path "$Bin\deOgg64.dll")) { throw "Нет deOgg64.dll — установи DeOgg, без него все exe падают с 0xC0000135" }

# Mikero требует абсолютные пути с буквой диска
$Source = (Resolve-Path $Source).Path
if ($Destination) { $Destination = [IO.Path]::GetFullPath($Destination) }

function Invoke-Mikero([string]$Exe, [string[]]$ArgList) {
    # Вывод идёт в stderr; через cmd собираем всё в одну строку и отбрасываем мусор про chcp
    $line = '"' + "$Bin\$Exe" + '" ' + (($ArgList | ForEach-Object { '"' + $_ + '"' }) -join ' ') + ' 2>&1'
    $out = cmd /c $line
    $code = $LASTEXITCODE
    $out | Where-Object { $_ -notmatch "chcp|operable program or batch file" }
    if ($code -ne 0) { Write-Error "$Exe завершился с кодом $code"; exit $code }
}

switch ($Action) {
    'list' { Invoke-Mikero 'ExtractPbo.exe' @('-P', '-LB', $Source) }
    'unpack' {
        if (-not $Destination) { throw 'Укажи папку назначения' }
        New-Item -ItemType Directory -Force $Destination | Out-Null
        $opts = if ($Raw) { '-PR' } else { '-P' }
        Invoke-Mikero 'ExtractPbo.exe' @($opts, $Source, $Destination)
    }
    'pack' {
        if (-not $Destination) { throw 'Укажи путь к итоговому .pbo' }
        New-Item -ItemType Directory -Force (Split-Path $Destination) | Out-Null
        Invoke-Mikero 'MakePbo.exe' @('-P', '-D', $Source, $Destination)
    }
    'lint' { Invoke-Mikero 'Rapify.exe' @('-P', '-L', $Source) }
}
