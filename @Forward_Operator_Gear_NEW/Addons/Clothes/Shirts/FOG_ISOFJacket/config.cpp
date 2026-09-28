class CfgPatches
{
	class FOG_Shirt_ISOF_Jacket_Shit
	{
		units[]=
		{
			"FOG_Shirt_ISOF_Jacket_Black",
			"FOG_Shirt_ISOF_Jacket_MC",
			"FOG_Shirt_ISOF_Jacket_MC2",
			"FOG_Shirt_ISOF_Jacket_M81",
			"FOG_Shirt_ISOF_Jacket_MCAL",
			"FOG_Shirt_ISOF_Jacket_RG",
			"FOG_Shirt_ISOF_Jacket_CB",
			"FOG_Shirt_ISOF_Jacket_White"
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
	class FOG_Shirt_ISOF_Jacket_ColorBase: Clothing
	{
		scope=0;
		displayName="ISOF Combat Armor Jacket";
		descriptionShort="ISOF Combat Jacket — это высокозащищённая тактическая куртка с интегрированными керамическими бронепластинами и 80 слотами для снаряжения, созданная для спецопераций в горячих точках, где каждый карман спроектирован под быстрый доступ к оружию и медикаментам под огнём | The ISOF Combat Jacket is a high-protection tactical rig featuring integrated ceramic armor plates and 80 gear slots, engineered for hot zone operations where every pouch is optimized for rapid access to weapons and meds under fire.";
		model="\FOG_MOD\Clothes\Shirts\FOG_ISOFJacket\FOG_ISOFJacket_G.p3d";
		inventorySlot[]=
		{
			"Body"
		};
		itemInfo[]=
		{
			"Clothing",
			"Body"
		};
		weight=870;
		itemSize[]={6,6};
		itemsCargoSize[]={10,8};
		quickBarBonus=3;
		varWetMax=0.249;
		heatIsolation=0.59999998;
		noVest=1;
		ragQuantity=4;
		repairableWithKits[]={5,2};
		repairCosts[]={30,25};
		soundAttType="WoolShirt";
		hiddenSelections[]=
		{
			"camo_jacket",
			"camo_shirt"
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=300;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"FOG_MOD\Clothes\Shirts\FOG_ISOFJacket\Data\ISOFjacket.rvmat",
								"FOG_MOD\Clothes\Shirts\FOG_Crye_G3Shirt\Data\G3_CombatShirt.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"FOG_MOD\Clothes\Shirts\FOG_ISOFJacket\Data\ISOFjacket.rvmat",
								"FOG_MOD\Clothes\Shirts\FOG_Crye_G3Shirt\Data\G3_CombatShirt.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"FOG_MOD\Clothes\Shirts\FOG_ISOFJacket\Data\ISOFjacket_damage.rvmat",
								"FOG_MOD\Clothes\Shirts\FOG_Crye_G3Shirt\Data\G3_CombatShirt_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"FOG_MOD\Clothes\Shirts\FOG_ISOFJacket\Data\ISOFjacket_damage.rvmat",
								"FOG_MOD\Clothes\Shirts\FOG_Crye_G3Shirt\Data\G3_CombatShirt_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"FOG_MOD\Clothes\Shirts\FOG_ISOFJacket\Data\ISOFjacket_destruct.rvmat",
								"FOG_MOD\Clothes\Shirts\FOG_Crye_G3Shirt\Data\G3_CombatShirt_destruct.rvmat"
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
						damage=0.15;
					};
					class Blood
					{
						damage=0;
					};
					class Shock
					{
						damage=0.15;
					};
				};
				class Melee
				{
					class Health
					{
						damage=0.15;
					};
					class Blood
					{
						damage=0;
					};
					class Shock
					{
						damage=0.15;
					};
				};
				class Infected
				{
					class Health
					{
						damage=0.15;
					};
					class Blood
					{
						damage=0;
					};
					class Shock
					{
						damage=0.15;
					};
				};
				class FragGrenade
				{
					class Health
					{
						damage=0.15;
					};
					class Blood
					{
						damage=0;
					};
					class Shock
					{
						damage=0.15;
					};
				};
			};
		};
		class ClothingTypes
		{
			male="\FOG_MOD\Clothes\Shirts\FOG_ISOFJacket\FOG_ISOFJacket_M.p3d";
			female="\FOG_MOD\Clothes\Shirts\FOG_ISOFJacket\FOG_ISOFJacket_F.p3d";
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
	class FOG_Shirt_ISOF_Jacket_Black: FOG_Shirt_ISOF_Jacket_ColorBase
	{
		scope=2;
		visibilityModifier=0.69999999;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\FOG_ISOFJacket\Data\Jacket_Black_co.paa",
			"FOG_MOD\Clothes\Shirts\FOG_Crye_G3Shirt\Data\G3_CombatShirt_Black_co.paa"
		};
	};
	class FOG_Shirt_ISOF_Jacket_MC: FOG_Shirt_ISOF_Jacket_ColorBase
	{
		scope=2;
		visibilityModifier=0.69999999;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\FOG_ISOFJacket\Data\Jacket_mcb_co.paa",
			"FOG_MOD\Clothes\Shirts\FOG_Crye_G3Shirt\Data\G3_CombatShirt_MC_OCP_co.paa"
		};
	};
	class FOG_Shirt_ISOF_Jacket_MC2: FOG_Shirt_ISOF_Jacket_ColorBase
	{
		scope=2;
		visibilityModifier=0.69999999;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\FOG_ISOFJacket\Data\Jacket_mc_co.paa",
			"FOG_MOD\Clothes\Shirts\FOG_Crye_G3Shirt\Data\G3_CombatShirt_MC_OCP_co.paa"
		};
	};
	class FOG_Shirt_ISOF_Jacket_M81: FOG_Shirt_ISOF_Jacket_ColorBase
	{
		scope=2;
		visibilityModifier=0.69999999;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\FOG_ISOFJacket\Data\Jacket_mcb_co.paa",
			"FOG_MOD\Clothes\Shirts\FOG_Crye_G3Shirt\Data\G3_CombatShirt_M81_co.paa"
		};
	};
	class FOG_Shirt_ISOF_Jacket_MCAL: FOG_Shirt_ISOF_Jacket_ColorBase
	{
		scope=2;
		visibilityModifier=0.40000001;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\FOG_ISOFJacket\Data\Jacket_mcal_co.paa",
			"FOG_MOD\Clothes\Shirts\FOG_Crye_G3Shirt\Data\G3_CombatShirt_White_co.paa"
		};
	};
	class FOG_Shirt_ISOF_Jacket_RG: FOG_Shirt_ISOF_Jacket_ColorBase
	{
		scope=2;
		visibilityModifier=0.40000001;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\FOG_ISOFJacket\Data\Jacket_rg_co.paa",
			"FOG_MOD\Clothes\Shirts\FOG_Crye_G3Shirt\Data\G3_CombatShirt_RG_co.paa"
		};
	};
	class FOG_Shirt_ISOF_Jacket_CB: FOG_Shirt_ISOF_Jacket_ColorBase
	{
		scope=2;
		visibilityModifier=0.40000001;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\FOG_ISOFJacket\Data\Jacket_cb_co.paa",
			"FOG_MOD\Clothes\Shirts\FOG_Crye_G3Shirt\Data\G3_CombatShirt_cb_co.paa"
		};
	};
	class FOG_Shirt_ISOF_Jacket_White: FOG_Shirt_ISOF_Jacket_ColorBase
	{
		scope=2;
		visibilityModifier=0.40000001;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\FOG_ISOFJacket\Data\Jacket_White_co.paa",
			"FOG_MOD\Clothes\Shirts\FOG_Crye_G3Shirt\Data\G3_CombatShirt_White_co.paa"
		};
	};
};
