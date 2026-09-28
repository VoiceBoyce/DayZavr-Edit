class CfgPatches
{
	class FOG_Helmet_Accessories_MTMand
	{
		units[]=
		{
			"FOG_FastMT_Mandable_Tan",
			"FOG_FastMT_Mandable_MC",
			"FOG_FastMT_Mandable_Black",
			"FOG_FastMT_Mandable_RG"
		};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Characters",
			"DZ_Characters_Headgear"
		};
	};
};
class CfgVehicles
{
	class Inventory_Base;
	class FOG_FastMT_Mandable_Base: Inventory_Base
	{
		scope=0;
		displayName="FAST MT Mandable";
		descriptionShort="Лёгкая бронированная мандибула для шлема FAST MT. Чисто косметический элемент. | A light armoured protective mandable for the FAST MT. Purely Cosmetic.";
		itemsize[]={3,2};
		allowOwnedCargoManipulation=1;
		Model="\FOG_MOD\Helmets\Accessories\FASTMT_Mand\FOG_Mand.p3d";
		inventorySlot[]=
		{
			"FOG_mand"
		};
		weight=100;
		hiddenSelections[]=
		{
			"camo"
		};
		repairableWithKits[]={8};
		repairCosts[]={25};
		class AnimEvents
		{
			class SoundWeapon
			{
				class pickUpItem
				{
					soundSet="pickUpPot_SoundSet";
					id=797;
				};
				class drop
				{
					soundset="BallisticHelmet_drop_SoundSet";
					id=898;
				};
			};
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
								"FOG_MOD\Helmets\Accessories\FASTMT_Mand\data\helmet_attachment_Mandible.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"FOG_MOD\Helmets\Accessories\FASTMT_Mand\data\helmet_attachment_Mandible.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"FOG_MOD\Helmets\Accessories\FASTMT_Mand\data\helmet_attachment_Mandible_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"FOG_MOD\Helmets\Accessories\FASTMT_Mand\data\helmet_attachment_Mandible_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"FOG_MOD\Helmets\Accessories\FASTMT_Mand\data\helmet_attachment_Mandible_destruct.rvmat"
							}
						}
					};
				};
			};
		};
	};
	class FOG_FastMT_Mandable_Tan: FOG_FastMT_Mandable_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Helmets\Accessories\FASTMT_Mand\data\helmet_attachment_Mandible_Tan_c.paa"
		};
	};
	class FOG_FastMT_Mandable_MC: FOG_FastMT_Mandable_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Helmets\Accessories\FASTMT_Mand\data\helmet_attachment_Mandible_MC_c.paa"
		};
	};
	class FOG_FastMT_Mandable_Black: FOG_FastMT_Mandable_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Helmets\Accessories\FASTMT_Mand\data\helmet_attachment_Mandible_c.paa"
		};
	};
	class FOG_FastMT_Mandable_RG: FOG_FastMT_Mandable_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Helmets\Accessories\FASTMT_Mand\data\helmet_attachment_Mandible_RG_co.paa"
		};
	};
};
