// Оболочка КПК: порт INT_PDA_Menu. Корпус 1500x890, экран (22, 24) 1456x800: верх 88, тело 674, низ 38.
const TAB = {PROFILE:0,MAP:1,CHAT:2,FACTION:3,NEWS:4,ADMIN:5,CHAR:6};
const TAB_KEYS = ['TAB_PROFILE','TAB_MAP','TAB_CHAT','TAB_FACTION','TAB_NEWS','TAB_ADMIN','TAB_CHAR'];
const Pages = {};
const PAGE_OF = {0:'profile',1:'map',2:'chat',3:'faction',4:'news',5:'admin',6:'char'};

const Shell = {
  tab:-1, tabIds:[], tabUI:[], page:null, pageRoot:null, closing:false, prompt:null, statusT:0, newsT:0, newsIdx:0,

  init(){
    this.stage=document.getElementById('stage');
    this.dim=document.getElementById('dim');
    this.hint=document.getElementById('hint');
    this.fit(); window.addEventListener('resize',()=>this.fit());
    window.addEventListener('keydown',e=>this.key(e));
    window.addEventListener('click',()=>{const d=document.querySelector('.ddp'); if(d) d.remove();});
    let last=performance.now();
    const loop=t=>{const dt=Math.min(.1,(t-last)/1000);last=t;this.tick(dt);requestAnimationFrame(loop);};
    requestAnimationFrame(loop);
    this.open();
  },
  fit(){
    const k=Math.min(innerWidth/1920,innerHeight/1080), s=this.stage.style;
    s.transform=`scale(${k})`; s.left=(innerWidth-1920*k)/2+'px'; s.top=(innerHeight-1080*k)/2+'px';
  },

  // --- открытие и закрытие -----------------------------------------------------------------------------------------
  open(){
    this.destroy();
    this.closing=false;
    document.getElementById('hud').style.display='none';
    this.hint.style.display='none';
    this.dim.style.opacity=1;
    if(S.screen==='hud'){ this.dim.style.opacity=0; Hud.show(); return; }
    if(S.screen==='off') S.screen='main';
    const devX=(1920-LOOK.DEV_W)/2, devY=(1080-LOOK.DEV_H)/2;
    const dev=this.device=D.Frame(this.stage,devX,devY+70,LOOK.DEV_W,LOOK.DEV_H,'Device'); dev.id='device';
    D.Img(dev,0,0,LOOK.DEV_W,LOOK.DEV_H,'assets/device.png');
    const scr=this.screenEl=D.Clip(dev,LOOK.SCR_X,LOOK.SCR_Y,LOOK.SCR_W,LOOK.SCR_H,'Screen');
    this.background(scr);
    const c=D.Frame(scr,0,0,LOOK.SCR_W,LOOK.SCR_H,'Content');
    this.buildTop(c);
    this.body=D.Frame(c,0,LOOK.TOP_H,LOOK.SCR_W,LOOK.BODY_H,'Body');
    this.buildFoot(c);
    this.overlay=D.Frame(scr,0,0,LOOK.SCR_W,LOOK.SCR_H,'Overlay'); this.overlay.style.pointerEvents='none';
    const sh=this.shutter=D.Frame(scr,0,0,LOOK.SCR_W,LOOK.SCR_H,'Shutter'); sh.style.pointerEvents='none';
    const h=LOOK.SCR_H;
    this.halfT=D.Line(sh,0,0,LOOK.SCR_W,h/2,'#050505'); this.halfB=D.Line(sh,0,h/2,LOOK.SCR_W,h/2,'#050505');
    this.halfT.classList.add('shutter'); this.halfB.classList.add('shutter');
    this.scan=D.Line(sh,0,h/2-1,LOOK.SCR_W,2,C.ORANGE); this.scan.style.transition='opacity .35s';
    const start=()=>{
      dev.style.top=devY+'px';
      this.halfT.style.transform=`translateY(${-h/2-4}px)`; this.halfB.style.transform=`translateY(${h/2+4}px)`; this.scan.style.opacity=0;
    };
    if(window.NOANIM) start(); else setTimeout(start,40);
    if(S.screen==='boot') this.boot(); else this.enter(S.screen);
  },
  powerOff(){
    if(this.closing||!this.device) return;
    this.closing=true;
    if(this.page&&this.page.onHide) this.page.onHide();
    const h=LOOK.SCR_H;
    this.halfT.style.transition=this.halfB.style.transition='transform .22s ease-in';
    this.halfT.style.transform=this.halfB.style.transform='none';
    this.scan.style.opacity=1;
    setTimeout(()=>{this.device.style.top=(1080-LOOK.DEV_H)/2+90+'px';this.device.style.opacity=0;},180);
    setTimeout(()=>{this.destroy();S.screen='off';this.dim.style.opacity=0;this.hint.style.display='block';Maket.sync();},520);
  },
  destroy(){
    if(this.device) this.device.remove();
    this.device=null; this.page=null; this.tab=-1; this.tabUI=[]; this.tabIds=[]; this.prompt=null; this.bootT=-1; this.body=null;
    Hud.hide();
  },
  // Сланец: плоский #0B0B0C, виньетка и лёгкий блик сверху (BuildBackground).
  background(s){
    D.Line(s,0,0,LOOK.SCR_W,LOOK.SCR_H,C.BG);
    const v=D.Line(s,0,0,LOOK.SCR_W,LOOK.SCR_H,'radial-gradient(ellipse 72% 72% at 50% 50%,transparent 62%,rgba(0,0,0,.45) 100%)'); v.style.background='radial-gradient(ellipse 72% 72% at 50% 50%,transparent 62%,rgba(0,0,0,.45) 100%)';
    const g=D.Line(s,0,0,LOOK.SCR_W,90,'transparent'); g.style.background='linear-gradient(180deg,rgba(255,255,255,.035),transparent)';
  },

  // --- верхняя панель ------------------------------------------------------------------------------------------------
  buildTop(c){
    const w=LOOK.SCR_W, top=this.top=D.Frame(c,0,0,w,LOOK.TOP_H,'Top');
    D.Line(top,0,LOOK.TOP_H-1,w,1,C.LINE);
    const sx=1156;
    this.clock=D.Text(top,sx,8,110,36,'00:00',30,C.TEXT);
    this.batIcon=D.Icon(top,sx+100,14,22,'battery',C.TEXT);
    this.batText=D.Text(top,sx+126,12,70,24,'--%',16,C.TEXT);
    D.Icon(top,sx+196,12,22,'signal',C.ORANGE);
    const mods=['GPS',T('CHIP_MAP'),T('CHIP_CHAT'),T('CHIP_GP')], keys=['gps','map','chat','gp'];
    let rowW=0; const widths=mods.map(m=>{const l=TextW(m.toUpperCase(),13)+16;rowW+=l+6;return l;});
    let mx=1386-rowW; this.chips=[];
    mods.forEach((m,i)=>{
      const cw=widths[i], chip=D.Frame(top,mx,52,cw,20,'Mod'), box=D.Box(chip,0,0,cw,20,3,null,C.LINE2), t=D.Text(chip,0,0,cw,20,m.toUpperCase(),13,C.DIM,'c');
      this.chips.push({box,t,key:keys[i]}); mx+=cw+6;
    });
    const xb=D.Frame(top,1396,23,42,42,'Close'); D.Box(xb,0,0,42,42,6,null,C.LINE2);
    const cx=D.Icon(xb,11,11,20,'close',C.TEXT);
    D.Hit(top,1396,23,42,42,()=>this.powerOff(),on=>cx.style.color=on?C.ORANGE:C.TEXT);
    this.tabLine=D.Line(top,40,LOOK.TOP_H-4,10,4,C.ORANGE); this.tabLine.classList.add('tabline'); this.tabLine.style.display='none';
    this.adminFlag=D.Text(top,40,62,600,18,'',13,C.ORANGE);
    const ht=this.headTitle=D.Frame(top,40,0,1000,LOOK.TOP_H,'HeadTitle');
    D.Icon(ht,0,32,24,'signal',C.DIM);
    const prod=Caps('PRODUCT'), pw=TextW(prod,24)+4;
    D.Text(ht,38,0,pw,LOOK.TOP_H,prod,24,C.TEXT);
    D.Text(ht,38+pw+10,0,20,LOOK.TOP_H,'/',24,C.ORANGE);
    D.Text(ht,38+pw+38,0,500,LOOK.TOP_H,Caps('HEAD_NET'),24,C.DIM);
    ht.style.display='none';
    this.status();
  },
  buildTabs(){
    this.tabIds=[TAB.PROFILE,TAB.MAP,TAB.CHAT,TAB.FACTION,TAB.NEWS,TAB.CHAR]; if(S.isAdmin) this.tabIds.push(TAB.ADMIN);
    let size=24, gap=26, total=0;
    this.tabIds.forEach(id=>total+=TextW(Caps(TAB_KEYS[id]),size)+gap+30);
    if(total>1080){ size=24*1080/total; gap=18; }
    let x=40; this.tabUI=[];
    this.tabIds.forEach(id=>{
      const cap=Caps(TAB_KEYS[id]), tw=TextW(cap,size)+4, tx=id===TAB.ADMIN?x+28:x, extras=[];
      if(id===TAB.ADMIN) extras.push(D.Icon(this.top,x,32,22,'admin',C.ORANGE));
      const t=D.Text(this.top,tx,0,tw,LOOK.TOP_H,cap,size,id===TAB.ADMIN?C.ADMIN:C.DIM);
      const badge=D.Frame(this.top,tx+tw+4,31,26,26,'Badge'); D.Pill(badge,0,0,26,26,C.ORANGE);
      const bt=D.Text(badge,0,0,26,26,'',15,C.ON_ORANGE,'c'); badge.style.display='none';
      const hit=D.Hit(this.top,x,0,tx+tw-x+30,LOOK.TOP_H,()=>this.select(id),on=>{ if(id!==this.tab) t.style.color=on?C.TEXT:(id===TAB.ADMIN?C.ADMIN:C.DIM); });
      if(id===TAB.ADMIN) extras.push(D.Text(this.top,tx,60,200,16,Caps('ADMIN_ONLY'),13,C.DIM2));
      this.tabUI.push({id,t,badge,bt,hit,x:tx,w:tw,extras});
      x=tx+tw+30+gap;
    });
    this.badges();
  },
  showTabs(v){
    this.tabUI.forEach(u=>{u.t.style.display=u.hit.style.display=v?'':'none';u.extras.forEach(e=>e.style.display=v?'':'none');if(!v)u.badge.style.display='none';});
    this.tabLine.style.display=v&&this.tab>=0?'':'none';
    if(v) this.badges();
  },
  badges(){
    const pm=Object.values(S.unread.pm).reduce((a,b)=>a+b,0);
    this.tabUI.forEach(u=>{
      const n=u.id===TAB.CHAT?S.unread.public+S.unread.faction+pm:u.id===TAB.NEWS?S.unread.news:0;
      u.badge.style.display=n>0?'':'none'; u.bt.textContent=n>99?'99':n;
    });
  },
  buildFoot(c){
    const w=LOOK.SCR_W, f=D.Frame(c,0,LOOK.SCR_H-LOOK.FOOT_H,w,LOOK.FOOT_H,'Foot');
    D.Line(f,0,0,w,1,C.LINE); D.Icon(f,40,11,16,'news',C.DIM);
    D.Text(f,66,0,170,LOOK.FOOT_H,Caps('ZONE_NEWS'),14,C.ORANGE);
    this.newsLine=D.Text(f,236,0,760,LOOK.FOOT_H,'',14,C.DIM);
    D.Text(f,1000,0,416,LOOK.FOOT_H,T('HINTS'),13,C.DIM2,'r');
    this.updateNews();
  },
  updateNews(){
    if(!this.newsLine) return;
    const n=S.news.length;
    if(!n){ this.newsLine.textContent=T('NEWS_NONE'); return; }
    if(this.newsIdx>=n) this.newsIdx=0;
    const it=S.news[this.newsIdx];
    this.newsLine.textContent=Fit((n>1?(this.newsIdx+1)+'/'+n+'  ':'')+it.Title+' — '+it.Text,756,14);
  },

  // --- состояния -----------------------------------------------------------------------------------------------------
  boot(){
    this.bootEl=D.Frame(this.body,0,0,LOOK.SCR_W,LOOK.BODY_H,'Boot');
    const x=470, full=['BOOT_1','BOOT_2','BOOT_3','BOOT_4'];
    this.bootTexts=[T('BOOT_1')+' 1.1.0',T('BOOT_2')+' '+this.modList(),T('BOOT_3')+' '+S.bat+'%',T('BOOT_4')];
    D.Text(this.bootEl,x,150,600,40,Caps('PRODUCT'),30,C.TEXT);
    D.Line(this.bootEl,x,196,520,1,C.LINE2);
    this.bootLines=full.map((_,i)=>D.Text(this.bootEl,x,214+i*30,700,26,'',16,C.DIM));
    D.Line(this.bootEl,x,350,520,4,C.LINE);
    this.bootBar=D.Line(this.bootEl,x,350,1,4,C.ORANGE);
    this.bootT=0; this.headTitle.style.display='';
  },
  modList(){
    const s=[]; if(S.mods.gps) s.push('GPS'); if(S.mods.map) s.push(T('MOD_MAP')); if(S.mods.chat) s.push(T('MOD_CHAT')); if(S.mods.gp) s.push(T('MOD_GP'));
    return s.length?s.join(' '):'—';
  },
  bootTick(dt){
    this.bootT+=dt;
    this.bootLines.forEach((l,i)=>{const k=(this.bootT-(.45+i*.32))/.28;if(k<=0)return;const f=this.bootTexts[i];l.textContent=f.slice(0,Math.round(f.length*Math.min(1,k)));});
    this.bootBar.style.width=Math.max(1,520*Math.min(1,Math.max(0,(this.bootT-.4)/1.4)))+'px';
    if(this.bootT>=1.95){ this.bootT=-1; this.bootEl.remove(); S.screen=S.loggedIn?'main':'auth'; this.enter(S.screen); Maket.sync(); }
  },
  enter(st){
    this.clearPage();
    if(st==='main'){
      if(!this.tabUI.length) this.buildTabs();
      this.headTitle.style.display='none'; this.showTabs(true);
      this.select(S.adminMode&&this.tabIds.includes(TAB.ADMIN)?TAB.ADMIN:(this.lastTab!=null&&this.tabIds.includes(this.lastTab)?this.lastTab:TAB.PROFILE),false);
    } else {
      this.tab=-1; if(this.tabUI.length) this.showTabs(false);
      this.headTitle.style.display='';
      this.mount(st==='auth'?Pages.auth:Pages.trophy);
    }
    this.adminFlag.textContent=S.adminMode?Caps('ADMIN_MODE'):'';
  },
  clearPage(){
    if(this.page&&this.page.onHide) this.page.onHide();
    if(this.pageRoot) this.pageRoot.remove();
    this.page=null; this.pageRoot=null;
  },
  mount(p){
    this.clearPage();
    this.pageRoot=D.Frame(this.body,0,0,LOOK.SCR_W,LOOK.BODY_H,'Page');
    this.page=p; p.build(this.pageRoot,LOOK.SCR_W,LOOK.BODY_H);
    Look.Reveal(this.pageRoot,0,10,260);
  },
  select(id,sound){
    if(id===this.tab||!this.tabIds.includes(id)) return;
    this.tab=id; this.lastTab=id;
    this.tabUI.forEach(u=>u.t.style.color=u.id===id?C.TEXT:(u.id===TAB.ADMIN?C.ADMIN:C.DIM));
    const u=this.tabUI.find(u=>u.id===id);
    this.tabLine.style.display=''; this.tabLine.style.left=u.x+'px'; this.tabLine.style.width=u.w+'px';
    this.mount(Pages[PAGE_OF[id]]);
    Maket.sync();
  },
  reload(){ if(this.device&&this.page) this.mount(this.page); this.status(); this.badges(); },

  // --- всплывающее окно ввода (INT_PDA_Prompt) ------------------------------------------------------------------------
  openPrompt(title,first,second,password,value,done){
    const w=560, h=second?310:230, W=LOOK.SCR_W, H=LOOK.SCR_H;
    const m=this.prompt=D.Frame(this.screenEl,0,0,W,H,'Prompt'); m.style.zIndex=70;
    D.Line(m,0,0,W,H,'rgba(0,0,0,.67)');
    const b=D.Frame(m,(W-w)/2,(H-h)/2,w,h,'PromptBox'); b.style.boxShadow='0 16px 44px rgba(0,0,0,.63)';
    D.Box(b,0,0,w,h,8,C.PANEL,C.LINE2); D.Line(b,0,0,w,3,C.ORANGE);
    D.Text(b,28,22,w-56,28,CapsT(title),20,C.TEXT);
    let fy=70; D.Text(b,28,fy,w-56,18,CapsT(first),13,C.DIM);
    const a=password?UI.Pass(b,28,fy+22,w-56,48):UI.Input(b,28,fy+22,w-56,48,'',60,null,value);
    let c=null;
    if(second){ fy+=80; D.Text(b,28,fy,w-56,18,CapsT(second),13,C.DIM); c=password?UI.Pass(b,28,fy+22,w-56,48):UI.Input(b,28,fy+22,w-56,48,'',60); }
    const finish=ok=>{const va=a.input.value,vb=c?c.input.value:'';this.closePrompt();if(done)done(ok,va,vb);};
    UI.Button(b,w-28-300,h-64,140,44,T('CANCEL'),'secondary',null,()=>finish(false));
    UI.Button(b,w-28-150,h-64,150,44,T('OK'),'primary','check',()=>finish(true));
    a.input.onkeydown=e=>{if(e.key==='Enter'){if(c&&!c.input.value)c.input.focus();else finish(true);}};
    if(c) c.input.onkeydown=e=>{if(e.key==='Enter')finish(true);};
    this.promptFinish=finish; a.input.focus();
    Look.Reveal(b,0,12,220);
  },
  closePrompt(){ if(this.prompt) this.prompt.remove(); this.prompt=null; },

  // --- кадр и клавиши ------------------------------------------------------------------------------------------------
  tick(dt){
    if(!this.device){ if(S.screen==='hud') Hud.tick(dt); return; }
    if(this.bootT>=0) this.bootTick(dt);
    this.statusT+=dt; if(this.statusT>=1){this.statusT=0;this.status();}
    this.newsT+=dt; if(this.newsT>=8){this.newsT=0;this.newsIdx++;this.updateNews();}
    if(this.page&&this.page.tick) this.page.tick(dt);
  },
  status(){
    if(!this.clock) return;
    const d=new Date(); this.clock.textContent=String(d.getHours()).padStart(2,'0')+':'+String(d.getMinutes()).padStart(2,'0');
    this.batText.textContent=S.bat+'%'; this.batIcon.style.color=S.bat<10?C.ENEMY:C.TEXT;
    this.chips.forEach(c=>{const on=S.mods[c.key]||S.adminMode;c.t.style.color=on?C.ORANGE:C.DIM2;c.box.style.borderColor=on?C.ORANGE:C.LINE;});
  },
  hasMod(k){ return S.adminMode||S.mods[k]; },
  key(e){
    const inField=document.activeElement&&document.activeElement.tagName==='INPUT';
    if(e.code==='KeyP'&&!inField&&!this.device&&S.screen==='off'){ S.screen='main'; this.open(); Maket.sync(); return; }
    if(!this.device) return;
    if(e.key==='Escape'){
      e.preventDefault();
      const dd=document.querySelector('.ddp'); if(dd){dd.remove();return;}
      if(this.prompt){this.promptFinish(false);return;}
      if(inField){document.activeElement.blur();return;}
      const ask=this.stage.querySelector('[data-w=Ask]'); if(ask){ask.remove();return;}
      if(this.page&&this.page.esc&&this.page.esc()) return;
      this.powerOff();
    } else if(e.key==='Tab'){
      e.preventDefault();
      if(this.prompt||inField||!this.tabIds.length||S.screen!=='main') return;
      this.select(this.tabIds[(this.tabIds.indexOf(this.tab)+1)%this.tabIds.length]);
    }
  },
};
