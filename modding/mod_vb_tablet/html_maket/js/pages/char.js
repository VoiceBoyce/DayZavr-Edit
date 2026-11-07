// «Персонаж»: порт INT_PDA_PageChar.c. Слева модель (в игре PlayerPreviewWidget), в центре список, справа действия.
Pages.char = {
  PREV_X:20, PREV_W:440, LIST_X:490, LIST_W:550, INFO_X:1070, INFO_W:346, ROW_PITCH:28, ROWS_MAX:20,
  sel:null,
  build(r,W,H){
    this.r=r;
    const host=D.Frame(r,this.PREV_X,0,this.PREV_W,H,'PreviewHost');
    Silhouette(host,0,4,this.PREV_W,H-14);
    if(!S.loggedIn){ this.lock(r,H); return; }
    if(!this.sel) this.sel=S.savedChar||S.curChar;
    const L=D.Frame(r,this.LIST_X,0,this.LIST_W,H,'List');
    Look.Label(L,0,24,this.LIST_W,'CHAR_TITLE');
    D.Line(L,0,48,this.LIST_W,1,C.LINE);
    this.cols=D.Frame(L,0,56,this.LIST_W,H-56,'Cols');
    const I=D.Frame(r,this.INFO_X,0,this.INFO_W,H,'Info');
    Look.Label(I,0,24,this.INFO_W,'TAB_CHAR');
    this.name=D.Text(I,0,44,this.INFO_W,64,'',52,C.TEXT);
    this.gender=D.Text(I,0,108,this.INFO_W,24,'',17,C.DIM);
    D.Line(I,0,148,this.INFO_W,1,C.LINE);
    this.now=this.stat(I,0,166,'CHAR_NOW',false);
    this.saved=this.stat(I,0,250,'CHAR_SAVED',true);
    D.MText(I,0,342,this.INFO_W,64,T('CHAR_SAVE_NOTE'),15,C.DIM);
    this.saveBtn=UI.Button(I,0,452,this.INFO_W,48,T('CHAR_SAVE'),'secondary','check',()=>this.save());
    this.rebornBtn=UI.Button(I,0,512,this.INFO_W,48,T('CHAR_RESPAWN'),'danger','refresh',()=>this.ask());
    this.wait=D.Text(I,0,568,this.INFO_W,22,'',14,C.DIM,'c');
    D.Icon(I,0,628,18,'refresh',C.DIM2);
    D.Text(I,28,624,this.INFO_W-28,26,T('CHAR_DRAG'),13,C.DIM2);
    this.rows();
    this.info();
    Look.Reveal(L,.06); Look.Reveal(I,.14);
  },
  lock(r,H){
    const X=this.LIST_X, w=this.INFO_X+this.INFO_W-X, m=D.Frame(r,X,0,w,H,'Lock');
    D.Icon(m,w*.5-32,230,64,'lock',C.DIM2);
    D.Text(m,0,314,w,30,Caps('CHAR_TITLE'),22,C.TEXT,'c');
    D.Text(m,0,354,w,24,T('NO_ACCOUNT'),16,C.DIM,'c');
  },
  stat(p,x,y,key,accent){
    D.Line(p,x,y,2,66,accent?C.ORANGE:C.LINE2);
    D.Text(p,x+16,y,this.INFO_W-16,18,Caps(key),13,C.DIM);
    return D.Text(p,x+16,y+22,this.INFO_W-16,40,'',28,C.TEXT);
  },
  title(t){ return t?T('CHR_'+t.replace(/^Survivor[MF]_/,'').toUpperCase()):T('CHAR_NONE'); },
  rows(){
    const c=this.cols, gap=20, colsM=Math.ceil(CHARS.men.length/this.ROWS_MAX), colsW=Math.ceil(CHARS.women.length/this.ROWS_MAX), cols=colsM+colsW, colW=(this.LIST_W-gap*(cols-1))/cols;
    c.innerHTML='';
    let x=0;
    [[CHARS.men,'CHAR_MEN','SurvivorM_'],[CHARS.women,'CHAR_WOMEN','SurvivorF_']].forEach(([list,key,pre])=>{
      D.Text(c,x,4,colW,20,Caps(key)+' · '+list.length,13,C.DIM);
      list.forEach((n,i)=>{
        const col=Math.floor(i/this.ROWS_MAX), cx=x+col*(colW+gap), y=36+(i%this.ROWS_MAX)*this.ROW_PITCH, t=pre+n, on=t===this.sel;
        const box=D.Box(c,cx,y,colW,26,4,on?'#22160C':'transparent',on?C.ORANGE:null);
        const tx=D.Text(c,cx+10,y,colW-34,26,T('CHR_'+n.toUpperCase()),16,on?C.ORANGE:t===S.savedChar?C.TEXT:C.DIM);
        if(t===S.savedChar) D.Icon(c,cx+colW-22,y+6,14,'check',C.ORANGE);
        D.Hit(c,cx,y,colW,26,()=>{this.sel=t;this.rows();this.info();},h=>{if(!on)tx.style.color=h?C.TEXT:(t===S.savedChar?C.TEXT:C.DIM);});
      });
      x+=(list===CHARS.men?colsM:colsW)*(colW+gap);
    });
  },
  info(){
    this.name.textContent=this.title(this.sel);
    this.gender.textContent=T(this.sel.startsWith('SurvivorF')?'CHAR_FEMALE':'CHAR_MALE');
    this.now.textContent=this.title(S.curChar);
    this.saved.textContent=this.title(S.savedChar);
    const same=this.sel===S.savedChar;
    this.saveBtn.label.textContent=T(same?'CHAR_SAVED':'CHAR_SAVE');
    this.saveBtn.classList.toggle('off',same);
    const left=S.charCooldownSec;
    this.rebornBtn.classList.toggle('off',left>0);
    this.wait.textContent=left>0?T('CHAR_WAIT').replace('%1',Math.ceil(left/60)):T('CHAR_COOLDOWN').replace('%1','10');
  },
  save(){ S.savedChar=this.sel; Toast(T('CHAR_SAVED_TOAST'),'ok'); this.rows(); this.info(); },
  ask(){
    UI.Ask(this.r,{title:T('CHAR_ASK_TITLE'),value:this.title(this.sel),text:T('CHAR_ASK_TEXT'),danger:true,cw:600,ch:244,no:T('NO'),yes:T('CHAR_RESPAWN'),yesIcon:'refresh',onYes:()=>{
      S.savedChar=this.sel; S.curChar=this.sel; S.charCooldownSec=600; Toast(T('CHAR_REBORN_TOAST'),'info'); Shell.powerOff();
    }});
  },
  tick(dt){ if(S.charCooldownSec>0){S.charCooldownSec=Math.max(0,S.charCooldownSec-dt);if(this.wait)this.info();} },
};
