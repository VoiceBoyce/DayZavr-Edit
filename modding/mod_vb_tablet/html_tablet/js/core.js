// Хелперы отрисовки: абсолютные координаты в пикселях холста 1920x1080, как INT_UIF_Draw в игре.
const C={BG:'var(--bg)',SIDE:'var(--side)',PANEL:'var(--panel)',PANEL2:'var(--panel2)',PANEL3:'var(--panel3)',LINE:'var(--line)',LINE2:'var(--line2)',TEXT:'var(--text)',DIM:'var(--dim)',DIM2:'var(--dim2)',ACCENT:'var(--accent)',ON:'var(--on)',GOLD:'var(--gold)',INF:'var(--inf)',OK:'var(--ok)',BAD:'var(--bad)'};
const _cv=document.createElement('canvas').getContext('2d');
function TextW(s,size){_cv.font=size+'px Metron';return _cv.measureText(s).width;}
function Fit(s,w,size){if(!s||TextW(s,size)<=w)return s;let n=s.length;while(n>1&&TextW(s.slice(0,n)+'…',size)>w)n--;return s.slice(0,n)+'…';}
const Num=n=>Math.round(n).toLocaleString('ru-RU').replace(/ /g,' ');
function _mk(p,cls,x,y,w,h){const e=document.createElement('div');e.className=cls;const s=e.style;s.left=x+'px';s.top=y+'px';if(w!=null)s.width=w+'px';if(h!=null)s.height=h+'px';if(p)p.appendChild(e);return e;}
const D={
  Frame(p,x,y,w,h){return _mk(p,'a',x,y,w,h);},
  Clip(p,x,y,w,h){return _mk(p,'clip',x,y,w,h);},
  Text(p,x,y,w,h,s,size,color,align){const e=_mk(p,'t'+(align?' '+align:''),x,y,w,h);e.style.fontSize=size+'px';e.style.color=color;e.textContent=s;return e;},
  MText(p,x,y,w,h,s,size,color){const e=_mk(p,'mt',x,y,w,h);e.style.fontSize=size+'px';e.style.color=color;e.textContent=s;return e;},
  Cap(p,x,y,w,s,color){return D.Text(p,x,y,w,18,s.toUpperCase(),13,color||C.DIM);},
  Line(p,x,y,w,h,color){const e=_mk(p,'box',x,y,w,h);e.style.background=color;return e;},
  Box(p,x,y,w,h,r,fill,border){const e=_mk(p,'box',x,y,w,h);e.style.borderRadius=r+'px';e.style.background=fill||'transparent';if(border)e.style.border='1px solid '+border;return e;},
  Circle(p,x,y,s,fill){return D.Box(p,x,y,s,s,s/2,fill);},
  Icon(p,x,y,s,name,color){const e=_mk(p,'ic i-'+name,x,y,s,s);e.style.color=color;return e;},
  Hit(p,x,y,w,h,fn,hover){const e=_mk(p,'hit',x,y,w,h);if(fn)e.onclick=v=>{v.stopPropagation();fn(v);};if(hover){e.onmouseenter=()=>hover(true);e.onmouseleave=()=>hover(false);}return e;},
  Bar(p,x,y,w,h,k,color){const b=_mk(p,'bar',x,y,w,h);b.style.borderRadius=h/2+'px';const i=document.createElement('i');i.style.width=Math.max(0,Math.min(1,k))*100+'%';i.style.background=color||C.ACCENT;b.appendChild(i);return b;},
};
const UI={
  Card(p,x,y,w,h,cap){const f=D.Frame(p,x,y,w,h);D.Box(f,0,0,w,h,10,C.PANEL,C.LINE);if(cap)D.Cap(f,20,16,w-40,cap);return f;},
  Button(p,x,y,w,h,text,variant,icon,fn,fs=15){const e=_mk(p,'btn '+(variant||'secondary'),x,y,w,h);e.style.fontSize=fs+'px';if(icon){const i=document.createElement('div');i.className='ic i-'+icon;e.appendChild(i);}if(text){const s=document.createElement('span');s.textContent=text;e.appendChild(s);e.label=s;}if(fn)e.onclick=v=>{v.stopPropagation();fn();};return e;},
  Input(p,x,y,w,h,ph,max,val){const f=_mk(p,'inp',x,y,w,h),n=document.createElement('input');n.placeholder=ph||'';if(max)n.maxLength=max;if(val)n.value=val;f.appendChild(n);f.input=n;return f;},
  // Многострочное поле, в игре MultilineEditBoxWidget.
  Area(p,x,y,w,h,ph,max,val){const f=_mk(p,'inp area',x,y,w,h),n=document.createElement('textarea');n.placeholder=ph||'';if(max)n.maxLength=max;if(val)n.value=val;f.appendChild(n);f.input=n;return f;},
  Switch(p,x,y,on,fn){const t=_mk(p,'sw'+(on?' on':''),x,y);t.appendChild(document.createElement('i'));t.onclick=v=>{v.stopPropagation();const o=!t.classList.contains('on');t.classList.toggle('on',o);if(fn)fn(o);};return t;},
  // Сумма: монета и число золотом.
  Money(p,x,y,val,size,color){const c=color||C.GOLD,s=Math.round(size*.95);D.Icon(p,x,y+(size*1.3-s)/2,s,'coin',c);return D.Text(p,x+s+Math.round(size*.35),y,TextW(Num(val),size)+6,size*1.3,Num(val),size,c);},
  // Сумма с выравниванием по правому краю: right это правая граница числа.
  MoneyR(p,right,y,val,size,color){const s=Math.round(size*.95);return UI.Money(p,right-TextW(Num(val),size)-s-Math.round(size*.35),y,val,size,color);},
  // Плитка показателя: подпись и значение.
  Tile(p,x,y,w,h,cap,val,icon,color){const f=UI.Card(p,x,y,w,h);D.Cap(f,20,18,w-64,cap);if(icon)D.Icon(f,w-42,16,22,icon,C.DIM2);const size=h>=110?30:26;return D.Text(f,20,h-size-24,w-40,size+8,val,size,color||C.TEXT);},
  // Эмблема клана: форма + символ, в игре это два тонированных ImageWidget.
  Emblem(p,x,y,s,e){const f=D.Frame(p,x,y,s,s);D.Icon(f,0,0,s,'shp_'+e.shape,e.color);const k=s*.52;D.Icon(f,(s-k)/2,(s-k)/2-(e.shape==='banner'?s*.05:0),k,'sym_'+e.symbol,'#0E0D0C');return f;},
  // Выпадающий список; up раскрывает его вверх, если снизу нет места.
  Dropdown(p,x,y,w,opts,sel,h,fn,up){
    const e=_mk(p,'dd',x,y,w,h),s=document.createElement('span'),a=document.createElement('div');a.className='ic i-chevron_down';a.style.cssText='position:static;width:14px;height:14px;color:var(--dim);flex:none';e.append(s,a);
    const set=i=>{e.sel=i;s.textContent=opts[i]!==undefined?Fit(opts[i],w-50,14):'';};set(sel);
    e.onclick=v=>{v.stopPropagation();const o=document.querySelector('.ddp');if(o)o.remove();const r=_mk(p,'ddp',x,up?y-opts.length*36-16:y+h+4,w);opts.forEach((t,i)=>{const d=document.createElement('div');d.textContent=t;if(i===e.sel)d.className='on';d.onclick=q=>{q.stopPropagation();set(i);r.remove();if(fn)fn(i);};r.appendChild(d);});};
    return e;
  },
  // Диалог с подтверждением.
  Ask(o){const r=Shell.screen,W=1468,H=848,w=520,h=o.h||220,m=D.Frame(r,0,0,W,H);m.style.zIndex=60;D.Line(m,0,0,W,H,'rgba(0,0,0,.6)');D.Hit(m,0,0,W,H);
    const b=D.Frame(m,(W-w)/2,(H-h)/2,w,h);b.style.boxShadow='0 16px 44px rgba(0,0,0,.6)';D.Box(b,0,0,w,h,12,C.PANEL,C.LINE2);D.Text(b,28,24,w-56,28,o.title,22,C.TEXT);D.MText(b,28,66,w-56,h-140,o.text||'',15,C.DIM);
    const x=()=>m.remove();UI.Button(b,w-28-300,h-68,140,44,'Отмена','secondary',null,x);UI.Button(b,w-28-150,h-68,150,44,o.yes||'Да',o.danger?'danger':'primary',null,()=>{x();o.fn&&o.fn();});return m;},
  // Окно поверх экрана: o.pop хранит его, o.closePop закрывает.
  Modal(o,w,h){if(o.pop)o.closePop();const W=1468,H=848,m=o.pop=D.Frame(Shell.screen,0,0,W,H);m.style.zIndex=60;D.Line(m,0,0,W,H,'rgba(0,0,0,.6)');D.Hit(m,0,0,W,H,()=>o.closePop());const b=D.Frame(m,(W-w)/2,(H-h)/2,w,h);b.style.boxShadow='0 16px 44px rgba(0,0,0,.6)';D.Box(b,0,0,w,h,12,C.PANEL,C.LINE2);const ci=D.Icon(b,w-46,26,20,'close',C.DIM);D.Hit(b,w-54,18,36,36,()=>o.closePop(),v=>ci.style.color=v?C.TEXT:C.DIM);return b;},
  // Опасное действие: кнопка откроется только после ввода точного слова.
  Confirm(o){const r=Shell.screen,W=1468,H=848,w=560,h=316,m=D.Frame(r,0,0,W,H);m.style.zIndex=60;D.Line(m,0,0,W,H,'rgba(0,0,0,.6)');D.Hit(m,0,0,W,H);
    const b=D.Frame(m,(W-w)/2,(H-h)/2,w,h);b.style.boxShadow='0 16px 44px rgba(0,0,0,.6)';D.Box(b,0,0,w,h,12,C.PANEL,C.BAD);D.Text(b,28,22,w-56,32,o.title,22,C.TEXT);D.MText(b,28,62,w-56,70,o.text,15,C.DIM);
    D.Text(b,28,140,w-56,22,Fit('Введите для подтверждения: '+o.word,w-56,14),14,C.DIM2);const i=UI.Input(b,28,166,w-56,44,o.word,40);const x=()=>m.remove();UI.Button(b,w-28-300,h-68,140,44,'Отмена','secondary',null,x);const y=UI.Button(b,w-28-150,h-68,150,44,o.yes||'Да','danger',null,()=>{x();o.fn&&o.fn();});y.classList.add('off');i.input.oninput=()=>y.classList.toggle('off',i.input.value.trim()!==o.word);i.input.focus();return m;},
  Prompt(title,ph,val,fn){const r=Shell.screen,W=1468,H=848,w=520,h=214,m=D.Frame(r,0,0,W,H);m.style.zIndex=60;D.Line(m,0,0,W,H,'rgba(0,0,0,.6)');D.Hit(m,0,0,W,H);
    const b=D.Frame(m,(W-w)/2,(H-h)/2,w,h);D.Box(b,0,0,w,h,12,C.PANEL,C.LINE2);D.Text(b,28,24,w-56,28,title,22,C.TEXT);const i=UI.Input(b,28,70,w-56,48,ph,40,val);
    const x=()=>m.remove(),ok=()=>{const v=i.input.value.trim();x();fn(v);};i.input.onkeydown=e=>{if(e.key==='Enter')ok();if(e.key==='Escape')x();};UI.Button(b,w-28-300,h-68,140,44,'Отмена','secondary',null,x);UI.Button(b,w-28-150,h-68,150,44,'Готово','primary',null,ok);i.input.focus();},
};
function Toast(text,kind='ok'){const col={ok:C.OK,err:C.BAD,inf:C.INF}[kind],ic={ok:'check',err:'warning',inf:'sym_bolt'}[kind],st=Shell.screen,e=document.createElement('div');e.className='toast';const i=document.createElement('div');i.className='ic i-'+ic;i.style.color=col;const s=document.createElement('span');s.textContent=text;e.append(i,s);st.appendChild(e);setTimeout(()=>{e.style.opacity=0;setTimeout(()=>e.remove(),260);},2400);}
// Копирование в буфер обмена, в игре g_Game.CopyToClipboard(text).
function Copy(text){
  const done=()=>Toast('Скопировано: '+text),old=()=>{const t=document.createElement('textarea');t.value=text;document.body.appendChild(t);t.select();document.execCommand('copy');t.remove();done();};
  if(navigator.clipboard&&window.isSecureContext)navigator.clipboard.writeText(text).then(done,old);else old();
}
// Перенос текста по ширине: длинное слово без пробелов режется.
function Wrap(s,w,size){
  const out=[];let line='';
  for(let word of s.split(/\s+/)){
    while(TextW(word,size)>w){let n=word.length;while(n>1&&TextW(word.slice(0,n),size)>w)n--;if(line){out.push(line);line='';}out.push(word.slice(0,n));word=word.slice(n);}
    const t=line?line+' '+word:word;if(TextW(t,size)>w){out.push(line);line=word;}else line=t;
  }
  if(line)out.push(line);return out;
}
function Reveal(w,delay=0){if(!w||window.NOANIM)return;w.animate([{opacity:0,transform:'translateY(10px)'},{opacity:1,transform:'none'}],{duration:260,delay:delay*1000,fill:'backwards',easing:'cubic-bezier(.22,.61,.36,1)'});}
// Заглушка 3D-модели: в игре PlayerPreviewWidget.
function Silhouette(p,x,y,w,h){const f=D.Frame(p,x,y,w,h);f.innerHTML=`<svg viewBox="0 0 200 560" width="${w}" height="${h}" preserveAspectRatio="xMidYMid meet" style="position:absolute;left:0;top:0"><g fill="#262c31" stroke="#394147" stroke-width="1.5"><circle cx="100" cy="44" r="27"/><path d="M66 82q34-16 68 0l22 34 8 134-22 4-10-112-3 96 8 150 6 142h-38l-5-168h-8l-5 168H53l6-142 8-150-3-96-10 112-22-4 8-134z"/></g></svg>`;return f;}

// Приборные элементы: шкала делений, засечки, дальномер, маршрут, уголки, сигнальная полоса. В игре это линии и тайловая текстура.
const Gauge={
  Ticks(p,x,y,w,h,n,k,color){const step=w/n,bw=Math.max(2,step*.56),on=Math.round(n*Math.max(0,Math.min(1,k)));for(let i=0;i<n;i++)D.Line(p,x+i*step,y,bw,h,i<on?(color||C.ACCENT):C.PANEL3);},
  // Засечки пятёрками, в ячейку входит 20. До 20 каждая равна одному убийству, дальше показывают долю пути к цели.
  Tally(p,x,y,h,count,goal){
    const n=count<=20?count:Math.floor(20*count/goal);let cx=x;
    for(let g=0;g<Math.ceil(n/5);g++){
      const left=Math.min(5,n-g*5);
      for(let i=0;i<Math.min(4,left);i++){const l=D.Line(p,cx+i*8,y+(i%2),3,h-(i%3),C.TEXT);l.style.transform=`rotate(${[-3,2,-1,3][i]}deg)`;l.style.borderRadius='2px';}
      if(left===5){const l=D.Line(p,cx-5,y+h/2-1,40,3,C.ACCENT);l.style.transform='rotate(-24deg)';l.style.borderRadius='2px';}
      cx+=42;
    }
  },
  // Счёт добычи: точки в два ряда, заполняются до цели.
  Dots(p,x,y,cols,k,color){const n=cols*2,on=Math.round(n*Math.max(0,Math.min(1,k)));for(let i=0;i<n;i++)D.Circle(p,x+(i%cols)*14,y+Math.floor(i/cols)*13,9,i<on?(color||C.ACCENT):C.PANEL3);},
  // Дальномер: шкала 0..max и метка попадания.
  Range(p,x,y,w,val,max){
    D.Line(p,x,y+9,w,1,C.LINE2);
    for(let i=0;i<=10;i++)D.Line(p,x+w*i/10,y+(i%5?6:2),1,i%5?7:14,C.DIM2);
    const k=Math.min(1,val/max),mx=x+w*k;D.Line(p,x,y+8,w*k,3,C.ACCENT);D.Icon(p,mx-10,y,20,'crosshair',C.ACCENT);
  },
  // Маршрут: пунктир до цели и точка назначения.
  Route(p,x,y,w,k){
    const n=16,step=(w-22)/n,on=Math.round(n*Math.min(1,k));
    D.Circle(p,x,y+6,8,C.ACCENT);
    for(let i=0;i<n;i++)D.Line(p,x+12+i*step,y+9,step*.6,2,i<on?C.ACCENT:C.LINE2);
    D.Icon(p,x+w-18,y,20,'pin',k>=1?C.ACCENT:C.DIM2);
  },
  Corners(p,w,h,color,k=18){
    const c=color||C.ACCENT,L=(x,y,a,b)=>D.Line(p,x,y,a,b,c);
    L(0,0,k,2);L(0,0,2,k);L(w-k,0,k,2);L(w-2,0,2,k);L(0,h-2,k,2);L(0,h-k,2,k);L(w-k,h-2,k,2);L(w-2,h-k,2,k);
  },
  Hazard(p,x,y,w,h){const e=D.Line(p,x,y,w,h,'');e.style.background='repeating-linear-gradient(135deg,var(--accent) 0 12px,transparent 12px 24px)';e.style.opacity=.85;return e;},
  Glow(p,x,y,w,h,r,color){const e=D.Box(p,x,y,w,h,r,'');e.style.background=`linear-gradient(90deg,color-mix(in srgb,${color||'var(--accent)'} 16%,transparent),transparent 58%)`;return e;},
  // Столбик из сегментов: вверх от оси положительное значение, вниз отрицательное.
  Column(p,x,axis,w,k,neg){const up=10,down=4;for(let i=0;i<up;i++)D.Line(p,x,axis-9-i*9,w,6,!neg&&i<Math.round(up*k)&&k>0?C.GOLD:C.PANEL3);for(let i=0;i<down;i++)D.Line(p,x,axis+4+i*9,w,6,neg&&i<Math.max(1,Math.round(down*k))?C.BAD:C.PANEL2);},
  // Плашка роли.
  Pill(p,x,y,text,color,fill){const w=TextW(text,13)+22;D.Box(p,x,y,w,24,12,fill||null,color);D.Text(p,x,y,w,24,text,13,color,'c');return w;},
  // Короткая запись цели: 250, 5 тыс., 2,5 млн.
  Short(v){const f=n=>String(Math.round(n*10)/10).replace('.',',');return v>=1e6?f(v/1e6)+' млн':v>=1e3?f(v/1e3)+' тыс.':String(v);},
  // Значок ранга: звёзды и шевроны, в игре несколько тонированных ImageWidget.
  Insignia(p,x,y,s,r,color){
    const f=D.Frame(p,x,y,s,s),c=color||C.ACCENT,ss=s*.3,ch=s*.34,gap=s*.21,H=(r.st?ss:0)+(r.st&&r.c?s*.08:0)+(r.c?ch+(r.c-1)*gap:0);let cy=(s-H)/2;
    if(r.st){const w=r.st*ss+(r.st-1)*s*.03;for(let i=0;i<r.st;i++)D.Icon(f,(s-w)/2+i*(ss+s*.03),cy,ss,'sym_star',c);cy+=ss+(r.c?s*.08:0);}
    for(let i=0;i<r.c;i++){const e=D.Icon(f,s*.14,cy+i*gap,s*.72,'chev',c);e.style.height=ch+'px';}
    return f;
  },
  // Следующая круглая цель без потолка: 10, 25, 50, 100, 250, 500, 1 000, 2 500...
  Goal(v){for(let e=10;;e*=10)for(const m of [1,2.5,5]){if(v<e*m)return e*m;}},
};
