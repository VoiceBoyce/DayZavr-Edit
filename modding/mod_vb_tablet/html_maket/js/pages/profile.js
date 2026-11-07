// «Профиль» и «Новости»: порт INT_PDA_PageProfile.c (там же класс INT_PDA_PageNews).
Pages.profile = {
  build(r,W,H){
    const host=D.Frame(r,20,0,420,H,'PreviewHost');
    Silhouette(host,0,4,420,H-14);
    const mid=D.Frame(r,450,0,480,H,'ProfMid'), inF=S.role>0, f=Data.myFaction();
    const av=D.Frame(mid,0,30,128,128,'ProfAvatar');
    D.Box(av,0,0,128,128,6,C.PANEL2,C.LINE2);
    const avImg=D.Avatar(av,4,4,120,S.me.Avatar);
    D.Line(av,4,124,120,4,C.ORANGE);
    D.Hit(mid,0,30,128,128,()=>{S.me.Avatar=(S.me.Avatar+1)%73;Shell.reload();});
    D.Text(mid,150,24,300,18,Caps('F_CALLSIGN'),13,C.ORANGE);
    D.Text(mid,148,40,340,90,Fit(S.me.Callsign,330,76),76,C.TEXT);
    D.Line(mid,0,188,480,1,C.LINE);
    if(inF){
      const row=D.Frame(mid,0,200,480,74,'ProfFaction');
      D.Emblem(row,0,4,66,f.Emblem);
      D.Text(row,88,2,390,40,Fit(CapsT(f.Name),386,32),32,C.TEXT);
      D.Text(row,88,42,390,26,Fit((S.rank?S.rank+' · ':'')+T(Look.RoleKey(S.role)),386,18),18,C.DIM);
    } else D.Text(mid,0,214,480,40,T('NO_FACTION'),20,C.DIM2);
    [['BTN_RENAME','edit'],['BTN_PASSWORD','key'],['BTN_LOGOUT','logout']].forEach(([k,ic],i)=>{
      const by=290+i*62, border=i===2?'#5A2B27':C.LINE2, tc=i===2?'#EC6A5C':C.TEXT;
      const b=D.Frame(mid,0,by,480,52,'ProfBtn'), box=D.Box(b,0,0,480,52,4,C.PANEL,border);
      D.Icon(b,20,15,22,ic,tc); D.Text(b,56,0,360,52,Caps(k),16,tc); D.Icon(b,446,18,16,'chevron_right',C.DIM2);
      D.Hit(mid,0,by,480,52,()=>this.btn(i),on=>box.style.background=on?C.PANEL2:C.PANEL);
    });
    const d=S.me.Registered, since=d.slice(8,10)+'.'+d.slice(5,7)+'.'+d.slice(0,4);
    this.stat(mid,0,484,'STAT_SINCE',since,false);
    this.stat(mid,246,484,'STAT_DAYS',String(S.me.DaysInZone),false);
    this.life=this.stat(mid,0,574,'STAT_LIFE','',false);
    this.stat(mid,246,574,'STAT_FACTION',inF?Fit(f.Name,224,30):'—',true);
    const rg=D.Frame(r,980,0,440,H,'ProfRight');
    D.Text(rg,0,30,300,18,Caps('MODULES'),13,C.DIM);
    D.Text(rg,0,50,220,70,S.bat+'%',60,C.TEXT);
    D.Line(rg,230,76,210,8,C.LINE2);
    D.Line(rg,230,76,Math.max(1,210*S.bat/100),8,S.bat<=10?C.ENEMY:C.ORANGE);
    D.Text(rg,230,90,210,20,T('BATTERY_9V')+' · ~'+Math.round(S.bat/100*50/.0025/3600)+' '+T('HOURS'),13,C.DIM,'r');
    [['GPS','MODSUB_GPS','gps','gps'],[T('MOD_MAP'),'MODSUB_MAP','map','map'],[T('MOD_CHAT'),'MODSUB_CHAT','chat','chat'],[T('MOD_GP'),'MODSUB_GP','flag','gp']].forEach(([n,sub,ic,key],m)=>{
      const on=S.mods[key], row=D.Frame(rg,0,130+m*84,440,76,'ProfMod');
      D.Box(row,0,0,440,76,4,C.PANEL,C.LINE); D.Line(row,0,0,3,76,C.ORANGE);
      D.Box(row,20,12,52,52,6,C.PANEL2); D.Icon(row,32,24,28,ic,C.ORANGE);
      D.Text(row,90,8,220,32,n.toUpperCase(),24,C.TEXT);
      D.Text(row,90,41,240,22,T(sub),15,C.DIM);
      D.Text(row,230,0,122,76,Caps(on?'MOD_ON':'MOD_OFF'),15,on?C.ALLY:C.DIM2,'r');
      UI.Switch(row,368,26,on);
    });
    D.Line(rg,0,470,440,1,C.LINE);
    this.setting(rg,480,'chat','SET_HUD_FEED',S.hudFeed,v=>S.hudFeed=v);
    this.setting(rg,522,'volume','SET_SOUNDS',S.sounds,v=>S.sounds=v);
    D.Line(rg,0,572,440,1,C.LINE);
    D.Icon(rg,0,588,22,'signal',C.ORANGE);
    D.Text(rg,36,584,404,30,T(S.mods.chat?'NET_ON':'NET_OFF'),16,C.DIM);
    D.Icon(rg,0,628,22,'user',C.ORANGE);
    D.Text(rg,36,624,404,30,Fit(T('F_DISCORD')+': '+S.me.Discord,400,16),16,C.DIM);
    Look.Reveal(mid,.06); Look.Reveal(rg,.14);
    this.t=1; this.tick(1);
  },
  setting(rg,y,icon,key,on,fn){
    const row=D.Frame(rg,0,y,440,40,'ProfSet');
    D.Icon(row,0,9,22,icon,C.ORANGE); D.Text(row,36,0,324,40,Fit(T(key),320,16),16,C.TEXT);
    UI.Switch(row,368,8,on,fn);
  },
  stat(p,x,y,key,val,accent){
    D.Line(p,x,y,2,76,accent?C.ORANGE:C.LINE2);
    D.Text(p,x+16,y,220,18,Caps(key),13,C.DIM);
    return D.Text(p,x+16,y+22,228,46,val,30,C.TEXT);
  },
  btn(i){
    if(i===0) Shell.openPrompt(T('BTN_RENAME'),T('F_CALLSIGN'),'',false,S.me.Callsign,(ok,a)=>{
      if(!ok) return;
      if(!/^[A-Za-z ]{3,20}$/.test(a)){Toast(T('R_CALLSIGN_FORMAT'),'err');return;}
      S.me.Callsign=a; Toast(T('R_OK'),'ok'); Shell.reload();
    });
    else if(i===1) Shell.openPrompt(T('BTN_PASSWORD'),T('F_OLD_PASSWORD'),T('F_NEW_PASSWORD'),true,'',(ok,a,b)=>{
      if(!ok) return;
      if(b.length<4){Toast(T('R_PASSWORD_FORMAT'),'err');return;}
      Toast(T('R_OK'),'ok');
    });
    else { S.loggedIn=false; S.registered=true; S.screen='auth'; Shell.enter('auth'); Maket.sync(); }
  },
  tick(dt){
    this.t+=dt; if(this.t<1) return; this.t=0;
    S.me.LifeSec+=1;
    const s=S.me.LifeSec; if(this.life) this.life.textContent=Math.floor(s/3600)+' '+T('H')+' '+Math.floor(s%3600/60)+' '+T('MIN');
  },
};

Pages.news = {
  build(r){
    S.unread.news=0; Shell.badges();
    const n=S.news.length;
    if(!n){ D.Text(r,40,300,1376,30,T('NEWS_NONE'),18,C.DIM2,'c'); return; }
    const top=Look.Panel(r,40,24,1376,230), f=S.news[0];
    D.Line(top,0,0,4,230,C.ORANGE);
    D.Icon(top,36,34,30,'radiation',C.ORANGE);
    D.Text(top,80,30,600,18,Caps('ZONE_NEWS'),13,C.ORANGE);
    D.Text(top,1000,30,340,18,f.Time,13,C.DIM,'r');
    D.Text(top,80,54,1260,56,Fit(CapsT(f.Title),1260,44),44,C.TEXT);
    D.MText(top,80,120,1260,90,f.Text,19,C.DIM);
    Look.Reveal(top,.04);
    const list=D.Frame(r,40,274,1376,380,'NewsList');
    S.news.slice(1,5).forEach((it,i)=>{
      const row=D.Frame(list,0,i*96,1376,86,'NewsRow');
      D.Box(row,0,0,1376,86,4,C.PANEL,C.LINE);
      D.Text(row,24,12,900,26,Fit(it.Title,900,20),20,C.TEXT);
      D.Text(row,1100,12,252,20,it.Time,13,C.DIM,'r');
      D.Text(row,24,44,1320,30,Fit(it.Text,1320,16),16,C.DIM);
      Look.Reveal(row,.08+i*.06);
    });
  },
};
