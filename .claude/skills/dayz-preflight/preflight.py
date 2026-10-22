"""DayZ preflight checks.

Verify the DayZ modding environment before any DayZ work. Hard-fails if P:\\ is
not mounted; warns on Tools / vanilla data / workshop folder / game version.

Path resolution order (DayZ Tools, game, server, vanilla data):
    1. Environment variable override
    2. Windows registry (DayZ Tools: Bohemia key + Steam uninstall key;
       game / server: Steam uninstall key "Steam App <appid>")
    3. Sibling folders in the same steamapps\\common as a found install
    4. Hard-coded common-default fallbacks

Version probe (skills target DayZ 1.30 stable, 1.30.164200):
    - P:\\scripts holds 1.30 vanilla scripts: 3_game\\vehicles\\motorbike.c exists
      and 3_game\\global\\game.c declares GAME_STORAGE_VERSION = 144 (142 on 1.29)
    - P:\\scripts.txt version= is the target Scripts Rev (127662 = 1.30.164200); another
      value means P:\\ was extracted from another build
    - DayZ_x64.exe / DayZDiag_x64.exe / DayZServer_x64.exe FileVersion is 1.30.x
      (read with ctypes + version.dll; no pywin32, no subprocess)

Run:
    python <claude-home>/skills/dayz-preflight/preflight.py
"""
from __future__ import annotations

import os
import re
import sys
from pathlib import Path
from typing import Optional

OK = "[OK]   "
WARN = "[WARN] "
FAIL = "[FAIL] "

# Version the skills are written and verified against.
TARGET_MAJOR_MINOR = (1, 30)
TARGET_BUILD = "1.30.164200"  # FileVersion 1.30.164.200, ProductVersion 1.30.164200.56
TARGET_SCRIPTS_REV = 127662  # P:\scripts.txt "version=" of the unpacked 1.30.164200 scripts.pbo
KNOWN_SCRIPTS_REVS = {127250: "1.30.164078", 127662: "1.30.164200"}
TARGET_STORAGE_VERSION = 144  # P:\scripts\3_game\global\game.c:5 on 1.30.164200 (142 on 1.29)

# Steam app ids (used for the "Steam App <id>" uninstall registry key).
STEAM_APP_DAYZ = 221100
STEAM_APP_DAYZ_SERVER = 223350
STEAM_APP_DAYZ_TOOLS = 830640


def _read_registry_string(hive, subkey: str, value_name: str) -> Optional[str]:
    """Read a REG_SZ value from the Windows registry. Returns None if unavailable."""
    if sys.platform != "win32":
        return None
    try:
        import winreg
    except ImportError:
        return None
    try:
        with winreg.OpenKey(hive, subkey) as key:
            val, _ = winreg.QueryValueEx(key, value_name)
            return str(val) if val else None
    except OSError:
        return None


def _steam_app_install(appid: int) -> Optional[Path]:
    """InstallLocation of a Steam app from its uninstall registry key, or None.

    Steam writes HKLM\\...\\Uninstall\\Steam App <appid> with InstallLocation for
    installs in any Steam library (the value may be empty, e.g. DayZ Server).
    """
    if sys.platform != "win32":
        return None
    try:
        import winreg
    except ImportError:
        return None
    for subkey in (
        rf"SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\Steam App {appid}",
        rf"SOFTWARE\WOW6432Node\Microsoft\Windows\CurrentVersion\Uninstall\Steam App {appid}",
    ):
        val = _read_registry_string(winreg.HKEY_LOCAL_MACHINE, subkey, "InstallLocation")
        if val:
            return Path(val)
    return None


def find_dayz_tools() -> Optional[Path]:
    """Locate the DayZ Tools install root.

    Resolution order:
      1. $DAYZ_TOOLS_PATH env var (explicit override)
      2. Windows registry (HKLM/HKCU under Bohemia Interactive\\DayZ Tools)
      3. Common Steam install paths

    Returns the install root (parent of Bin/) if AddonBuilder.exe is found, else None.
    """
    candidates: list[Path] = []

    env_override = os.environ.get("DAYZ_TOOLS_PATH")
    if env_override:
        candidates.append(Path(env_override))

    if sys.platform == "win32":
        try:
            import winreg
        except ImportError:
            winreg = None  # type: ignore[assignment]
        if winreg is not None:
            registry_locations = [
                (winreg.HKEY_LOCAL_MACHINE, r"SOFTWARE\WOW6432Node\Bohemia Interactive\DayZ Tools"),
                (winreg.HKEY_LOCAL_MACHINE, r"SOFTWARE\Bohemia Interactive\DayZ Tools"),
                (winreg.HKEY_CURRENT_USER, r"SOFTWARE\Bohemia Interactive\DayZ Tools"),
            ]
            for hive, subkey in registry_locations:
                val = _read_registry_string(hive, subkey, "Path")
                if val:
                    candidates.append(Path(val))
                    break

    steam_tools = _steam_app_install(STEAM_APP_DAYZ_TOOLS)
    if steam_tools is not None:
        candidates.append(steam_tools)

    candidates.extend(
        [
            Path(r"C:\Program Files (x86)\Steam\steamapps\common\DayZ Tools"),
            Path(r"C:\Program Files\Steam\steamapps\common\DayZ Tools"),
        ]
    )

    for root in candidates:
        if (root / "Bin" / "AddonBuilder" / "AddonBuilder.exe").exists():
            return root
    return None


def find_dayz_game() -> Optional[Path]:
    """Locate the DayZ game install root containing DayZ_x64.exe.

    Resolution order:
      1. $DAYZ_GAME_PATH env var
      2. Steam uninstall registry key (Steam App 221100)
      3. Sibling of the DayZ Tools install (<steamapps>\\common\\DayZ)
      4. Common Steam install paths
    """
    candidates: list[Path] = []

    env_override = os.environ.get("DAYZ_GAME_PATH")
    if env_override:
        candidates.append(Path(env_override))

    steam_game = _steam_app_install(STEAM_APP_DAYZ)
    if steam_game is not None:
        candidates.append(steam_game)

    tools_root = find_dayz_tools()
    if tools_root is not None:
        candidates.append(tools_root.parent / "DayZ")

    candidates.extend(
        [
            Path(r"C:\Program Files (x86)\Steam\steamapps\common\DayZ"),
            Path(r"C:\Program Files\Steam\steamapps\common\DayZ"),
        ]
    )

    for root in candidates:
        if (root / "DayZ_x64.exe").exists():
            return root
    return None


def find_dayz_diag() -> Optional[Path]:
    """Locate DayZDiag_x64.exe — the diagnostic client required for mod development.

    Required because retail DayZ_x64.exe blocks past the loading screen with
    -filePatching enabled, and -filePatching is needed for live source iteration.
    DayZ Diag lives alongside the retail exe in the DayZ game install dir.

    Resolution order:
      1. $DAYZ_DIAG_PATH env var
      2. Inside the DayZ game install (DayZDiag_x64.exe next to DayZ_x64.exe)
      3. Common Steam install paths
    """
    candidates: list[Path] = []

    env_override = os.environ.get("DAYZ_DIAG_PATH")
    if env_override:
        candidates.append(Path(env_override))

    game_root = find_dayz_game()
    if game_root is not None:
        candidates.append(game_root / "DayZDiag_x64.exe")

    candidates.extend(
        [
            Path(r"C:\Program Files (x86)\Steam\steamapps\common\DayZ\DayZDiag_x64.exe"),
            Path(r"C:\Program Files\Steam\steamapps\common\DayZ\DayZDiag_x64.exe"),
        ]
    )

    for path in candidates:
        if path.exists():
            return path
    return None


def find_dayz_server() -> Optional[Path]:
    """Locate the DayZ Server install root containing DayZServer_x64.exe.

    DayZ Server is a separate free Steam download (appid 223350) — not bundled
    with the game.

    Resolution order:
      1. $DAYZ_SERVER_PATH env var
      2. Steam uninstall registry key (Steam App 223350; often has no InstallLocation)
      3. Sibling of the DayZ game install (<steamapps>\\common\\DayZServer)
      4. Common Steam install paths
    """
    candidates: list[Path] = []

    env_override = os.environ.get("DAYZ_SERVER_PATH")
    if env_override:
        candidates.append(Path(env_override))

    steam_server = _steam_app_install(STEAM_APP_DAYZ_SERVER)
    if steam_server is not None:
        candidates.append(steam_server)

    game_root = find_dayz_game()
    if game_root is not None:
        candidates.append(game_root.parent / "DayZServer")

    candidates.extend(
        [
            Path(r"C:\Program Files (x86)\Steam\steamapps\common\DayZServer"),
            Path(r"C:\Program Files\Steam\steamapps\common\DayZServer"),
        ]
    )

    for root in candidates:
        if (root / "DayZServer_x64.exe").exists():
            return root
    return None


def find_vanilla_data() -> Optional[Path]:
    """Locate unpacked vanilla DayZ data on P:\\.

    Resolution order:
      1. $DAYZ_VANILLA_DATA_PATH env var
      2. Canonical names: P:\\dz, P:\\DZ, P:\\dta
    """
    candidates: list[Path] = []

    env_override = os.environ.get("DAYZ_VANILLA_DATA_PATH")
    if env_override:
        candidates.append(Path(env_override))

    candidates.extend([Path(r"P:\dz"), Path(r"P:\DZ"), Path(r"P:\dta")])

    for path in candidates:
        try:
            if path.exists() and any(path.iterdir()):
                return path
        except OSError:
            continue
    return None


def check_p_drive() -> bool:
    """P:\\ is mandatory. Hard fail if not mounted."""
    p = Path("P:\\")
    if p.exists() and p.is_dir():
        print(f"{OK} P:\\ is mounted")
        return True
    print(f"{FAIL} P:\\ is NOT mounted", file=sys.stderr)
    print(
        "        Open DayZ Tools and mount the P drive "
        "(Tools menu > Mount P drive),\n"
        "        or map it directly: "
        'subst P: "<dayz-projects>"',
        file=sys.stderr,
    )
    return False


def check_dayz_tools() -> None:
    """Look for AddonBuilder.exe via env / registry / Steam paths. Warn only."""
    root = find_dayz_tools()
    if root:
        print(f"{OK} DayZ Tools found: {root}")
        return
    print(f"{WARN} DayZ Tools not detected")
    print("        Tried: $DAYZ_TOOLS_PATH, Windows registry, common Steam paths.")
    print("        Set DAYZ_TOOLS_PATH if your install is elsewhere.")


def check_dayz_diag() -> None:
    """DayZDiag_x64.exe locatable via env / game install / Steam paths. Warn only.

    Required for -filePatching iteration: retail binaries block past the
    loading screen with -filePatching enabled.
    """
    path = find_dayz_diag()
    if path:
        print(f"{OK} DayZDiag_x64.exe found: {path}")
        return
    print(f"{WARN} DayZDiag_x64.exe not found")
    print("        Tried: $DAYZ_DIAG_PATH, DayZ game install, common Steam paths.")
    print("        Required for -filePatching iteration; set DAYZ_DIAG_PATH if elsewhere.")


def check_vanilla_data() -> None:
    """Vanilla DayZ data unpacked under P:\\. Warn only."""
    path = find_vanilla_data()
    if path:
        print(f"{OK} Vanilla data found: {path}")
        return
    print(f"{WARN} Vanilla DayZ data not detected on P:\\")
    print("        Tried: $DAYZ_VANILLA_DATA_PATH, P:\\dz, P:\\DZ, P:\\dta.")
    print("        Unpack vanilla PBOs via DayZ Tools 'Extract Game Data'.")


def validate_p_mods() -> Optional[str]:
    """Verify P:\\Mods\\ is a directory junction (or symlink) to the DayZ
    `!Workshop` folder, so built PBOs deployed under P:\\Mods\\@<Mod>\\Addons\\
    actually land where the engine can load them.

    Read-only: inspects state and returns a description of any problem.
    Never mutates the filesystem — preflight is for reporting, fixes belong
    to dedicated setup skills.

    Returns None on success, or a multi-line error message describing the issue
    and the exact fix command. Used by preflight (warn) and build-pbo (hard fail).
    """
    import os
    mods = Path(r"P:\Mods")
    game_root = find_dayz_game()
    workshop = game_root / "!Workshop" if game_root else None
    if workshop:
        fix_cmd = f'cmd /c mklink /J P:\\Mods "{workshop}"'
    else:
        fix_cmd = 'cmd /c mklink /J P:\\Mods "<DayZ install>\\!Workshop"'

    if not os.path.lexists(mods):
        return (
            "P:\\Mods\\ does not exist. It MUST be a junction to\n"
            "       <DayZ install>\\!Workshop\\ so built PBOs deploy where the\n"
            "       engine actually loads them.\n"
            f"       Fix: {fix_cmd}"
        )

    try:
        target_str = os.readlink(mods)
    except OSError:
        return (
            "P:\\Mods\\ exists as a regular folder, not a junction.\n"
            "       Built PBOs deployed under P:\\Mods\\@<Mod>\\Addons\\ will be\n"
            "       invisible to the engine.\n"
            "       Fix: remove the folder and create a junction:\n"
            "         cmd /c rmdir P:\\Mods\n"
            f"         {fix_cmd}"
        )

    if target_str.startswith("\\\\?\\"):
        target_str = target_str[4:]
    target_path = Path(target_str)
    if not target_path.exists():
        return (
            f"P:\\Mods\\ junction is dangling (target {target_path} is missing).\n"
            "       Fix: remove and recreate the junction with a valid target:\n"
            "         cmd /c rmdir P:\\Mods\n"
            f"         {fix_cmd}"
        )

    return None  # all good


def check_workshop_deploy() -> None:
    """P:\\Mods\\ must be a junction to the DayZ `!Workshop` folder. Warn only.

    Build skills should hard-fail on the same condition by calling validate_p_mods()
    directly.
    """
    err = validate_p_mods()
    if err is None:
        import os
        target = os.readlink(Path(r"P:\Mods"))
        if target.startswith("\\\\?\\"):
            target = target[4:]
        print(f"{OK} P:\\Mods -> {target}")
        return
    print(f"{WARN} {err}")


def read_exe_version(path: Path) -> Optional[tuple[tuple[int, int, int, int], Optional[str]]]:
    """Read an exe's fixed FileVersion (and its ProductVersion string) via version.dll.

    Pure ctypes: no pywin32, no subprocess. Returns ((a, b, c, d), product_version)
    or None when the version resource cannot be read (non-Windows, no resource).
    DayZ 1.30.164200 reads as ((1, 30, 164, 200), "1.30.164200.56").
    """
    if sys.platform != "win32":
        return None
    try:
        import ctypes
        from ctypes import wintypes
        ver = ctypes.WinDLL("version")
    except (ImportError, OSError):
        return None

    ver.GetFileVersionInfoSizeW.argtypes = [wintypes.LPCWSTR, ctypes.POINTER(wintypes.DWORD)]
    ver.GetFileVersionInfoSizeW.restype = wintypes.DWORD
    ver.GetFileVersionInfoW.argtypes = [wintypes.LPCWSTR, wintypes.DWORD, wintypes.DWORD, ctypes.c_void_p]
    ver.GetFileVersionInfoW.restype = wintypes.BOOL
    ver.VerQueryValueW.argtypes = [
        ctypes.c_void_p, wintypes.LPCWSTR, ctypes.POINTER(ctypes.c_void_p), ctypes.POINTER(wintypes.UINT)
    ]
    ver.VerQueryValueW.restype = wintypes.BOOL

    size = ver.GetFileVersionInfoSizeW(str(path), None)
    if not size:
        return None
    buf = ctypes.create_string_buffer(size)
    if not ver.GetFileVersionInfoW(str(path), 0, size, buf):
        return None

    ptr = ctypes.c_void_p()
    length = wintypes.UINT()
    if not ver.VerQueryValueW(buf, "\\", ctypes.byref(ptr), ctypes.byref(length)) or not length.value:
        return None
    # VS_FIXEDFILEINFO: dwSignature, dwStrucVersion, dwFileVersionMS, dwFileVersionLS, ...
    fixed = ctypes.cast(ptr, ctypes.POINTER(wintypes.DWORD * 4)).contents
    ms, ls = fixed[2], fixed[3]
    file_version = (ms >> 16, ms & 0xFFFF, ls >> 16, ls & 0xFFFF)

    product: Optional[str] = None
    if ver.VerQueryValueW(buf, "\\VarFileInfo\\Translation", ctypes.byref(ptr), ctypes.byref(length)) and length.value >= 4:
        lang_cp = ctypes.cast(ptr, ctypes.POINTER(wintypes.WORD * 2)).contents
        key = f"\\StringFileInfo\\{lang_cp[0]:04x}{lang_cp[1]:04x}\\ProductVersion"
        if ver.VerQueryValueW(buf, key, ctypes.byref(ptr), ctypes.byref(length)) and length.value:
            product = ctypes.wstring_at(ptr.value, length.value).rstrip("\x00") or None
    return file_version, product


def check_vanilla_scripts_version() -> None:
    """P:\\scripts must hold DayZ 1.30 vanilla scripts. Warn only.

    1.30 markers: 3_game\\vehicles\\motorbike.c exists (new in 1.30) and
    3_game\\global\\game.c declares GAME_STORAGE_VERSION = 144 (142 on 1.29).
    """
    scripts = Path(r"P:\scripts")
    game_c = scripts / "3_game" / "global" / "game.c"
    motorbike_c = scripts / "3_game" / "vehicles" / "motorbike.c"

    if not game_c.exists():
        print(f"{WARN} Vanilla scripts not found ({game_c} missing)")
        print("        Skills cite P:\\scripts (1.30) as ground truth. Unpack scripts.pbo")
        print("        via DayZ Tools 'Extract Game Data'.")
        return

    storage: Optional[int] = None
    try:
        match = re.search(r"GAME_STORAGE_VERSION\s*=\s*(\d+)", game_c.read_text(encoding="utf-8", errors="replace"))
        if match:
            storage = int(match.group(1))
    except OSError:
        storage = None
    has_motorbike = motorbike_c.exists()

    if storage is None:
        print(f"{WARN} Could not read GAME_STORAGE_VERSION from {game_c}")
        return
    if storage == TARGET_STORAGE_VERSION and has_motorbike:
        print(f"{OK} P:\\scripts is DayZ 1.30 vanilla (GAME_STORAGE_VERSION = {storage}, motorbike.c present)")
        return
    if storage < TARGET_STORAGE_VERSION or not has_motorbike:
        print(f"{WARN} P:\\ holds pre-1.30 vanilla data; skills target 1.30 - re-extract")
        print(f"        GAME_STORAGE_VERSION = {storage} (1.30 = {TARGET_STORAGE_VERSION}, 1.29 = 142), "
              f"motorbike.c {'present' if has_motorbike else 'missing'}.")
        print("        Re-extract with DayZ Tools 'Extract Game Data' from a 1.30 install.")
        return
    print(f"{WARN} P:\\ holds vanilla data newer than 1.30 (GAME_STORAGE_VERSION = {storage})")
    print(f"        Skills were verified on {TARGET_BUILD}; grep P:\\scripts before trusting an API claim.")


def check_scripts_revision() -> None:
    """P:\\scripts.txt `version=` must be the Scripts Rev of the target build. Warn only.

    The skills' vanilla line refs are numbered against the target build's scripts; a P:\\ extracted
    from another build (an older stable, Experimental) shifts them.
    """
    scripts_txt = Path(r"P:\scripts.txt")
    if not scripts_txt.exists():
        print(f"{WARN} {scripts_txt} not found - cannot tell which build P:\\scripts was extracted from")
        print("        Re-extract scripts.pbo with DayZ Tools 'Extract Game Data' (it writes scripts.txt).")
        return
    rev: Optional[int] = None
    try:
        match = re.search(r"^\s*version\s*=\s*(\d+)", scripts_txt.read_text(encoding="utf-8", errors="replace"),
                          re.MULTILINE)
        if match:
            rev = int(match.group(1))
    except OSError:
        rev = None
    if rev is None:
        print(f"{WARN} No 'version=' line in {scripts_txt}")
        return
    if rev == TARGET_SCRIPTS_REV:
        print(f"{OK} P:\\scripts.txt version={rev} (Scripts Rev of {TARGET_BUILD})")
        return
    known = KNOWN_SCRIPTS_REVS.get(rev)
    print(f"{WARN} P:\\scripts.txt version={rev}, expected {TARGET_SCRIPTS_REV} ({TARGET_BUILD}): "
          f"P:\\ extracted from another build{f' ({known})' if known else ''}")
    print("        Vanilla line refs in the skills are numbered for the target build; re-extract scripts.pbo")
    print("        from the installed game, or grep P:\\scripts before trusting a line number.")


def check_game_versions() -> None:
    """DayZ_x64.exe / DayZDiag_x64.exe / DayZServer_x64.exe FileVersion is 1.30.x. Warn only."""
    game_root = find_dayz_game()
    server_root = find_dayz_server()
    exes = [
        ("DayZ_x64.exe", game_root / "DayZ_x64.exe" if game_root else None),
        ("DayZDiag_x64.exe", find_dayz_diag()),
        ("DayZServer_x64.exe", server_root / "DayZServer_x64.exe" if server_root else None),
    ]
    if game_root is None:
        print(f"{WARN} DayZ_x64.exe not found - game version not checked")
        print("        Tried: $DAYZ_GAME_PATH, Steam registry (App 221100), DayZ Tools sibling, common Steam paths.")

    for name, path in exes:
        if path is None or not path.exists():
            continue  # DayZDiag already warned above; the server is optional for preflight
        info = read_exe_version(path)
        if info is None:
            print(f"{WARN} Could not read the FileVersion of {path}")
            continue
        (a, b, c, d), product = info
        file_version = f"{a}.{b}.{c}.{d:03d}"
        build = f"{a}.{b}.{c}{d:03d}"
        shown = f"{file_version}" + (f" (product {product})" if product else "")
        if (a, b) == TARGET_MAJOR_MINOR:
            note = "" if build == TARGET_BUILD else f" - skills verified on {TARGET_BUILD}"
            print(f"{OK} {name} {shown}{note}")
        else:
            print(f"{WARN} {name} is {shown}, not 1.30.x - skills target DayZ 1.30 ({TARGET_BUILD})")
            print(f"        {path}")


def main() -> int:
    print("DayZ preflight\n", flush=True)
    if not check_p_drive():
        return 1
    check_dayz_tools()
    check_dayz_diag()
    check_vanilla_data()
    check_workshop_deploy()
    check_vanilla_scripts_version()
    check_scripts_revision()
    check_game_versions()
    print("\nPreflight complete.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
