class CfgPatches
{
	class FOG_MOD_FleeceCap
	{
		units[]=
		{
			"FOG_Fleececap_Black",
			"FOG_Fleececap_Grey",
			"FOG_Fleececap_CB",
			"FOG_Fleececap_RG",
			"FOG_Fleececap_White"
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
	class FOG_Fleececap_ColorBase: Clothing
	{
		scope=0;
		displayName="Fleece Cap";
		descriptionShort="Штатная флисовая шапка Армии, носится в холодную погоду. | The standard Army fleece hat, worn in cold weather.";
		model="\FOG_MOD\Gear\Headgear\FleeceCap\FOG_FleeceCap_G.p3d";
		repairableWithKits[]={5,2};
		repairCosts[]={30,25};
		inventorySlot[]=
		{
			"Headgear"
		};
		attachments[]=
		{
			"FOG_Headset_slot"
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
		varWetMax=0.34900001;
		heatIsolation=0.89999998;
		noNVStrap=0;
		noMask=0;
		noHelmet=0;
		noEyewear=0;
		headSelectionsToHide[]=
		{
			"Clipping_BeanieHat"
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
								"FOG_MOD\Gear\Headgear\FleeceCap\data\FleeceCap.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"FOG_MOD\Gear\Headgear\FleeceCap\data\FleeceCap.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"FOG_MOD\Gear\Headgear\FleeceCap\data\FleeceCap.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"FOG_MOD\Gear\Headgear\FleeceCap\data\FleeceCap_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"FOG_MOD\Gear\Headgear\FleeceCap\data\FleeceCap_destruct.rvmat"
							}
						}
					};
				};
			};
		};
		class ClothingTypes
		{
			male="\FOG_MOD\Gear\Headgear\FleeceCap\FOG_FleeceCap_M.p3d";
			female="\FOG_MOD\Gear\Headgear\FleeceCap\FOG_FleeceCap_F.p3d";
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
	class FOG_Fleececap_Black: FOG_Fleececap_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Gear\Headgear\FleeceCap\data\FleeceCap_Black_co.paa"
		};
	};
	class FOG_Fleececap_Grey: FOG_Fleececap_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Gear\Headgear\FleeceCap\data\FleeceCap_Grey_co.paa"
		};
	};
	class FOG_Fleececap_CB: FOG_Fleececap_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Gear\Headgear\FleeceCap\data\FleeceCap_CB_co.paa"
		};
	};
	class FOG_Fleececap_RG: FOG_Fleececap_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Gear\Headgear\FleeceCap\data\FleeceCap_RG_co.paa"
		};
	};
	class FOG_Fleececap_White: FOG_Fleececap_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Gear\Headgear\FleeceCap\data\FleeceCap_White_co.paa"
		};
	};
};
