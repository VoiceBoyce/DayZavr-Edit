// «Связь»: порт INT_PDA_PageChat.c. Лента прокручивается целыми сообщениями, новые снизу.
Pages.chat = {
  COL_X:36, COL_W:340, RIGHT:1420, FEED_X:410, FEED_W:1010, FEED_Y:88, FEED_BOTTOM:588, BAR_Y:598, BAR_H:64,
  TEXT_X:74, HEAD_H:26, ROW_H:23, ROW_GAP:12, CARD_Y:30, CARD_PITCH:84, CARD_H:74,
  sub:0, open:null, scroll:0, reply:null, anon:false, asGp:false,

  build(r,W,H){
    this.r=r; this.W=W; this.H=H; this.ctx=null; this.reply=null;
    if(!Shell.hasMod('chat')){ Look.Locked(r,W,H,'chat','CHAT_LOCKED','CHAT_LOCKED_TEXT'); return; }
    this.subTabs(r); this.left(r); this.right(r); this.paint();
  },
  subTabs(r){
    let x=this.COL_X; this.subs=[];
    ['CHAT_SUB_PUBLIC','CHAT_SUB_FACTION','CHAT_SUB_PRIVATE'].forEach((k,i)=>{
      const cap=Caps(k), tw=TextW(cap,20)+4, t=D.Text(r,x,16,tw+40,52,cap,20,C.DIM);
      const badge=D.Frame(r,x+tw+10,31,30,22,'Badge'); D.Pill(badge,0,0,30,22,C.ORANGE); const bt=D.Text(badge,0,0,30,22,'',14,C.ON_ORANGE,'c');
      D.Hit(r,x-8,16,tw+56,52,()=>this.setSub(i),on=>{if(i!==this.sub)t.style.color=on?C.TEXT:C.DIM;});
      this.subs.push({t,x,w:tw,badge,bt}); x+=tw+56+22;
    });
    this.subLine=D.Line(r,this.COL_X,65,10,3,C.ORANGE); this.subLine.classList.add('tabline');
    D.Line(r,this.COL_X,68,this.RIGHT-this.COL_X,1,C.LINE);
    const rcap=Caps('CHAT_REFRESH'), rw=TextW(rcap,18), rt=D.Text(r,this.RIGHT-300,16,300,52,rcap,18,C.DIM,'r'), ri=D.Icon(r,this.RIGHT-rw-30,31,22,'refresh',C.DIM);
    D.Hit(r,this.RIGHT-rw-38,16,rw+42,52,()=>{this.scroll=0;this.paint();Toast(T('MAP_UPDATED'),'ok');},on=>{rt.style.color=ri.style.color=on?C.TEXT:C.DIM;});
  },
  left(r){
    const L=this.leftEl=D.Frame(r,this.COL_X,this.FEED_Y,this.COL_W,574,'Left');
    this.cards=D.Frame(L,0,0,this.COL_W,500,'Cards');
    this.legend=D.Frame(L,0,290,this.COL_W,70,'Legend');
    D.Line(this.legend,0,0,this.COL_W,1,C.LINE);
    D.Text(this.legend,0,12,this.COL_W,20,Caps('CHAT_LEGEND'),13,C.DIM);
    let x=0; [['CHAT_REL_OWN',C.OWN],['CHAT_REL_ALLY',C.ALLY],['CHAT_REL_NEUTRAL',C.NEUTRAL],['CHAT_REL_ENEMY',C.ENEMY]].forEach(([k,col],i)=>{
      if(i){D.Text(this.legend,x,38,16,22,'·',15,C.DIM2,'c');x+=16;}
      const w=TextW(T(k),15)+2; D.Text(this.legend,x,38,w+10,22,T(k),15,col); x+=w;
    });
    const air=this.air=Look.Panel(L,0,382,this.COL_W,120,'CHAT_AIR');
    D.Text(air,18,40,150,22,T('CHAT_AIR_NET'),15,C.DIM); this.airNet=D.Text(air,150,40,this.COL_W-168,22,T('CHAT_AIR_ON'),15,C.TEXT,'r');
    D.Text(air,18,64,120,22,T('CHAT_AIR_CHANNEL'),15,C.DIM); this.airCh=D.Text(air,100,64,this.COL_W-118,22,'',15,C.ORANGE,'r');
    D.Text(air,18,88,150,22,T('CHAT_AIR_COUNT'),15,C.DIM); this.airN=D.Text(air,150,88,this.COL_W-168,22,'',15,C.TEXT,'r');
    const nb=D.Frame(L,0,518,this.COL_W,48,'New'), box=D.Box(nb,0,0,this.COL_W,48,4,C.PANEL,C.LINE2);
    D.Icon(nb,18,14,20,'plus',C.ORANGE); D.Text(nb,50,0,this.COL_W-60,48,Caps('CHAT_NEW_DIALOG'),15,C.TEXT);
    D.Hit(L,0,518,this.COL_W,48,()=>this.newDialog(),on=>box.style.background=on?C.PANEL2:C.PANEL);
  },
  right(r){
    const R=this.rightEl=D.Frame(r,this.FEED_X,0,this.FEED_W,this.H,'Right');
    this.head=D.Frame(R,0,this.FEED_Y-4,this.FEED_W,36,'DialogHead');
    D.Icon(this.head,0,7,18,'mail',C.ORANGE); this.headName=D.Text(this.head,28,0,700,30,'',20,C.TEXT); D.Line(this.head,0,33,this.FEED_W,1,C.LINE);
    this.feed=D.Clip(R,0,this.FEED_Y,this.FEED_W,this.FEED_BOTTOM-this.FEED_Y,'Feed');
    this.feed.onwheel=e=>{e.preventDefault();this.scroll=Math.max(0,this.scroll+(e.deltaY<0?1:-1));this.paint();};
    this.empty=D.Text(R,0,300,this.FEED_W,30,'',17,C.DIM2,'c');
    this.track=D.Line(R,this.FEED_W+16,this.FEED_Y,2,this.FEED_BOTTOM-this.FEED_Y,C.LINE);
    this.thumb=D.Line(R,this.FEED_W+16,this.FEED_Y,2,40,C.ORANGE);
    this.replyBar=D.Frame(R,0,this.BAR_Y-40,this.FEED_W,34,'ReplyBar');
    D.Box(this.replyBar,0,0,this.FEED_W,34,4,C.PANEL,C.LINE); D.Line(this.replyBar,0,0,3,34,C.ORANGE); D.Icon(this.replyBar,16,8,18,'chat',C.ORANGE);
    this.replyT=D.Text(this.replyBar,46,0,this.FEED_W-110,34,'',15,C.TEXT);
    D.Icon(this.replyBar,this.FEED_W-32,9,16,'close',C.DIM); D.Hit(this.replyBar,this.FEED_W-46,0,46,34,()=>{this.reply=null;this.paint();});
    const bar=this.bar=D.Frame(R,0,this.BAR_Y,this.FEED_W,this.BAR_H,'Bar');
    D.Box(bar,0,0,this.FEED_W,this.BAR_H,6,'#101012',C.LINE);
    this.inp=UI.Input(bar,10,8,430,48,T('CHAT_PLACEHOLDER'),S.max||200);
    this.inp.input.oninput=()=>this.count();
    this.inp.input.onkeydown=e=>{if(e.key==='Enter')this.send();};
    this.counter=D.Text(bar,446,0,84,this.BAR_H,'',15,C.DIM,'r');
    this.sep=D.Line(bar,542,14,1,36,C.LINE);
    this.tAnon=UI.Toggle(bar,556,16,146,T('CHAT_ANON'),this.anon,v=>{this.anon=v;});
    this.tGp=UI.Toggle(bar,716,16,178,T('CHAT_AS_GP'),this.asGp,v=>{this.asGp=v;});
    const sb=D.Frame(bar,908,9,92,46,'Send'), box=D.Box(sb,0,0,92,46,4,C.ORANGE); D.Icon(sb,33,10,26,'send',C.ON_ORANGE);
    D.Hit(bar,908,9,92,46,()=>this.send(),on=>box.style.background=on?'#FF9648':C.ORANGE);
    this.count();
  },
  count(){ this.counter.textContent=this.inp.input.value.length+' / 200'; },
  setSub(i){
    if(i===2&&!this.open&&S.dialogs.length) this.open=S.dialogs[0].name;
    this.sub=i; this.scroll=0; this.reply=null; this.paint();
  },
  lines(){
    if(this.sub===0) return S.Public;
    if(this.sub===1) return S.role>0&&Shell.hasMod('gp')?S.Faction:[];
    const d=S.dialogs.find(x=>x.name===this.open); if(!d) return [];
    return d.lines.map(l=>({Channel:2,Flags:0,Time:l.Time,Author:l.me?S.me.Callsign:d.name,Avatar:l.me?S.me.Avatar:d.Avatar,Text:l.Text,me:l.me,FactionId:l.me?S.factionId:''}));
  },
  paint(){
    if(!this.feed) return;
    const u=S.unread, pmN=Object.values(u.pm).reduce((a,b)=>a+b,0), i=this.sub, pr=i===2;
    if(i===0) u.public=0; if(i===1) u.faction=0;
    if(pr&&this.open) delete u.pm[this.open];
    [u.public,u.faction,pmN].forEach((n,k)=>{this.subs[k].badge.style.display=n>0&&k!==i?'':'none';this.subs[k].bt.textContent=n;});
    Shell.badges();
    this.subs.forEach((s,k)=>s.t.style.color=k===i?C.TEXT:C.DIM);
    const sl=this.subs[i]; this.subLine.style.left=sl.x+'px'; this.subLine.style.width=sl.w+'px';
    this.cardsPaint();
    this.legend.style.display=this.air.style.display=pr?'none':'';
    this.airCh.textContent=T(['CHAT_SUB_PUBLIC','CHAT_SUB_FACTION','CHAT_SUB_PRIVATE'][i]);
    this.head.style.display=pr&&this.open?'':'none';
    if(this.open) this.headName.textContent=this.open;
    // поле шире в личных: без «Анонимно» и «От имени ГП»
    this.inp.style.width=(pr?790:430)+'px'; this.counter.style.left=(pr?806:446)+'px';
    this.sep.style.display=this.tAnon.style.display=pr?'none':''; this.tGp.style.display=(i===1&&S.role>=2)?'':'none';
    this.replyBar.style.display=this.reply?'':'none';
    if(this.reply) this.replyT.textContent=T('CHAT_REPLYING').replace('%1',this.reply.Author||T('CHAT_ANON_AUTHOR'));
    const ls=this.lines();
    this.airN.textContent=ls.length;
    this.bar.style.display=(i===1&&!(S.role>0&&Shell.hasMod('gp')))||(pr&&!this.open)?'none':'';
    this.empty.textContent=i===1&&!Shell.hasMod('gp')?T('CHAT_NEED_GP'):i===1&&S.role===0?T('CHAT_NEED_FACTION'):pr&&!this.open?T('CHAT_PICK'):T('CHAT_EMPTY');
    this.empty.style.display=ls.length?'none':'';
    this.feedPaint(ls);
  },
  cardsPaint(){
    const c=this.cards; c.innerHTML='';
    const pr=this.sub===2, max=pr?5:3;
    D.Text(c,0,0,this.COL_W,20,Caps('CHAT_DIALOGS'),13,C.DIM);
    if(!S.dialogs.length) D.Text(c,0,this.CARD_Y+8,this.COL_W,24,T('CHAT_NO_DIALOGS'),15,C.DIM2);
    S.dialogs.slice(0,max).forEach((d,i)=>{
      const y=this.CARD_Y+i*this.CARD_PITCH, sel=pr&&d.name===this.open, card=D.Frame(c,0,y,this.COL_W,this.CARD_H,'Card');
      const box=D.Box(card,0,0,this.COL_W,this.CARD_H,6,sel?'#1A130E':C.PANEL,sel?'#5A3418':C.LINE);
      D.Avatar(card,12,12,50,d.Avatar);
      D.Text(card,74,10,180,26,Fit(d.name,180,18),18,C.TEXT);
      const last=d.lines[d.lines.length-1];
      D.Text(card,this.COL_W-96,12,84,18,last.Time.slice(6),13,C.DIM,'r');
      D.Text(card,74,40,this.COL_W-126,22,Fit(last.Text,this.COL_W-130,14),14,C.DIM);
      const n=S.unread.pm[d.name]||0;
      if(n){const b=D.Frame(card,this.COL_W-42,38,30,22,'B');D.Pill(b,0,0,30,22,C.ORANGE);D.Text(b,0,0,30,22,String(n),14,C.ON_ORANGE,'c');}
      const h=D.Hit(c,0,y,this.COL_W,this.CARD_H,()=>{this.open=d.name;this.sub=2;this.scroll=0;this.paint();},on=>{if(!sel)box.style.background=on?C.PANEL2:C.PANEL;});
      h.oncontextmenu=e=>{e.preventDefault();this.askDelete(d);};
    });
  },
  askDelete(d){
    UI.Ask(this.r,{title:T('CHAT_DEL_Q'),value:d.name,text:T('CHAT_DEL_TEXT'),danger:true,no:T('NO'),yes:T('YES'),yesIcon:'trash',ch:252,onYes:()=>{
      S.dialogs=S.dialogs.filter(x=>x!==d); delete S.unread.pm[d.name]; if(this.open===d.name)this.open=null; this.paint();
    }});
  },
  // Высота сообщения: шапка + строки текста (перенос считает браузер, в игре — скрипт по метрикам движка).
  feedPaint(ls){
    const f=this.feed; f.innerHTML=''; this.closeCtx();
    const h=this.FEED_BOTTOM-this.FEED_Y, n=ls.length, top=this.head.style.display===''?36:0;
    let bottom=h-(this.reply?40:0), shown=0;
    for(let i=n-1-this.scroll;i>=0&&bottom>top;i--){
      const row=this.row(f,ls[i]), rh=row.offsetHeight||row._h;
      bottom-=rh; row.style.top=bottom+'px'; shown++;
      bottom-=this.ROW_GAP;
      if(bottom<top){ row.remove(); shown--; }
    }
    const canUp=n>shown;
    this.track.style.display=this.thumb.style.display=canUp?'':'none';
    if(canUp){const trackH=h,th=Math.max(40,trackH*shown/n),maxS=n-shown,pos=(1-Math.min(1,this.scroll/maxS))*(trackH-th);this.thumb.style.height=th+'px';this.thumb.style.top=this.FEED_Y+pos+'px';}
  },
  row(f,l){
    const W=this.FEED_W, X=this.TEXT_X;
    const row=D.Frame(f,0,0,W,null,'Row');
    if(l.Channel===3){
      const t=D.MText(row,120,7,W-240,null,l.Text,15,C.DIM); t.style.textAlign='center'; t.style.position='relative'; t.style.left='120px'; t.style.top='0'; t.style.padding='4px 0';
      row.style.height=(t.offsetHeight||30)+'px'; row._h=30; return row;
    }
    const anon=l.Flags&1, gp=l.Flags&2;
    let ax=0;
    if(anon) Look.Anon(row,0,0,58);
    else if(gp) D.Emblem(row,0,0,58,l.FactionEmblem);
    else D.Avatar(row,0,0,58,l.Avatar);
    const name=anon?T('CHAT_ANON_AUTHOR'):gp?l.FactionName:l.Author;
    const col=anon?'#A8A49C':Look.RelColor(l.FactionId);
    const nw=TextW(name,19)+4; D.Text(row,X,0,nw,this.HEAD_H,name,19,col); let x=X+nw+10;
    if(!anon&&!gp&&l.FactionName){const w=TextW(l.FactionName,14)+4;D.Text(row,x,0,w,this.HEAD_H,l.FactionName,14,C.DIM);x+=w+10;}
    if(anon||gp){const tag=T(anon?'CHAT_TAG_ANON':'CHAT_TAG_FACTION'),tw=TextW(tag,12)+20;D.Pill(row,x,3,tw,20,anon?'#222226':'#2A1512');D.Text(row,x,3,tw,20,tag,12,anon?'#A8A49C':'#EC6A5C','c');}
    D.Text(row,W-180,0,180,this.HEAD_H,l.Time,14,C.DIM,'r');
    let ty=this.HEAD_H+2;
    if(l.ReplyId>0&&l.ReplyText){D.Line(row,X,this.HEAD_H+4,2,17,C.ORANGE);D.Text(row,X+10,this.HEAD_H+1,W-X-24,24,Fit((l.ReplyAuthor||T('CHAT_ANON_AUTHOR'))+': '+l.ReplyText,W-X-24,14),14,C.DIM);ty+=24;}
    const t=D.MText(row,X,ty,W-X-8,null,l.Text,17,C.TEXT); t.style.lineHeight=this.ROW_H+'px';
    const rows=Math.max(1,Math.ceil(TextW(l.Text,17)/(W-X-8)+.08)), rh=Math.max(58,ty+rows*this.ROW_H);
    row.style.height=rh+'px'; row._h=rh; row.style.position='absolute';
    row.oncontextmenu=e=>{e.preventDefault();this.openCtx(e,l);};
    return row;
  },
  openCtx(e,l){
    this.closeCtx();
    if(this.sub===2) return;
    const b=this.r.getBoundingClientRect(), k=b.width/this.W, x=(e.clientX-b.left)/k, y=(e.clientY-b.top)/k, w=250, items=[['CHAT_CTX_REPLY','chat',()=>{this.reply=l;this.paint();this.inp.input.focus();}]];
    if(l.Author&&!(l.Flags&1)&&l.Author!==S.me.Callsign) items.push(['CHAT_CTX_PM','mail',()=>{this.startDialog(l.Author,l.Avatar);}]);
    const m=this.ctx=D.Frame(this.r,x,y,w,items.length*40+8,'Ctx'); m.style.zIndex=30; m.style.boxShadow='0 12px 30px rgba(0,0,0,.6)';
    D.Box(m,0,0,w,items.length*40+8,6,C.PANEL2,C.LINE2);
    items.forEach(([k2,ic,fn],i)=>{const y2=4+i*40,bg=D.Box(m,4,y2,w-8,40,4,'transparent');D.Icon(m,16,y2+11,18,ic,C.ORANGE);D.Text(m,46,y2,w-56,40,T(k2),15,C.TEXT);D.Hit(m,4,y2,w-8,40,()=>{this.closeCtx();fn();},on=>bg.style.background=on?'#26262B':'transparent');});
  },
  closeCtx(){ if(this.ctx){this.ctx.remove();this.ctx=null;} },
  startDialog(name,av){
    let d=S.dialogs.find(x=>x.name===name);
    if(!d){d={name,Avatar:av||0,lines:[{me:true,Time:'',Text:'…'}]};d.lines=[];S.dialogs.unshift(d);d.lines.push({me:false,Time:this.now(),Text:'Диалог начат'});}
    this.open=name; this.sub=2; this.scroll=0; this.paint(); this.inp.input.focus();
  },
  newDialog(){
    Shell.openPrompt(T('CHAT_NEW_DIALOG'),T('CHAT_NEW_HINT'),'',false,'',(ok,a)=>{
      if(!ok||!a) return;
      if(a===S.me.Callsign){Toast(T('CHAT_SELF'),'err');return;}
      this.startDialog(a,(a.length*7)%73);
    });
  },
  now(){ const d=new Date(), p=v=>String(v).padStart(2,'0'); return p(d.getDate())+'.'+p(d.getMonth()+1)+' '+p(d.getHours())+':'+p(d.getMinutes()); },
  send(){
    const s=this.inp.input.value.trim(); if(!s) return;
    const f=Data.myFaction();
    if(this.sub===2){ S.dialogs.find(d=>d.name===this.open).lines.push({me:true,Time:this.now(),Text:s}); }
    else {
      const flags=(this.sub===0&&this.anon?1:0)|(this.sub===1&&this.asGp?2:0);
      const line={Channel:this.sub,Flags:flags,Time:this.now(),Author:S.me.Callsign,Avatar:S.me.Avatar,FactionId:f?f.Id:'',FactionName:f?f.Name:'',FactionEmblem:f?f.Emblem:1,Text:s,Id:Date.now()};
      if(this.reply){line.ReplyId=this.reply.Id||1;line.ReplyAuthor=this.reply.Author;line.ReplyText=this.reply.Text;}
      (this.sub===0?S.Public:S.Faction).push(line);
    }
    this.inp.input.value=''; this.reply=null; this.scroll=0; this.count(); this.paint();
  },
  esc(){ if(this.ctx){this.closeCtx();return true;} if(this.reply){this.reply=null;this.paint();return true;} return false; },
};
