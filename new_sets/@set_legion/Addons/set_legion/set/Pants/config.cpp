class CfgPatches
{
	class Vp_SET20
	{
		units[]=
		{
			"Vp_SET20_Pants"
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
	class Clothing_Base;
	class Clothing: Clothing_Base
	{
	};
	class Vp_SET20_Pants_ColorBase: Clothing
	{
		displayName="$STR_Legion_Pants";
		descriptionShort="";
		model="set_legion\set\Pants\model\Pants_g.p3d";
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
		itemsCargoSize[]={5,4};
		weight=630;
		ragQuantity=3;
		varWetMax=0.49000001;
		visibilityModifier=0.80000001;
		repairableWithKits[]={5,2};
		repairCosts[]={30,25};
		quickBarBonus=2;
		hiddenSelections[]=
		{
			"koleno",
			"pants",
			"personality"
		};
		hiddenSelectionsTextures[]={};
		class ClothingTypes
		{
			male="set_legion\set\Pants\model\Pants_m.p3d";
			female="set_legion\set\Pants\model\Pants_f.p3d";
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=150;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"set_legion\set\Pants\data\koleno.rvmat",
								"set_legion\set\Pants\data\pants.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"set_legion\set\Pants\data\koleno.rvmat",
								"set_legion\set\Pants\data\pants.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"set_legion\set\Pants\data\koleno_damage.rvmat",
								"set_legion\set\Pants\data\pants_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"set_legion\set\Pants\data\koleno_damage.rvmat",
								"set_legion\set\Pants\data\pants_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"set_legion\set\Pants\data\koleno_destruct.rvmat",
								"set_legion\set\Pants\data\pants_destruct.rvmat"
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
						damage=0.85000002;
					};
					class Blood
					{
						damage=0.80000001;
					};
					class Shock
					{
						damage=0.89999998;
					};
				};
				class Infected
				{
					class Health
					{
						damage=0.85000002;
					};
					class Blood
					{
						damage=0.80000001;
					};
					class Shock
					{
						damage=0.89999998;
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
	class Vp_SET20_Pants: Vp_SET20_Pants_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"set_legion\set\Pants\data\koleno_co.paa",
			"set_legion\set\Pants\data\pants_co.paa"
		};
	};
};
