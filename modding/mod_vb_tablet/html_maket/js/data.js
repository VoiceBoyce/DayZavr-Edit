// Состояние макета. REAL (data_real.js) взят из конфига старого КПК; новости, диалоги и позиции придуманы для наполнения.
const S = {
  screen:'main',          // boot | auth | main | trophy | hud | off
  registered:true,
  loggedIn:true,
  adminMode:false,
  isAdmin:true,
  bat:72,
  mods:{gps:true,map:true,chat:true,gp:true},
  hudFeed:true, sounds:true,
  listening:true,
  role:3,                 // 3 лидер, 2 заместитель, 1 боец, 0 без группировки
  invite:false,           // входящее приглашение при role = 0
  factionId:'gp003',
  rank:'Атаман',
  me:{Callsign:'Voice',Avatar:12,Discord:'voiceboyce',Registered:'2026-10-24 19:12:40',DaysInZone:14,LifeSec:13920,SteamId:'76561198903710848',Joined:'2026-10-25'},
  pos:{x:4780,z:6310,heading:38},
  owner:'Kamazist',       // владелец трофейного КПК
  unread:{public:3,faction:0,news:1,pm:{Loki:2}},
  savedChar:'SurvivorM_Boris',
  curChar:'SurvivorM_Mirek',
  marksOwn:[{Id:1,Kind:3,Name:'Схрон у ручья',X:5230,Z:6020},{Id:2,Kind:4,Name:'Жарка',X:4310,Z:6940},{Id:3,Kind:5,Name:'Встреча с Диго',X:4580,Z:5880}],
  mates:[{Callsign:'Loki',X:4990,Z:6540},{Callsign:'Digo',X:5120,Z:6260},{Callsign:'Ganja',X:4410,Z:6200}],
  sos:[{Callsign:'Digo',X:5330,Z:5640,TimeMs:Date.now()-140000}],
  near:[{name:'Sipliy',dist:3,steam:'76561199000000001'},{name:'Kesary',dist:4,steam:'76561199000000002'}],
  news:[
    {Id:5,Time:'07.11 19:40',Title:'Выброс через 20 минут',Text:'Учёные с Янтаря фиксируют рост пси-фона в районе ЧАЭС. Всем сталкерам укрыться в подвалах и бункерах, рации держать включёнными.',Minutes:20},
    {Id:4,Time:'07.11 18:05',Title:'Перемирие на Баре',Text:'Торговцы объявили бар «100 Рентген» нейтральной территорией. Стрельба в радиусе 100 метров карается пулей без разговоров.',Minutes:0},
    {Id:3,Time:'07.11 12:30',Title:'Новая аномалия у насосного завода',Text:'Сталкеры сообщают о скоплении жарок на северной дороге. Обходить по лесу.',Minutes:0},
    {Id:2,Time:'06.11 21:15',Title:'ОКСОП усилил КПП',Text:'На Кордоне проверка документов, без лицензии не пропускают.',Minutes:0},
    {Id:1,Time:'06.11 09:00',Title:'Деребас принимает новичков',Text:'Староста деревни ждёт салаг: еда, вода и ночлег за помощь по хозяйству.',Minutes:0},
  ],
  dialogs:[
    {name:'Loki',Avatar:21,lines:[
      {me:false,Time:'07.11 17:58',Text:'Ты на насосной? Там опять Долг шарится.'},
      {me:true,Time:'07.11 18:01',Text:'Нет, у Бара. Что надо?'},
      {me:false,Time:'07.11 18:02',Text:'Патроны 5.45 есть? Куплю по нормальной цене.'},
      {me:false,Time:'07.11 18:03',Text:'И сигарет возьми, если будешь в Дэпо.'}]},
    {name:'Kamazist',Avatar:10,lines:[{me:false,Time:'06.11 23:10',Text:'Заходи в Деребас, надо перетереть насчёт хабара.'}]},
    {name:'Reznov VM',Avatar:33,lines:[{me:true,Time:'05.11 20:44',Text:'Переговоры по складам в силе?'}]},
  ],
  charCooldownSec:0,
  Public:[], Faction:[],
};

const CHARS = {
  men:['Mirek','Boris','Cyril','Denis','Elias','Francis','Guo','Hassan','Indar','Jose','Kaito','Lewis','Manua','Niki','Oliver','Peter','Quinn','Rolf','Seth','Taiki'],
  women:['Eva','Frida','Gabi','Helga','Irena','Judy','Keiko','Linda','Maria','Naomi','Baty'],
};

const Data = {
  faction(id){ return REAL.Factions.find(f=>f.Id===id)||null; },
  myFaction(){ return S.role>0?Data.faction(S.factionId):null; },
  relation(a,b){ const e=REAL.Relations.find(r=>(r.A===a&&r.B===b)||(r.A===b&&r.B===a)); return e?e.Rel:0; },
  relReason(a,b){ const e=REAL.Relations.find(r=>(r.A===a&&r.B===b)||(r.A===b&&r.B===a)); return e?e.Reason:''; },
  relationsOf(id){
    return REAL.Factions.filter(f=>f.Id!==id).map(f=>({f,Rel:Data.relation(id,f.Id),Reason:Data.relReason(id,f.Id)}))
      .sort((a,b)=>(b.Rel===0?-1:b.Rel)-(a.Rel===0?-1:a.Rel));
  },
  // Состав группировки вместе с игроком макета.
  members(){
    const f=Data.myFaction(); if(!f) return [];
    const list=f.Members.filter(m=>m.Callsign!==S.me.Callsign).map(m=>({...m,Role:S.role===3&&m.Role===3?2:m.Role,Online:Data.online(m.Callsign)}));
    list.unshift({Callsign:S.me.Callsign,Rank:S.rank,Role:S.role,Joined:S.me.Joined,Online:true,SteamId:S.me.SteamId,me:true});
    return list;
  },
  // Кто в сети: для наполнения, от позывного.
  online(name){ let n=0; for(const c of name) n+=c.charCodeAt(0); return n%4!==0; },
};

function initData(){
  S.Public=REAL.Public.map(l=>({...l}));
  S.Faction=[
    {Channel:1,Flags:0,Time:'07.11 16:02',Author:'Loki',Avatar:21,FactionId:'gp003',FactionName:'Свобода',FactionEmblem:41,Text:'Кто сегодня на насосной? Нужно двое на смену.',Id:9001},
    {Channel:1,Flags:0,Time:'07.11 16:05',Author:'Ganja',Avatar:44,FactionId:'gp003',FactionName:'Свобода',FactionEmblem:41,Text:'Я буду через полчаса, закину хабар на базу.',Id:9002},
    {Channel:3,Flags:0,Time:'07.11 16:20',Author:'',FactionId:'gp003',Text:'Digo вступил в группировку (приглашение Loki)',Id:0},
    {Channel:1,Flags:0,Time:'07.11 17:11',Author:'Digo',Avatar:30,FactionId:'gp003',FactionName:'Свобода',FactionEmblem:41,Text:'Видел патруль Долга у складов, четверо, с пулемётом.',Id:9003},
  ];
}
