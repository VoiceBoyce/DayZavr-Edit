class CfgPatches
{
	class FOG_Clothing_Pants_G99
	{
		units[]=
		{
			"FOG_G99_Pants_Black",
			"FOG_G99_Pants_White",
			"FOG_G99_Pants_CB",
			"FOG_G99_Pants_RG",
			"FOG_G99_Pants_M81"
		};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Characters"
		};
	};
};
class CfgVehicles
{
	class Clothing;
	class FOG_G99_Pants_ColorBase: Clothing
	{
		displayName="G99 Pants";
		descriptionShort="G99 это профессиональные тактические брюки премиум-класса с 54 слотами для снаряжения и встроенной скрытой кобурой, созданные для оперативников и специалистов, где каждый элемент продуман для скоростного доступа и максимальной надежности в экстремальных условиях | The G99 are premium-grade tactical pants featuring 54 gear slots and integrated concealed holster, engineered for operatives and professionals where every detail is optimized for rapid access and ultimate reliability in high-stakes environments.";
		model="\FOG_MOD\Clothes\Pants\G99\FOG_G99_G.p3d";
		attachments[]=
		{
			"Pistol"
		};
		ContinuouActions[]=
		{
			"AT_WRING_CLOTHES"
		};
		inventorySlot[]=
		{
			"Legs"
		};
		simulation="clothing";
		itemInfo[]=
		{
			"Clothing",
			"Legs"
		};
		itemSize[]={4,3};
		itemsCargoSize[]={9,6};
		weight=300;
		ragQuantity=3;
		varWetMax=0.249;
		heatIsolation=0.75;
		repairableWithKits[]={5,2};
		repairCosts[]={30,25};
		quickBarBonus=2;
		hiddenSelections[]=
		{
			"camo"
		};
		class ClothingTypes
		{
			male="\FOG_MOD\Clothes\Pants\G99\FOG_G99_M.p3d";
			female="\FOG_MOD\Clothes\Pants\G99\FOG_G99_F.p3d";
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=790;
					transferToAttachmentsCoef=0.2;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"FOG_MOD\Clothes\Pants\G99\data\G99.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"FOG_MOD\Clothes\Pants\G99\data\G99.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"FOG_MOD\Clothes\Pants\G99\data\G99_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"FOG_MOD\Clothes\Pants\G99\data\G99_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"FOG_MOD\Clothes\Pants\G99\data\G99_destruct.rvmat"
							}
						}
					};
				};
			};
			class GlobalArmor
			{
				class Melee
				{
					class Health
					{
						damage=0.89999998;
					};
					class Blood
					{
						damage=0.89999998;
					};
					class Shock
					{
						damage=1;
					};
				};
				class Infected
				{
					class Health
					{
						damage=0.89999998;
					};
					class Blood
					{
						damage=0.89999998;
					};
					class Shock
					{
						damage=1;
					};
				};
			};
		};
		class AnimEvents
		{
			class SoundWeapon
			{
				class pickUpItem
				{
					soundSet="Shirt_pickup_SoundSet";
					id=797;
				};
				class drop
				{
					soundset="Shirt_drop_SoundSet";
					id=898;
				};
			};
		};
	};
	class FOG_G99_Pants_Black: FOG_G99_Pants_ColorBase
	{
		scope=2;
		visibilityModifier=0.75;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Pants\G99\data\G99_black_co.paa"
		};
	};
	class FOG_G99_Pants_White: FOG_G99_Pants_ColorBase
	{
		scope=2;
		visibilityModifier=0.75;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Pants\G99\data\G99_White_co.paa"
		};
	};
	class FOG_G99_Pants_CB: FOG_G99_Pants_ColorBase
	{
		scope=2;
		visibilityModifier=0.75;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Pants\G99\data\G99_cb_co.paa"
		};
	};
	class FOG_G99_Pants_RG: FOG_G99_Pants_ColorBase
	{
		scope=2;
		visibilityModifier=0.75;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Pants\G99\data\G99_rg_co.paa"
		};
	};
	class FOG_G99_Pants_M81: FOG_G99_Pants_ColorBase
	{
		scope=2;
		visibilityModifier=0.75;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Pants\G99\data\G99_M81_co.paa"
		};
	};
};
