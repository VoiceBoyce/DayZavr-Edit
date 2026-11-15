# -*- coding: utf-8 -*-
"""Чтение .rvmat (бинарный raP или текст) в dict: значения верхнего уровня и классы Stage*/StageTI."""
import re
import struct


def _cint(d, p):
    v = d[p]
    p += 1
    if v & 0x80:
        v += (d[p] - 1) * 0x80
        p += 1
    return v, p


def _asciiz(d, p):
    e = d.index(b"\0", p)
    return d[p:e].decode("latin-1"), e + 1


def _array(d, p):
    n, p = _cint(d, p)
    out = []
    for _ in range(n):
        t = d[p]
        p += 1
        if t == 0:
            s, p = _asciiz(d, p)
            out.append(s)
        elif t == 1:
            out.append(struct.unpack_from("<f", d, p)[0]); p += 4
        elif t == 2:
            out.append(struct.unpack_from("<i", d, p)[0]); p += 4
        elif t == 3:
            a, p = _array(d, p)
            out.append(a)
        else:
            s, p = _asciiz(d, p)
            out.append(s)
    return out, p


def _body(d, p):
    _, p = _asciiz(d, p)                         # родитель
    n, p = _cint(d, p)
    res = {}
    for _ in range(n):
        t = d[p]
        p += 1
        if t == 0:
            name, p = _asciiz(d, p)
            off = struct.unpack_from("<I", d, p)[0]; p += 4
            res[name] = _body(d, off)[0]
        elif t == 1:
            st = d[p]; p += 1
            name, p = _asciiz(d, p)
            if st == 0:
                v, p = _asciiz(d, p)
            elif st == 1:
                v = struct.unpack_from("<f", d, p)[0]; p += 4
            else:
                v = struct.unpack_from("<i", d, p)[0]; p += 4
            res[name] = v
        elif t == 2:
            name, p = _asciiz(d, p)
            res[name], p = _array(d, p)
        elif t in (3, 4):
            _, p = _asciiz(d, p)
        elif t == 5:
            p += 4
            name, p = _asciiz(d, p)
            res[name], p = _array(d, p)
    return res, p


def _text(s):
    """Упрощённый разбор текстового rvmat: верхние значения и class StageN { texture = ...; }."""
    s = re.sub(r"//[^\n]*", "", s)
    res = {}
    for m in re.finditer(r"class\s+(\w+)\s*\{(.*?)\};", s, re.S):
        tex = re.search(r'texture\s*=\s*"([^"]*)"', m.group(2))
        res[m.group(1)] = {"texture": tex.group(1)} if tex else {}
    top = re.sub(r"class\s+\w+\s*\{.*?\};", "", s, flags=re.S)
    for m in re.finditer(r"(\w+)\[\]\s*=\s*\{([^}]*)\}", top):
        vals = []
        for x in m.group(2).split(","):
            try:
                vals.append(float(x))
            except ValueError:
                pass
        res[m.group(1)] = vals
    for m in re.finditer(r"(\w+)\s*=\s*([^;\n]+);", top):
        v = m.group(2).strip().strip('"')
        try:
            v = float(v)
        except ValueError:
            pass
        res.setdefault(m.group(1), v)
    return res


def read_rvmat(path):
    d = open(path, "rb").read()
    if d[:4] == b"\0raP":
        return _body(d, 16)[0]
    return _text(d.decode("latin-1"))
