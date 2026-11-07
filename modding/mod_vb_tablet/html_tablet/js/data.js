// Состояние макета: всё придумано для наполнения. CLAN_PRICE и SERVER в моде будут настройками сервера.
const SERVER={name:'НАЗВАНИЕ СЕРВЕРА',sub:'Логотип и название из конфига'};
const DEFAULT_ACCENT='#FF7A1A';
const MAX_CLANS=20;
// Цены настроек клана, платит склад денег. В моде это конфиг.
const CLAN_PRICES={name:25000,tag:15000,emblem:20000,color:5000};
// Цена клана и общий ранг для создания: индекс в RANKS, по умолчанию «Следопыт». Из конфига.
const CLAN_PRICE=1000000, CLAN_RANK=2, CLAN_MAX=20, BANK_MAX=1000000;
// Налог клана в день, льготные дни нового клана, длина заявки. Всё из конфига.
const CLAN_TAX=5000, TAX_FREE_DAYS=7, APPLY_MAX=300;
// Чат клана: сколько сообщений хранить, длина сообщения, пауза между сообщениями в секундах.
const CHAT_MAX=100, CHAT_LEN=200, CHAT_GAP=2;
const SHAPES=['shield','crest','chevron','circle','hex','octagon','pentagon','diamond','square','banner','badge','star'];
const SYMBOLS=['crown','bolt','flame','mountain','star','cross','sword','tree','target','gem','moon','skull','wolf','paw','fish','feather','leaf','sun','snow','wave','drop','axe','hammer','spear','arrow','anchor','key','cog','castle','tower','compass','hourglass','eye','bell','heart','horseshoe'];
const COLORS=['#FF7A1A','#E9C55B','#B4D455','#6FBF8E','#2FC4B2','#5CB8F0','#8FA6FF','#B08CFF','#D66BB0','#EF5B4C','#C98A5A','#CFCAC2'];
const TAKEN=['Северный ветер','Волки','Чёрный рынок'], TAKEN_TAGS=['SVR','WLF','BLK'];
const S={
  page:'profile',
  name:'Voice', discord:'voice_dz', admin:false, invFull:false, applied:null, banned:['админ','модератор'], money:1250000, days:14, doneContracts:14, life:13920, record:40800, zombies:212, mutants:18, animals:46, dist:41800,
  pos:{x:4780,z:6310,heading:38},
  objects:[
    {id:1,type:'mark',icon:'stash',color:'#CFCAC2',name:'Схрон у ручья',pts:[[5230,6020]],clan:false,author:'Voice'},
    {id:2,type:'mark',icon:'danger',color:'#EF5B4C',name:'Опасный район',pts:[[4310,6900]],clan:false,author:'Voice'},
    {id:3,type:'zone',cat:'hunt',name:'Охотничьи угодья',color:'#6FBF8E',pts:[[3900,6480],[4250,6760],[4620,6640],[4520,6300],[4080,6200]],clan:true,author:'Loki'},
    {id:4,type:'zone',cat:'danger',name:'Стая, не соваться',color:'#EF5B4C',pts:[[5300,6720],[5720,6840],[5820,6420],[5400,6360]],clan:true,author:'Makler'},
    {id:5,type:'circle',cat:'base',name:'Наша база',color:'#2FC4B2',pts:[[5080,5880]],r:190,clan:true,author:'Makler'},
    {id:6,type:'route',cat:'base',name:'Дорога на базу',color:'#E9C55B',pts:[[4780,6310],[4840,6120],[4960,6010],[5000,5960]],clan:true,author:'Loki'},
    {id:7,type:'mark',cat:'base',icon:'rally',color:'#E9C55B',name:'Сбор у моста',pts:[[4560,6010]],clan:true,author:'Digo'},
  ],
  clan:null, invite:null, mates:[{n:'Loki',X:4990,Z:6540},{n:'Digo',X:5120,Z:6260},{n:'Makler',X:4310,Z:6120}], near:[{name:'Sipliy',dist:3},{name:'Kesary',dist:4}],
};
const INVITE={name:'Лесные',tag:'FOR',by:'Kesary',level:3,members:11,desc:'Охотимся, строимся, новичкам помогаем.',emblem:{shape:'circle',color:'#6FBF8E',symbol:'tree'}};
// Ранг: общая система, от клана не зависит. Пороги и стоимость действий в моде будут в конфиге.
const RANKS=[{name:'Новичок',need:0,c:1,st:0},{name:'Выживший',need:500,c:2,st:0},{name:'Следопыт',need:1500,c:3,st:0},{name:'Охотник',need:4000,c:1,st:1},{name:'Боец',need:10000,c:2,st:1},
  {name:'Ветеран',need:25000,c:3,st:1},{name:'Страж',need:60000,c:1,st:2},{name:'Командир',need:120000,c:2,st:2},{name:'Герой',need:250000,c:3,st:2},{name:'Легенда',need:500000,c:0,st:3}];
// Опыт общего ранга за один контракт.
const CONTRACT_XP=10;
const XP_SRC=[{icon:'star',name:'Достижения',xp:1,ach:true,get:()=>Ach.xp()},{icon:'zombie',name:'Заражённый',xp:1,get:()=>S.zombies},{icon:'crosshair',name:'Мутант',xp:10,get:()=>S.mutants},{icon:'stash',name:'Животное',xp:2,get:()=>S.animals},{icon:'route',name:'1 км пути',xp:2,get:()=>Math.floor(S.dist/1000)},{icon:'contract',name:'Контракт',xp:CONTRACT_XP,get:()=>S.doneContracts,clan:true}];
// Игроки рейтинга: имя, опыт, индекс клана в RIVALS (-1 без клана). Наполнение макета.
const PLAYERS=[['Makler',96400,0],['Loki',41250,0],['Kamazist',28300,1],['Reznov',12100,2],['Kochevnikov',9400,3],['Otec',7300,1],['Grishin',5200,-1],['Ganja',3900,4],['Digo',2600,3],['Baltazar',1800,-1],['Krest',1240,5],['Sipliy',900,-1],['Kesary',420,3],['Shans',180,-1]];
function playerList(){
  const inMine=n=>S.clan&&S.clan.members.some(m=>m.name===n),l=PLAYERS.filter(p=>p[0]!==S.name).map(([n,x,c])=>({n,x,clan:inMine(n)?S.clan:c>=0?RIVALS[c]:null}));
  l.push({n:S.name,x:Rank.xp(),clan:S.clan,me:true});
  l.sort((a,b)=>b.x-a.x);l.forEach((p,i)=>p.place=i+1);return l;
}
// Достижения игрока: по три ступени, за каждую разовая награда опытом. Только личные, клановых нет. Список в конфиге.
const ACH_XP=[50,250,1000];
const ACH=[
  {id:'zombies',icon:'zombie',name:'Чистильщик',text:'Убить заражённых',steps:[100,1000,10000]},
  {id:'mutants',icon:'crosshair',name:'Гроза мутантов',text:'Убить мутантов',steps:[10,100,1000]},
  {id:'animals',icon:'stash',name:'Зверолов',text:'Добыть животных',steps:[25,250,2500]},
  {id:'km',icon:'route',name:'Дальний путь',text:'Пройти, км',steps:[50,500,5000]},
  {id:'hours',icon:'heart',name:'Живучий',text:'Рекорд жизни, часов',steps:[24,100,500]},
  {id:'days',icon:'clock',name:'Старожил',text:'Дней на сервере',steps:[7,30,180]},
  {id:'contracts',icon:'contract',name:'Контрактник',text:'Контрактов с кланом',steps:[10,100,1000]},
  {id:'rank',icon:'rank',name:'Восхождение',text:'Ступень общего ранга',steps:[3,6,10]},
];
const Ach={
  mine(){return {zombies:S.zombies,mutants:S.mutants,animals:S.animals,km:Math.floor(S.dist/1000),hours:Math.floor(S.record/3600),days:S.days,contracts:S.doneContracts,rank:Rank.of(Rank.base())+1};},
  of(st){return {zombies:st.zombies,mutants:st.mutants,animals:st.animals,km:Math.floor(st.dist/1000),hours:Math.floor(st.record/3600),days:st.days,contracts:st.contracts,rank:Rank.of(st.xp)+1};},
  done(a,v){return a.steps.filter(s=>v>=s).length;},
  count(vals){return ACH.reduce((n,a)=>n+Ach.done(a,vals[a.id]),0);},
  total(){return ACH.reduce((n,a)=>n+a.steps.length,0);},
  xp(){const v=Ach.mine();let x=0;ACH.forEach(a=>{for(let i=0;i<Ach.done(a,v[a.id]);i++)x+=ACH_XP[i];});return x;},
  // Следующая ступень: это же цель в профиле. null, если все ступени взяты.
  goal(id,v){return ACH.find(a=>a.id===id).steps.find(s=>v<s)||null;},
};
const Rank={
  // Контракты есть только у игроков в клане.
  src(){return XP_SRC.filter(s=>S.clan||!s.clan);},
  xp(){return Rank.src().reduce((a,s)=>a+s.get()*s.xp,0);},
  // Опыт без наград за достижения: от него считается достижение за ступень ранга.
  base(){return Rank.src().filter(s=>!s.ach).reduce((a,s)=>a+s.get()*s.xp,0);},
  of(xp){let i=0;RANKS.forEach((r,k)=>{if(xp>=r.need)i=k;});return i;},
  my(){return Rank.of(Rank.xp());},
};
const CAT_ICONS=['flag','base','danger','stash','rally','zombie','car','hammer','route','crosshair','heart','fish'];
// Значки меток: игрок выбирает любой, задаёт цвет и пишет название. Фиксированных типов нет.
const MARK_ICONS=['pin','flag','base','stash','danger','rally','sos','crosshair','zombie','radiation','anomaly','car','heart','fish','axe','hammer','key','lock','star','mate','gps','clock','sym_paw','sym_tree','sym_mountain','sym_drop','sym_flame','sym_sun','sym_skull','sym_castle','sym_tower','sym_target'];
// Порог влияния для уровня: LEVELS[n-1] открывает уровень n. В моде это настройка сервера.
const LEVELS=[0,1000,2000,5000,10000];
const LEVEL_INFO=['6 обычных контрактов','1 сложный контракт вместо обычного','2 сложных контракта','3 сложных контракта','1 элитный контракт и 2 сложных'];
// Лимиты карты: зоны считаются вместе с кругами. Метки 3D у игрока: сколько включено одновременно.
const MAP_LIMITS={me:{zones:10,marks:30,routes:5},clan:{zones:20,marks:50,routes:10},server:{marks:100}},D3_MAX=15;
// Личные контракты: только у игроков в клане, три в день, сложность по уровню клана. Награда влиянием и опытом, денег нет.
const MY_POOL=[
  [{icon:'zombie',title:'Чистка района',text:'Убить заражённых',need:15,reward:10},{icon:'route',title:'Прогулка',text:'Пройти пешком, км',need:3,reward:10},{icon:'stash',title:'Добыча',text:'Добыть животных',need:3,reward:10},{icon:'fish',title:'Улов',text:'Поймать рыбу',need:5,reward:10}],
  [{icon:'route',title:'Долгая дорога',text:'Пройти пешком, км',need:10,reward:25},{icon:'crosshair',title:'Встреча с мутантами',text:'Убить мутантов',need:3,reward:25},{icon:'heart',title:'Дотянуть',text:'Прожить без смерти, часов',need:2,reward:25}],
  [{icon:'crosshair',title:'Один против стаи',text:'Убить мутантов',need:10,reward:60},{icon:'route',title:'Марафон',text:'Пройти пешком, км',need:25,reward:60}],
];
const MY_MIX=[[3,0,0],[3,0,0],[2,1,0],[2,1,0],[1,1,1]],MY_INFO=['3 обычных','3 обычных','2 обычных и 1 сложный','2 обычных и 1 сложный','1 обычный, 1 сложный, 1 элитный'];
function myContractsFor(level,demo){
  const out=[],fr=[1,.5,0];MY_MIX[level-1].forEach((n,t)=>MY_POOL[t].slice(0,n).forEach(c=>out.push({...c,tier:t})));
  return out.map((c,i)=>{const f=demo?fr[i]:0;return {...c,id:100+i,my:true,have:Math.round(c.need*f),state:demo&&i===0?'ready':'active'};});
}
const TIERS=[{name:'Обычный',color:'var(--dim)'},{name:'Сложный',color:'var(--gold)'},{name:'Элитный',color:'var(--bad)'}];
const POOL=[
  [{icon:'zombie',title:'Зачистка',text:'Убить заражённых',need:30,reward:60,money:0},{icon:'route',title:'Дальний рейс',text:'Пройти пешком, км',need:5,reward:40,money:5000},{icon:'car',title:'Механик',text:'Починить транспорт',need:3,reward:70,money:0},{icon:'fish',title:'Рыбалка',text:'Поймать рыбу',need:10,reward:30,money:2000},{icon:'crosshair',title:'Охота',text:'Добыть животных',need:5,reward:50,money:0},{icon:'axe',title:'Заготовка',text:'Срубить деревьев',need:8,reward:40,money:0}],
  [{icon:'heart',title:'Выживание',text:'Прожить без смерти, часов',need:4,reward:120,money:0},{icon:'hammer',title:'Стройка',text:'Построить элементов базы',need:6,reward:150,money:0},{icon:'crosshair',title:'Истребитель',text:'Убить мутантов',need:5,reward:200,money:10000}],
  [{icon:'heart',title:'Железная воля',text:'Прожить без смерти, часов',need:12,reward:400,money:25000},{icon:'crosshair',title:'Гроза мутантов',text:'Убить мутантов',need:25,reward:500,money:40000}],
];
// Набор на день: чем выше уровень клана, тем больше сложных заданий.
function contractsFor(level,demo){
  const mix=[[6,0,0],[5,1,0],[4,2,0],[3,3,0],[3,2,1]][level-1],out=[],fr=[1,.8,.33,.4,.75,0];
  mix.forEach((n,t)=>POOL[t].slice(0,n).forEach(c=>out.push({...c,tier:t})));
  return out.map((c,i)=>{const f=demo?fr[i]:0,have=Math.round(c.need*f);return {...c,id:i+1,have,state:demo&&i===0?'done':'active'};});
}
// Кланы рейтинга: наполнение макета.
const RIVALS=[['Волки','WLF','#EF5B4C','hex','skull',5,18,2840,16200],['Чёрный рынок','BLK','#CFCAC2','diamond','gem',5,14,2310,14750],['Железный крест','IRN','#8FA6FF','shield','cross',4,20,1640,8900],
  ['Лесные','FOR','#6FBF8E','circle','tree',3,11,960,3100],['Ночная смена','NGT','#D66BB0','square','moon',4,9,820,6400],['Дальнобойщики','TRK','#E9C55B','banner','bolt',2,7,540,1800],
  ['Рыбаки','FSH','#2FC4B2','circle','target',2,12,430,1350],['Старый завод','OLD','#FF7A1A','square','flame',3,6,310,2600],['Север','NRT','#8FA6FF','hex','mountain',1,5,120,640],['Горцы','HGH','#6FBF8E','diamond','star',1,4,60,420]]
  .map(([name,tag,color,shape,symbol,level,members,week,total])=>({name,tag,emblem:{shape,color,symbol},level,members,week,total}));
// Эмблема уникальна как пара форма + символ, цвет не считается. Вернёт клан, который её занял.
[['Стая не бросает своих','Rex',620000],['Купим и продадим всё','Baron',480000],['Крепче железа','Stalker',350000],['Лес всё помнит','Leshy',90000],['Работаем, пока вы спите','Sova',210000],['Везём что угодно','Kuzmich',45000],['Тихая вода, полный садок','Rybak',26000],['Завод живёт','Mastak',70000],['Север рядом','Gromov',12000],['Выше только небо','Aslan',4000]].forEach(([desc,leader,bank],i)=>{Object.assign(RIVALS[i],{desc,leader,bank,open:i!==1,discord:i%3===0?'discord.gg/'+RIVALS[i].tag.toLowerCase()+'clan':''});});
function emblemOwner(e){return RIVALS.find(r=>r.emblem.shape===e.shape&&r.emblem.symbol===e.symbol)||null;}
// Сколько кланов на сервере; extraClans только для проверки заполненного сервера в макете.
const clanCount=()=>RIVALS.length+(S.clan?1:0)+(S.extraClans||0);
const REWARDS=[100000,60000,30000], MEDAL=['#E9C55B','#CFCAC2','#C98A5A'];
function ratingList(mode){
  const k=mode?'total':'week',list=RIVALS.map(r=>({...r}));
  if(S.clan)list.push({name:S.clan.name,tag:S.clan.tag,emblem:S.clan.emblem,level:S.clan.level,members:S.clan.members.length,week:S.clan.weekInf,total:S.clan.influence,me:true});
  if(S.extraClans)for(let i=list.length;i<MAX_CLANS;i++)list.push({name:'Клан '+(i+1),tag:'K'+(i+1),emblem:{shape:'circle',color:'#6FBF8E',symbol:'star'},level:1,members:3,week:40-i,total:300-i,desc:'',leader:'Игрок '+(i+1),bank:20000,open:true});
  list.sort((a,b)=>b[k]-a[k]);list.forEach((r,i)=>{r.place=i+1;r.value=r[k];});return list;
}
const clanPlace=()=>ratingList(0).find(r=>r.me).place;
function setLevel(n){const c=S.clan;if(!c)return;c.level=n;c.influence=LEVELS[n]?LEVELS[n-1]+Math.round((LEVELS[n]-LEVELS[n-1])*.45):LEVELS[n-1];c.next=LEVELS[n]||LEVELS[n-1];c.contracts=contractsFor(n,true);c.my=myContractsFor(n,true);}
// Объекты тестового клана: новый клан начинает с чистой карты, выбор тестового возвращает их.
// Ранги клана: порядок в списке задаёт старшинство, первый Лидер, последний ранг новичков. Права хранятся на ранге.
const MAX_RANKS=8;
const NEWS_TAGS=[{name:'Объявление',color:'var(--accent)'},{name:'Сбор',color:'#E9C55B'},{name:'Задание',color:'#6FBF8E'},{name:'Важно',color:'#EF5B4C'}];
const PERMS=[['invite','Приглашать игроков','Звать в клан по нику и рядом'],['kick','Исключать участников','Только младших по рангу'],['assign','Назначать ранги','Выдавать младшим ранги ниже своего'],['ranks','Управлять рангами','Создавать ранги и менять доступ'],
  ['map','Вести карту клана','Создавать объекты и категории'],['bank','Работать со складом','Пополнять и снимать деньги'],['color','Настройки клана','Название, эмблема, цвет'],['news','Писать новости','Публиковать и удалять новости'],['atm','Ставить банкомат','Устанавливать и переносить банкомат']];
const allPerms=()=>Object.fromEntries(PERMS.map(p=>[p[0],true]));
const defaultRanks=()=>[{id:'leader',name:'Лидер',color:'#FF7A1A',perms:allPerms()},{id:'deputy',name:'Заместитель',color:'#E9C55B',perms:{invite:true,kick:true,assign:true,map:true,news:true}},{id:'member',name:'Участник',color:'#CFCAC2',perms:{}}];
const Clan={
  me(){return S.clan.members.find(m=>m.me);},
  rank(m){return S.clan.ranks.find(r=>r.id===m.rank)||S.clan.ranks[S.clan.ranks.length-1];},
  can(p){return !!S.clan&&!!Clan.rank(Clan.me()).perms[p];},
  level(m){return S.clan.ranks.indexOf(Clan.rank(m));},
  over(m){return Clan.level(Clan.me())<Clan.level(m);},
  isLeader(){return Clan.rank(Clan.me()).id==='leader';},
  // Менять можно только ранги младше своего, у Лидера любые.
  edits(k){return Clan.can('ranks')&&(Clan.isLeader()||S.clan.ranks.indexOf(k)>Clan.level(Clan.me()));},
  // Самое старшее место, куда игрок может поставить ранг.
  topRank(){return Clan.isLeader()?1:Clan.level(Clan.me())+1;},
  // Сколько полных дней налога оплачено. null означает льготный срок.
  taxDays(){const c=S.clan;return c.taxFree>0?null:Math.floor(c.bank/CLAN_TAX);},
  // Налог за день. Когда на складе становится 0, клан распускается.
  payTax(){
    const c=S.clan;if(c.taxFree>0){c.taxFree--;return 'free';}
    const got=Math.min(c.bank,CLAN_TAX);c.bank-=got;c.history.unshift(['Система','Налог клана',-got,Pages.csettings.now()]);c.week[(new Date().getDay()+6)%7]-=got;c.journal.unshift(['coin','Налог клана','сейчас','','−'+Num(got)]);
    if(c.bank<=0){Clan.dissolve();return 'dissolved';}
    return 'paid';
  },
  // Роспуск: удаляется всё, что связано с кланом. Эмблема и название освобождаются, личный ранг игрока остаётся.
  dissolve(){S.objects=S.objects.filter(o=>!o.clan);S.clan=null;},
  accept(r){
    const c=S.clan;if(c.members.length>=CLAN_MAX){Toast('В клане нет свободных мест','err');return false;}
    c.members.push({name:r.name,role:1,rank:'member',today:0,total:0,online:false,last:'только что',discord:r.discord});c.journal.unshift(['users',r.name+' вступил в клан','сейчас',r.name]);c.requests=c.requests.filter(q=>q!==r);Toast(r.name+' принят в клан');return true;
  },
  // Сообщение в чат клана: старые удаляются сверх лимита.
  say(who,text){const c=S.clan,d=new Date(),p=v=>String(v).padStart(2,'0');c.chat.push({who,text,day:dayLabel(0),time:p(d.getHours())+':'+p(d.getMinutes())});while(c.chat.length>CHAT_MAX)c.chat.shift();},
  // Банкомат клана: один на клан, ставится и переносится бесплатно. В игре это голограмма, здесь сразу на месте игрока.
  placeAtm(){const c=S.clan,moved=!!c.atm;c.atm={x:S.pos.x,z:S.pos.z};c.journal.unshift(['coin',S.name+(moved?' перенёс банкомат':' установил банкомат'),'сейчас',S.name]);return moved;},
  // Запрещённое слово в названии или теге, пустая строка если всё чисто.
  banned(s){const t=s.toLowerCase();return S.banned.find(w=>t.includes(w))||'';},
  // Зачисление на склад в пределах лимита. Возвращает, сколько поместилось: остальное не зачисляется.
  deposit(who,what,sum){
    const c=S.clan,got=Math.min(sum,Math.max(0,BANK_MAX-c.bank));
    if(got>0){c.bank+=got;c.history.unshift([who,what,got,Pages.csettings.now()]);c.week[(new Date().getDay()+6)%7]+=got;}
    return got;
  },
};
// Вклад по дням недели с понедельника, сегодняшний день равен m.today. Наполнение макета.
function memberWeek(m){
  let n=0;for(const ch of m.name)n+=ch.charCodeAt(0);
  const today=(new Date().getDay()+6)%7;
  return [0,1,2,3,4,5,6].map(i=>i>today?0:i===today?m.today:Math.round(m.total/40*(((n+i*i*3+i)%7)+1)/7));
}
// Показатели участника. У других наполнение макета, в моде их присылает сервер только соклановцам.
// Discord участника: свой хранится у игрока, чужой приходит с сервера только соклановцам.
// Ссылка на Discord-сервер клана: только приглашения Discord, приводятся к виду discord.gg/код. Иначе null.
function clanInvite(v){const m=v.trim().match(/^(?:https?:\/\/)?(?:www\.)?(?:discord\.gg|discord(?:app)?\.com\/invite)\/([A-Za-z0-9-]{2,32})\/?$/i);return m?'discord.gg/'+m[1]:null;}
function memberDiscord(m){return m.me?S.discord:(m.discord||'');}
// Проверка имени пользователя Discord: пустая строка значит, что всё верно.
function discordError(v){
  if(v.length<2||v.length>32)return 'Нужно от 2 до 32 символов';
  if(!/^[a-z0-9_.]+$/.test(v))return 'Только латиница, цифры, точка и подчёркивание';
  if(v.includes('..'))return 'Две точки подряд нельзя';
  return '';
}
// Типы записей журнала: по значку записи.
const JOURNAL_KINDS=[['users','Участники','users',C.ACCENT],['contract','Контракты','contract',C.INF],['bank','Склад','vault',C.GOLD],['clan','Клан','flag','#CFCAC2']];
const MONTHS_GEN=['января','февраля','марта','апреля','мая','июня','июля','августа','сентября','октября','ноября','декабря'];
// Подпись дня записи: сегодня, вчера, дальше дата.
function dayLabel(i){if(i===0)return 'сегодня';if(i===1)return 'вчера';const d=new Date(Date.now()-i*864e5);return d.getDate()+' '+MONTHS_GEN[d.getMonth()];}
function journalKind(ic){return ['users','kick','rank','logout','invite'].includes(ic)?'users':ic==='check'?'contract':ic==='coin'?'bank':'clan';}
// Только для макета: полный состав, чтобы проверить прокрутку.
function demoFull(c){
  ['Sipliy','Kesary','Orlan','Dobryak','Grach','Vityaz','Filin','Bars','Sever','Kuzya','Tundra','Medved','Yakut','Zorro'].forEach((name,i)=>{
    if(c.members.length<CLAN_MAX)c.members.push({name,role:1,rank:'member',today:i%3?0:12+i*4,total:400-i*22,online:i%4===0,last:i%4===0?'сейчас':(i+2)+' дня назад',discord:i%2?'':name.toLowerCase()+'_dz'});
  });
}
// Доли контракта: каждому его часть работы. В моде сервер считает прогресс по участникам, здесь доли условные.
function contractShares(k){
  const c=S.clan,who=c.members.filter(m=>m.online||m.today>0),list=who.length?who:[Clan.me()],w=list.map(m=>{let n=k.id*7;for(const ch of m.name)n+=ch.charCodeAt(0);return 1+n%5;}),sum=w.reduce((a,b)=>a+b,0),out=list.map((m,i)=>[m,Math.floor(k.reward*w[i]/sum)]);
  out[0][1]+=k.reward-out.reduce((a,x)=>a+x[1],0);
  return out;
}
function memberStats(m){
  if(m.me)return {days:S.days,zombies:S.zombies,mutants:S.mutants,animals:S.animals,dist:S.dist,life:S.life,record:S.record,contracts:S.doneContracts,week:memberWeek(m),xp:Rank.xp()};
  let n=0;for(const ch of m.name)n+=ch.charCodeAt(0);
  const zombies=Math.round(90+m.total*.24+n%40),mutants=Math.round(m.total/70+n%7),animals=Math.round(30+m.total/40+n%11),dist=Math.round(30000+m.total*35+n*40),contracts=Math.round(m.total/60),life=1800+n*37%20000;
  return {days:3+n%90,zombies,mutants,animals,dist,life,record:life+n*53%30000,contracts,week:memberWeek(m),xp:(PLAYERS.find(p=>p[0]===m.name)||[0,zombies+mutants*10+animals*2+Math.floor(dist/1000)*2+contracts*10])[1]};
}
const DEMO_OBJ=JSON.stringify(S.objects.filter(o=>o.clan));
function demoClan(role){
  if(!S.objects.some(o=>o.clan))S.objects.push(...JSON.parse(DEMO_OBJ));
  const c=makeDemoClan(role),rk=defaultRanks();rk.splice(2,0,{id:'scout',name:'Разведчик',color:'#6FBF8E',perms:{map:true}});c.ranks=rk;c.members.forEach(m=>{m.rank=m.rank||{3:'leader',2:'deputy',1:'member'}[m.role];});const dc={Makler:'makler_north',Loki:'loki.svr',Digo:'digo_777',Baltazar:'baltazar.dz'};c.members.forEach(m=>{m.discord=dc[m.name]||'';});return c;
}
function makeDemoClan(role){
  const me={name:S.name,role,today:96,total:2140,online:true,last:'сейчас',me:true};
  return {
    name:'Северный ветер',tag:'SVR',desc:'Север держит тех, кто держит слово',
    emblem:{shape:'shield',color:'#2FC4B2',symbol:'mountain'},level:3,influence:2340,next:LEVELS[3],bank:412500,created:'24.10.2026',taxFree:0,open:true,discord:'discord.gg/severwind',atm:{x:4620,z:6240},
    chatUnread:3,chat:[{who:'Loki',text:'Кто сегодня вечером в сети?',day:dayLabel(1),time:'19:42'},{who:'Digo',text:'Буду после девяти, могу взять машину',day:dayLabel(1),time:'19:50'},{who:S.name,text:'Я тоже, встречаемся на базе',day:dayLabel(1),time:'19:58'},
      {who:'Ganja',text:'Закинул патроны в общий ящик',day:dayLabel(0),time:'11:20'},{who:'Loki',text:'Отлично, спасибо',day:dayLabel(0),time:'11:24'},{who:'Loki',text:'У моста замечены мутанты, в одиночку туда не ходите. Лучше собраться втроём и зайти со стороны озера.',day:dayLabel(0),time:'11:26'},{who:S.name,text:'Понял, поставлю метку на карте',day:dayLabel(0),time:'11:31'},
      {who:'Digo',text:'Контракт на машину почти закрыт, осталось одно колесо',day:dayLabel(0),time:'13:05'},{who:'Digo',text:'Кто рядом с гаражом?',day:dayLabel(0),time:'13:06'},{who:'Loki',text:'Иду, буду минут через пять',day:dayLabel(0),time:'13:09'}],
    requests:[{name:'Sipliy',discord:'sipliy_dz',text:'Играю вечерами, в основном на севере. Ищу клан для контрактов и совместных вылазок, есть машина.',time:'07.11 16:02',xp:900},{name:'Shans',discord:'',text:'Новичок, но быстро учусь. Хочу в клан, где помогают разобраться.',time:'07.11 11:40',xp:180}],
    members:[role===3?me:{name:'Makler',role:3,today:64,total:3120,online:true,last:'сейчас'},role===2?me:{name:'Loki',role:2,today:120,total:2860,online:true,last:'сейчас'},
      role===1?me:{name:'Ganja',role:1,today:45,total:1490,online:false,last:'2 ч назад'},{name:'Digo',role:1,rank:'scout',today:30,total:980,online:true,last:'сейчас'},{name:'Baltazar',role:1,today:0,total:760,online:false,last:'вчера'},{name:'Krest',role:1,today:0,total:430,online:false,last:'3 дня назад'}],
    journal:[['rank','Digo получил ранг «Разведчик»',dayLabel(0)+', 17:30','Digo'],['users','Digo вступил в клан',dayLabel(0)+', 17:11','Digo'],['check','Контракт «Зачистка» выполнен',dayLabel(0)+', 16:40','','+60'],['coin','Loki пополнил склад',dayLabel(0)+', 15:02','Loki','+25 000'],['news','Новость: Рейд на заражённых у Каменки',dayLabel(0)+', 12:10'],['pin','На карте новое: Стая',dayLabel(0)+', 10:36'],['coin','Налог клана',dayLabel(0)+', 00:00','','−5 000'],
      ['star','Клан достиг 3 уровня',dayLabel(1)+', 22:18'],['coin','Makler снял со склада',dayLabel(1)+', 21:40','Makler','−40 000'],['kick','Sanya исключён из клана',dayLabel(1)+', 19:30','Sanya'],['check','Контракт «Стройка» выполнен',dayLabel(1)+', 18:12','','+80'],['coin','Налог клана',dayLabel(1)+', 00:00','','−5 000'],
      ['coin','Ganja пополнил склад',dayLabel(2)+', 19:12','Ganja','+8 000'],['coin','Digo пополнил склад',dayLabel(2)+', 13:25','Digo','+15 000'],['check','Контракт «Охота» выполнен',dayLabel(2)+', 11:03','','+40'],
      ['sym_cog','Изменены настройки: цвет',dayLabel(3)+', 23:40'],['users','Krest вступил в клан',dayLabel(3)+', 20:14','Krest'],['users','Baltazar вступил в клан',dayLabel(5)+', 18:02','Baltazar']],
    cats:[{id:'gen',name:'Общее',color:'#CFCAC2',icon:'flag'},{id:'base',name:'База',color:'#2FC4B2',icon:'base'},{id:'hunt',name:'Охота',color:'#6FBF8E',icon:'crosshair'},{id:'danger',name:'Опасно',color:'#EF5B4C',icon:'danger'}],
    newsUnread:2,news:[
      {id:4,tag:0,title:'Сбор клана в субботу',text:'В субботу в 20:00 общий сбор на базе. Обсудим склад, контракты и кто идёт на север. Быть всем, кто в сети.',author:'Makler',time:'07.11 18:20',pin:true},
      {id:3,tag:1,title:'Рейд на заражённых у Каменки',text:'Нужно шесть человек с патронами, выходим от базы.',author:'Loki',time:'07.11 12:10'},
      {id:2,tag:2,title:'Контракт «Стройка» почти закрыт',text:'Осталось одно строение, кто рядом с базой, доделайте.',author:'Loki',time:'06.11 21:40'},
      {id:1,tag:3,title:'Мутанты у озера',text:'На зоне «Стая» замечены мутанты, в одиночку не ходить.',author:'Makler',time:'06.11 19:05'},
    ],
    contracts:contractsFor(3,true),my:myContractsFor(3,true),myReplace:1,weekInf:1180,
    history:[['Loki','Пополнение',25000,'07.11 15:02'],['Система','Налог клана',-5000,'07.11 00:00'],['Makler','Снятие',-40000,'06.11 21:40'],['Система','Контракты за день',12000,'06.11 00:00'],['Ganja','Пополнение',8000,'05.11 19:12'],['Digo','Пополнение',15000,'05.11 13:25'],['Система','Контракты за день',9000,'05.11 00:00']],
    week:[18000,24000,9000,31000,-40000,12000,25000],
  };
}
// Серверными могут быть только метки: ставит администратор, видят все, с кланом и без. Наполнение макета.
S.objects.push({id:201,type:'mark',icon:'sym_castle',color:'#6FBF8E',name:'Торговец',pts:[[4960,6010]],server:true,clan:false,author:'Администрация'},{id:202,type:'mark',icon:'heart',color:'#EF5B4C',name:'Больница',pts:[[4470,6560]],server:true,clan:false,author:'Администрация'});
