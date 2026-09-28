class CfgPatches
{
	class FOG_Helmet_Accessories_MK1Pack
	{
		units[]=
		{
			"FOG_SF_BatteryPack_Tan",
			"FOG_SF_BatteryPack_RG",
			"FOG_SF_BatteryPack_MC",
			"FOG_SF_BatteryPack_MCB",
			"FOG_SF_BatteryPack_Black"
		};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Characters",
			"DZ_Characters_Headgear"
		};
	};
};
class CfgVehicles
{
	class Inventory_Base;
	class FOG_SF_BatteryPack_Base: Inventory_Base
	{
		scope=0;
		displayName="FAST SF Battery Pack";
		descriptionShort="Батарейный блок FAST SF с ИК стробом Helstar. | Battery Pack with an accompanying Helstar IR Strobe.";
		itemsize[]={3,2};
		allowOwnedCargoManipulation=1;
		Model="\FOG_MOD\Helmets\Accessories\MK1BatteryPack\Fast_SF_Battery_Pack.p3d";
		inventorySlot[]=
		{
			"SF_BattPack"
		};
		attachments[]=
		{
			"FOG_small_patch",
			"BatteryD"
		};
		weight=50;
		hiddenSelections[]=
		{
			"batterypack",
			"helstar3"
		};
		repairableWithKits[]={5,2};
		repairCosts[]={30,25};
		class AnimEvents
		{
			class SoundWeapon
			{
				class pickUpItem
				{
					soundSet="pickUpPot_SoundSet";
					id=797;
				};
				class drop
				{
					soundset="BallisticHelmet_drop_SoundSet";
					id=898;
				};
			};
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
								"FOG_MOD\Helmets\Accessories\MK1BatteryPack\data\SF_BatteryPack.rvmat",
								"FOG_MOD\Helmets\Accessories\MK1BatteryPack\data\helstar.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"FOG_MOD\Helmets\Accessories\MK1BatteryPack\data\SF_BatteryPack.rvmat",
								"FOG_MOD\Helmets\Accessories\MK1BatteryPack\data\helstar.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"FOG_MOD\Helmets\Accessories\MK1BatteryPack\data\SF_BatteryPack_damage.rvmat",
								"FOG_MOD\Helmets\Accessories\MK1BatteryPack\data\helstar.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"FOG_MOD\Helmets\Accessories\MK1BatteryPack\data\SF_BatteryPack_damage.rvmat",
								"FOG_MOD\Helmets\Accessories\MK1BatteryPack\data\helstar.rvmat"
							}
						},
						
						{
							0,
							
							{
								"FOG_MOD\Helmets\Accessories\MK1BatteryPack\data\SF_BatteryPack_destruct.rvmat",
								"FOG_MOD\Helmets\Accessories\MK1BatteryPack\data\helstar.rvmat"
							}
						}
					};
				};
			};
		};
	};
	class FOG_SF_BatteryPack_Tan: FOG_SF_BatteryPack_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Helmets\Accessories\MK1BatteryPack\data\battery_pack_tan_co.paa",
			"FOG_MOD\Helmets\Accessories\MK1BatteryPack\data\helstar_co.paa"
		};
	};
	class FOG_SF_BatteryPack_RG: FOG_SF_BatteryPack_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Helmets\Accessories\MK1BatteryPack\data\battery_pack_rg_co.paa",
			"FOG_MOD\Helmets\Accessories\MK1BatteryPack\data\helstar_co.paa"
		};
	};
	class FOG_SF_BatteryPack_MC: FOG_SF_BatteryPack_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Helmets\Accessories\MK1BatteryPack\data\battery_pack_mc.paa",
			"FOG_MOD\Helmets\Accessories\MK1BatteryPack\data\helstar_co.paa"
		};
	};
	class FOG_SF_BatteryPack_MCB: FOG_SF_BatteryPack_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Helmets\Accessories\MK1BatteryPack\data\battery_pack_mcb.paa",
			"FOG_MOD\Helmets\Accessories\MK1BatteryPack\data\helstar_co.paa"
		};
	};
	class FOG_SF_BatteryPack_Black: FOG_SF_BatteryPack_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Helmets\Accessories\MK1BatteryPack\data\battery_pack_blk.paa",
			"FOG_MOD\Helmets\Accessories\MK1BatteryPack\data\helstar_co.paa"
		};
	};
};
