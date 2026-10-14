class CfgPatches
{
	class TGK_FSB_ALPHA
	{
		units[]={};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Characters",
			"DZ_Characters_Shoes",
			"DZ_Scripts"
		};
	};
};
class CfgVehicles
{
	class Clothing_Base;
	class Clothing: Clothing_Base
	{
	};
	class CombatBoots_ColorBase;
	class TGK_FSB_ALPHA_Boots_ColorBase: CombatBoots_ColorBase
	{
		displayName="$STR_FSB_ALPHA_Boots_01";
		descriptionShort="$STR_FSB_ALPHA_Boots_02";
		model="TGK_FSB_Alpha\set\Boots\models\Boots_g.p3d";
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
		weight=1200;
		durability=0.34999999;
		varWetMax=0.25;
		heatIsolation=0.64999998;
		visibilityModifier=0.89999998;
		repairableWithKits[]={3};
		repairCosts[]={25};
		soundAttType="Sneakers";
		soundImpactType="default";
		hiddenSelections[]=
		{
			"camo"
		};
		rotationFlags=16;
		class ClothingTypes
		{
			male="TGK_FSB_Alpha\set\Boots\models\Boots_m.p3d";
			female="TGK_FSB_Alpha\set\Boots\models\Boots_m.p3d";
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=500;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"TGK_FSB_Alpha\set\Boots\data\Boots.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"TGK_FSB_Alpha\set\Boots\data\Boots.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"TGK_FSB_Alpha\set\Boots\data\Boots_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"TGK_FSB_Alpha\set\Boots\data\Boots_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"TGK_FSB_Alpha\set\Boots\data\Boots_destruct.rvmat"
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
	class TGK_FSB_ALPHA_Boots_Black: TGK_FSB_ALPHA_Boots_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_FSB_Alpha\set\Boots\data\Boots_Black_co.paa"
		};
	};
};
