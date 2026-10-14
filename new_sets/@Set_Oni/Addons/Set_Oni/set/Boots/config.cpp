class CfgPatches
{
	class VP_SET13
	{
		units[]=
		{
			"VP_SET13_Shoes_Black"
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
	class VP_SET13_Shoes_ColorBase: Clothing
	{
		scope=0;
		displayName="$STR_sHOES";
		descriptionShort="";
		model="Set_Oni\set\Boots\model\Boots_g.p3d";
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
			"camo"
		};
		class ClothingTypes
		{
			male="Set_Oni\set\Boots\model\Boots_mf.p3d";
			female="Set_Oni\set\Boots\model\Boots_mf.p3d";
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
								"Set_Oni\set\Boots\data\Boots.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"Set_Oni\set\Boots\data\Boots.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"Set_Oni\set\Boots\data\Boots_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"Set_Oni\set\Boots\data\Boots_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"Set_Oni\set\Boots\data\Boots_destruct.rvmat"
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
	class VP_SET13_Shoes_Black: VP_SET13_Shoes_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"Set_Oni\set\Boots\data\Boots_co.paa"
		};
	};
};
