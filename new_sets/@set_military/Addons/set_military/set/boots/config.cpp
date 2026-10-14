class CfgPatches
{
	class VP_SET19
	{
		units[]=
		{
			"VP_SET19_Boots_Desert",
			"VP_SET19_Boots_Black",
			"VP_SET19_Boots_Black_Pink",
			"VP_SET19_Boots_Black_Pixel"
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
	class VP_SET19_Boots_ColorBase: Clothing
	{
		scope=0;
		displayName="$STR_set_military_Boots";
		descriptionShort="";
		model="\set_military\set\boots\model\Boots_g.p3d";
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
			male="\set_military\set\boots\model\Boots_mf.p3d";
			female="\set_military\set\boots\model\Boots_mf.p3d";
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
								"\set_military\set\boots\data\Boots.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"\set_military\set\boots\data\Boots.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"\set_military\set\boots\data\Boots_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"\set_military\set\boots\data\Boots_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"\set_military\set\boots\data\Boots_destruct.rvmat"
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
						damage=0.75;
					};
					class Blood
					{
						damage=0.60000002;
					};
					class Shock
					{
						damage=0.75;
					};
				};
				class Melee
				{
					class Health
					{
						damage=0.75;
					};
					class Blood
					{
						damage=0.60000002;
					};
					class Shock
					{
						damage=0.75;
					};
				};
				class Infected
				{
					class Health
					{
						damage=0.75;
					};
					class Blood
					{
						damage=0.60000002;
					};
					class Shock
					{
						damage=0.75;
					};
				};
				class FragGrenade
				{
					class Health
					{
						damage=0.2;
					};
					class Blood
					{
						damage=0.2;
					};
					class Shock
					{
						damage=0.2;
					};
				};
			};
		};
		class Protection
		{
			biological=1;
			chemical=1;
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
	class VP_SET19_Boots_Desert: VP_SET19_Boots_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"\set_military\set\boots\data\Boots_co.paa"
		};
	};
	class VP_SET19_Boots_Black: VP_SET19_Boots_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"\set_military\set\retexture\black\Boots_black.paa"
		};
	};
	class VP_SET19_Boots_Black_Pink: VP_SET19_Boots_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"\set_military\set\retexture\black_pink\Boots_black_pink.paa"
		};
	};
	class VP_SET19_Boots_Black_Pixel: VP_SET19_Boots_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"\set_military\set\retexture\black_pixel\boots_black_pixel.paa"
		};
	};
};
