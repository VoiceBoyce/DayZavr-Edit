# Указываем кодировку файла
[System.Text.Encoding]::UTF8

# Размер карты
$mapWidth = 20480
$mapHeight = 20480

# Количество секций
$sectionsX = 4
$sectionsY = 4

# Размер одной секции (вручную заданные значения)
$cellSizeX = $mapWidth / $sectionsX  # 20480 / 4 = 5120
$cellSizeY = $mapHeight / $sectionsY # 20480 / 4 = 5120

# Фиксированные границы и центры секций
$sectionData = @{
    "A1" = @{ LeftBottom = @(0, 0); RightTop = @(5120, 5120); Center = @(2560, 2560) }
    "A2" = @{ LeftBottom = @(5120, 0); RightTop = @(10240, 5120); Center = @(7680, 2560) }
    "A3" = @{ LeftBottom = @(10240, 0); RightTop = @(15360, 5120); Center = @(12800, 2560) }
    "A4" = @{ LeftBottom = @(15360, 0); RightTop = @(20480, 5120); Center = @(17920, 2560) }
    "B1" = @{ LeftBottom = @(0, 5120); RightTop = @(5120, 10240); Center = @(2560, 7680) }
    "B2" = @{ LeftBottom = @(5120, 5120); RightTop = @(10240, 10240); Center = @(7680, 7680) }
    "B3" = @{ LeftBottom = @(10240, 5120); RightTop = @(15360, 10240); Center = @(12800, 7680) }
    "B4" = @{ LeftBottom = @(15360, 5120); RightTop = @(20480, 10240); Center = @(17920, 7680) }
    "C1" = @{ LeftBottom = @(0, 10240); RightTop = @(5120, 15360); Center = @(2560, 12800) }
    "C2" = @{ LeftBottom = @(5120, 10240); RightTop = @(10240, 15360); Center = @(7680, 12800) }
    "C3" = @{ LeftBottom = @(10240, 10240); RightTop = @(15360, 15360); Center = @(12800, 12800) }
    "C4" = @{ LeftBottom = @(15360, 10240); RightTop = @(20480, 15360); Center = @(17920, 12800) }
    "D1" = @{ LeftBottom = @(0, 15360); RightTop = @(5120, 20480); Center = @(2560, 17920) }
    "D2" = @{ LeftBottom = @(5120, 15360); RightTop = @(10240, 20480); Center = @(7680, 17920) }
    "D3" = @{ LeftBottom = @(10240, 15360); RightTop = @(15360, 20480); Center = @(12800, 17920) }
    "D4" = @{ LeftBottom = @(15360, 15360); RightTop = @(20480, 20480); Center = @(17920, 17920) }
}

# Функция для определения секции по координатам
function Get-Section($x, $z) {
    $col = [math]::Floor($x / $cellSizeX) + 1
    $row = [math]::Floor($z / $cellSizeY) + 1
    $rowChar = [char][int](64 + $row) # Преобразуем номер строки в букву (A, B, C, D)
    return "$rowChar$col"
}

# Инициализация хэш-таблицы для всех секций
$allSections = @{}
foreach ($section in $sectionData.Keys) {
    $allSections[$section] = @{}
}

# Выводим дебаг-информацию о границах и центрах секций
foreach ($section in $sectionData.Keys) {
    $cornersAndCenter = $sectionData[$section]
    Write-Host "Секция $section :"
    Write-Host "  Левый нижний угол: [$($cornersAndCenter.LeftBottom[0]), $($cornersAndCenter.LeftBottom[1])]"
    Write-Host "  Правый верхний угол: [$($cornersAndCenter.RightTop[0]), $($cornersAndCenter.RightTop[1])]"
    Write-Host "  Центр: [$($cornersAndCenter.Center[0]), $($cornersAndCenter.Center[1])]"
    Write-Host ""
}

# Получаем список файлов с аномалиями
$anomalyFiles = Get-ChildItem -Path .\* -Include Anomaly_*.txt

# Обрабатываем каждый файл с аномалиями
foreach ($file in $anomalyFiles) {
    $content = Get-Content -Path $file.FullName
    $anomalyName = $file.BaseName -replace "Anomaly_", ""

    Write-Host "Обработка файла: $($file.Name)"

    # Обрабатываем каждую строку с координатами
    foreach ($line in $content) {
        if ($line -match '\[([\d\.]+),\s*([\d\.]+),\s*([\d\.]+)\]') {
            $x = [double]$matches[1]
            $z = [double]$matches[3]

            Write-Host "Обработка координат: [$x, $z]"

            # Проверяем, что координаты находятся в пределах карты
            if ($x -lt 0 -or $x -gt $mapWidth -or $z -lt 0 -or $z -gt $mapHeight) {
                Write-Host "Координаты выходят за пределы карты: [$x, $z] в файле $($file.Name). Точка пропущена."
                continue
            }

            $section = Get-Section $x $z
            Write-Host "Секция: $section"

            # Добавляем координаты в соответствующую секцию
            if (-not $allSections[$section].ContainsKey($anomalyName)) {
                $allSections[$section][$anomalyName] = @()
            }
            $allSections[$section][$anomalyName] += $line
        }
    }
}

# Сохраняем данные в файлы для каждой секции
foreach ($section in $allSections.Keys) {
    $output = @()

    # Добавляем центр секции в начало файла
    $cornersAndCenter = $sectionData[$section]
    $output += "Секция $section :"
    $output += "  Левый нижний угол: [$($cornersAndCenter.LeftBottom[0]), $($cornersAndCenter.LeftBottom[1])]"
    $output += "  Правый верхний угол: [$($cornersAndCenter.RightTop[0]), $($cornersAndCenter.RightTop[1])]"
    $output += "  Центр: [$($cornersAndCenter.Center[0]), $($cornersAndCenter.Center[1])]"
    $output += "" # Пустая строка для разделения

    # Добавляем данные об аномалиях
    foreach ($anomalyName in $allSections[$section].Keys) {
        $output += "Аномалия $anomalyName`:"
        $output += $allSections[$section][$anomalyName] -join "`n"
        $output += "" # Пустая строка для разделения
    }

    # Сохраняем данные в файл
    if ($output.Count -gt 0) {
        $output | Out-File -FilePath "$section`_Anomalies.txt" -Encoding UTF8
    }
}

# Завершающее сообщение
Write-Host "Обработка завершена."
