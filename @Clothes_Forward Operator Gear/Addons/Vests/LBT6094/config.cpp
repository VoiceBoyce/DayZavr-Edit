class CfgPatches
{
	class FOG_MOD_Vest_LBT6094
	{
		units[]=
		{
			"FOG_Vest_LBT6094_CB",
			"FOG_Vest_LBT6094_MC",
			"FOG_Vest_LBT6094_Black",
			"FOG_Vest_LBT6094_RG",
			"FOG_Vest_LBT6094_Blue",
			"FOG_Vest_LBT6094_AOR1",
			"FOG_Vest_LBT6094_AOR2"
		};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_data"
		};
	};
};
class CfgVehicles
{
	class Clothing;
	class FOG_Vest_LBT6094_Base: Clothing
	{
		scope=0;
		displayName="LBT 6094 Plate Carrier";
		descriptionShort="The LBT 6094 Plate Carrier is a battle-tested tactical vest favored by elite Special Operations Forces worldwide, including units like U.S. Navy SEALs and Army Special Forces. Built for durability and adaptability, its rugged Cordura nylon construction and modular design make it a versatile platform for customizing gear setups.";
		model="\FOG_MOD\Vests\LBT6094\FOG_LBT6094_G.p3d";
		attachments[]=
		{
			"WalkieTalkie",
			"FOG_tourniquet",
			"VestPouch",
			"FOG_admin_small",
			"FOG_VestSlotFR",
			"FOG_gren_pouch",
			"FOG_ifak_vest",
			"FOG_vest_belly",
			"FOG_vest_panel"
		};
		itemInfo[]=
		{
			"Clothing",
			"Vest"
		};
		inventorySlot[]=
		{
			"Vest"
		};
		weight=6000;
		itemSize[]={4,4};
		quickBarBonus=3;
		varWetMax=0.49000001;
		heatIsolation=0.60000002;
		repairableWithKits[]={3};
		repairCosts[]={25};
		hiddenSelections[]=
		{
			"camo_vest",
			"camo_radiopouch"
		};
		simpleHiddenSelections[]=
		{
			"selection_radio"
		};
		hiddenSelectionsMaterials[]=
		{
			"FOG_MOD\Vests\LBT6094\data\LBT.rvmat",
			"FOG_MOD\Vests\LBT6094\data\ANPRC152.rvmat"
		};
		class ClothingTypes
		{
			male="\FOG_MOD\Vests\LBT6094\FOG_LBT6094_M.p3d";
			female="\FOG_MOD\Vests\LBT6094\FOG_LBT6094_F.p3d";
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
								"FOG_MOD\Vests\LBT6094\data\LBT.rvmat",
								"FOG_MOD\Vests\LBT6094\data\ANPRC152.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"FOG_MOD\Vests\LBT6094\data\LBT.rvmat",
								"FOG_MOD\Vests\LBT6094\data\ANPRC152.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"FOG_MOD\Vests\LBT6094\data\LBT.rvmat",
								"FOG_MOD\Vests\LBT6094\data\ANPRC152.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"FOG_MOD\Vests\LBT6094\data\LBT_damage.rvmat",
								"FOG_MOD\Vests\LBT6094\data\ANPRC152.rvmat"
							}
						},
						
						{
							0,
							
							{
								"FOG_MOD\Vests\LBT6094\data\LBT_destruct.rvmat",
								"FOG_MOD\Vests\LBT6094\data\ANPRC152.rvmat"
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
						damage=0.30000001;
					};
					class Blood
					{
						damage=0;
					};
					class Shock
					{
						damage=0.60000002;
					};
				};
				class Melee
				{
					class Health
					{
						damage=0.25;
					};
					class Blood
					{
						damage=0;
					};
					class Shock
					{
						damage=0.25;
					};
				};
				class Infected
				{
					class Health
					{
						damage=0.25;
					};
					class Blood
					{
						damage=0;
					};
					class Shock
					{
						damage=0.25;
					};
				};
				class FragGrenade
				{
					class Health
					{
						damage=0.5;
					};
					class Blood
					{
						damage=0;
					};
					class Shock
					{
						damage=0.25;
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
					soundSet="SmershVest_pickup_SoundSet";
					id=797;
				};
				class drop
				{
					soundset="SmershVest_drop_SoundSet";
					id=898;
				};
			};
		};
	};
	class FOG_Vest_LBT6094_CB: FOG_Vest_LBT6094_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Vests\LBT6094\data\LBT_CB_co.paa",
			"FOG_MOD\Vests\LBT6094\data\ANPRC152_CB_co.paa"
		};
	};
	class FOG_Vest_LBT6094_MC: FOG_Vest_LBT6094_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Vests\LBT6094\data\LBT_MC_co.paa",
			"FOG_MOD\Vests\LBT6094\data\ANPRC152_MC_co.paa"
		};
	};
	class FOG_Vest_LBT6094_Black: FOG_Vest_LBT6094_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Vests\LBT6094\data\LBT_Black_co.paa",
			"FOG_MOD\Vests\LBT6094\data\ANPRC152_Black_co.paa"
		};
	};
	class FOG_Vest_LBT6094_RG: FOG_Vest_LBT6094_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Vests\LBT6094\data\LBT_RG_co.paa",
			"FOG_MOD\Vests\LBT6094\data\ANPRC152_MC_co.paa"
		};
	};
	class FOG_Vest_LBT6094_Blue: FOG_Vest_LBT6094_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Vests\LBT6094\data\LBT_Blue_co.paa",
			"FOG_MOD\Vests\LBT6094\data\ANPRC152_Black_co.paa"
		};
	};
	class FOG_Vest_LBT6094_AOR1: FOG_Vest_LBT6094_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Vests\LBT6094\data\LBT_AOR1_co.paa",
			"FOG_MOD\Vests\LBT6094\data\ANPRC152_AOR1_co.paa"
		};
	};
	class FOG_Vest_LBT6094_AOR2: FOG_Vest_LBT6094_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Vests\LBT6094\data\LBT_AOR2_co.paa",
			"FOG_MOD\Vests\LBT6094\data\ANPRC152_AOR2_co.paa"
		};
	};
};
