"""Скриншоты макета headless-Chrome: python -I shots.py <папка макета> <папка снимков> st:m [st:m ...]"""
import os, subprocess, sys, urllib.parse, pathlib
CHROME = r'C:\Program Files\Google\Chrome\Application\chrome.exe'
mock, out = sys.argv[1], sys.argv[2]
os.makedirs(out, exist_ok=True)
url0 = pathlib.Path(mock, 'index.html').resolve().as_uri()
for spec in sys.argv[3:]:
    st, m, *cat = spec.split(':')
    png = os.path.join(out, f'{st}_{m}{"_" + cat[0] if cat else ""}.png')
    url = f'{url0}?st={st}&m={m}&noanim=1' + (f'&cat={cat[0]}' if cat else '')
    p = subprocess.run([CHROME, '--headless=new', '--disable-gpu', '--hide-scrollbars', '--force-device-scale-factor=1',
                        '--window-size=1920,1080', '--virtual-time-budget=4000', '--enable-logging=stderr', '--v=0',
                        f'--screenshot={png}', url], capture_output=True, text=True, encoding='utf-8', errors='replace', timeout=90)
    errs = [l for l in p.stderr.splitlines() if 'CONSOLE' in l or 'Uncaught' in l]
    print(spec, 'ok' if os.path.exists(png) else 'NO PNG', *errs[:3], sep='\n  ')
