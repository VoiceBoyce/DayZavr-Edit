class CfgPatches
{
	class TGK_Ghost_mask
	{
		units[]=
		{
			""
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
	class Clothing_Base;
	class Clothing: Clothing_Base
	{
	};
	class BalaclavaMask_ColorBase;
	class TGK_Ghost_mask_ColorBase: BalaclavaMask_ColorBase
	{
		scope=0;
		displayName="$STR_Ghost_Mask01";
		descriptionShort="$STR_Ghost_Mask02";
		model="TGK_Ghost\set\mask\models\Mask_g.p3d";
		inventorySlot[]=
		{
			"Mask"
		};
		itemInfo[]=
		{
			"Clothing",
			"Mask"
		};
		rotationFlags=17;
		weight=300;
		itemSize[]={2,3};
		absorbency=0;
		varWetMax=0;
		visibilityModifier=0.89999998;
		noMask=1;
		headSelectionsToHide[]=
		{
			"Clipping_Balaclava"
		};
		hiddenSelections[]=
		{
			"camo"
		};
		repairableWithKits[]={5,2};
		repairCosts[]={30,25};
		class ClothingTypes
		{
			male="TGK_Ghost\set\mask\models\Mask_m.p3d";
			female="TGK_Ghost\set\mask\models\Mask_m.p3d";
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=400;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"TGK_Ghost\set\mask\data\Mask.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"TGK_Ghost\set\mask\data\Mask.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"TGK_Ghost\set\mask\data\Mask_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"TGK_Ghost\set\mask\data\Mask_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"TGK_Ghost\set\mask\data\Mask_destruct.rvmat"
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
						damage=1;
					};
					class Blood
					{
						damage=1;
					};
					class Shock
					{
						damage=1;
					};
				};
				class Melee
				{
					class Health
					{
						damage=0.89999998;
					};
					class Blood
					{
						damage=0.15000001;
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
						damage=0.15000001;
					};
					class Shock
					{
						damage=1;
					};
				};
			};
		};
		class Protection
		{
			biological=0.25;
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
	class TGK_Ghost_mask: TGK_Ghost_mask_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_Ghost\set\mask\data\Mask_co.paa"
		};
	};
	class TGK_Ghost_mask_Black: TGK_Ghost_mask_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_Ghost\set\mask\data\Mask_Black.paa"
		};
	};
};
