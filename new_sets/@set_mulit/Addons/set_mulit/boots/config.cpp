class CfgPatches
{
	class SM_VP_Multi_Boots_Set
	{
		units[]={};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Data"
		};
	};
};
class CfgVehicles
{
	class Clothing;
	class SM_VP_Multi_Boots_ColorBase: Clothing
	{
		scope=0;
		displayName="Ботинки Мульти";
		descriptionShort="Ботинки от сета Мульти.";
		model="set_mulit\boots\model\boots_g.p3d";
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
			male="set_mulit\boots\model\boots_m.p3d";
			female="set_mulit\boots\model\boots_f.p3d";
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
								"set_mulit\boots\data\boots.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"set_mulit\boots\data\boots.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"set_mulit\boots\data\boots_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"set_mulit\boots\data\boots_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"set_mulit\boots\data\boots_destruct.rvmat"
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
					soundSet="Athleticshoes_pickup_SoundSet";
					id=797;
				};
				class drop
				{
					soundset="Athleticshoes_drop_SoundSet";
					id=898;
				};
			};
		};
	};
	class SM_VP_Multi_Boots: SM_VP_Multi_Boots_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"set_mulit\boots\data\boots_co.paa"
		};
	};
};
