class CfgPatches
{
	class KOD_Weapons_Stickers
	{
		units[]=
		{
			"KOD_Sticker_Anarchy",
			"KOD_Sticker_Bang",
			"KOD_Sticker_Cat",
			"KOD_Sticker_Cat2",
			"KOD_Sticker_Cats",
			"KOD_Sticker_Cat3",
			"KOD_Sticker_Clown",
			"KOD_Sticker_Oni",
			"KOD_Sticker_Dog",
			"KOD_Sticker_Eyes",
			"KOD_Sticker_Girl",
			"KOD_Sticker_Girl2",
			"KOD_Sticker_Girl3",
			"KOD_Sticker_Girl4",
			"KOD_Sticker_Girl5",
			"KOD_Sticker_Girl6",
			"KOD_Sticker_Girl7",
			"KOD_Sticker_Girl8",
			"KOD_Sticker_Girl9",
			"KOD_Sticker_Girl10",
			"KOD_Sticker_Harley",
			"KOD_Sticker_Heart",
			"KOD_Sticker_HelloKitty",
			"KOD_Sticker_Shpagat",
			"KOD_Sticker_Dragon",
			"KOD_Sticker_Jerry",
			"KOD_Sticker_Joker",
			"KOD_Sticker_Jordan",
			"KOD_Sticker_Lavrov",
			"KOD_Sticker_Lavrov2",
			"KOD_Sticker_Oni2",
			"KOD_Sticker_Ronin",
			"KOD_Sticker_Skull",
			"KOD_Sticker_Skull2",
			"KOD_Sticker_Skull3",
			"KOD_Sticker_Skull4",
			"KOD_Sticker_Stitch",
			"KOD_Sticker_TheEnd",
			"KOD_Sticker_Wasserman",
			"KOD_Sticker_YinYang",
			"KOD_Sticker_ZaLupoy"
		};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Data"
		};
		magazines[]={};
	};
};
class cfgVehicles
{
	class Inventory_Base;
	class KOD_Sticker_Base: Inventory_Base
	{
		scope=0;
		displayName="Наклейка";
		descriptionShort="Наклейка для оружия.";
		model="KOD_weapons\attachments\Stickers\sticker.p3d";
		rotationFlags=1;
		reversed=0;
		inventorySlot[]=
		{
			"KODSticker1",
			"KODSticker2",
			"KODSticker3"
		};
		weight=1;
		itemSize[]={1,1};
		hiddenSelections[]=
		{
			"color"
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=100;
					healthLevels[]=
					{
						
						{
							1,
							{}
						},
						
						{
							0.69999999,
							{}
						},
						
						{
							0.5,
							{}
						},
						
						{
							0.30000001,
							{}
						},
						
						{
							0,
							{}
						}
					};
				};
			};
			class GlobalArmor
			{
				class Projectile
				{
					class Health
					{
						damage=0;
					};
					class Blood
					{
						damage=0;
					};
					class Shock
					{
						damage=0;
					};
				};
				class Melee
				{
					class Health
					{
						damage=0;
					};
					class Blood
					{
						damage=0;
					};
					class Shock
					{
						damage=0;
					};
				};
				class Infected
				{
					class Health
					{
						damage=0;
					};
					class Blood
					{
						damage=0;
					};
					class Shock
					{
						damage=0;
					};
				};
				class FragGrenade
				{
					class Health
					{
						damage=0;
					};
					class Blood
					{
						damage=0;
					};
					class Shock
					{
						damage=0;
					};
				};
			};
		};
	};
	class KOD_Sticker_Anarchy: KOD_Sticker_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\attachments\Stickers\data\sticker_anarchy_ca.paa"
		};
	};
	class KOD_Sticker_Bang: KOD_Sticker_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\attachments\Stickers\data\sticker_bang_ca.paa"
		};
	};
	class KOD_Sticker_Cat: KOD_Sticker_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\attachments\Stickers\data\sticker_cat_ca.paa"
		};
	};
	class KOD_Sticker_Cat2: KOD_Sticker_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\attachments\Stickers\data\sticker_cat2_ca.paa"
		};
	};
	class KOD_Sticker_Cats: KOD_Sticker_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\attachments\Stickers\data\sticker_cats_ca.paa"
		};
	};
	class KOD_Sticker_Cat3: KOD_Sticker_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\attachments\Stickers\data\sticker_catwpacket_ca.paa"
		};
	};
	class KOD_Sticker_Clown: KOD_Sticker_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\attachments\Stickers\data\sticker_clown_ca.paa"
		};
	};
	class KOD_Sticker_Oni: KOD_Sticker_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\attachments\Stickers\data\sticker_cyberpunk_ca.paa"
		};
	};
	class KOD_Sticker_Dog: KOD_Sticker_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\attachments\Stickers\data\sticker_dog_ca.paa"
		};
	};
	class KOD_Sticker_Eyes: KOD_Sticker_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\attachments\Stickers\data\sticker_eyes_ca.paa"
		};
	};
	class KOD_Sticker_Girl: KOD_Sticker_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\attachments\Stickers\data\sticker_girl_ca.paa"
		};
	};
	class KOD_Sticker_Girl2: KOD_Sticker_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\attachments\Stickers\data\sticker_girl2_ca.paa"
		};
	};
	class KOD_Sticker_Girl3: KOD_Sticker_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\attachments\Stickers\data\sticker_girl3_ca.paa"
		};
	};
	class KOD_Sticker_Girl4: KOD_Sticker_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\attachments\Stickers\data\sticker_girl4_ca.paa"
		};
	};
	class KOD_Sticker_Girl5: KOD_Sticker_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\attachments\Stickers\data\sticker_girl5_ca.paa"
		};
	};
	class KOD_Sticker_Girl6: KOD_Sticker_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\attachments\Stickers\data\sticker_girl6_ca.paa"
		};
	};
	class KOD_Sticker_Girl7: KOD_Sticker_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\attachments\Stickers\data\sticker_girl7_ca.paa"
		};
	};
	class KOD_Sticker_Girl8: KOD_Sticker_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\attachments\Stickers\data\sticker_girl8_ca.paa"
		};
	};
	class KOD_Sticker_Girl9: KOD_Sticker_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\attachments\Stickers\data\sticker_girl9_ca.paa"
		};
	};
	class KOD_Sticker_Girl10: KOD_Sticker_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\attachments\Stickers\data\sticker_girlpanties_ca.paa"
		};
	};
	class KOD_Sticker_Harley: KOD_Sticker_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\attachments\Stickers\data\sticker_harley_ca.paa"
		};
	};
	class KOD_Sticker_Heart: KOD_Sticker_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\attachments\Stickers\data\sticker_heart_ca.paa"
		};
	};
	class KOD_Sticker_HelloKitty: KOD_Sticker_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\attachments\Stickers\data\sticker_hellokitty_ca.paa"
		};
	};
	class KOD_Sticker_Shpagat: KOD_Sticker_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\attachments\Stickers\data\sticker_idk_ca.paa"
		};
	};
	class KOD_Sticker_Dragon: KOD_Sticker_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\attachments\Stickers\data\sticker_japandragon_ca.paa"
		};
	};
	class KOD_Sticker_Jerry: KOD_Sticker_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\attachments\Stickers\data\sticker_jerry_ca.paa"
		};
	};
	class KOD_Sticker_Joker: KOD_Sticker_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\attachments\Stickers\data\sticker_joker_ca.paa"
		};
	};
	class KOD_Sticker_Jordan: KOD_Sticker_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\attachments\Stickers\data\sticker_jordan_ca.paa"
		};
	};
	class KOD_Sticker_Lavrov: KOD_Sticker_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\attachments\Stickers\data\sticker_lavrov_ca.paa"
		};
	};
	class KOD_Sticker_Lavrov2: KOD_Sticker_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\attachments\Stickers\data\sticker_lavrov2_ca.paa"
		};
	};
	class KOD_Sticker_Oni2: KOD_Sticker_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\attachments\Stickers\data\sticker_oni_ca.paa"
		};
	};
	class KOD_Sticker_Ronin: KOD_Sticker_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\attachments\Stickers\data\sticker_ronin_ca.paa"
		};
	};
	class KOD_Sticker_Skull: KOD_Sticker_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\attachments\Stickers\data\sticker_skull_ca.paa"
		};
	};
	class KOD_Sticker_Skull2: KOD_Sticker_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\attachments\Stickers\data\sticker_skull2_ca.paa"
		};
	};
	class KOD_Sticker_Skull3: KOD_Sticker_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\attachments\Stickers\data\sticker_skull3_ca.paa"
		};
	};
	class KOD_Sticker_Skull4: KOD_Sticker_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\attachments\Stickers\data\sticker_skull4_ca.paa"
		};
	};
	class KOD_Sticker_Stitch: KOD_Sticker_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\attachments\Stickers\data\sticker_stitch_ca.paa"
		};
	};
	class KOD_Sticker_TheEnd: KOD_Sticker_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\attachments\Stickers\data\sticker_theend_ca.paa"
		};
	};
	class KOD_Sticker_Wasserman: KOD_Sticker_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\attachments\Stickers\data\sticker_wasserman_ca.paa"
		};
	};
	class KOD_Sticker_YinYang: KOD_Sticker_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\attachments\Stickers\data\sticker_yinyang_ca.paa"
		};
	};
	class KOD_Sticker_ZaLupoy: KOD_Sticker_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\attachments\Stickers\data\sticker_zalupoy_ca.paa"
		};
	};
};
class CfgNonAIVehicles
{
	class ProxyAttachment;
	class ProxyStickerproxy1: ProxyAttachment
	{
		scope=0;
		inventorySlot="KODSticker1";
		model="KOD_weapons\attachments\Stickers\proxies\stickerproxy1.p3d";
	};
	class ProxyStickerproxy2: ProxyAttachment
	{
		scope=0;
		inventorySlot="KODSticker2";
		model="KOD_weapons\attachments\Stickers\proxies\stickerproxy2.p3d";
	};
	class ProxyStickerproxy3: ProxyAttachment
	{
		scope=0;
		inventorySlot="KODSticker3";
		model="KOD_weapons\attachments\Stickers\proxies\stickerproxy3.p3d";
	};
};
class CfgSlots
{
	class Slot_KODSticker1
	{
		name="KODSticker1";
		displayName="Наклейка";
		GhostIcon="set:kod_icons image:Sticker";
	};
	class Slot_KODSticker2
	{
		name="KODSticker2";
		displayName="Наклейка";
		GhostIcon="set:kod_icons image:Sticker";
	};
	class Slot_KODSticker3
	{
		name="KODSticker3";
		displayName="Наклейка";
		GhostIcon="set:kod_icons image:Sticker";
	};
};
