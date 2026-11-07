// Оболочка: боковое меню 240, верхняя строка 72, рабочая область 1228x776 с полями 28 (внутри 1172x720).
const Pages={};
const NAV_BASE=[['profile','Профиль','user'],['rank','Ранг','rank'],['map','Карта','map'],['rating','Рейтинг','trophy']];
const NAV_CLAN=[['clan','Клан','flag'],['cmembers','Участники','users'],['cchat','Чат','chat'],['cjournal','Журнал','clock'],['cnews','Новости','news'],['cranks','Ранги клана','key'],['contracts','Контракты','contract'],['bank','Склад денег','vault'],['csettings','Настройки','sym_cog']];
const TITLES={profile:'Профиль',rank:'Ранг',map:'Карта',rating:'Рейтинг',create:'Создание клана',clan:'Клан',cmembers:'Участники',cchat:'Чат клана',cjournal:'Журнал клана',cnews:'Новости клана',csettings:'Настройки клана',admin:'Кланы сервера',cranks:'Ранги клана',contracts:'Контракты',bank:'Склад денег'};
const Shell={
  init(){
    this.stage=document.getElementById('stage'); this.screen=document.getElementById('screen');
    this.fit(); addEventListener('resize',()=>this.fit());
    addEventListener('keydown',e=>{if(e.key==='Escape'&&this.page&&this.page.esc)this.page.esc();});
    addEventListener('click',()=>{const d=document.querySelector('.ddp');if(d)d.remove();});
    this.build();
    let last=performance.now(); const loop=t=>{const dt=Math.min(.1,(t-last)/1000);last=t;if(this.page&&this.page.tick)this.page.tick(dt);this.clockT=(this.clockT||0)+dt;if(this.clockT>1){this.clockT=0;this.clock();}requestAnimationFrame(loop);}; requestAnimationFrame(loop);
  },
  // Панель макета занимает 240 справа: планшет вписывается в оставшееся место.
  fit(){const w=innerWidth-240,k=Math.min(w/1560,innerHeight/940),s=this.stage.style;s.transform=`scale(${k})`;s.left=w/2-960*k+'px';s.top=innerHeight/2-540*k+'px';},
  build(){
    const r=this.screen; r.innerHTML='';
    this.side=D.Frame(r,0,0,240,848); D.Line(this.side,0,0,240,848,C.SIDE); D.Line(this.side,239,0,1,848,C.LINE);
    D.Box(this.side,18,20,44,44,10,C.PANEL2,C.LINE2); D.Icon(this.side,29,31,22,'signal',C.ACCENT);
    D.Text(this.side,74,20,156,24,Fit(SERVER.name,156,14),14,C.TEXT);
    D.Text(this.side,74,42,156,20,'Планшет',13,C.DIM2);
    this.nav=D.Frame(this.side,0,84,240,760);
    const top=this.top=D.Frame(r,240,0,1228,72); D.Line(top,0,71,1228,1,C.LINE);
    this.title=D.Text(top,28,0,700,72,'',26,C.TEXT);
    this.clockT0=D.Text(top,1000,0,70,72,'00:00',20,C.TEXT,'r');
    D.Icon(top,1086,26,20,'signal',C.ACCENT); D.Icon(top,1118,26,20,'battery',C.ACCENT);
    const cb=D.Box(top,1164,16,40,40,8,C.PANEL2,C.LINE2),ci=D.Icon(top,1174,26,20,'close',C.DIM);
    D.Hit(top,1164,16,40,40,()=>Toast('В игре планшет закрывается','inf'),o=>ci.style.color=o?C.TEXT:C.DIM);
    this.body=D.Clip(r,240,72,1228,776);
    this.go(S.page);
  },
  navPaint(){
    const n=this.nav;n.innerHTML='';let y=0;
    const item=([id,label,ic,badge,bad])=>{const e=_mk(n,'nav'+(S.page===id?' on':''),14,y,212,44);if(S.page===id)D.Box(e,0,10,3,24,2,C.ACCENT);D.Icon(e,16,11,22,ic,S.page===id?C.ACCENT:C.DIM);D.Text(e,52,0,156,44,label,16,S.page===id?C.TEXT:C.DIM);if(badge){D.Box(e,172,11,24,22,11,bad?C.BAD:C.ACCENT);D.Text(e,172,11,24,22,String(badge),13,C.ON,'c');}e.onclick=()=>this.go(id);y+=48;};
    NAV_BASE.forEach(i=>item(i[0]==='profile'&&S.invite?[...i,1]:i));
    if(!S.clan) item(['create','Создать клан','plusclan']);
    else{y+=14;D.Cap(n,30,y,190,S.clan.name,C.DIM2);y+=26;const ready=S.clan.my.filter(c=>c.state==='ready').length;NAV_CLAN.filter(i=>i[0]!=='csettings'||Clan.can('color')).forEach(i=>item(i[0]==='contracts'?[...i,ready]:i[0]==='cnews'?[...i,S.clan.newsUnread||0]:i[0]==='cchat'?[...i,S.clan.chatUnread||0]:i[0]==='cmembers'&&Clan.can('invite')?[...i,S.clan.requests.length]:i[0]==='bank'&&Clan.taxDays()!==null&&Clan.taxDays()<=7?[...i,'!',true]:i));}
    if(S.admin){y+=14;D.Cap(n,30,y,190,'Администратор',C.DIM2);y+=26;item(['admin','Кланы сервера','admin']);}
  },
  // Акцент интерфейса: цвет клана, без клана оранжевый по умолчанию. c задаёт предпросмотр при выборе цвета.
  accent(c){
    const col=c||(S.clan?S.clan.emblem.color:DEFAULT_ACCENT),n=parseInt(col.slice(1),16),mix=v=>Math.round(v+(255-v)*.28);
    const st=document.documentElement.style;st.setProperty('--accent',col);st.setProperty('--accent2',`rgb(${mix(n>>16)},${mix(n>>8&255)},${mix(n&255)})`);
  },
  go(id){
    if(id==='admin'&&!S.admin)id='profile';
    if(['clan','cmembers','cchat','cjournal','cnews','cranks','contracts','bank','csettings'].includes(id)&&!S.clan)id='create';
    if(id==='csettings'&&!Clan.can('color'))id='clan';
    if(id==='create'&&S.clan)id='clan';
    // Обновление того же экрана идёт без анимации появления, иначе интерфейс прыгает.
    const same=this.page===Pages[id];
    if(this.page&&this.page.leave)this.page.leave();
    Pages.profile.closeDiscord();Atm.closePop();
    S.page=id;this.accent();this.navPaint();this.title.textContent=TITLES[id];this.body.innerHTML='';
    this.page=Pages[id];this.page.build(id==='map'?this.body:D.Frame(this.body,28,28,1172,720));
    if(!same)Reveal(this.body.firstChild);this.clock();Panel.sync();
  },
  clock(){const d=new Date();this.clockT0.textContent=String(d.getHours()).padStart(2,'0')+':'+String(d.getMinutes()).padStart(2,'0');},
  refresh(){this.go(S.page);},
};
