// Регистрация, вход и чужой КПК: порт INT_PDA_PageAuth.c и INT_PDA_PageTrophy (INT_PDA_Menu.c).
Pages.auth = {
  avatar:6, gridPage:0,
  build(r,W,H){
    const reg=!S.registered, L=D.Frame(r,56,0,400,H,'AuthLeft');
    if(reg) D.Img(L,0,20,300,84,'assets/logo.png');
    D.Text(L,0,118,380,18,Caps(reg?'FIRST_START':'WELCOME_BACK'),13,C.ORANGE);
    D.Text(L,0,140,400,62,Caps(reg?'REG_TITLE_1':'LOGIN_TITLE_1'),54,C.TEXT);
    D.Text(L,0,198,400,62,Caps(reg?'REG_TITLE_2':'LOGIN_TITLE_2'),54,C.TEXT);
    D.MText(L,0,272,380,84,T(reg?'REG_DESC':'LOGIN_DESC'),17,C.DIM);
    const av=D.Frame(L,0,372,200,200,'AuthAvatar');
    D.Box(av,0,0,200,200,6,C.PANEL2,C.LINE2);
    this.big=D.Avatar(av,6,6,188,reg?this.avatar:S.me.Avatar);
    D.Line(av,6,190,188,4,C.ORANGE);
    D.Text(L,220,410,180,18,Caps('AVATAR'),13,C.DIM);
    this.no=D.Text(L,220,432,180,40,'№ '+((reg?this.avatar:S.me.Avatar)+1),32,C.TEXT);
    D.Icon(L,0,604,18,'warning',C.ORANGE);
    D.MText(L,28,600,360,50,T('AUTH_NOTE'),14,C.DIM);
    const F=D.Frame(r,500,0,450,H,'AuthForm');
    let y=34;
    D.Text(F,0,y,200,18,Caps('F_CALLSIGN'),13,C.DIM);
    D.Text(F,200,y,250,18,T('F_CALLSIGN_HINT'),13,C.DIM2,'r');
    const cs=UI.Input(F,0,y+22,450,56,'',20,null,reg?'':S.me.Callsign);
    const errI=D.Icon(F,0,y+88,18,'warning',C.ENEMY), err=D.Text(F,28,y+84,422,26,'',15,C.ENEMY); errI.style.display='none';
    const setErr=s=>{err.textContent=s;errI.style.display=s?'':'none';};
    cs.input.oninput=()=>setErr(/^[A-Za-z ]*$/.test(cs.input.value)?'':T('R_CALLSIGN_FORMAT'));
    y+=132;
    D.Text(F,0,y,200,18,Caps('F_PASSWORD'),13,C.DIM);
    D.Text(F,200,y,250,18,T('F_PASSWORD_HINT'),13,C.DIM2,'r');
    const pw=UI.Pass(F,0,y+22,450,56,reg?'':'zona2026');
    y+=100;
    if(reg){
      D.Text(F,0,y,200,18,Caps('F_DISCORD'),13,C.DIM);
      D.Text(F,200,y,250,18,T('F_DISCORD_HINT'),13,C.DIM2,'r');
      UI.Input(F,0,y+22,450,56,'',40);
    }
    y+=104;
    UI.Check(F,0,y,450,T('SAVE_PASSWORD'),true);
    y+=48;
    UI.Button(F,0,y,450,66,Caps(reg?'REGISTER':'LOGIN'),'primary',null,()=>{
      if(!/^[A-Za-z ]{3,20}$/.test(cs.input.value)){setErr(T('R_CALLSIGN_FORMAT'));return;}
      if(pw.input.value.length<4){setErr(T('R_PASSWORD_FORMAT'));return;}
      if(reg){S.me.Callsign=cs.input.value;S.me.Avatar=this.avatar;S.registered=true;}
      S.loggedIn=true; S.screen='main'; Shell.enter('main'); Maket.sync();
    },18);
    y+=82;
    if(reg){
      const full=T('HAVE_ACCOUNT'), cut=full.lastIndexOf('?')+1, ask=full.slice(0,cut), link=full.slice(cut).trim();
      const aw=TextW(ask,17), lw=TextW(link,17), x=(450-aw-6-lw)/2;
      D.Text(F,x,y,aw,26,ask,17,C.DIM); D.Text(F,x+aw+6,y,lw,26,link,17,C.ORANGE);
      D.Hit(F,60,y-4,330,34,()=>{S.registered=true;Shell.reload();Maket.sync();});
    }
    D.Icon(F,110,y+46,14,'lock',C.DIM2);
    D.Text(F,132,y+42,320,22,T('DATA_NOTE'),13,C.DIM2);
    if(reg) this.grid(r);
    Look.Reveal(L,.05); Look.Reveal(F,.12);
  },
  grid(r){
    const CELL=70,GAP=10,COLS=5,ROWS=6,per=COLS*ROWS,pages=Math.ceil(73/per), g=this.g=D.Frame(r,1010,0,400,674,'AuthGrid');
    D.Text(g,0,34,260,18,Caps('PICK_AVATAR'),13,C.DIM);
    const counter=D.Text(g,260,34,140,18,'',13,C.ORANGE,'r');
    const sel=D.Frame(g,0,0,CELL+6,CELL+6,'Sel'); D.Box(sel,0,0,CELL+6,CELL+6,6,null,C.ORANGE); sel.style.zIndex=5;
    const body=D.Frame(g,0,0,400,600,'Cells');
    const py=64+ROWS*(CELL+GAP)+6;
    const pageT=D.Text(g,40,py,310,26,'',15,C.DIM,'c');
    const paint=()=>{
      body.innerHTML='';
      for(let i=0;i<per;i++){
        const idx=this.gridPage*per+i; if(idx>=73) break;
        const cx=(i%COLS)*(CELL+GAP), cy=64+Math.floor(i/COLS)*(CELL+GAP);
        D.Line(body,cx,cy,CELL,CELL,C.PANEL2); D.Avatar(body,cx,cy,CELL,idx);
        D.Hit(body,cx,cy,CELL,CELL,()=>{this.avatar=idx;paint();this.big.remove();this.big=D.Avatar(this.bigHost,6,6,188,idx);this.no.textContent='№ '+(idx+1);});
      }
      const cell=this.avatar-this.gridPage*per, on=cell>=0&&cell<per;
      sel.style.display=on?'':'none';
      if(on){sel.style.left=(cell%COLS)*(CELL+GAP)-3+'px';sel.style.top=64+Math.floor(cell/COLS)*(CELL+GAP)-3+'px';}
      pageT.textContent=(this.gridPage+1)+' / '+pages; counter.textContent=(this.avatar+1)+' / 73';
    };
    this.bigHost=this.big.parentElement;
    D.Icon(g,0,py,26,'minus',C.TEXT); D.Hit(g,0,py-4,40,34,()=>{this.gridPage=Math.max(0,this.gridPage-1);paint();});
    D.Icon(g,364,py,26,'plus',C.TEXT); D.Hit(g,350,py-4,50,34,()=>{this.gridPage=Math.min(pages-1,this.gridPage+1);paint();});
    this.gridPage=Math.floor(this.avatar/per); paint();
    Look.Reveal(g,.2);
  },
};

Pages.trophy = {
  build(r,W){
    const cx=W*.5;
    D.Icon(r,cx-32,150,64,'lock',C.ORANGE);
    D.Text(r,0,240,W,30,Caps('TROPHY_TITLE'),22,C.DIM,'c');
    D.Text(r,0,280,W,70,S.owner,56,C.TEXT,'c');
    D.Text(r,0,370,W,26,T('TROPHY_TEXT'),17,C.DIM,'c');
  },
};
