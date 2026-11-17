# python -I deobf.py <папка скриптов> <папка вывода> <config.cpp> [<config.cpp> ...]
# Снимает обфускацию KGB: вычисляет #ifdef/#ifndef по объединению defines из всех config.bin мода,
# выкидывает комментарии (в них только приманки). Настоящие defines движка оставляет как есть.
import sys, os, re

def load_defines(paths):
    # defines[] из всех классов CfgMods текстовых конфигов (DeRap config.bin -> config.cpp)
    found = set()
    for p in paths:
        t = open(p, encoding='utf-8', errors='replace').read()
        for m in re.finditer(r'defines\[\]\s*=\s*\{([^}]*)\}', t):
            found.update(re.findall(r'"([^"]+)"', m.group(1)))
    return found

DEFINES = set()

REAL = {'SERVER', 'DEVELOPER', 'DIAG_DEVELOPER', 'DIAG', 'GAME_TEMPLATE', 'PLATFORM_WINDOWS', 'PLATFORM_CONSOLE',
        'PLATFORM_XBOX', 'PLATFORM_PS4', 'BUILD_EXPERIMENTAL', 'NO_GUI', 'NO_GUI_INGAME', 'ENABLE_LOGGING',
        'DAYZ_1_25', 'DAYZ_1_26', 'DAYZ_1_27', 'DAYZ_1_28', 'DAYZ_1_29', 'DAYZ_1_30', 'WORKBENCH',
        'EXPANSIONMODCORE', 'EXPANSIONMODAI', 'JM_COT', 'VPPADMINTOOLS', 'CF_MODULES'}

RE_COND = re.compile(r'#(ifdef|ifndef|define|undef)\b[ \t]*([A-Za-z0-9_]*)')
RE_PLAIN = re.compile(r'#(else|endif|include)\b')

def process(text, defines, stats):
    out = []
    stack = []  # (kind, parent_active, cond)
    active = True
    i, n = 0, len(text)
    while i < n:
        c = text[i]
        if c == '"':
            j = i + 1
            while j < n and text[j] != '"':
                if text[j] == '\\':
                    j += 1
                j += 1
            if active: out.append(text[i:j+1])
            i = j + 1; continue
        if text.startswith('/*', i):
            j = text.find('*/', i + 2)
            i = n if j < 0 else j + 2
            if active: out.append(' ')
            continue
        if text.startswith('//', i):
            j = text.find('\n', i)
            i = n if j < 0 else j
            continue
        if c == '#':
            m = RE_COND.match(text, i)
            if m:
                d, sym = m.group(1), m.group(2)
                i = m.end()
                if d in ('ifdef', 'ifndef'):
                    if sym in REAL:
                        stack.append(('keep', active, True))
                        if active: out.append('\n#%s %s\n' % (d, sym))
                        stats['kept'].add(sym)
                    else:
                        cond = (sym in defines) == (d == 'ifdef')
                        stack.append(('eval', active, cond))
                        active = active and cond
                elif d == 'define':
                    if active:
                        if sym in REAL: out.append('\n#define %s\n' % sym)
                        else: defines.add(sym); stats['defined'].add(sym)
                else:
                    if active: defines.discard(sym)
                continue
            m = RE_PLAIN.match(text, i)
            if m:
                d = m.group(1)
                i = m.end()
                if d == 'else':
                    kind, pa, cond = stack.pop()
                    stack.append((kind, pa, cond))
                    if kind == 'keep':
                        active = pa
                        if active: out.append('\n#else\n')
                    else:
                        active = pa and not cond
                elif d == 'endif':
                    kind, pa, cond = stack.pop()
                    active = pa
                    if kind == 'keep' and active: out.append('\n#endif\n')
                continue
        if active: out.append(c)
        i += 1
    if stack:
        stats['unbalanced'] += 1
    return ''.join(out)

def tidy(code):
    code = re.sub(r'[ \t]+', ' ', code)
    code = re.sub(r' *\n *', '\n', code)
    code = re.sub(r'\n{2,}', '\n', code)
    return code.strip()

src, dst = sys.argv[1], sys.argv[2]
DEFINES = load_defines(sys.argv[3:])
print('defines:', len(DEFINES))
os.makedirs(dst, exist_ok=True)
for name in sorted(os.listdir(src)):
    p = os.path.join(src, name)
    if not os.path.isfile(p) or '.c' not in name:
        continue
    raw = open(p, 'rb').read().decode('utf-8', errors='replace')
    stats = {'kept': set(), 'defined': set(), 'unbalanced': 0}
    code = tidy(process(raw, set(DEFINES), stats))
    safe = re.sub(r'[^A-Za-z0-9_.]', '_', name)
    if code:
        open(os.path.join(dst, safe + '.txt'), 'w', encoding='utf-8').write(code + '\n')
    print('%-40s raw=%7d code=%6d kept=%s defs=%s unbal=%d' % (safe[:40], len(raw), len(code), sorted(stats['kept']), sorted(stats['defined']), stats['unbalanced']))
