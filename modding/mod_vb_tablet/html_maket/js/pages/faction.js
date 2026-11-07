// «Группировка»: порт INT_PDA_PageFaction.c. Шапка 168, ниже три колонки: отношения, состав, инструменты.
Pages.faction = {
  COL_Y:184, COL_H:474, REL_X:24, REL_W:400, MEM_X:444, MEM_W:540, RIGHT_X:1004, RIGHT_W:428, IN_W:392, BTN_H:40,
  memFirst:0, relFirst:0, sel:null,

  build(r,W,H){
    this.r=r; this.W=W; this.H=H;
    if(!Shell.hasMod('gp')){ Look.Locked(r,W,H,'flag','FAC_LOCK_TITLE','FAC_LOCK_TEXT'); return; }
    if(S.role===0){ this.none(r); return; }
    this.f=Data.myFaction(); this.mem=Data.members(); this.memFirst=0; this.relFirst=0; this.sel=null;
    this.lay=S.role===3||S.adminMode?0:S.role===2?1:2;
    this.head(r); this.relations(r); this.members(r); this.rightCol(r);
  },
  none(r){
    const W=this.W, H=this.H;
    if(!S.invite){
      const p=D.Frame(r,0,H*.5-90,W,160,'None');
      D.Icon(p,W*.5-32,0,64,'flag',C.DIM2);
      D.Text(p,0,84,W,30,Caps('FAC_NONE_TITLE'),22,C.TEXT,'c');
      D.Text(p,0,124,W,24,T('FAC_NONE_TEXT'),16,C.DIM,'c');
      return;
    }
    const f=Data.faction('gp005'), bw=680, bh=414, b=D.Frame(r,(W-bw)/2,130,bw,bh,'InvBox');
    b.style.boxShadow='0 16px 44px rgba(0,0,0,.63)';
    D.Box(b,0,0,bw,bh,8,C.PANEL,C.LINE2); D.Line(b,0,0,bw,3,C.ORANGE);
    D.Icon(b,28,20,18,'invite',C.ORANGE); D.Text(b,54,20,600,20,Caps('FAC_INV_IN_TITLE'),13,C.ORANGE);
    Look.FactionCard(b,28,56,210,f.Emblem,f.Name);
    D.Text(b,268,58,384,52,Fit(f.Name,384,40),40,C.TEXT);
    D.Text(b,268,124,384,18,Caps('FAC_INV_FROM'),13,C.DIM);
    D.Text(b,268,144,384,36,'Kochevnikov',28,C.OWN);
    D.MText(b,268,196,384,90,T('FAC_INV_TEXT'),15,C.DIM);
    UI.Button(b,268,344,180,50,T('FAC_DECLINE'),'secondary','close',()=>{S.invite=false;Toast(T('FAC_DECLINED'),'info');Shell.reload();Maket.sync();});
    UI.Button(b,462,344,190,50,T('FAC_ACCEPT'),'primary','check',()=>{S.invite=false;S.factionId='gp005';S.role=1;S.rank='Отмычка';Toast(T('FAC_JOINED'),'ok');Shell.reload();Maket.sync();});
  },
  head(r){
    const f=this.f, W=this.W, h=D.Frame(r,0,0,W,170,'Head');
    Look.FactionCard(h,24,8,111,f.Emblem,f.Name);
    D.Text(h,156,2,740,110,Fit(CapsT(f.Name),736,96),96,C.TEXT);
    D.Text(h,160,112,736,32,Fit(f.Motto,736,22),22,C.DIM);
    const i=D.Frame(h,916,0,516,168,'HeadInfo');
    D.Icon(i,0,21,22,'pin',C.ORANGE);
    D.Text(i,34,16,482,32,Fit(T('FAC_BASE')+': '+(f.Base||T('FAC_NO_BASE')),482,19),19,C.TEXT);
    D.Icon(i,0,66,24,'users',C.ORANGE);
    const on=this.mem.filter(m=>m.Online).length;
    D.Text(i,34,54,482,48,on+' / '+this.mem.length+' '+T('FAC_ONLINE'),32,C.TEXT);
    const gc=Caps('FAC_CHIP_GP'), gw=TextW(gc,13)+24;
    D.Pill(i,0,112,gw,26,'#2A1D10','rgba(255,122,26,.55)'); D.Text(i,0,112,gw,26,gc,13,C.ORANGE,'c');
    const mc=(T('FAC_YOU')+' — '+T(Look.RoleKey(S.role))).toUpperCase(), mw=TextW(mc,13)+24;
    D.Pill(i,gw+10,112,mw,26,'#1D241F','rgba(111,208,160,.55)'); D.Text(i,gw+10,112,mw,26,mc,13,C.ALLY,'c');
    D.Line(h,24,168,W-48,1,C.LINE);
  },
  pager(p,w,onUp,onDown){
    const mk=(x,ic,fn)=>{D.Box(p,x,0,24,22,4,C.PANEL2,C.LINE2);D.Icon(p,x+5,4,14,ic,C.DIM);D.Hit(p,x,0,24,22,fn);};
    mk(w-54,'chevron_up',onUp); mk(w-24,'chevron_down',onDown);
  },
  relations(r){
    const col=this.relCol=D.Frame(r,this.REL_X,this.COL_Y,this.REL_W,this.COL_H,'Rel');
    D.Text(col,0,2,190,18,Caps('FAC_RELATIONS'),13,C.DIM);
    this.relPage=D.Text(col,196,2,142,18,'',13,C.DIM2,'r');
    const all=this.rel=Data.relationsOf(this.f.Id);
    this.pager(col,this.REL_W,()=>{this.relFirst=Math.max(0,this.relFirst-1);this.relPaint();},()=>{this.relFirst=Math.min(Math.max(0,all.length-5),this.relFirst+1);this.relPaint();});
    this.relBody=D.Frame(col,0,0,this.REL_W,this.COL_H,'RelBody');
    this.relPaint();
  },
  relPaint(){
    const b=this.relBody; b.innerHTML=''; const all=this.rel;
    this.relPage.textContent=(this.relFirst+1)+'–'+Math.min(all.length,this.relFirst+5)+' '+T('FAC_OF')+' '+all.length;
    all.slice(this.relFirst,this.relFirst+5).forEach((e,i)=>{
      const y=24+i*92, c=e.Rel===1?C.ALLY:e.Rel===2?C.ENEMY:C.NEUTRAL, k=e.Rel===1?'FAC_REL_ALLY':e.Rel===2?'FAC_REL_ENEMY':'FAC_REL_NEUTRAL';
      const row=D.Frame(b,0,y,this.REL_W,82,'RelRow');
      D.Box(row,0,0,this.REL_W,82,6,C.PANEL,C.LINE); D.Line(row,0,3,4,76,c);
      D.Emblem(row,18,15,52,e.f.Emblem);
      D.Text(row,84,8,200,28,Fit(e.f.Name,200,20),20,C.TEXT);
      D.Text(row,84,36,200,20,Fit(e.Reason||T('FAC_REASON_NONE'),200,14),14,C.DIM);
      D.Pill(row,290,29,96,24,null,c); D.Text(row,290,29,96,24,Caps(k),12,c,'c');
    });
  },
  members(r){
    const c=this.center=D.Frame(r,this.MEM_X,this.COL_Y,this.MEM_W,this.COL_H,'Members');
    D.Text(c,0,2,420,18,Caps('FAC_MEMBERS')+' · '+this.mem.length+' '+T('FAC_TOTAL'),13,C.DIM);
    this.pager(c,this.MEM_W,()=>{this.memFirst=Math.max(0,this.memFirst-1);this.memPaint();},()=>{this.memFirst=Math.min(Math.max(0,this.mem.length-8),this.memFirst+1);this.memPaint();});
    D.Box(c,0,24,this.MEM_W,this.COL_H-24,6,C.PANEL,C.LINE);
    D.Text(c,17,32,180,20,Caps('FAC_COL_CALLSIGN'),13,C.DIM); D.Text(c,207,32,150,20,Caps('FAC_COL_RANK'),13,C.DIM);
    D.Text(c,367,32,120,20,Caps('FAC_COL_ROLE'),13,C.DIM); D.Text(c,this.MEM_W-80,32,60,20,Caps('FAC_COL_NET'),13,C.DIM,'r');
    D.Line(c,1,60,this.MEM_W-2,1,C.LINE);
    this.memBody=D.Frame(c,0,0,this.MEM_W,this.COL_H,'MemBody');
    this.memPaint();
  },
  memPaint(){
    const b=this.memBody; b.innerHTML=''; const rw=this.MEM_W-2, rh=49;
    this.mem.slice(this.memFirst,this.memFirst+8).forEach((m,i)=>{
      const sel=this.sel===m, row=D.Frame(b,1,61+i*50,rw,50,'Row'), box=D.Box(row,0,0,rw,rh,4,m.me?'#22160C':sel?C.PANEL2:C.PANEL);
      D.Line(row,0,rh,rw,1,'#1E1E22');
      if(sel||m.me) D.Line(row,0,0,3,rh,C.ORANGE);
      const nw=Math.min(180,TextW(m.Callsign,18)+4); D.Text(row,16,0,180,rh,Fit(m.Callsign,178,18),18,C.TEXT);
      if(m.me){const yc=Caps('FAC_YOU'),yw=TextW(yc,11)+12;D.Box(row,16+nw+6,15,yw,19,4,null,C.ORANGE);D.Text(row,16+nw+6,15,yw,19,yc,11,C.ORANGE,'c');}
      D.Text(row,206,0,150,rh,Fit(m.Rank,150,17),17,C.DIM);
      D.Text(row,366,0,120,rh,T(Look.RoleKey(m.Role)),17,m.Role===3?C.ORANGE:C.DIM);
      D.Circle(row,rw-30,20,10,m.Online?C.ALLY:C.DIM2);
      D.Hit(row,0,0,rw,rh,()=>{this.sel=m;this.memPaint();this.selPaint();},on=>{if(!sel&&!m.me)box.style.background=on?C.PANEL2:C.PANEL;});
    });
  },
  rightCol(r){
    const invite=this.lay!==2, ranks=this.lay===0, invH=ranks?240:288, actH=ranks?222:174, X=this.RIGHT_X, W=this.RIGHT_W, IW=this.IN_W;
    if(invite){
      const f=D.Frame(r,X,this.COL_Y,W,invH,'Invite'); D.Box(f,0,0,W,invH,6,C.PANEL,C.LINE);
      D.Text(f,18,14,IW,18,Caps('FAC_INVITE_TITLE'),13,C.ORANGE);
      const ibw=138, fw=IW-ibw-10, inp=UI.Input(f,18,40,fw,46,T('FAC_STEAM_PH'),17);
      UI.Button(f,18+fw+10,40,ibw,46,T('FAC_INVITE_BTN'),'primary','plus',()=>{
        if(!/^7656\d{13}$/.test(inp.input.value)){Toast(T('FAC_BAD_STEAM'),'err');return;}
        Toast(T('FAC_INVITE_SENT'),'ok'); inp.input.value='';
      },14);
      D.Text(f,18,92,IW,16,T('FAC_STEAM_HINT'),12,C.DIM2);
      D.Text(f,18,114,IW-40,18,Caps('FAC_NEARBY')+' · 5 '+T('FAC_M'),13,C.DIM);
      D.Icon(f,18+IW-18,114,18,'refresh',C.DIM);
      S.near.slice(0,ranks?2:3).forEach((n,i)=>{
        const row=D.Frame(f,18,140+i*44,IW,44,'Near'); D.Line(row,0,0,IW,1,'#222226');
        const av=D.Frame(row,0,7,30,30,'Av'); D.Box(av,0,0,30,30,4,C.PANEL2,C.LINE2); D.Icon(av,6,6,18,'user',C.DIM);
        D.Text(row,42,0,150,44,n.name,17,C.TEXT); D.Text(row,200,0,60,44,n.dist+' '+T('FAC_M'),14,C.DIM);
        UI.Button(row,IW-122,7,122,30,T('FAC_INVITE_BTN'),'primary','plus',()=>Toast(T('FAC_INVITE_SENT'),'ok'),13);
      });
    } else {
      const f=D.Frame(r,X,this.COL_Y,W,288,'Me'); D.Box(f,0,0,W,288,6,C.PANEL,C.LINE);
      Look.Label(f,18,16,IW,'FAC_MY_RANK');
      D.Text(f,18,38,IW,44,S.rank||T('FAC_NO_RANK'),32,C.TEXT);
      D.Text(f,18,84,IW,20,T(Look.RoleKey(S.role)),14,C.ORANGE);
      D.Line(f,18,116,IW,1,C.LINE);
      Look.Label(f,18,130,IW,'FAC_COL_JOINED');
      D.Text(f,18,150,IW,34,S.me.Joined,24,C.TEXT);
      D.Line(f,18,196,IW,1,C.LINE);
      D.Icon(f,18,214,18,'sos',C.SOS_RED); D.Text(f,46,210,IW-28,26,Fit(T('FAC_SOS_HINT'),IW-28,14),14,C.DIM);
      const note=T('FAC_MEMBER_NOTE'), cut=note.lastIndexOf(' ',Math.floor(note.length*.55));
      D.Text(f,18,242,IW,18,note.slice(0,cut),13,C.DIM2); D.Text(f,18,260,IW,18,note.slice(cut+1),13,C.DIM2);
    }
    const a=this.act=D.Frame(r,X,this.COL_Y+invH+12,W,actH,'Actions'); D.Box(a,0,0,W,actH,6,C.PANEL,C.LINE);
    D.Text(a,18,14,70,18,Caps('FAC_SEL'),13,C.DIM);
    this.selName=D.Text(a,80,11,300,24,'',16,C.ORANGE);
    let y=40; const half=(IW-10)/2;
    if(ranks){
      const giveW=150, dropW=IW-giveW-10;
      this.drop=UI.Dropdown(a,18,40,dropW,this.f.Ranks,0,this.BTN_H);
      this.tool(a,18+IW-giveW,40,giveW,'rank',T('FAC_GIVE_RANK'),C.TEXT,()=>this.giveRank());
      y=88;
    }
    if(this.lay!==2){
      this.tool(a,18,y,half,'kick',T('FAC_BTN_KICK'),'#7A4A44',()=>this.kick());
      this.tool(a,18+half+10,y,half,'sos',T('FAC_BTN_SOS'),C.SOS_RED,()=>this.sos(),C.SOS_BORDER);
    } else this.tool(a,18,y,IW,'sos',T('FAC_BTN_SOS'),C.SOS_RED,()=>this.sos(),C.SOS_BORDER);
    this.tool(a,18,y+48,IW,'chat',T('FAC_BTN_CHAT'),C.TEXT,()=>{Pages.chat.sub=1;Shell.select(TAB.CHAT);});
    const lw=TextW(T('FAC_BTN_LEAVE'),14), lf=D.Frame(a,18,y+98,lw+30,22,'Leave');
    const li=D.Icon(lf,0,3,16,'logout',C.DIM), lt=D.Text(lf,24,0,lw+6,22,T('FAC_BTN_LEAVE'),14,C.DIM);
    D.Hit(lf,0,0,lw+30,22,()=>this.leave(),on=>li.style.color=lt.style.color=on?C.TEXT:C.DIM);
    this.selPaint();
  },
  tool(p,x,y,w,ic,text,col,fn,border){
    const b=D.Frame(p,x,y,w,this.BTN_H,'Btn'), box=D.Box(b,0,0,w,this.BTN_H,4,C.PANEL,border||'#3C3C42'), tw=TextW(text,14), sx=(w-18-10-tw)/2;
    D.Icon(b,sx,11,18,ic,col); D.Text(b,sx+28,0,tw+6,this.BTN_H,text,14,col);
    D.Hit(b,0,0,w,this.BTN_H,fn,on=>box.style.background=on?C.PANEL2:C.PANEL); return b;
  },
  selPaint(){ this.selName.textContent=this.sel?this.sel.Callsign:'—'; },
  need(){ if(!this.sel||this.sel.me){Toast(T('FAC_PICK'),'warn');return false;} return true; },
  giveRank(){ if(!this.need()) return; this.sel.Rank=this.f.Ranks[this.drop.sel]; Toast(T('FAC_RANK_SET'),'ok'); this.memPaint(); },
  kick(){
    if(!this.need()) return; const m=this.sel;
    UI.Ask(this.r,{title:T('FAC_KICK_Q'),value:m.Callsign,text:T('FAC_KICK_TEXT'),danger:true,ch:244,onYes:()=>{this.f.Members=this.f.Members.filter(x=>x.Callsign!==m.Callsign);this.mem=Data.members();this.sel=null;Toast(T('FAC_KICKED'),'ok');this.memPaint();this.selPaint();}});
  },
  sos(){
    if(!Shell.hasMod('gps')){Toast(T('FAC_SOS_NEED_GPS'),'warn');return;}
    const l=SosClock.left(); if(l>0){Toast(T('FAC_SOS_WAIT'),'warn');return;}
    UI.Ask(this.r,{title:T('FAC_SOS_Q'),text:T('FAC_SOS_TEXT'),danger:true,ch:220,yes:T('FAC_BTN_SOS'),yesIcon:'sos',onYes:()=>{SosClock.start();Toast(T('FAC_SOS_SENT'),'ok');}});
  },
  leave(){
    UI.Ask(this.r,{title:T('FAC_LEAVE_Q'),value:this.f.Name,text:S.role===3?T('FAC_LEAVE_LEADER'):T('FAC_LEAVE_TEXT'),danger:true,ch:244,onYes:()=>{S.role=0;S.rank='';Toast(T('FAC_LEFT'),'ok');Shell.reload();Maket.sync();}});
  },
};
