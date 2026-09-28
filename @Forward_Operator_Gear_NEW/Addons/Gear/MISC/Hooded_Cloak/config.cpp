class CfgPatches
{
	class FOG_Hooded_Cloak_Stuff
	{
		units[]=
		{
			"FOG_Hooded_Cloak_Grey",
			"FOG_Hooded_Cloak_RG",
			"FOG_Hooded_Cloak_Black"
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
	class FOG_Hooded_Cloak_Base: Clothing
	{
		scope=0;
		displayName="Hooded Cloak";
		descriptionShort="Лёгкий плащ с капюшоном для быстрого и бесшумного передвижения. Из высокотехнологичных материалов с отличным камуфляжем. | A lightweight garment that allows for quick and silent movement. Made of high-tech materials, it provides excellent camouflage.";
		model="\FOG_MOD\Gear\MISC\Hooded_Cloak\Hooded_Cloak_G.p3d";
		itemInfo[]=
		{
			"Clothing",
			"Armband"
		};
		inventorySlot[]=
		{
			"Armband"
		};
		weight=80;
		itemSize[]={4,3};
		ragQuantity=1;
		varWetMax=0.249;
		heatIsolation=0.80000001;
		absorbency=0.80000001;
		repairableWithKits[]={5,2};
		repairCosts[]={30,25};
		hiddenSelections[]=
		{
			"camo"
		};
		class ClothingTypes
		{
			male="\FOG_MOD\Gear\MISC\Hooded_Cloak\Hooded_Cloak_M.p3d";
			female="\FOG_MOD\Gear\MISC\Hooded_Cloak\Hooded_Cloak_F.p3d";
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
								"FOG_MOD\Gear\MISC\Hooded_Cloak\data\hooded_cloak.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"FOG_MOD\Gear\MISC\Hooded_Cloak\data\hooded_cloak.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"FOG_MOD\Gear\MISC\Hooded_Cloak\data\hooded_cloak_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"FOG_MOD\Gear\MISC\Hooded_Cloak\data\hooded_cloak_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"FOG_MOD\Gear\MISC\Hooded_Cloak\data\hooded_cloak_destruct.rvmat"
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
	class FOG_Hooded_Cloak_Grey: FOG_Hooded_Cloak_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Gear\MISC\Hooded_Cloak\data\hooded_cloak_grey_co.paa"
		};
	};
	class FOG_Hooded_Cloak_RG: FOG_Hooded_Cloak_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Gear\MISC\Hooded_Cloak\data\hooded_cloak_RG_co.paa"
		};
	};
	class FOG_Hooded_Cloak_Black: FOG_Hooded_Cloak_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Gear\MISC\Hooded_Cloak\data\hooded_cloak_Black_co.paa"
		};
	};
};
