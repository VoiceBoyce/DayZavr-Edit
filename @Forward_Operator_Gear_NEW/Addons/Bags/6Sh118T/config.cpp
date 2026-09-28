class CfgPatches
{
	class FOG_Bag_6SHTBag
	{
		units[]=
		{
			"FOG_Bag_6ShBagT_Green",
			"FOG_Bag_6ShBagT_BlackEMR",
			"FOG_Bag_6ShBagT_Black",
			"FOG_Bag_6ShBagT_RG",
			"FOG_Bag_6ShBagT_CB",
			"FOG_Bag_6ShBagT_White",
			"FOG_Bag_6ShBagT_EMR_White"
		};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Data",
			"DZ_Characters",
			"DZ_Scripts"
		};
	};
};
class cfgVehicles
{
	class Clothing;
	class FOG_Bag_6ShTBag_ColorBase: Clothing
	{
		scope=0;
		displayName="6SH118 Backpack";
		descriptionShort="Один из лучших рюкзаков которые вы можете найти во вселенной DayZavr. Вместимость настолько впечатляющая, что сзади вы просто ходячий рюкзак на ножках. Уместите ли вы сюда свою жизнь или нет - покажет, истинный ли Вы хомяк. | One of the best backpacks you can find in the DayZavr universe. The capacity is so impressive that you're just a walking backpack with legs on the back. Whether you fit your life here or not will show whether you are a true hamster.";
		model="FOG_MOD\Bags\6Sh118T\models\Bag6SHT_g.p3d";
		inventorySlot[]=
		{
			"Back"
		};
		itemInfo[]=
		{
			"Clothing",
			"Back"
		};
		simulation="clothing";
		vehicleClass="Clothing";
		rotationFlags=16;
		itemSize[]={10,14};
		quickBarBonus=2;
		itemsCargoSize[]={10,24};
		weight=1600;
		varWetMax=0.1;
		heatIsolation=0.6;
		repairableWithKits[]={5,2};
		repairCosts[]={30,25};
		soundAttType="Outdoor";
		randomQuantity=4;
		canBeDigged=0;
		hiddenSelections[]=
		{
			"camoflage"
		};
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Bags\6Sh118T\data\6sh118T_co.paa"
		};
		attachments[]=
		{
			"Chemlight",
			"WalkieTalkie",
			"Backpack_1"
		};
		class ClothingTypes
		{
			male="FOG_MOD\Bags\6Sh118T\models\Bag6SHT.p3d";
			female="FOG_MOD\Bags\6Sh118T\models\Bag6SHT.p3d";
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=3000;
					transferToAttachmentsCoef=0.5;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"FOG_MOD\Bags\6Sh118T\data\6sh118T.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"FOG_MOD\Bags\6Sh118T\data\6sh118T.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"FOG_MOD\Bags\6Sh118T\data\6sh118T_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"FOG_MOD\Bags\6Sh118T\data\6sh118T_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"FOG_MOD\Bags\6Sh118T\data\6sh118T_destruct.rvmat"
							}
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
						damage=0.99;
					};
					class Blood
					{
						damage=0;
					};
					class Shock
					{
						damage=0.99;
					};
				};
				class Melee
				{
					class Health
					{
						damage=0.95;
					};
					class Blood
					{
						damage=0;
					};
					class Shock
					{
						damage=0.95;
					};
				};
				class Infected
				{
					class Health
					{
						damage=0.90;
					};
					class Blood
					{
						damage=0;
					};
					class Shock
					{
						damage=0.90;
					};
				};
				class FragGrenade
				{
					class Health
					{
						damage=0.99;
					};
					class Blood
					{
						damage=0;
					};
					class Shock
					{
						damage=0.99;
					};
				};
			};
		};
		class AnimEvents
		{
			class SoundWeapon
			{
				class pickUpItem_Light
				{
					soundSet="pickUpBackPack_Plastic_Light_SoundSet";
					id=796;
				};
				class pickUpItem
				{
					soundSet="pickUpBackPack_Plastic_SoundSet";
					id=797;
				};
				class drop
				{
					soundset="taloonbag_drop_SoundSet";
					id=898;
				};
			};
		};
	};
	class FOG_Bag_6ShBagT_Green: FOG_Bag_6ShTBag_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Bags\6Sh118T\data\6sh118T_co.paa"
		};
	};
	class FOG_Bag_6ShBagT_BlackEMR: FOG_Bag_6ShTBag_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Bags\6Sh118T\data\6sh118T_black_EMR_co.paa"
		};
	};
	class FOG_Bag_6ShBagT_Black: FOG_Bag_6ShTBag_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Bags\6Sh118T\data\6sh118T_Black_co.paa"
		};
	};
	class FOG_Bag_6ShBagT_RG: FOG_Bag_6ShTBag_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Bags\6Sh118T\data\6sh118T_RG_co.paa"
		};
	};
	class FOG_Bag_6ShBagT_CB: FOG_Bag_6ShTBag_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Bags\6Sh118T\data\6sh118T_CB_co.paa"
		};
	};
	class FOG_Bag_6ShBagT_White: FOG_Bag_6ShTBag_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Bags\6Sh118T\data\6sh118T_White_co.paa"
		};
	};
	class FOG_Bag_6ShBagT_EMR_White: FOG_Bag_6ShTBag_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Bags\6Sh118T\data\6sh118T_EMR_White_co.paa"
		};
	};
};
