// Порт INT_UIF_Draw / INT_UIF_Surface / INT_PDA_Look: те же вызовы и те же координаты, что в Enforce-коде.
// Поэтому правка в макете переносится в мод почти один к одному.
const C = {BG:'#0B0B0C',PANEL:'#131315',PANEL2:'#19191C',LINE:'#2A2A2F',LINE2:'#3A3A40',TEXT:'#ECE8E1',DIM:'#8C8982',DIM2:'#5F5D58',
  ORANGE:'#FF7A1A',ORANGE2:'#C85A0A',OWN:'#FF9A4D',ALLY:'#6FD0A0',NEUTRAL:'#B5B2AB',ENEMY:'#EC4A3C',ADMIN:'#8F7B66',ON_ORANGE:'#0B0B0C',WHITE:'#FFFFFF',
  PLATE:'rgba(14,14,16,.92)',SOS_RED:'#EC6A5C',SOS_BORDER:'#5A2B27'};
const LOOK = {DEV_W:1500,DEV_H:890,SCR_X:22,SCR_Y:24,SCR_W:1456,SCR_H:800,TOP_H:88,FOOT_H:38,BODY_H:674};
const T = k => STR[k] !== undefined ? STR[k] : k;
const Caps = k => T(k).toUpperCase();
const CapsT = s => (s || '').toUpperCase();

const _cv = document.createElement('canvas').getContext('2d');
function TextW(s, size){ _cv.font = size + 'px Metron'; return _cv.measureText(s).width; }
function Fit(s, width, size){
  if (!s || TextW(s, size) <= width) return s;
  let n = s.length;
  while (n > 1 && TextW(s.slice(0, n) + '…', size) > width) n--;
  return s.slice(0, n) + '…';
}
function _mk(p, cls, x, y, w, h){
  const e = document.createElement('div'); e.className = cls;
  const s = e.style; s.left = x + 'px'; s.top = y + 'px';
  if (w != null) s.width = w + 'px';
  if (h != null) s.height = h + 'px';
  if (p) p.appendChild(e);
  return e;
}
const D = {
  Frame(p,x,y,w,h,name){ const e=_mk(p,'a',x,y,w,h); if(name) e.dataset.w=name; return e; },
  Clip(p,x,y,w,h,name){ const e=_mk(p,'clip',x,y,w,h); if(name) e.dataset.w=name; return e; },
  Text(p,x,y,w,h,s,size,color,align){ const e=_mk(p,'t'+(align?' '+align:''),x,y,w,h); e.style.fontSize=size+'px'; e.style.color=color; e.textContent=s; return e; },
  MText(p,x,y,w,h,s,size,color){ const e=_mk(p,'mt',x,y,w,h); e.style.fontSize=size+'px'; e.style.color=color; e.textContent=s; return e; },
  Line(p,x,y,w,h,color){ const e=_mk(p,'box',x,y,w,h); e.style.background=color; return e; },
  Box(p,x,y,w,h,r,fill,border){ const e=_mk(p,'box tr',x,y,w,h); e.style.borderRadius=r+'px'; e.style.background=fill||'transparent'; if(border) e.style.border='1px solid '+border; return e; },
  Pill(p,x,y,w,h,fill,border){ return D.Box(p,x,y,w,h,h/2,fill,border); },
  Circle(p,x,y,s,fill){ return D.Box(p,x,y,s,s,s/2,fill); },
  Img(p,x,y,w,h,url){ const e=_mk(p,'img',x,y,w,h); e.style.backgroundImage='url('+url+')'; return e; },
  Icon(p,x,y,s,name,color){ const e=_mk(p,'ic i-'+name,x,y,s,s); e.style.color=color; return e; },
  Hit(p,x,y,w,h,fn,hover){
    const e=_mk(p,'hit',x,y,w,h);
    if(fn) e.onclick=ev=>{ev.stopPropagation();fn(ev);};
    if(hover){e.onmouseenter=()=>hover(true);e.onmouseleave=()=>hover(false);}
    return e;
  },
  Avatar(p,x,y,s,i){ return D.Img(p,x,y,s,s,'assets/avatars/a'+String(Math.max(0,Math.min(72,i|0))).padStart(2,'0')+'.png'); },
  Emblem(p,x,y,s,i){ return D.Img(p,x,y,s,s,'assets/emblems/q'+String(Math.max(1,Math.min(45,i|0))).padStart(2,'0')+'.png'); },
};

const Look = {
  // Вертикальная карточка группировки: эмблема и название (INT_PDA_Look.DrawFactionCard).
  FactionCard(p,x,y,w,emblem,name){
    const h=w*4/3, card=D.Frame(p,x,y,w,h,'Card');
    D.Box(card,0,0,w,h,6,C.PANEL2,C.LINE2);
    const pad=w*.06, ty=pad+(w-2*pad), fs=Math.max(13,w*.11);
    D.Emblem(card,pad,pad,w-2*pad,emblem);
    D.Text(card,4,ty,w-8,h-ty,Fit(CapsT(name),w-8,fs),fs,C.TEXT,'c');
    return card;
  },
  // Анонимный аватар: чёрная плашка «НЕТ ДАННЫХ».
  Anon(p,x,y,s){
    const f=D.Frame(p,x,y,s,s,'Anon'); D.Box(f,0,0,s,s,4,'#000',C.LINE2);
    const ic=s*.42, txt=T('NO_DATA'); let fs=s*.16; const tw=TextW(txt,fs);
    if (tw>s-6) fs=fs*(s-6)/tw;
    if (fs<9){ D.Icon(f,(s-ic)/2,(s-ic)/2,ic,'eye_off',C.DIM); return f; }
    D.Icon(f,(s-ic)/2,s*.16,ic,'eye_off',C.DIM);
    D.Text(f,0,s*.62,s,fs+4,txt,fs,C.DIM2,'c');
    return f;
  },
  Panel(p,x,y,w,h,cap){
    const f=D.Frame(p,x,y,w,h,'Panel'); D.Box(f,0,0,w,h,6,C.PANEL,C.LINE);
    if(cap) D.Text(f,18,12,w-36,18,Caps(cap),13,C.DIM);
    return f;
  },
  Label(p,x,y,w,key){ return D.Text(p,x,y,w,18,Caps(key),13,C.DIM); },
  // «Нет модуля / нет доступа» посреди страницы.
  Locked(p,w,h,icon,title,text){
    const f=D.Frame(p,0,0,w,h,'Locked'), cy=h*.5-90;
    D.Icon(f,w*.5-32,cy,64,icon,C.DIM2);
    D.Text(f,0,cy+84,w,30,Caps(title),22,C.TEXT,'c');
    D.Text(f,0,cy+124,w,24,T(text),16,C.DIM,'c');
    return f;
  },
  RelColor(fid){
    const me=Data.myFaction();
    if(!fid||!me) return C.NEUTRAL;
    if(fid===me.Id) return C.OWN;
    const r=Data.relation(me.Id,fid);
    return r===1?C.ALLY:r===2?C.ENEMY:C.NEUTRAL;
  },
  RoleKey(r){ return r===3?'ROLE_LEADER':r===2?'ROLE_DEPUTY':r===1?'ROLE_MEMBER':'ROLE_NONE'; },
  Reveal(w,delay=0,dy=14,dur=340){
    if(!w||window.NOANIM) return;
    w.animate([{opacity:0,transform:`translateY(${dy}px)`},{opacity:1,transform:'none'}],{duration:dur,delay:delay*1000,fill:'backwards',easing:'cubic-bezier(.22,.61,.36,1)'});
  },
};

// Компоненты INT_UIF: кнопка, поле, тумблер, флажок, список.
const UI = {
  Button(p,x,y,w,h,text,variant,icon,fn,fs=15){
    const e=_mk(p,'btn '+(variant||'secondary'),x,y,w,h); e.style.fontSize=fs+'px';
    if(icon){ const i=document.createElement('div'); i.className='ic i-'+icon; e.appendChild(i); }
    const s=document.createElement('span'); s.textContent=text; e.appendChild(s); e.label=s;
    if(fn) e.onclick=ev=>{ev.stopPropagation();fn();};
    return e;
  },
  Input(p,x,y,w,h,ph,max,icon,val){
    const f=_mk(p,'inp',x,y,w,h);
    if(icon){ const i=document.createElement('div'); i.className='ic i-'+icon; f.appendChild(i); }
    const n=document.createElement('input'); n.placeholder=ph||'';
    if(max) n.maxLength=max;
    if(val) n.value=val;
    f.appendChild(n); f.input=n;
    return f;
  },
  Pass(p,x,y,w,h,val){
    const f=UI.Input(p,x,y,w,h,'',32,null,val); f.input.type='password'; f.input.style.paddingRight='8px';
    const eye=document.createElement('div'); eye.className='ic i-eye';
    eye.style.cssText='position:static;margin:0 16px;width:22px;height:22px;cursor:pointer;pointer-events:auto;color:#8C8982;flex:none';
    f.appendChild(eye);
    eye.onclick=()=>{ const open=f.input.type==='password'; f.input.type=open?'text':'password'; eye.className='ic '+(open?'i-eye_off':'i-eye'); eye.style.color=open?C.ORANGE:C.DIM; };
    return f;
  },
  // Переключатель КПК: 46x24, ручка 18.
  Switch(p,x,y,on,fn){
    const t=_mk(p,'sw'+(on?' on':''),x,y,46,24); t.style.background='#2B2B30'; t.appendChild(document.createElement('i'));
    if(fn){ t.style.cursor='pointer'; t.onclick=ev=>{ev.stopPropagation();const v=!t.classList.contains('on');t.classList.toggle('on',v);fn(v);}; }
    return t;
  },
  // INT_UIF_Toggle: подпись слева, дорожка справа.
  Toggle(p,x,y,w,text,on,fn){
    const f=D.Frame(p,x,y,w,32,'Toggle'); D.Text(f,0,0,w-52,32,text,14,C.TEXT);
    f.sw=UI.Switch(f,w-46,4,on,fn);
    return f;
  },
  Check(p,x,y,w,text,on,fn){
    const f=D.Frame(p,x,y,w,30,'Check'), b=D.Box(f,0,5,20,20,6,on?C.ORANGE:'#19191C',on?C.ORANGE:C.LINE2), m=D.Icon(f,3,8,14,'check',C.BG);
    m.style.display=on?'':'none'; D.Text(f,32,0,w-32,30,text,14,C.TEXT); f.style.cursor='pointer';
    let v=on;
    f.onclick=()=>{v=!v;b.style.background=v?C.ORANGE:'#19191C';b.style.borderColor=v?C.ORANGE:C.LINE2;m.style.display=v?'':'none';if(fn)fn(v);};
    return f;
  },
  Dropdown(p,x,y,w,opts,sel,h=44,fn){
    const e=_mk(p,'dd',x,y,w,h), s=document.createElement('span'), a=document.createElement('div');
    a.className='ic i-chevron_down'; a.style.cssText='position:static;width:14px;height:14px;color:#8C8982'; e.append(s,a);
    const set=i=>{e.sel=i;s.textContent=opts[i]!==undefined?Fit(opts[i],w-50,14):'';};
    set(sel);
    e.onclick=ev=>{
      ev.stopPropagation();
      const open=document.querySelector('.ddp'); if(open) open.remove();
      const r=_mk(p,'ddp',x,y+h+4,w);
      opts.forEach((o,i)=>{const d=document.createElement('div');d.textContent=o;if(i===e.sel)d.className='on';d.onclick=e2=>{e2.stopPropagation();set(i);r.remove();if(fn)fn(i);};r.appendChild(d);});
    };
    return e;
  },
  // Диалог подтверждения (INT_PDA_PageAdminConfirm и окна страниц).
  Ask(root,o){
    const W=o.W||LOOK.SCR_W, H=o.H||LOOK.BODY_H, cw=o.cw||620, ch=o.ch||240, m=D.Frame(root,0,0,W,H,'Ask');
    m.style.zIndex=60; D.Line(m,0,0,W,H,'rgba(0,0,0,.67)'); D.Hit(m,0,0,W,H);
    const b=D.Frame(m,(W-cw)/2,(H-ch)/2,cw,ch,'AskBox'); b.style.boxShadow='0 16px 44px rgba(0,0,0,.63)';
    D.Box(b,0,0,cw,ch,8,C.PANEL,C.LINE2); D.Line(b,0,0,cw,3,o.danger?C.ENEMY:C.ORANGE);
    D.Text(b,28,22,cw-56,28,CapsT(o.title),20,C.TEXT);
    if(o.value) D.Text(b,28,58,cw-56,36,o.value,28,C.ORANGE);
    D.MText(b,28,102,cw-56,70,o.text||'',15,C.DIM);
    const close=()=>m.remove();
    UI.Button(b,cw-28-300,ch-64,140,44,o.no||T('CANCEL'),'secondary',null,()=>{close();if(o.onNo)o.onNo();});
    UI.Button(b,cw-28-150,ch-64,150,44,o.yes||T('OK'),o.danger?'danger':'primary',o.yesIcon||'check',()=>{close();if(o.onYes)o.onYes();});
    Look.Reveal(b,0,12,220);
    return m;
  },
};

// Тосты INT_UIF.
function Toast(text,kind='info'){
  const col={ok:C.ALLY,err:C.ENEMY,warn:C.OWN,info:C.NEUTRAL}[kind], ic={ok:'check',err:'warning',warn:'warning',info:'note'}[kind];
  const st=document.getElementById('stage'), e=document.createElement('div'); e.className='toast';
  e.style.top=(30+st.querySelectorAll('.toast').length*68)+'px'; e.style.borderLeftColor=col;
  const i=document.createElement('div'); i.className='ic i-'+ic; i.style.color=col;
  const s=document.createElement('span'); s.textContent=text; e.append(i,s); st.appendChild(e);
  e.animate([{opacity:0,transform:'translateX(30px)'},{opacity:1,transform:'none'}],{duration:220});
  setTimeout(()=>{e.style.opacity=0;setTimeout(()=>e.remove(),260);},2600);
}

// Заглушка 3D-модели игрока: в игре здесь PlayerPreviewWidget. Перетаскивание вращает её.
function Silhouette(p,x,y,w,h){
  const f=D.Frame(p,x,y,w,h,'Preview');
  f.innerHTML=`<svg viewBox="0 0 200 560" width="${w}" height="${h}" preserveAspectRatio="xMidYMid meet" style="position:absolute;left:0;top:0">
<defs><linearGradient id="sg" x1="0" y1="0" x2="1" y2="1"><stop offset="0" stop-color="#3f3f44"/><stop offset="1" stop-color="#17171a"/></linearGradient></defs>
<g fill="url(#sg)" stroke="#4a4a50" stroke-width="1.5"><circle cx="100" cy="44" r="27"/><path d="M66 82q34-16 68 0l22 34 8 134-22 4-10-112-3 96 8 150 6 142h-38l-5-168h-8l-5 168H53l6-142 8-150-3-96-10 112-22-4 8-134z"/></g></svg>`;
  const svg=f.firstChild; let yaw=0, drag=false, lx=0;
  f.style.cursor='grab';
  f.onmousedown=e=>{drag=true;lx=e.clientX;f.style.cursor='grabbing';};
  const up=()=>{drag=false;f.style.cursor='grab';};
  const mv=e=>{if(!drag)return;yaw+=(e.clientX-lx)*.6;lx=e.clientX;svg.style.transform=`perspective(900px) rotateY(${yaw}deg)`;};
  window.addEventListener('mouseup',up); window.addEventListener('mousemove',mv);
  return f;
}
