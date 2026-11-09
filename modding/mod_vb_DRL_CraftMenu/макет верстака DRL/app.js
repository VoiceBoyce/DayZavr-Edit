// Макет крафта DRL на трёх станциях (и свободное меню F9). Рецепты и категории — сервер 18 (data.js).
// Каркас общий: слева рецепты, справа очередь / обвесы / разбор; центр рисуется по типу станции.
const $ = (s, r = document) => r.querySelector(s);
const $$ = (s, r = document) => [...r.querySelectorAll(s)];
const D = window.DRL, ICN = window.ICONS;
const R = (id) => D.recipes[id];
const nm = (cls) => D.names[cls] || cls;
const fmt = (n) => Number(n).toLocaleString('ru-RU');
const hash = (s) => { let h = 7; for (const c of String(s)) h = (h * 31 + c.charCodeAt(0)) | 0; return Math.abs(h); };
const fmtSec = (s) => { if (s < 60) return `${s} с`; const m = Math.floor(s / 60), r = s % 60; return `${m} мин${r ? ` ${r} с` : ''}`; };
const clock = (s) => `${String(Math.floor(s / 60)).padStart(2, '0')}:${String(s % 60).padStart(2, '0')}`;
const I = {
    x: '<path d="M18 6 6 18M6 6l12 12"/>', check: '<path d="m5 12 5 5 9-10"/>', clock: '<circle cx="12" cy="12" r="9"/><path d="M12 7v5l3 2"/>',
    lock: '<rect x="5" y="11" width="14" height="10" rx="2"/><path d="M8 11V7a4 4 0 0 1 8 0v4"/>', search: '<circle cx="11" cy="11" r="7"/><path d="m20 20-4-4"/>',
    alert: '<circle cx="12" cy="12" r="9"/><path d="M12 7v5M12 16h.01"/>', arrow: '<path d="M5 12h14M13 6l6 6-6 6"/>', swap: '<path d="M7 4 3 8l4 4M3 8h14M17 20l4-4-4-4M21 16H7"/>',
    shield: '<path d="M12 3l8 3v6c0 5-3.5 8-8 9-4.5-1-8-4-8-9V6l8-3z"/>', book: '<path d="M4 5a2 2 0 0 1 2-2h13v16H6a2 2 0 0 0-2 2zM4 19V5M8 7h7"/>',
    refresh: '<path d="M20 12a8 8 0 1 1-2.3-5.7L20 8M20 3v5h-5"/>', hammer: '<path d="M14 4l6 6-3 3-6-6zM11 7l-8 8 3 3 8-8"/>', rot: '<path d="M21 12a9 9 0 1 1-3-6.7L21 8M21 3v5h-5"/>',
    plus: '<path d="M12 5v14M5 12h14"/>', minus: '<path d="M5 12h14"/>', layers: '<path d="M12 3 2 8l10 5 10-5zM2 13l10 5 10-5"/>',
};
const svg = (n, s = 18) => `<svg class="i" viewBox="0 0 24 24" style="width:${s}px;height:${s}px">${I[n] || ''}</svg>`;
const ic = (name, s = 24) => `<i class="ic" style="--m:url(${ICN[name] || ICN['drl_cog']});width:${s}px;height:${s}px"></i>`;
const icFor = (cls) => { for (const r of Object.values(D.recipes)) { if (r.res === cls) return r.ic; const c = r.comps.find((x) => x.cls === cls); if (c) return c.ic; } return D.icons[cls] || 'drl_cog'; };

// ---------- станции (слоты — из config.cpp, состояние обвесов — пример) ----------
const ST = {
    wb: { name: 'Верстак', small: 'Станция игрока', icon: 'drl_claw-hammer', cls: 'sheet bp', verb: 'Скрафтить', doc: 'ЧЕРТЁЖ', cat: 'Оружие', sel: 400,
        slots: [['Тиски', 'DRL_Craft_Vise', 78], ['Наковальня', 'DRL_Craft_Anvil', 64, ['DRL_Craft_Anvil2']], ['Сверл. станок', 'DRL_Craft_DrillSmal', null], ['Штатив', 'DRL_Craft_lab_bunsen', 100],
            ['Инструменты', 'DRL_Craft_Tools', 92], ['Микроскоп', 'DRL_Craft_Microscope', 55], ['Швейный стол', 'DRL_Craft_Sewing_Table', 100], ['Ящик', 'DRL_Craft_Box', 100]],
        missing: 'DRL_Craft_Tools' },
    sew: { name: 'Швейная машинка', small: 'На швейном столе верстака', icon: 'drl_sewing-machine', cls: 'sheet sew', verb: 'Сшить', doc: 'ВЫКРОЙКА', cat: 'Спецназ', sel: null, selRes: 'Vest',
        slots: [['Ножницы', 'DRL_Craft_Sewing_scissors', 88], ['Шпуля', 'DRL_Craft_Metalic_thimble', 70]], missing: 'DRL_Craft_Sewing_scissors' },
    lift: { name: 'Подъёмник', small: 'Сборка транспорта', icon: 'drl_tire-iron', cls: 'sheet lift', verb: 'Собрать', doc: 'СБОРКА', cat: 'Interceptor', sel: 4001,
        slots: [['Домкрат', 'DRL_Craft_Car_Jack', 81], ['Ящик', 'DRL_Craft_Box', 100]], missing: 'DRL_Craft_Car_Jack' },
    free: { name: 'Свободный крафт', small: 'Меню F9 — без станции', icon: 'drl_scroll-unfurled', cls: 'sheet bp', verb: 'Скрафтить', doc: 'ЧЕРТЁЖ', cat: 'Оружие', sel: 400, slots: null },
};
const SETPIC = { 'Король': 'set_cod2', 'Демон': 'set_demon', 'Мориарти': 'set_elegant', 'Призрак': 'set_ghost', 'Штурмовик': 'set_heavy', 'Экзо': 'set_incite', 'Легион': 'set_legion',
    'Спецназ': 'set_military', 'Самурай': 'set_oni', 'Оператор': 'set_operator', 'Райдер': 'set_rider', 'Альфа': 'setcomplete_eod_specialist' };
// варианты ингредиента для состояния «Вариант ингредиента» (Classnames + EnableVariationSelection)
const MOCK_VARS = { wb: ['OOTS_ART_Gravi', ['OOTS_ART_Gravi', 'OOTS_ART_Drop', 'OOTS_ART_Crustal']], free: ['OOTS_ART_Gravi', ['OOTS_ART_Gravi', 'OOTS_ART_Drop', 'OOTS_ART_Crustal']],
    sew: ['EFT_Cloth_cordura', ['EFT_Cloth_cordura', 'EFT_Cloth_ripstop', 'EFT_Cloth_fleece']], lift: ['EFT_Wires', ['EFT_Wires', 'EFT_PowerCord', 'CableReel']] };

const S = { st: 'wb', mode: 'ok', cat: 0, sel: 400, qty: 1, q: '', only: false, vsel: {}, parts: {}, modal: null };
const st = () => ST[S.st];
const recipe = () => R(S.sel);

// ---------- инвентарь-пример ----------
// «Не хватает»: у выбранного рецепта — первые два расходника, у остальных — по хешу
const lacking = (cls) => S.mode === 'lack' && (hash(cls) % 3 === 0 || R(S.sel).comps.filter((c) => c.k === 'c').slice(0, 2).some((c) => c.cls === cls));
const have = (cls, need) => (lacking(cls) ? Math.floor(need * 0.4) : need * (2 + (hash(cls) % 3)) + (hash(cls) % 4));
const slotOf = (cls) => (st().slots || []).find((s) => s[1] === cls || (s[3] || []).includes(cls));
const slotHp = (s) => (S.mode === 'notool' && s[1] === st().missing ? null : s[2]);
const toolHp = (cls) => 35 + (hash(cls) % 66);
// у предмета нет своего слота (респиратор, перчатки…): резолвер ищет его по всему инвентарю станции (ящик и т.п.)
const inStore = (cls) => !!st().slots && !slotOf(cls);
const installed = (cls) => {
    if (!st().slots) return null;
    const s = slotOf(cls);
    if (!s) return toolHp(cls);
    return slotHp(s) != null ? slotHp(s) : null;
};
const consumables = (r) => r.comps.filter((c) => c.k === 'c');
const missingAtt = (r) => r.comps.find((c) => c.k === 'a' && installed(c.cls) == null);
const isLocked = (r) => (S.mode === 'locked' || S.modal === 'learn') && r.id === S.sel ? true : r.id !== S.sel && hash(r.id) % 13 === 0;
const maxOf = (r, q = 1) => {
    if (missingAtt(r) || isLocked(r)) return 0;
    let m = 99;
    consumables(r).forEach((c) => { m = Math.min(m, Math.floor(have(c.cls, c.n) / c.n)); });
    return m;
};
const listStatus = (r) => {
    if (QUEUE().some((x) => x.id === r.id)) return ['q', svg('clock', 12), 'В очереди крафта'];
    if (isLocked(r)) return ['lk', svg('lock', 12), 'Не изучен'];
    const ma = missingAtt(r);
    if (ma) return ['att', ic(ma.ic, 12), `${S.st === 'free' ? 'Только на станции' : 'Нужен обвес'}: ${nm(ma.cls)}`];
    // для разнообразия списка часть чужих рецептов «не хватает»; иначе — тот же максимум, что у кнопки
    const lack = r.id !== S.sel && hash(r.id) % 4 === 0 ? consumables(r).slice(0, 1) : consumables(r).filter((c) => have(c.cls, c.n) < c.n);
    if (lack.length) return ['no', svg('alert', 12), `Не хватает: ${nm(lack[0].cls)}${lack.length > 1 ? ` +${lack.length - 1}` : ''}`];
    return ['ok', svg('check', 12), `Можно скрафтить: ${maxOf(r)}`];
};

// ---------- очередь (MaxQueueSlots сервера = 2) ----------
const QUEUE = () => {
    const ids = Object.keys(D.recipes).map(Number).filter((id) => R(id).st === (S.st === 'free' ? 'wb' : S.st) && id !== S.sel);
    if (S.mode === 'run') return [{ id: S.sel, left: 9, total: 15 * S.qty, run: 1 }, { id: ids[3], run: 0 }];
    if (S.mode === 'full') return [{ id: ids[1], left: 41, total: 60, run: 1 }, { id: ids[5], run: 0 }];
    return [];
};

// ---------- варианты (цвета) ----------
const COLORS = { black: ['Чёрный', '#1d1f22'], white: ['Белый', '#e8e8e8'], silver: ['Серебро', '#b9c0c8'], gold: ['Золото', '#d9a92e'], red: ['Красный', '#c73a32'], blue: ['Синий', '#2f62c9'],
    yellow: ['Жёлтый', '#e7c021'], green: ['Зелёный', '#3f8a3a'], pink: ['Розовый', '#e06aa8'], purple: ['Фиолетовый', '#7b46c9'], olive: ['Олива', '#6b6e3a'], tan: ['Песок', '#c2a57a'],
    grey: ['Серый', '#7d8389'], gray: ['Серый', '#7d8389'], orange: ['Оранжевый', '#e07a26'], cosmos: ['Космос', '#4b2f9a'], forest: ['Лес', '#3f6a2c'], mix: ['Микс', '#8a6a3a'],
    old: ['Старый', '#6e5a44'], away: ['выездной', null], camo: ['Камуфляж', '#5d6b45'], multicam: ['Мультикам', '#8c7d55'], rust: ['Ржавый', '#8a4b2a'],
    desert: ['Пустыня', '#c9ad7f'], pixel: ['пиксель', 'repeating-conic-gradient(#23272a 0 25%,#4b5446 0 50%) 0 0/8px 8px'], no_tail: ['Без хвоста', null], tail: ['С хвостом', null] };
// имя и образец цвета варианта — по хвосту класса после общей части (Black_Pink, _blue, Desert…)
const varInfo = (r) => {
    const vs = r.vars.length ? r.vars : [r.res];
    let p = vs[0];
    vs.forEach((v) => { while (!v.startsWith(p)) p = p.slice(0, -1); });
    return vs.map((v) => {
        const tail = v.slice(p.length).replace(/(^|_)vb$/i, '').toLowerCase().replace('no_tail', 'no-tail');
        const toks = tail.split('_').filter(Boolean).map((t) => t.replace('no-tail', 'no_tail'));
        const parts = toks.map((t) => COLORS[t]).filter(Boolean);
        const cols = parts.map((x) => x[1]).filter(Boolean);
        const name = parts.length ? parts.map((x, i) => (i ? x[0].toLowerCase() : x[0])).join(' / ') : toks.length ? toks.join(' ') : 'Стандарт';
        const pat = cols.find((c) => c.includes('gradient'));
        const bg = pat || (cols.length > 1 ? `linear-gradient(135deg,${cols[0]} 50%,${cols[1]} 50%)` : cols[0] || '#9aa4ad');
        return { cls: v, name, bg };
    });
};

// ---------- каркас ----------
function skeleton() {
    return `<div class="shade"></div><div class="win st-${S.st}" id="win">
        <div class="col left" id="left"></div><div id="sheet"></div><div class="col right" id="right"></div></div>
        <div id="modal"></div><div id="toast"></div>`;
}

// ---------- слева: рецепты ----------
const cats = () => D.cats[S.st];
const shown = () => {
    const q = S.q.trim().toLowerCase(), c = cats()[S.cat];
    return c.ids.map(R).filter((r) => (!q || r.name.toLowerCase().includes(q)) && (!S.only || listStatus(r)[0] === 'ok'));
};
function leftHtml() {
    const s = st(), cs = cats(), car = S.st === 'lift', xs = shown(), ok = xs.filter((r) => listStatus(r)[0] === 'ok').length;
    return `<div class="h"><span class="h__ic">${ic(s.icon, 24)}</span><div class="h__t"><small>${s.small} · Survivor</small><b>${s.name}</b></div></div>
        <label class="search">${svg('search', 15)}<input id="q" placeholder="Найти рецепт" spellcheck="false" value="${S.q}"></label>
        <div class="cats${car ? ' cars' : ''}"${car ? '' : ` style="grid-template-columns:repeat(${Math.min(cs.length, 8)},minmax(0,1fr))"`}>${cs.map((c, i) => `<div class="cat${i === S.cat ? ' on' : ''}" data-a="cat" data-v="${i}" title="${c.name} — ${c.ids.length}">${ic(c.ic, car ? 20 : 20)}${car ? `<span>${c.all ? 'Все' : c.name}</span>` : ''}<em>${c.ids.length}</em></div>`).join('')}</div>
        <div class="sec"><b>${cs[S.cat].name}</b><span>можно: ${ok} из ${xs.length}</span></div>
        <div class="only${S.only ? ' on' : ''}" data-a="only"><span class="sw"></span>Только доступные к крафту</div>
        <div class="list" id="list">${xs.map((r) => { const [k, i, t] = listStatus(r); return `<div class="it${r.id === S.sel ? ' on' : ''}${k === 'lk' ? ' lock' : ''}" data-a="pick" data-v="${r.id}">
            <div class="it__pic">${ic(r.ic, 30)}</div><div class="it__t"><b>${r.name}</b><div class="stt ${k}">${i}${t}</div></div>
            <div class="it__tm">${fmtSec(r.t)}${r.cnt > 1 ? `<br>×${r.cnt}` : ''}</div></div>`; }).join('') || '<div class="note">Ничего не нашлось</div>'}</div>`;
}

// ---------- кнопка крафта и панель количества ----------
function craftState(r) {
    const q = QUEUE(), mine = q.find((x) => x.id === r.id), ma = missingAtt(r), lack = consumables(r).some((c) => have(c.cls, c.n * S.qty) < c.n * S.qty);
    if (isLocked(r)) return { off: 1, html: 'Рецепт не изучен' };
    if (mine && mine.run) { const p = Math.round((1 - mine.left / mine.total) * 100); return { cls: 'cancel', html: `<span class="gl">${svg('x', 15)}Отменить</span><small>идёт крафт — ${p}%, ещё ${clock(mine.left)}</small><i style="width:${p}%"></i>` }; }
    if (mine) return { cls: 'wait', html: `${svg('clock', 16)}В очереди — отменить` };
    if (ma) return { off: 1, html: S.st === 'free' ? `Только на станции:<br>${nm(ma.cls)}` : `Установите обвес:<br>${nm(ma.cls)}` };
    if (lack) return { off: 1, html: 'Не хватает предметов' };
    if (q.length >= D.maxQueue) return { off: 1, html: `Очередь заполнена (${q.length}/${D.maxQueue})` };
    if (q.length) return { cls: 'queue', html: `${svg('plus', 17)}В очередь (${q.length + 1}/${D.maxQueue})` };
    return { html: `${ic(st().icon, 20)}${st().verb}` };
}
function barHtml(r, extraTime = 0, verbOut = '') {
    const s = craftState(r), max = Math.max(1, maxOf(r)), q = S.qty, lock = isLocked(r);
    if (lock) return `<div class="barw"><div class="bar"><div class="tot" style="text-align:left"><small>Рецепт закрыт</small><b style="justify-content:flex-start">Изучите его в окне «Изучение»</b></div>
        <div class="go gold" data-a="learn">${svg('book', 18)}Изучить</div></div></div>`;
    return `<div class="barw"><div class="bar"><div class="stp"><span class="${q <= 1 ? 'off' : ''}" data-a="qty" data-v="-1">−</span><input value="${q}" readonly><span class="${q >= max ? 'off' : ''}" data-a="qty" data-v="1">+</span></div>
        <div class="chips">${[1, 5, 10].map((n) => `<span class="${q === n ? 'on' : ''}${n > max ? ' off' : ''}" data-a="set" data-v="${n}">×${n}</span>`).join('')}<span class="${q === max ? 'on' : ''}" data-a="set" data-v="${max}">Макс ${max}</span></div>
        <div class="tot"><small>Получите / время</small><b>${verbOut || `×${fmt(r.cnt * q)}`} ${svg('clock', 14)}${fmtSec((r.t + extraTime) * q)}</b></div>
        <div class="go${s.off ? ' off' : ''}${s.cls ? ` ${s.cls}` : ''}" data-a="craft">${s.html}</div></div></div>`;
}
function headHtml(r, stampExtra) {
    const vi = varInfo(r), sel = vi[S.vsel[r.id] || 0];
    const catName = cats()[S.cat].all ? '' : ` — ${cats()[S.cat].name.toUpperCase()}`;
    const reqOk = !isLocked(r) && !missingAtt(r) && maxOf(r) > 0;
    const stamp = stampExtra || [['ВРЕМЯ', fmtSec(r.t)], ['ВЫХОД', `×${r.cnt}`], ['СОСТОЯНИЕ', r.dmg ? `${Math.round(r.dmg * 100)}% урона` : 'Нетронутое', 'ok'], ...(r.qf ? [['НАПОЛНЕНИЕ', `${Math.round(r.qf * 100)}%`]] : [])];
    return `<div class="sh"><div class="sh__t"><small>${st().doc} № ${r.id}${catName}</small><b>${r.name}</b><p>${r.desc || `${nm(sel.cls)} — описание из конфига предмета (у рецепта своего нет).`}</p></div>
        <div class="sh__r"><div class="stamp">${stamp.map(([k, v, c]) => `<div><small>${k}</small><b class="${c || ''}">${v}</b></div>`).join('')}</div>
        <div class="req ${reqOk ? 'ok' : 'no'}" data-a="req">${svg('shield', 14)}Требования ${reqOk ? 'выполнены' : 'не выполнены'}</div></div></div>`;
}

// ---------- центр: верстак / F9 — «Чертёж» ----------
const CO = { w: 214, h: 50, gap: 8, side: 16, core: 94 };
const hpColor = (p) => (p >= 60 ? 'rgb(84,222,67)' : p >= 30 ? 'rgb(255,201,60)' : 'rgb(255,77,77)');
const hpName = (p) => (p >= 70 ? 'Нетронутое' : p >= 50 ? 'Изношенное' : p >= 30 ? 'Повреждённое' : 'Сильно повреждённое');
const durBar = (p) => `<i class="dur"><i style="width:${p}%;background:${hpColor(p)}"></i></i>`;
const statsFor = (r) => {
    const c = r.res.toLowerCase();
    if (c.includes('crossbow') && !c.includes('bolt') && !c.includes('mag')) return [['ПРОЧНОСТЬ', '250 hp'], ['УРОН ВЫСТРЕЛА', '96'], ['СКОРОСТРЕЛЬНОСТЬ', '60/мин'], ['РЕЖИМ', 'одиночный'], ['БОЕПРИПАС', 'болты']];
    if (c.includes('knife') || c.includes('sword') || c.includes('hatchet') || c.includes('hammer') || c.includes('kirka')) return [['ПРОЧНОСТЬ', '200 hp'], ['УРОН', '45 / 90'], ['ДАЛЬНОСТЬ', '1.6 м'], ['КРОВОТЕЧЕНИЕ', 'да']];
    if (c.includes('boltstack') || c.includes('mag')) return [['УРОН', '96'], ['ЁМКОСТЬ', c.includes('mag') ? '50' : '20']];
    if (c.includes('exo') || c.includes('respirator') || c.includes('glove') || c.includes('boots')) return [['ПРОЧНОСТЬ', '300 hp'], ['ПУЛИ', '35%'], ['ХОЛОДНОЕ', '50%'], ['ЗАРАЖЁННЫЕ', '60%'], ['ВЗРЫВЫ', '20%']];
    return null;
};
function sheetBp(r) {
    return `${headHtml(r)}<div class="dw${isLocked(r) ? ' locked' : ''}" id="dw"></div>${barHtml(r)}`;
}
function drawBp(W, H) {
    const r = recipe(), q = S.qty, cx = W / 2, att = r.comps.filter((c) => c.k === 'a'), side = r.comps.filter((c) => c.k !== 'a');
    const vi = varInfo(r), stats = statsFor(r), top = att.length ? 70 : 0, bottom = (stats ? 56 : 0) + (vi.length > 1 ? 50 : 0);
    const cy = top + (H - top - bottom) / 2;
    const left = Math.ceil(side.length / 2), gap = Math.min(CO.gap, (H - top - bottom - Math.max(left, 1) * CO.h) / Math.max(left - 1, 1));
    const rows = (k) => Array.from({ length: k }, (_, i) => cy + (i - (k - 1) / 2) * (CO.h + gap));
    const ys = [...rows(left), ...rows(side.length - left)];
    const [vcls, vlist] = MOCK_VARS[S.st];
    let lines = '', cards = '';
    side.forEach((c, i) => {
        const L = i < left, y = ys[i], x = L ? CO.side : W - CO.side - CO.w, tool = c.k === 't';
        const isVar = S.mode === 'var' && c.cls === vcls, cls = isVar ? vlist[0] : c.cls;
        const need = c.n * q, h = tool ? 1 : have(c.cls, need), ok = tool || h >= need, k = tool ? 'tool' : ok ? 'ok' : 'no';
        const px = L ? x + CO.w : x, ang = Math.atan2(y - cy, px - cx), ex = cx + Math.cos(ang) * CO.core, ey = cy + Math.sin(ang) * CO.core, bend = (ex - px) / 2;
        lines += `<path class="${k}" d="M ${px} ${y} C ${px + bend} ${y}, ${ex - bend / 2} ${ey}, ${ex} ${ey}"/><circle class="dot ${k}" cx="${ex}" cy="${ey}" r="4"/><circle class="dot ${k}" cx="${px}" cy="${y}" r="3"/>`;
        const sub = tool ? `инструмент · износ ${c.wear}% ${c.ax === 2 ? '· станция или инвентарь' : ''}` : `${fmt(h)} <small>/ ${fmt(need)}${c.mh < 3 ? ' · не хуже «Изношенного»' : ''}</small>`;
        // «Сделать» — только если недостающий предмет сам крафтится на этой станции (переход к его рецепту)
        const src = !ok && !tool && Object.values(D.recipes).find((x) => x.res === c.cls && x.st === r.st);
        const go = src ? `<span class="co__go" data-a="pick" data-v="${src.id}">Сделать${svg('arrow', 12)}</span>` : '';
        cards += `<div class="co${tool ? ' tool' : ''}${ok ? '' : ' no'}${isVar ? ' var sel' : ''}" style="left:${x}px;top:${y - CO.h / 2}px"${isVar ? ' data-a="var"' : ''}>
            <div class="co__pic">${ic(c.ic, 28)}${isVar ? `<span class="sw2">${svg('swap', 11)}</span>` : ''}</div><div class="co__t"><b>${nm(cls)}${c.inh ? ' (любой)' : ''}</b><span>${sub}</span></div>${go}</div>`;
    });
    // обвесы станции (AttachmentsEx = 1) — строкой сверху
    const tw = Math.min(260, (W - 2 * CO.side - (att.length - 1) * 10) / Math.max(att.length, 1)), tx0 = cx - (att.length * tw + (att.length - 1) * 10) / 2;
    att.forEach((c, i) => {
        const x = tx0 + i * (tw + 10), bx = x + tw / 2, by = 12 + 46, hp = installed(c.cls), k = hp == null ? 'no' : 'att';
        const ang = Math.atan2(by - cy, bx - cx), ex = cx + Math.cos(ang) * CO.core, ey = cy + Math.sin(ang) * CO.core;
        lines += `<path class="${k}" d="M ${bx} ${by} C ${bx} ${by + 30}, ${ex} ${ey - 30}, ${ex} ${ey}"/><circle class="dot ${k}" cx="${ex}" cy="${ey}" r="4"/>`;
        const sub = hp == null ? (S.st === 'free' ? 'нужна станция с этим обвесом' : 'не установлен — поставьте на верстак') : `${inStore(c.cls) ? 'в инвентаре станции' : 'обвес'} · износ ${c.wear}% · ${hpName(hp)} ${hp}%`;
        cards += `<div class="tc${k === 'no' ? ' no' : ''}" style="left:${x}px;top:12px;width:${tw}px"><div class="co__pic">${ic(c.ic, 26)}</div><div class="co__t"><b>${nm(c.cls)}</b><span>${sub}</span>${hp != null ? durBar(hp) : ''}</div></div>`;
    });
    const sel = S.vsel[r.id] || 0, okAll = maxOf(r) >= q && !isLocked(r);
    const vrow = vi.length > 1 ? `<div class="vlab" style="left:${cx}px;top:${cy + CO.core + 20}px">ВАРИАНТ РЕЗУЛЬТАТА · ${vi.length}</div>
        <div class="vrow" style="left:${cx}px;top:${cy + CO.core + 36}px">${vi.map((v, i) => `<span class="vr${i === sel ? ' on' : ''}" data-a="vsel" data-v="${i}"><i style="background:${v.bg}"></i>${v.name}</span>`).join('')}</div>` : '';
    return `<svg class="ln" viewBox="0 0 ${W} ${H}">${lines}</svg>
        <div class="core${okAll ? ' ok' : ''}" style="left:${cx}px;top:${cy}px">${ic(r.ic, 104)}${r.cnt * q > 1 ? `<em>×${fmt(r.cnt * q)}</em>` : ''}</div>
        ${cards}${vrow}
        ${stats ? `<div class="wst">${stats.map(([k, v]) => `<div><small>${k}</small><b>${v}</b></div>`).join('')}</div>` : ''}
        <div class="hint3d">${svg('rot', 13)}Тяните — вращать, колесо — масштаб</div>
        ${isLocked(r) ? `<div class="seal">Не изучен<small>изучение — в окне «Изучение»</small></div>` : ''}`;
}

// ---------- центр: швейная машинка — «Раскрой» ----------
const BODY = [['helmet|motohelmet', 'Шлем', 34, 7], ['cap', 'Кепка', 34, 7], ['mask', 'Маска', 68, 15], ['top', 'Куртка', 30, 31], ['vest', 'Жилет', 70, 39], ['belt', 'Пояс', 30, 50],
    ['pants', 'Штаны', 70, 63], ['gloves', 'Перчатки', 16, 47], ['boots|sneakers', 'Обувь', 72, 80]];
const bodyOf = (cls) => BODY.find(([k]) => new RegExp(k, 'i').test(cls));
function sheetSew(r) {
    const c = cats()[S.cat], vi = varInfo(r), sel = S.vsel[r.id] || 0, part = bodyOf(r.res) || BODY[3];
    const stamp = [['ВРЕМЯ', fmtSec(r.t)], ['ЧАСТЬ СЕТА', part[1]], ['РАСЦВЕТОК', vi.length], ['СОСТОЯНИЕ', 'Нетронутое', 'ok']];
    const set = c.all ? null : c.name, pic = SETPIC[set] || 'set_military';
    const pins = (c.all ? [] : c.ids.map(R)).map((x) => { const b = bodyOf(x.res); if (!b) return ''; const k = listStatus(x)[0];
        return `<span class="pin${x.id === r.id ? ' on' : ''}${k === 'no' ? ' no' : ''}" style="left:${b[2]}%;top:${b[3]}%" data-a="pick" data-v="${x.id}">${ic(x.ic, 15)}${b[1]}</span>`; }).join('');
    const [vcls, vlist] = MOCK_VARS.sew, q = S.qty;
    const tags = consumables(r).map((cp) => { const isVar = S.mode === 'var' && cp.cls === vcls, need = cp.n * q, h = have(cp.cls, need), ok = h >= need;
        return `<div class="tag${ok ? '' : ' no'}${isVar ? ' sel' : ''}"${isVar ? ' data-a="var"' : ''}>${ic(cp.ic, 26)}<div class="tag__t"><b>${nm(cp.cls)}</b><span>${fmt(h)} <small>/ ${fmt(need)}</small></span></div>${isVar ? `<span class="sw2">${svg('swap', 10)}</span>` : ''}</div>`; }).join('');
    const tools = r.comps.filter((cp) => cp.k !== 'c').map((cp) => {
        if (cp.k === 'a') { const hp = installed(cp.cls); return `<div class="tl at${hp == null ? ' no' : ''}">${ic(cp.ic, 22)}<div>${nm(cp.cls)}<br><span>${hp == null ? 'не стоит на машинке' : `на машинке · ${hp}%`}</span></div></div>`; }
        return `<div class="tl">${ic(cp.ic, 22)}<div>${nm(cp.cls)}<br><span>инструмент · износ ${cp.wear}%</span></div></div>`;
    }).join('');
    const arm = /vest|helmet|motohelmet/i.test(r.res) ? [80, 55, 70, 40] : /top|pants|belt/i.test(r.res) ? [25, 40, 60, 15] : [10, 20, 45, 5];
    return `${headHtml(r, stamp)}
        <div class="cutw"><div class="ruler"></div>
            <div class="mq"><img src="img/sets/${pic}.jpg" alt="">${pins}
                <div class="mq__f"><small>СЕТ</small><b class="big">${set ? `«${set}»` : 'Все сеты'}</b>${set ? `Вещей: <b>${c.ids.length}</b> · метка — где сидит вещь, клик — её рецепт` : 'Выберите сет слева'}</div></div>
            <div class="cut">
                ${vi.length > 1 ? `<div class="cut__sec">РАСЦВЕТКА<em>${vi.length} варианта</em></div><div class="swatches">${vi.map((v, i) => `<div class="swt${i === sel ? ' on' : ''}" data-a="vsel" data-v="${i}"><i style="background:${v.bg}"></i><b>${v.name}</b></div>`).join('')}</div>` : `<div class="cut__sec">РАСЦВЕТКА<em>одна</em></div>`}
                <div class="cut__sec">МАТЕРИАЛЫ<em>${consumables(r).length} позиций · ×${q}</em></div><div class="tags">${tags}</div>
                <div class="cut__sec">ИНСТРУМЕНТЫ И НАВЕСЫ МАШИНКИ</div><div class="tools">${tools}</div>
                <div class="cut__sec">ЗАЩИТА</div><div class="armor">${[['ПРОЧНОСТЬ', 300], ['ПУЛИ', arm[0]], ['ХОЛОДНОЕ', arm[1]], ['ЗАРАЖЁННЫЕ', arm[2]], ['ВЗРЫВЫ', arm[3]]].map(([k, v], i) =>
                    `<div><small>${k}</small><b>${i ? `${v}%` : `${v} hp`}</b>${i ? durBar(v) : ''}</div>`).join('')}</div>
            </div>
            ${isLocked(r) ? '<div class="seal">Не изучен<small>изучение — в окне «Изучение»</small></div>' : ''}
        </div>${barHtml(r)}`;
}

// ---------- центр: подъёмник — «Сборка» ----------
const PART = [['hood', 'Капот', 'top'], ['kovh', 'Воздухозаборник', 'top2'], ['bamper', 'Бампер', 'front'], ['trunk', 'Багажник', 'back'], ['codriver', 'Пассаж. дверь', 'right'], ['driver', 'Водит. дверь', 'left'], ['wheel', 'Колёса', 'wheel']];
const partOf = (cls) => PART.find(([k]) => cls.toLowerCase().includes(k));
const partState = (id) => (S.parts[id] = S.parts[id] || { on: true, qty: partOf(R(id).res)[2] === 'wheel' ? 4 : 1 });
function liftMerged(r) {
    const m = new Map(), add = (c, mult) => { const x = m.get(c.cls) || { ...c, n: 0 }; x.n += c.n * mult; m.set(c.cls, x); };
    r.comps.forEach((c) => add(c, S.qty));
    r.att.forEach((id) => { const p = partState(id); if (p.on) R(id).comps.forEach((c) => add(c, p.qty)); });
    return [...m.values()];
}
const GROUPS = [['Металл и крепёж', /metalplate|metalparts|bolts|nuts|screw/i], ['Электрика', /battery|wire|cable|rele|capacit|headlight|magnet|engine|gauge/i],
    ['Химия и жидкости', /oil|gasolin|poxearm|klean|foam|tape|skoth/i], ['Кузов и салон', /./]];
function sheetLift(r) {
    const parts = r.att.map(R), on = parts.filter((p) => partState(p.id).on), extra = on.reduce((t, p) => t + p.t * partState(p.id).qty, 0);
    const vi = varInfo(r), sel = S.vsel[r.id] || 0;
    const stamp = [['ВРЕМЯ', fmtSec(r.t + extra)], ['ДЕТАЛИ', `${on.length} из ${parts.length}`], ['ЦВЕТ', vi[sel].name]];
    const all = liftMerged(r), mat = all.filter((c) => c.k === 'c'), need = mat.reduce((t, c) => t + c.n, 0), got = mat.reduce((t, c) => t + Math.min(c.n, have(c.cls, c.n)), 0);
    const rows = GROUPS.map(([g, re]) => [g, mat.filter((c) => re.test(c.cls) && !GROUPS.slice(0, GROUPS.findIndex((x) => x[0] === g)).some(([, r2]) => r2.test(c.cls)))]).filter(([, xs]) => xs.length);
    const row = (c) => { const h = have(c.cls, c.n), ok = h >= c.n; return `<div class="row${ok ? '' : ' no'}">${ic(c.ic, 18)}<b>${nm(c.cls)}</b><span>${fmt(h)} <small>/ ${fmt(c.n)}</small></span></div>`; };
    const tools = all.filter((c) => c.k !== 'c').map((c) => c.k === 'a' ? `<div class="row at">${ic(c.ic, 18)}<b>${nm(c.cls)}</b><span>${installed(c.cls) == null ? 'нет на подъёмнике' : 'на подъёмнике'}</span></div>`
        : `<div class="row tool">${ic(c.ic, 18)}<b>${nm(c.cls)}</b><span>износ ${c.wear}%</span></div>`).join('');
    return `${headHtml(r, stamp)}
        <div class="asm"><div class="bay" id="bay"></div>
            <div class="spec"><div class="spec__h"><small>СПЕЦИФИКАЦИЯ · КУЗОВ + ВЫБРАННЫЕ ДЕТАЛИ</small><b>${fmt(got)} <em>/ ${fmt(need)} предметов</em></b><div class="pbar"><i style="width:${Math.round((got / need) * 100)}%"></i></div></div>
                <div class="spec__l">${rows.map(([g, xs]) => `<div class="grp">${g}</div>${xs.map(row).join('')}`).join('')}<div class="grp">Инструменты и подъёмник</div>${tools}</div>
                <div class="store"><div>Хранилище подъёмника<b>412 / 650</b></div><div>Листовой металл в слотах<b>120 / 120</b></div></div></div>
            ${isLocked(r) ? '<div class="seal">Не изучен<small>изучение — в окне «Изучение»</small></div>' : ''}</div>
        ${barHtml(r, extra, `${nm(vi[sel].cls)} + ${on.length} дет.`)}`;
}
function drawLift(W, H) {
    const r = recipe(), vi = varInfo(r), sel = S.vsel[r.id] || 0, sc = Math.min((H - 70) / 470, 1.05), cw = 150 * sc, ch = 440 * sc, x0 = W / 2 - cw / 2, y0 = (H - ch) / 2 - 8;
    const X = (v) => x0 + v * sc, Y = (v) => y0 + v * sc, paint = vi[sel].bg;
    const parts = r.att.map(R), byPos = {};
    parts.forEach((p) => { const k = partOf(p.res); if (k) byPos[k[2]] = p; });
    const st2 = (pos) => { const p = byPos[pos]; if (!p) return ''; const s = partState(p.id); return s.on ? (consumables(p).some((c) => lacking(c.cls)) ? 'no' : 'on') : ''; };
    const wheelOn = st2('wheel');
    let g = `<rect x="${X(-38)}" y="${Y(-14)}" width="${226 * sc}" height="${468 * sc}" rx="${14 * sc}" fill="none" stroke="rgba(255,201,60,.25)" stroke-dasharray="8 6"/>`;
    [[-12, 60], [138, 60], [-12, 330], [138, 330]].forEach(([x, y]) => { g += `<rect class="car-wheel ${wheelOn}" x="${X(x)}" y="${Y(y)}" width="${24 * sc}" height="${70 * sc}" rx="${6 * sc}"/>`; });
    g += `<path class="car-body" style="fill:${paint};fill-opacity:.22" d="M ${X(14)} ${Y(30)} Q ${X(75)} ${Y(-6)} ${X(136)} ${Y(30)} L ${X(146)} ${Y(120)} L ${X(150)} ${Y(380)} Q ${X(146)} ${Y(440)} ${X(75)} ${Y(446)} Q ${X(4)} ${Y(440)} ${X(0)} ${Y(380)} L ${X(4)} ${Y(120)} Z"/>`;
    const zone = (pos, d) => byPos[pos] ? `<path class="car-part ${st2(pos)}" d="${d}"/>` : '';
    g += zone('top', `M ${X(18)} ${Y(36)} Q ${X(75)} ${Y(8)} ${X(132)} ${Y(36)} L ${X(138)} ${Y(124)} L ${X(12)} ${Y(124)} Z`);
    g += zone('top2', `M ${X(55)} ${Y(60)} h ${40 * sc} v ${40 * sc} h ${-40 * sc} Z`);
    g += zone('front', `M ${X(14)} ${Y(14)} Q ${X(75)} ${Y(-14)} ${X(136)} ${Y(14)} L ${X(132)} ${Y(30)} Q ${X(75)} ${Y(6)} ${X(18)} ${Y(30)} Z`);
    g += zone('back', `M ${X(10)} ${Y(340)} L ${X(140)} ${Y(340)} L ${X(144)} ${Y(400)} Q ${X(75)} ${Y(430)} ${X(6)} ${Y(400)} Z`);
    g += zone('left', `M ${X(2)} ${Y(140)} h ${16 * sc} v ${110 * sc} h ${-16 * sc} Z`);
    g += zone('right', `M ${X(132)} ${Y(140)} h ${16 * sc} v ${110 * sc} h ${-16 * sc} Z`);
    g += `<path class="car-glass" d="M ${X(22)} ${Y(132)} L ${X(128)} ${Y(132)} L ${X(118)} ${Y(178)} L ${X(32)} ${Y(178)} Z"/><rect class="car-line" x="${X(32)}" y="${Y(180)}" width="${86 * sc}" height="${120 * sc}" rx="${8 * sc}"/>
        <path class="car-glass" d="M ${X(32)} ${Y(302)} L ${X(118)} ${Y(302)} L ${X(126)} ${Y(334)} L ${X(24)} ${Y(334)} Z"/>`;
    // метки деталей
    const POS = { top: [W / 2, Y(-14) - 34, 'c'], top2: [X(162) + 14, Y(80), 'r'], front: [W / 2 - 120, Y(-14) - 34, 'c'], back: [W / 2, Y(446) + 36, 'c'], left: [X(-12) - 14, Y(195), 'l'], right: [X(162) + 14, Y(195), 'r'], wheel: [X(-12) - 14, Y(365), 'l'] };
    const ANCH = { top: [W / 2, Y(70)], top2: [X(75), Y(80)], front: [X(40), Y(8)], back: [W / 2, Y(390)], left: [X(10), Y(195)], right: [X(140), Y(195)], wheel: [X(0), Y(365)] };
    let lines = '', chips = '';
    Object.entries(byPos).forEach(([pos, p]) => {
        const s = partState(p.id), k = s.on ? st2(pos) : 'off', [px, py, al] = POS[pos], [ax, ay] = ANCH[pos], pvi = varInfo(p), wheel = pos === 'wheel';
        lines += `<path class="lead ${k === 'no' ? 'no' : k === 'on' ? 'on' : 'off'}" d="M ${px} ${py} L ${ax} ${ay}"/><circle cx="${ax}" cy="${ay}" r="3.5" fill="${k === 'on' ? 'rgb(84,222,67)' : k === 'no' ? '#ff4d4d' : 'rgba(255,255,255,.4)'}"/>`;
        const tr = al === 'c' ? 'translate(-50%,-50%)' : al === 'l' ? 'translate(-100%,-50%)' : 'translate(0,-50%)';
        const dot = pvi.length > 1 ? `<span class="dot" style="background:${pvi[S.vsel[p.id] || sel % pvi.length].bg}" title="Цвет детали"></span>` : '';
        const qty = wheel ? `<span class="q"><span data-a="pq" data-v="${p.id}|-1">−</span><b>${s.qty}</b><span data-a="pq" data-v="${p.id}|1">+</span></span>` : '';
        chips += `<div class="pt ${s.on ? (k === 'no' ? 'on no' : 'on') : ''}" style="left:${px}px;top:${py}px;transform:${tr}" data-a="part" data-v="${p.id}" title="${p.name}${k === 'no' ? ' — не хватает материалов' : ''}"><span class="ck">${svg('check', 12)}</span>
            <div class="pt__t"><b>${PART.find((x) => x[2] === pos)[1]}${dot}</b><span>+${fmtSec(p.t * s.qty)}${qty}</span></div></div>`;
    });
    return `<svg viewBox="0 0 ${W} ${H}">${g}${lines}</svg>${chips}
        <div class="paint"><small>ОКРАСКА</small>${vi.map((v, i) => `<span class="pc${i === sel ? ' on' : ''}" style="background:${v.bg}" title="${v.name}" data-a="vsel" data-v="${i}"></span>`).join('')}</div>
        <div class="bayh">Доп. детали — отдельные рецепты, ставятся сразу на машину.<br>Колёс можно выбрать до 10.</div>`;
}

// ---------- справа: очередь, обвесы, разбор ----------
function rightHtml() {
    const q = QUEUE(), s = st(), r = recipe();
    const qs = Array.from({ length: D.maxQueue }, (_, k) => {
        const x = q[k];
        if (!x) return '<div class="qi free">Свободное место</div>';
        const rr = R(x.id);
        if (!x.run) return `<div class="qi wait" data-a="pick" data-v="${x.id}"><div class="qi__pic">${ic(rr.ic, 26)}</div><div class="qi__t"><b>${rr.name}</b><small>в очереди — начнётся следом</small></div><span class="qi__tm">${svg('clock', 14)}</span><span class="qi__x">${svg('x', 13)}</span></div>`;
        return `<div class="qi" data-a="pick" data-v="${x.id}"><div class="qi__pic">${ic(rr.ic, 26)}</div><div class="qi__t"><b>${rr.name}</b><small>×${S.qty * rr.cnt} — идёт крафт</small></div>
            <span class="qi__tm">${clock(x.left)}</span><span class="qi__x">${svg('x', 13)}</span><div class="qi__bar"><i style="width:${(1 - x.left / x.total) * 100}%"></i></div></div>`;
    }).join('');
    const need = new Set(r.comps.filter((c) => c.k === 'a').map((c) => c.cls));
    const slots = s.slots ? `<div class="blk"><div class="blk__h"><small>Обвесы ${S.st === 'sew' ? 'машинки' : S.st === 'lift' ? 'подъёмника' : 'верстака'}</small><em>${s.slots.filter((x) => slotHp(x) != null).length} / ${s.slots.length}</em></div>
        <div class="slots">${s.slots.map((x) => { const hp = slotHp(x), nd = need.has(x[1]) || (x[3] || []).some((a) => need.has(a));
            return hp == null ? `<div class="slot free${nd ? ' need' : ''}"><span class="slot__n">${x[0]}</span>${svg('plus', 16)}<small>${nd ? 'нужен' : 'пусто'}</small></div>`
                : `<div class="slot${nd ? ' need' : ''}" title="${x[0]} — ${hpName(hp)} ${hp}%"><span class="slot__n">${x[0]}</span>${ic(icFor(x[1]), 26)}${durBar(hp)}</div>`; }).join('')}</div>
        <p>Обвесы ставятся через инвентарь станции. Рамка — нужен выбранному рецепту, износ списывается за каждый крафт.</p></div>`
        : `<div class="blk"><div class="blk__h"><small>Без станции</small></div><p>Рецепты с обвесами (тиски, наковальня, инструменты…) в меню F9 не крафтятся — только на верстаке.</p></div>`;
    const sheets = S.st === 'lift' ? `<div class="blk"><div class="sheets">${ic('drl_metal-plate', 26)}<div><b>Листовой металл: 120 / 120</b><small>5 слотов подъёмника · ещё хранилище 10×65</small></div></div></div>` : '';
    // разбор (RecraftTimeSec, RecraftDropChance)
    const drops = r.comps.filter((c) => c.ch > 0), hasItem = S.mode === 'recraft';
    const rc = drops.length ? `<div class="rc"><div class="rc__h">${svg('refresh', 18)}<div><small>Разбор</small><b>${r.name}</b></div><em>${fmtSec(r.rt)}</em></div>
        <div class="rc__g">${drops.slice(0, 12).map((c) => `<i title="${nm(c.cls)} — шанс ${Math.round(c.ch * 100)}%">${ic(c.ic, 24)}<em>×1</em>${c.ch < 1 ? `<u>${Math.round(c.ch * 100)}%</u>` : ''}</i>`).join('')}</div>
        ${hasItem ? `<div class="rc__b">${svg('refresh', 15)}Разбирается… 6 с</div><div class="rc__p"><i style="width:60%"></i></div>` : `<div class="rc__b na">Нет предмета в инвентаре</div>`}</div>`
        : `<div class="rc"><div class="rc__h">${svg('refresh', 18)}<div><small>Разбор</small><b>Этот предмет не разбирается</b></div></div></div>`;
    return `<div class="qh"><div class="qh__t"><small>Очередь</small><b>${q.length}<em>/ ${D.maxQueue}</em></b></div>
        <span class="learn" data-a="learn">${svg('book', 14)}Изучение</span><span class="x" data-a="noop">${svg('x', 15)}</span></div>
        <div class="qs">${qs}</div>${slots}${sheets}${rc}
        <div class="inv"><span>${svg('layers', 13)} Свободно в инвентаре${s.slots ? ' и станции' : ''}</span><b>${S.st === 'lift' ? '238 из 1174' : '38 из 77'}</b></div>`;
}

// ---------- окна поверх ----------
function modalHtml() {
    const r = recipe();
    if (S.modal === 'var') {
        const [vcls, vlist] = MOCK_VARS[S.st], c = r.comps.find((x) => x.cls === vcls) || consumables(r)[0], need = c.n * S.qty;
        return `<div class="m" data-a="close"><div class="mb" data-a="noop"><div class="mh"><span class="mh__ic">${svg('swap', 26)}</span><div><small>ВАРИАНТЫ ИНГРЕДИЕНТА</small><b>${nm(vcls)}</b></div></div>
            <div class="ml">Чем заменить<em>есть / нужно</em></div><div class="pks">${vlist.map((v, i) => { const h = i === 2 ? Math.floor(need / 3) : have(v, need) + i;
                return `<div class="pk${i === 0 ? ' on' : ''}"><span class="ck">${svg('check', 13)}</span><div class="pk__pic">${ic(D.icons[v] || c.ic, 26)}</div><div class="pk__t"><b>${nm(v)}</b><small>${i === 0 ? 'выбран сейчас' : 'подходит для этого рецепта'}</small></div><span class="pk__r${h >= need ? '' : ' no'}">${h} / ${need}</span></div>`; }).join('')}</div>
            <div class="mnote">В игре панель выезжает поверх колонки предмета. Если вещь разбита на части, ниже идёт выбор по каждой части.</div>
            <div class="mbtn one"><div class="ghost" data-a="close">Закрыть</div></div></div></div>`;
    }
    if (S.modal === 'req') {
        const att = r.comps.filter((c) => c.k === 'a'), tools = r.comps.filter((c) => c.k === 't'), lack = consumables(r).filter((c) => have(c.cls, c.n * S.qty) < c.n * S.qty);
        const row = (icn, t, sub, ok, right) => `<div class="pk"><div class="pk__pic">${icn}</div><div class="pk__t"><b>${t}</b><small>${sub}</small></div><span class="pk__r${ok ? '' : ' no'}">${right}</span></div>`;
        return `<div class="m" data-a="close"><div class="mb" data-a="noop"><div class="mh"><span class="mh__ic">${svg('shield', 26)}</span><div><small>ТРЕБОВАНИЯ</small><b>${r.name}</b></div></div>
            <div class="ml">Изучение</div><div class="pks">${row(ic(r.ic, 26), r.name, 'Изучение рецепта', !isLocked(r), isLocked(r) ? 'Не изучен' : 'Изучен')}</div>
            ${att.length ? `<div class="ml">Обвесы станции</div><div class="pks">${att.map((c) => row(ic(c.ic, 26), nm(c.cls), `износ ${c.wear}% за крафт`, installed(c.cls) != null, installed(c.cls) != null ? 'Установлен' : 'Нет')).join('')}</div>` : ''}
            ${tools.length ? `<div class="ml">Инструменты<em>не тратятся, изнашиваются</em></div><div class="pks">${tools.map((c) => row(ic(c.ic, 26), nm(c.cls), `износ ${c.wear}% · состояние ${toolHp(c.cls)}%`, true, 'Есть')).join('')}</div>` : ''}
            ${lack.length ? `<div class="ml">Не хватает<em>есть / нужно</em></div><div class="pks">${lack.map((c) => row(ic(c.ic, 26), nm(c.cls), 'Расходуется', false, `${have(c.cls, c.n * S.qty)} / ${c.n * S.qty}`)).join('')}</div>` : ''}
            <div class="mbtn one"><div class="ghost" data-a="close">Закрыть</div></div></div></div>`;
    }
    if (S.modal === 'learn') {
        return `<div class="m" data-a="close"><div class="mb" data-a="noop"><div class="mh"><span class="mh__ic">${svg('book', 26)}</span><div><small>ИЗУЧЕНИЕ РЕЦЕПТА</small><b>${r.name}</b></div></div>
            <div class="ml">Способ изучения<em>выберите один · есть / нужно</em></div><div class="pks">
                <div class="pk on"><span class="ck">${svg('check', 13)}</span><div class="pk__pic">${ic('drl_open-book', 26)}</div><div class="pk__t"><b>Журнал мастера</b><small>Предмет-книга, тратится при изучении</small></div><span class="pk__r">1 / 1</span></div>
                <div class="pk"><span class="ck">${svg('check', 13)}</span><div class="pk__pic">${ic(r.ic, 26)}</div><div class="pk__t"><b>${nm(r.res)} — образец</b><small>Разобрать готовый предмет</small></div><span class="pk__r no">0 / 1</span></div></div>
            <div class="ml">Сначала изучить<em>требуемые рецепты</em></div><div class="pks"><div class="pk"><div class="pk__pic">${ic('drl_rune-stone', 26)}</div><div class="pk__t"><b>Руна Огня</b><small>Рецепт-требование</small></div><span class="pk__r">Изучен</span></div></div>
            <div class="ml">Откроет<em>рецептов: 1</em></div>
            <div class="mbtn"><div class="ghost" data-a="close">Отмена</div><div class="go gold" data-a="close">${svg('book', 18)}Изучить</div></div>
            <div class="mnote">Сейчас LearningRecipes на сервере пуст — всё открыто. Окно показано для книг EFT (ClochingBook, Jurnals1).</div></div></div>`;
    }
    return '';
}

// ---------- отрисовка ----------
function render() {
    const s = st(), r = recipe();
    $('#win').className = `win st-${S.st}`;
    $('#left').innerHTML = leftHtml();
    const sh = $('#sheet');
    sh.className = s.cls;
    sh.innerHTML = S.st === 'sew' ? sheetSew(r) : S.st === 'lift' ? sheetLift(r) : sheetBp(r);
    const dw = $('#dw');
    if (dw) dw.innerHTML = drawBp(dw.clientWidth, dw.clientHeight);
    const bay = $('#bay');
    if (bay) bay.innerHTML = drawLift(bay.clientWidth, bay.clientHeight);
    $('#right').innerHTML = rightHtml();
    $('#modal').innerHTML = modalHtml();
    $('#toast').innerHTML = S.mode === 'run' ? `<div class="toast"><span class="toast__ic">${ic(s.icon, 22)}</span><div><b>Крафт начат</b><small>${r.name} ×${S.qty} — ${fmtSec(r.t * S.qty)}</small></div><i class="tb"></i></div>` : '';
}
function selectStation(k) {
    S.st = k; const s = ST[k];
    S.cat = Math.max(0, cats().findIndex((c) => c.name === s.cat));
    S.sel = s.sel || cats()[S.cat].ids.map(R).find((r) => r.res.includes(s.selRes)).id;
    S.qty = 1; S.q = ''; S.only = false;
}
const A = {
    noop: () => {},
    cat: (v) => { S.cat = +v; const r = shown()[0]; if (r) S.sel = r.id; S.qty = 1; render(); },
    pick: (v) => { S.sel = +v; S.qty = 1; render(); },
    only: () => { S.only = !S.only; render(); },
    qty: (d) => { S.qty = Math.max(1, Math.min(Math.max(1, maxOf(recipe())), S.qty + +d)); render(); },
    set: (n) => { S.qty = Math.max(1, +n); render(); },
    vsel: (i) => { S.vsel[S.sel] = +i; render(); },
    part: (id) => { const p = partState(+id); p.on = !p.on; render(); },
    pq: (v) => { const [id, d] = v.split('|').map(Number); const p = partState(id); p.qty = Math.max(1, Math.min(10, p.qty + d)); render(); },
    var: () => { S.modal = 'var'; render(); },
    req: () => { S.modal = 'req'; render(); },
    learn: () => { S.modal = 'learn'; render(); },
    craft: () => { S.mode = 'run'; mockMark('run'); render(); },
    close: () => { S.modal = null; render(); },
};
const mockMark = (m) => $$('.mock [data-m]').forEach((x) => x.classList.toggle('a', x.dataset.m === m));
$('#app').innerHTML = skeleton();
$('#app').addEventListener('click', (e) => {
    const t = e.target.closest('[data-a]');
    if (!t) return;
    if (t.dataset.a === 'pq') e.stopPropagation();
    if (A[t.dataset.a]) { A[t.dataset.a](t.dataset.v); }
}, true);
$('#app').addEventListener('input', (e) => { if (e.target.id === 'q') { S.q = e.target.value; render(); const i = $('#q'); i.focus(); i.setSelectionRange(i.value.length, i.value.length); } });

// ---------- переключатели макета ----------
const MODES = { ok: null, lack: null, notool: null, locked: null, run: null, full: null, var: 'var', req: 'req', learn: 'learn', recraft: null };
$$('.mock [data-st]').forEach((b) => (b.onclick = () => { $$('.mock [data-st]').forEach((x) => x.classList.toggle('a', x === b)); selectStation(b.dataset.st); render(); }));
$$('.mock [data-m]').forEach((b) => (b.onclick = () => { S.mode = b.dataset.m; S.modal = MODES[S.mode]; mockMark(S.mode); render(); }));
const P = new URLSearchParams(location.search);
selectStation(P.get('st') || 'wb');
if (P.get('cat')) { S.cat = +P.get('cat'); S.sel = cats()[S.cat].ids[0]; }
if (P.get('noanim')) document.body.classList.add('noanim');
S.mode = P.get('m') || 'ok'; S.modal = MODES[S.mode] || null;
$$('.mock [data-st]').forEach((x) => x.classList.toggle('a', x.dataset.st === S.st)); mockMark(S.mode);
render();
const fit = () => ($('#stage').style.transform = `translate(-50%,-50%) scale(${Math.min(innerWidth / 1920, innerHeight / 1080)})`);
addEventListener('resize', () => { fit(); render(); }); fit();
