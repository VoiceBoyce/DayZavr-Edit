# INT UI Framework — руководство разработчика

`@INT_UIFramework` — отдельный клиентский UI-фреймворк для модов DayZ. Даёт
современный тёмный дизайн, анимации, готовые окна, компоненты, диалоги и тосты.
Зависит только от ванили (`DZ_Data`, `DZ_Scripts`): без CF и без Dabs.

Версия: **2.0.0**. Архитектура, контракты и критерии приёмки описаны в [`SPEC.md`](SPEC.md).

Живая витрина: в игре нажмите **Ctrl+F10**. Откроется демо с двумя вкладками:
«Обзор» (кнопки, чипы, данные, список) и «Формы» (все поля ввода с живым превью).
Исходник витрины — [`INT_UIF_DemoMenu.c`](INT_UIFramework/scripts/5_Mission/INT_UIF_DemoMenu.c).
Это эталон того, как писать меню на фреймворке.

---

## Содержание

1. [Что внутри](#1-что-внутри)
2. [Подключение к своему моду](#2-подключение-к-своему-моду)
3. [Первое окно за пять минут](#3-первое-окно-за-пять-минут)
4. [Холст и координаты](#4-холст-и-координаты)
5. [Окно: INT_UIF_Menu](#5-окно-int_uif_menu)
6. [Компоненты](#6-компоненты)
7. [Примитивы: INT_UIF_Draw и INT_UIF_Surface](#7-примитивы-int_uif_draw-и-int_uif_surface)
8. [Диалоги и тосты](#8-диалоги-и-тосты)
9. [Тема, цвета, варианты](#9-тема-цвета-варианты)
10. [Анимации](#10-анимации)
11. [Владение объектами и жизненный цикл](#11-владение-объектами-и-жизненный-цикл)
12. [Локализация](#12-локализация)
13. [Иконки](#13-иконки)
14. [Тестирование](#14-тестирование)
15. [Сборка и генераторы](#15-сборка-и-генераторы)
16. [Подводные камни движка](#16-подводные-камни-движка)
17. [Структура проекта](#17-структура-проекта)

---

## 1. Что внутри

| Слой | Что даёт |
|---|---|
| **Окно** `INT_UIF_Menu` | Окно со шапкой (плитка приложения, заголовок, подзаголовок, два слота, крестик), телом, подвалом и подсказками клавиш под окном. Затемнение, размытие, заморозка игрока, Esc, модальные диалоги, анимация открытия и закрытия |
| **Компоненты** | Button, IconButton, Badge (чип), Card (панель), Divider, ProgressBar, Ring (кольцо), Callout (выноска), StatTile, List (богатые строки с аватарами, чипами и действиями по наведению), Toggle, Checkbox, TextInput, Slider, Stepper, Dropdown, Tabs (сегменты или подчёркивание), Tooltip |
| **Оверлеи** | Dialog (Confirm и Alert), Toast (стек справа сверху с очередью) |
| **Примитивы** | `INT_UIF_Draw`: рамка, картинка, иконка, текст, rich-текст, многострочный текст, поле ввода, скролл, хит-зона, аватар, плитка с иконкой, точка-индикатор, keycap, подпись секции. `INT_UIF_Surface`: скруглённая плашка 9-slice с рамкой, пилюля, тень, свечение |
| **Тема** | Палитра «Modern dark» (уголь, бирюзовый акцент, янтарь для главного действия) и вариант «Field». Переключение на лету, все компоненты перекрашиваются сами |
| **Анимации** | Твины alpha, позиции, размера, цвета, поворота, маски и значения. 21 кривая (включая OUT_BACK, OUT_ELASTIC, OUT_BOUNCE). Петли, `Reveal`-каскады, пульс |
| **Автотест** | Внутриигровой self-test (маркер-файл) плюс хук `OnTestCommand` для скриншотов |

## 2. Подключение к своему моду

### 2.1. config.cpp

В `CfgPatches` клиентского аддона добавьте фреймворк в `requiredAddons`. Это гарантирует,
что классы `INT_UIF_*` скомпилируются раньше ваших:

```cpp
class CfgPatches
{
	class MyMod_CL
	{
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Scripts", "INT_UIFramework"};
	};
};
```

### 2.2. Запуск

`@INT_UIFramework` грузится **и на сервере, и на клиенте**:
`-mod=...;@INT_UIFramework;@MyMod`. Сервер компилирует все скриптовые модули загруженных
модов, а ваш аддон ссылается на `INT_UIF_*`. Без фреймворка на сервере будет ошибка компиляции.
На выделенном сервере фреймворк ничего не делает: `INT_UIF_Runtime.Init` сразу выходит.

### 2.3. Где доступны классы

Все классы фреймворка лежат в `3_Game`, поэтому ими можно пользоваться в `3_Game`, `4_World` и `5_Mission`
вашего мода. Регистрацию меню обычно делают в `5_Mission`.

### 2.4. ID меню

Фреймворк занимает ID `9110001` (хост диалогов) и `9110002` (демо). Ваниль занимает 1..46.
Возьмите свой диапазон, например `9120000+`. Терминал защиты базы использует `900001`.

## 3. Первое окно за пять минут

```c
// MyMod_CL/scripts/5_Mission/MyMenu.c
const int MY_MENU = 9120001;

class MyMenu extends INT_UIF_Menu
{
	protected INT_UIF_List m_List;

	override string GetTitle()
	{
		return "#STR_MY_TITLE";
	}

	override string GetSubtitle()
	{
		return "#STR_MY_SUBTITLE";          // мелкая строка КАПСОМ под заголовком
	}

	override string GetIcon()
	{
		return "shield_fill";               // иконка плитки приложения (раздел 13)
	}

	override void GetWindowSize(out float w, out float h)
	{
		w = 1100;                           // дизайн-пиксели холста 1920x1080
		h = 680;
	}

	override void OnBuild(Widget body)
	{
		float bw = GetBodyW();
		float bh = GetBodyH();
		INT_UIF_Card card = INT_UIF_Card.Create(m_Bag, body, 0, 0, bw, bh, "#STR_MY_CAPTION", "users", "#STR_MY_PANEL");
		RevealOnOpen(card.GetRoot());       // каскадное появление при открытии

		Widget c = card.GetContent();
		float cw = card.GetContentW();
		float ch = card.GetContentH();
		m_List = INT_UIF_List.Create(m_Bag, c, 0, 0, cw, ch - 56, "#STR_MY_EMPTY");
		INT_UIF_Button ok = INT_UIF_Button.Create(m_Bag, c, 0, ch - 40, 200, 40, "#STR_MY_SAVE", INT_UIF_Variant.PRIMARY, "check");
		ok.OnClick().Insert(OnSave);
	}

	override void OnReady()
	{
		int r = m_List.AddRow("Intenso", "#STR_MY_OWNER");
		m_List.SetRowAvatar(r, "I", INT_UIF_ThemeKeys.AMBER);
		m_List.Reveal(0.3);
	}

	protected void OnSave(INT_UIF_Button button)
	{
		INT_UIF_Toast.Success("#STR_MY_SAVED");
		RequestClose();                     // закрытие с анимацией
	}
}

modded class MissionGameplay
{
	override void OnInit()
	{
		super.OnInit();
		INT_UIF_MenuRegistry.Register(MY_MENU, MyMenu);
	}
}
```

Открыть окно можно откуда угодно на клиенте: из обработчика клавиши, из RPC или из действия:

```c
INT_UIF_Menu.Open(MY_MENU);      // если открыто другое меню, оно закроется
INT_UIF_Menu.CloseCurrent();     // закрыть с анимацией
```

Как это работает: `Open` идёт ванильным путём `EnterScriptedMenu(id)`. Фреймворк модифицирует
`MissionGameplay.CreateScriptedMenu`: сначала спрашивает ваниль и другие моды, затем свой реестр.

## 4. Холст и координаты

Все размеры и позиции в v2 задаются в **дизайн-пикселях холста 1920×1080**.
`INT_UIF_Scale.Factor() = min(экран_h / 1080, экран_w / 1920)`. Окно 1100×680, нарисованное один раз,
выглядит одинаково на 1080p, 1440p, 4K и ultrawide. На 4:3 оно сжимается по ширине и не обрезается.

- `INT_UIF_Draw.S(v)` переводит дизайн-px в экранные px.
- **Размер шрифта — только через `INT_UIF_Scale.TextPx(size)`, не через `S()`.** Движок сам масштабирует
  `SetTextExactSize` по высоте экрана / 1080. С `S()` текст рос дважды: на 2560×1440 блоки ×1,33, а текст
  ×1,75, и в 2K/4K всё вылезало из блоков (замерено 2026-10-15). `TextPx` = дизайн-размер на 16:9 и
  ultrawide, на 4:3 / 5:4 — с поправкой по ширине. Все примитивы `INT_UIF_Draw.Text/Rich/MText` уже так делают.
- Каждый компонент строится в прямоугольнике `x, y, w, h` **относительно родителя**.
- Тело окна: `GetBodyW() × GetBodyH()` (окно минус шапка 76, отступы 16 и подвал).
- Контент карточки: `card.GetContent()`, его размеры `GetContentW()` и `GetContentH()`.
- Слоты шапки: 420×76 (`GetHeaderSlotW()`).

> Позиции, которые вы двигаете анимацией (`MoveX/MoveY/ResizeW`), передаются в **экранных px**:
> `INT_UIF_Animator.MoveX(w, INT_UIF_Draw.S(120), 0.3)`.

## 5. Окно: INT_UIF_Menu

### 5.1. Хуки (переопределяются в наследнике)

| Хук | По умолчанию | Назначение |
|---|---|---|
| `string GetTitle()` | `""` | Заголовок (жирный, 20 px) |
| `string GetSubtitle()` | `""` | Строка КАПСОМ под заголовком |
| `string GetIcon()` | `""` | Иконка плитки приложения; `""` — без плитки |
| `string GetIconHue()` | `amber` | Цвет плитки (ключ палитры) |
| `void GetWindowSize(out w, out h)` | 1100×720 | Размер окна в дизайн-px |
| `bool HasFooter()` | `false` | Подвал высотой 44 px |
| `bool ShowWindow()` | `true` | `false` — без окна, только затемнение (так устроен хост диалогов) |
| `bool CloseOnEscape()` | `true` | Esc закрывает по очереди: открытый выпадающий список, модальный диалог, окно |
| `bool FreezesPlayer()` | `true` | Блокирует ходьбу и камеру, прячет HUD |
| `bool UsesBlur()` | `true` | Размывает мир за окном |
| `bool WantsKeyboard()` | `false` | `true`, если в окне есть `TextInput`: меню забирает клавиатуру |
| `void OnBuildHeader(Widget center, Widget right)` | — | Слоты шапки. Вызывается **до** `OnBuild`, поэтому тело может цеплять страницы к вкладкам шапки |
| `void OnBuild(Widget body)` | — | Тело окна |
| `void OnBuildFooter(Widget footer)` | — | Подвал (если `HasFooter()`) |
| `void OnReady()` | — | Всё построено: заполнить данными, запустить анимации значений |
| `void OnApplyTheme()` | — | Перекрасить свои «сырые» виджеты при смене темы |
| `void OnTestCommand(string cmd)` | — | Хук автотеста: перевести окно в нужное для скриншота состояние |

Порядок вызовов: `OnBuildHeader` → `OnBuild` → `OnBuildFooter` → `OnReady` → при показе анимация открытия.

### 5.2. Помощники

| Метод | Что делает |
|---|---|
| `GetBodyW()`, `GetBodyH()`, `GetHeaderSlotW()` | Геометрия в дизайн-px |
| `GetBag()` / поле `m_Bag` | Мешок владения компонентами окна (раздел 11) |
| `GetBody()`, `GetFooter()` | Контейнеры |
| `RevealOnOpen(Widget w)` | Добавить панель в каскад появления: она проявится и поднимется на место |
| `AddHint(string key, string label)` | Подсказка клавиши под окном: `AddHint("#STR_INT_UIF_KEY_DBLCLICK", "#STR_MY_HINT")`. Подпись клавиши тоже может быть ключом (готовый `STR_INT_UIF_KEY_DBLCLICK` — «2×ЛКМ», «2×LMB» и т. д.). Подсказку для Esc окно добавляет само |
| `SetTitle(..)`, `SetSubtitle(..)` | Сменить текст шапки на лету |
| `RequestClose()` | Закрыть с анимацией; `CloseNow()` — без анимации |
| `ShowModal(INT_UIF_Dialog d)`, `HasModal()` | Модальный диалог внутри окна (обычно через `INT_UIF_Dialog.Confirm`) |
| `static Open(id)`, `static Current()`, `static CloseCurrent()` | Управление открытым окном |

### 5.3. Вкладки в шапке (паттерн витрины)

```c
override void OnBuildHeader(Widget center, Widget right)
{
	float slot = GetHeaderSlotW();
	m_Tabs = INT_UIF_Tabs.Create(m_Bag, center, (slot - 320) * 0.5, 20, 320, 36);
}

override void OnBuild(Widget body)
{
	m_Tabs.SetPageArea(body, 0, 0, GetBodyW(), GetBodyH());   // одна страница на вкладку
	m_Tabs.AddTab("#STR_MY_TAB_MAIN", "home");
	m_Tabs.AddTab("#STR_MY_TAB_SETTINGS", "gear");
	BuildMain(m_Tabs.GetPage(0));
	BuildSettings(m_Tabs.GetPage(1));
	m_Tabs.OnChanged().Insert(OnTab);        // void OnTab(int index, INT_UIF_Tabs tabs)
}
```

## 6. Компоненты

Общее у всех компонентов:

- Фабрика `static X Create(INT_UIF_Bag bag, Widget parent, x, y, w, ...)` строит компонент
  в прямоугольнике дизайн-px внутри `parent` и кладёт его в `bag`.
- События отдаются через `ScriptInvoker`. Сигнатура обработчика указана в таблицах.
- Общий API базового класса: `GetRoot()`, `Show(bool)`, `IsVisible()`, `SetEnabled(bool)`
  (выключенный компонент полупрозрачен и не реагирует), `IsEnabled()`, `SetTooltip(string)`,
  `GetWidth()`, `GetHeight()`, `Destroy()`.
- Цвета берутся из темы, красить вручную не нужно. Смена темы перекрашивает всё.
- Текст можно передавать ключом `#STR_...`: он переводится автоматически.

### 6.1. Кнопки и отображение

| Компонент | Фабрика | События и API |
|---|---|---|
| **Button** | `Create(bag, parent, x, y, w, h, text, variant = SECONDARY, icon = "")` | `OnClick()` → `(INT_UIF_Button b)`; `SetText`, `GetText`, `SetIcon`, `SetVariant`, `GetVariant`. Наведение подсвечивает заливку, рамку и свечение, нажатие притапливает подпись |
| **IconButton** | `Create(bag, parent, x, y, size, icon, variant = SECONDARY, tooltip = "")` | `OnClick()` → `(INT_UIF_IconButton b)`; `SetIcon`, `SetVariant` |
| **Badge** (чип) | `Create(bag, parent, x, y, text, variant = INFO, h = 22, icon = "", w = 0)` | `SetText`, `SetVariant`. Ширину удобно считать через `INT_UIF_Badge.FitWidth(text, h, icon)` |
| **Card** (панель) | `Create(bag, parent, x, y, w, h, caption, icon = "", title = "")` | `GetContent()`, `GetContentW()`, `GetContentH()`, `SetTitle`, `SetMeta(text, color)`, `AddTitleBadge(text, variant)` |
| **Divider** | `Create(bag, parent, x, y, w)` | Тонкая линия |
| **ProgressBar** | `Create(bag, parent, x, y, w, h = 8, value = 0, hueKey = "teal")` | `SetValue(v, animate = true)`, `GetValue`, `SetHue`, `SetLabel` |
| **Ring** | `Create(bag, parent, x, y, size, hueKey = "teal")` | `SetValue(0..1, animate = true)`: дуга по маске со счётчиком процентов. `SetCenterText`, `SetSubText`, `SetHue` |
| **Callout** | `Create(bag, parent, x, y, w, h, title, text, variant = WARNING, icon = "warning")` | `SetText(title, text)` |
| **StatTile** | `Create(bag, parent, x, y, w, h, label, value, hueKey = "text")` | `SetValue(text, hue)`, `CountTo(int, suffix, duration)` — счёт от нуля |

### 6.2. Список (INT_UIF_List)

```c
INT_UIF_List list = INT_UIF_List.Create(m_Bag, parent, x, y, w, h, "#STR_EMPTY");
int r = list.AddRow("Serega", "#STR_ADMIN", playerIndex);   // primary, secondary, data
list.SetRowAvatar(r, "S", INT_UIF_ThemeKeys.TEAL);             // или SetRowIcon(r, "user", hue)
list.SetRowBadge(r, "#STR_ADMIN", INT_UIF_Variant.SUCCESS, "shield");
list.AddRowAction(r, "remove", "user_minus", INT_UIF_Variant.DANGER, "#STR_REMOVE");  // появляется при наведении
list.SetRowButton(r2, "grant", "#STR_GRANT", INT_UIF_Variant.SUCCESS, "user_plus", 128);
list.OnAction().Insert(OnRowAction);      // (int row, string actionId, INT_UIF_List list)
list.OnSelected().Insert(OnRowSelected);  // (int row, INT_UIF_List list)
list.OnActivated().Insert(OnRowOpen);     // двойной клик: (int row, INT_UIF_List list)
list.Reveal(0.2);                          // строки въезжают по очереди
```

Ещё: `SetRowRight(row, text, color)`, `SetSelectable`, `Select(row, notify)`, `GetSelected`, `GetData(row)`,
`GetPrimary(row)`, `Count`, `Clear`, `SetEmptyText`, а для тестов `PinHover(row)` и `IsActionsShown(row)`.
Высота строки 56 px, зазор 8. Кнопки действий выезжают при наведении, чип при этом сдвигается влево.

### 6.3. Формы

| Компонент | Фабрика | События и API |
|---|---|---|
| **Toggle** | `Create(bag, parent, x, y, w, text, on = false, sub = "", h = 0)` | `OnChanged()` → `(bool on, INT_UIF_Toggle t)`; `IsOn`, `SetOn(on, notify = false, animate = true)`. Трек справа становится бирюзовым и светится, ручка перепрыгивает с пружинкой. С подстрокой `sub` высота 44, без неё 32 |
| **Checkbox** | `Create(bag, parent, x, y, w, text, checked = false, h = 28)` | `OnChanged()` → `(bool checked, INT_UIF_Checkbox c)`; `IsChecked`, `SetChecked(v, notify, animate)` |
| **TextInput** | `Create(bag, parent, x, y, w, h = 40, placeholder = "", maxLength = 0, icon = "", hintKey = "")` | `OnChanged()` → `(string text, INT_UIF_TextInput t)` на каждый символ; `OnSubmit()` → то же по Enter (один раз на нажатие; уход фокуса не отправляет, с 2.0.3). `GetText`, `SetText`, `SetPlaceholder`, `Focus`. `maxLength` считает **символы UTF-8**, а не байты. `hintKey` рисует keycap справа, например `"Enter"`. Фокус подсвечивает рамку бирюзой |
| **Slider** | `Create(bag, parent, x, y, w, min, max, value, step = 0, label = "", h = 0)` | `OnChanged()` → `(float value, INT_UIF_Slider s, bool isFinal)`: во время перетаскивания `isFinal = false`, при отпускании `true`. `GetValue`, `SetValue(v, notify, animate)`, `SetRange(min, max, step)`, `SetFormat(decimals, suffix)`. Работают перетаскивание, клик по дорожке и колесо |
| **Stepper** | `Create(bag, parent, x, y, w, min, max, value, step = 1, suffix = "", h = 36)` | `OnChanged()` → `(float value, INT_UIF_Stepper s)`; `GetValue`, `SetValue(v, notify)`. Кнопки ± гаснут на границах |
| **Dropdown** | `Create(bag, parent, x, y, w, options, selected = -1, placeholder = "", h = 40, icon = "")` | `OnSelected()` → `(int index, string text, INT_UIF_Dropdown d)`; `SetOptions`, `GetSelected`, `GetSelectedText`, `SetSelected(i, notify)`, `OpenPopup`, `ClosePopup`, `IsOpen`, `static CloseActive()` |
| **Tabs** | `Create(bag, parent, x, y, w, h = 40, style = SEGMENTED)` | `AddTab(title, icon = "", count = -1)` → индекс; `SetPageArea(host, x, y, w, h)` (до `AddTab`), `GetPage(i)`, `Select(i, notify, animate)`, `GetActive`, `Count`, `SetTitle`, `SetCount`; `OnChanged()` → `(int index, INT_UIF_Tabs t)` |

`INT_UIF_TabStyle.SEGMENTED` — дорожка-плитка, за активной вкладкой скользит приподнятая плашка с лёгким перелётом.
`UNDERLINE` — подписи над волосяной линией, под активной вкладкой тянется светящаяся бирюзовая полоса.
`count >= 0` рисует чип-счётчик. Если задана область страниц, вкладки переключают страницы, и новая страница
въезжает со стороны движения.

Выпадающий список открывает панель **поверх** меню (виджет верхнего уровня, сортировка 15000), поэтому карточки
её не обрезают. Панель открывается вниз, а если внизу нет места — вверх. Строки появляются каскадом, выбранная
отмечена бирюзовой галочкой. При больше чем 7 вариантах включается прокрутка колесом с тонким индикатором,
и список сразу прокручивается к выбранному варианту. Панель закрывается при выборе, по клику мимо, по Esc
и когда поле исчезает.

### 6.4. Подсказки (Tooltip)

```c
button.SetTooltip("#STR_MY_TIP");                   // любой компонент
INT_UIF_Tooltip.ShowFor(widget, "#STR_MY_TIP");    // вручную, над виджетом
INT_UIF_Tooltip.ShowText("text");                  // у курсора
INT_UIF_Tooltip.HideTip();
```

Пузырь появляется через 0.3 с (намерение навести) по центру над элементом. Если сверху нет места, он
встаёт под элементом и никогда не вылезает за край экрана. Ширина считается по реальному тексту.

## 7. Примитивы: INT_UIF_Draw и INT_UIF_Surface

Компоненты собраны из примитивов, и свои нестандартные элементы стоит собирать так же.
Все размеры в дизайн-px. Корни всех примитивов **пропускают мышь** (`ignorepointer`): мышь ловит только `Hit`.

```c
Widget   f   = INT_UIF_Draw.Frame(parent, x, y, w, h, "Name");                 // пустой контейнер
ImageWidget i = INT_UIF_Draw.Image(parent, x, y, w, h, "iuf_radial", color);  // ячейка атласа
ImageWidget t = INT_UIF_Draw.Texture(parent, x, y, w, h, "path/x.paa", color);
ImageWidget ic = INT_UIF_Draw.Icon(parent, x, y, 16, "shield", color);
ImageWidget ln = INT_UIF_Draw.Line(parent, x, y, w, 1, color);
TextWidget tx = INT_UIF_Draw.Text(parent, x, y, w, h, "#STR_X", 14, color, INT_UIF_Align.LEFT);   // big = true: крупный шрифт
RichTextWidget rt = INT_UIF_Draw.Rich(parent, x, y, w, h, value, 14, color);   // с inline-иконками
MultilineTextWidget mt = INT_UIF_Draw.MText(parent, x, y, w, h, value, 13, color);
EditBoxWidget eb = INT_UIF_Draw.Edit(parent, x, y, w, h, 14);                  // шрифт 14 px, по центру слота
ScrollWidget sw = INT_UIF_Draw.Scroll(parent, x, y, w, h);                      // + ScrollContent / SetScrollContent
ButtonWidget hit = INT_UIF_Draw.Hit(parent, x, y, w, h, handler);               // невидимая зона клика
float tw = INT_UIF_Draw.TextWidth("#STR_X", 14);                                // оценка ширины (0.53 em)

INT_UIF_Draw.Avatar(parent, x, y, 40, "I", hue);          // круг с буквой
INT_UIF_Draw.IconTile(parent, x, y, 40, "shield", hue);   // иконка в мягкой плитке
INT_UIF_Draw.Dot(parent, x, y, 8, color, true);           // точка с «дыханием»
float adv;
INT_UIF_Draw.Keycap(parent, x, y, "Esc", "#STR_CLOSE", adv);
INT_UIF_Draw.Caption(parent, x, y, w, "#STR_CAPS", "users");   // подпись секции КАПСОМ

INT_UIF_Surface box  = INT_UIF_Surface.Box(parent, x, y, w, h, 10, fill, border);  // радиусы 4..14
INT_UIF_Surface pill = INT_UIF_Surface.Pill(parent, x, y, w, h, fill, border);     // радиус h/2
INT_UIF_Surface.Shadow(parent, x - 24, y - 16, w + 48, h + 44, ARGB(150, 0, 0, 0));
INT_UIF_Surface glow = INT_UIF_Surface.Glow(parent, x - 14, y - 12, w + 28, h + 24, color);
box.FadeFill(newColor, 0.2);
box.FadeBorder(newColor, 0.2);    // рамка с альфой 0 скрыта
```

Полезные ячейки атласа: `iuf_white`, `iuf_radial` (мягкое пятно), `iuf_grad_v`, `iuf_grad_h`, `iuf_grad_band`,
`iuf_circle16/32/64`, `iuf_ringline16/32/64` и иконки `iuf_icon_*` (раздел 13).

Мелкие помощники `INT_UIF_Widgets`: `SetIcon(img, "check")`, `IconMarkup("plus")` (иконка внутри RichText),
`Contains(w, x, y)`, `IsDescendant(w, ancestor)`, `ClearChildren(parent)`, `Find*`.

## 8. Диалоги и тосты

```c
INT_UIF_Dialog d = INT_UIF_Dialog.Confirm("#STR_TITLE", "#STR_TEXT", "#STR_REMOVE", "#STR_CANCEL", INT_UIF_Variant.DANGER);
if (d)
	d.OnResult().Insert(OnConfirm);   // void OnConfirm(int result, INT_UIF_Dialog dlg)
// result: INT_UIF_Dialog.RESULT_YES (1), RESULT_NO (0) или INT_UIF_DIALOG_CANCEL (-1, Esc)

INT_UIF_Dialog.Alert("#STR_TITLE", "#STR_TEXT", INT_UIF_ToastKind.SUCCESS);
```

Если открыто наше окно, диалог становится оверлеем внутри него: клики за ним блокируются, Esc закрывает
сначала диалог. Если окна нет, фреймворк сам откроет хост `INT_UIF_DialogMenu`.

Если в этот момент открыто **ванильное** меню, фреймворк его закрывает. Так же работает любой
`INT_UIF_Menu.Open`. Инвентарь, пауза и карта закрываются ванильным путём (`INT_UIF_Menu.CloseVanillaMenu`:
`Mission.HideInventory`, `Mission.Continue`, `MapMenu.CloseMapMenu`), остальные меню — через `UIManager.CloseMenu`.
До 20.10.2026 инвентарь закрывался голым `CloseMenu`, и у игрока оставалась группа исключений ввода
`inventory`: до перезахода нельзя было поднять оружие, прицелиться, сменить стойку, нажать F
(тикет по уведомлению BaseProtection «Вас переместили»). Свои ванильные меню закрывайте той же функцией,
не `CloseMenu`.
Свой набор кнопок: `new INT_UIF_Dialog()` → `SetContent(..)` → `AddButton(label, variant)` → `INT_UIF_Dialog.Present(d)`.
Результат — индекс кнопки в порядке `AddButton`.

> **Исправлено 12.10.2026.** В сборках до этой даты результаты кнопок были перевёрнуты: «Да» в `Confirm()` возвращало
> `RESULT_NO`, а «Нет» — `RESULT_YES`. Кнопки строились справа налево и в том же порядке складывались в список.
> Все моды, которые используют диалоги (INT_Trader, INT_BaseProtection), должны работать с исправленной сборкой.
> Для тестов есть `d.PressLabel("#STR_...")`: нажимает кнопку с этой надписью так же, как клик мышью.

```c
INT_UIF_Toast.Success("#STR_SAVED");                  // Info / Success / Warning / Error
INT_UIF_Toast.Show("#STR_X", INT_UIF_ToastKind.INFO, 3.0);
```

Тосты въезжают справа сверху с пружинкой. Одновременно видно до 4, остальные ждут в очереди.
У каждого бежит полоска таймера. Открытое меню для тостов не нужно.

## 9. Тема, цвета, варианты

```c
int c   = INT_UIF.Color(INT_UIF_ThemeKeys.TEAL);      // цвет активной темы
int s   = INT_UIF.Soft(INT_UIF_ThemeKeys.RED, 40);    // тот же цвет с альфой 40
float p = INT_UIF.Px(24);                              // 24 дизайн-px в экранных
INT_UIF_ThemeManager.Get().SetActive("khaki");        // переключить тему: всё перекрасится
```

**Ключи палитры** (`INT_UIF_ThemeKeys`): `OVERLAY`, `SHADOW`, `WINDOW`, `WINDOW_BORDER`, `PANEL`, `PANEL_BORDER`,
`TILE`, `TILE_HOVER`, `TILE_BORDER`, `SHEEN`, `TEXT`, `TEXT_MUTED`, `TEXT_FAINT`, `TEXT_DISABLED`, `TEXT_ON_ACCENT`,
`TEAL`, `AMBER`, `AMBER_TEXT`, `RED`, `GREEN`, `BLUE`, `VIOLET`, `TRACK`, `KNOB`, а также совместимые
`ACCENT`, `SUCCESS`, `WARNING`, `DANGER`, `INFO` и их `*_HOVER`. Метрики: `ANIM_FAST` (0.14), `ANIM_NORMAL` (0.24),
`ANIM_SLOW` (0.4), `ROW_HEIGHT`, `TOAST_WIDTH`, `TOAST_HEIGHT`.

**Варианты** (`INT_UIF_Variant`) задают роль элемента:

| Вариант | Вид | Когда |
|---|---|---|
| `PRIMARY` | Сплошной янтарь | Главное действие экрана, одно на окно |
| `ACCENT` | Сплошная бирюза | Позитивное главное действие |
| `SECONDARY` | Нейтральная плитка | Обычные кнопки |
| `GHOST` | Без фона | Третичные действия |
| `SUCCESS` / `DANGER` / `WARNING` / `INFO` | Мягкая заливка, рамка и текст своего цвета | Чипы, действия с подтекстом |

**Своя тема:**

```c
INT_UIF_Theme t = new INT_UIF_Theme("night", "Night ops");
t.SetColor(INT_UIF_ThemeKeys.TEAL, ARGB(255, 120, 200, 255));
t.InheritFrom(INT_UIF_ThemePresets.Tactical());   // остальное взять из базовой
INT_UIF_ThemeManager.Get().Register(t);
INT_UIF_ThemeManager.Get().SetActive("night");
```

## 10. Анимации

```c
INT_UIF_Animator.FadeIn(w, 0.2);
INT_UIF_Animator.FadeOut(w, 0.15);                        // hideOnEnd = true
INT_UIF_Animator.FadeInColor(overlay, 0.24);              // для полупрозрачных подложек (раздел 16)
INT_UIF_Animator.MoveX(w, INT_UIF_Draw.S(100), 0.3, INT_UIF_Ease.OUT_BACK);
INT_UIF_Animator.ResizeW(w, INT_UIF_Draw.S(240), 0.3);
INT_UIF_Animator.ColorTo(w, color, 0.2);
INT_UIF_Animator.Rotate(w, 180, 0.25);                   // поворот в плоскости экрана
INT_UIF_Animator.Mask(img, 0.75, 0.9);                   // прогресс маски (кольца)
INT_UIF_Animator.Reveal(w, delay, 10, 0.34);             // появиться, поднявшись на 10 px
INT_UIF_Animator.Pulse(w, 0.1, 0.6, 1.8);                // бесконечное «дыхание»

INT_UIF_Tween t = INT_UIF_Animator.Value(0, 100, 0.8);   // твин без виджета
t.OnUpdate().Insert(OnValue);                            // (float v, INT_UIF_Tween t)
t.OnComplete().Insert(OnDone);                           // (INT_UIF_Tween t)
INT_UIF_Animator.Wait(0.5).OnComplete().Insert(OnLater); // таймер
t.Delay(0.2).Loop(-1, true);                             // задержка, бесконечная петля туда-обратно
INT_UIF_Animator.Get().Cancel(w);                        // остановить всё на виджете
```

Новый твин того же свойства того же виджета заменяет старый. Кривые `INT_UIF_Ease`: `LINEAR`,
`IN_/OUT_/IN_OUT_` × `SINE, QUAD, CUBIC, QUART, EXPO, BACK`, а также `OUT_ELASTIC` и `OUT_BOUNCE`.
Для каскада передавайте разные задержки, например `0.04 + i * 0.06`.

## 11. Владение объектами и жизненный цикл

Виджет держит на свой обработчик только **слабую** ссылку. Компонент, на который нет `ref`,
удаляется сборщиком, и его виджеты пропадают вместе с ним. Поэтому:

- каждая фабрика принимает `INT_UIF_Bag`. Передавайте `m_Bag` окна или `GetBag()` родительского компонента.
  Мешок держит компоненты, пока живёт окно;
- в полях окна храните компоненты **обычными** (не `ref`) ссылками: владелец — мешок;
- `null` вместо мешка допустим, только если вы сами храните компонент в `ref`-поле.

Ещё несколько правил:

- **Не стройте UI прямо в RPC-обработчике**: рабочее пространство клиента может ещё не существовать.
  Открывайте меню через `INT_UIF_Menu.Open`, а данные передавайте ему через свой кэш и `ScriptInvoker`.
  Так сделан терминал защиты базы.
- Обработчик клика может закрыть окно и освободить сам компонент. Фреймворк держит локальную ссылку
  `keep` на время вызова, повторять это не нужно.
- `INT_UIF_Runtime.Init()` и `Shutdown()` фреймворк вызывает сам в `MissionGameplay.OnInit` и `OnMissionFinish`.
  Виджеты верхнего уровня (тосты, подсказка, попапы) снимаются при выходе из миссии.

## 12. Локализация

- Весь текст отдавайте ключами `#STR_...` из `stringtable.csv` своего мода. Фреймворк переводит их сам,
  в том числе при подсчёте ширины (`TextWidth`, `Badge.FitWidth`).
- CSV должен содержать **полную шапку из 14 колонок**:
  `Language,original,english,spanish,german,russian,chinesesimp,french,italian,polish,czech,hungarian,japanese,portuguese`.
  С неполной шапкой движок оставляет ключи сырыми.
- Строки фреймворка переведены на все 12 языков. Источник — [`tools/i18n/int_uif.json`](tools/i18n/int_uif.json),
  CSV собирает `tools/gen_stringtable.py`.
- Если строку нужно склеить из кусков, переводите куски заранее: `Widget.TranslateString("#STR_X") + " " + value`.

## 13. Иконки

`check`, `close`, `chevron_up`, `chevron_down`, `chevron_left`, `chevron_right`, `plus`, `minus`, `info`, `warning`, `error`,
`success`, `search`, `user`, `users`, `user_plus`, `user_minus`, `shield`, `shield_check`, `shield_fill`, `clock`,
`calendar`, `gear`, `lock`, `flag`, `crown`, `swap`, `radar`, `pin`, `home`, `trash`, `refresh`, `logout`, `key`, `star`,
`eye`, `list`, `wallet`, `target`, `door`, `bolt`, `dot`.

Иконки задаются по имени без префикса: `Icon(parent, x, y, 16, "shield", color)`, `Button.Create(..., "check")`.
Несуществующее имя рисуется **белым квадратом без ошибки в логе** (раздел 16), поэтому сверяйтесь со списком.

## 14. Тестирование

### 14.1. Автотест фреймворка

Положите файл `int_uif_selftest.txt` в папку профиля **клиента** и зайдите на сервер.
Через секунду после входа в скриптовый лог клиента пишется:

```
[INT_UIF] [TEST] PASS button.root
...
[INT_UIF] [TEST] DONE pass=<n> fail=0
```

Первая строка файла определяет, что останется на экране:

| Содержимое | Результат |
|---|---|
| пусто или любой другой текст | Демо открывается, проверяется и закрывается |
| `keep` | Демо остаётся открытым (для скриншота) |
| `keep forms` | Демо остаётся открытым на вкладке «Формы» |
| `keep forms popup` | То же, плюс открытый выпадающий список часового пояса |
| `keep forms popup scrolled` | То же, и список прокручен до конца (проверка прокрутки и индикатора) |
| `legacy` | Обычный прогон, плюс в конце контроль старого пути: инвентарь закрывается голым `UIManager.CloseMenu`, и в лог пишется `[TEST] CONTROL ...`, осталось ли исключение `inventory`. Контроль нарочно ломает инвентарь до перезахода, поэтому по умолчанию он выключен |

Что проверяется:

- корни и родители всех компонентов, отсутствие «бродячих» виджетов верхнего уровня;
- функции форм: состояние переключателя, ограничение и шаг слайдера, формат значения, степпер на границе,
  выбор в выпадающем списке и закрытие по Esc, длина ввода в символах UTF-8, отправка по Enter,
  страницы вкладок, подсказка;
- настоящий путь меню `EnterScriptedMenu`;
- диалог подтверждения: «Да» возвращает `RESULT_YES`, «Нет» — `RESULT_NO` (`dialog.yes_reports_yes`, `dialog.no_reports_no`);
- диалог поверх открытого ванильного инвентаря (без `keep`): инвентарь закрыт ванильным путём, группа
  исключений `inventory` снята сразу, а после закрытия диалога не осталось ни `inventory`, ни `menu`,
  ни ограничения `INVENTORY` (`vanilla.inventory_opened`, `vanilla.dialog_on_top`,
  `vanilla.inventory_exclude_released`, `vanilla.dialog_closed`, `vanilla.inputs_free_after_dialog`).

После прогона **удалите маркер**.

### 14.2. Свои окна

Переопределите `OnTestCommand(string cmd)` и переведите окно в нужное состояние: откройте вкладку,
наведите строку через `List.PinHover`, откройте `Dropdown.OpenPopup`. Для тестовых данных заведите свой маркер,
как `int_bp_preview.txt` у терминала защиты базы: он заполняет кэш примерами и открывает окно без сервера.

### 14.3. Скриншоты через dayz-mcp

`client_shot` снимает окно клиента без фокуса. Клик `ui_click via=cursor` в оконном клиенте промахивается
примерно на 31 px (рамка окна), поэтому наведение проверяйте программно (`PinHover`), а не курсором.

## 15. Сборка и генераторы

Сборка — только через **dayz-mcp**: `project_open(<корень фреймворка>)` → `mod_lint(strict)` → `mod_build`.
CLI AddonBuilder и самодельные Python-упаковщики не использовать: они собирают устаревший или пустой PBO.
После сборки удалите `@INT_UIFramework/keys`. Перед пересборкой остановите сервер стенда: он держит PBO открытым.

Всё визуальное генерируется скриптами из `tools/`. Файлы `.layout`, `.imageset` и `.styles` руками не правятся.

| Скрипт | Что пишет |
|---|---|
| `tools/gen_iuf_atlas.py` | Атлас `iuf_ui_ca.png` (512×512: 9-slice плашки, пилюли, тень, свечение, круги, градиенты, 42 иконки), кольца и маску, `iuf_ui.imageset`, `iuf.styles`. PNG → PAA через dayz-mcp `asset_convert` |
| `tools/gen_layouts.py` | Все 41 `.layout` и `INT_UIF_LayoutTable.c` (литеральные пути `CreateWidgets`). Удаляет устаревшие `.layout` |
| `tools/gen_stringtable.py` | `stringtable.csv` из `tools/i18n/int_uif.json` (12 языков) |
| `tools/iuf_kit.py`, `tools/mock_terminal.py` | Python-макеты дизайна для быстрой итерации через `ui_load("$profile:...")` без пересборки |

## 16. Подводные камни движка

Всё ниже измерено в игре, и фреймворк это уже учитывает. Знать стоит, если пишете свои примитивы.

| Ловушка | Как обходим |
|---|---|
| `CreateWidgets` с путём, собранным в рантайме, иногда возвращает дерево **другого** layout | Только литеральные пути: генерируемая `INT_UIF_LayoutTable` |
| `Widget.SetAlpha` **заменяет** альфу цвета: подложка 66 % после `SetAlpha(1)` становится непрозрачной | `FadeInColor` / `FadeInTo` до собственной альфы цвета |
| По умолчанию `clipchildren 1`: потомки за пределами родителя не рисуются (ваниль явно пишет `clipchildren 0` 1506 раз) | У контейнеров фреймворка `clipchildren 0`, поэтому тени и свечения видны. У скролла `clipchildren 1` |
| Рамка без `ignorepointer` перехватывает наведение и клики | Корни всех примитивов — `ignorepointer 1`, мышь ловит только `Hit` |
| Отсутствующая текстура или ячейка атласа рисуется белым и ничего не пишет в лог | После изменений атласа проверять имена ячеек поиском |
| Маска кольца должна нести значение и в RGB, и в альфе | Генератор пишет маску RGB+A |
| Поворот виджета в плоскости экрана задаёт **третий** угол: `SetRotation(0, 0, deg)` (ваниль: `mapmenu.c`, `carhud.c`) | Так устроен твин `ROTATION` |
| Обычный EditBox подгоняет шрифт под **свою высоту**: `"exact text size"` (его понимает только MultilineEditBox) и `text_proportion` он игнорирует | Один `px_edit`; `Draw.Edit` делает поле высотой ровно с кегль и центрирует его, а клик по всему полю ловит отдельная хит-зона |
| Зажатую кнопку мыши опрашивают через `GetMouseState(MouseState.LEFT) & MB_PRESSED_MASK` | Перетаскивание слайдера, клик мимо выпадающего списка |
| Исключения ввода инвентаря, паузы и карты ставит и снимает **миссия**: `UIManager.CloseMenu(MENU_INVENTORY)` закрывает окно, но группа `inventory` (прицел, стойки, F, жесты) остаётся включённой до перезахода | `INT_UIF_Menu.CloseVanillaMenu`: `Mission.HideInventory` / `Mission.Continue` / `MapMenu.CloseMapMenu` |
| Нативный скроллбар ScrollWidget — белая полоса, которую не берёт ни один стиль (в `dayzwidgets.styles` нет секции ScrollWidget); без него `VScrollToPos01` не двигает содержимое | Выпадающий список не использует ScrollWidget: строки лежат во фрейме с обрезкой (`Draw.Clip`), листаются целыми строками, а невидимые строки скрываются |
| В Enforce нет тернарного оператора; имена вида `ref1080` ломают лексер; `%` внутри float-выражения не компилируется | Стиль кода фреймворка |

## 17. Структура проекта

```
mod_Intenso_UIFramework/
  readme.md, SPEC.md, dayz-mcp.toml (+ .local.toml)
  tools/                      генераторы, макеты, i18n/int_uif.json
  INT_UIFramework/            исходники аддона (PBO-префикс INT_UIFramework)
    config.cpp, $PBOPREFIX$, pbo.json, inputs.xml (Ctrl+F10), stringtable.csv
    gui/  imagesets/iuf_ui.imageset, styles/iuf.styles, textures/*.paa, layouts/*.layout (генерируются)
    scripts/3_Game/
      Core/        Constants, Log, Color, Scale, Widgets, LayoutTable (ген.), Draw (+Surface), Assets, Runtime, SelfTest
      Theme/       Theme, ThemeKeys, ThemePresets, ThemeManager, фасад INT_UIF
      Anim/        Ease, Tween, Animator
      Components/  Component (+Bag), Button, Display, List, Inputs, Dropdown, Tabs, Tooltip
      Overlays/    Menu (+Registry), Dialog (+DialogMenu), Toast
    scripts/5_Mission/
      INT_UIF_Mission.c       modded MissionGameplay: Init/Shutdown, реестр, Ctrl+F10, автотест
      INT_UIF_DemoMenu.c      витрина и эталон
  @INT_UIFramework/addons/    собранный PBO
```
