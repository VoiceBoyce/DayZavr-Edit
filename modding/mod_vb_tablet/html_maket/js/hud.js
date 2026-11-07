// HUD-слой поверх игры: порт INT_PDA_Hud.c. В макете все части показаны разом на своих местах по умолчанию.
const Hud = {
  show(){
    const h=document.getElementById('hud'); h.style.display=''; h.innerHTML='';
    const pl=S.Public.filter(l=>l.Channel===0).slice(-3);
    this.feed(h,pl); this.news(h);
    let y=40;
    y=this.card(h,y,'mail','HUD_PM_TITLE',C.ORANGE,T('HUD_PM_FROM')+' Loki!',T('HUD_PM_COUNT')+' 2');
    y=this.card(h,y,'chat','HUD_FACTION_TITLE',C.OWN,'Digo: Видел патруль Долга у складов','');
    y=this.sos(h,y);
    y=this.card(h,y,'invite','HUD_INVITE_TITLE',C.ORANGE,T('HUD_INVITE_IN')+' Чистое небо',T('HUD_INVITE_BY')+' Kochevnikov · '+T('HUD_INVITE_OPEN'));
    this.battery(h);
  },
  hide(){ const h=document.getElementById('hud'); if(h){h.style.display='none';h.innerHTML='';} },
  tick(){},
  who(l){ return l.Flags&1?T('HUD_ANON'):l.Flags&2?l.FactionName:(l.Author||T('HUD_SYSTEM')); },
  feed(h,lines){
    const W=460,k=16, rows=lines.map(l=>{const s=Hud.who(l)+': '+l.Text;return TextW(s,k)+54>W-16?2:1;});
    const H=34+rows.reduce((a,r)=>a+Math.max(36,r*22+8)+4,0)+10, b=D.Frame(h,40,40,W,H,'Feed');
    D.Box(b,0,0,W,H,6,'rgba(9,9,10,.72)'); D.Line(b,0,0,3,H,C.ORANGE);
    D.Icon(b,16,10,16,'signal',C.ORANGE); D.Text(b,40,8,W-56,20,Caps('HUD_FEED_TITLE'),13,C.ORANGE);
    let y=34;
    lines.forEach((l,i)=>{
      const rh=Math.max(36,rows[i]*22+8);
      if(l.Flags&1) Look.Anon(b,14,y,30); else D.Avatar(b,14,y,30,l.Avatar);
      const nm=Hud.who(l), nw=TextW(nm+':',k)+6, x=54, wr=W-x-14;
      D.Text(b,x,y,nw,22,nm+':',k,Look.RelColor(l.FactionId));
      const t=D.MText(b,x,y,wr,rh,'',k,C.TEXT); t.style.lineHeight='22px'; t.style.textIndent=nw+'px'; t.style.overflow='hidden'; t.style.maxHeight=rows[i]*22+'px'; t.textContent=l.Text;
      y+=rh+4;
    });
  },
  card(h,y,icon,cap,accent,r1,sub){
    const W=440,Hh=sub?100:82,x=1920-40-W, b=D.Frame(h,x,y,W,Hh,'Card');
    D.Box(b,0,0,W,Hh,6,'rgba(9,9,10,.82)'); D.Line(b,W-4,0,4,Hh,accent);
    const sq=56,sy=(Hh-sq)/2; D.Box(b,18,sy,sq,sq,8,accent); D.Icon(b,29,sy+11,34,icon,C.ON_ORANGE);
    D.Text(b,92,14,W-110,20,Caps(cap),13,accent);
    D.Text(b,92,36,W-110,24,Fit(r1,W-110,18),18,C.WHITE);
    if(sub) D.Text(b,92,64,W-110,20,Fit(sub,W-110,14),14,C.DIM);
    return y+Hh+12;
  },
  sos(h,y){
    const W=400,Hh=84,x=1920-40-W,b=D.Frame(h,x,y,W,Hh,'Sos');
    D.Box(b,0,0,W,Hh,6,'rgba(40,8,6,.80)');
    const p=D.Frame(b,0,0,W,Hh,'Pulse'); p.classList.add('blink'); D.Line(p,0,0,4,Hh,C.ENEMY); D.Icon(p,20,20,44,'sos',C.ENEMY);
    D.Text(b,84,12,W-100,30,'SOS · Digo',24,C.WHITE);
    D.Text(b,84,44,W-100,24,T('HUD_SOS_HELP')+' · 1.2 '+T('MAP_KM')+' · '+T('DIR_3'),16,'#FFB3AA');
    return y+Hh+12;
  },
  news(h){
    const W=800,Hh=72,b=D.Frame(h,(1920-W)/2,34,W,Hh,'News'), n=S.news[0];
    D.Box(b,0,0,W,Hh,6,'rgba(9,9,10,.72)'); D.Line(b,6,0,W-12,2,C.ORANGE);
    const lab=Caps('ZONE_NEWS'), lw=TextW(lab,15)+6;
    D.Text(b,22,0,lw,Hh,lab,15,C.ORANGE); const dx=22+lw+16; D.Line(b,dx,21,1,30,C.LINE2);
    const tx=dx+16, tw=W-tx-20;
    D.Text(b,tx,9,tw,28,Fit(n.Title,tw,21),21,C.WHITE);
    D.Text(b,tx,39,tw,22,Fit(n.Text,tw,16),16,C.TEXT);
  },
  battery(h){
    const s=T('HUD_BATTERY')+' 8%', lw=TextW(s,17)+6, W=56+lw, b=D.Frame(h,98,862,W,34,'Bat');
    D.Box(b,0,0,W,34,4,'rgba(9,9,10,.72)',C.ENEMY); D.Icon(b,12,6,22,'battery','#FF7A6A'); D.Text(b,42,0,lw,34,s,17,'#FF7A6A');
  },
};
