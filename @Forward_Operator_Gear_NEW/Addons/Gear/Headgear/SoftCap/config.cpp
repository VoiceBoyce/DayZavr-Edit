class CfgPatches
{
	class FOG_MOD_SoftCap
	{
		units[]=
		{
			"FOG_SoftCap_MC"
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
	class FOG_SoftCap_ColorBase: Clothing
	{
		scope=0;
		displayName="Patrol Cap";
		descriptionShort="Штатная кепка Армии США. Носится параллельно поверхности марша. | Standard Issue US Army Softcap, typically worn parallel with the marching surface, displaying the wearers rank and last name.";
		model="\FOG_MOD\Gear\Headgear\SoftCap\FOG_SoftCap_G.p3d";
		repairableWithKits[]={5,2};
		repairCosts[]={30,25};
		inventorySlot[]=
		{
			"Headgear"
		};
		vehicleClass="Clothing";
		simulation="clothing";
		itemInfo[]=
		{
			"Clothing",
			"Headgear"
		};
		weight=10;
		itemSize[]={3,2};
		varWetMax=0.60000002;
		heatIsolation=0.30000001;
		noNVStrap=0;
		noMask=0;
		noHelmet=0;
		noEyewear=0;
		headSelectionsToHide[]=
		{
			"Clipping_baseballcap"
		};
		hiddenSelections[]=
		{
			"camo"
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=70;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"FOG_MOD\Gear\Headgear\SoftCap\data\softcap.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"FOG_MOD\Gear\Headgear\SoftCap\data\softcap.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"FOG_MOD\Gear\Headgear\SoftCap\data\softcap.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"FOG_MOD\Gear\Headgear\SoftCap\data\softcap_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"FOG_MOD\Gear\Headgear\SoftCap\data\softcap_destruct.rvmat"
							}
						}
					};
				};
			};
		};
		class ClothingTypes
		{
			male="\FOG_MOD\Gear\Headgear\SoftCap\FOG_SoftCap_M.p3d";
			female="\FOG_MOD\Gear\Headgear\SoftCap\FOG_SoftCap_F.p3d";
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
	class FOG_SoftCap_MC: FOG_SoftCap_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Gear\Headgear\SoftCap\data\softcap_mc_blank_co.paa"
		};
	};
};
