<#
.SYNOPSIS
  Работа с PBO: распаковка и lint (Mikero), упаковка (CfgConvert + FileBank из DayZ Tools),
  подпись и проверка (DsUtils).

.EXAMPLE
  pbo.ps1 build  D:\DayZavr\DayZavr-Edit\modding\mod_vb_X\X   # X -> ..\@X\Addons\X.pbo + подпись + keys + проверка
  pbo.ps1 build  D:\...\mod_vb_X\X -Binarize                  # то же, модели .p3d бинаризуются (MLOD -> ODOL)
  pbo.ps1 list   D:\path\Mod.pbo
  pbo.ps1 unpack D:\path\Mod.pbo D:\work\unpacked          # создаст D:\work\unpacked\<prefix>\
  pbo.ps1 unpack D:\path\Mod.pbo D:\work\unpacked -Raw     # не конвертировать config.bin в config.cpp
  pbo.ps1 pack   D:\work\unpacked\Mod D:\out\Mod.pbo       # упаковать и подписать ключом DayZavrN
  pbo.ps1 pack   D:\work\unpacked\Mod D:\out\Mod.pbo -NoSign
  pbo.ps1 sign   D:\out\Mod.pbo                            # подписать готовый PBO
  pbo.ps1 check  D:\mods\@Mod\Addons                       # проверить подписи всех PBO в папке
  pbo.ps1 lint   D:\work\unpacked\Mod                      # проверить все config.cpp в папке (или один файл)
#>
param(
    [Parameter(Mandatory)][ValidateSet('build', 'list', 'unpack', 'pack', 'sign', 'check', 'lint')][string]$Action,
    [Parameter(Mandatory)][string]$Source,
    [string]$Destination,
    [switch]$Raw,
    [switch]$NoSign,
    [switch]$Binarize,
    [string]$Key = 'DayZavrN'
)

$ErrorActionPreference = 'Stop'
$Bin = 'C:\Program Files (x86)\Mikero\DePboTools\bin'
$Tools = 'D:\SteamLibrary\steamapps\common\DayZ Tools\Bin'
$DsUtils = "$Tools\DsUtils"
# Служебные файлы исходников, которые не должны попадать в PBO
$ExcludeFiles = @('pbo.json', '$PBOPREFIX$.txt', '$PBOPREFIX$')
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
    $out | Where-Object { $_ -notmatch "chcp|operable program or batch file" } | Out-Host
    if ($code -ne 0) { throw "$Exe завершился с кодом $code" }
}

function Invoke-Lint([string]$Path) {
    $files = if (Test-Path $Path -PathType Container) { Get-ChildItem $Path -Recurse -Filter config.cpp } else { Get-Item $Path }
    $bad = 0
    foreach ($f in $files) {
        $out = cmd /c "`"$Bin\Rapify.exe`" -P -L `"$($f.FullName)`" 2>&1"
        if (($out -join "`n") -notmatch 'No Error') {
            $bad++
            Write-Host "ОШИБКА в $($f.FullName):" -ForegroundColor Red
            $out | Where-Object { $_ -match 'circa|rror' } | Out-Host
        }
    }
    if ($bad) { throw "Ошибки в $bad config.cpp" }
    "Lint: $(@($files).Count) config.cpp без ошибок"
}

function Get-PboProperties([string]$Src) {
    # Источник заголовков: $PBOPREFIX$.txt, иначе pbo.json, иначе prefix = имя папки
    $props = [ordered]@{}
    $prefixFile = Join-Path $Src '$PBOPREFIX$.txt'
    $json = Join-Path $Src 'pbo.json'
    if (Test-Path -LiteralPath $prefixFile) {
        foreach ($l in Get-Content -LiteralPath $prefixFile) {
            if ($l -match '^\s*([A-Za-z]+)\s*=\s*(.*?);?\s*$') { $props[$Matches[1]] = $Matches[2] }
        }
    }
    elseif (Test-Path $json) {
        foreach ($h in (Get-Content $json -Raw -Encoding UTF8 | ConvertFrom-Json).headers) { $props[$h.name] = $h.value }
    }
    if (-not $props.Contains('prefix')) { $props['prefix'] = Split-Path $Src -Leaf }
    $props
}

function Get-P3dHeader([string]$Path) {
    # первые 4 байта модели: MLOD (исходник) или ODOL (бинарная); '' если файла нет
    if (-not (Test-Path -LiteralPath $Path)) { return '' }
    $fs = [IO.File]::OpenRead($Path)
    try { $buf = New-Object byte[] 4; $null = $fs.Read($buf, 0, 4); return [Text.Encoding]::ASCII.GetString($buf) }
    finally { $fs.Close() }
}

function Invoke-BinarizeModels([string]$Src, [string]$Stage, [string]$Prefix, [string]$Work) {
    # Модели MLOD -> ODOL (binarize.exe из DayZ Tools); бинарные копии заменяют .p3d в staging.
    # Бинаризатор ищет текстуры и материалы по путям <prefix>\...\x.paa на диске P:, поэтому исходники должны быть
    # видны как P:\<prefix> — ссылка-junction на папку исходников (создаётся сама, чужую не трогаем).
    # Подпапки binarize.exe не обходит: запуск на каждую папку с моделями.
    # Уже бинарные модели (ODOL, например правленые копии ванильных) не трогаем: binarize.exe на них падает (0xC0000005).
    # Путь — в полной форме, как его отдаёт Get-ChildItem (иначе 8.3-имя вида VOICEB~1 ломает относительные пути)
    $Src = (Get-Item -LiteralPath $Src).FullName.TrimEnd('\')
    $all = @(Get-ChildItem $Src -Recurse -Filter *.p3d)
    $models = @($all | Where-Object { (Get-P3dHeader $_.FullName) -eq 'MLOD' })
    $odol = $all.Count - $models.Count
    if ($odol) { "Уже ODOL, оставлены как есть: $odol" }
    if (-not $models) { return }
    if (-not (Test-Path 'P:\scripts')) { throw 'Диск P: не подключён (subst P: G:\DayZ_P): без него бинаризатор не найдёт текстуры' }
    $pPath = "P:\$Prefix"
    if (Test-Path $pPath) {
        $item = Get-Item $pPath
        $target = if ($item.LinkType) { [string](@($item.Target)[0]) } else { '' }
        if ($target -and (Test-Path -LiteralPath $target)) { $target = (Get-Item -LiteralPath $target).FullName.TrimEnd('\') }
        if (-not $target -or $target -ne $Src) {
            throw "$pPath уже существует и ведёт не на $Src — разобраться вручную"
        }
    }
    else {
        $parent = Split-Path $pPath
        if (-not (Test-Path $parent)) { New-Item -ItemType Directory -Force $parent | Out-Null }   # "P:\" трогать нельзя
        New-Item -ItemType Junction -Path $pPath -Target $Src | Out-Null
    }
    $exe = "$Tools\Binarize\binarize.exe"
    $binOut = "$Work\bin"
    $log = "$Work\binarize.log"
    foreach ($grp in ($models | Group-Object DirectoryName)) {
        $dir = $grp.Name
        $rel = $dir.Substring($Src.Length).TrimStart('\')
        $srcDir = if ($rel) { "$pPath\$rel" } else { $pPath }
        $dstDir = if ($rel) { "$binOut\$rel" } else { $binOut }
        New-Item -ItemType Directory -Force $dstDir | Out-Null
        # в папке есть ODOL — по одной модели, иначе всю папку одним запуском
        $masks = if (@(Get-ChildItem $dir -Filter *.p3d).Count -eq $grp.Count) { @('*.p3d') } else { @($grp.Group | ForEach-Object { $_.Name }) }
        foreach ($mask in $masks) {
            $argLine = "-always -maxProcesses=0 `"-addon=$pPath`" `"-textures=$Work\bintex`" `"$srcDir`" `"$dstDir`" `"$mask`""
            $p = Start-Process -FilePath $exe -ArgumentList $argLine -NoNewWindow -Wait -PassThru `
                -RedirectStandardOutput "$log.out" -RedirectStandardError "$log.err"
            Get-Content "$log.out", "$log.err" -ErrorAction SilentlyContinue | Add-Content $log
            if ($p.ExitCode -ne 0) { Get-Content $log -Tail 30 | Out-Host; throw "binarize.exe завершился с кодом $($p.ExitCode) на $srcDir\$mask" }
        }
    }
    # Каждая модель должна получить свежий ODOL (папка вывода новая, старых файлов в ней нет)
    $bad = @()
    $sizeBin = 0
    foreach ($m in $models) {
        $rel = $m.FullName.Substring($Src.Length).TrimStart('\')
        $b = "$binOut\$rel"
        if ((Get-P3dHeader $b) -ne 'ODOL') { $bad += $rel; continue }
        Copy-Item -LiteralPath $b "$Stage\$rel" -Force
        $sizeBin += (Get-Item -LiteralPath $b).Length
    }
    if ($bad) { Get-Content $log -Tail 40 | Out-Host; throw "Бинаризатор не сделал ODOL для: $($bad -join ', ')" }
    $sizeSrc = ($models | Measure-Object Length -Sum).Sum
    "Бинаризовано моделей: $($models.Count), $([math]::Round($sizeSrc / 1MB, 1)) -> $([math]::Round($sizeBin / 1MB, 1)) МБ"
}

function Invoke-Pack([string]$Src, [string]$Dst) {
    Invoke-Lint $Src
    $name = Split-Path $Src -Leaf
    $work = Join-Path $env:TEMP "pbo_build_$PID"
    $stage = "$work\stage\$name"
    try {
        # 1. Копия исходников во временную папку (оригинал не трогаем)
        $null = robocopy $Src $stage /MIR /NFL /NDL /NJH /NJS /NP /XF @ExcludeFiles
        if ($LASTEXITCODE -ge 8) { throw "robocopy завершился с кодом $LASTEXITCODE" }
        # 1a. По ключу -Binarize: модели MLOD -> ODOL
        if ($Binarize) {
            $prefix = (Get-PboProperties $Src)['prefix']
            Invoke-BinarizeModels $Src $stage $prefix $work
        }
        # 2. Бинаризация конфигов: config.cpp -> config.bin (CfgConvert из DayZ Tools)
        foreach ($cpp in Get-ChildItem $stage -Recurse -Filter config.cpp) {
            $binPath = Join-Path $cpp.DirectoryName 'config.bin'
            $out = & "$Tools\CfgConvert\CfgConvert.exe" -bin -dst $binPath $cpp.FullName 2>&1
            if ($LASTEXITCODE -ne 0 -or -not (Test-Path $binPath)) { throw "CfgConvert не смог бинаризовать $($cpp.FullName): $out" }
            Remove-Item $cpp.FullName
        }
        # 3. Упаковка FileBank (не проверяет p3d, в отличие от бесплатного MakePbo)
        $props = Get-PboProperties $Src
        $fbArgs = @()
        foreach ($k in $props.Keys) { $fbArgs += '-property'; $fbArgs += "$k=$($props[$k])" }
        New-Item -ItemType Directory -Force "$work\out" | Out-Null
        $out = & "$Tools\PboUtils\FileBank.exe" @fbArgs -exclude "$Tools\PboUtils\exclude.lst" -dst "$work\out" $stage 2>&1
        if ($LASTEXITCODE -ne 0 -or -not (Test-Path "$work\out\$name.pbo")) { $out | Out-Host; throw "FileBank завершился с кодом $LASTEXITCODE" }
        # 4. На место кладём только успешно собранный PBO
        New-Item -ItemType Directory -Force (Split-Path $Dst) | Out-Null
        Move-Item "$work\out\$name.pbo" $Dst -Force
    }
    finally {
        Remove-Item $work -Recurse -Force -ErrorAction SilentlyContinue
    }
    "Собрано: $Dst ($([math]::Round((Get-Item $Dst).Length / 1MB, 1)) МБ, prefix=$($props['prefix']))"
}

function Invoke-Sign([string]$Pbo) {
    $priv = "$DsUtils\$Key.biprivatekey"
    if (-not (Test-Path $priv)) { throw "Нет приватного ключа $priv" }
    # Старая подпись от прошлой сборки заведомо неверна — заменяем
    Remove-Item "$Pbo.$Key.bisign" -ErrorAction SilentlyContinue
    $out = cmd /c "`"$DsUtils\DSSignFile.exe`" `"$priv`" `"$Pbo`" 2>&1"
    $code = $LASTEXITCODE
    $out | Out-Host
    if ($code -ne 0 -or -not (Test-Path "$Pbo.$Key.bisign")) { throw "Подпись не создана (код $code)" }
    "Подписано: $Pbo.$Key.bisign"
}

function Invoke-Check([string]$AddonsDir) {
    # DSCheckSignatures всегда возвращает 0, поэтому результат разбираем по тексту
    $keys = Join-Path $env:TEMP "pbo_check_keys_$PID"
    New-Item -ItemType Directory -Force $keys | Out-Null
    Copy-Item "$DsUtils\$Key.bikey" $keys
    $out = cmd /c "`"$DsUtils\DSCheckSignatures.exe`" -deep `"$AddonsDir`" `"$keys`" 2>&1"
    Remove-Item $keys -Recurse -Force
    $out | Out-Host
    if ($out -match 'is wrong|No signature found|not found|error') { throw "Проверка подписей не пройдена (ключ $Key)" }
    "Все подписи $Key в порядке"
}

try {
    switch ($Action) {
        'build' {
            # Source — папка исходников <корень>\<Мод>; результат — <корень>\@<Мод>
            $name = Split-Path $Source -Leaf
            $modDir = Join-Path (Split-Path $Source) "@$name"
            Invoke-Pack $Source "$modDir\Addons\$name.pbo"
            Invoke-Sign "$modDir\Addons\$name.pbo"
            New-Item -ItemType Directory -Force "$modDir\keys" | Out-Null
            Copy-Item "$DsUtils\$Key.bikey" "$modDir\keys" -Force
            Invoke-Check "$modDir\Addons"
            "Готово: $modDir"
        }
        'list' { Invoke-Mikero 'ExtractPbo.exe' @('-P', '-LB', $Source) }
        'unpack' {
            if (-not $Destination) { throw 'Укажи папку назначения' }
            New-Item -ItemType Directory -Force $Destination | Out-Null
            $opts = if ($Raw) { '-PR' } else { '-P' }
            Invoke-Mikero 'ExtractPbo.exe' @($opts, $Source, $Destination)
        }
        'pack' {
            if (-not $Destination) { throw 'Укажи путь к итоговому .pbo' }
            Invoke-Pack $Source $Destination
            if (-not $NoSign) { Invoke-Sign $Destination }
        }
        'sign' { Invoke-Sign $Source }
        'check' { Invoke-Check $Source }
        'lint' { Invoke-Lint $Source }
    }
}
catch {
    Write-Host "ОШИБКА: $($_.Exception.Message)" -ForegroundColor Red
    exit 1
}
