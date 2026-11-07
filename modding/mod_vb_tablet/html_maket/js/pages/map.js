// «Карта»: порт INT_PDA_PageMap.c. В игре это MapWidget, здесь картинка-заглушка той же логики (перетаскивание ЛКМ,
// колесо вокруг курсора, ПКМ или двойной клик: новая метка). WORLD_M = сторона мира в метрах для заглушки.
const WORLD_M = 10240;
const KIND = [
  {icon:'base',color:'#FF7A1A',key:'MAP_KIND_BASE'},{icon:'rally',color:'#FFB27A',key:'MAP_KIND_RALLY'},{icon:'danger',color:'#EC6A5C',key:'MAP_KIND_DANGER'},
  {icon:'stash',color:'#E8E2D6',key:'MAP_KIND_STASH'},{icon:'anomaly',color:'#E8C35A',key:'MAP_KIND_ANOMALY'},{icon:'note',color:'#B5B2AB',key:'MAP_KIND_NOTE'},
];
Pages.map = {
  EDGE:30, COL_W:290, view:null, layers:[true,true,true,true], matesAt:Date.now()-52000, sosT:0,
  build(r,W,H){
    this.W=W; this.H=H; this.r=r; this.pop=null; this.staticImg=null; this.items=null; this.ni=null;
    this.mapMod=Shell.hasMod('map'); this.gps=Shell.hasMod('gps');
    this.fac=this.gps&&Shell.hasMod('gp')&&S.role>0; this.manage=S.adminMode||S.role>=2;
    if(!this.mapMod){ this.buildNoMap(); return; }
    const m=this.host=D.Clip(r,0,0,W,H,'MapHost'); m.style.background='#c9c7b2';
    this.img=document.createElement('img'); this.img.src='assets/map_placeholder.jpg'; this.img.style.cssText='position:absolute;left:0;top:0;pointer-events:none;image-rendering:auto'; this.img.draggable=false; m.appendChild(this.img);
    D.Line(m,0,0,W,H,'rgba(12,10,8,.25)');
    const v=D.Line(m,0,0,W,H,''); v.style.background='radial-gradient(ellipse at 50% 50%,transparent 55%,rgba(0,0,0,.55) 100%)';
    this.pins=D.Frame(m,0,0,W,H,'Pins'); this.pins.style.pointerEvents='none';
    if(!this.view) this.view=this.gps?{x:S.pos.x,z:S.pos.z,s:.1*3}:{x:WORLD_M/2,z:WORLD_M/2,s:1};
    this.input(m);
    this.buildLeft(m); this.buildRight(m); this.frame(m);
    this.rebuildPins(); this.apply(); this.cooldowns();
  },
  buildNoMap(){
    const {r,W,H}=this, n=D.Clip(r,0,0,W,H,'NoMap');
    D.Line(n,0,0,W,H,C.BG);
    this.staticImg=D.Img(n,0,0,W+256,H+256,'assets/nomap.png'); this.staticImg.style.opacity=.27; this.staticImg.style.backgroundSize='512px 512px'; this.staticImg.style.backgroundRepeat='repeat';
    const pw=660,ph=220,px=(W-pw)/2,py=H*.5-122;
    D.Box(n,px,py,pw,ph,6,'rgba(11,11,12,.9)',C.LINE); D.Line(n,px,py,pw,3,C.ORANGE);
    Look.Locked(n,W,H,'map','MAP_NOMAP_TITLE','MAP_NOMAP_TEXT');
    this.host=null;
  },
  plate(p,x,y,w,h){ const f=D.Frame(p,x,y,w,h,'MapPlate'); D.Box(f,0,0,w,h,6,'rgba(14,14,16,.92)',C.LINE); f.onmousedown=e=>e.stopPropagation(); f.onwheel=e=>e.stopPropagation(); f.oncontextmenu=e=>{e.preventDefault();e.stopPropagation();}; f.ondblclick=e=>e.stopPropagation(); f.onclick=e=>e.stopPropagation(); return f; },
  buildLeft(m){
    const w=this.COL_W,E=this.EDGE, a=this.plate(m,E,E,w,198);
    D.Text(a,18,12,w-36,20,Caps('MAP_LAYERS'),13,C.DIM);
    ['MAP_LAYER_MINE','MAP_LAYER_FACTION','MAP_LAYER_MATES','MAP_LAYER_SOS'].forEach((k,i)=>{
      const ry=42+i*36, on=i===0||this.fac, row=D.Frame(a,18,ry,w-36,36,'Sw');
      D.Text(row,0,0,w-92,36,T(k),16,on?C.TEXT:C.DIM2);
      UI.Switch(row,w-36-46,6,this.layers[i]&&on,on?v=>{this.layers[i]=v;this.rebuildPins();this.place();}:null);
    });
    const l=this.plate(m,E,E+212,w,324);
    D.Text(l,18,12,w-36,20,Caps('MAP_LEGEND'),13,C.DIM);
    D.Icon(l,19,46,22,'arrow',C.ORANGE); D.Text(l,54,42,w-72,30,T('MAP_LEG_ME'),15,C.TEXT);
    D.Circle(l,22,79,16,C.ALLY); D.Text(l,54,72,w-72,30,T('MAP_LEG_MATE'),15,C.TEXT);
    D.Circle(l,19,106,22,C.ENEMY); D.Icon(l,23,110,14,'sos',C.WHITE); D.Text(l,54,102,w-72,30,T('MAP_LEG_SOS'),15,C.TEXT);
    KIND.forEach((k,i)=>{const ry=132+i*30; D.Circle(l,19,ry+4,22,k.color); D.Icon(l,23,ry+8,14,k.icon,C.BG); D.Text(l,54,ry,w-72,30,T(k.key),15,C.TEXT);});
    const b=this.plate(m,E,this.H-E-34,w,34);
    this.tickL=D.Line(b,14,13,2,11,C.WHITE); this.bar=D.Line(b,14,22,100,2,C.WHITE); this.tickR=D.Line(b,112,13,2,11,C.WHITE);
    this.scaleT=D.Text(b,130,0,w-140,34,'',15,C.TEXT);
  },
  btn(p,x,y,w,h,icon,text,style,fs,fn){
    const e=UI.Button(p,x,y,w,h,text,style==='sos'?'danger':'secondary',icon,fn,fs); e.style.borderRadius='4px';
    if(style==='danger'){e.style.color='#EC6A5C';e.style.borderColor='#5A2B27';}
    const i=e.querySelector('.ic'); if(i){const s=Math.max(14,Math.min(30,h*.46)); i.style.width=i.style.height=s+'px';}
    return e;
  },
  buildRight(m){
    const w=this.COL_W,x=this.W-this.EDGE-w,h=this.H-2*this.EDGE,bw=(w-46)/2, rp=this.plate(m,x,this.EDGE,w,h);
    D.Text(rp,18,12,160,20,Caps('MAP_COORDS'),13,C.DIM);
    if(!this.gps){ D.Pill(rp,w-18-96,10,96,24,null,C.DIM2); D.Text(rp,w-18-96,10,96,24,Caps('MAP_NO_GPS'),12,C.DIM,'c'); }
    const g=this.gps;
    D.Text(rp,18,36,w-36,40,g?'X '+Math.round(S.pos.x):'X —',34,C.TEXT);
    D.Text(rp,18,74,w-36,40,g?'Y '+Math.round(S.pos.z):'Y —',34,C.TEXT);
    D.Text(rp,18,116,w-36,18,g?Caps('MAP_GRID')+' '+String(Math.floor(S.pos.x/100)).padStart(3,'0')+' '+String(Math.floor(S.pos.z/100)).padStart(3,'0'):'',13,C.DIM);
    D.Line(rp,18,146,w-36,1,C.LINE);
    const ring=D.Box(rp,18,158,54,54,27,null,C.LINE2); D.Line(rp,44,158,2,6,C.DIM);
    const turn=D.Frame(rp,27,167,36,36,'Compass'); D.Icon(turn,5,5,26,'arrow',C.ORANGE); turn.style.transform=`rotate(${g?S.pos.heading:0}deg)`;
    D.Text(rp,86,158,w-104,18,Caps('MAP_HEADING'),13,C.DIM);
    D.Text(rp,86,176,w-104,36,g?S.pos.heading+'° '+T('MAP_DIR_NE'):'—',30,C.TEXT);
    this.btn(rp,18,226,bw,46,'plus','', '',15,()=>this.zoom(1/1.6));
    this.btn(rp,28+bw,226,bw,46,'minus','', '',15,()=>this.zoom(1.6));
    const cb=this.btn(rp,18,282,w-36,46,'center',CapsT(T('MAP_CENTER')),'',15,()=>{this.view.x=S.pos.x;this.view.z=S.pos.z;this.apply();});
    cb.style.justifyContent='flex-start'; cb.style.paddingLeft='16px';
    if(!g) cb.classList.add('off');
    const rb=this.btn(rp,18,338,w-36,46,'refresh',CapsT(T('MAP_REFRESH')),'',14,()=>{this.matesAt=Date.now();Toast(T('MAP_UPDATED'),'ok');});
    rb.style.justifyContent='flex-start'; rb.style.paddingLeft='16px';
    if(!this.fac) rb.classList.add('off');
    this.matesAge=D.Text(rp,18,388,w-36,18,'',13,C.DIM2);
    this.btn(rp,18,414,bw,44,'pin',CapsT(T('MAP_MARK')),'',13,()=>this.openAdd(S.pos.x,S.pos.z,this.W/2,this.H/2));
    this.btn(rp,28+bw,414,bw,44,'search',CapsT(T('MAP_NEXT')),'',13,()=>this.nextMark());
    D.MText(rp,18,468,w-36,60,T('MAP_HINT'),13,C.DIM2);
    this.sosBtn=this.btn(rp,18,h-78,w-36,60,'sos',T('MAP_SOS'),'sos',26,()=>this.askSos());
    this.sosBtn.style.borderRadius='4px';
    if(!this.fac) this.sosBtn.classList.add('off');
  },
  frame(m){
    const W=this.W,H=this.H,k=22,o=C.ORANGE,e=C.LINE2;
    const L=(x,y,w,h,c)=>{const l=D.Line(m,x,y,w,h,c);l.style.zIndex=4;};
    L(0,0,W,1,e);L(0,H-1,W,1,e);L(0,0,1,H,e);L(W-1,0,1,H,e);
    L(0,0,k,2,o);L(0,0,2,k,o);L(W-k,0,k,2,o);L(W-2,0,2,k,o);L(0,H-2,k,2,o);L(0,H-k,2,k,o);L(W-k,H-2,k,2,o);L(W-2,H-k,2,k,o);
  },
  // --- вид ------------------------------------------------------------------------------------------------------------
  mpp(){ return WORLD_M*this.view.s/this.W; },
  toLocal(x,z){ const m=this.mpp(); return [this.W/2+(x-this.view.x)/m, this.H/2-(z-this.view.z)/m]; },
  toWorld(lx,ly){ const m=this.mpp(); return [this.view.x+(lx-this.W/2)*m, this.view.z-(ly-this.H/2)*m]; },
  apply(){
    const v=this.view; v.s=Math.max(.02,Math.min(1,v.s));
    const m=this.mpp(), size=WORLD_M/m, [lx,ly]=this.toLocal(0,WORLD_M);
    this.img.style.width=this.img.style.height=size+'px'; this.img.style.left=lx+'px'; this.img.style.top=ly+'px';
    this.place();
    const steps=[10,25,50,100,250,500,1000,2500]; let sm=steps[0]; for(const s of steps) if(s/m<=100) sm=s;
    const px=sm/m; this.bar.style.width=px+'px'; this.tickR.style.left=14+px-2+'px';
    this.scaleT.textContent=sm>=1000?(sm/1000)+' '+T('MAP_KM'):sm+' '+T('MAP_M');
  },
  zoom(k,cx,cy){
    if(cx==null){cx=this.W/2;cy=this.H/2;}
    const [wx,wz]=this.toWorld(cx,cy); this.view.s*=k; this.view.s=Math.max(.02,Math.min(1,this.view.s));
    const m=this.mpp(); this.view.x=wx-(cx-this.W/2)*m; this.view.z=wz+(cy-this.H/2)*m; this.apply();
  },
  input(m){
    let drag=null, moved=false;
    const local=e=>{const b=m.getBoundingClientRect(),k=b.width/this.W;return [(e.clientX-b.left)/k,(e.clientY-b.top)/k];};
    m.onmousedown=e=>{ if(e.button!==0) return; const [lx,ly]=local(e); drag={lx,ly,x:this.view.x,z:this.view.z}; moved=false; m.style.cursor='grabbing'; };
    this.mv=e=>{ if(!drag) return; const [lx,ly]=local(e), mp=this.mpp(), dx=lx-drag.lx, dy=ly-drag.ly; if(Math.abs(dx)+Math.abs(dy)>3) moved=true; this.view.x=drag.x-dx*mp; this.view.z=drag.z+dy*mp; this.apply(); };
    this.up=()=>{drag=null;m.style.cursor='';};
    window.addEventListener('mousemove',this.mv); window.addEventListener('mouseup',this.up);
    m.onwheel=e=>{e.preventDefault();const [lx,ly]=local(e);this.zoom(e.deltaY>0?1.25:1/1.25,lx,ly);};
    m.oncontextmenu=e=>{e.preventDefault();const [lx,ly]=local(e),[x,z]=this.toWorld(lx,ly);this.openAdd(x,z,lx,ly);};
    m.ondblclick=e=>{const [lx,ly]=local(e),[x,z]=this.toWorld(lx,ly);this.openAdd(x,z,lx,ly);};
    m.onclick=()=>{ if(!moved) this.closePops(); };
  },
  onHide(){ if(this.mv){window.removeEventListener('mousemove',this.mv);window.removeEventListener('mouseup',this.up);} },
  // --- метки ----------------------------------------------------------------------------------------------------------
  rebuildPins(){
    const p=this.pins; p.innerHTML=''; this.items=[];
    const add=(type,o)=>{const e=D.Frame(p,0,0,0,0,'Pin'); e.className='pin'; e.style.pointerEvents='auto'; this.items.push({type,o,e}); return e;};
    const style=(e,size,fill,icon,ink,tag)=>{
      const d=D.Circle(e,-size/2,-size/2,size,fill); d.classList.add('dot'); d.style.cursor='pointer'; d.style.pointerEvents='auto';
      if(icon) {const s=size*.64,i=D.Icon(e,-s/2,-s/2,s,icon,ink);}
      if(tag){const t=document.createElement('div');t.className='tag';t.textContent=tag;e.appendChild(t);}
      return d;
    };
    if(this.layers[0]) S.marksOwn.forEach(m=>{const k=KIND[m.Kind],e=add('own',m),d=style(e,26,k.color,k.icon,C.BG,m.Name);d.onclick=ev=>{ev.stopPropagation();this.openInfo('own',m);};});
    const f=Data.myFaction();
    if(this.layers[1]&&this.fac&&f) f.Marks.forEach(m=>{const k=KIND[m.Kind],e=add('fac',m),d=style(e,30,k.color,k.icon,C.BG,m.Name);d.style.boxShadow='0 0 0 2px '+C.ORANGE;d.onclick=ev=>{ev.stopPropagation();this.openInfo('fac',m);};});
    if(this.layers[2]&&this.fac) S.mates.forEach(m=>{const e=add('mate',m),d=style(e,16,C.ALLY,null,null,m.Callsign);d.onclick=ev=>{ev.stopPropagation();this.openInfo('mate',m);};});
    if(this.layers[3]&&this.fac) S.sos.forEach(m=>{const e=add('sos',m),d=style(e,34,C.ENEMY,'sos',C.WHITE,'SOS · '+m.Callsign);e.classList.add('blink');d.onclick=ev=>{ev.stopPropagation();this.openInfo('sos',m);};});
    this.me=D.Frame(p,0,0,0,0,'Me'); this.me.className='pin'; this.me.style.display=this.gps?'':'none';
    if(this.gps){
      const r1=document.createElement('div'),r2=document.createElement('div'); r1.className=r2.className='ring'; r2.style.animationDelay='1.1s'; this.me.append(r1,r2);
      const t=D.Frame(this.me,-22,-22,44,44,'MeTurn'); D.Icon(t,0,0,44,'arrow',C.BG); D.Icon(t,6,6,32,'arrow',C.ORANGE); t.style.transform=`rotate(${S.pos.heading}deg)`;
      const tw=TextW(T('MAP_ME'),15)+14, g=D.Frame(this.me,18,-42,tw,22,'MeTag'); D.Line(g,0,0,tw,22,C.ORANGE); D.Text(g,0,0,tw,22,T('MAP_ME'),15,C.ON_ORANGE,'c');
    }
  },
  place(){
    if(!this.items) return;
    this.items.forEach(i=>{const [x,y]=this.toLocal(i.o.X,i.o.Z);i.e.style.left=x+'px';i.e.style.top=y+'px';});
    if(this.gps){const [x,y]=this.toLocal(S.pos.x,S.pos.z);this.me.style.left=x+'px';this.me.style.top=y+'px';}
  },
  dist(x,z){
    const dx=x-S.pos.x,dz=z-S.pos.z,d=Math.round(Math.hypot(dx,dz)); let a=Math.atan2(dx,dz)*180/Math.PI; if(a<0)a+=360;
    return (d>=1000?(d/1000).toFixed(1)+' '+T('MAP_KM'):d+' '+T('MAP_M'))+' · '+T('DIR_'+(Math.round(a/45)%8));
  },
  closePops(){ if(this.pop){this.pop.remove();this.pop=null;} },
  popup(x,y,w,h,stripe){
    this.closePops();
    const px=Math.max(330,Math.min(this.W-330-w,x+14)), py=Math.max(20,Math.min(this.H-h-20,y-20));
    const f=this.pop=D.Frame(this.host,px,py,w,h,'Pop'); f.style.zIndex=20; f.style.boxShadow='0 14px 36px rgba(0,0,0,.6)'; f.onmousedown=e=>e.stopPropagation(); f.onclick=e=>e.stopPropagation(); f.ondblclick=e=>e.stopPropagation(); f.oncontextmenu=e=>{e.preventDefault();e.stopPropagation();};
    D.Box(f,0,0,w,h,6,'rgba(19,19,21,.98)',C.LINE2); if(stripe) D.Line(f,0,0,w,3,stripe);
    return f;
  },
  openAdd(x,z,lx,ly){
    const f=this.popup(lx,ly,336,256,C.ORANGE); let forFaction=false;
    D.Text(f,18,12,266,26,Caps('MAP_NEW_MARK'),16,C.TEXT);
    this.btn(f,294,10,28,28,'close','', '',13,()=>this.closePops());
    D.Text(f,18,38,300,18,'X '+Math.round(x)+'   Y '+Math.round(z),13,C.DIM);
    KIND.forEach((k,i)=>{
      const tx=18+(i%3)*102, ty=64+Math.floor(i/3)*70, t=D.Frame(f,tx,ty,96,64,'Kind'), u=D.Box(t,0,0,96,64,4,C.PANEL2,C.LINE2);
      D.Circle(t,35,7,26,k.color); D.Icon(t,40,12,16,k.icon,C.BG); D.Text(t,2,38,92,20,T(k.key),12,C.TEXT,'c');
      D.Hit(t,0,0,96,64,()=>this.pickKind(i,x,z,forFaction),on=>u.style.background=on?'#26262B':C.PANEL2);
    });
    if(this.fac&&this.manage){ D.Text(f,18,208,250,36,T('MAP_FOR_FACTION'),16,C.TEXT); UI.Switch(f,272,214,false,v=>forFaction=v); }
    else D.Text(f,18,208,300,36,T('MAP_PERSONAL_NOTE'),13,C.DIM2);
  },
  pickKind(i,x,z,forFaction){
    this.closePops();
    Shell.openPrompt(T('MAP_NEW_MARK'),T('MAP_F_NAME'),'',false,T(KIND[i].key),(ok,a)=>{
      if(!ok) return;
      const m={Id:Date.now(),Kind:i,Name:a||T(KIND[i].key),X:Math.round(x),Z:Math.round(z),Author:S.me.Callsign};
      if(forFaction){Data.myFaction().Marks.push(m);Toast(T('MAP_FMARK_ADDED'),'ok');} else S.marksOwn.push(m);
      this.rebuildPins(); this.place();
    });
  },
  openInfo(type,m){
    const [lx,ly]=this.toLocal(m.X,m.Z), f=this.popup(lx,ly,300,150), own=type==='own', canDel=own||(type==='fac'&&this.manage);
    const stripe=type==='sos'?C.ENEMY:type==='mate'?C.ALLY:KIND[m.Kind].color;
    D.Line(f,0,0,4,150,stripe);
    const title=type==='mate'?m.Callsign:type==='sos'?'SOS · '+m.Callsign:m.Name;
    D.Text(f,20,12,234,28,Fit(title,234,20),20,C.TEXT);
    this.btn(f,260,10,28,28,'close','', '',13,()=>this.closePops());
    const sub=type==='mate'?T('MAP_LEG_MATE'):type==='sos'?T('MAP_LEG_SOS'):T(KIND[m.Kind].key)+' · '+T(own?'MAP_PERSONAL':'MAP_FACTION_MARK');
    D.Text(f,20,44,266,18,sub,13,C.DIM);
    D.Text(f,20,64,266,18,this.gps?this.dist(m.X,m.Z):'X '+Math.round(m.X)+'   Y '+Math.round(m.Z),13,C.DIM);
    const act=this.btn(f,20,94,164,42,canDel?'trash':'eye_off',CapsT(T(canDel?'MAP_DELETE':'MAP_HIDE')),canDel?'danger':'',14,()=>{
      if(own) S.marksOwn=S.marksOwn.filter(x=>x!==m);
      else if(type==='fac'){const fm=Data.myFaction().Marks;fm.splice(fm.indexOf(m),1);}
      else if(type==='sos') S.sos=S.sos.filter(x=>x!==m);
      this.closePops(); this.rebuildPins(); this.place(); if(canDel) Toast(T('MAP_MARK_DELETED'),'ok');
    });
    if(type==='mate') act.classList.add('off');
    this.btn(f,194,94,86,42,'center','', '',14,()=>{this.view.x=m.X;this.view.z=m.Z;this.apply();this.closePops();});
  },
  nextMark(){
    if(!this.items||!this.items.length){Toast(T('MAP_NO_MARKS'),'warn');return;}
    this.ni=((this.ni==null?-1:this.ni)+1)%this.items.length;
    const o=this.items[this.ni].o; this.view.x=o.X; this.view.z=o.Z; this.apply();
  },
  askSos(){
    const w=420,h=204,f=this.popup(this.W/2-w/2,this.H/2-h/2+20,w,h,C.ENEMY);
    f.style.left=(this.W-w)/2+'px'; f.style.top=(this.H-h)/2+'px';
    D.Icon(f,24,22,30,'sos',C.ENEMY); D.Text(f,66,20,w-90,34,Caps('MAP_SOS_TITLE'),22,C.TEXT);
    D.MText(f,24,66,w-48,60,T('MAP_SOS_TEXT'),15,C.DIM);
    this.btn(f,24,140,178,46,'',CapsT(T('CANCEL')),'',15,()=>this.closePops());
    this.btn(f,w-24-178,140,178,46,'sos',T('MAP_SOS'),'sos',18,()=>{this.closePops();SosClock.start();Toast(T('MAP_SOS_SENT'),'ok');});
  },
  cooldowns(){
    if(this.matesAge&&this.fac){const s=Math.round((Date.now()-this.matesAt)/1000);this.matesAge.textContent=T('MAP_UPDATED')+' '+s+' '+T('MAP_SEC_AGO');}
    if(this.sosBtn){const left=SosClock.left();this.sosBtn.label.textContent=left>0?left+' '+T('FAC_SEC'):T('MAP_SOS');this.sosBtn.classList.toggle('off',left>0||!this.fac);}
  },
  tick(dt){
    if(this.staticImg){this.staticT=(this.staticT||0)+dt;if(this.staticT>.07){this.staticT=0;this.staticImg.style.left=-Math.random()*256+'px';this.staticImg.style.top=-Math.random()*256+'px';}return;}
    this.sosT+=dt; if(this.sosT>=.25){this.sosT=0;this.cooldowns();}
  },
  esc(){ if(this.pop){this.closePops();return true;} return false; },
};

// Общий счётчик SOS для «Карты» и «Группировки» (INT_PDA_SosClock), 30 секунд.
const SosClock = {
  at:0,
  start(){ this.at=Date.now(); },
  left(){ return Math.max(0,30-Math.floor((Date.now()-this.at)/1000)); },
};
