// Карта: метки, зоны, круги и маршруты, личные или общие на клан. В игре MapWidget; зоны рисуются на CanvasWidget линиями:
// сплошной заливки у движка нет, поэтому контур и штриховка. Всё хранится в мировых координатах, экран пересчитывается при сдвиге.
const WORLD_M=10240;
const TOOLS=[['hand','Обзор','cursor'],['mark','Метка','pin'],['zone','Зона','polygon'],['circle','Круг','circleo'],['route','Маршрут','route']];
const TYPE_NAME={mark:'Метка',zone:'Зона',circle:'Круг',route:'Маршрут'};
const MAX_POINTS=16;

Pages.map={
  view:null,layers:{mine:true,clan:true,mates:true,server:true},tool:'hand',dest:'me',
  build(host){
    const W=this.W=1228,H=this.H=776,m=this.m=D.Clip(host,0,0,W,H);m.style.background='#c9c8ba';
    this.sel=null;this.draft=null;this.pp=null;this.isNew=false;this.pe=false;this.drag=null;this.leftP=null;this.dBar=null;
    this.img=document.createElement('img');this.img.src='assets/map_placeholder.jpg';this.img.draggable=false;this.img.style.cssText='position:absolute;pointer-events:none';m.appendChild(this.img);
    const ve=D.Line(m,0,0,W,H,'');ve.style.background='radial-gradient(ellipse at 50% 50%,rgba(14,13,12,0) 55%,rgba(14,13,12,.26) 100%)';
    this.svg=document.createElementNS('http://www.w3.org/2000/svg','svg');this.svg.setAttribute('width',W);this.svg.setAttribute('height',H);this.svg.style.cssText='position:absolute;left:0;top:0;pointer-events:none';m.appendChild(this.svg);
    this.pins=D.Frame(m,0,0,W,H);this.pins.style.pointerEvents='none';
    if(!this.view)this.view={x:S.pos.x+150,z:S.pos.z-100,s:.34};
    if(!S.clan)this.dest='me';
    this.input();this.ui();this.refresh();this.apply();
    if(window.SEL){const o=S.objects.find(x=>x.id===window.SEL);if(o){this.select(o);if(window.PE){this.pe=true;this.paintProps();}}}
  },
  // --- вид ------------------------------------------------------------------------------------------------------------
  mpp(){return WORLD_M*this.view.s/this.W;},
  loc(x,z){const k=this.mpp();return[this.W/2+(x-this.view.x)/k,this.H/2-(z-this.view.z)/k];},
  world(lx,ly){const k=this.mpp();return[this.view.x+(lx-this.W/2)*k,this.view.z-(ly-this.H/2)*k];},
  apply(){
    this.view.s=Math.max(.03,Math.min(1,this.view.s));
    const k=this.mpp(),[lx,ly]=this.loc(0,WORLD_M),z=WORLD_M/k;Object.assign(this.img.style,{width:z+'px',height:z+'px',left:lx+'px',top:ly+'px'});
    this.place();
    let v=10;[10,25,50,100,250,500,1000,2500].forEach(s=>{if(s/k<=80)v=s;});this.bar.style.width=v/k+'px';this.sc.textContent=v>=1000?v/1000+' км':v+' м';
  },
  zoom(f,cx=this.W/2,cy=this.H/2){const[wx,wz]=this.world(cx,cy);this.view.s=Math.max(.03,Math.min(1,this.view.s*f));const k=this.mpp();this.view.x=wx-(cx-this.W/2)*k;this.view.z=wz+(cy-this.H/2)*k;this.apply();},
  centerOn(o){const c=this.anchor(o);this.view.x=c[0];this.view.z=c[1];this.apply();},
  // --- мышь -----------------------------------------------------------------------------------------------------------
  input(){
    const m=this.m;let d=null,moved=false;const l=e=>{const b=m.getBoundingClientRect(),k=b.width/this.W;return[(e.clientX-b.left)/k,(e.clientY-b.top)/k];};
    m.onmousedown=e=>{
      if(e.button)return;const[a,b]=l(e);moved=false;
      const h=this.handleAt(a,b);if(h!=null){this.drag=h;return;}
      if(this.tool==='circle'&&!this.draft){this.draft={type:'circle',pts:[this.world(a,b)],r:0};this.drag='new';return;}
      d={a,b,x:this.view.x,z:this.view.z};
    };
    this.mv=e=>{
      const[a,b]=l(e);
      if(this.cT&&a>=0&&b>=0&&a<=this.W&&b<=this.H)this.coords(this.world(a,b));
      if(this.drag==='new'){const c=this.draft.pts[0],w=this.world(a,b);this.draft.r=Math.hypot(w[0]-c[0],w[1]-c[1]);this.paintShapes();return;}
      if(this.drag!=null){const w=this.world(a,b),o=this.sel;if(o.type==='circle')o.r=Math.max(20,Math.hypot(w[0]-o.pts[0][0],w[1]-o.pts[0][1]));else o.pts[this.drag]=w;this.place();return;}
      if(this.draft&&this.draft.type!=='circle'){this.draft.cur=this.world(a,b);this.paintShapes();}
      if(!d)return;const k=this.mpp();if(Math.abs(a-d.a)+Math.abs(b-d.b)>3)moved=true;this.view.x=d.x-(a-d.a)*k;this.view.z=d.z+(b-d.b)*k;this.apply();
    };
    this.up=e=>{
      if(this.drag==='new'){this.drag=null;if(this.draft.r<20)this.draft.r=150;this.finish();return;}
      if(this.drag!=null){this.drag=null;this.paintProps();return;}
      if(d&&!moved){const[a,b]=l(e);this.clickAt(a,b);}d=null;
    };
    addEventListener('mousemove',this.mv);addEventListener('mouseup',this.up);
    m.onmouseleave=()=>{if(this.cT)this.coords();};
    m.onwheel=e=>{e.preventDefault();const[a,b]=l(e);this.zoom(e.deltaY>0?1.25:.8,a,b);};
    m.oncontextmenu=e=>{e.preventDefault();if(this.draft)this.undo();};
  },
  leave(){removeEventListener('mousemove',this.mv);removeEventListener('mouseup',this.up);},
  clickAt(a,b){
    const w=this.world(a,b);
    if(this.tool==='hand'){const o=this.hit(a,b);if(o)this.select(o);else this.close();return;}
    if(this.tool==='mark'){this.draft={type:'mark',pts:[w]};this.finish();return;}
    if(this.tool==='zone'||this.tool==='route'){
      if(!this.draft)this.draft={type:this.tool,pts:[]};
      const p=this.draft.pts;
      if(this.tool==='zone'&&p.length>=3){const f=this.loc(p[0][0],p[0][1]);if(Math.hypot(a-f[0],b-f[1])<14){this.finish();return;}}
      p.push(w);this.draftBar();this.paintShapes();
      if(p.length>=MAX_POINTS)this.finish();
    }
  },
  handleAt(a,b){
    const o=this.sel;if(!o||!this.canEdit(o)||o.type==='mark')return null;
    if(o.type==='circle'){const c=this.loc(o.pts[0][0],o.pts[0][1]),r=o.r/this.mpp();return Math.hypot(a-c[0]-r,b-c[1])<12?0:null;}
    for(let i=0;i<o.pts.length;i++){const p=this.loc(o.pts[i][0],o.pts[i][1]);if(Math.hypot(a-p[0],b-p[1])<12)return i;}
    return null;
  },
  hit(a,b){
    const l=this.visible().filter(o=>o.type!=='mark').reverse(),w=this.world(a,b),k=this.mpp();
    for(const o of l){
      if(o.type==='circle'&&Math.hypot(w[0]-o.pts[0][0],w[1]-o.pts[0][1])<=o.r)return o;
      if(o.type==='zone'&&this.inPoly(w,o.pts))return o;
      if(o.type==='route')for(let i=1;i<o.pts.length;i++)if(this.segDist(w,o.pts[i-1],o.pts[i])/k<9)return o;
    }
    return null;
  },
  inPoly(p,v){let c=false;for(let i=0,j=v.length-1;i<v.length;j=i++)if((v[i][1]>p[1])!==(v[j][1]>p[1])&&p[0]<(v[j][0]-v[i][0])*(p[1]-v[i][1])/(v[j][1]-v[i][1])+v[i][0])c=!c;return c;},
  segDist(p,a,b){const dx=b[0]-a[0],dy=b[1]-a[1],t=Math.max(0,Math.min(1,((p[0]-a[0])*dx+(p[1]-a[1])*dy)/(dx*dx+dy*dy||1)));return Math.hypot(p[0]-a[0]-t*dx,p[1]-a[1]-t*dy);},
  // --- данные ---------------------------------------------------------------------------------------------------------
  // Клановые объекты ведут те, чей ранг это разрешает.
  access(){return Clan.can('map');},
  canEdit(o){return o.server?!!S.admin:(!o.clan||this.access());},
  scopeOf(o){return o.server?'server':o.clan?'clan':'me';},
  limitKey(t){return t==='mark'?'marks':t==='route'?'routes':'zones';},
  used(scope,t){const k=this.limitKey(t);return S.objects.filter(o=>this.scopeOf(o)===scope&&this.limitKey(o.type)===k).length;},
  // Кому видно новое: личное всегда, клану по рангу, всем только администратору.
  dests(type){const l=[['me','Только мне',76]];if(this.access())l.push(['clan','Клану',60]);if(S.admin&&type==='mark')l.push(['server','Всем',52]);return l;},
  catOf(o){return S.clan?S.clan.cats.find(c=>c.id===o.cat)||S.clan.cats[0]:null;},
  inLayer(o){if(o.server)return this.layers.server;if(!o.clan)return this.layers.mine;if(!S.clan||!this.layers.clan)return false;const c=this.catOf(o);return !(c&&c.hidden);},
  visible(){return S.objects.filter(o=>this.inLayer(o)&&!o.hidden);},
  colorOf(o){return o.color;},
  anchor(o){if(o.type==='zone'){const n=o.pts.length;return[o.pts.reduce((a,p)=>a+p[0],0)/n,o.pts.reduce((a,p)=>a+p[1],0)/n];}return o.type==='route'?o.pts[Math.floor(o.pts.length/2)]:o.pts[0];},
  measure(o){
    if(o.type==='zone'){let s=0;o.pts.forEach((p,i)=>{const q=o.pts[(i+1)%o.pts.length];s+=p[0]*q[1]-q[0]*p[1];});return 'Площадь '+(Math.abs(s)/2e6).toFixed(2).replace('.',',')+' км²';}
    if(o.type==='circle')return 'Радиус '+Math.round(o.r)+' м';
    if(o.type==='route'){let s=0;for(let i=1;i<o.pts.length;i++)s+=Math.hypot(o.pts[i][0]-o.pts[i-1][0],o.pts[i][1]-o.pts[i-1][1]);return 'Длина '+(s/1000).toFixed(2).replace('.',',')+' км';}
    return 'X '+Math.round(o.pts[0][0])+'   Y '+Math.round(o.pts[0][1]);
  },
  dist(x,z){const d=Math.round(Math.hypot(x-S.pos.x,z-S.pos.z));return d>=1000?(d/1000).toFixed(1).replace('.',',')+' км':d+' м';},
  // --- рисование ------------------------------------------------------------------------------------------------------
  setTool(t){if(this.draft)this.draft=null;this.tool=t;this.close();this.m.style.cursor=t==='hand'?'':'crosshair';this.paintTools();this.draftBar();this.paintShapes();},
  undo(){const d=this.draft;if(!d)return;if(d.pts&&d.pts.length>1&&d.type!=='circle')d.pts.pop();else this.draft=null;this.draftBar();this.paintShapes();},
  finish(){
    const d=this.draft;if(!d)return;
    if(d.type==='zone'&&d.pts.length<3){Toast('Зоне нужно минимум 3 точки','err');return;}
    if(d.type==='route'&&d.pts.length<2){Toast('Маршруту нужно минимум 2 точки','err');return;}
    const clan=this.dest==='clan'&&this.access(),server=this.dest==='server'&&!!S.admin&&d.type==='mark',scope=server?'server':clan?'clan':'me',lim=MAP_LIMITS[scope][this.limitKey(d.type)];if(this.used(scope,d.type)>=lim){Toast('Достигнут лимит: '+lim,'err');this.draft=null;this.setTool('hand');return;}
    const n=S.objects.filter(o=>o.type===d.type).length+1,cat=clan?(S.clan.cats.find(c=>c.id===this.curCat)||S.clan.cats[0]):null;
    const o={id:Date.now(),type:d.type,icon:'pin',name:TYPE_NAME[d.type]+' '+n,color:cat?cat.color:DEFAULT_ACCENT,cat:cat?cat.id:null,pts:d.pts.map(p=>[Math.round(p[0]),Math.round(p[1])]),clan,server,author:server?'Администрация':S.name};
    if(d.type==='circle')o.r=Math.round(d.r);
    S.objects.push(o);this.draft=null;this.tool='hand';this.m.style.cursor='';this.paintTools();this.draftBar();this.isNew=true;this.sel=o;this.refresh();this.reveal(o);this.paintProps();
  },
  // Карта сдвигается, если объект оказался бы под карточкой.
  reveal(o){const ax=this.loc(...this.anchor(o))[0],lim=this.W-12-244-40;if(ax>lim){this.view.x+=(ax-lim)*this.mpp();this.apply();}},
  select(o){this.sel=o;this.isNew=false;this.pe=false;this.reveal(o);this.paintProps();this.paintShapes();this.paintList();this.declutter();},
  close(){
    if(this.sel&&this.isNew&&this.sel.clan&&S.clan)S.clan.journal.unshift(['pin','На карте новое: '+this.sel.name,'сейчас']);
    this.sel=null;this.isNew=false;this.pe=false;if(this.pp){this.pp.remove();this.pp=null;}if(this.m)this.paintShapes();if(this.mList)this.paintList();this.declutter();
  },
  remove(o){S.objects=S.objects.filter(x=>x!==o);this.isNew=false;this.close();this.refresh();Toast('Удалено');},
  esc(){if(this.draft){this.draft=null;this.draftBar();this.paintShapes();return;}this.close();},
  // --- интерфейс ------------------------------------------------------------------------------------------------------
  // Стекло поверх карты: только прозрачность, без размытия, в игре это альфа виджета. При наведении плотнее, чтобы легко читалось.
  plate(x,y,w,h,a=.62){
    const f=D.Frame(this.m,x,y,w,h),bg=D.Box(f,0,0,w,h,8,`rgba(14,13,12,${a})`,'rgba(255,255,255,.09)');f.style.zIndex=5;
    ['onmousedown','onclick','onwheel','oncontextmenu'].forEach(k=>f[k]=e=>{e.stopPropagation();if(k==='oncontextmenu')e.preventDefault();});
    f.onmouseenter=()=>bg.style.background='rgba(14,13,12,.9)';f.onmouseleave=()=>bg.style.background=`rgba(14,13,12,${a})`;
    return f;
  },
  // Кнопка-значок без подложки, подсказка с названием появляется при наведении.
  icon(p,x,y,s,ic,fn,on,tip){
    const soft=on==='soft',bg=D.Box(p,x,y,s,s,6,soft?'rgba(255,255,255,.14)':on?C.ACCENT:'transparent'),i=D.Icon(p,x+(s-18)/2,y+(s-18)/2,18,ic,soft?C.ACCENT:on?C.ON:C.TEXT);let t=null;
    D.Hit(p,x,y,s,s,fn,v=>{if(!on)bg.style.background=v?'rgba(255,255,255,.12)':'transparent';if(t){t.remove();t=null;}if(v&&tip){const w=TextW(tip,13)+20;t=D.Frame(p,x+s+10,y+(s-26)/2,w,26);D.Box(t,0,0,w,26,6,'rgba(14,13,12,.92)');D.Text(t,10,0,w-12,26,tip,13,C.TEXT);}});
    return i;
  },
  ui(){
    const W=this.W;
    this.rail=this.plate(12,12,44,282);this.open=this.open||new Set(['base']);if(!S.clan){if(this.side!=='server')this.side=null;}else if(!this.side)this.side='clan';
    const z=this.plate(W-48,12,36,110);this.icon(z,2,2,32,'plus',()=>this.zoom(.7),false);this.icon(z,2,38,32,'minus',()=>this.zoom(1.4),false);this.icon(z,2,74,32,'center',()=>{this.view.x=S.pos.x;this.view.z=S.pos.z;this.apply();},false);
    this.mChip=this.mPop=null;this.compass();this.paintTools();this.paintSide();this.paintMates();this.draftBar();
  },
  paintTools(){
    const b=this.rail;[...b.children].slice(1).forEach(c=>c.remove());
    TOOLS.forEach(([id,name,ic],i)=>this.icon(b,4,4+i*38,36,ic,()=>this.setTool(id),this.tool===id,name));
    D.Line(b,8,197,28,1,'rgba(255,255,255,.16)');
    this.icon(b,4,204,36,'layers',()=>{this.drawer=!this.drawer;this.top=0;this.paintTools();this.paintSide();},this.drawer?'soft':false,'Объекты');
    this.icon(b,4,242,36,'note',()=>{this.labels=this.labels===false;this.paintTools();this.declutter();},this.labels!==false?'soft':false,'Подписи');
  },
  // Полоска рисования: появляется только с выбранным инструментом.
  draftBar(){
    if(this.dBar){this.dBar.remove();this.dBar=null;}
    const d=this.draft,t=this.tool;if(t==='hand'&&!d)return;
    const type=d?d.type:t,pts=d&&d.pts&&type!=='circle'&&type!=='mark',opts=this.dests(type),many=opts.length>1;if(!opts.some(o=>o[0]===this.dest))this.dest='me';const scope=this.dest==='server'?'server':this.dest==='clan'?'clan':'me',lim=MAP_LIMITS[scope][this.limitKey(type)],ow=many?opts.reduce((a,o)=>a+o[2]+2,10):0,w=146+ow+(pts?204:72),f=this.dBar=this.plate((this.W-w)/2,12,w,36,.76);
    D.Text(f,14,0,126,36,TYPE_NAME[type]+' '+(pts?d.pts.length+'/'+MAX_POINTS:this.used(scope,type)+'/'+lim),13,C.TEXT);let x=146;
    if(many){opts.reduce((px,[v,tx,bw])=>{const on=this.dest===v;D.Box(f,px,6,bw,24,5,on?C.ACCENT:'rgba(255,255,255,.08)');D.Text(f,px,6,bw,24,tx,12,on?C.ON:C.TEXT,'c');D.Hit(f,px,6,bw,24,()=>{this.dest=v;this.draftBar();});return px+bw+2;},x);x+=ow;}
    const stop=()=>{this.draft=null;this.setTool('hand');};
    if(pts){UI.Button(f,x,5,62,26,'Назад','secondary',null,()=>this.undo(),12);UI.Button(f,x+66,5,66,26,'Отмена','secondary',null,stop,12);UI.Button(f,x+136,5,60,26,'Готово','primary',null,()=>this.finish(),12);}
    else UI.Button(f,x,5,64,26,'Отмена','secondary',null,stop,12);
  },
  // Карточка объекта: всегда у правого края по центру высоты, короткая, правка по кнопке.
  paintProps(){
    const o=this.sel;if(this.pp){this.pp.remove();this.pp=null;}if(!o)return;
    if(this.matesOpen){this.matesOpen=false;this.paintMates();}
    const ed=this.canEdit(o),opts=this.dests(o.type),many=opts.length>1,edit=ed&&(this.pe||this.isNew),w=244,h=edit?46+40+(o.type==='mark'?134+88:88)+(many?52:0)+(o.clan?54:0)+24+42:128,f=this.pp=this.plate(this.W-12-w,(this.H-h)/2,w,h,.84);f.style.zIndex=20;
    D.Cap(f,12,10,w-50,TYPE_NAME[o.type]);const ci=D.Icon(f,w-28,10,16,'close',C.DIM);D.Hit(f,w-34,4,30,28,()=>this.close(),v=>ci.style.color=v?C.TEXT:C.DIM);
    let y=34;
    if(edit){
      const i=UI.Input(f,12,y,w-24,34,'Название',32,o.name);i.input.oninput=()=>{o.name=i.input.value;this.refresh();};if(this.isNew){i.input.focus();i.input.select();}y+=44;
      if(o.type==='mark'){D.Cap(f,12,y,w-24,'Значок');y+=20;MARK_ICONS.forEach((ic,n)=>{const bx=12+(n%8)*28,cy=y+Math.floor(n/8)*27,on=o.icon===ic;D.Box(f,bx,cy,24,24,5,on?o.color:'rgba(255,255,255,.08)');D.Icon(f,bx+5,cy+5,14,ic,on?'#0E0D0C':C.TEXT);D.Hit(f,bx,cy,24,24,()=>{o.icon=ic;this.refresh();this.paintProps();});});y+=114;}
      D.Cap(f,12,y,w-24,'Цвет');y+=20;COLORS.forEach((c,n)=>{const bx=12+(n%6)*37,cy=y+Math.floor(n/6)*32;if(o.color===c)D.Box(f,bx-2,cy-2,30,30,15,null,C.TEXT);D.Circle(f,bx+1,cy+1,24,c);D.Hit(f,bx-2,cy-2,30,30,()=>{o.color=c;this.refresh();this.paintProps();});});y+=70;
      if(many){D.Cap(f,12,y,w-24,'Кому видно');y+=20;const bw=Math.floor((w-24-(opts.length-1)*4)/opts.length);opts.forEach(([v,t],n)=>{const bx=12+n*(bw+4),on=this.scopeOf(o)===v;D.Box(f,bx,y,bw,26,5,on?C.ACCENT:'rgba(255,255,255,.08)');D.Text(f,bx,y,bw,26,opts.length>2&&v==='me'?'Мне':t,12,on?C.ON:C.TEXT,'c');D.Hit(f,bx,y,bw,26,()=>{o.clan=v==='clan';o.server=v==='server';o.cat=o.clan?(o.cat||S.clan.cats[0].id):null;this.refresh();this.paintProps();});});y+=32;}
      if(o.clan){D.Cap(f,12,y,w-24,'Категория');y+=20;const cs=S.clan.cats;UI.Dropdown(f,12,y,w-24,cs.map(c=>c.name),Math.max(0,cs.findIndex(c=>c.id===o.cat)),28,n=>{o.cat=cs[n].id;this.curCat=o.cat;this.refresh();});y+=34;}
      D.Text(f,12,y,w-24,20,Fit(this.measure(o),w-24,12),12,C.DIM);y+=24;
      UI.Button(f,12,y,40,32,null,'danger','trash',()=>this.remove(o));UI.Button(f,60,y,w-72,32,'Готово','primary',null,()=>this.close(),13);
    }else{
      D.Text(f,12,y-2,w-24,24,Fit(o.name,w-24,16),16,C.TEXT);D.Text(f,12,y+22,w-24,18,Fit(this.measure(o)+' · '+this.dist(...this.anchor(o))+' от вас',w-24,12),12,C.DIM);D.Text(f,12,y+40,w-24,18,Fit((o.server?'Всем игрокам':o.clan?this.catOf(o).name+' · клан':'Только вам')+' · '+o.author,w-24,12),12,C.DIM2);
      let bx=8;const put=(ic,fn,tip)=>{this.icon(f,bx,94,28,ic,fn,false,tip);bx+=32;};
      if(ed)put('edit',()=>{this.pe=true;this.paintProps();},'Изменить');
      put('center',()=>{this.centerOn(o);this.paintProps();},'Показать на карте');this.vis(f,bx,94,28,o);bx+=32;
      if(ed)put('trash',()=>this.remove(o),'Удалить');
    }
  },
  // Режимы показа по кругу: скрыт, 2D, 3D. У зон, кругов и маршрутов 3D нет. Настройка своя у каждого игрока, в том числе для клановых и серверных меток.
  cycle(o){
    if(o.hidden){o.hidden=false;o.d3=false;}
    else if(o.d3){o.d3=false;o.hidden=true;}
    else if(o.type!=='mark')o.hidden=true;
    else if(S.objects.filter(x=>x.d3).length>=D3_MAX){Toast('Одновременно не больше '+D3_MAX+' 3D меток','err');o.hidden=true;}
    else o.d3=true;
    this.refresh();if(this.sel===o)this.paintProps();
  },
  // Кнопка режима: зачёркнутый глаз, 2D или 3D.
  vis(p,x,y,s,o){
    const bg=D.Box(p,x,y,s,s,6,'transparent');
    if(o.hidden)D.Icon(p,x+(s-16)/2,y+(s-16)/2,16,'eye_off',C.DIM);else if(o.type!=='mark')D.Icon(p,x+(s-16)/2,y+(s-16)/2,16,'eye',C.TEXT);else D.Text(p,x,y,s,s,o.d3?'3D':'2D',12,o.d3?C.ACCENT:C.TEXT,'c');
    D.Hit(p,x,y,s,s,()=>this.cycle(o),v=>{bg.style.background=v?'rgba(255,255,255,.12)':'transparent';});
  },
  // Список объектов: открывается кнопкой, высота по содержимому. С кланом две вкладки, без клана только личное.
  listRows(){
    const rows=[];
    if(this.side==='clan'&&S.clan)S.clan.cats.forEach(c=>{const l=S.objects.filter(o=>o.clan&&this.catOf(o)===c);rows.push({c,n:l.length});if(this.open.has(c.id))l.forEach(o=>rows.push({o,ind:14}));});
    else if(this.side==='server')S.objects.filter(o=>o.server).forEach(o=>rows.push({o,ind:0}));
    else S.objects.filter(o=>!o.clan&&!o.server).reverse().forEach(o=>rows.push({o,ind:0}));
    return rows;
  },
  paintSide(){
    if(this.leftP){this.leftP.remove();this.leftP=null;}this.mList=null;if(!this.drawer)return;
    const clanSide=this.side==='clan'&&!!S.clan,key=clanSide?'clan':this.side==='server'?'server':'mine',cur=this.side||'me',btn=clanSide&&this.access(),rows=this.listRows(),max=Math.floor((this.H-24-44-(btn?36:8))/36);this.top=Math.min(this.top||0,Math.max(0,rows.length-max));
    const shown=Math.max(1,Math.min(rows.length,max)),h=44+shown*36+(btn?36:8),l=this.leftP=this.plate(64,12,244,h,.76);
    (S.clan?[['clan','Клан'],['me','Мои'],['server','Сервер']]:[['me','Мои'],['server','Сервер']]).forEach(([v,t],i)=>{const x=8+i*58,on=cur===v;D.Box(l,x,8,55,28,6,on?C.ACCENT:'rgba(255,255,255,.08)');D.Text(l,x,8,55,28,t,12,on?C.ON:C.TEXT,'c');D.Hit(l,x,8,55,28,()=>{this.side=v;this.top=0;this.paintSide();});});
    const vis=this.layers[key],ey=D.Icon(l,188,14,16,vis?'eye':'eye_off',vis?C.TEXT:C.DIM2);D.Hit(l,182,8,28,28,()=>{this.layers[key]=!vis;this.refresh();},v=>{if(v)ey.style.color=C.ACCENT;else ey.style.color=vis?C.TEXT:C.DIM2;});
    const cl=D.Icon(l,218,14,16,'close',C.DIM);D.Hit(l,212,8,28,28,()=>{this.drawer=false;this.paintTools();this.paintSide();},v=>cl.style.color=v?C.TEXT:C.DIM);
    const b=this.mList=D.Clip(l,0,44,244,shown*36);b.onwheel=e=>{e.preventDefault();e.stopPropagation();this.top=Math.max(0,(this.top||0)+(e.deltaY>0?1:-1));this.paintSide();};
    if(!rows.length)D.Text(b,14,0,216,36,'Пока пусто',14,C.DIM2);
    rows.slice(this.top,this.top+max).forEach((r,i)=>{if(r.c)this.catRow(b,r.c,i*36,r.n);else this.objRow(b,r.o,i*36,r.ind);});
    if(btn){const e=D.Icon(l,16,h-27,14,'plus',C.DIM),t=D.Text(l,36,h-34,190,28,'Новая категория',13,C.DIM);D.Hit(l,8,h-34,228,28,()=>this.catEdit(null),v=>{e.style.color=t.style.color=v?C.TEXT:C.DIM;});}
  },
  objRow(b,o,y,ind){
    const col=this.colorOf(o),sel=this.sel===o,x=12+ind,tw=166-ind,bg=D.Box(b,6,y+1,232,34,6,sel?'rgba(255,255,255,.14)':'transparent');if(o.hidden)bg.style.opacity=.5;
    if(o.type==='mark'){D.Circle(b,x,y+9,18,col);D.Icon(b,x+4,y+13,10,o.icon,'#0E0D0C');}else D.Icon(b,x,y+9,18,o.type==='zone'?'polygon':o.type==='circle'?'circleo':'route',col);
    D.Text(b,x+24,y,tw,36,Fit(o.name,tw,14),14,o.hidden?C.DIM2:C.TEXT);
    D.Hit(b,6,y+1,200,34,()=>{this.centerOn(o);this.select(o);},v=>{if(!sel)bg.style.background=v?'rgba(255,255,255,.08)':'transparent';});
    this.vis(b,208,y+4,28,o);
  },
  catRow(b,c,y,n){
    const on=this.open.has(c.id),bg=D.Box(b,6,y+1,232,34,6,'transparent'),acc=this.access(),tw=acc?96:118;
    D.Icon(b,12,y+13,10,on?'chevron_down':'chevron_right',C.DIM);D.Icon(b,28,y+10,16,c.icon,c.color);
    D.Text(b,52,y,tw,36,Fit(c.name,tw,14),14,c.hidden?C.DIM2:C.TEXT);D.Text(b,150,y,28,36,String(n),12,C.DIM,'r');
    D.Hit(b,6,y+1,172,34,()=>{if(on)this.open.delete(c.id);else this.open.add(c.id);this.curCat=c.id;this.paintList();},v=>{bg.style.background=v?'rgba(255,255,255,.08)':'transparent';});
    const e=D.Icon(b,214,y+11,14,c.hidden?'eye_off':'eye',c.hidden?C.DIM2:C.DIM);D.Hit(b,208,y+4,28,28,()=>{c.hidden=!c.hidden;this.refresh();},v=>e.style.color=v?C.TEXT:(c.hidden?C.DIM2:C.DIM));
    if(acc){const i=D.Icon(b,186,y+11,14,'edit',C.DIM);D.Hit(b,180,y+4,28,28,()=>this.catEdit(c),v=>i.style.color=v?C.TEXT:C.DIM);}
  },
  paintList(){this.paintSide();},
  // Кто в сети: маленький значок со счётчиком, список по клику.
  paintMates(){
    if(this.mChip){this.mChip.remove();this.mChip=null;}if(this.mPop){this.mPop.remove();this.mPop=null;}if(!S.clan)return;
    const W=this.W,n=S.mates.length,on=this.layers.mates,c=this.mChip=this.plate(W-154,12,98,36);D.Circle(c,12,14,8,on?C.OK:C.DIM2);D.Text(c,28,0,66,36,'В сети '+n,13,C.TEXT);D.Hit(c,0,0,98,36,()=>{this.matesOpen=!this.matesOpen;if(this.matesOpen&&this.sel)this.close();this.paintMates();});
    if(!this.matesOpen)return;
    const p=this.mPop=this.plate(W-236,54,180,44+n*30,.78);D.Text(p,12,6,100,28,'На карте',13,C.DIM);UI.Switch(p,124,8,on,v=>{this.layers.mates=v;this.refresh();this.paintMates();});
    S.mates.forEach((m,i)=>{const y=40+i*30,bg=D.Box(p,6,y,168,28,5,'transparent');D.Circle(p,14,y+10,8,C.OK);D.Text(p,30,y,84,28,Fit(m.n,84,13),13,C.TEXT);D.Text(p,104,y,62,28,this.dist(m.X,m.Z),11,C.DIM,'r');D.Hit(p,6,y,168,28,()=>{this.view.x=m.X;this.view.z=m.Z;this.apply();},v=>{bg.style.background=v?'rgba(255,255,255,.1)':'transparent';});});
  },
  // Категория клана: название, цвет, значок. Доступно главе и тем, у кого есть доступ к карте.
  catEdit(cat){
    const W=1468,H=848,w=560,h=352,v=cat?{...cat}:{name:'',color:COLORS[0],icon:CAT_ICONS[0]},m=D.Frame(Shell.screen,0,0,W,H);m.style.zIndex=60;D.Line(m,0,0,W,H,'rgba(0,0,0,.6)');D.Hit(m,0,0,W,H);
    const b=D.Frame(m,(W-w)/2,(H-h)/2,w,h);b.style.boxShadow='0 16px 44px rgba(0,0,0,.6)';D.Box(b,0,0,w,h,12,C.PANEL,C.LINE2);D.Text(b,28,22,w-56,30,cat?'Категория клана':'Новая категория',22,C.TEXT);
    const i=UI.Input(b,28,64,w-56,46,'Название, например «Охота»',24,v.name);D.Cap(b,28,122,w-56,'Цвет');D.Cap(b,28,196,w-56,'Значок');const pc=D.Frame(b,28,144,w-56,46),pi=D.Frame(b,28,218,w-56,46);
    const paint=()=>{pc.innerHTML='';pi.innerHTML='';COLORS.forEach((c,n)=>{const x=n*42;if(v.color===c)D.Box(pc,x-3,3,40,40,20,null,C.TEXT);D.Circle(pc,x+1,7,32,c);D.Hit(pc,x,6,34,34,()=>{v.color=c;paint();});});CAT_ICONS.forEach((n,k)=>{const x=k*42,on=v.icon===n;D.Box(pi,x,4,36,36,8,on?C.PANEL3:C.PANEL2,on?C.ACCENT:C.LINE2);D.Icon(pi,x+8,12,20,n,on?v.color:C.DIM);D.Hit(pi,x,4,36,36,()=>{v.icon=n;paint();});});};paint();
    UI.Button(b,w-28-300,h-64,140,44,'Отмена','secondary',null,()=>m.remove());
    UI.Button(b,w-28-150,h-64,150,44,'Сохранить','primary',null,()=>{const n=i.input.value.trim();if(!n){Toast('Введите название','err');return;}if(cat)Object.assign(cat,{name:n,color:v.color,icon:v.icon});else{const c={id:'c'+Date.now(),name:n,color:v.color,icon:v.icon};S.clan.cats.push(c);this.open.add(c.id);this.curCat=c.id;}m.remove();this.refresh();this.paintSide();});
    if(cat&&cat.id!=='gen')UI.Button(b,28,h-64,140,44,'Удалить','danger','trash',()=>{S.objects.forEach(o=>{if(o.cat===cat.id)o.cat='gen';});S.clan.cats=S.clan.cats.filter(c=>c!==cat);m.remove();this.refresh();this.paintSide();});
  },
  refresh(){this.paintPins();this.paintList();this.place();},
  paintPins(){
    const p=this.pins;p.innerHTML='';this.items=[];
    const add=(o,pt,fill,icon,tag,size,fn)=>{const e=D.Frame(p,0,0,0,0);e.className='pin';if(fill){const d=D.Circle(e,-size/2,-size/2,size,fill);d.className='box dot';d.style.pointerEvents='auto';if(fn)d.onclick=v=>{if(this.tool!=='hand')return;v.stopPropagation();fn();};d.onmousedown=v=>{if(this.tool==='hand')v.stopPropagation();};d.onmouseenter=()=>{t.style.display='';e.style.zIndex=3;};d.onmouseleave=()=>{e.style.zIndex='';this.declutter();};if(icon){const k=14;D.Icon(e,-k/2,-k/2,k,icon,'#0E0D0C');}}const t=document.createElement('div');t.className='tag';t.textContent=tag;if(!fill){t.style.left='0';t.style.transform='translateX(-50%)';}e.appendChild(t);this.items.push({o,pt,e,t,size});};
    this.visible().forEach(o=>{if(o.type==='mark')add(o,o.pts[0],o.color,o.icon,o.name,22,()=>this.select(o));else add(o,null,null,null,o.name,0);});
    if(S.clan&&S.clan.atm&&this.layers.clan)add({n:'Банкомат'},[S.clan.atm.x,S.clan.atm.z],C.GOLD,'vault','Банкомат',22);
    if(S.clan&&this.layers.mates)S.mates.forEach(m=>add(m,[m.X,m.Z],C.OK,null,m.n,12));
    const me=this.me=D.Frame(p,0,0,0,0);me.className='pin';const t=D.Frame(me,-15,-15,30,30);D.Icon(t,0,0,30,'arrow','#0E0D0C');D.Icon(t,5,5,20,'arrow',C.ACCENT);t.style.transform=`rotate(${S.pos.heading}deg)`;
  },
  place(){
    if(!this.items)return;
    this.items.forEach(i=>{const w=i.pt||this.anchor(i.o),[x,y]=this.loc(w[0],w[1]);i.x=x;i.y=y;i.e.style.left=x+'px';i.e.style.top=y+'px';});
    const[x,y]=this.loc(S.pos.x,S.pos.z);this.me.style.left=x+'px';this.me.style.top=y+'px';this.meAt=[x,y];this.declutter();this.paintShapes();
  },
  // Подписи не наезжают: у каждой несколько мест, берём первое свободное, иначе прячем. Выбранный объект виден всегда.
  declutter(){
    if(!this.items||!this.meAt)return;
    const used=[{x:this.meAt[0]-15,y:this.meAt[1]-15,w:30,h:30}],free=a=>!used.some(b=>a.x<b.x+b.w+3&&b.x<a.x+a.w+3&&a.y<b.y+b.h+2&&b.y<a.y+a.h+2);
    this.items.forEach(i=>{if(i.size)used.push({x:i.x-i.size/2,y:i.y-i.size/2,w:i.size,h:i.size});});
    const rank=i=>(i.o===this.sel?2:0)+(i.size?1:0);
    [...this.items].sort((a,b)=>rank(b)-rank(a)).forEach(i=>{
      if(this.labels===false&&i.o!==this.sel){i.t.style.display='none';return;}
      const w=TextW(i.t.textContent,12)+12,h=20,s=i.size/2,spots=i.size?[[s+4,-10],[-s-4-w,-10],[-w/2,-s-4-h],[-w/2,s+4]]:[[-w/2,-10],[-w/2,-34],[-w/2,14]];
      const box=([dx,dy])=>({x:i.x+dx,y:i.y+dy,w,h}),spot=spots.find(q=>free(box(q)))||(i.o===this.sel?spots[0]:null);
      i.t.style.display=spot?'':'none';if(!spot)return;
      i.t.style.left=spot[0]+'px';i.t.style.top=spot[1]+'px';i.t.style.transform='none';used.push(box(spot));
    });
  },
  // Контур и штриховка: то, что умеет CanvasWidget.
  paintShapes(){
    if(!this.svg)return;
    const k=this.mpp(),pts=a=>a.map(p=>this.loc(p[0],p[1]).map(v=>v.toFixed(1)).join(',')).join(' '),hatch=c=>'h'+c.slice(1),cols=new Set();let s='';
    const list=this.visible().filter(o=>o.type!=='mark');
    list.forEach(o=>{
      const sel=this.sel===o,w=sel?3:2;cols.add(o.color);
      if(o.type==='zone')s+=`<polygon points="${pts(o.pts)}" fill="url(#${hatch(o.color)})" stroke="${o.color}" stroke-width="${w}"/>`;
      if(o.type==='circle'){const[c,d]=this.loc(o.pts[0][0],o.pts[0][1]);s+=`<circle cx="${c}" cy="${d}" r="${o.r/k}" fill="url(#${hatch(o.color)})" stroke="${o.color}" stroke-width="${w}"/>`;}
      if(o.type==='route')s+=`<polyline points="${pts(o.pts)}" fill="none" stroke="#0E0D0C" stroke-width="${w+3}" stroke-opacity=".55"/><polyline points="${pts(o.pts)}" fill="none" stroke="${o.color}" stroke-width="${w+1}" stroke-dasharray="12 7"/>`;
    });
    const o=this.sel;
    if(o&&o.type!=='mark'&&this.canEdit(o)){
      if(o.type==='circle'){const[c,d]=this.loc(o.pts[0][0],o.pts[0][1]);s+=`<circle cx="${c+o.r/k}" cy="${d}" r="6" fill="#fff" stroke="#0E0D0C" stroke-width="2"/>`;}
      else o.pts.forEach(p=>{const[c,d]=this.loc(p[0],p[1]);s+=`<circle cx="${c}" cy="${d}" r="6" fill="#fff" stroke="#0E0D0C" stroke-width="2"/>`;});
    }
    const d=this.draft;
    if(d){
      const col=this.dest==='clan'&&S.clan?S.clan.emblem.color:DEFAULT_ACCENT;
      if(d.type==='circle'){const[c,e]=this.loc(d.pts[0][0],d.pts[0][1]);s+=`<circle cx="${c}" cy="${e}" r="${d.r/k}" fill="none" stroke="${col}" stroke-width="2" stroke-dasharray="8 6"/>`;}
      else if(d.pts.length){const a=d.cur?[...d.pts,d.cur]:d.pts;s+=`<polyline points="${pts(a)}" fill="none" stroke="${col}" stroke-width="2" stroke-dasharray="8 6"/>`;d.pts.forEach((p,i)=>{const[c,e]=this.loc(p[0],p[1]);s+=`<circle cx="${c}" cy="${e}" r="${i?5:8}" fill="${i?'#fff':col}" stroke="#0E0D0C" stroke-width="2"/>`;});}
    }
    const defs=[...cols].map(c=>`<pattern id="${hatch(c)}" patternUnits="userSpaceOnUse" width="9" height="9" patternTransform="rotate(45)"><line x1="0" y1="0" x2="0" y2="9" stroke="${c}" stroke-width="3" stroke-opacity=".85"/></pattern>`).join('');
    this.svg.innerHTML='<defs>'+defs+'</defs>'+s;
  },
  // Нижняя полоса: координаты под курсором, лента компаса и масштаб в одну строку.
  compass(){
    const w=440,f=this.plate((this.W-w)/2,this.H-40,w,28),h=S.pos.heading,dirs=['С','СВ','В','ЮВ','Ю','ЮЗ','З','СЗ'];
    D.Icon(f,10,7,14,'gps',C.ACCENT);this.cT=D.Text(f,30,0,114,28,'',12,C.TEXT);this.coords();D.Line(f,148,6,1,16,'rgba(255,255,255,.16)');
    const t=D.Clip(f,154,0,180,28),px=2;
    for(let a=Math.ceil((h-45)/5)*5;a<=h+45;a+=5){const x=90+(a-h)*px,n=((a%360)+360)%360,mj=n%45===0;D.Line(t,x,mj?18:21,1,mj?8:5,mj?C.TEXT:C.DIM);if(mj)D.Text(t,x-16,1,32,16,dirs[n/45],11,n===0?C.ACCENT:C.TEXT,'c');}
    D.Box(t,89,16,2,12,1,C.ACCENT);D.Line(f,340,6,1,16,'rgba(255,255,255,.16)');
    this.sc=D.Text(f,348,1,86,16,'',11,C.TEXT);this.bar=D.Line(f,348,20,40,2,C.TEXT);
  },
  coords(w){const p=w||[S.pos.x,S.pos.z];this.cT.textContent='X '+Math.round(p[0])+'   Y '+Math.round(p[1]);},
};
