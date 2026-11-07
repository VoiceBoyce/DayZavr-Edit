// «Админ»: порт INT_PDA_PageAdmin.c и INT_PDA_PageAdminFactions.c. Подвкладки: игроки, группировки, новости, система.
Pages.admin = {
  BAR_H:60, AREA_Y:76, ROW_H:44, sub:0, sel:null, first:0, filter:'', showPass:false,

  build(r,W,H){
    this.r=r; this.W=W; this.H=H;
    const b=D.Frame(r,0,0,W,this.BAR_H,'Bar'); D.Line(b,40,this.BAR_H-1,W-80,1,C.LINE);
    let x=40; this.subs=[];
    ['ADMIN_SUB_PLAYERS','ADMIN_SUB_FACTIONS','ADMIN_SUB_NEWS','ADMIN_SUB_SYSTEM'].forEach((k,i)=>{
      const cap=Caps(k), tw=TextW(cap,20)+4, t=D.Text(b,x,0,tw,this.BAR_H-6,cap,20,C.DIM);
      D.Hit(b,x-8,0,tw+16,this.BAR_H,()=>this.setSub(i),on=>{if(i!==this.sub)t.style.color=on?C.TEXT:C.DIM;});
      this.subs.push({t,x,w:tw}); x+=tw+44;
    });
    this.line=D.Line(b,40,this.BAR_H-4,10,3,C.ORANGE); this.line.classList.add('tabline');
    const cap=Caps('ADMIN_REFRESH'), tw=TextW(cap,18)+4, rx=W-40-tw;
    const rt=D.Text(b,rx,0,tw,this.BAR_H-6,cap,18,C.DIM), ri=D.Icon(b,rx-30,17,20,'refresh',C.DIM);
    D.Hit(b,rx-38,0,tw+46,this.BAR_H,()=>Toast(T('MAP_UPDATED'),'ok'),on=>rt.style.color=ri.style.color=on?C.TEXT:C.DIM);
    this.area=D.Frame(r,0,this.AREA_Y,W,H-this.AREA_Y,'Area');
    this.setSub(this.sub);
  },
  setSub(i){
    this.sub=i;
    this.subs.forEach((s,k)=>s.t.style.color=k===i?C.TEXT:C.DIM);
    this.line.style.left=this.subs[i].x+'px'; this.line.style.width=this.subs[i].w+'px';
    this.area.innerHTML='';
    [this.players,this.factions,this.newsTab,this.system][i].call(this,this.area);
    Look.Reveal(this.area,0,8,220);
  },
  panel(p,x,y,w,h,key){
    const f=Look.Panel(p,x,y,w,h); D.Icon(f,18,13,15,'admin',C.ADMIN);
    D.Text(f,40,12,w-58,18,Caps(key),13,C.ADMIN); return f;
  },
  // --- игроки ---------------------------------------------------------------------------------------------------------
  list(){
    const q=this.filter.toLowerCase();
    return REAL.Players.filter(p=>!q||p.Callsign.toLowerCase().includes(q)||p.SteamId.includes(q));
  },
  players(a){
    const s=UI.Input(a,40,0,470,44,T('ADMIN_PL_SEARCH_HINT'),24,'search',this.filter);
    const find=()=>{this.filter=s.input.value;this.first=0;this.tablePaint();};
    s.input.onkeydown=e=>{if(e.key==='Enter')find();};
    UI.Button(a,520,0,130,44,Caps('ADMIN_PL_FIND'),'primary','search',find);
    UI.Button(a,660,0,110,44,Caps('ADMIN_PL_ALL'),'secondary',null,()=>{s.input.value='';find();});
    this.found=D.Text(a,780,0,200,44,'',13,C.DIM,'r');
    const t=this.table=D.Frame(a,40,58,940,484,'Table'); D.Box(t,0,0,940,484,6,C.PANEL,C.LINE);
    [[34,'F_CALLSIGN'],[182,'ADMIN_PL_COL_GAME'],[290,'ADMIN_PL_COL_STEAM'],[470,'STAT_FACTION'],[602,'ADMIN_PL_COL_ROLE'],[706,'ADMIN_PL_COL_RANK'],[810,'ADMIN_PL_COL_SEEN']].forEach(([x,k])=>D.Text(t,x,0,120,40,Caps(k),13,C.ADMIN));
    D.Line(t,0,40,940,1,C.LINE);
    this.tbody=D.Frame(t,0,0,940,484,'Body');
    t.onwheel=e=>{e.preventDefault();const n=this.list().length;this.first=Math.max(0,Math.min(Math.max(0,n-10),this.first+(e.deltaY>0?1:-1)));this.tablePaint();};
    UI.Button(a,40,552,40,40,'','secondary','chevron_left',()=>{this.first=Math.max(0,this.first-10);this.tablePaint();});
    this.pageT=D.Text(a,88,552,180,40,'',15,C.TEXT,'c');
    UI.Button(a,276,552,40,40,'','secondary','chevron_right',()=>{const n=this.list().length;this.first=Math.min(Math.max(0,n-10),this.first+10);this.tablePaint();});
    D.Text(a,334,552,300,40,T('ADMIN_PL_WHEEL'),13,C.DIM2);
    this.rangeT=D.Text(a,640,552,340,40,'',14,C.DIM,'r');
    this.card=this.panel(a,1000,0,416,592,'ADMIN_PL_CARD');
    this.tablePaint(); this.cardPaint();
  },
  tablePaint(){
    const b=this.tbody; b.innerHTML=''; const l=this.list(), H=this.ROW_H;
    this.found.textContent=T('ADMIN_PL_FOUND')+': '+l.length;
    this.pageT.textContent=T('ADMIN_PL_PAGE')+' '+(Math.floor(this.first/10)+1)+' '+T('ADMIN_PL_OF')+' '+Math.max(1,Math.ceil(l.length/10));
    this.rangeT.textContent=(l.length?this.first+1:0)+'–'+Math.min(l.length,this.first+10)+' '+T('ADMIN_PL_OF')+' '+l.length;
    if(!l.length){D.Text(b,0,230,940,30,T('ADMIN_PL_EMPTY'),17,C.DIM2,'c');return;}
    l.slice(this.first,this.first+10).forEach((p,i)=>{
      const f=D.Frame(b,1,41+i*H,938,H,'Row'), sel=this.sel===p, bg=D.Line(f,0,0,938,H-1,sel?'#22160C':'transparent');
      D.Line(f,12,H-1,914,1,C.LINE); if(sel) D.Line(f,0,0,3,H-1,C.ORANGE);
      const on=Data.online(p.Callsign), fac=Data.faction(p.FactionId);
      D.Circle(f,14,17,10,on?C.ALLY:C.DIM2);
      D.Text(f,34,0,140,H,Fit(p.Callsign,140,16),16,C.TEXT);
      D.Text(f,182,0,100,H,Fit(p.GameName||'',100,15),15,C.DIM);
      D.Text(f,290,0,172,H,p.SteamId,13,C.DIM);
      D.Text(f,470,0,124,H,fac?Fit(fac.Name,124,15):'—',15,C.TEXT);
      D.Text(f,602,0,96,H,Fit(p.Role?T(Look.RoleKey(p.Role)):'—',96,15),15,C.DIM);
      D.Text(f,706,0,96,H,Fit(p.Rank||'—',96,15),15,C.DIM);
      D.Text(f,810,0,118,H,(p.LastSeen||'').slice(0,16),13,C.DIM);
      D.Hit(f,0,0,938,H,()=>{this.sel=p;this.showPass=false;this.tablePaint();this.cardPaint();},h=>{if(!sel)bg.style.background=h?'rgba(255,255,255,.03)':'transparent';});
    });
  },
  cardPaint(){
    const c=this.card; [...c.children].slice(3).forEach(x=>x.remove());
    const p=this.sel;
    if(!p){ D.Icon(c,176,220,64,'user',C.DIM2); D.Text(c,20,302,376,26,T('ADMIN_PL_PICK'),16,C.DIM,'c'); return; }
    const av=D.Frame(c,18,44,84,84,'Av'); D.Box(av,0,0,84,84,6,C.PANEL2,C.LINE2); D.Avatar(av,3,3,78,p.Avatar); D.Line(av,3,78,78,3,C.ORANGE);
    D.Text(c,118,40,284,40,Fit(p.Callsign,284,30),30,C.TEXT);
    const on=Data.online(p.Callsign); D.Circle(c,120,89,10,on?C.ALLY:C.DIM2); D.Text(c,138,82,264,24,T(on?'ADMIN_PL_ONLINE':'ADMIN_PL_OFFLINE'),14,C.DIM);
    const fac=Data.faction(p.FactionId);
    D.Text(c,118,106,284,22,fac?fac.Name:T('NO_FACTION'),14,C.DIM);
    D.Line(c,18,142,380,1,C.LINE);
    const rows=[['ADMIN_PL_COL_STEAM',p.SteamId],['ADMIN_PL_COL_GAME',p.GameName],['F_DISCORD',p.Discord],['ADMIN_PL_REGISTERED',(p.Registered||'').slice(0,16)],['ADMIN_PL_COL_SEEN',(p.LastSeen||'').slice(0,16)],['STAT_FACTION',fac?fac.Name:'—'],['ADMIN_PL_COL_ROLE',p.Role?T(Look.RoleKey(p.Role)):'—'],['ADMIN_PL_COL_RANK',p.Rank||'—'],['F_PASSWORD',this.showPass?'zona2026':'••••••••']];
    rows.forEach(([k,v],i)=>{const fy=150+i*30;D.Text(c,18,fy,i===8?50:140,30,Caps(k),13,C.DIM);D.Text(c,160,fy,238,30,Fit(v||'—',238,15),15,C.TEXT);});
    const fy=390, e=D.Icon(c,70,fy+5,20,this.showPass?'eye_off':'eye',C.DIM); D.Text(c,94,fy,64,30,T(this.showPass?'ADMIN_PL_HIDE':'ADMIN_PL_SHOW'),12,C.DIM);
    D.Hit(c,18,fy,140,30,()=>{this.showPass=!this.showPass;this.cardPaint();});
    D.Line(c,18,424,380,1,C.LINE);
    D.Text(c,18,434,380,18,Caps('ADMIN_PL_ROLE_IN_GP'),13,C.ADMIN);
    UI.Dropdown(c,18,458,190,[T('ADMIN_PL_ROLE_NONE'),T('ROLE_MEMBER'),T('ROLE_DEPUTY'),T('ROLE_LEADER')],p.Role||0,40);
    UI.Button(c,218,458,180,40,Caps('ADMIN_PL_KICK'),'warning','kick',()=>UI.Ask(this.r,{title:T('ADMIN_CF_KICK_T'),value:p.Callsign,text:T('ADMIN_CF_KICK'),danger:true}),14);
    UI.Button(c,18,514,190,44,Caps('BTN_PASSWORD'),'secondary','key',()=>Shell.openPrompt(T('BTN_PASSWORD'),T('F_NEW_PASSWORD'),'',false,'',ok=>{if(ok)Toast(T('R_OK'),'ok');}),14);
    UI.Button(c,218,514,180,44,Caps('ADMIN_PL_DELETE'),'danger','trash',()=>UI.Ask(this.r,{title:T('ADMIN_CF_DELETE_T'),value:p.Callsign,text:T('ADMIN_CF_DELETE'),danger:true}),14);
  },
  // --- группировки: 400 список + редактор на четыре раздела -------------------------------------------------------------
  fSel:null, fSec:0, fFirst:0,
  factions(a){
    const root=D.Frame(a,40,0,1376,592,'AdmF'), L=D.Frame(root,0,0,400,592,'List');
    D.Box(L,0,0,400,592,6,C.PANEL,C.LINE); D.Line(L,0,10,3,26,C.ADMIN); D.Icon(L,18,13,18,'flag',C.ADMIN);
    D.Text(L,46,13,200,18,Caps('AF_LIST'),13,C.ADMIN);
    D.Text(L,240,13,142,18,REAL.Factions.length+' '+T('AF_PPL'),13,C.ORANGE,'r');
    UI.Button(L,16,46,368,44,Caps('AF_CREATE'),'primary','plus',()=>Shell.openPrompt(T('AF_NEW_TITLE'),T('AF_F_NAME'),'',false,'',ok=>{if(ok)Toast(T('AF_CREATED'),'ok');}));
    const rows=D.Frame(L,0,0,400,592,'Rows');
    const paint=()=>{
      rows.innerHTML='';
      REAL.Factions.slice(this.fFirst,this.fFirst+7).forEach((f,i)=>{
        const y=102+i*62, row=D.Frame(rows,8,y,384,56,'Row'), sel=this.fSel===f, box=D.Box(row,0,0,384,56,4,sel?'#22160C':C.PANEL,sel?C.ORANGE:C.LINE);
        D.Emblem(row,8,6,44,f.Emblem);
        D.Text(row,62,6,300,24,Fit(f.Name,300,18),18,C.TEXT);
        const lead=f.Members.find(m=>m.Role===3);
        D.Text(row,62,31,300,18,f.Members.length+' '+T('AF_PPL')+' · '+(lead?lead.Callsign:T('AF_NO_LEADER')),13,C.DIM);
        D.Hit(row,0,0,384,56,()=>{this.fSel=f;this.fSec=0;paint();this.editor(E);},h=>{if(!sel)box.style.background=h?C.PANEL2:C.PANEL;});
      });
    };
    L.onwheel=e=>{e.preventDefault();this.fFirst=Math.max(0,Math.min(REAL.Factions.length-7,this.fFirst+(e.deltaY>0?1:-1)));paint();};
    const E=D.Frame(root,420,0,956,592,'Editor');
    paint(); this.editor(E);
  },
  editor(E){
    E.innerHTML=''; const EW=956, f=this.fSel;
    if(!f){
      const cy=196; D.Icon(E,EW*.5-32,cy,64,'flag',C.DIM2);
      D.Text(E,0,cy+84,EW,30,Caps('AF_NONE_TITLE'),22,C.TEXT,'c'); D.Text(E,0,cy+124,EW,24,T('AF_NONE_TEXT'),16,C.DIM,'c'); return;
    }
    const em=D.Frame(E,0,2,64,64,'Emb'); D.Box(em,0,0,64,64,6,C.PANEL2,C.LINE2); D.Emblem(em,4,4,56,f.Emblem); D.Line(em,4,60,56,3,C.ORANGE);
    D.Text(E,82,-2,EW-420,44,Fit(CapsT(f.Name),EW-420,34),34,C.TEXT);
    D.Text(E,84,44,EW-420,20,T('AF_ID')+': '+f.Id+' · '+f.Members.length+' '+T('AF_PPL'),14,C.DIM);
    D.Icon(E,EW-18,41,16,'admin',C.ADMIN); D.Text(E,EW-330,40,304,18,Caps('AF_ADMIN_EDIT'),13,C.ADMIN,'r');
    let tx=0; const secs=[]; ['AF_SEC_MAIN','AF_SEC_MEMBERS','AF_SEC_RANKS','AF_SEC_RELATIONS'].forEach((k,i)=>{
      const cap=Caps(k), tw=TextW(cap,18)+4, t=D.Text(E,tx,78,tw,36,cap,18,i===this.fSec?C.TEXT:C.DIM);
      D.Hit(E,tx-6,78,tw+12,40,()=>{this.fSec=i;this.editor(E);},h=>{if(i!==this.fSec)t.style.color=h?C.TEXT:C.DIM;});
      secs.push({x:tx,w:tw}); tx+=tw+34;
    });
    D.Line(E,0,119,EW,1,C.LINE); D.Line(E,secs[this.fSec].x,116,secs[this.fSec].w,3,C.ORANGE);
    const sh=592-132, b=D.Frame(E,0,132,EW,sh,'Sec');
    [this.secMain,this.secMem,this.secRanks,this.secRel][this.fSec].call(this,b,f,EW,sh);
  },
  secMain(b,f,EW,sh){
    const CW=168, ch=CW*4/3, fx=CW+28, fw=EW-fx, half=(fw-20)*.6, rx=fx+half+20, rw=fw-half-20;
    Look.FactionCard(b,0,0,CW,f.Emblem,f.Name);
    UI.Button(b,0,ch+12,CW,40,Caps('AF_EMBLEM_PICK'),'secondary','star',()=>this.emblemPop(f),14);
    D.Text(b,0,ch+60,CW,20,'№ '+f.Emblem,13,C.DIM,'c');
    Look.Label(b,fx,0,200,'AF_F_NAME'); D.Text(b,fx+200,0,half-200,18,T('AF_NAME_HINT'),13,C.DIM2,'r');
    UI.Input(b,fx,22,half,48,T('AF_PH_NAME'),32,null,f.Name);
    Look.Label(b,rx,0,rw,'AF_F_ID'); D.Text(b,rx,22,rw,48,f.Id,26,C.ADMIN);
    Look.Label(b,fx,84,300,'AF_F_MOTTO'); UI.Input(b,fx,106,fw,48,T('AF_PH_MOTTO'),120,null,f.Motto);
    Look.Label(b,fx,168,300,'AF_F_BASE'); UI.Input(b,fx,190,half,48,T('AF_PH_BASE'),60,null,f.Base);
    Look.Label(b,fx,252,300,'AF_F_SPAWN'); D.Text(b,fx+300,252,half-300,18,T('AF_SPAWN_HINT'),13,C.DIM2,'r');
    UI.Input(b,fx,274,half,48,T('AF_PH_SPAWN'),40);
    UI.Button(b,rx,274,rw,48,Caps('AF_MY_POS'),'secondary','pin',()=>Toast(T('AF_POS_SET'),'ok'));
    D.MText(b,fx,336,fw,44,T('AF_SPAWN_NOTE'),13,C.DIM2);
    UI.Button(b,fx,sh-56,280,48,Caps('AF_DELETE'),'danger','trash',()=>UI.Ask(this.r,{title:T('AF_DEL_TITLE'),value:f.Name,text:T('AF_DEL_CONFIRM'),danger:true}));
    UI.Button(b,EW-280,sh-56,280,48,Caps('AF_SAVE'),'primary','check',()=>Toast(T('AF_SAVED'),'ok'));
  },
  emblemPop(f){
    const W=this.W,H=this.H,pw=9*72+40,ph=5*72+90,m=D.Frame(this.r,0,0,W,H,'Ask'); m.style.zIndex=60;
    D.Line(m,0,0,W,H,'rgba(0,0,0,.67)'); D.Hit(m,0,0,W,H,()=>m.remove());
    const b=D.Frame(m,(W-pw)/2,(H-ph)/2,pw,ph,'Pop'); D.Box(b,0,0,pw,ph,8,C.PANEL,C.LINE2); D.Line(b,0,0,pw,3,C.ORANGE);
    D.Text(b,20,16,pw-40,24,Caps('AF_EMBLEM_TITLE'),18,C.TEXT);
    for(let i=1;i<=45;i++){const x=20+(i-1)%9*72,y=52+Math.floor((i-1)/9)*72;if(i===f.Emblem)D.Box(b,x-3,y-3,70,70,6,null,C.ORANGE);D.Emblem(b,x,y,64,i);D.Hit(b,x,y,64,64,()=>{f.Emblem=i;m.remove();this.setSub(1);});}
  },
  secMem(b,f,EW,sh){
    D.Text(b,0,0,700,18,Caps('AF_MEMBERS')+' · '+f.Members.length,13,C.DIM);
    D.Box(b,0,26,EW,30,4,C.PANEL2);
    [[14,'AF_COL_CALLSIGN'],[220,'AF_COL_STEAM'],[440,'AF_COL_RANK'],[620,'AF_COL_ROLE']].forEach(([x,k])=>D.Text(b,x,26,190,30,Caps(k),13,C.DIM));
    if(!f.Members.length) D.Text(b,0,142,EW,26,T('AF_MEMBERS_EMPTY'),16,C.DIM2,'c');
    f.Members.slice(0,7).forEach((m,i)=>{
      const row=D.Frame(b,0,62+i*48,EW,44,'Mem'); D.Box(row,0,0,EW,44,4,C.PANEL,C.LINE);
      D.Text(row,14,0,200,44,Fit(m.Callsign,200,17),17,C.TEXT);
      D.Text(row,220,0,200,44,m.SteamId,13,C.DIM);
      D.Text(row,440,0,170,44,Fit(m.Rank,170,15),15,C.DIM);
      UI.Dropdown(row,EW-212,4,200,[T('AF_ROLE_KICK'),T('ROLE_MEMBER'),T('ROLE_DEPUTY'),T('ROLE_LEADER')],m.Role,36);
    });
    UI.Button(b,0,sh-52,300,48,Caps('AF_JOIN'),'secondary','invite',()=>Toast(T('AF_JOIN_OK'),'ok'));
    D.MText(b,320,sh-50,400,46,T('AF_MEMBERS_NOTE'),13,C.DIM2);
  },
  secRanks(b,f,EW,sh){
    D.Text(b,0,0,500,18,Caps('AF_RANKS')+' · '+f.Ranks.length,13,C.DIM);
    const inp=UI.Input(b,0,26,560,44,T('AF_PH_RANK'),24);
    UI.Button(b,576,26,200,44,Caps('AF_RANK_ADD'),'secondary','plus',()=>{if(inp.input.value){f.Ranks.push(inp.input.value);this.editorRe();}});
    D.MText(b,796,26,EW-796,44,T('AF_RANK_HINT'),13,C.DIM2);
    f.Ranks.slice(0,8).forEach((n,i)=>{
      const row=D.Frame(b,0,84+i*42,EW,38,'Rank'); D.Box(row,0,0,EW,38,4,C.PANEL,C.LINE);
      D.Text(row,14,0,40,38,String(i+1),15,C.DIM2); D.Text(row,54,0,500,38,n,17,C.TEXT);
      D.Icon(row,EW-34,10,18,'trash',C.DIM2); D.Hit(row,EW-42,0,36,38,()=>{f.Ranks.splice(i,1);this.editorRe();});
      D.Icon(row,EW-74,10,18,'edit',C.DIM2);
    });
    UI.Button(b,EW-520,sh-52,220,48,Caps('AF_RANK_RESET'),'secondary','refresh');
    UI.Button(b,EW-284,sh-52,284,48,Caps('AF_RANK_SAVE'),'primary','check',()=>Toast(T('AF_RANKS_SAVED'),'ok'));
  },
  secRel(b,f,EW){
    D.Text(b,0,0,500,18,Caps('AF_RELATIONS'),13,C.DIM);
    D.Text(b,EW-520,0,520,18,T('AF_REL_HINT'),13,C.DIM2,'r');
    Data.relationsOf(f.Id).slice(0,6).forEach((e,i)=>{
      const row=D.Frame(b,0,28+i*68,EW,60,'Rel'); D.Box(row,0,0,EW,60,4,C.PANEL,C.LINE);
      D.Emblem(row,10,6,48,e.f.Emblem); D.Text(row,70,0,256,60,Fit(e.f.Name,256,18),18,C.TEXT);
      UI.Dropdown(row,336,12,214,[T('AF_REL_NEUTRAL'),T('AF_REL_ALLY'),T('AF_REL_ENEMY')],e.Rel,36);
      UI.Input(row,562,8,EW-562-72,44,T('AF_PH_REASON'),120,null,e.Reason);
      UI.Button(row,EW-64,8,56,44,'','primary','check',()=>Toast(T('AF_REL_SAVED'),'ok'));
    });
  },
  editorRe(){ const E=this.area.querySelector('[data-w=Editor]'); if(E) this.editor(E); },
  // --- новости --------------------------------------------------------------------------------------------------------
  newsTab(a){
    const H=84, feed=this.panel(a,40,0,900,592,'ADMIN_NW_FEED');
    D.Text(feed,700,12,182,18,S.news.length+' '+T('FAC_TOTAL'),13,C.ADMIN,'r');
    S.news.slice(0,5).forEach((n,i)=>{
      const f=D.Frame(feed,1,40+i*H,898,H,'Row'); D.Line(f,18,H-1,862,1,C.LINE);
      D.Text(f,24,12,490,28,Fit(n.Title,490,19),19,C.TEXT);
      D.Text(f,520,14,306,20,n.Time+' · '+(n.Minutes?T('ADMIN_NW_LEFT').replace('%1',n.Minutes):T('ADMIN_NW_FOREVER')),13,C.DIM,'r');
      D.Text(f,24,46,800,26,Fit(n.Text,800,15),15,C.DIM);
      const t=D.Icon(f,850,(H-22)/2,22,'trash',C.DIM2);
      D.Hit(f,838,(H-46)/2,46,46,()=>UI.Ask(this.r,{title:T('ADMIN_CF_NEWS_T'),value:Fit(n.Title,560,28),text:T('ADMIN_CF_NEWS'),danger:true,onYes:()=>{S.news=S.news.filter(x=>x!==n);this.setSub(2);Shell.updateNews();}}),h=>t.style.color=h?C.ENEMY:C.DIM2);
    });
    const sy=40+5*H; D.Line(feed,18,sy,862,1,C.LINE2);
    D.Text(feed,24,sy+10,600,18,Caps('ADMIN_NW_SETTINGS'),13,C.ADMIN);
    Look.Label(feed,24,sy+34,220,'ADMIN_NW_ROTATE'); UI.Input(feed,24,sy+56,120,44,'',3,null,'8');
    Look.Label(feed,270,sy+34,300,'ADMIN_NW_DEFMIN'); UI.Input(feed,270,sy+56,140,44,'',6,null,'0');
    UI.Button(feed,680,sy+56,196,44,Caps('ADMIN_SYS_SAVE'),'primary','check',()=>Toast(T('R_OK'),'ok'),14);
    const form=this.panel(a,960,0,456,592,'ADMIN_NW_NEW');
    Look.Label(form,18,44,280,'ADMIN_NW_TITLE');
    const tc=D.Text(form,298,44,140,18,'0 / 48',13,C.DIM2,'r'), ti=UI.Input(form,18,66,420,44,'',48);
    Look.Label(form,18,124,280,'ADMIN_NW_TEXT');
    const xc=D.Text(form,298,124,140,18,'0 / 300',13,C.DIM2,'r'), xi=UI.Input(form,18,146,420,44,'',300);
    Look.Label(form,18,204,200,'ADMIN_NW_MIN'); D.Text(form,200,204,238,18,T('ADMIN_NW_MIN_SUB'),13,C.DIM2,'r');
    const mi=UI.Input(form,18,226,130,44,'',6,null,'0');
    Look.Label(form,18,284,280,'ADMIN_NW_PREVIEW');
    const pv=D.Frame(form,18,306,420,158,'Preview'); D.Box(pv,0,0,420,158,4,C.PANEL2,C.LINE); D.Line(pv,0,0,3,158,C.ORANGE);
    const pt=D.Text(pv,18,10,388,28,'',18,C.TEXT), px=D.MText(pv,18,42,388,108,'',15,C.DIM);
    const upd=()=>{tc.textContent=ti.input.value.length+' / 48';xc.textContent=xi.input.value.length+' / 300';pt.textContent=Fit(ti.input.value,388,18);px.textContent=xi.input.value;};
    ti.input.oninput=xi.input.oninput=upd;
    UI.Button(form,18,526,420,48,Caps('ADMIN_NW_POST'),'primary','check',()=>{
      if(!ti.input.value||!xi.input.value){Toast(T('R_TEXT'),'err');return;}
      S.news.unshift({Id:Date.now(),Time:Pages.chat.now(),Title:ti.input.value,Text:xi.input.value,Minutes:+mi.input.value||0});
      Toast(T('R_OK'),'ok'); Shell.updateNews(); this.setSub(2);
    });
  },
  // --- система --------------------------------------------------------------------------------------------------------
  system(a){
    const sp=this.panel(a,40,0,660,236,'ADMIN_SYS_SETTINGS');
    D.Text(sp,24,46,612,30,Caps('ADMIN_SYS_RADIUS'),22,C.TEXT);
    D.Text(sp,24,80,612,22,T('ADMIN_SYS_RADIUS_SUB'),14,C.DIM);
    UI.Input(sp,24,140,130,48,'',5,null,'5');
    D.Text(sp,164,140,40,48,T('ADMIN_SYS_M'),18,C.DIM);
    UI.Button(sp,210,140,180,48,Caps('ADMIN_SYS_SAVE'),'primary','check',()=>Toast(T('R_OK'),'ok'));
    D.Text(sp,400,140,236,48,T('ADMIN_SYS_NOW')+': 5 '+T('ADMIN_SYS_M'),15,C.DIM,'r');
    const rp=this.panel(a,40,256,660,336,'ADMIN_SYS_RELOAD');
    [['ADMIN_SYS_RL_DB','ADMIN_SYS_RL_DB_SUB','users','ADMIN_CF_RELOAD_DB'],['ADMIN_SYS_RL_FACTIONS','ADMIN_SYS_RL_FACTIONS_SUB','flag','ADMIN_CF_RELOAD_FACTIONS'],['ADMIN_SYS_RL_CONFIG','ADMIN_SYS_RL_CONFIG_SUB','note','ADMIN_CF_RELOAD_CONFIG']].forEach(([k,sub,ic,cf],i)=>{
      const row=D.Frame(rp,18,44+i*94,624,84,'Reload');
      D.Box(row,0,0,624,84,4,C.PANEL2,C.LINE); D.Box(row,16,16,52,52,6,C.PANEL); D.Icon(row,28,28,28,ic,C.ORANGE);
      D.Text(row,84,12,340,30,Caps(k),20,C.TEXT); D.Text(row,84,46,350,22,Fit(T(sub),350,14),14,C.DIM);
      UI.Button(row,444,20,164,44,Caps('ADMIN_SYS_RELOAD_BTN'),'secondary','refresh',()=>UI.Ask(this.r,{title:T('ADMIN_CF_RELOAD_T'),value:T(k),text:T(cf),onYes:()=>Toast(T('R_OK'),'ok')}),14);
    });
    const ip=this.panel(a,720,0,696,592,'ADMIN_SUB_SYSTEM');
    [['ADMIN_SYS_VERSION','1.1.0'],['ADMIN_SYS_PROTOCOL','2'],['ADMIN_SYS_MY_STEAM',S.me.SteamId],['ADMIN_SYS_ACCOUNT',S.me.Callsign],['ADMIN_SYS_MODE',T(S.adminMode?'ADMIN_SYS_YES':'ADMIN_SYS_OFF')],['ADMIN_SYS_RIGHTS',T('ADMIN_SYS_YES')]].forEach(([k,v],i)=>{
      const cx=24+(i%2)*332, cy=56+Math.floor(i/2)*116;
      D.Line(ip,cx,cy,2,80,i<2?C.ORANGE:C.LINE2);
      D.Text(ip,cx+16,cy,300,18,Caps(k),13,C.DIM);
      D.Text(ip,cx+16,cy+24,304,44,Fit(v,304,28),28,C.TEXT);
    });
    D.Line(ip,24,420,648,1,C.LINE);
    D.Icon(ip,24,442,18,'note',C.DIM2);
    D.MText(ip,52,438,620,60,T('ADMIN_SYS_NOTE'),14,C.DIM2);
  },
};
