// Макет верстака склада v2 «Чертёж». Каркас рисуется один раз, при кликах меняются только части (список, лист, очередь, окно улучшения).
const $ = (s, r = document) => r.querySelector(s);
const $$ = (s, r = document) => [...r.querySelectorAll(s)];
const fmt = (n) => Number(n).toLocaleString('ru-RU');
const IMG = '../../server/client_packages/images/inventoryItems/items/';
const I = {
    hammer: '<path d="M14 4l6 6-3 3-6-6zM11 7l-8 8 3 3 8-8"/>',
    tool: '<path d="M14.7 6.3a1 1 0 000 1.4l1.6 1.6a1 1 0 001.4 0l3.8-3.8a6 6 0 01-7.9 7.9l-6.9 6.9a2.1 2.1 0 01-3-3l6.9-6.9a6 6 0 017.9-7.9z"/>',
    gun: '<path d="M3 8h15l2 2v2h-8l-1 3H7l1-3H3z"/><path d="M12 12v2"/>',
    chip: '<rect x="6" y="6" width="12" height="12" rx="2"/><path d="M9 2v4M15 2v4M9 18v4M15 18v4M2 9h4M2 15h4M18 9h4M18 15h4"/>',
    box: '<path d="M3 7l9-4 9 4v10l-9 4-9-4V7z"/><path d="M3 7l9 4 9-4M12 11v10"/>',
    shield: '<path d="M12 3l8 3v6c0 5-3.5 8-8 9-4.5-1-8-4-8-9V6l8-3z"/>',
    star: '<path d="M12 3l2.8 5.7 6.2.9-4.5 4.4 1 6.2L12 17.3 6.5 20.2l1-6.2L3 9.6l6.2-.9z"/>',
    x: '<path d="M18 6 6 18M6 6l12 12"/>', check: '<path d="m5 12 5 5 9-10"/>',
    clock: '<circle cx="12" cy="12" r="9"/><path d="M12 7v5l3 2"/>',
    lock: '<rect x="5" y="11" width="14" height="10" rx="2"/><path d="M8 11V7a4 4 0 0 1 8 0v4"/>',
    up: '<path d="M12 19V5M6 11l6-6 6 6"/>', arrow: '<path d="M5 12h14M13 6l6 6-6 6"/>',
    search: '<circle cx="11" cy="11" r="7"/><path d="m20 20-4-4"/>',
    alert: '<circle cx="12" cy="12" r="9"/><path d="M12 7v5M12 16h.01"/>',
    fire: '<path d="M12 22a7 7 0 007-7c0-4-3-6-4-9-2 2-3 4-3 6-1-1-2-2-2-4-3 3-5 6-5 7a7 7 0 007 7z"/>',
    saw: '<circle cx="12" cy="12" r="2.5"/><path d="M12 2.5l2.2 2.6 3.2-1 .4 3.4 3.2 1.4-1.9 2.8 1.9 2.8-3.2 1.4-.4 3.4-3.2-1-2.2 2.6-2.2-2.6-3.2 1-.4-3.4-3.2-1.4 1.9-2.8-1.9-2.8 3.2-1.4.4-3.4 3.2 1z"/>',
    vise: '<path d="M4 5h6v8H4zM14 5h6v8h-6zM10 9h4M7 13v6M17 13v6M3 19h18"/>',
    plus: '<path d="M12 5v14M5 12h14"/>',
    bag: '<path d="M6 2 3 6v14a2 2 0 0 0 2 2h14a2 2 0 0 0 2-2V6l-3-4Z"/><path d="M3 6h18M16 10a4 4 0 0 1-8 0"/>',
};
const ico = (n, s = 18) => `<svg class="i" viewBox="0 0 24 24" style="width:${s}px;height:${s}px">${I[n] || ''}</svg>`;
const fmtSec = (s) => { if (s < 60) return `${s} с`; if (s < 3600) { const m = Math.floor(s / 60), r = s % 60; return `${m} мин${r ? ` ${r} с` : ''}`; } const h = Math.floor(s / 3600), m = Math.floor((s % 3600) / 60); return `${h} ч${m ? ` ${m} мин` : ''}`; };
const clock = (s) => (s >= 3600 ? `${Math.floor(s / 3600)}:${String(Math.floor((s % 3600) / 60)).padStart(2, '0')}:${String(s % 60).padStart(2, '0')}` : `${String(Math.floor(s / 60)).padStart(2, '0')}:${String(s % 60).padStart(2, '0')}`);

// разделы и типы оружия (в игре — из настроек крафта, как в редакторе)
const CATS = [['Оружие', 'gun'], ['Инструменты', 'tool'], ['Электроника', 'chip'], ['Материалы', 'box'], ['Броня', 'shield'], ['Прочее', 'star']];
const SUBS = ['Пистолеты', 'Дробовики', 'Пистолеты-пулемёты', 'Винтовки', 'Снайперские'];
const STATS = [['damage', 'УРОН'], ['rateFire', 'СКОРОСТРЕЛЬНОСТЬ'], ['accuracy', 'ТОЧНОСТЬ'], ['range', 'ДАЛЬНОСТЬ']];
const FACTORY = { melting: ['Переплавка', 'fire'], sawmill: ['Лесопилка', 'saw'] };
const STAT_MAX = 5, MAX_QTY = 100, AMMO_STEP = 10, MAX_QUEUE = 5, NEW_SHOWN = 8, QUICK = [1, 5, 10];
// чертёж: выноски — колонками слева и справа от изделия, обвесы — строкой сверху
const CO = { w: 210, h: 50, gap: 8, side: 16, core: 92 };
const TC = { w: 250, h: 46, gap: 10, top: 12, row: 70, narrow: 230 };   // обвесы строкой: 3–4 штуки сужаются, уже narrow — компактный вид
const DUR_GOOD = 60, DUR_LOW = 30;   // цвет полоски прочности — как в инвентаре (inventory/config.js durabilityColor)
const durColor = (pct) => (pct >= DUR_GOOD ? '#54de43' : pct >= DUR_LOW ? '#ffc93c' : '#ff4d4d');

// ---------- данные-примеры (в игре — с сервера) ----------
const D = window.DATA;
D.wb.forEach((r, i) => (r.id = i));
const name = (id) => (D.items[id] ? D.items[id][0] : `Предмет ${id}`);
const img = (id) => `${IMG}${id}.png`;
const P = { frac: 2, league: 1, schemes: ['Pistol', 'HeavyPistol'], owner: true };   // игрок: Ballas, лига «Бронза», схемы семьи
const MOCK_FRAC = [2, 'Ballas'];   // переключатель макета «во фракции / не во фракции»
const WH = { id: 7, area: 'Эльбург-Хайтс', lvl: 2, max: 3 };
// цена улучшения до уровня (в игре — /craftedit → «Уровни»)
const UPGRADES = {
    2: { lvl: 2, money: 350000, coins: 2500, items: [[245, 400], [501, 250], [504, 1]] },
    3: { lvl: 3, money: 700000, coins: 6000, items: [[506, 100], [500, 150], [505, 1]] },
};
let NEXT = UPGRADES[WH.lvl + 1];
const WALLET = { money: 1250000, coins: 4200 };
const INV_FULL = { 13: 1840, 463: 2, 500: 9, 542: 30, 418: 3, 511: 7, 501: 40, 237: 120, 506: 60, 505: 1, 516: 6, 487: 40, 486: 4, 408: 1, 532: 6, 497: 12, 493: 10, 494: 2, 499: 1, 410: 3, 491: 4, 489: 15, 488: 3, 244: 1, 515: 2, 513: 1 };
const INV_LACK = { ...INV_FULL, 500: 2, 511: 1 };
let INV = INV_FULL;
const QUEUE_BASE = [{ item: 501, count: 1, left: 0, total: 10 }, { item: 235, count: 1, left: 75, total: 200 }, { item: 497, count: 30, left: 1810, total: 3000 }];
let QUEUE = QUEUE_BASE;
// обвесы: стоят на верстаке (по местам, null — пусто) и лежат в инвентаре игрока
const INSTALLED_BASE = [{ item: 194, dur: 320 }, { item: 658, dur: 40 }, null];
const INV_TOOLS_BASE = [{ item: 417, dur: 300 }, { item: 488, dur: 150 }];
let INSTALLED = INSTALLED_BASE.map((x) => x && { ...x }), INV_TOOLS = INV_TOOLS_BASE.map((x) => ({ ...x }));
const toolOf = (id) => D.tools.find((t) => t.item === id);
const slots = () => D.toolSlots[WH.lvl - 1];
const installed = (id) => INSTALLED.slice(0, slots()).find((x) => x && x.item === id);
const missingTool = (r) => (r.tools || []).find(([id]) => !installed(id));

// доступ (как Craft.Repository.HasPlayerAccessCraft): хватает одного условия; без условий — всем
const access = (r) => { const c = []; if (r.frac) c.push(r.frac.includes(P.frac)); if (r.league !== undefined) c.push(P.league >= r.league); if (r.scheme) c.push(P.schemes.includes(r.scheme)); return !c.length || c.some(Boolean); };
const visible = () => D.wb.filter((r) => !r.hide && access(r));
const have = (id) => INV[id] || 0;
const maxOf = (r) => Math.min(MAX_QTY, ...r.needs.map(([id, n]) => Math.floor(have(id) / n)), ...(r.tools || []).map(([id, w]) => { const x = installed(id); return x ? Math.floor(x.dur / w) : 0; }));
const locked = (r) => r.lvl > WH.lvl;
const rank = (r) => (locked(r) ? 2 : maxOf(r) > 0 ? 0 : 1);   // в списке: можно — не хватает — закрыто
// где делается недостающий предмет: верстак (рецепт — перейти) или завод
const source = (id) => { const w = visible().find((r) => r.item === id); if (w) return ['wb', w.id]; for (const t of ['melting', 'sawmill']) if (D[t].some((r) => r.item === id)) return [t]; return null; };

const S = { cat: 1, sub: 0, sel: null, qty: 1, ammo: 50, q: '', modal: false };
const recipe = () => D.wb[S.sel];

// ---------- каркас ----------
function skeleton() {
    return `<div class="owb__shade"></div>
    <div class="owb-win">
        <div class="owb-col owb-left">
            <div class="owb-h"><span class="owb-h__ic">${ico('hammer', 21)}</span>
                <div class="owb-h__t"><small>Склад № ${WH.id} — ${WH.area}</small><b>Верстак</b></div></div>
            <label class="owb-search">${ico('search', 15)}<input id="q" placeholder="Найти рецепт" spellcheck="false"></label>
            <div class="owb-cats" id="cats"></div>
            <div id="sec"></div>
            <div class="owb-list" id="list"></div>
        </div>
        <div class="owb-sheet" id="sheet"></div>
        <div class="owb-col owb-right" id="queue"></div>
    </div>
    <div id="modal"></div>`;
}
const lvHtml = () => {
    const top = WH.lvl >= WH.max, opens = top ? 0 : D.wb.filter((r) => r.lvl === NEXT.lvl && !r.hide && access(r)).length;
    return `<div class="owb-lvc"><div class="owb-lvc__h"><div><small>Уровень верстака</small><b>${WH.lvl} <em>из ${WH.max}</em></b></div>
        <span class="owb-pips">${Array.from({ length: WH.max }, (_, i) => `<i class="${i < WH.lvl ? 'on' : ''}"></i>`).join('')}</span></div>
        ${top ? '<p>Открыты все чертежи</p>' : `<p>На ${NEXT.lvl} уровне откроется чертежей: <b>${opens}</b></p>
        ${P.owner ? `<div class="owb-up" data-a="upgrade">${ico('up', 15)}Улучшить верстак</div>` : '<div class="owb-up na">Улучшает владелец склада</div>'}`}</div>`;
};
const catsHtml = () => CATS.map(([n, ic], i) => { const c = visible().filter((r) => r.cat === i).length; return `<div class="owb-cat${S.cat === i && !S.q ? ' on' : ''}${c ? '' : ' off'}" data-a="cat" data-v="${i}" title="${n}">${ico(ic, 19)}<em>${c || ''}</em></div>`; }).join('');
const statusHtml = (r) => {
    if (locked(r)) return `<div class="owb-st lk">${ico('lock', 12)}Откроется на ${r.lvl} уровне</div>`;
    const mt = missingTool(r);
    if (mt) return `<div class="owb-st tl">${ico('vise', 12)}Нужен обвес: ${name(mt[0])}</div>`;
    const m = maxOf(r);
    if (m > 0) return `<div class="owb-st ok">${ico('check', 12)}Можно скрафтить: ${m}</div>`;
    const lack = r.needs.filter(([id, n]) => have(id) < n);
    if (!lack.length) return `<div class="owb-st no">${ico('alert', 12)}Не хватит прочности обвеса</div>`;
    return `<div class="owb-st no">${ico('alert', 12)}Не хватает: ${name(lack[0][0])}${lack.length > 1 ? ` +${lack.length - 1}` : ''}</div>`;
};
const shown = () => {
    const q = S.q.trim().toLowerCase();
    return visible().filter((r) => (q ? name(r.item).toLowerCase().includes(q) : r.cat === S.cat && (S.cat !== 0 || r.sub === S.sub)))
        .sort((a, b) => rank(a) - rank(b));
};
const secHtml = () => {
    const xs = shown(), ok = xs.filter((r) => rank(r) === 0).length;
    const title = S.q ? `Поиск — <b>«${S.q}»</b>` : `<b>${CATS[S.cat][0]}</b>`;
    return `<div class="owb-sec">${title}<span>можно: ${ok} из ${xs.length}</span></div>
        ${S.cat === 0 && !S.q ? `<div class="owb-subs">${SUBS.map((n, i) => [n, i, visible().filter((r) => r.cat === 0 && r.sub === i).length]).filter(([, , c]) => c).map(([n, i]) => `<span class="owb-sub${S.sub === i ? ' on' : ''}" data-a="sub" data-v="${i}">${n}</span>`).join('')}</div>` : ''}`;
};
const listHtml = () => shown().map((r) => `<div class="owb-it${r.id === S.sel ? ' on' : ''}${locked(r) ? ' lock' : ''}" data-a="pick" data-v="${r.id}">
    <div class="owb-it__pic"><img src="${img(r.item)}" alt=""></div>
    <div class="owb-it__t"><b>${name(r.item)}</b>${statusHtml(r)}</div>
    <div class="owb-it__tm">${fmtSec(r.sec)}${r.out > 1 ? `<br>×${r.out}` : ''}</div></div>`).join('') || '<div class="owb-qnote">Ничего не нашлось</div>';

// ---------- лист-чертёж ----------
function sheetHtml() {
    const r = recipe(), lock = locked(r), q = S.qty, max = maxOf(r);
    const inQueue = QUEUE.some((x) => x.item === r.item), full = QUEUE.length >= MAX_QUEUE, ok = max >= q;
    const stamp = [['ВРЕМЯ', fmtSec(r.sec)], ['ВЫХОД', `×${r.out}`], ...(r.data ? [['ПРОЧНОСТЬ', fmt(r.data)]] : []), ['УРОВЕНЬ', `${r.lvl} из ${WH.max}`]];
    const lackItems = r.needs.some(([id, n]) => have(id) < n * q);
    const why = full ? 'Очередь заполнена — заберите готовое' : inQueue ? 'Уже на верстаке — заберите или дождитесь' : missingTool(r) ? 'Установите обвес на верстак' : lackItems ? 'Не хватает предметов' : !ok ? 'Не хватит прочности обвеса' : '';
    const ammo = r.ammo && D.wb.find((x) => x.item === r.ammo);
    const bar = lock
        ? `<div class="owb-bar"><div class="owb-tot" style="text-align:left"><small>Рецепт закрыт</small><b style="justify-content:flex-start">Откроется на ${r.lvl} уровне верстака</b></div>
            ${P.owner ? `<div class="owb-go gold" data-a="upgrade">${ico('up', 18)}Улучшить верстак</div>` : '<div class="owb-go off">Улучшает владелец склада</div>'}</div>`
        : `<div class="owb-bar"><div class="owb-stp"><span class="${q <= 1 ? 'off' : ''}" data-a="qty" data-v="-1">−</span><input data-in="qty" value="${q}"><span class="${q >= MAX_QTY ? 'off' : ''}" data-a="qty" data-v="1">+</span></div>
            <div class="owb-chips">${QUICK.map((n) => `<span class="${q === n ? 'on' : ''}" data-a="set" data-v="${n}">×${n}</span>`).join('')}<span class="${max > 0 && q === max ? 'on' : ''}${max > 0 ? '' : ' off'}" data-a="set" data-v="${max}">Макс${max > 0 ? ` ${max}` : ''}</span></div>
            <div class="owb-tot"><small>Получите / время</small><b>×${fmt(r.out * q)} ${ico('clock', 14)}${fmtSec(r.sec * q)}</b></div>
            ${why ? `<div class="owb-go off">${why}</div>` : `<div class="owb-go" data-a="craft">${ico('hammer', 18)}Скрафтить</div>`}</div>`;
    return `<div class="owb-sh"><div class="owb-sh__t"><small>${ico(CATS[r.cat][1], 13)}ЧЕРТЁЖ № ${r.item} — ${(r.cat === 0 ? SUBS[r.sub] : CATS[r.cat][0]).toUpperCase()}</small>
            <b>${name(r.item)}</b><span>${D.items[r.item] ? D.items[r.item][1] : ''}${r.out > 1 ? ` — за 1 крафт ${r.out} шт.` : ''}</span></div>
        <div class="owb-stamp">${stamp.map(([k, v]) => `<div><small>${k}</small><b>${v}</b></div>`).join('')}</div></div>
    <div class="owb-dw${lock ? ' locked' : ''}" id="dw"></div>
    <div class="owb-barw">${ammo && !lock ? `<div class="owb-ammo"><img src="${img(ammo.item)}" alt=""><div class="owb-ammo__t"><b>${name(ammo.item)}</b><small>${fmt(ammo.needs[0][1] * S.ammo)} ${name(ammo.needs[0][0])} — у вас ${fmt(have(ammo.needs[0][0]))}</small></div>
        <div class="owb-stp"><span data-a="ammo" data-v="-1">−</span><input value="${S.ammo}"><span data-a="ammo" data-v="1">+</span></div><span class="owb-ammo__go" data-a="noop">Скрафтить патроны</span></div>` : ''}${bar}</div>`;
}
// поле чертежа: изделие в центре, выноски ингредиентов колонками слева и справа, пунктир к изделию
function drawHtml(W, H) {
    const r = recipe(), q = S.qty, cx = W / 2, tools = r.tools || [], top = tools.length ? TC.row : 0, cy = top + (H - top) * (r.stats ? 0.44 : 0.5);
    const left = Math.ceil(r.needs.length / 2), rows = (k) => Array.from({ length: k }, (_, i) => cy + (i - (k - 1) / 2) * (CO.h + CO.gap));
    const ys = [...rows(left), ...rows(r.needs.length - left)];
    let lines = '', cards = '';
    r.needs.forEach(([id, n], i) => {
        const L = i < left, y = ys[i], x = L ? CO.side : W - CO.side - CO.w, need = n * q, h = have(id), ok = h >= need;
        const px = L ? x + CO.w : x, ang = Math.atan2(y - cy, (px - cx)), ex = cx + Math.cos(ang) * CO.core, ey = cy + Math.sin(ang) * CO.core;
        const bend = (ex - px) / 2;
        lines += `<path class="${ok ? 'ok' : 'no'}" d="M ${px} ${y} C ${px + bend} ${y}, ${ex - bend / 2} ${ey}, ${ex} ${ey}"/><circle class="dot ${ok ? 'ok' : 'no'}" cx="${ex}" cy="${ey}" r="4"/><circle class="dot ${ok ? 'ok' : 'no'}" cx="${px}" cy="${y}" r="3"/>`;
        const src = !ok && !locked(r) && source(id);
        const go = src ? (src[0] === 'wb' ? `<span class="owb-co__go" data-a="pick" data-v="${src[1]}">Сделать${ico('arrow', 12)}</span>` : `<span class="owb-co__go far" title="Делается на заводе">${ico(FACTORY[src[0]][1], 12)}${FACTORY[src[0]][0]}</span>`) : '';
        cards += `<div class="owb-co${ok ? '' : ' no'}" style="left:${x}px;top:${y - CO.h / 2}px"><div class="owb-co__pic"><img src="${img(id)}" alt=""></div>
            <div class="owb-co__t"><b>${name(id)}</b><span>${fmt(h)} <small>/ ${fmt(need)}</small></span></div>${go}</div>`;
    });
    // обвесы — строкой над изделием, пунктир к верху круга
    const tw = Math.min(TC.w, (W - 2 * CO.side - (tools.length - 1) * TC.gap) / tools.length), sm = tw < TC.narrow;
    const tx0 = cx - (tools.length * tw + (tools.length - 1) * TC.gap) / 2;
    tools.forEach(([id, w], i) => {
        const x = tx0 + i * (tw + TC.gap), bx = x + tw / 2, by = TC.top + TC.h, t = toolOf(id), on = installed(id), wear = w * q;
        const st = !on ? 'no' : on.dur < wear ? 'no' : 'tl', ang = Math.atan2(by - cy, bx - cx), ex = cx + Math.cos(ang) * CO.core, ey = cy + Math.sin(ang) * CO.core;
        lines += `<path class="${st}" d="M ${bx} ${by} C ${bx} ${by + 30}, ${ex} ${ey - 30}, ${ex} ${ey}"/><circle class="dot ${st}" cx="${ex}" cy="${ey}" r="4"/>`;
        const inv = !on && INV_TOOLS.find((z) => z.item === id), free = INSTALLED.slice(0, slots()).some((z) => !z);
        const go = !on && !locked(r) ? (inv ? `<span class="owb-co__go${free ? '' : ' far'}"${free ? ` data-a="putInv" data-v="${id}"` : ''}>${free ? (sm ? ico('plus', 14) : 'Установить') : sm ? ico('lock', 12) : 'Нет места'}</span>` : '') : '';
        cards += `<div class="owb-tc ${st}${sm ? ' sm' : ''}" style="left:${x}px;top:${TC.top}px;width:${tw}px"><div class="owb-co__pic"><img src="${img(id)}" alt=""></div>
            <div class="owb-co__t"><b>${name(id)}</b>${on ? `<span>износ ${fmt(wear)} <small>— ${fmt(on.dur)} / ${fmt(t.max)}</small></span>${durBar(on)}` : `<span>${inv ? (sm ? 'в инвентаре' : 'есть в инвентаре') : 'не установлен'}</span>`}</div>${go}</div>`;
    });
    const allOk = maxOf(r) >= q;
    return `<svg class="ln" viewBox="0 0 ${W} ${H}">${lines}</svg>
        <div class="owb-core${allOk && !locked(r) ? ' ok' : ''}" style="left:${cx}px;top:${cy}px"><img src="${img(r.item)}" alt="">${r.out * q > 1 ? `<em>×${fmt(r.out * q)}</em>` : ''}</div>
        ${cards}
        ${r.stats ? `<div class="owb-wst">${STATS.map(([k, n]) => `<div><small>${n}</small><div>${Array.from({ length: STAT_MAX }, (_, i) => `<i class="${i < (r.stats[k] || 0) ? 'on' : ''}"></i>`).join('')}</div></div>`).join('')}</div>` : ''}
        ${locked(r) ? `<div class="owb-seal">Откроется на ${r.lvl} уровне<small>верстака склада</small></div>` : ''}`;
}

// ---------- очередь ----------
function queueHtml() {
    const ready = QUEUE.filter((x) => x.left <= 0).length;
    return `<div class="owb-qh"><div class="owb-qh__t"><small>Очередь</small><b>${QUEUE.length}<em>/ ${MAX_QUEUE}</em></b></div>
        <span class="owb-take${ready ? '' : ' off'}" data-a="takeAll">${ico('bag', 14)}Забрать всё${ready ? ` (${ready})` : ''}</span>
        <span class="owb-x" data-a="noop">${ico('x', 15)}</span></div>
    <div class="owb-qs">${Array.from({ length: MAX_QUEUE }, (_, k) => {
        const q = QUEUE[k];
        if (!q) return '<div class="owb-qi free">Свободное место</div>';
        const done = q.left <= 0;
        return `<div class="owb-qi${done ? ' done' : ''}"${done ? ` data-a="take" data-v="${k}"` : ''}><div class="owb-qi__pic"><img src="${img(q.item)}" alt=""></div>
            <div class="owb-qi__t"><b>${name(q.item)}</b><small>×${fmt(q.count)}${done ? ' — готово' : ''}</small></div>
            <span class="owb-qi__tm">${done ? 'Забрать' : clock(q.left)}</span>
            ${done ? '' : `<div class="owb-qi__bar"><i style="width:${(1 - q.left / q.total) * 100}%"></i></div>`}</div>`;
    }).join('')}</div>
    ${toolsHtml()}
    ${lvHtml()}`;
}

// ---------- обвесы: ячейки в правой колонке и окно места ----------
const slotLevel = (k) => D.toolSlots.findIndex((n) => n > k) + 1;
const durBar = (x, cls = '') => { const pct = Math.floor((x.dur / toolOf(x.item).max) * 100); return `<i class="owb-dur${cls}"><i style="width:${pct}%;background:${durColor(pct)}"></i></i>`; };
function toolsHtml() {
    if (!D.tools.length) return '';   // видов обвесов нет (админ ещё не завёл) — блока нет
    const n = slots(), used = INSTALLED.slice(0, n).filter(Boolean).length;
    return `<div class="owb-tl"><div class="owb-tl__h"><small>Обвесы</small><em>${used} / ${n}</em></div><div class="owb-tl__g">${Array.from({ length: Math.max(...D.toolSlots) }, (_, k) => {
        if (k >= n) return `<div class="owb-slot lk">${ico('lock', 13)}<small>${slotLevel(k)} ур.</small></div>`;
        const x = INSTALLED[k];
        if (!x) return `<div class="owb-slot free" data-a="slot" data-v="${k}">${ico('plus', 18)}</div>`;
        return `<div class="owb-slot" data-a="slot" data-v="${k}"><img src="${img(x.item)}" alt="">${durBar(x)}</div>`;
    }).join('')}</div></div>`;
}
function slotHtml(k) {
    const x = INSTALLED[k];
    if (x) {
        const t = toolOf(x.item), uses = D.wb.filter((r) => (r.tools || []).some(([id]) => id === x.item) && !r.hide && access(r));
        return `<div class="owb-m" data-a="close"><div class="owb-mb" data-a="noop">
            <div class="owb-mh"><span class="owb-mh__ic tl"><img src="${img(x.item)}" alt=""></span><div><small>ОБВЕС — МЕСТО ${k + 1}</small><b>${name(x.item)}</b></div></div>
            <p class="owb-mdesc">${t.desc}</p>
            <div class="owb-durbig"><div><small>Прочность</small><b>${fmt(x.dur)} <em>/ ${fmt(t.max)}</em></b></div>${durBar(x, ' big')}</div>
            <div class="owb-new tl"><small>Нужен в чертежах: ${uses.length}</small><div>${uses.slice(0, NEW_SHOWN).map((r) => `<i title="${name(r.item)}"><img src="${img(r.item)}" alt=""></i>`).join('')}${uses.length > NEW_SHOWN ? `<i class="more">+${uses.length - NEW_SHOWN}</i>` : ''}</div></div>
            <div class="owb-mbtn"><div class="owb-ghost" data-a="close">Закрыть</div><div class="owb-go" data-a="unput" data-v="${k}">${ico('bag', 18)}Снять в инвентарь</div></div>
            <div class="owb-note">Прочность остаётся в предмете — обвес можно поставить на другой верстак</div></div></div>`;
    }
    const others = D.tools.filter((t) => !INV_TOOLS.some((z) => z.item === t.item) && !installed(t.item));
    const row = (id, right, attrs) => `<div class="owb-pk${attrs ? '' : ' off'}"${attrs || ''}><div class="owb-co__pic"><img src="${img(id)}" alt=""></div>
        <div class="owb-pk__t"><b>${name(id)}</b><small>${toolOf(id).desc}</small></div>${right}</div>`;
    return `<div class="owb-m" data-a="close"><div class="owb-mb" data-a="noop">
        <div class="owb-mh"><span class="owb-mh__ic tl">${ico('vise', 28)}</span><div><small>МЕСТО ${k + 1} — СВОБОДНО</small><b>Установить обвес</b></div></div>
        <div class="owb-ml">Из инвентаря<em>прочность</em></div>
        <div class="owb-pks">${INV_TOOLS.map((z, i) => row(z.item, `<div class="owb-pk__d"><b>${fmt(z.dur)} <em>/ ${fmt(toolOf(z.item).max)}</em></b>${durBar(z)}</div>`, ` data-a="put" data-v="${k}|${i}"`)).join('') || '<div class="owb-note">В инвентаре нет обвесов</div>'}</div>
        ${others.length ? `<div class="owb-ml">Другие обвесы<em>нет в инвентаре</em></div><div class="owb-pks">${others.map((t) => row(t.item, '')).join('')}</div>` : ''}
        <div class="owb-mbtn one"><div class="owb-ghost" data-a="close">Отмена</div></div></div></div>`;
}

// ---------- улучшение верстака ----------
function modalHtml() {
    const opens = D.wb.filter((r) => r.lvl === NEXT.lvl && !r.hide && access(r));
    const item = ([id, n]) => `<div class="${have(id) < n ? 'lack' : ''}"><small>${name(id)}</small><b>${fmt(have(id))} <em>/ ${fmt(n)}</em></b></div>`;
    const ok = WALLET.money >= NEXT.money && WALLET.coins >= NEXT.coins && NEXT.items.every(([id, n]) => have(id) >= n);
    return `<div class="owb-m" data-a="close"><div class="owb-mb" data-a="noop">
        <div class="owb-mh"><span class="owb-mh__ic">${ico('up', 30)}</span><div><small>УЛУЧШЕНИЕ ВЕРСТАКА</small><b>${WH.lvl} уровень ${ico('arrow', 20)} ${NEXT.lvl} уровень</b></div></div>
        <div class="owb-new"><small>Откроются чертежи: ${opens.length}${D.tools.length && D.toolSlots[NEXT.lvl - 1] > slots() ? ` — и +${D.toolSlots[NEXT.lvl - 1] - slots()} место для обвеса` : ''}</small><div>${opens.slice(0, NEW_SHOWN).map((r) => `<i title="${name(r.item)}"><img src="${img(r.item)}" alt=""></i>`).join('')}${opens.length > NEW_SHOWN ? `<i class="more">+${opens.length - NEW_SHOWN}</i>` : ''}</div></div>
        <div class="owb-ml">Стоимость<em>у вас / нужно</em></div>
        <div class="owb-cost"><div class="${WALLET.money < NEXT.money ? 'lack' : ''}"><small>Наличные</small><b>$ ${fmt(NEXT.money)}</b><em>у вас $ ${fmt(WALLET.money)}</em></div>
            <div class="${WALLET.coins < NEXT.coins ? 'lack' : ''}"><small>Oasis Coin</small><b>${fmt(NEXT.coins)} Coin</b><em>у вас ${fmt(WALLET.coins)} Coin</em></div>
            ${NEXT.items.map(item).join('')}</div>
        <div class="owb-mbtn"><div class="owb-ghost" data-a="close">Отмена</div><div class="owb-go gold${ok ? '' : ' off'}" data-a="close">${ok ? `${ico('up', 18)}Улучшить` : 'Не хватает ресурсов'}</div></div>
        <div class="owb-note">Деньги и предметы списываются у владельца склада</div></div></div>`;
}

// ---------- отрисовка по частям ----------
const keepScroll = (sel, fn) => { const el = $(sel), t = el ? el.scrollTop : 0; fn(); const n = $(sel); if (n) n.scrollTop = t; };
const draw = {
    list: () => { $('#cats').innerHTML = catsHtml(); $('#sec').innerHTML = secHtml(); keepScroll('#list', () => ($('#list').innerHTML = listHtml())); },
    sheet: () => { $('#sheet').innerHTML = sheetHtml(); const dw = $('#dw'); dw.innerHTML = drawHtml(dw.clientWidth, dw.clientHeight); },
    queue: () => ($('#queue').innerHTML = queueHtml()),
    modal: () => ($('#modal').innerHTML = !S.modal ? '' : S.modal === 'up' ? modalHtml() : slotHtml(S.modal.slot)),
};
const all = () => Object.values(draw).forEach((f) => f());
const redraw = () => { draw.list(); draw.sheet(); draw.queue(); draw.modal(); };
const pickFirst = () => { const r = shown()[0]; S.sel = r ? r.id : S.sel; S.qty = 1; };

const A = {
    noop: () => {},
    cat: (v) => { S.cat = +v; S.sub = 0; S.q = ''; $('#q').value = ''; pickFirst(); draw.list(); draw.sheet(); },
    sub: (v) => { S.sub = +v; pickFirst(); draw.list(); draw.sheet(); },
    pick: (v) => { const r = D.wb[+v]; if (S.sel === +v) return; S.sel = +v; S.qty = 1; if (!S.q) { S.cat = r.cat; S.sub = r.sub || 0; } draw.list(); draw.sheet(); },
    qty: (d) => { S.qty = Math.max(1, Math.min(MAX_QTY, S.qty + +d)); draw.sheet(); },
    set: (n) => { S.qty = Math.max(1, +n); draw.sheet(); },
    ammo: (d) => { S.ammo = Math.max(AMMO_STEP, S.ammo + +d * AMMO_STEP); draw.sheet(); },
    craft: () => {
        const r = recipe(); QUEUE = [...QUEUE, { item: r.item, count: r.out * S.qty, left: r.sec * S.qty, total: r.sec * S.qty }];
        (r.tools || []).forEach(([id, w]) => { const x = installed(id); x.dur -= w * S.qty; if (x.dur <= 0) INSTALLED[INSTALLED.indexOf(x)] = null; });   // на нуле обвес исчезает
        redraw();
    },
    take: (k) => { QUEUE = QUEUE.filter((_, i) => i !== +k); draw.queue(); draw.sheet(); },
    takeAll: () => { QUEUE = QUEUE.filter((x) => x.left > 0); draw.queue(); draw.sheet(); },
    upgrade: () => { S.modal = 'up'; draw.modal(); },
    slot: (k) => { S.modal = { slot: +k }; draw.modal(); },
    put: (v) => { const [k, i] = v.split('|').map(Number); INSTALLED[k] = INV_TOOLS[i]; INV_TOOLS = INV_TOOLS.filter((_, j) => j !== i); S.modal = false; redraw(); },
    putInv: (id) => { const k = INSTALLED.slice(0, slots()).findIndex((z) => !z), i = INV_TOOLS.findIndex((z) => z.item === +id); A.put(`${k}|${i}`); },
    unput: (k) => { INV_TOOLS = [...INV_TOOLS, INSTALLED[+k]]; INSTALLED[+k] = null; S.modal = false; redraw(); },
    close: () => { S.modal = false; draw.modal(); },
};
$('#app').addEventListener('click', (e) => { const t = e.target.closest('[data-a]'); if (t && A[t.dataset.a]) A[t.dataset.a](t.dataset.v); });
$('#app').addEventListener('input', (e) => {
    if (e.target.id === 'q') { S.q = e.target.value; draw.list(); return; }
    if (e.target.dataset.in !== 'qty') return;
    const v = parseInt(e.target.value.replace(/\D/g, ''), 10); S.qty = Math.max(1, Math.min(MAX_QTY, v || 1));
    draw.sheet(); const inp = $('#sheet [data-in="qty"]'); inp.focus(); inp.setSelectionRange(inp.value.length, inp.value.length);
});

// ---------- состояния макета ----------
const byItem = (id) => D.wb.find((r) => r.item === id).id;
// макет: лейке нужно N обвесов (первые N из списка; 2 стоят на верстаке, 2 — в инвентаре)
const LEIKA = byItem(525), LEIKA_TOOLS = D.wb[LEIKA].tools;
// макет: «обвесов нет» — так окно выглядит, пока админ не завёл ни одного обвеса
const TOOLS_ALL = D.tools, RECIPE_TOOLS = D.wb.map((r) => r.tools);
const toolsN = (n) => { S.cat = 1; S.sel = LEIKA; D.wb[LEIKA].tools = D.tools.slice(0, n).map((t) => [t.item, 1]); };
const MOCK = {
    tool: () => { S.cat = 1; S.sel = byItem(525); },
    weapon: () => { S.cat = 0; S.sub = 0; S.sel = byItem(100); },
    lack: () => { INV = INV_LACK; S.cat = 1; S.sel = byItem(525); S.qty = 2; },
    locked: () => { S.cat = 1; S.sel = byItem(518); },
    upgrade: () => { S.cat = 1; S.sel = byItem(525); S.modal = 'up'; },
    needtool: () => { S.cat = 2; S.sel = byItem(521); },
    put: () => { S.cat = 1; S.sel = byItem(525); S.modal = { slot: 2 }; },
    toolinfo: () => { S.cat = 1; S.sel = byItem(525); S.modal = { slot: 0 }; },
    notools: () => { S.cat = 1; S.sel = LEIKA; D.tools = []; D.wb.forEach((r) => (r.tools = undefined)); },
    t1: () => toolsN(1), t2: () => toolsN(2), t3: () => toolsN(3), t4: () => toolsN(4),
    full: () => { QUEUE = [...QUEUE_BASE, { item: 516, count: 2, left: 40, total: 60 }, { item: 486, count: 5, left: 300, total: 500 }]; S.cat = 3; S.sel = byItem(501); },
    member: () => { P.owner = false; S.cat = 1; S.sel = byItem(518); },
};
$$('.mock [data-m]').forEach((b) => (b.onclick = () => {
    $$('.mock [data-m]').forEach((x) => x.classList.toggle('a', x === b));
    INV = INV_FULL; QUEUE = QUEUE_BASE; P.owner = true; D.tools = TOOLS_ALL; D.wb.forEach((r, i) => (r.tools = RECIPE_TOOLS[i])); D.wb[LEIKA].tools = LEIKA_TOOLS; INSTALLED = INSTALLED_BASE.map((x) => x && { ...x }); INV_TOOLS = INV_TOOLS_BASE.map((x) => ({ ...x })); S.modal = false; S.qty = 1; S.sub = 0; S.q = ''; $('#q').value = '';
    MOCK[b.dataset.m](); fixSel(); all();
}));
// выбранный рецепт пропал из доступных (сменился доступ) — первый доступный в разделе или в первом непустом разделе
const fixSel = () => {
    const vis = visible();
    if (vis.some((r) => r.id === S.sel)) return;
    if (!vis.some((r) => r.cat === S.cat)) S.cat = vis[0].cat;
    if (S.cat === 0 && !vis.some((r) => r.cat === 0 && r.sub === S.sub)) S.sub = vis.find((r) => r.cat === 0).sub;
    pickFirst();
};
$$('.mock [data-lv]').forEach((b) => (b.onclick = () => {
    WH.lvl = +b.dataset.lv; NEXT = UPGRADES[WH.lvl + 1];
    $$('.mock [data-lv]').forEach((x) => x.classList.toggle('a', x === b));
    if (S.modal === 'up' && !NEXT) S.modal = false;
    fixSel(); all();
}));
$('.mock [data-t="frac"]').onclick = (e) => {
    const b = e.currentTarget;
    P.frac = P.frac ? 0 : MOCK_FRAC[0];
    b.classList.toggle('a', !!P.frac); b.textContent = P.frac ? `Во фракции: ${MOCK_FRAC[1]}` : 'Не во фракции';
    fixSel(); all();
};
$('#app').innerHTML = skeleton();
MOCK.tool(); all();
const fit = () => ($('#stage').style.transform = `translate(-50%,-50%) scale(${Math.min(innerWidth / 1920, innerHeight / 1080)})`);
addEventListener('resize', fit); fit();
