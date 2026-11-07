// Экраны планшета. Область каждого 1172x720, карта занимает всю рабочую зону 1228x776.
const ROLE=['','Боец','Офицер','Глава'];
const lifeText=s=>{const h=Math.floor(s/3600);return h>=1000?Num(h)+' ч':h+' ч '+Math.floor(s%3600/60)+' мин';};

Pages.profile={
  build(r){
    const card=UI.Card(r,0,0,340,720);Gauge.Corners(card,340,720,C.ACCENT,22);Silhouette(card,30,20,280,604);
    D.Box(card,16,636,308,68,8,C.PANEL2,C.LINE);D.Icon(card,30,658,24,'discord',S.discord?C.ACCENT:C.DIM2);D.Cap(card,66,648,196,S.clan?'Discord · видит клан':'Discord');
    D.Text(card,66,668,196,26,S.discord?Fit('@'+S.discord,196,16):'Не указан',16,S.discord?C.TEXT:C.DIM2);UI.Button(card,272,650,40,40,null,'secondary',S.discord?'edit':'plus',()=>this.editDiscord());
    const R=this.R=D.Frame(r,364,0,808,720),c=S.clan;
    this.header(R,c);this.life(R);this.stats(R);
    if(c)this.clanCard(R,c);else if(S.invite)this.inviteCard(R,S.invite);else this.joinCard(R);
    this.tT=1;this.tick(0);
  },
  closeDiscord(){if(this.dp){this.dp.remove();this.dp=null;}},
  // Попап Discord: открывается из профиля и из своей карточки участника.
  editDiscord(){
    const W=1468,H=848,w=520,h=240,m=this.dp=D.Frame(Shell.screen,0,0,W,H),close=()=>this.closeDiscord();m.style.zIndex=60;
    D.Line(m,0,0,W,H,'rgba(0,0,0,.6)');D.Hit(m,0,0,W,H,close);
    const b=D.Frame(m,(W-w)/2,(H-h)/2,w,h);b.style.boxShadow='0 16px 44px rgba(0,0,0,.6)';D.Box(b,0,0,w,h,12,C.PANEL,C.LINE2);
    D.Text(b,28,22,400,32,'Discord',22,C.TEXT);const ci=D.Icon(b,w-46,28,20,'close',C.DIM);D.Hit(b,w-54,20,36,36,close,o=>ci.style.color=o?C.TEXT:C.DIM);
    const i=UI.Input(b,28,70,w-56,48,'Имя пользователя',32,S.discord),base='Имя пользователя из Discord. Его видят соклановцы.',note=D.Text(b,28,124,w-56,24,base,14,C.DIM2);
    const finish=(v,text)=>{S.discord=v;close();Shell.refresh();Toast(text);};
    const save=()=>{const v=i.input.value.trim().replace(/^@+/,'');if(!v){if(S.discord)finish('','Discord удалён');else close();return;}const e=discordError(v);if(e){note.textContent=e;note.style.color=C.BAD;return;}if(v===S.discord){close();return;}finish(v,'Discord сохранён');};
    i.input.oninput=()=>{const p=i.input.selectionStart;i.input.value=i.input.value.toLowerCase();i.input.setSelectionRange(p,p);note.textContent=base;note.style.color=C.DIM2;};
    i.input.onkeydown=e=>{if(e.key==='Enter')save();if(e.key==='Escape')close();};
    if(S.discord)UI.Button(b,28,h-68,120,44,'Удалить','danger',null,()=>finish('','Discord удалён'));
    UI.Button(b,w-28-300,h-68,140,44,'Отмена','secondary',null,close);UI.Button(b,w-28-150,h-68,150,44,'Сохранить','primary',null,save);
    i.input.focus();i.input.select();
  },
  header(R,c){
    D.Text(R,0,-4,540,76,Fit(S.name,530,64),64,C.TEXT);
    if(c){UI.Emblem(R,0,84,28,c.emblem);D.Text(R,38,82,442,32,Fit('['+c.tag+'] '+c.name+' · '+Clan.rank(Clan.me()).name,442,17),17,C.DIM);}
    else{const s='БЕЗ КЛАНА',w=TextW(s,13)+26;D.Box(R,0,84,w,28,14,null,C.LINE2);D.Text(R,0,84,w,28,s,13,C.DIM,'c');}
    // Прогресс ранга: клик открывает раздел «Ранг».
    const xp=Rank.xp(),ri=Rank.my(),cur=RANKS[ri],nx=RANKS[ri+1],m=D.Frame(R,488,6,320,104);D.Box(m,0,0,320,104,10,C.PANEL2);Gauge.Corners(m,320,104,C.ACCENT,12);
    Gauge.Insignia(m,16,12,52,cur);D.Text(m,80,12,226,30,cur.name,22,C.TEXT);
    D.Text(m,80,42,226,22,nx?Num(xp)+'/'+Num(nx.need)+' до ранга «'+nx.name+'»':'Высший ранг',14,C.DIM);
    Gauge.Ticks(m,16,76,288,14,34,nx?(xp-cur.need)/(nx.need-cur.need):1);D.Hit(m,0,0,320,104,()=>Shell.go('rank'));
  },
  life(R){
    const f=D.Frame(R,0,126,808,160);D.Box(f,0,0,808,160,10,C.PANEL,C.LINE);Gauge.Glow(f,1,1,806,158,10);
    D.Circle(f,24,21,10,C.OK);D.Cap(f,44,17,300,'Текущая жизнь');
    this.digits=[];const hd=Math.max(2,String(Math.floor(S.life/3600)).length),w=[...Array(hd).fill(36),18,36,36,18,36,36];let x=24;w.forEach(a=>{this.digits.push(D.Text(f,x,36,a,64,'0',56,C.TEXT,'c'));x+=a;});
    const rx=24,rw=590,H=24,hNow=Math.min(H,S.life/3600);
    for(let i=0;i<=H*2;i++){const t=i/2,big=i%12===0,on=t<=hNow;D.Line(f,rx+rw*i/(H*2),108,big?2:1,big?16:9,on?C.ACCENT:C.LINE2);}
    [0,6,12,18,24].forEach(h=>D.Text(f,rx+rw*h/H-20,127,40,18,h?h+' ч':'0',12,C.DIM2,'c'));
    const nx=this.now=D.Line(f,rx+rw*hNow/H-1,102,3,26,C.TEXT);nx.style.borderRadius='2px';
    D.Line(f,rx+rw*S.record/3600/H-1,102,3,26,C.GOLD);
    D.Line(f,640,20,1,120,C.LINE);
    D.Cap(f,660,18,130,'На сервере');D.Text(f,660,38,130,40,S.days+' дн.',30,C.TEXT);
    D.Cap(f,660,88,130,'Рекорд жизни',C.GOLD);D.Text(f,660,108,130,32,lifeText(S.record),20,C.GOLD);
  },
  stats(R){
    const f=D.Frame(R,0,298,808,126),cw=202;D.Box(f,0,0,808,126,10,C.PANEL,C.LINE);
    const cell=(i,cap,val,goal)=>{const x=i*cw;if(i)D.Line(f,x,16,1,94,C.LINE);D.Cap(f,x+20,14,cw-34,cap);let sz=30;while(sz>18&&TextW(val,sz)>162)sz-=1;D.Text(f,x+20,34,cw-34,40,val,sz,C.TEXT);if(goal){const room=162-TextW(cap.toUpperCase(),13)-8,g=[`до ${Gauge.Short(goal)}`,Gauge.Short(goal)].find(t=>TextW(t,13)<=room);if(g)D.Text(f,x+20,14,162,18,g,13,C.DIM2,'r');}return x+20;};
    const gz=Ach.goal('zombies',S.zombies)||Gauge.Goal(S.zombies),gm=Ach.goal('mutants',S.mutants)||Gauge.Goal(S.mutants),ga=Ach.goal('animals',S.animals)||Gauge.Goal(S.animals),gd=Ach.goal('km',Math.floor(S.dist/1000))||Gauge.Goal(S.dist/1000),km=S.dist<1e6?(S.dist/1000).toFixed(1).replace('.',','):Num(S.dist/1000);
    let x=cell(0,'Заражённых',Num(S.zombies),gz);Gauge.Ticks(f,x,86,162,20,24,S.zombies/gz);
    x=cell(1,'Мутантов',Num(S.mutants),gm);Gauge.Tally(f,x+4,80,30,S.mutants,gm);
    x=cell(2,'Животных',Num(S.animals),ga);Gauge.Dots(f,x,84,12,S.animals/ga);
    x=cell(3,'Пройдено',km+' км',gd);Gauge.Route(f,x,87,162,S.dist/1000/gd);
  },
  joinCard(R){
    const f=D.Frame(R,0,436,808,284);D.Box(f,0,0,808,284,10,C.PANEL,C.LINE);
    const h=D.Clip(f,0,0,808,8);h.style.borderRadius='10px 10px 0 0';Gauge.Hazard(h,0,0,808,8);
    const s=D.Frame(f,24,32,216,228);D.Box(s,0,0,216,228,8,C.PANEL2);Gauge.Corners(s,216,228,C.ACCENT);
    this.slot=D.Frame(s,44,26,128,128);this.slotI=0;this.paintSlot();
    D.Text(s,0,170,216,22,'Твоя эмблема',15,C.TEXT,'c');D.Text(s,0,194,216,18,'форма, символ, цвет',13,C.DIM2,'c');
    D.Text(f,264,28,330,40,'Собери свой клан',32,C.TEXT);UI.Button(f,604,28,180,40,'Найти клан','secondary','users',()=>{Pages.rating.tab=0;Shell.go('rating');},14);
    if(S.applied){D.Text(f,264,70,390,34,Fit('Заявка в клан «'+S.applied.clan+'» на рассмотрении',390,15),15,C.ACCENT);UI.Button(f,664,70,120,34,'Отозвать','secondary',null,()=>{S.applied=null;Toast('Заявка отозвана','inf');Shell.refresh();},13);}
    else D.MText(f,264,72,520,44,'Свои люди, контракты, общие деньги и место в рейтинге.',15,C.DIM);
    [['contract','Контракты'],['sym_bolt','Влияние и уровни'],['vault','Склад денег'],['trophy','Рейтинг кланов']].forEach(([i,t],k)=>{
      const x=264+(k%2)*262,y=112+Math.floor(k/2)*48;D.Box(f,x,y,40,40,8,C.PANEL2);D.Icon(f,x+10,y+10,20,i,C.ACCENT);D.Text(f,x+52,y,200,40,t,16,C.TEXT);
    });
    const rk=Rank.my()>=CLAN_RANK,ok=S.money>=CLAN_PRICE,b=UI.Button(f,264,212,520,48,'Создать клан','primary','plusclan',()=>Shell.go(rk?'create':'rank'),17);b.style.justifyContent='flex-start';b.style.paddingLeft='20px';
    const pw=rk&&ok?170:250;D.Box(b,520-pw-5,5,pw,38,6,'#0E0D0C');
    if(!rk)D.Text(b,520-pw-5,5,pw,38,'Нужен ранг «'+RANKS[CLAN_RANK].name+'»',15,C.BAD,'c');else if(ok)UI.Money(D.Frame(b,520-pw+13,14,150,24),0,0,CLAN_PRICE,18);else D.Text(b,520-pw-5,5,pw,38,'Не хватает '+Num(CLAN_PRICE-S.money),15,C.BAD,'c');
  },
  // Входящее приглашение: у игрока без клана заменяет блок создания.
  inviteCard(R,v){
    const f=D.Frame(R,0,436,808,284);D.Box(f,0,0,808,284,10,C.PANEL,C.ACCENT);Gauge.Glow(f,1,1,806,282,10);
    const s=D.Frame(f,24,28,190,228);D.Box(s,0,0,190,228,8,C.PANEL2);Gauge.Corners(s,190,228,C.ACCENT);UI.Emblem(s,30,22,130,v.emblem);
    D.Text(s,0,166,190,20,'УРОВЕНЬ',13,C.DIM,'c');D.Text(s,0,186,190,30,String(v.level),26,C.ACCENT,'c');
    D.Icon(f,238,26,20,'invite',C.ACCENT);D.Cap(f,266,27,400,'Приглашение в клан',C.ACCENT);
    D.Text(f,238,50,546,44,Fit(v.name,546,34),34,C.TEXT);
    D.Text(f,238,96,546,24,'['+v.tag+'] · '+v.members+' из '+CLAN_MAX+' · зовёт '+v.by,15,C.DIM);
    D.MText(f,238,128,546,60,v.desc,16,C.DIM);
    UI.Button(f,238,212,265,44,'Вступить','primary','check',()=>{const c=demoClan(1);S.clan={...c,name:v.name,tag:v.tag,desc:v.desc,emblem:{...v.emblem},level:v.level};S.invite=null;S.applied=null;Toast('Вы вступили в клан «'+v.name+'»');Shell.go('clan');},16);
    UI.Button(f,519,212,265,44,'Отказать','secondary','close',()=>{S.invite=null;Toast('Приглашение отклонено','inf');Shell.refresh();},16);
  },
  paintSlot(){
    const list=[{shape:'shield',color:'var(--accent)',symbol:'crown'},{shape:'hex',color:'#8FA6FF',symbol:'skull'},{shape:'circle',color:'#6FBF8E',symbol:'tree'},{shape:'diamond',color:'#E9C55B',symbol:'bolt'},{shape:'banner',color:'#EF5B4C',symbol:'sword'}];
    this.slot.innerHTML='';UI.Emblem(this.slot,0,0,128,list[this.slotI%list.length]);
    if(!window.NOANIM)this.slot.animate([{opacity:0,transform:'scale(.92)'},{opacity:1,transform:'none'}],{duration:260});
  },
  clanCard(R,c){
    const f=D.Frame(R,0,436,808,284),me=c.members.find(m=>m.me),done=c.contracts.filter(k=>k.state==='done').length;
    D.Box(f,0,0,808,284,10,C.PANEL,C.LINE);Gauge.Glow(f,1,1,806,282,10);
    const s=D.Frame(f,24,28,190,228);D.Box(s,0,0,190,228,8,C.PANEL2);Gauge.Corners(s,190,228,C.ACCENT);UI.Emblem(s,30,22,130,c.emblem);
    D.Text(s,0,166,190,20,'УРОВЕНЬ',13,C.DIM,'c');D.Text(s,0,186,190,30,String(c.level),26,C.ACCENT,'c');
    D.Text(f,238,24,546,42,Fit(c.name,546,32),32,C.TEXT);
    D.Text(f,238,66,546,24,'['+c.tag+'] · '+c.members.length+' из '+CLAN_MAX+' · место '+clanPlace(),15,C.DIM);
    D.Cap(f,238,102,300,'Влияние');D.Text(f,500,100,284,20,Num(c.influence)+'/'+Num(c.next),15,C.INF,'r');
    Gauge.Ticks(f,238,126,546,16,50,c.influence/c.next,C.INF);
    D.Cap(f,238,158,200,'Контракты сегодня');
    c.contracts.forEach((k,i)=>D.Box(f,238+i*32,182,26,10,2,k.state==='done'?C.ACCENT:C.PANEL3));
    D.Cap(f,460,158,150,'Ваш вклад');D.Text(f,460,176,150,24,'+'+me.today,20,C.INF);
    D.Cap(f,630,158,150,'Склад денег');UI.Money(f,630,176,c.bank,18);
    UI.Button(f,238,212,265,44,'Открыть клан','primary','flag',()=>Shell.go('clan'));UI.Button(f,519,212,265,44,'Контракты','secondary','contract',()=>Shell.go('contracts'));
  },
  tick(dt){
    this.tT+=dt;
    if(this.tT>=1){
      this.tT=0;if(dt)S.life+=1;
      const p=v=>String(v).padStart(2,'0'),s=p(Math.floor(S.life/3600))+':'+p(Math.floor(S.life%3600/60))+':'+p(S.life%60);
      if(s.length!==this.digits.length){Shell.refresh();return;}this.digits.forEach((e,i)=>e.textContent=s[i]);
    }
    if(this.slot&&!window.NOANIM&&!S.clan){this.sT=(this.sT||0)+dt;if(this.sT>2.2){this.sT=0;this.slotI++;this.paintSlot();}}
  },
};

// Ранг: общая система для всех игроков, от клана не зависит.
Pages.rank={
  tab:0,
  build(r){this.r=r;this.tabs=D.Frame(Shell.top,110,0,400,72);this.paint();},
  leave(){if(this.tabs){this.tabs.remove();this.tabs=null;}},
  paint(){
    const t=this.tabs,r=this.r;t.innerHTML='';r.innerHTML='';let x=0;
    ['Обзор','Достижения'].forEach((n,i)=>{const w=TextW(n,18)+8,on=i===this.tab,l=D.Text(t,x,0,w,72,n,18,on?C.TEXT:C.DIM);if(on)D.Line(t,x,68,w,3,C.ACCENT);D.Hit(t,x-10,0,w+20,72,()=>{this.tab=i;this.paint();},o=>{if(!on)l.style.color=o?C.TEXT:C.DIM;});x+=w+40;});
    if(this.tab)this.achievements(r);else this.main(r);
  },
  // Достижения: сверху итог и ближайшая цель, ниже карточки со ступенями.
  achievements(r){
    const v=Ach.mine(),got=Ach.count(v),all=Ach.total(),open=ACH.filter(a=>Ach.done(a,v[a.id])<a.steps.length).map(a=>{const d=Ach.done(a,v[a.id]),from=d?a.steps[d-1]:0;return {a,next:a.steps[d],k:(v[a.id]-from)/(a.steps[d]-from)};}).sort((x,y)=>y.k-x.k)[0];
    const h=D.Frame(r,0,0,1172,112);D.Box(h,0,0,1172,112,10,C.PANEL,C.LINE);Gauge.Glow(h,1,1,1170,110,10);
    D.Cap(h,24,18,240,'Получено ступеней');D.Text(h,24,40,160,44,got+'/'+all,34,C.TEXT);Gauge.Ticks(h,190,56,190,14,all,got/all);
    D.Line(h,404,18,1,76,C.LINE);D.Cap(h,424,18,240,'Опыт за достижения');D.Text(h,424,40,240,44,'+'+Num(Ach.xp()),34,C.ACCENT);
    D.Line(h,684,18,1,76,C.LINE);D.Cap(h,704,18,240,'Ближе всего');
    if(open){D.Icon(h,704,46,26,open.a.icon,C.ACCENT);D.Text(h,742,38,220,28,Fit(open.a.name,220,19),19,C.TEXT);D.Text(h,742,66,220,24,Num(v[open.a.id])+'/'+Num(open.next),15,C.DIM);Gauge.Ticks(h,966,56,182,14,20,open.k);}
    else D.Text(h,704,40,440,44,'Все ступени взяты',24,C.GOLD);
    ACH.forEach((a,i)=>{
      const x=(i%4)*297,y=128+(i>>2)*304,val=v[a.id],d=Ach.done(a,val),n=a.steps.length,full=d>=n,col=d?MEDAL[n-d]:C.DIM,f=UI.Card(r,x,y,281,288);
      D.Box(f,20,20,48,48,10,C.PANEL2,d?col:C.LINE2);D.Icon(f,32,32,24,a.icon,col);D.Text(f,80,18,186,28,Fit(a.name,186,18),18,C.TEXT);D.Text(f,80,46,186,22,Fit(a.text,186,14),14,C.DIM);
      a.steps.forEach((st,k)=>{const sx=20+k*83,on=k<d,m=MEDAL[n-1-k];D.Box(f,sx,92,75,62,8,C.PANEL2,on?m:null);D.Text(f,sx,98,75,26,Gauge.Short(st),15,on?m:C.DIM,'c');D.Text(f,sx,124,75,22,'+'+Num(ACH_XP[k]),13,on?C.DIM:C.DIM2,'c');});
      if(full){D.Text(f,20,176,241,28,'Все ступени взяты',15,C.GOLD);Gauge.Ticks(f,20,206,241,12,24,1,C.GOLD);}
      else{const next=a.steps[d],from=d?a.steps[d-1]:0;D.Cap(f,20,178,120,'Ступень '+(d+1));D.Text(f,100,176,161,22,Num(val)+'/'+Num(next),15,C.TEXT,'r');Gauge.Ticks(f,20,206,241,12,24,(val-from)/(next-from));D.Text(f,20,232,241,24,'Награда: +'+Num(ACH_XP[d])+' опыта',14,C.ACCENT);}
    });
  },
  main(r){
    const xp=Rank.xp(),i=Rank.my(),cur=RANKS[i],nx=RANKS[i+1];
    const h=D.Frame(r,0,0,680,292);D.Box(h,0,0,680,292,10,C.PANEL,C.LINE);Gauge.Glow(h,1,1,678,290,10);
    const s=D.Frame(h,24,28,236,236);D.Box(s,0,0,236,236,8,C.PANEL2);Gauge.Corners(s,236,236,C.ACCENT,16);Gauge.Insignia(s,38,38,160,cur);
    D.Cap(h,288,30,360,'Ваш ранг');D.Text(h,288,50,370,62,cur.name,50,C.TEXT);D.Text(h,288,112,370,24,'Ступень '+(i+1)+' из '+RANKS.length+' · место '+playerList().find(p=>p.me).place,16,C.DIM);
    D.Cap(h,288,160,200,'Опыт');D.Text(h,470,158,186,20,nx?Num(xp)+'/'+Num(nx.need):Num(xp),15,C.ACCENT,'r');
    const k=nx?(xp-cur.need)/(nx.need-cur.need):1;Gauge.Ticks(h,288,186,368,18,30,k);
    D.Text(h,288,216,368,24,nx?'До ранга «'+nx.name+'»: '+Num(nx.need-xp)+' опыта':'Высший ранг',16,C.DIM);
    this.scene(r);this.ladder(r,i);this.sources(r,xp);this.top(r,xp);
  },
  // Как это выглядит в игре: ник, эмблема клана и ранг над головой.
  scene(r){
    const f=D.Clip(r,696,0,476,292);f.style.borderRadius='10px';const bg=D.Box(f,0,0,476,292,10,'');bg.style.background='linear-gradient(180deg,#2b2f2c 0,#1c1f1c 55%,#101110 100%)';D.Box(f,0,0,476,292,10,null,C.LINE);
    D.Cap(f,20,16,436,'Над головой в игре');
    const my={name:S.name,clan:S.clan,rank:RANKS[Rank.my()]};
    [[104,'Sipliy',null,RANKS[0],.72,.75],[238,my.name,my.clan?my.clan.emblem:null,my.rank,1,1],[372,'Loki',{shape:'hex',color:'#8FA6FF',symbol:'skull'},RANKS[4],.82,.85]].forEach(([cx,n,e,ra,k,o])=>{
      const g=D.Frame(f,0,0,476,292);g.style.opacity=o;const by=292,hs=Math.round(58*k),hy=by-hs*2.1;
      D.Circle(g,cx-hs/2,hy,hs,'#0c0d0c');const b=D.Box(g,cx-hs*.95,hy+hs*1.02,hs*1.9,hs*2,hs*.5,'#0c0d0c');
      const py=hy-58*k-10,sz=Math.round(19*k),ns=Math.round(13*k),es=e?Math.round(24*k):0,w=TextW(n,sz)+(e?es+6:0),x=cx-w/2;
      if(e)UI.Emblem(g,x,py,es,e);const t=D.Text(g,x+(e?es+6:0),py,TextW(n,sz)+6,es||Math.round(24*k),n,sz,'#fff');t.style.textShadow='0 1px 3px #000,0 0 6px #000';
      const rw=TextW(ra.name,ns)+Math.round(20*k)+4,rx=cx-rw/2,ry=py+Math.round(27*k);Gauge.Insignia(g,rx,ry,Math.round(18*k),ra);const u=D.Text(g,rx+Math.round(20*k),ry,rw,Math.round(18*k),ra.name,ns,C.ACCENT);u.style.textShadow='0 1px 3px #000,0 0 6px #000';
    });
  },
  ladder(r,i){
    const f=UI.Card(r,0,308,1172,190,'Лестница рангов'),n=RANKS.length,step=(1172-120)/(n-1),x0=60,y=78;
    D.Line(f,x0,y,step*(n-1),2,C.LINE2);D.Line(f,x0,y,step*i,2,C.ACCENT);
    RANKS.forEach((a,k)=>{
      const x=x0+k*step,on=k<=i,me=k===i;
      D.Box(f,x-28,y-28,56,56,12,C.PANEL2,me?C.ACCENT:C.LINE2);Gauge.Insignia(f,x-20,y-20,40,a,on?C.ACCENT:C.DIM2);
      D.Text(f,x-60,y+36,120,24,a.name,15,me?C.ACCENT:on?C.TEXT:C.DIM,'c');D.Text(f,x-60,y+60,120,20,a.need?Num(a.need):'старт',13,C.DIM2,'c');
    });
  },
  sources(r,xp){
    const f=UI.Card(r,0,514,760,206,'Откуда опыт'),l=Rank.src(),w=720/l.length;
    l.forEach((a,k)=>{
      const x=20+k*w,v=a.get()*a.xp;if(k)D.Line(f,x-10,50,1,136,C.LINE);
      D.Box(f,x,50,44,44,8,C.PANEL2);D.Icon(f,x+11,61,22,a.icon,C.ACCENT);
      D.Text(f,x,102,w-18,24,a.name,15,C.TEXT);D.Text(f,x,126,w-18,24,a.ach?'за ступени':'+'+a.xp+' опыта',14,C.ACCENT);const tot='итого '+Gauge.Short(v);D.Text(f,x,152,w-18,24,tot,TextW(tot,14)>w-18?12:14,C.DIM2);if(a.ach)D.Hit(f,x-6,44,w-8,140,()=>{this.tab=1;this.paint();});
    });
  },
  top(r,xp){
    const f=UI.Card(r,776,514,396,206,'Лучшие на сервере'),l=playerList().slice(0,4),lk=D.Text(f,236,14,140,20,'Весь рейтинг',14,C.ACCENT,'r');D.Hit(f,236,10,140,28,()=>{Pages.rating.tab=1;Shell.go('rating');});
    l.forEach((p,i)=>{const y=46+i*38,ra=RANKS[Rank.of(p.x)];D.Text(f,20,y,24,34,String(i+1),15,C.DIM2);Gauge.Insignia(f,46,y+4,26,ra,p.me?C.ACCENT:C.DIM);D.Text(f,82,y,160,34,Fit(p.n,160,16),16,p.me?C.ACCENT:C.TEXT);D.Text(f,236,y,140,34,Gauge.Short(p.x),15,C.DIM,'r');});
  },
};

Pages.create={
  name:'',tag:'',desc:'',e:{shape:'shield',color:COLORS[0],symbol:'crown'},
  build(r){
    const L=UI.Card(r,0,0,724,720);this.L=L;
    D.Cap(L,28,24,300,'Название');this.cn=D.Text(L,300,24,136,18,'',13,C.DIM2,'r');
    this.iN=UI.Input(L,28,48,408,52,'От 3 до 24 символов',24,this.name);
    D.Cap(L,460,24,120,'Тег');D.Text(L,560,24,136,18,'2–4 символа',13,C.DIM2,'r');
    this.iT=UI.Input(L,460,48,236,52,'Например, SVR',4,this.tag);
    D.Cap(L,28,122,300,'Слоган');this.iD=UI.Input(L,28,146,668,52,'Девиз клана, одной фразой',60,this.desc);
    D.Line(L,28,222,668,1,C.LINE);
    D.Cap(L,28,236,300,'Форма эмблемы');this.pick(L,28,258,SHAPES,'shape',50,6,'shp_');
    D.Cap(L,28,320,300,'Символ');D.Text(L,396,320,300,18,'Занято '+RIVALS.length+' из '+SHAPES.length*SYMBOLS.length,13,C.DIM2,'r');this.pick(L,28,342,SYMBOLS,'symbol',50,6,'sym_');
    D.Cap(L,28,516,500,'Цвет клана: им окрасится весь планшет');Shell.accent(this.e.color);this.colors=D.Frame(L,28,538,668,48);this.paintColors();
    this.iN.input.oninput=()=>{this.name=this.iN.input.value;this.refresh();};
    this.iT.input.oninput=()=>{this.iT.input.value=this.iT.input.value.toUpperCase().replace(/[^A-Z0-9]/g,'');this.tag=this.iT.input.value;this.refresh();};
    this.iD.input.oninput=()=>{this.desc=this.iD.input.value;this.refresh();};
    D.Line(L,28,600,668,1,C.LINE);
    D.MText(L,28,616,668,84,'Форма и символ закрепляются за кланом: такую же пару другой клан взять не сможет, цвет здесь не считается.\nНазвание, тег и эмблему нельзя будет сменить бесплатно.',14,C.DIM);
    this.R=D.Frame(r,748,0,424,720);this.refresh();
  },
  // Занятые пары затемнены и не выбираются: эмблема не должна повторяться.
  pick(p,x,y,list,key,s,g,pre){
    list.forEach((v,i)=>{
      const cx=x+(i%12)*(s+g),cy=y+Math.floor(i/12)*(s+g),sel=this.e[key]===v,own=emblemOwner({...this.e,[key]:v}),off=own&&!sel,b=D.Box(p,cx,cy,s,s,8,sel?C.PANEL3:C.PANEL2,sel?C.ACCENT:C.LINE2),k=s*.58,ic=D.Icon(p,cx+(s-k)/2,cy+(s-k)/2,k,pre+v,sel?C.ACCENT:C.DIM);
      if(off){ic.style.opacity=.28;D.Icon(p,cx+s-17,cy+3,14,'lock',C.DIM);}
      D.Hit(p,cx,cy,s,s,()=>{if(off){Toast('Эмблема занята: '+own.name,'err');return;}this.e[key]=v;Shell.refresh();},o=>{if(!sel)b.style.background=o?C.PANEL3:C.PANEL2;});
    });
  },
  paintColors(){this.colors.innerHTML='';COLORS.forEach((c,i)=>{const x=i*56;if(this.e.color===c)D.Box(this.colors,x+2,1,46,46,23,null,C.TEXT);D.Circle(this.colors,x+7,6,36,c);D.Hit(this.colors,x+5,4,40,40,()=>{this.e.color=c;Shell.accent(c);this.paintColors();this.refresh();});});},
  checks(){
    const n=this.name.trim(),t=this.tag;
    const need=RANKS[CLAN_RANK],xp=Rank.xp(),rk=['Ранг',Rank.of(xp)>=CLAN_RANK,Rank.of(xp)>=CLAN_RANK?RANKS[Rank.of(xp)].name:Num(xp)+'/'+Num(need.need)+' до «'+need.name+'»'],l=[['Название',n.length>=3&&!TAKEN.includes(n)&&!Clan.banned(n),n.length<3?'Минимум 3 символа':Clan.banned(n)?'Запрещённое слово':TAKEN.includes(n)?'Уже занято':'Свободно'],['Тег',t.length>=2&&!TAKEN_TAGS.includes(t)&&!Clan.banned(t),t.length<2?'Минимум 2 символа':Clan.banned(t)?'Запрещённое слово':TAKEN_TAGS.includes(t)?'Уже занят':'Свободен'],['Эмблема',!emblemOwner(this.e),emblemOwner(this.e)?'Занята: '+emblemOwner(this.e).name:'Свободна'],['Баланс',S.money>=CLAN_PRICE,S.money>=CLAN_PRICE?'Хватает':'Не хватает '+Num(CLAN_PRICE-S.money)],['Кланов на сервере',clanCount()<MAX_CLANS,clanCount()+' из '+MAX_CLANS]];l.splice(3,0,rk);return l;
  },
  refresh(){
    const r=this.R;r.innerHTML='';const c=UI.Card(r,0,0,424,300);Gauge.Glow(c,1,1,422,298,10);Gauge.Corners(c,424,300,C.ACCENT,16);
    UI.Emblem(c,142,26,140,this.e);
    D.Text(c,16,180,392,38,Fit(this.name.trim()||'Название клана',392,28),28,this.name.trim()?C.TEXT:C.DIM2,'c');
    D.Text(c,16,220,392,24,this.tag?'['+this.tag+']':'[ТЕГ]',17,C.ACCENT,'c');
    D.Text(c,16,256,392,22,'Основатель: '+S.name,14,C.DIM,'c');
    const p=UI.Card(r,0,316,424,404);D.Cap(p,24,20,376,'Оплата');
    D.Text(p,24,44,190,26,'Стоимость',16,C.DIM);UI.MoneyR(p,400,42,CLAN_PRICE,20);
    D.Text(p,24,74,190,26,'Ваш баланс',16,C.DIM);UI.MoneyR(p,400,72,S.money,20,S.money>=CLAN_PRICE?C.GOLD:C.BAD);
    D.Text(p,24,104,190,26,'Останется',16,C.DIM);UI.MoneyR(p,400,102,Math.max(0,S.money-CLAN_PRICE),20,C.TEXT);
    D.Line(p,24,138,376,1,C.LINE);
    const ch=this.checks();ch.forEach(([a,ok,s],i)=>{const y=146+i*30;D.Icon(p,24,y+3,20,ok?'check':'close',ok?C.OK:C.BAD);D.Text(p,54,y,TextW(a,16)+6,26,a,16,C.TEXT);D.Text(p,150,y,250,26,Fit(s,250,14),14,ok?C.DIM:C.BAD,'r');});
    const ready=ch.every(x=>x[1]),b=UI.Button(p,24,332,376,56,'Создать клан','primary','plusclan',()=>UI.Ask({title:'Создать клан?',text:'С баланса спишется '+Num(CLAN_PRICE)+'. Вы станете главой клана «'+this.name.trim()+'».',yes:'Создать',fn:()=>this.create()}),18);
    b.style.justifyContent='flex-start';b.style.paddingLeft='20px';D.Box(b,206,5,165,46,6,'#0E0D0C');UI.Money(D.Frame(b,218,16,150,26),0,0,CLAN_PRICE,18);
    b.classList.toggle('off',!ready);
  },
  create(){
    S.money-=CLAN_PRICE;S.applied=null;const d=demoClan(3);
    S.objects=S.objects.filter(o=>!o.clan);S.clan={...d,news:[],newsUnread:0,ranks:defaultRanks(),cats:[{id:'gen',name:'Общее',color:this.e.color,icon:'flag'}],name:this.name.trim(),tag:this.tag,desc:this.desc||'',emblem:{...this.e},level:1,influence:0,next:LEVELS[1],weekInf:0,bank:0,taxFree:TAX_FREE_DAYS,open:true,discord:'',atm:null,requests:[],chat:[],chatUnread:0,
      members:[{name:S.name,role:3,rank:'leader',today:0,total:0,online:true,last:'сейчас',me:true}],journal:[['flag','Клан создан','сейчас']],history:[],week:[0,0,0,0,0,0,0],
      contracts:contractsFor(1,false),my:myContractsFor(1,false),myReplace:1};
    Toast('Клан «'+S.clan.name+'» создан');Shell.go('clan');
  },
};

Pages.clan={
  build(r){
    const c=S.clan,on=c.members.filter(m=>m.online).length;this.r=r;
    const h=D.Frame(r,0,0,1172,172);D.Box(h,0,0,1172,172,10,C.PANEL,C.LINE);Gauge.Glow(h,1,1,1170,170,10);
    const s=D.Frame(h,18,18,136,136);D.Box(s,0,0,136,136,8,C.PANEL2);Gauge.Corners(s,136,136,C.ACCENT,14);UI.Emblem(s,18,18,100,c.emblem);
    D.Text(h,178,16,560,46,Fit(c.name,540,36),36,C.TEXT);D.Text(h,178+Math.min(540,TextW(c.name,36))+14,26,90,28,'['+c.tag+']',18,C.ACCENT);
    D.Text(h,178,64,620,24,c.desc?'«'+Fit(c.desc,600,15)+'»':'',15,C.DIM);
    D.Cap(h,178,102,300,'Уровень '+c.level+' · влияние');D.Text(h,560,100,238,20,Num(c.influence)+'/'+Num(c.next),15,C.INF,'r');
    Gauge.Ticks(h,178,126,620,16,52,c.influence/c.next,C.INF);
    [['Участники',c.members.length+'/'+CLAN_MAX,c.members.length/CLAN_MAX],['В сети',String(on),on/c.members.length],['Место',String(clanPlace()),1-(clanPlace()-1)/(RIVALS.length+1)]].forEach(([a,b,k],i)=>{
      const x=826+i*116;D.Line(h,x,20,1,132,C.LINE);D.Cap(h,x+16,24,96,a);D.Text(h,x+16,50,96,44,b,30,C.TEXT);Gauge.Ticks(h,x+16,120,86,14,12,k);
    });
    const cn=c.contracts,done=cn.filter(k=>k.state==='done'),left=cn.length-done.length,gain=done.reduce((a,k)=>a+k.reward,0);
    const a=UI.Card(r,0,188,380,224,'Контракты сегодня');
    D.Text(a,20,44,140,48,done.length+'/'+cn.length,36,C.TEXT);D.Text(a,150,44,210,48,left?'Осталось: '+left:'Все выполнены',15,left?C.DIM:C.OK,'r');
    Gauge.Ticks(a,20,100,340,14,40,cn.length?done.length/cn.length:0);
    D.Text(a,20,124,200,28,'Влияние за день',14,C.DIM);D.Text(a,220,124,140,28,'+'+Num(gain),16,C.INF,'r');
    UI.Button(a,20,164,340,40,'Контракты','secondary','contract',()=>Shell.go('contracts'),14);
    const b=UI.Card(r,396,188,380,224,'Склад денег'),tx=Clan.taxDays(),low=tx!==null&&tx<=7;
    UI.Money(b,20,50,c.bank,34);Gauge.Ticks(b,20,100,340,14,40,c.bank/BANK_MAX,C.GOLD);
    D.Text(b,20,124,200,28,tx===null?'Без налога':'Налога хватит на',14,low?C.BAD:C.DIM);D.Text(b,220,124,140,28,(tx===null?c.taxFree:tx)+' дн.',16,low?C.BAD:C.GOLD,'r');
    UI.Button(b,20,164,340,40,'Склад денег','secondary','vault',()=>Shell.go('bank'),14);
    const t=UI.Card(r,792,188,380,224,'Лучшие за сегодня'),top=c.members.filter(m=>m.today>0).sort((x,y)=>y.today-x.today).slice(0,3);
    if(!top.length)D.Text(t,0,84,380,30,'Сегодня вклада ещё нет',15,C.DIM2,'c');
    top.forEach((m,i)=>{const y=42+i*38;D.Text(t,20,y,24,36,String(i+1),17,i?C.DIM:C.ACCENT);D.Circle(t,46,y+14,8,m.online?C.OK:C.DIM2);D.Text(t,64,y,200,36,Fit(m.name,200,16),16,C.TEXT);D.Text(t,264,y,96,36,'+'+m.today,16,C.INF,'r');});
    UI.Button(t,20,164,340,40,'Все участники','secondary','users',()=>Shell.go('cmembers'),14);
    const n=UI.Card(r,0,428,572,232,'Последняя новость'),w=c.news[0];
    if(w){
      const g=NEWS_TAGS[w.tag];Gauge.Pill(n,20,44,g.name,g.color);D.Text(n,300,44,252,24,w.author+' · '+w.time,13,C.DIM2,'r');
      D.Text(n,20,74,532,32,Fit(w.title,532,20),20,C.TEXT);D.MText(n,20,108,532,50,w.text.length>130?w.text.slice(0,130)+'…':w.text,15,C.DIM);
    }else D.Text(n,0,84,572,30,'Новостей пока нет',15,C.DIM2,'c');
    UI.Button(n,20,172,532,40,'Все новости','secondary','news',()=>Shell.go('cnews'),14);
    const j=UI.Card(r,588,428,584,232,'Журнал клана');
    c.journal.slice(0,3).forEach(([ic,x,when,who,val],k)=>{const y=44+k*40,l=x+(val?', '+val:'');D.Icon(j,20,y+7,18,ic,k?C.DIM2:C.ACCENT);D.Text(j,50,y,350,32,Fit(l,350,15),15,C.TEXT);D.Text(j,404,y,160,32,when,13,C.DIM2,'r');});
    UI.Button(j,20,172,544,40,'Весь журнал','secondary','clock',()=>Shell.go('cjournal'),14);
    if(Clan.can('color'))UI.Button(r,0,676,190,44,'Настройки клана','secondary','sym_cog',()=>Shell.go('csettings'),14);
    const dc=D.Frame(r,206,676,760,44);D.Box(dc,0,0,760,44,8,C.PANEL2);D.Icon(dc,14,11,22,'discord',c.discord?C.ACCENT:C.DIM2);D.Cap(dc,46,13,130,'Discord клана');D.Text(dc,180,0,450,44,c.discord?Fit(c.discord,450,15):'Не указан',15,c.discord?C.TEXT:C.DIM2);
    if(c.discord)UI.Button(dc,716,6,36,32,null,'secondary','copy',()=>Copy('https://'+c.discord));else if(Clan.can('color'))UI.Button(dc,640,6,112,32,'Указать','secondary',null,()=>Shell.go('csettings'),13);
    const lead=Clan.isLeader();UI.Button(r,982,676,190,44,lead?'Распустить клан':'Покинуть клан','danger','logout',()=>{if(lead)UI.Confirm({title:'Распустить клан?',text:'Удалится всё: склад с деньгами, банкомат, ранги, новости, журнал, метки на карте, место в рейтинге. Это нельзя отменить.',word:c.name,yes:'Распустить',fn:()=>{Clan.dissolve();Toast('Клан распущен','inf');Shell.go('profile');}});else UI.Ask({title:'Покинуть клан?',text:'Вклад и доступ к складу денег будут потеряны.',yes:'Подтвердить',danger:true,fn:()=>{S.clan=null;Shell.go('profile');Panel.sync();}});},14);
  },
};

// Участники: слева состав и приглашение, справа карточка выбранного. Выбран всегда кто-то, по умолчанию сам игрок.
Pages.cmembers={
  sort:'rank',
  build(r){
    const c=S.clan;this.r=r;this.sel=window.PICK?c.members[window.PICK]:Clan.me();window.PICK=0;
    const t=UI.Card(r,0,0,664,720);this.head=D.Frame(t,0,0,664,46);D.Line(t,0,46,664,1,C.LINE);this.rows=D.Clip(t,0,47,664,672);this.rows.classList.add('scroll');this.paintRows();
    this.cardF=D.Frame(r,680,0,492,720);this.paintCard();
    this.sent=new Set();
    if(Clan.can('invite')){this.invBtn=UI.Button(Shell.top,840,16,150,40,'Пригласить','primary','invite',()=>this.invite(),14);this.reqBtn=UI.Button(Shell.top,680,16,150,40,'Заявки · '+c.requests.length,'secondary','mail',()=>this.requests(),14);if(window.INV){window.INV=0;this.invite();}}
  },
  leave(){this.closePop();[this.invBtn,this.reqBtn].forEach(b=>b&&b.remove());this.invBtn=this.reqBtn=null;},
  // Заявки в клан: принимают те, кому ранг разрешает приглашать.
  requests(){
    const c=S.clan,n=c.requests.length,shown=Math.min(3,n),w=680,h=n?84+shown*172:150,b=UI.Modal(this,w,h);
    D.Text(b,28,22,400,32,'Заявки в клан',22,C.TEXT);D.Text(b,w-132,22,70,32,c.members.length+'/'+CLAN_MAX,15,C.DIM,'r');
    if(!n){D.Text(b,28,76,w-56,32,'Новых заявок нет',16,C.DIM2);return;}
    const L=D.Clip(b,0,72,w,shown*172);L.classList.add('scroll');D.Frame(L,0,0,1,n*172);
    c.requests.forEach((q,i)=>{
      const y=i*172,ra=RANKS[Rank.of(q.xp)];D.Box(L,28,y,624,160,8,C.PANEL2);Gauge.Insignia(L,42,y+12,36,ra);D.Text(L,92,y+8,240,26,Fit(q.name,240,17),17,C.TEXT);D.Text(L,92,y+34,240,20,ra.name+' · '+q.time,13,C.DIM);
      D.Text(L,340,y+10,256,30,q.discord?Fit('@'+q.discord,256,14):'Discord не указан',14,q.discord?C.TEXT:C.DIM2,'r');if(q.discord)UI.Button(L,604,y+10,36,30,null,'secondary','copy',()=>Copy(q.discord));
      D.MText(L,42,y+58,596,64,q.text,15,C.DIM);
      UI.Button(L,42,y+120,140,32,'Принять','primary','check',()=>{if(Clan.accept(q))this.afterRequest();},13);UI.Button(L,192,y+120,140,32,'Отклонить','secondary','close',()=>{c.requests=c.requests.filter(x=>x!==q);Toast('Заявка отклонена','inf');this.afterRequest();},13);
    });
  },
  afterRequest(){this.closePop();this.reqBtn.label.textContent='Заявки · '+S.clan.requests.length;this.paintRows();Shell.navPaint();this.requests();},
  esc(){this.closePop();},
  closePop(){if(this.pop){this.pop.remove();this.pop=null;}},
  // Попап приглашения: поле для ника и игроки рядом, кроме тех, кто уже в клане.
  invite(){
    const c=S.clan,W=1468,H=848,w=520,full=c.members.length>=CLAN_MAX,near=S.near.filter(n=>!c.members.some(m=>m.name===n.name)).slice(0,5),h=full?136:168+Math.max(1,near.length)*48+16,m=this.pop=D.Frame(Shell.screen,0,0,W,H),close=()=>this.closePop();m.style.zIndex=60;
    D.Line(m,0,0,W,H,'rgba(0,0,0,.6)');D.Hit(m,0,0,W,H,close);
    const b=D.Frame(m,(W-w)/2,(H-h)/2,w,h);b.style.boxShadow='0 16px 44px rgba(0,0,0,.6)';D.Box(b,0,0,w,h,12,C.PANEL,C.LINE2);
    D.Text(b,28,22,300,32,'Пригласить в клан',22,C.TEXT);D.Text(b,w-132,22,70,32,c.members.length+'/'+CLAN_MAX,15,C.DIM,'r');
    const ci=D.Icon(b,w-46,28,20,'close',C.DIM);D.Hit(b,w-54,20,36,36,close,o=>ci.style.color=o?C.TEXT:C.DIM);
    if(full){D.Text(b,28,72,w-56,32,'В клане нет свободных мест',16,C.DIM);return;}
    const send=x=>{this.sent.add(x);Toast('Приглашение отправлено: '+x);};
    const i=UI.Input(b,28,72,326,44,'Ник игрока',24),call=()=>{const v=i.input.value.trim();if(!v)return;if(c.members.some(q=>q.name.toLowerCase()===v.toLowerCase())){Toast('Этот игрок уже в клане','err');return;}send(v);i.input.value='';paint();};
    UI.Button(b,364,72,128,44,'Позвать','primary',null,call);i.input.onkeydown=e=>{if(e.key==='Enter')call();};i.input.focus();
    D.Line(b,28,132,464,1,C.LINE);D.Cap(b,28,146,300,'Рядом');
    const L=D.Frame(b,0,168,w,h-168),paint=()=>{
      L.innerHTML='';if(!near.length)D.Text(L,28,0,w-56,44,'Рядом никого нет',15,C.DIM2);
      near.forEach((n,k)=>{const y=k*48,s=this.sent.has(n.name);D.Text(L,28,y,220,44,Fit(n.name,220,16),16,C.TEXT);D.Text(L,260,y,90,44,n.dist+' м',14,C.DIM);const e=UI.Button(L,364,y+4,128,36,s?'Отправлено':'Позвать','secondary',null,()=>{send(n.name);paint();},14);if(s)e.classList.add('off');});
    };paint();
  },
  list(){
    const by={name:(a,b)=>a.name.localeCompare(b.name),rank:(a,b)=>Clan.level(a)-Clan.level(b)||b.total-a.total,today:(a,b)=>b.today-a.today,total:(a,b)=>b.total-a.total,online:(a,b)=>(b.online?1:0)-(a.online?1:0)};
    return [...S.clan.members].sort(by[this.sort]);
  },
  paintRows(){
    const R=this.rows,top=R.scrollTop,h=this.head,l=this.list();h.innerHTML='';R.innerHTML='';
    [['name','Игрок',42,150],['rank','Ранг',212,110],['today','Сегодня',340,80],['total','Всего',430,80],['online','В сети',520,124]].forEach(([k,t,x,w])=>{
      const on=this.sort===k,e=D.Cap(h,x,18,w,t,on?C.ACCENT:undefined);D.Hit(h,x-6,8,w,32,()=>{this.sort=k;this.paintRows();},o=>{if(!on)e.style.color=o?C.TEXT:C.DIM;});
    });
    D.Frame(R,0,0,1,l.length*48+6);
    l.forEach((m,i)=>{
      const y=i*48,sel=this.sel===m,bg=D.Box(R,8,y+3,640,42,6,sel?C.PANEL3:m.me?C.PANEL2:'transparent',sel?C.ACCENT:null),ra=Clan.rank(m);
      if(m.me)D.Box(R,8,y+11,3,26,2,C.ACCENT);
      D.Circle(R,24,y+20,8,m.online?C.OK:C.DIM2);D.Text(R,42,y,160,48,Fit(m.name,160,17),17,C.TEXT);
      Gauge.Pill(R,212,y+12,Fit(ra.name,92,13),ra.color);
      D.Text(R,340,y,80,48,m.today?'+'+m.today:'0',15,m.today?C.INF:C.DIM2);D.Text(R,430,y,80,48,Num(m.total),15,C.DIM);D.Text(R,520,y,124,48,m.last,14,C.DIM2);
      D.Hit(R,8,y+3,640,42,()=>this.pick(m),o=>{if(!sel&&!m.me)bg.style.background=o?C.PANEL2:'transparent';});
    });
    R.scrollTop=top;
  },
  pick(m){this.sel=m;this.paintRows();this.paintCard();},
  paintCard(){
    const p=this.cardF,m=this.sel,c=S.clan,st=memberStats(m),ri=Rank.of(st.xp),ra=RANKS[ri],nx=RANKS[ri+1],cr=Clan.rank(m),act=!m.me&&Clan.over(m)&&(Clan.can('assign')||Clan.can('kick'));p.innerHTML='';
    const f=D.Frame(p,0,0,492,720);D.Box(f,0,0,492,720,10,C.PANEL,C.LINE);Gauge.Glow(f,1,1,490,718,10);Gauge.Corners(f,492,720,C.ACCENT,14);
    D.Text(f,24,16,440,40,Fit(m.name,440,30),30,C.TEXT);D.Circle(f,24,66,10,m.online?C.OK:C.DIM2);D.Text(f,42,58,200,26,m.online?'В сети':m.last,14,C.DIM);Gauge.Pill(f,492-24-TextW(Fit(cr.name,160,13),13)-22,58,Fit(cr.name,160,13),cr.color);
    D.Line(f,24,94,444,1,C.LINE);
    const dc=memberDiscord(m);D.Box(f,24,104,444,44,8,C.PANEL2);D.Icon(f,36,115,22,'discord',dc?C.ACCENT:C.DIM2);D.Cap(f,68,117,80,'Discord');D.Text(f,146,104,270,44,dc?Fit('@'+dc,270,16):'Не указан',16,dc?C.TEXT:C.DIM2);
    if(m.me)UI.Button(f,424,110,36,32,null,'secondary',dc?'edit':'plus',()=>Pages.profile.editDiscord());else if(dc)UI.Button(f,424,110,36,32,null,'secondary','copy',()=>Copy(dc));
    Gauge.Insignia(f,24,158,56,ra);D.Text(f,94,156,374,30,ra.name,22,C.TEXT);D.Text(f,94,186,374,22,nx?Num(st.xp)+'/'+Num(nx.need)+' опыта':Num(st.xp)+' опыта',14,C.DIM);Gauge.Ticks(f,24,222,444,10,44,nx?(st.xp-ra.need)/(nx.need-ra.need):1);
    [['Заражённых',Num(st.zombies)],['Мутантов',Num(st.mutants)],['Животных',Num(st.animals)],['Пройдено',(st.dist/1000).toFixed(1).replace('.',',')+' км'],['Эта жизнь',lifeText(st.life)],['Рекорд жизни',lifeText(st.record)]].forEach(([a,b],i)=>{
      const x=24+(i%3)*151,y=246+Math.floor(i/3)*70;D.Box(f,x,y,142,62,8,C.PANEL2);D.Text(f,x+12,y+7,124,18,a.toUpperCase(),12,C.DIM);D.Text(f,x+12,y+28,124,28,b,17,i===5?C.GOLD:C.TEXT);
    });
    D.Line(f,24,392,444,1,C.LINE);
    [['Вклад',Num(m.total),C.INF],['Сегодня',m.today?'+'+m.today:'0',m.today?C.INF:C.DIM2],['Контрактов',Num(st.contracts),C.TEXT]].forEach(([a,b,col],i)=>{const x=24+i*151;D.Cap(f,x,404,140,a);D.Text(f,x,424,140,32,b,22,col);});
    D.Line(f,24,466,444,1,C.LINE);
    const wk=st.week,mx=Math.max(1,...wk),segs=act?10:18,base=500+segs*9,today=(new Date().getDay()+6)%7,days=['пн','вт','ср','чт','пт','сб','вс'];
    D.Cap(f,24,478,240,'Вклад за неделю');D.Text(f,300,476,168,20,'+'+Num(wk.reduce((x,y)=>x+y,0)),15,C.INF,'r');
    wk.forEach((v,i)=>{const x=24+i*65,lit=v>0?Math.max(1,Math.round(segs*v/mx)):0;for(let k=0;k<segs;k++)D.Line(f,x,base-k*9,54,6,k<lit?(i===today?C.ACCENT:C.INF):C.PANEL3);D.Text(f,x,base+10,54,18,days[i],13,i===today?C.ACCENT:C.DIM2,'c');});
    if(act){
      D.Line(f,24,634,444,1,C.LINE);
      const lim=Clan.can('assign')&&Clan.can('kick');
      if(Clan.can('assign')){const opts=c.ranks.slice(Clan.level(Clan.me())+1);UI.Dropdown(f,24,650,lim?250:444,opts.map(r=>r.name),Math.max(0,opts.indexOf(cr)),44,i=>{m.rank=opts[i].id;c.journal.unshift(['rank',m.name+' получил ранг «'+opts[i].name+'»','сейчас',m.name]);Toast(m.name+': '+opts[i].name);this.paintRows();this.paintCard();},true);}
      if(Clan.can('kick'))UI.Button(f,lim?286:24,650,lim?182:444,44,'Исключить','danger','kick',()=>UI.Ask({title:'Исключить из клана?',text:m.name+' потеряет доступ к контрактам и складу.',yes:'Исключить',danger:true,fn:()=>{c.members=c.members.filter(x=>x!==m);c.journal.unshift(['kick',m.name+' исключён из клана','сейчас',m.name]);Shell.refresh();}}),14);
    }
  },
};

// Чат клана: пишут все участники, новые сообщения видны всем, кто в клане. Хранятся последние CHAT_MAX.
Pages.cchat={
  last:0,
  build(r){
    const c=S.clan;this.r=r;c.chatUnread=0;Shell.navPaint();
    const t=UI.Card(r,0,0,856,720);this.list=D.Clip(t,0,1,856,630);this.list.classList.add('scroll');D.Line(t,0,632,856,1,C.LINE);
    this.inp=UI.Input(t,20,652,600,48,'Сообщение клану',CHAT_LEN);this.cnt=D.Text(t,626,652,58,48,'0/'+CHAT_LEN,13,C.DIM2,'r');UI.Button(t,692,652,144,48,'Отправить','primary','send',()=>this.send(),14);
    this.inp.input.oninput=()=>{this.cnt.textContent=this.inp.input.value.length+'/'+CHAT_LEN;};
    this.inp.input.onkeydown=e=>{if(e.key==='Enter')this.send();};
    this.paintSide();this.paintList();this.inp.input.focus();
  },
  send(){
    const v=this.inp.input.value.trim(),now=Date.now();if(!v)return;
    if(now-this.last<CHAT_GAP*1000){Toast('Подождите пару секунд','err');return;}
    if(Clan.banned(v)){Toast('В сообщении запрещённое слово','err');return;}
    this.last=now;Clan.say(S.name,v);this.inp.input.value='';this.cnt.textContent='0/'+CHAT_LEN;this.paintList();
  },
  paintList(){
    const L=this.list,c=S.clan;L.innerHTML='';
    if(!c.chat.length){D.Text(L,0,296,856,30,'Сообщений пока нет',17,C.DIM2,'c');return;}
    let y=16,prev=null,day=null;
    c.chat.forEach(m=>{
      if(m.day!==day){day=m.day;prev=null;D.Text(L,0,y,848,20,m.day,13,C.DIM2,'c');y+=32;}
      const own=m.who===S.name,mate=c.members.find(q=>q.name===m.who),lines=Wrap(m.text,520,15),tw=Math.ceil(Math.max(...lines.map(l=>TextW(l,15)))),bw=tw+30,bh=lines.length*22+14,x=own?828-bw:20;
      if(m.who!==prev){
        if(prev)y+=10;
        if(own)D.Text(L,628,y,200,20,m.time,12,C.DIM2,'r');
        else{const nw=TextW(m.who,14)+6;D.Text(L,20,y,nw,20,m.who,14,mate?Clan.rank(mate).color:C.DIM);D.Text(L,20+nw+6,y,80,20,m.time,12,C.DIM2);}
        y+=22;
      }
      D.Box(L,x,y,bw,bh,10,own?C.PANEL3:C.PANEL2,own?C.ACCENT:null);lines.forEach((l,i)=>D.Text(L,x+15,y+7+i*22,tw+4,22,l,15,C.TEXT));
      y+=bh+6;prev=m.who;
    });
    D.Frame(L,0,y,1,10);L.scrollTop=L.scrollHeight;
  },
  paintSide(){
    const c=S.clan,on=c.members.filter(m=>m.online).length,t=UI.Card(this.r,872,0,300,720,'Участники'),l=[...c.members].sort((a,b)=>(b.online?1:0)-(a.online?1:0)||Clan.level(a)-Clan.level(b));D.Text(t,160,14,120,22,on+'/'+c.members.length,15,C.DIM,'r');
    const L=D.Clip(t,0,46,300,672);L.classList.add('scroll');D.Frame(L,0,0,1,l.length*44+6);
    l.forEach((m,i)=>{
      const y=i*44,bg=D.Box(L,8,y+2,276,40,6,'transparent'),ra=Clan.rank(m);D.Circle(L,20,y+18,8,m.online?C.OK:C.DIM2);D.Text(L,38,y,240,28,Fit(m.name,240,15),15,m.online?C.TEXT:C.DIM);D.Text(L,38,y+21,240,18,Fit(ra.name,240,12),12,ra.color);
      D.Hit(L,8,y+2,276,40,()=>{window.PICK=c.members.indexOf(m);Shell.go('cmembers');},o=>{bg.style.background=o?C.PANEL2:'transparent';});
    });
  },
};

// Журнал клана: сверху шкала недели и типы событий, ниже лента с осью времени.
Pages.cjournal={
  build(r){
    this.r=r;this.kind='all';this.day=null;this.top=D.Frame(r,0,0,1172,152);
    const t=UI.Card(r,0,168,1172,552);this.list=D.Clip(t,0,1,1172,550);this.list.classList.add('scroll');this.paint();
  },
  dayName(e){return e[2].includes(', ')?e[2].split(', ')[0]:'сегодня';},
  // Сколько дней назад была запись, -1 если старше недели.
  dayOf(e){const d=this.dayName(e);for(let i=0;i<7;i++)if(dayLabel(i)===d)return i;return -1;},
  byKind(e){return this.kind==='all'||journalKind(e[0])===this.kind;},
  byDay(e){return this.day===null||this.dayOf(e)===this.day;},
  paint(){this.paintTop();this.paintList();},
  paintTop(){
    const f=this.top,c=S.clan,wk=['вс','пн','вт','ср','чт','пт','сб'],days=[6,5,4,3,2,1,0],lists=days.map(i=>c.journal.filter(e=>this.dayOf(e)===i&&this.byKind(e))),mx=Math.max(8,...lists.map(l=>l.length));
    f.innerHTML='';D.Box(f,0,0,1172,152,10,C.PANEL,C.LINE);Gauge.Glow(f,1,1,1170,150,10);
    D.Cap(f,24,16,300,'Активность за неделю');
    days.forEach((i,n)=>{
      const x=24+n*82,on=this.day===i,d=new Date(Date.now()-i*864e5),bg=D.Box(f,x-6,38,76,106,6,on?C.PANEL3:'transparent',on?C.ACCENT:null),seg=[];
      JOURNAL_KINDS.forEach(k=>lists[n].filter(e=>journalKind(e[0])===k[0]).forEach(()=>seg.push(k[3])));
      for(let k=0;k<8;k++)D.Line(f,x,110-k*9,64,6,seg[Math.floor(k*mx/8)]||C.PANEL3);
      D.Text(f,x-6,120,76,20,wk[d.getDay()]+' '+d.getDate(),13,i?C.DIM:C.ACCENT,'c');
      D.Hit(f,x-6,38,76,106,()=>{this.day=on?null:i;this.paint();},o=>{if(!on)bg.style.background=o?C.PANEL2:'transparent';});
    });
    D.Line(f,606,18,1,116,C.LINE);
    JOURNAL_KINDS.forEach(([k,t,ic,col],i)=>{
      const x=624+(i%2)*268,y=16+(i>>1)*64,on=this.kind===k,n=c.journal.filter(e=>journalKind(e[0])===k&&this.byDay(e)).length,bg=D.Box(f,x,y,260,56,8,on?C.PANEL3:C.PANEL2,on?col:null);
      D.Box(f,x+10,y+14,3,28,2,col);D.Icon(f,x+24,y+17,22,ic,col);D.Text(f,x+58,y,130,56,t,16,on?C.TEXT:C.DIM);D.Text(f,x+190,y,56,56,String(n),18,n?C.TEXT:C.DIM2,'r');
      D.Hit(f,x,y,260,56,()=>{this.kind=on?'all':k;this.paint();},o=>{if(!on)bg.style.background=o?C.PANEL3:C.PANEL2;});
    });
  },
  paintList(){
    const L=this.list,groups=[],week=['воскресенье','понедельник','вторник','среда','четверг','пятница','суббота'];L.innerHTML='';L.scrollTop=0;
    S.clan.journal.filter(e=>this.byKind(e)&&this.byDay(e)).forEach(e=>{const d=this.dayName(e);let g=groups[groups.length-1];if(!g||g.d!==d)groups.push(g={d,i:this.dayOf(e),rows:[]});g.rows.push(e);});
    if(!groups.length){D.Text(L,0,250,1172,30,'Записей нет',17,C.DIM2,'c');return;}
    let y=14;
    groups.forEach((g,gi)=>{
      const h=Math.max(92,g.rows.length*52),date=g.i>=0?new Date(Date.now()-g.i*864e5):null;
      if(date){D.Text(L,24,y+6,100,46,String(date.getDate()),36,g.i?C.DIM:C.TEXT);D.Cap(L,24,y+54,110,g.i===0?'сегодня':g.i===1?'вчера':week[date.getDay()],g.i?C.DIM2:C.ACCENT);}
      else D.Text(L,24,y+6,110,40,g.d,16,C.DIM);
      if(g.rows.length>1)D.Line(L,219,y+24,2,(g.rows.length-1)*52,C.LINE2);
      g.rows.forEach(([ic,t,w,who,val],k)=>{
        const yy=y+k*52,kind=JOURNAL_KINDS.find(q=>q[0]===journalKind(ic)),neg=val&&val[0]==='−',col=ic==='kick'?C.BAD:kind[3],at=w.includes(', ')?w.split(', ')[1]:w,free=val?724:876;
        D.Text(L,130,yy,66,48,at,14,C.DIM2,'r');D.Box(L,204,yy+8,32,32,16,C.PANEL2,col);D.Icon(L,212,yy+16,16,ic,col);
        if(who&&t.startsWith(who)){const nw=TextW(who,17)+6;D.Text(L,252,yy,nw,48,who,17,C.TEXT);D.Text(L,252+nw,yy,free-nw,48,Fit(t.slice(who.length).trim(),free-nw,16),16,C.DIM);}
        else D.Text(L,252,yy,free,48,Fit(t,free,16),16,C.TEXT);
        if(val)D.Text(L,980,yy,150,48,val,19,neg?C.BAD:kind[3],'r');
      });
      y+=h+6;if(gi<groups.length-1){D.Line(L,24,y,1106,1,C.LINE);y+=8;}
    });
    D.Frame(L,0,y,1,12);
  },
};

Pages.contracts={
  tab:0,
  build(r){this.r=r;this.tabs=D.Frame(Shell.top,180,0,360,72);this.paint();},
  leave(){if(this.tabs){this.tabs.remove();this.tabs=null;}},
  paint(){
    const t=this.tabs,r=this.r;t.innerHTML='';r.innerHTML='';let x=0;
    ['Клановые','Личные'].forEach((n,i)=>{const w=TextW(n,18)+8,on=i===this.tab,l=D.Text(t,x,0,w,72,n,18,on?C.TEXT:C.DIM);if(on)D.Line(t,x,68,w,3,C.ACCENT);D.Hit(t,x-10,0,w+20,72,()=>{this.tab=i;this.paint();},o=>{if(!on)l.style.color=o?C.TEXT:C.DIM;});x+=w+40;});
    if(this.tab)this.mine(r);else this.clan(r);
  },
  // Личные: три задания на день, видит и забирает только сам игрок.
  mine(r){
    const c=S.clan,l=c.my,done=l.filter(k=>k.state==='done'),gain=done.reduce((a,k)=>a+k.reward,0);
    const f=D.Frame(r,0,0,1172,112);D.Box(f,0,0,1172,112,10,C.PANEL,C.LINE);Gauge.Glow(f,1,1,1170,110,10);
    D.Circle(f,20,23,10,C.ACCENT);D.Cap(f,40,19,230,'Новые задания через');this.timer=D.Text(f,20,46,250,48,'00:00:00',38,C.TEXT);
    D.Line(f,292,18,1,76,C.LINE);D.Cap(f,312,19,220,'Выполнено сегодня');D.Text(f,312,44,120,36,done.length+'/'+l.length,28,C.TEXT);
    l.forEach((k,i)=>D.Box(f,312+i*32,86,26,8,2,k.state==='done'?C.ACCENT:k.state==='ready'?C.GOLD:C.PANEL3));
    D.Line(f,540,18,1,76,C.LINE);D.Cap(f,560,19,200,'Влияние клану');D.Icon(f,560,52,24,'sym_bolt',C.INF);D.Text(f,592,46,160,40,'+'+gain,30,C.INF);
    D.Line(f,770,18,1,76,C.LINE);D.Cap(f,790,19,240,'Уровень клана '+c.level);D.Text(f,790,40,362,30,MY_INFO[c.level-1],20,C.TEXT);D.Text(f,790,74,362,22,'Замена задания на сегодня: '+c.myReplace+'/1',14,c.myReplace?C.DIM:C.DIM2);
    l.forEach((k,i)=>this.card(r,0,128+i*200,k,844));
    const u=D.Frame(r,856,128,316,226);D.Box(u,0,0,316,226,10,C.PANEL2);Gauge.Corners(u,316,226,C.ACCENT,14);D.Cap(u,20,18,276,'Награда за задание');
    TIERS.forEach((t,i)=>{const y=48+i*40;D.Text(u,20,y,170,34,t.name,16,C.TEXT);D.Text(u,190,y,106,34,'+'+MY_POOL[i][0].reward,16,C.INF,'r');});D.Text(u,20,172,276,40,'И +'+CONTRACT_XP+' опыта в общий ранг',14,C.ACCENT);
    this.contrib(r);this.t=1;this.tick(1);
  },
  contrib(r){
    const c=S.clan,t=UI.Card(r,856,370,316,350,'Вклад сегодня'),list=[...c.members].sort((a,b)=>b.today-a.today),max=Math.max(1,list[0].today);
    list.slice(0,5).forEach((m,i)=>{const y=50+i*58;D.Text(t,20,y,190,24,Fit(m.name,190,16),16,m.me?C.ACCENT:C.TEXT);D.Text(t,210,y,86,24,m.today?'+'+m.today:'0',15,m.today?C.INF:C.DIM2,'r');Gauge.Ticks(t,20,y+30,276,10,30,m.today/max,C.INF);});
  },
  claimMy(k){
    const c=S.clan,me=Clan.me();k.state='done';c.influence+=k.reward;c.weekInf+=k.reward;me.today+=k.reward;me.total+=k.reward;S.doneContracts++;
    const up=this.levelUp();if(!up)Toast('Получено +'+k.reward+' влияния клану','inf');Shell.refresh();
  },
  // Замена: одна в день, только для начатого, но не выполненного задания.
  replace(k){
    const c=S.clan;if(!c.myReplace||k.state!=='active')return;const fresh=MY_POOL[k.tier].find(q=>!c.my.some(x=>x.title===q.title));if(!fresh){Toast('Заменить пока не на что','err');return;}
    Object.assign(k,fresh,{have:0,state:'active'});c.myReplace=0;Toast('Задание заменено');Shell.refresh();
  },
  levelUp(){
    const c=S.clan;let up=false;while(LEVELS[c.level]&&c.influence>=LEVELS[c.level]){c.level++;up=true;}c.next=LEVELS[c.level]||LEVELS[c.level-1];
    if(up){c.journal.unshift(['star','Клан достиг '+c.level+' уровня','сейчас']);Toast('Уровень '+c.level+': новые задания со следующего обновления','inf');}return up;
  },
  clan(r){
    const c=S.clan,done=c.contracts.filter(k=>k.state==='done').length,gain=c.contracts.filter(k=>k.state==='done').reduce((a,k)=>a+k.reward,0);
    const f=D.Frame(r,0,0,1172,112);D.Box(f,0,0,1172,112,10,C.PANEL,C.LINE);Gauge.Glow(f,1,1,1170,110,10);
    D.Circle(f,20,23,10,C.ACCENT);D.Cap(f,40,19,230,'Новые контракты через');this.timer=D.Text(f,20,46,250,48,'00:00:00',38,C.TEXT);
    D.Line(f,292,18,1,76,C.LINE);D.Cap(f,312,19,220,'Выполнено сегодня');D.Text(f,312,44,120,36,done+'/'+c.contracts.length,28,C.TEXT);
    c.contracts.forEach((k,i)=>D.Box(f,312+i*32,86,26,8,2,k.state==='done'?C.ACCENT:C.PANEL3));
    D.Line(f,540,18,1,76,C.LINE);D.Cap(f,560,19,200,'Влияние за день');D.Icon(f,560,52,24,'sym_bolt',C.INF);D.Text(f,592,46,160,40,'+'+gain,30,C.INF);
    D.Line(f,770,18,1,76,C.LINE);D.Cap(f,790,19,200,'Уровень '+c.level);D.Text(f,930,17,222,20,Num(c.influence)+'/'+Num(c.next),15,C.INF,'r');
    Gauge.Ticks(f,790,48,362,16,30,c.influence/c.next,C.INF);
    D.Text(f,790,74,362,22,c.level<LEVELS.length?'Осталось '+Num(Math.max(0,c.next-c.influence))+' до уровня '+(c.level+1):'Максимальный уровень',14,C.DIM);
    c.contracts.forEach((k,i)=>this.card(r,(i%2)*428,128+Math.floor(i/2)*200,k));
    const u=D.Frame(r,856,128,316,226);D.Box(u,0,0,316,226,10,C.PANEL2);Gauge.Corners(u,316,226,C.ACCENT,14);
    if(c.level<LEVELS.length){D.Cap(u,20,18,276,'Откроется на уровне '+(c.level+1));D.MText(u,20,48,276,100,LEVEL_INFO[c.level],20,C.TEXT);}
    else{D.Cap(u,20,18,276,'Уровень '+c.level);D.MText(u,20,48,276,100,'Все контракты открыты',20,C.TEXT);}
    UI.Button(u,20,164,276,44,'Все уровни','secondary','rank',()=>this.levels());
    this.contrib(r);this.t=1;this.tick(1);
  },
  card(r,x,y,k,w=412){
    const h=192,e=w-20,f=UI.Card(r,x,y,w,h),tier=TIERS[k.tier],ready=k.state==='ready';if(k.state==='done')f.style.opacity=.62;
    if(ready){Gauge.Glow(f,1,1,w-2,h-2,10);Gauge.Corners(f,w,h,C.ACCENT,14);}
    if(k.tier===2){const s=D.Clip(f,0,0,w,6);s.style.borderRadius='10px 10px 0 0';const e=Gauge.Hazard(s,0,0,w,6);e.style.background='repeating-linear-gradient(135deg,var(--bad) 0 10px,transparent 10px 20px)';}
    else if(k.tier===1)D.Box(f,0,18,3,56,2,tier.color);
    D.Box(f,20,20,52,52,10,C.PANEL2);D.Icon(f,33,33,26,k.icon,k.state==='active'?(k.tier?tier.color:C.DIM):C.ACCENT);
    D.Text(f,88,18,210,30,k.title,20,C.TEXT);D.Text(f,88,48,300,24,k.text,15,C.DIM);
    const tw0=TextW(tier.name.toUpperCase(),12)+20;D.Box(f,e-tw0,22,tw0,22,11,null,tier.color);D.Text(f,e-tw0,22,tw0,22,tier.name.toUpperCase(),12,tier.color,'c');
    const p=Math.round(k.have/k.need*100);D.Text(f,20,90,240,22,k.have+'/'+k.need,16,C.TEXT);D.Text(f,e-100,90,100,22,p+'%',15,C.DIM,'r');Gauge.Ticks(f,20,116,e-20,12,Math.round((e-20)/10.3),k.have/k.need,k.state==='active'?C.ACCENT:C.OK);
    D.Icon(f,20,150,18,'sym_bolt',C.INF);let tw=TextW('+'+k.reward,16)+8;D.Text(f,44,144,tw,30,'+'+k.reward,16,C.INF);
    if(k.money)UI.Money(f,44+tw+18,149,k.money,16);
    if(k.my){const xt='+'+CONTRACT_XP+' опыта',x0=44+tw+18;D.Icon(f,x0,150,18,'rank',C.ACCENT);D.Text(f,x0+24,144,TextW(xt,16)+8,30,xt,16,C.ACCENT);}
    if(ready)UI.Button(f,e-120,140,120,38,'Забрать','primary',null,()=>this.claimMy(k),14);
    else if(k.state==='done'){const dw=TextW('Выполнен',14)+4;D.Icon(f,e-dw-22,151,16,'check',C.OK);D.Text(f,e-dw,144,dw,30,'Выполнен',14,C.OK);}
    else if(k.my&&k.state==='active'&&S.clan.myReplace)UI.Button(f,e-120,140,120,38,'Заменить','secondary','refresh',()=>this.replace(k),14);
  },
  // Клановый контракт засчитывается сам, как только набран прогресс: кнопки нет, награду начисляет сервер.
  complete(k){
    k.have=k.need;const c=S.clan,paid=k.money?Clan.deposit('Система','Контракт «'+k.title+'»',k.money):0;k.state='done';c.influence+=k.reward;c.weekInf+=k.reward;contractShares(k).forEach(([m,v])=>{m.today+=v;m.total+=v;if(m.me)S.doneContracts++;});
    c.journal.unshift(['check','Контракт «'+k.title+'» выполнен','сейчас','','+'+k.reward]);
    const up=this.levelUp();if(paid<k.money)Toast('Склад заполнен: '+Num(k.money-paid)+' не поместилось','err');else if(!up)Toast('Получено +'+k.reward+' влияния','inf');
    Shell.refresh();
  },
  // Все уровни: порог влияния и что открывается.
  levels(){
    const c=S.clan,W=1468,H=848,w=660,h=444,m=D.Frame(Shell.screen,0,0,W,H);m.style.zIndex=60;D.Line(m,0,0,W,H,'rgba(0,0,0,.6)');D.Hit(m,0,0,W,H,()=>m.remove());
    const b=D.Frame(m,(W-w)/2,(H-h)/2,w,h);b.style.boxShadow='0 16px 44px rgba(0,0,0,.6)';b.onclick=e=>e.stopPropagation();D.Box(b,0,0,w,h,12,C.PANEL,C.LINE2);Gauge.Corners(b,w,h,C.ACCENT,18);D.Text(b,28,22,w-120,30,'Уровни клана',22,C.TEXT);
    const ci=D.Icon(b,w-46,24,20,'close',C.DIM);D.Hit(b,w-54,16,36,36,()=>m.remove(),o=>ci.style.color=o?C.TEXT:C.DIM);
    LEVELS.forEach((need,i)=>{const n=i+1,y=68+i*70,has=c.level>=n,cur=c.level===n;
      D.Box(b,20,y,w-40,62,8,cur?C.PANEL3:C.PANEL2,cur?C.ACCENT:null);D.Text(b,36,y,44,62,String(n),24,has?C.ACCENT:C.DIM2,'c');
      D.Text(b,92,y+8,430,24,LEVEL_INFO[i],16,has?C.TEXT:C.DIM);D.Text(b,92,y+34,430,20,need?Num(need)+' влияния':'Сразу после создания',13,C.DIM2);D.Icon(b,w-58,y+21,20,has?'check':'lock',has?C.OK:C.DIM2);});
  },
  tick(dt){this.t+=dt;if(this.t<1)return;this.t=0;const d=new Date(),s=86400-(d.getHours()*3600+d.getMinutes()*60+d.getSeconds()),p=v=>String(v).padStart(2,'0');if(this.timer)this.timer.textContent=p(Math.floor(s/3600))+':'+p(Math.floor(s%3600/60))+':'+p(s%60);},
};

// Рейтинг кланов: виден всем игрокам. За неделю считается заработанное влияние, за всё время накопленное.
Pages.rating={
  mode:0,tab:0,
  build(r){
    this.r=r;
    // вкладки в верхней строке: рабочая область остаётся целиком под рейтинг
    this.tabs=D.Frame(Shell.top,160,0,360,72);this.paint();
  },
  leave(){this.closePop();if(this.tabs){this.tabs.remove();this.tabs=null;}},
  closePop(){if(this.pop){this.pop.remove();this.pop=null;}},
  esc(){this.closePop();},
  // Карточка игрока: открытые показатели. Discord здесь не показывается, он только для соклановцев.
  playerCard(p){
    const w=600,h=384,b=UI.Modal(this,w,h),mate=S.clan&&S.clan.members.find(m=>m.name===p.n&&!m.me),st=p.me?memberStats({me:true,name:p.n,total:0,today:0}):memberStats(mate||{name:p.n,total:Math.round(p.x/3),today:0}),vals=p.me?Ach.mine():Ach.of({...st,xp:p.x}),ra=RANKS[Rank.of(p.x)],s=D.Frame(b,28,28,112,112);
    D.Box(s,0,0,112,112,8,C.PANEL2);Gauge.Corners(s,112,112,C.ACCENT,10);Gauge.Insignia(s,20,20,72,ra);D.Text(b,160,26,380,38,Fit(p.n,380,26),26,C.TEXT);D.Text(b,160,64,380,24,ra.name,16,C.ACCENT);
    if(p.clan){UI.Emblem(b,160,100,24,p.clan.emblem);D.Text(b,192,98,348,28,Fit('['+p.clan.tag+'] '+p.clan.name,348,15),15,C.DIM);}else D.Text(b,160,98,300,28,'Без клана',15,C.DIM2);
    [['Заражённых',Num(st.zombies)],['Мутантов',Num(st.mutants)],['Животных',Num(st.animals)],['Пройдено',(st.dist/1000).toFixed(1).replace('.',',')+' км'],['Рекорд жизни',lifeText(st.record)],['Достижения',Ach.count(vals)+'/'+Ach.total()]].forEach(([a,v],i)=>{const x=28+(i%3)*184,y=156+Math.floor(i/3)*70;D.Box(b,x,y,176,62,8,C.PANEL2);D.Text(b,x+12,y+7,156,18,a.toUpperCase(),12,C.DIM);D.Text(b,x+12,y+28,156,28,Fit(v,156,17),17,i===4?C.GOLD:C.TEXT);});
    D.Cap(b,28,306,150,'Место');D.Text(b,28,326,150,34,String(p.place),24,C.ACCENT);D.Cap(b,212,306,150,'Опыт');D.Text(b,212,326,300,34,Num(p.x),24,C.TEXT);
  },
  // Открытая карточка клана: видна всем, отсюда подают заявку.
  clanCard(c){
    const mine=c.me?S.clan:c,w=600,h=mine.discord?408:352,b=UI.Modal(this,w,h),lead=c.me?S.clan.members.find(m=>m.rank==='leader').name:c.leader,s=D.Frame(b,28,28,112,112);D.Box(s,0,0,112,112,8,C.PANEL2);Gauge.Corners(s,112,112,C.ACCENT,10);UI.Emblem(s,14,14,84,c.emblem);
    D.Text(b,160,26,380,38,Fit(c.name,380,26),26,C.TEXT);D.Text(b,160,64,380,24,'['+c.tag+'] · уровень '+c.level,15,C.DIM);Gauge.Pill(b,160,100,mine.open?'Набор открыт':'Набор закрыт',mine.open?C.OK:C.DIM2);
    D.MText(b,28,156,w-56,28,mine.desc?'«'+Fit(mine.desc,w-90,15)+'»':'Слогана нет',15,C.DIM);D.Line(b,28,190,w-56,1,C.LINE);
    [['Участники',c.members+'/'+CLAN_MAX],['Место',String(c.place)],['Лидер',Fit(lead,190,20)]].forEach(([a,v],i)=>{const x=28+[0,160,290][i];D.Cap(b,x,202,150,a);D.Text(b,x,222,i===2?254:150,32,v,20,C.TEXT);});
    if(mine.discord){D.Box(b,28,266,w-56,44,8,C.PANEL2);D.Icon(b,42,277,22,'discord',C.ACCENT);D.Text(b,76,266,430,44,Fit(mine.discord,430,15),15,C.TEXT);UI.Button(b,w-28-44,272,36,32,null,'secondary','copy',()=>Copy('https://'+mine.discord));}
    const y=h-68,full=c.members>=CLAN_MAX;
    if(c.me)UI.Button(b,28,y,w-56,44,'Открыть клан','primary','flag',()=>Shell.go('clan'));
    else if(S.clan)D.Text(b,28,y,w-56,44,'Заявку можно подать только без клана',15,C.DIM2);
    else if(S.applied&&S.applied.clan===c.name){D.Text(b,28,y,300,44,'Заявка на рассмотрении',15,C.ACCENT);UI.Button(b,w-28-160,y,160,44,'Отозвать','secondary',null,()=>{S.applied=null;Toast('Заявка отозвана','inf');this.clanCard(c);});}
    else if(S.applied)D.Text(b,28,y,w-56,44,Fit('Заявка уже отправлена в клан «'+S.applied.clan+'»',w-56,15),15,C.DIM2);
    else if(!c.open||full)D.Text(b,28,y,w-56,44,full?'В клане нет свободных мест':'Клан сейчас не принимает заявки',15,C.DIM2);
    else UI.Button(b,28,y,w-56,44,'Подать заявку','primary','send',()=>this.applyForm(c));
  },
  applyForm(c){
    const w=600,h=438,b=UI.Modal(this,w,h);D.Text(b,28,22,w-90,32,Fit('Заявка в клан «'+c.name+'»',w-90,22),22,C.TEXT);
    D.Cap(b,28,70,300,'О себе');const cnt=D.Text(b,w-128,70,100,18,'0/'+APPLY_MAX,13,C.DIM2,'r'),a=UI.Area(b,28,92,w-56,126,'Чем занимаетесь в игре, когда бываете в сети',APPLY_MAX);
    D.Cap(b,28,232,300,'Discord для связи');D.Text(b,w-228,232,200,18,'Необязательно',13,C.DIM2,'r');const d=UI.Input(b,28,254,w-56,46,'Имя пользователя',32,S.discord),err=D.Text(b,28,306,w-56,24,'',14,C.BAD);
    a.input.oninput=()=>{cnt.textContent=a.input.value.length+'/'+APPLY_MAX;err.textContent='';};d.input.oninput=()=>{d.input.value=d.input.value.toLowerCase();err.textContent='';};
    UI.Button(b,w-28-300,h-68,140,44,'Назад','secondary',null,()=>this.clanCard(c));
    UI.Button(b,w-28-150,h-68,150,44,'Отправить','primary','send',()=>{const t=a.input.value.trim(),ds=d.input.value.trim().replace(/^@+/,'');if(t.length<10){err.textContent='Напишите о себе хотя бы 10 символов';return;}const e=ds?discordError(ds):'';if(e){err.textContent='Discord: '+e.toLowerCase();return;}S.applied={clan:c.name,text:t,discord:ds};this.closePop();Toast('Заявка отправлена в клан «'+c.name+'»');},14);
    a.input.focus();
  },
  paint(){
    const t=this.tabs;t.innerHTML='';let x=0;
    ['Кланы','Игроки'].forEach((n,i)=>{const w=TextW(n,18)+8,on=i===this.tab,l=D.Text(t,x,0,w,72,n,18,on?C.TEXT:C.DIM);if(on)D.Line(t,x,68,w,3,C.ACCENT);D.Hit(t,x-10,0,w+20,72,()=>{this.tab=i;this.paint();},o=>{if(!on)l.style.color=o?C.TEXT:C.DIM;});x+=w+36;});
    this.r.innerHTML='';this.left=null;
    if(this.tab)this.players();else this.clans();
  },
  // Игроки: по накопленному опыту, ранг общий и от клана не зависит.
  players(){
    const r=this.r,list=playerList(),me=list.find(p=>p.me),top=Math.max(1,list[0].x);
    const clanOf=(p,f,x,y)=>{if(p.clan){UI.Emblem(f,x,y+2,20,p.clan.emblem);D.Text(f,x+27,y,110,24,'['+p.clan.tag+']',14,C.DIM);}else D.Text(f,x,y,120,24,'Без клана',14,C.DIM2);};
    [[386,0,400,168],[0,18,370,150],[802,18,370,150]].forEach(([x,y,w,h],i)=>{
      const p=list[i],m=MEDAL[i],ra=RANKS[Rank.of(p.x)],f=D.Frame(r,x,y,w,h);D.Box(f,0,0,w,h,10,C.PANEL,p.me?C.ACCENT:C.LINE);Gauge.Glow(f,1,1,w-2,h-2,10,m);f.style.cursor='pointer';f.onclick=()=>this.playerCard(p);
      D.Box(f,14,0,w-28,3,2,m);D.Text(f,16,12,44,50,String(p.place),i?36:42,m);
      const s=D.Frame(f,62,18,92,92);D.Box(s,0,0,92,92,8,C.PANEL2);Gauge.Corners(s,92,92,m,10);Gauge.Insignia(s,14,14,64,ra,m);
      D.Text(f,170,14,w-186,30,Fit(p.n,w-186,20),20,C.TEXT);D.Text(f,170,44,w-186,22,ra.name,15,m);clanOf(p,f,170,68);
      D.Text(f,w-136,68,120,24,Gauge.Short(p.x),16,C.TEXT,'r');
      Gauge.Ticks(f,16,h-26,w-32,10,Math.round((w-32)/9),p.x/top,m);
    });
    const t=UI.Card(r,0,184,800,536);
    D.Cap(t,24,18,40,'№');D.Cap(t,76,18,200,'Игрок');D.Cap(t,280,18,120,'Клан');D.Cap(t,410,18,180,'Ранг');D.Text(t,620,18,156,18,'ОПЫТ',13,C.DIM,'r');
    D.Line(t,0,46,800,1,C.LINE);
    const rows=list.slice(3,13);if(me.place>13)rows[9]=me;
    rows.forEach((p,i)=>{
      const y=50+i*47,ra=RANKS[Rank.of(p.x)];if(p.me){D.Box(t,8,y,784,44,6,C.PANEL2);D.Box(t,8,y+9,3,26,2,C.ACCENT);}
      D.Text(t,24,y,44,44,String(p.place),16,C.DIM);D.Text(t,76,y,196,44,Fit(p.n,196,16),16,p.me?C.ACCENT:C.TEXT);clanOf(p,t,280,y+10);
      Gauge.Insignia(t,410,y+8,28,ra,p.me?C.ACCENT:C.DIM);D.Text(t,446,y,150,44,ra.name,15,p.me?C.ACCENT:C.TEXT);D.Text(t,620,y,156,44,Num(p.x),16,C.TEXT,'r');
      const bg=p.me?null:D.Box(t,8,y,784,44,6,'transparent');if(bg)t.insertBefore(bg,t.children[6]);D.Hit(t,8,y,784,44,()=>this.playerCard(p),o=>{if(bg)bg.style.background=o?C.PANEL2:'transparent';});
    });
    const ra=RANKS[Rank.of(me.x)],m=D.Frame(r,816,184,356,256);D.Box(m,0,0,356,256,10,C.PANEL,C.LINE);Gauge.Glow(m,1,1,354,254,10);Gauge.Corners(m,356,256,C.ACCENT,14);
    D.Cap(m,20,18,316,'Ваше место');Gauge.Insignia(m,20,44,60,ra);D.Text(m,92,44,244,30,Fit(me.n,244,22),22,C.TEXT);D.Text(m,92,76,244,22,ra.name,15,C.ACCENT);
    D.Cap(m,20,122,150,'Место');D.Text(m,20,142,150,44,String(me.place),34,C.ACCENT);D.Cap(m,180,122,156,'Опыт');D.Text(m,180,142,156,44,Num(me.x),26,C.TEXT);
    const up=list[me.place-2];if(up){Gauge.Ticks(m,20,194,316,10,34,me.x/up.x);D.Text(m,20,212,316,24,'До '+(me.place-1)+' места: '+Num(up.x-me.x+1)+' опыта',14,C.DIM);}else D.Text(m,20,200,316,24,'Вы на первом месте',15,C.ACCENT);
    const d=UI.Card(r,816,456,356,264,'Ранги на сервере'),cnt=RANKS.map(()=>0);list.forEach(p=>cnt[Rank.of(p.x)]++);const max=Math.max(1,...cnt),base=204,cw=32;
    D.Line(d,16,base,324,1,C.LINE2);
    RANKS.forEach((a,i)=>{const x=18+i*cw,h=cnt[i]?Math.max(6,cnt[i]/max*128):0,cur=i===Rank.of(me.x);if(h)D.Box(d,x+3,base-h,cw-8,h,2,cur?C.ACCENT:C.DIM2);if(cnt[i])D.Text(d,x-4,base-h-22,cw+6,20,String(cnt[i]),13,cur?C.ACCENT:C.DIM,'c');Gauge.Insignia(d,x+3,base+8,24,a,cur?C.ACCENT:C.DIM2);});
  },
  clans(){
    const r=this.r,list=ratingList(this.mode),me=list.find(x=>x.me),top=Math.max(1,list[0].value);r.innerHTML='';
    // пьедестал: первое место в центре и выше остальных
    [[386,0,400,168],[0,18,370,150],[802,18,370,150]].forEach(([x,y,w,h],i)=>{
      const c=list[i],m=MEDAL[i],f=D.Frame(r,x,y,w,h);D.Box(f,0,0,w,h,10,C.PANEL,c.me?C.ACCENT:C.LINE);Gauge.Glow(f,1,1,w-2,h-2,10,m);f.style.cursor='pointer';f.onclick=()=>this.clanCard(c);
      D.Box(f,14,0,w-28,3,2,m);D.Text(f,16,12,44,50,String(c.place),i?36:42,m);
      const s=D.Frame(f,62,18,92,92);D.Box(s,0,0,92,92,8,C.PANEL2);Gauge.Corners(s,92,92,m,10);UI.Emblem(s,12,12,68,c.emblem);
      D.Text(f,170,16,w-186,30,Fit(c.name,w-186,20),20,C.TEXT);D.Text(f,170,46,w-186,22,'['+c.tag+'] · уровень '+c.level,14,C.DIM);
      D.Icon(f,170,78,20,'sym_bolt',C.INF);D.Text(f,198,72,w-214,32,Num(c.value),24,C.INF);
      Gauge.Ticks(f,16,h-26,w-32,10,Math.round((w-32)/9),c.value/top,m);
    });
    const t=UI.Card(r,0,184,800,536);
    D.Cap(t,24,18,40,'№');D.Cap(t,76,18,300,'Клан');D.Cap(t,400,18,70,'Уровень');D.Cap(t,480,18,70,'Состав');D.Text(t,560,18,216,18,'ВЛИЯНИЕ',13,C.DIM,'r');
    D.Line(t,0,46,800,1,C.LINE);
    const L=D.Clip(t,0,47,800,488),rest=list.slice(3);L.classList.add('scroll');D.Frame(L,0,0,1,rest.length*47+6);
    rest.forEach((c,i)=>{
      const y=3+i*47;if(c.me){D.Box(L,8,y,776,44,6,C.PANEL2);D.Box(L,8,y+9,3,26,2,C.ACCENT);}
      D.Text(L,24,y,40,44,String(c.place),16,C.DIM);UI.Emblem(L,70,y+6,32,c.emblem);
      D.Text(L,112,y,280,44,Fit(c.name+'  ['+c.tag+']',280,16),16,c.me?C.ACCENT:C.TEXT);D.Text(L,400,y,70,44,String(c.level),15,C.DIM);D.Text(L,480,y,70,44,String(c.members),15,C.DIM);
      Gauge.Ticks(L,560,y+16,120,12,16,c.value/top,C.INF);D.Text(L,686,y,90,44,Num(c.value),16,C.INF,'r');
      const bg=c.me?null:D.Box(L,8,y,776,44,6,'transparent');if(bg)L.insertBefore(bg,L.children[1]);D.Hit(L,8,y,776,44,()=>this.clanCard(c),o=>{if(bg)bg.style.background=o?C.PANEL2:'transparent';});
    });
    if(me&&me.place>3)L.scrollTop=Math.max(0,(me.place-4)*47-200);
    UI.Button(r,816,184,170,40,'За неделю',this.mode?'secondary':'primary',null,()=>{this.mode=0;this.paint();},14);UI.Button(r,1002,184,170,40,'За всё время',this.mode?'primary':'secondary',null,()=>{this.mode=1;this.paint();},14);
    const w=D.Frame(r,816,240,356,226);D.Box(w,0,0,356,226,10,C.PANEL,C.LINE);Gauge.Corners(w,356,226,C.GOLD,14);D.Cap(w,20,16,316,'Награда на склад за неделю');
    REWARDS.forEach((v,i)=>{const y=46+i*40;D.Text(w,20,y,30,30,String(i+1),20,MEDAL[i]);D.Text(w,50,y,120,30,'место',15,C.DIM);UI.MoneyR(w,336,y+3,v,18);});
    D.Line(w,20,170,316,1,C.LINE);D.Text(w,20,178,120,32,'Итоги через',14,C.DIM);this.left=D.Text(w,140,178,196,32,'',16,C.TEXT,'r');
    const m=UI.Card(r,816,482,356,238,'Ваш клан');
    if(me){
      UI.Emblem(m,20,44,52,me.emblem);D.Text(m,88,42,248,28,Fit(me.name,248,20),20,C.TEXT);D.Text(m,88,70,248,22,'Уровень '+me.level,14,C.DIM);
      D.Cap(m,20,110,150,'Место');D.Text(m,20,130,150,44,String(me.place),34,C.ACCENT);D.Cap(m,180,110,156,'Влияние');D.Text(m,180,130,156,44,Num(me.value),26,C.INF);
      const up=list[me.place-2];if(up){Gauge.Ticks(m,20,182,316,10,34,me.value/up.value,C.ACCENT);D.Text(m,20,198,316,24,'До '+(me.place-1)+' места: '+Num(up.value-me.value+1)+' влияния',14,C.DIM);}else D.Text(m,20,186,316,24,'Вы на первом месте',15,C.ACCENT);
    }else{D.MText(m,20,48,316,90,'Создайте клан, чтобы попасть в рейтинг и бороться за награды на склад.',16,C.DIM);UI.Button(m,20,170,316,48,'Создать клан','primary','plusclan',()=>Shell.go('create'));}
    this.t=1;this.tick(1);
  },
  tick(dt){this.t+=dt;if(this.t<1||!this.left)return;this.t=0;const d=new Date(),day=(8-(d.getDay()||7))*86400,s=day+86400-(d.getHours()*3600+d.getMinutes()*60+d.getSeconds()),dd=Math.floor(s/86400),h=Math.floor(s%86400/3600),mn=Math.floor(s%3600/60);this.left.textContent=dd+' д '+h+' ч '+mn+' мин';},
};

Pages.bank={
  build(r){
    const c=S.clan,days=Clan.taxDays(),low=days!==null&&days<=7,a=D.Frame(r,0,0,380,236);D.Box(a,0,0,380,236,10,C.PANEL,C.LINE);Gauge.Glow(a,1,1,378,234,10,'var(--gold)');Gauge.Corners(a,380,236,C.GOLD,16);
    D.Cap(a,28,24,300,'На складе');UI.Money(a,28,54,c.bank,40);
    D.Cap(a,28,152,140,'Заполнение');D.Text(a,150,150,202,20,Num(c.bank)+'/'+Num(BANK_MAX),15,C.DIM,'r');Gauge.Ticks(a,28,180,324,18,30,c.bank/BANK_MAX,C.GOLD);
    const x=D.Frame(r,396,0,380,236);D.Box(x,0,0,380,236,10,C.PANEL,low?C.BAD:C.LINE);D.Cap(x,24,24,160,'Налог клана');D.Text(x,170,22,186,20,Num(CLAN_TAX)+' в день',15,C.GOLD,'r');
    D.Text(x,24,46,332,44,days===null?'Без налога ещё '+c.taxFree+' дн.':'Хватит на '+days+' дн.',28,low?C.BAD:C.TEXT);Gauge.Ticks(x,24,98,332,14,30,days===null?1:days/30,low?C.BAD:C.GOLD);
    D.Text(x,24,118,200,28,'Списание через',14,C.DIM);this.taxT=D.Text(x,216,118,140,28,'',16,C.TEXT,'r');D.Line(x,24,152,332,1,C.LINE);
    D.MText(x,24,160,332,66,'Налог списывается со склада каждый день. Когда на складе будет 0, клан распустится и всё удалится.',14,low?C.BAD:C.DIM2);
    const w=UI.Card(r,792,0,380,236,'Движение за неделю'),max=Math.max(...c.week.map(Math.abs),1),axis=146,dn=['пн','вт','ср','чт','пт','сб','вс'],net=c.week.reduce((p,q)=>p+q,0),today=(new Date().getDay()+6)%7;
    D.Text(w,200,14,160,22,(net>=0?'+':'−')+Num(Math.abs(net)),18,net>=0?C.GOLD:C.BAD,'r');
    D.Line(w,20,axis,340,1,C.LINE2);
    c.week.forEach((v,i)=>{const px=24+i*48;Gauge.Column(w,px,axis,36,Math.abs(v)/max,v<0);D.Text(w,px,204,36,18,dn[i],13,i===today?C.ACCENT:C.DIM2,'c');});
    // Пополнение и снятие только у банкомата клана, здесь просмотр. Банкомат один, ставится и переносится бесплатно.
    const ab=D.Frame(r,0,252,1172,60);D.Box(ab,0,0,1172,60,10,C.PANEL,c.atm?C.LINE:C.GOLD);D.Box(ab,16,12,36,36,8,C.PANEL2);D.Icon(ab,25,21,18,'vault',c.atm?C.GOLD:C.DIM);D.Cap(ab,68,12,300,'Банкомат клана');D.Text(ab,68,28,560,24,c.atm?'X '+c.atm.x+'   Y '+c.atm.z+' · '+Pages.map.dist(c.atm.x,c.atm.z)+' от вас':'Не установлен: без него нельзя пополнить склад и снять деньги',14,c.atm?C.TEXT:C.GOLD);
    let ax=1172-16;if(Clan.can('atm')){const w0=c.atm?140:214;ax-=w0;UI.Button(ab,ax,12,w0,36,c.atm?'Перенести':'Установить банкомат',c.atm?'secondary':'primary','vault',()=>{const mv=Clan.placeAtm();Toast(mv?'В игре планшет закроется и появится голограмма: банкомат перенесён':'В игре планшет закроется и появится голограмма: банкомат установлен');Shell.refresh();},13);ax-=10;}
    if(c.atm){ax-=120;UI.Button(ab,ax,12,120,36,'На карте','secondary','map',()=>{Pages.map.view={x:c.atm.x,z:c.atm.z,s:.2};Shell.go('map');},13);}
    const h=UI.Card(r,0,324,1172,396);D.Cap(h,70,18,240,'Кто');D.Cap(h,340,18,300,'Операция');D.Cap(h,740,18,200,'Сумма');D.Cap(h,980,18,170,'Когда');D.Line(h,0,46,1172,1,C.LINE);
    if(!c.history.length)D.Text(h,0,190,1172,30,'Операций пока нет',17,C.DIM2,'c');
    const L=D.Clip(h,0,47,1172,348);L.classList.add('scroll');
    c.history.forEach(([u,o,s,t],i)=>{
      const y=4+i*68,col=s>0?C.GOLD:C.BAD;D.Box(L,24,y+14,32,32,8,C.PANEL2);D.Icon(L,31,y+21,18,s>0?'plus':'minus',col);
      D.Text(L,70,y,250,60,Fit(u,250,16),16,C.TEXT);D.Text(L,340,y,380,60,o,15,C.DIM);D.Text(L,740,y,220,60,(s>0?'+':'−')+Num(Math.abs(s)),17,col);D.Text(L,980,y,170,60,t,14,C.DIM2);if(i<c.history.length-1)D.Line(L,24,y+64,1124,1,C.LINE);
    });
    D.Frame(L,0,c.history.length*68+4,1,8);
    this.t=1;this.tick(1);
  },
  tick(dt){this.t+=dt;if(this.t<1)return;this.t=0;const d=new Date(),n=86400-(d.getHours()*3600+d.getMinutes()*60+d.getSeconds()),p=v=>String(v).padStart(2,'0');if(this.taxT)this.taxT.textContent=p(Math.floor(n/3600))+':'+p(Math.floor(n%3600/60))+':'+p(n%60);},
};

// Ранги клана: глава создаёт ранги и задаёт доступ. Порядок в списке это старшинство.
Pages.cranks={
  sel:'leader',
  build(r){this.r=r;if(!S.clan.ranks.find(x=>x.id===this.sel))this.sel=S.clan.ranks[0].id;this.paint();},
  paint(){
    const r=this.r,c=S.clan,ed=Clan.can('ranks'),rs=c.ranks,n=rs.length;r.innerHTML='';
    const L=UI.Card(r,0,0,400,720);D.Cap(L,20,18,240,'Ранги · '+n+'/'+MAX_RANKS);
    rs.forEach((x,i)=>{
      const y=48+i*70,on=x.id===this.sel,cnt=c.members.filter(m=>Clan.rank(m)===x).length,free=Clan.edits(x)&&i>0&&i<n-1,f=D.Frame(L,12,y,376,62),bg=D.Box(f,0,0,376,62,8,on?C.PANEL3:C.PANEL2,on?C.ACCENT:null);
      D.Box(f,0,12,4,38,2,x.color);D.Text(f,18,8,250,26,Fit(x.name,250,18),18,C.TEXT);D.Text(f,18,34,250,20,cnt+' чел. · прав '+PERMS.filter(p=>x.perms[p[0]]).length+'/'+PERMS.length,13,C.DIM);
      if(free)D.Icon(f,340,20,22,'grip',C.DIM);else if(ed)D.Icon(f,344,23,16,'lock',C.DIM2);
      const h=D.Hit(f,0,0,376,62,()=>{if(this.dragged){this.dragged=false;return;}this.sel=x.id;this.paint();},o=>{if(!on&&!this.drag)bg.style.background=o?C.PANEL3:C.PANEL2;});
      if(free){h.style.cursor='grab';h.onmousedown=e=>this.dragStart(e,f,i,L);}
    });
    if(ed)D.MText(L,20,n*70+56,360,48,'Перетащите ранг мышкой: чем выше, тем старше. Лидер всегда сверху, Участник всегда снизу.',14,C.DIM2);
    if(ed)UI.Button(L,20,656,360,46,'Новый ранг','secondary','plus',()=>this.add(),14).classList.toggle('off',n>=MAX_RANKS||Clan.topRank()>n-1);
    const k=rs.find(x=>x.id===this.sel),lead=k.id==='leader',base=k.id==='member',edk=Clan.edits(k),R=D.Frame(r,416,0,756,720);D.Box(R,0,0,756,720,10,C.PANEL,C.LINE);Gauge.Glow(R,1,1,754,718,10);
    D.Cap(R,24,20,300,'Название');
    if(edk){const i=UI.Input(R,24,44,400,48,'Название ранга',20,k.name);i.input.onchange=()=>{k.name=i.input.value.trim()||k.name;Shell.refresh();};}else D.Text(R,24,44,400,48,k.name,28,C.TEXT);
    D.Cap(R,452,20,280,'Цвет');COLORS.forEach((x,i)=>{const cx=452+(i%6)*46,cy=44+Math.floor(i/6)*42;if(k.color===x)D.Box(R,cx-3,cy-3,38,38,19,null,C.TEXT);D.Circle(R,cx+1,cy+1,30,x);if(edk)D.Hit(R,cx-2,cy-2,36,36,()=>{k.color=x;this.paint();});});
    D.Line(R,24,142,708,1,C.LINE);D.Cap(R,24,158,400,'Доступ');
    PERMS.forEach(([id,t,sub],i)=>{
      const x=24+(i%2)*362,y=180+Math.floor(i/2)*57,on=!!k.perms[id];D.Box(R,x,y,346,51,8,C.PANEL2);D.Text(R,x+14,y+5,262,22,t,15,on?C.TEXT:C.DIM);D.Text(R,x+14,y+27,262,18,sub,12,C.DIM2);
      if(lead)D.Icon(R,x+310,y+16,18,'lock',C.DIM2);else{const sw=UI.Switch(R,x+288,y+13,on,v=>{k.perms[id]=v;this.paint();});if(!edk||!Clan.can(id)){sw.style.pointerEvents='none';if(edk)sw.style.opacity=.4;}}
    });
    D.Line(R,24,472,708,1,C.LINE);const ms=c.members.filter(m=>Clan.rank(m)===k);D.Cap(R,24,488,400,'С этим рангом · '+ms.length);
    let x=24,y=514;if(!ms.length)D.Text(R,24,y,400,26,'Пока никого',15,C.DIM2);ms.forEach(m=>{const w=TextW(m.name,14)+22;if(x+w>732){x=24;y+=34;}Gauge.Pill(R,x,y,m.name,k.color);x+=w+8;});
    D.MText(R,24,596,700,44,lead?'У Лидера всегда все права, их нельзя снять. Лидер в клане один.':base?'Этот ранг получают новые участники. Удалить его нельзя.':'Ранги выше в списке старше: участник управляет только теми, кто ниже него.',14,C.DIM);
    if(edk&&!lead&&!base)UI.Button(R,24,656,200,46,'Удалить ранг','danger','trash',()=>UI.Ask({title:'Удалить ранг?',text:'Участники с рангом «'+k.name+'» получат ранг «'+rs[n-1].name+'».',yes:'Удалить',danger:true,fn:()=>{c.members.forEach(m=>{if(m.rank===k.id)m.rank='member';});c.ranks=rs.filter(q=>q!==k);this.sel=rs[0].id;this.paint();}}),14);
    if(!edk)D.Text(R,24,664,700,30,ed?'Менять можно только ранги младше вашего.':'Менять ранги может тот, чей ранг это разрешает.',14,C.DIM2);else if(!Clan.isLeader())D.Text(R,24+(lead||base?0:216),664,480,30,'Выдавать можно только права, которые есть у вас.',14,C.DIM2);
  },
  // Перетаскивание: ряд идёт за мышью, линия показывает новое место, порядок меняется при отпускании.
  dragStart(e,f,from,L){
    e.preventDefault();const n=S.clan.ranks.length,lo=Clan.topRank(),k=f.getBoundingClientRect().height/62,y0=e.clientY,top0=48+from*70;let to=from,line=null;
    const mv=v=>{
      const d=(v.clientY-y0)/k;if(!this.drag&&Math.abs(d)<4)return;
      if(!this.drag){this.drag=true;f.style.zIndex=10;f.style.boxShadow='0 10px 26px rgba(0,0,0,.6)';f.firstChild.nextSibling;line=D.Box(L,12,0,376,3,2,C.ACCENT);}
      const top=Math.max(48+lo*70,Math.min(48+(n-2)*70,top0+d));f.style.top=top+'px';to=Math.max(lo,Math.min(n-2,Math.round((top-48)/70)));line.style.top=48+to*70+(to>from?66:-5)+'px';
    };
    const up=()=>{removeEventListener('mousemove',mv);removeEventListener('mouseup',up);if(!this.drag)return;this.drag=false;this.dragged=true;this.move(from,to);setTimeout(()=>{this.dragged=false;},0);};
    addEventListener('mousemove',mv);addEventListener('mouseup',up);
  },
  move(from,to){const r=S.clan.ranks,x=r[from],lo=Clan.topRank();if(from<lo||to<lo||from>r.length-2||to>r.length-2)return;r.splice(from,1);r.splice(to,0,x);this.paint();},
  add(){const c=S.clan,k={id:'r'+Date.now(),name:'Новый ранг',color:COLORS[5],perms:{}};c.ranks.splice(c.ranks.length-1,0,k);this.sel=k.id;this.paint();},
};

// Настройки клана: правки копятся черновиком, справа предпросмотр и счёт. Платит склад денег.
Pages.csettings={
  e:null,
  build(r){
    const c=S.clan;this.r=r;
    if(this.owner!==c){this.owner=c;this.e={...c.emblem};this.name=c.name;this.tag=c.tag;this.desc=c.desc;this.open=c.open;this.disc=c.discord;}
    const L=UI.Card(r,0,0,724,720);
    this.price(L,28,24,408,'Название',CLAN_PRICES.name);this.iN=UI.Input(L,28,48,408,52,'От 3 до 24 символов',24,this.name);
    this.price(L,460,24,236,'Тег',CLAN_PRICES.tag);this.iT=UI.Input(L,460,48,236,52,'2–4 символа',4,this.tag);
    this.price(L,28,122,484,'Слоган',0);this.iD=UI.Input(L,28,146,484,52,'Девиз клана, одной фразой',60,this.desc);D.Cap(L,536,122,160,'Набор в клан');const ot=D.Text(L,536,146,100,52,this.open?'Открыт':'Закрыт',16,this.open?C.OK:C.DIM);UI.Switch(L,640,160,this.open,o=>{this.open=o;ot.textContent=o?'Открыт':'Закрыт';ot.style.color=o?C.OK:C.DIM;this.paintRight();});
    D.Line(L,28,214,668,1,C.LINE);
    this.price(L,28,226,668,'Эмблема: форма и символ',CLAN_PRICES.emblem);Pages.create.pick.call(this,L,28,250,SHAPES,'shape',50,6,'shp_');Pages.create.pick.call(this,L,28,310,SYMBOLS,'symbol',50,6,'sym_');
    this.price(L,28,488,668,'Цвет клана',CLAN_PRICES.color);Shell.accent(this.e.color);this.colors=D.Frame(L,28,510,668,48);this.paintColors();
    D.Line(L,28,568,668,1,C.LINE);
    this.iN.input.oninput=()=>{this.name=this.iN.input.value;this.paintRight();};
    this.iT.input.oninput=()=>{this.iT.input.value=this.iT.input.value.toUpperCase().replace(/[^A-Z0-9]/g,'');this.tag=this.iT.input.value;this.paintRight();};
    this.iD.input.oninput=()=>{this.desc=this.iD.input.value;this.paintRight();};
    this.price(L,28,584,320,'Discord-сервер',0);this.iC=UI.Input(L,28,608,320,44,'discord.gg/код',60,this.disc);this.iC.input.oninput=()=>{this.disc=this.iC.input.value;this.paintRight();};
    if(Clan.isLeader()){
      const others=c.members.filter(m=>!m.me);D.Cap(L,368,584,328,'Передать лидерство');
      if(others.length){const dd=UI.Dropdown(L,368,608,190,others.map(m=>m.name),0,44,null,true);UI.Button(L,566,608,130,44,'Передать','secondary','rank',()=>{const t=others[dd.sel];UI.Ask({title:'Передать лидерство?',text:t.name+' станет лидером, вы получите ранг «'+c.ranks[1].name+'». Отменить это сможет только новый лидер.',yes:'Передать',danger:true,fn:()=>{t.rank='leader';Clan.me().rank=c.ranks[1].id;c.journal.unshift(['rank','Лидер клана: '+t.name,'сейчас']);Toast('Лидерство передано');Shell.go('clan');Panel.sync();}});},14);}
      else D.Text(L,368,608,328,44,'В клане больше никого нет',15,C.DIM2);
    }
    this.R=D.Frame(r,748,0,424,720);this.paintRight();
  },
  price(p,x,y,w,cap,v){D.Cap(p,x,y,w-120,cap);if(v)UI.Money(p,x+w-TextW(Num(v),14)-22,y-1,v,14);else D.Text(p,x+w-120,y,120,18,'Бесплатно',13,C.DIM2,'r');},
  paintColors(){this.colors.innerHTML='';COLORS.forEach((c,i)=>{const x=i*56;if(this.e.color===c)D.Box(this.colors,x+2,1,46,46,23,null,C.TEXT);D.Circle(this.colors,x+7,6,36,c);D.Hit(this.colors,x+5,4,40,40,()=>{this.e.color=c;Shell.accent(c);this.paintColors();this.paintRight();});});},
  changes(){
    const c=S.clan,e=this.e,l=[];
    if(this.name.trim()!==c.name)l.push(['Название',CLAN_PRICES.name]);
    if(this.tag!==c.tag)l.push(['Тег',CLAN_PRICES.tag]);
    if(e.shape!==c.emblem.shape||e.symbol!==c.emblem.symbol)l.push(['Эмблема',CLAN_PRICES.emblem]);
    if(e.color!==c.emblem.color)l.push(['Цвет клана',CLAN_PRICES.color]);
    if(this.desc!==c.desc)l.push(['Слоган',0]);
    if(this.open!==c.open)l.push(['Набор в клан',0]);
    if((clanInvite(this.disc)||this.disc.trim())!==c.discord)l.push(['Discord-сервер',0]);
    return l;
  },
  problem(){
    const c=S.clan,n=this.name.trim(),own=emblemOwner(this.e),all=[...TAKEN,...RIVALS.map(r=>r.name)],tags=[...TAKEN_TAGS,...RIVALS.map(r=>r.tag)];
    if(this.disc.trim()&&!clanInvite(this.disc))return 'Нужна ссылка вида discord.gg/код';
    if(n!==c.name&&Clan.banned(n)||this.tag!==c.tag&&Clan.banned(this.tag))return 'Запрещённое слово';
    if(n!==c.name&&(n.length<3||all.includes(n)))return n.length<3?'Название: минимум 3 символа':'Название уже занято';
    if(this.tag!==c.tag&&(this.tag.length<2||tags.includes(this.tag)))return this.tag.length<2?'Тег: минимум 2 символа':'Тег уже занят';
    if(own)return 'Эмблема занята: '+own.name;
    const sum=this.changes().reduce((a,x)=>a+x[1],0);if(sum>c.bank)return 'На складе не хватает '+Num(sum-c.bank);
    return '';
  },
  paintRight(){
    const r=this.R,c=S.clan,l=this.changes(),sum=l.reduce((a,x)=>a+x[1],0),err=this.problem();r.innerHTML='';
    const v=UI.Card(r,0,0,424,250);Gauge.Glow(v,1,1,422,248,10);Gauge.Corners(v,424,250,C.ACCENT,16);UI.Emblem(v,152,22,120,this.e);
    D.Text(v,16,150,392,38,Fit(this.name.trim()||'Название клана',392,26),26,C.TEXT,'c');D.Text(v,16,190,392,24,'['+this.tag+']',17,C.ACCENT,'c');
    const p=UI.Card(r,0,266,424,454);D.Cap(p,24,20,376,'Изменения');
    if(!l.length)D.Text(p,24,50,376,30,'Пока ничего не менялось',16,C.DIM2);
    l.forEach(([t,m],i)=>{const y=50+i*34;D.Icon(p,24,y+6,16,'edit',C.ACCENT);D.Text(p,50,y,190,30,t,16,C.TEXT);if(m)UI.MoneyR(p,400,y+3,m,18);else D.Text(p,250,y,150,30,'Бесплатно',14,C.DIM2,'r');});
    D.Line(p,24,226,376,1,C.LINE);
    D.Text(p,24,238,200,28,'Итого',16,C.DIM);UI.MoneyR(p,400,238,sum,20);
    D.Text(p,24,272,200,28,'На складе',16,C.DIM);UI.MoneyR(p,400,272,c.bank,20,C.TEXT);
    D.Text(p,24,306,200,28,'Останется',16,C.DIM);UI.MoneyR(p,400,306,Math.max(0,c.bank-sum),20,C.TEXT);
    D.Text(p,24,340,376,24,err,14,C.BAD);
    const a=UI.Button(p,24,376,250,54,'Применить','primary','check',()=>this.apply(),17);a.classList.toggle('off',!l.length||!!err);
    const b=UI.Button(p,284,376,116,54,'Сбросить','secondary',null,()=>{this.owner=null;Shell.refresh();},14);b.classList.toggle('off',!l.length);
  },
  apply(){
    const c=S.clan,l=this.changes(),sum=l.reduce((a,x)=>a+x[1],0);if(!l.length||this.problem())return;
    c.name=this.name.trim();c.tag=this.tag;c.desc=this.desc;c.open=this.open;c.discord=clanInvite(this.disc)||'';c.emblem={...this.e};c.bank-=sum;
    if(sum)c.history.unshift([S.name,'Настройки клана',-sum,Pages.csettings.now()]);
    c.journal.unshift(['sym_cog','Изменены настройки: '+l.map(x=>x[0].toLowerCase()).join(', '),'сейчас']);
    Toast(sum?'Настройки применены, списано '+Num(sum):'Настройки применены');Shell.refresh();
  },
  now(){const d=new Date(),p=v=>String(v).padStart(2,'0');return p(d.getDate())+'.'+p(d.getMonth()+1)+' '+p(d.getHours())+':'+p(d.getMinutes());},
};

// Администратор: все кланы сервера и запрещённые слова. Виден только тем, кто указан в конфиге сервера.
Pages.admin={
  build(r){this.r=r;this.sel=null;this.paint();},
  leave(){this.closePop();},
  closePop(){if(this.pop){this.pop.remove();this.pop=null;}},
  esc(){this.closePop();},
  rows(){
    const l=RIVALS.map(x=>({ref:x,members:x.members,bank:x.bank,leader:x.leader,days:Math.floor(x.bank/CLAN_TAX)}));
    if(S.clan)l.push({ref:S.clan,own:true,members:S.clan.members.length,bank:S.clan.bank,leader:S.clan.members.find(m=>m.rank==='leader').name,days:Clan.taxDays()});
    return l;
  },
  paint(){
    const r=this.r,rows=this.rows();r.innerHTML='';if(!rows.some(x=>x.ref===this.sel))this.sel=rows.length?rows[0].ref:null;
    const t=UI.Card(r,0,0,724,720);D.Cap(t,58,18,200,'Клан · '+rows.length+'/'+MAX_CLANS);D.Cap(t,290,18,110,'Лидер');D.Cap(t,410,18,70,'Состав');D.Cap(t,486,18,110,'Склад');D.Cap(t,606,18,100,'Налог');D.Line(t,0,46,724,1,C.LINE);
    const L=D.Clip(t,0,47,724,672);L.classList.add('scroll');D.Frame(L,0,0,1,rows.length*50+6);
    if(!rows.length)D.Text(L,0,300,724,30,'На сервере нет кланов',17,C.DIM2,'c');
    rows.forEach((x,i)=>{
      const y=3+i*50,c=x.ref,on=c===this.sel,bg=D.Box(L,8,y,700,46,6,on?C.PANEL3:'transparent',on?C.ACCENT:null),low=x.days!==null&&x.days<=7;UI.Emblem(L,20,y+8,30,c.emblem);
      D.Text(L,58,y,226,46,Fit(c.name+' ['+c.tag+']',226,15),15,x.own?C.ACCENT:C.TEXT);D.Text(L,290,y,112,46,Fit(x.leader,112,14),14,C.DIM);D.Text(L,410,y,70,46,x.members+'/'+CLAN_MAX,14,C.DIM);D.Text(L,486,y,112,46,Num(x.bank),14,C.GOLD);D.Text(L,606,y,96,46,x.days===null?'льгота':x.days+' дн.',14,low?C.BAD:C.DIM);
      D.Hit(L,8,y,700,46,()=>{this.sel=c;this.paint();},o=>{if(!on)bg.style.background=o?C.PANEL2:'transparent';});
    });
    const p=D.Frame(r,748,0,424,356);D.Box(p,0,0,424,356,10,C.PANEL,C.LINE);Gauge.Glow(p,1,1,422,354,10);
    if(this.sel){
      const c=this.sel,x=rows.find(q=>q.ref===c);UI.Emblem(p,24,24,72,c.emblem);D.Text(p,112,24,288,34,Fit(c.name,288,22),22,C.TEXT);D.Text(p,112,60,288,24,'['+c.tag+'] · уровень '+c.level,14,C.DIM);
      [['Лидер',x.leader],['Участники',x.members+'/'+CLAN_MAX],['Склад',Num(x.bank)],['Налог',x.days===null?'льгота':'на '+x.days+' дн.']].forEach(([a,v],i)=>{const cx=24+(i%2)*200,cy=118+(i>>1)*64;D.Cap(p,cx,cy,180,a);D.Text(p,cx,cy+20,184,30,Fit(v,184,18),18,C.TEXT);});
      UI.Button(p,24,272,120,48,'Переименовать','secondary',null,()=>this.rename(x),13);UI.Button(p,152,272,120,48,'Сменить лидера','secondary',null,()=>this.leader(x),13);
      UI.Button(p,280,272,120,48,'Распустить','danger',null,()=>UI.Confirm({title:'Распустить клан?',text:'Клан «'+c.name+'» будет удалён целиком: склад, банкомат, ранги, новости, журнал, метки. Это нельзя отменить.',word:c.name,yes:'Распустить',fn:()=>{if(x.own)Clan.dissolve();else RIVALS.splice(RIVALS.indexOf(c),1);this.sel=null;Toast('Клан распущен','inf');Shell.navPaint();this.paint();}}),13);
    }
    const f=UI.Card(r,748,372,424,348,'Запрещённые слова'),i=UI.Input(f,24,46,264,44,'Слово',24);UI.Button(f,298,46,102,44,'Добавить','secondary',null,()=>{const v=i.input.value.trim().toLowerCase();if(v&&!S.banned.includes(v))S.banned.push(v);this.paint();},14);
    let cx=24,cy=106;S.banned.forEach(w=>{const tw=TextW(w,14)+42;if(cx+tw>400){cx=24;cy+=42;}D.Box(f,cx,cy,tw,34,17,C.PANEL2,C.LINE2);D.Text(f,cx+14,cy,tw-36,34,w,14,C.TEXT);const ic=D.Icon(f,cx+tw-26,cy+10,14,'close',C.DIM);D.Hit(f,cx,cy,tw,34,()=>{S.banned=S.banned.filter(q=>q!==w);this.paint();},o=>ic.style.color=o?C.BAD:C.DIM);cx+=tw+8;});
    D.MText(f,24,276,376,54,'Проверяются в названиях и тегах при создании и смене. Уже созданные кланы не меняются.',14,C.DIM2);
  },
  // Смена лидера: для клана, чей лидер пропал. Старый лидер получает ранг Заместитель.
  leader(x){
    const c=x.ref,names=x.own?c.members.filter(m=>m.rank!=='leader').map(m=>m.name):Array.from({length:Math.min(5,x.members-1)},(_,i)=>c.tag+' игрок '+(i+1)),w=460,h=236,b=UI.Modal(this,w,h);D.Text(b,28,22,w-90,32,'Сменить лидера',22,C.TEXT);
    if(!names.length){D.Text(b,28,76,w-56,32,'В клане больше никого нет',15,C.DIM2);return;}
    D.Text(b,28,60,w-56,24,'Сейчас: '+Fit(x.leader,300,14),14,C.DIM);const dd=UI.Dropdown(b,28,92,w-56,names,0,44);
    UI.Button(b,w-28-300,h-68,140,44,'Отмена','secondary',null,()=>this.closePop());
    UI.Button(b,w-28-150,h-68,150,44,'Назначить','primary',null,()=>{const n=names[dd.sel];if(x.own){const t=c.members.find(m=>m.name===n),o=c.members.find(m=>m.rank==='leader');t.rank='leader';if(o)o.rank=c.ranks[1].id;c.journal.unshift(['rank','Администратор назначил лидера: '+n,'сейчас']);}else c.leader=n;this.closePop();Toast('Лидер клана изменён');this.paint();},14);
  },
  rename(x){
    const c=x.ref,w=520,h=304,b=UI.Modal(this,w,h);D.Text(b,28,22,w-90,32,'Переименовать клан',22,C.TEXT);
    D.Cap(b,28,72,300,'Название');const n=UI.Input(b,28,94,316,48,'От 3 до 24 символов',24,c.name);D.Cap(b,360,72,130,'Тег');const t=UI.Input(b,360,94,132,48,'2–4',4,c.tag),err=D.Text(b,28,152,w-56,24,'',14,C.BAD);
    t.input.oninput=()=>{t.input.value=t.input.value.toUpperCase().replace(/[^A-Z0-9]/g,'');};
    UI.Button(b,w-28-300,h-68,140,44,'Отмена','secondary',null,()=>this.closePop());
    UI.Button(b,w-28-150,h-68,150,44,'Сохранить','primary',null,()=>{
      const nn=n.input.value.trim(),tt=t.input.value,other=this.rows().filter(q=>q.ref!==c).map(q=>q.ref);
      if(nn.length<3){err.textContent='Название: минимум 3 символа';return;}if(tt.length<2){err.textContent='Тег: минимум 2 символа';return;}
      if(other.some(q=>q.name===nn)){err.textContent='Такое название уже есть';return;}if(other.some(q=>q.tag===tt)){err.textContent='Такой тег уже есть';return;}
      c.name=nn;c.tag=tt;if(x.own)c.journal.unshift(['sym_cog','Администратор изменил название клана','сейчас']);this.closePop();Toast('Клан переименован');Shell.navPaint();this.paint();
    },14);
  },
};

// Банкомат клана: мини-окно «положить» или «снять». В игре открывается действием у банкомата, все проверки делает сервер.
const Atm={
  pop:null,mode:0,amt:'',
  closePop(){if(this.pop){this.pop.remove();this.pop=null;}},
  open(){
    this.mode=0;this.amt='';
    if(Clan.can('bank')){this.paint();return;}
    const w=420,h=196,b=UI.Modal(this,w,h);D.Text(b,28,22,w-90,32,'Банкомат клана',22,C.TEXT);D.MText(b,28,66,w-56,44,'Ваш ранг не даёт права работать со складом.',15,C.DIM);UI.Button(b,28,h-68,w-56,44,'Понятно','secondary',null,()=>this.closePop());
  },
  max(){const c=S.clan;return this.mode?c.bank:Math.min(S.money,Math.max(0,BANK_MAX-c.bank));},
  paint(){
    const c=S.clan,w=440,h=440,b=UI.Modal(this,w,h),m=this.mode,name=m?'Снять':'Положить';
    UI.Emblem(b,28,22,32,c.emblem);D.Text(b,70,20,300,36,Fit(c.name,300,20),20,C.TEXT);
    D.Cap(b,28,74,180,'На складе');UI.Money(b,28,96,c.bank,22);D.Cap(b,232,74,180,'При себе');UI.Money(b,232,96,S.money,22,C.TEXT);
    ['Положить','Снять'].forEach((t,i)=>{const x=28+i*196,on=i===m;D.Box(b,x,146,188,38,8,on?C.ACCENT:C.PANEL2,on?null:C.LINE2);D.Text(b,x,146,188,38,t,15,on?C.ON:C.TEXT,'c');D.Hit(b,x,146,188,38,()=>{this.mode=i;this.amt='';this.paint();});});
    const i=UI.Input(b,28,196,w-56,52,'Сумма',9,this.amt),note=D.Text(b,28,296,w-56,22,'',14,C.DIM),err=this.errT=D.Text(b,28,320,w-56,40,'',14,C.BAD),ok=UI.Button(b,28,h-68,w-56,48,name,'primary',null,()=>this.submit(),16);
    const sync=()=>{
      const v=+this.amt||0,after=m?c.bank-v:c.bank+v,days=Math.floor(Math.max(0,after)/CLAN_TAX);i.input.value=this.amt;err.textContent='';ok.label.textContent=v?name+' '+Num(v):name;ok.classList.toggle('off',!v);
      note.textContent=v?'На складе станет '+Num(Math.max(0,after))+(c.taxFree>0?'':' · налог на '+days+' дн.'):'';note.style.color=c.taxFree<=0&&v&&days<=7?C.BAD:C.DIM;
    };
    i.input.oninput=()=>{this.amt=i.input.value.replace(/\D/g,'').replace(/^0+/,'');sync();};i.input.onkeydown=e=>{if(e.key==='Enter')this.submit();};
    [[10000,'10 000'],[100000,'100 000'],[500000,'500 000']].forEach(([v,t],k)=>UI.Button(b,28+k*98,256,92,32,t,'secondary',null,()=>{this.amt=String((+this.amt||0)+v);sync();},13));
    UI.Button(b,322,256,90,32,'Всё','secondary',null,()=>{this.amt=String(this.max()||'');sync();},13);
    sync();i.input.focus();
  },
  // Операция целиком или никак: на землю ничего не падает, при нехватке места всё остаётся как было.
  submit(){
    const c=S.clan,v=+this.amt||0,m=this.mode,bad=t=>{this.errT.textContent=t;};if(!v)return;
    if(m){
      if(v>c.bank)return bad('Недостаточно средств на складе (на складе '+Num(c.bank)+')');
      if(S.invFull)return this.noRoom();
    }else{
      if(v>S.money)return bad('Недостаточно наличных (при себе '+Num(S.money)+')');
      if(c.bank+v>BANK_MAX)return bad('Склад заполнен: можно положить ещё '+Num(Math.max(0,BANK_MAX-c.bank)));
      if(S.invFull&&v%1000)return this.noRoom();
    }
    const sum=m?-v:v;c.bank+=sum;S.money-=sum;c.history.unshift([S.name,m?'Снятие':'Пополнение',sum,Pages.csettings.now()]);c.week[(new Date().getDay()+6)%7]+=sum;c.journal.unshift(['coin',S.name+(m?' снял со склада':' пополнил склад'),'сейчас',S.name,(m?'−':'+')+Num(v)]);
    Toast((m?'Снято ':'Внесено ')+Num(v));this.amt='';Shell.refresh();this.paint();
  },
  noRoom(){
    const w=420,h=236,b=UI.Modal(this,w,h);D.Text(b,28,22,w-90,32,'Инвентарь заполнен',22,C.TEXT);D.MText(b,28,66,w-56,88,'Купюры не помещаются в инвентарь, операция отменена. Освободите место и попробуйте снова.',15,C.DIM);UI.Button(b,28,h-68,w-56,44,'Понятно','primary',null,()=>this.paint());
  },
};

// Новости клана: читают все участники, пишут те, чей ранг это разрешает. Закреплённая новость одна и всегда сверху.
Pages.cnews={
  top:0,
  build(r){this.r=r;S.clan.newsUnread=0;Shell.navPaint();this.f=this.f||this.blank();this.paint();},
  blank(){return {id:0,tag:0,title:'',text:'',pin:false};},
  paint(){
    const r=this.r,c=S.clan,ed=Clan.can('news');r.innerHTML='';
    const pinned=c.news.find(n=>n.pin),rest=c.news.filter(n=>!n.pin),W=ed?760:1172;
    let y=0;
    if(pinned){this.hero(r,pinned,W,ed);y=206;}
    const rows=Math.floor((720-y+8)/104);this.top=Math.max(0,Math.min(this.top,Math.max(0,rest.length-rows)));
    const list=D.Frame(r,0,y,W,720-y);list.onwheel=e=>{e.preventDefault();this.top+=e.deltaY>0?1:-1;this.paint();};
    if(!c.news.length){const f=UI.Card(r,0,0,W,720);D.Icon(f,W/2-30,250,60,'news',C.DIM2);D.Text(f,0,326,W,34,'Новостей пока нет',24,C.TEXT,'c');D.Text(f,0,366,W,26,ed?'Напишите первую справа':'Здесь появятся новости клана',16,C.DIM,'c');}
    rest.slice(this.top,this.top+rows).forEach((n,i)=>this.row(list,n,i*104,W,ed));
    if(ed)this.form(r);
  },
  tag(p,x,y,n){return Gauge.Pill(p,x,y,NEWS_TAGS[n.tag].name.toUpperCase(),NEWS_TAGS[n.tag].color);},
  actions(p,w,y,n){
    [['pin',n.pin?'Открепить':'Закрепить',()=>this.pin(n)],['edit','Править',()=>{this.f={...n};this.paint();}],['trash','Удалить',()=>this.del(n)]].forEach(([ic,,fn],i)=>{
      const x=w-116+i*34,e=D.Icon(p,x+5,y+5,20,ic,ic==='pin'&&n.pin?C.ACCENT:C.DIM);D.Hit(p,x,y,30,30,fn,o=>{if(!(ic==='pin'&&n.pin))e.style.color=o?(ic==='trash'?C.BAD:C.TEXT):C.DIM;});
    });
  },
  hero(r,n,W,ed){
    const f=D.Frame(r,0,0,W,190);D.Box(f,0,0,W,190,10,C.PANEL,C.LINE);Gauge.Glow(f,1,1,W-2,188,10);Gauge.Corners(f,W,190,C.ACCENT,16);
    D.Icon(f,24,23,18,'pin',C.ACCENT);D.Cap(f,50,24,160,'Закреплено',C.ACCENT);this.tag(f,170,20,n);
    if(ed)this.actions(f,W-10,16,n);
    D.Text(f,24,54,W-48,42,Fit(n.title,W-48,30),30,C.TEXT);D.MText(f,24,100,W-48,50,n.text,16,C.DIM);D.Text(f,24,156,W-48,22,n.author+' · '+n.time,13,C.DIM2);
  },
  row(p,n,y,W,ed){
    const f=D.Frame(p,0,y,W,96);D.Box(f,0,0,W,96,10,C.PANEL,C.LINE);D.Box(f,0,14,3,68,2,NEWS_TAGS[n.tag].color);
    const tw=this.tag(f,20,14,n);D.Text(f,20+tw+12,12,W-tw-210,28,Fit(n.title,W-tw-210,19),19,C.TEXT);
    D.Text(f,20,44,W-40,24,Fit(n.text,W-40,15),15,C.DIM);D.Text(f,20,68,W-40,20,n.author+' · '+n.time,13,C.DIM2);
    if(ed)this.actions(f,W-8,10,n);
  },
  form(r){
    const f=D.Frame(r,776,0,396,720),d=this.f;D.Box(f,0,0,396,720,10,C.PANEL,C.LINE);D.Cap(f,24,20,348,d.id?'Правка новости':'Новая новость');
    D.Cap(f,24,54,348,'Тип');NEWS_TAGS.forEach((t,i)=>{const x=24+(i%2)*178,y=78+Math.floor(i/2)*44,on=d.tag===i;D.Box(f,x,y,170,38,8,on?C.PANEL3:C.PANEL2,on?t.color:C.LINE2);D.Text(f,x,y,170,38,t.name,14,on?C.TEXT:C.DIM,'c');D.Hit(f,x,y,170,38,()=>{d.tag=i;this.paint();});});
    D.Cap(f,24,178,240,'Заголовок');const cT=D.Text(f,272,178,100,18,'',13,C.DIM2,'r'),a=UI.Input(f,24,200,348,48,'Коротко о главном',60,d.title);
    D.Cap(f,24,262,240,'Текст');const cX=D.Text(f,272,262,100,18,'',13,C.DIM2,'r'),b=UI.Area(f,24,284,348,250,'Что хотите сказать клану',400,d.text);
    const up=()=>{cT.textContent=d.title.length+'/60';cX.textContent=d.text.length+'/400';};up();
    a.input.oninput=()=>{d.title=a.input.value;up();};b.input.oninput=()=>{d.text=b.input.value;up();};
    D.Text(f,24,546,270,36,'Закрепить сверху',15,C.TEXT);UI.Switch(f,328,552,d.pin,v=>{d.pin=v;});
    UI.Button(f,24,596,348,52,d.id?'Сохранить':'Опубликовать','primary','check',()=>this.save(),16);
    if(d.id)UI.Button(f,24,656,348,44,'Отмена','secondary',null,()=>{this.f=this.blank();this.paint();},14);
  },
  save(){
    const c=S.clan,d=this.f,t=d.title.trim(),x=d.text.trim();if(!t||!x){Toast('Нужны заголовок и текст','err');return;}
    if(d.pin)c.news.forEach(n=>{n.pin=false;});
    if(d.id){Object.assign(c.news.find(n=>n.id===d.id),{tag:d.tag,title:t,text:x,pin:d.pin});Toast('Новость обновлена');}
    else{c.news.unshift({id:Date.now(),tag:d.tag,title:t,text:x,pin:d.pin,author:S.name,time:Pages.csettings.now()});c.journal.unshift(['news','Новость: '+t,'сейчас']);Toast('Новость опубликована');}
    this.f=this.blank();this.paint();
  },
  pin(n){const on=!n.pin;S.clan.news.forEach(x=>{x.pin=false;});n.pin=on;this.paint();},
  del(n){UI.Ask({title:'Удалить новость?',text:'«'+n.title+'» исчезнет у всех участников.',yes:'Удалить',danger:true,fn:()=>{S.clan.news=S.clan.news.filter(x=>x!==n);if(this.f.id===n.id)this.f=this.blank();this.paint();Toast('Новость удалена');}});},
};
