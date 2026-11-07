// Панель макета справа: переключает экраны и состояния, которые в игре зависят от модулей, батареи и роли. Не часть КПК.
const Maket = {
  init(){
    const m=document.getElementById('mk');
    m.innerHTML=`<h4>Экран</h4><div id="mkS"></div><h4>Модули</h4><div id="mkM"></div>
<h4>Батарея: <span id="mkBv"></span>%</h4><input type="range" id="mkB" min="0" max="100">
<h4>Роль в группировке</h4><div id="mkR"></div><h4>Прочее</h4><label><input type="checkbox" id="mkA">Вкладка «Админ»</label>
<p>P открывает КПК, TAB листает вкладки, Esc закрывает. На карте: ЛКМ двигает, колесо масштабирует, ПКМ ставит метку. В чате ПКМ по сообщению открывает меню.</p>`;
    document.getElementById('mkb').onclick=()=>m.classList.toggle('open');
    const scr=[['main','Основной экран'],['boot','Загрузка'],['reg','Регистрация'],['auth','Вход'],['trophy','Чужой КПК'],['admin','Админ-режим (Alt+L)'],['hud','HUD поверх игры']];
    this.btnS=scr.map(([k,t])=>{const b=document.createElement('button');b.textContent=t;b.onclick=()=>this.screen(k);document.getElementById('mkS').appendChild(b);return{k,b};});
    [['gps','GPS'],['map','Карта'],['chat','Связь'],['gp','Группировка']].forEach(([k,t])=>{
      const l=document.createElement('label'), c=document.createElement('input'); c.type='checkbox'; c.checked=S.mods[k];
      c.onchange=()=>{S.mods[k]=c.checked;Shell.reload();}; l.append(c,t); document.getElementById('mkM').appendChild(l);
    });
    const r=document.getElementById('mkB'); r.value=S.bat; document.getElementById('mkBv').textContent=S.bat;
    r.oninput=()=>{S.bat=+r.value;document.getElementById('mkBv').textContent=S.bat;Shell.reload();};
    const roles=[[3,'Лидер','Атаман'],[2,'Заместитель','Смертник'],[1,'Боец','Вольный'],[0,'Без группировки',''],[-1,'Приглашение пришло','']];
    this.btnR=roles.map(([v,t,rank])=>{const b=document.createElement('button');b.textContent=t;b.onclick=()=>{S.invite=v<0;S.role=Math.max(0,v);S.rank=rank;S.factionId='gp003';Shell.lastTab=v<=0?TAB.FACTION:Shell.lastTab;this.ensureMain();Shell.reload();this.sync();};document.getElementById('mkR').appendChild(b);return{v,b};});
    const a=document.getElementById('mkA'); a.checked=S.isAdmin;
    a.onchange=()=>{S.isAdmin=a.checked;if(!a.checked)S.adminMode=false;if(Shell.lastTab===TAB.ADMIN)Shell.lastTab=TAB.PROFILE;S.screen='main';Shell.open();this.sync();};
    this.sync();
  },
  ensureMain(){ if(S.screen!=='main'){S.screen='main';Shell.open();} },
  screen(k){
    S.adminMode=false;
    if(k==='main'){S.loggedIn=true;S.screen='main';}
    else if(k==='boot'){S.screen='boot';}
    else if(k==='reg'){S.loggedIn=false;S.registered=false;S.screen='auth';}
    else if(k==='auth'){S.loggedIn=false;S.registered=true;S.screen='auth';}
    else if(k==='trophy'){S.screen='trophy';}
    else if(k==='admin'){S.loggedIn=true;S.adminMode=true;S.isAdmin=true;S.screen='main';Shell.lastTab=TAB.ADMIN;}
    else if(k==='hud'){S.screen='hud';}
    Shell.open(); this.sync();
  },
  sync(){
    if(!this.btnS) return;
    const cur=S.adminMode?'admin':S.screen==='auth'?(S.registered?'auth':'reg'):S.screen;
    this.btnS.forEach(o=>o.b.classList.toggle('on',o.k===cur));
    const rv=S.invite?-1:S.role;
    this.btnR.forEach(o=>o.b.classList.toggle('on',o.v===rv));
  },
};
