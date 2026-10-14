class CfgPatches
{
	class VP_Set11
	{
		units[]=
		{
			"VP_Set11_Shoes"
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
class CfgVehicles
{
	class Clothing;
	class VP_Set11_Shoes_ColorBase: Clothing
	{
		scope=0;
		displayName="$STR_Shoes";
		descriptionShort="";
		model="set_moto\set\boots\model\moto_shoes_g.p3d";
		inventorySlot[]=
		{
			"Feet"
		};
		itemInfo[]=
		{
			"Clothing",
			"Feet"
		};
		itemSize[]={4,2};
		weight=500;
		durability=1;
		varWetMax=1;
		heatIsolation=1;
		visibilityModifier=0.89999998;
		repairableWithKits[]={3};
		repairCosts[]={25};
		soundAttType="Sneakers";
		soundImpactType="default";
		hiddenSelections[]=
		{
			"camo",
			"camo1"
		};
		class ClothingTypes
		{
			male="set_moto\set\boots\model\moto_shoes_mf.p3d";
			female="set_moto\set\boots\model\moto_shoes_mf.p3d";
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=90;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"set_moto\set\boots\data\shoes.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"set_moto\set\boots\data\shoes.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"set_moto\set\boots\data\shoes_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"set_moto\set\boots\data\shoes_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"set_moto\set\boots\data\shoes_destruct.rvmat"
							}
						}
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
					soundSet="AthleticShoes_pickup_SoundSet";
					id=797;
				};
				class drop
				{
					soundset="AthleticShoes_drop_SoundSet";
					id=898;
				};
			};
		};
	};
	class VP_Set11_Shoes: VP_Set11_Shoes_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"set_moto\set\boots\data\shoes_right.paa",
			"set_moto\set\boots\data\shoes_left.paa"
		};
	};
};
