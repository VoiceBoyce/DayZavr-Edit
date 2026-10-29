import json
import os
import re

def parse_anomaly_txt(file_path):
    anomalies = {}
    with open(file_path, 'r', encoding='utf-8') as file:
        content = file.read()

    # Разделяем содержимое файла на строки
    lines = content.splitlines()
    current_anomaly = None

    for line in lines:
        # Ищем строки с названием аномалии
        # Пробел перед двоеточием допустим: "Аномалия Holera :" (из имени файла с пробелом)
        anomaly_match = re.match(r'Аномалия\s+(?P<name>\w+)\s*:', line)
        if anomaly_match:
            current_anomaly = f"Anomaly_{anomaly_match.group('name')}"
            anomalies[current_anomaly] = []
        # Ищем строку с координатами
        elif current_anomaly and re.match(r'\s*\[\s*\d+\.\d+,\s*\d+\.\d+,\s*\d+\.\d+\s*\],?', line):
            coords = re.findall(r'[\d\.]+', line)
            coords = [float(x) for x in coords]
            anomalies[current_anomaly].append(coords)

    return anomalies

def update_anomaly_json(json_path, anomalies):
    with open(json_path, 'r', encoding='utf-8') as file:
        json_content = json.load(file)

    for item in json_content:
        anomaly_name = item['Classname_Anomaly_Object']
        
        # Если аномалия есть в .txt файле, обновляем её координаты
        if anomaly_name in anomalies:
            # Удаляем старые координаты
            item["Positions_Spawn_Anomaly"] = []
            # Добавляем новые координаты
            for coords in anomalies[anomaly_name]:
                item["Positions_Spawn_Anomaly"].append(coords)
            print(f"Обновлены координаты для: {anomaly_name}")
        else:
            # Если аномалии нет в .txt файле, оставляем её координаты пустыми
            item["Positions_Spawn_Anomaly"] = []
            print(f"Координаты не найдены для: {anomaly_name}")

    # Сохраняем JSON с улучшенным форматированием
    with open(json_path, 'w', encoding='utf-8') as file:
        # Используем dumps для гибкого форматирования
        json_content_str = json.dumps(json_content, ensure_ascii=False, indent=4)
        # Форматируем координаты в одну строку
        json_content_str = re.sub(r'\[\s*([\d\.]+),\s*([\d\.]+),\s*([\d\.]+)\s*\]', r'[\1, \2, \3]', json_content_str)
        file.write(json_content_str)

def main():
    print("Поиск файлов .txt и .json...")

    # Получаем список всех .txt и .json файлов
    txt_files = [f for f in os.listdir() if f.endswith('_Anomalies.txt')]
    json_files = [f for f in os.listdir() if f.endswith('_Anomalies.json')]

    # Выводим найденные файлы
    print("Найдены .txt файлы:")
    for file in txt_files:
        print(f"  {file}")

    print("Найден .json файлы:")
    for file in json_files:
        print(f"  {file}")

    # Запрашиваем подтверждение перед продолжением
    continue_processing = input("Продолжить обработку? (Y/N): ").strip().upper()
    if continue_processing != "Y":
        print("Обработка отменена.")
        return

    # Обработка файлов
    for txt_file in txt_files:
        section_name = txt_file.replace("_Anomalies.txt", "")
        json_file = f"{section_name}_Anomalies.json"

        if os.path.exists(json_file):
            print(f"\nОбработка секции: {section_name}")

            # Парсим .txt файл
            print(f"Чтение .txt файла: {txt_file}")
            anomalies = parse_anomaly_txt(txt_file)

            # Выводим найденные аномалии для дебага
            print("Найдены аномалии и их координаты:")
            for name, coords in anomalies.items():
                print(f"  Аномалия: {name}")
                print(f"  Координаты: {coords}")

            # Обновляем соответствующий .json файл
            print(f"Обновление .json файла: {json_file}")
            update_anomaly_json(json_file, anomalies)

            print(f"Секция {section_name} обработана.")
        else:
            print(f"Файл {json_file} не найден для секции {section_name}.")

    print("\nОбработка завершена.")

if __name__ == "__main__":
    main()
