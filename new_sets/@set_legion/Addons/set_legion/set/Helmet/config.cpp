class CfgPatches
{
	class Vp_SET20
	{
		units[]=
		{
			"Vp_SET20_Helmet",
			"Vp_SET20_Helmet_No_Tail"
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
	class Vp_SET20_Helmet_ColorBase: Clothing
	{
		displayName="$STR_Legion_Helmet";
		descriptionShort="";
		model="set_legion\set\Helmet\model\Helmet_g.p3d";
		weight=2000;
		itemSize[]={4,3};
		repairableWithKits[]={8};
		attachments[]=
		{
			"NVG"
		};
		repairCosts[]={25};
		inventorySlot[]=
		{
			"Headgear"
		};
		simulation="clothing";
		vehicleClass="Clothing";
		itemInfo[]=
		{
			"Clothing",
			"Headgear"
		};
		visibilityModifier=0.89999998;
		varWetMax=0.249;
		heatIsolation=0.30000001;
		headSelectionsToHide[]=
		{
			"Clipping_Balaclava"
		};
		hiddenSelections[]=
		{
			"bolti",
			"helmet",
			"krep_helmet",
			"krep_xfost",
			"naushnik",
			"xvost",
			"zalypa1",
			"zalypa2"
		};
		class ClothingTypes
		{
			male="set_legion\set\Helmet\model\Helmet_mf.p3d";
			female="set_legion\set\Helmet\model\Helmet_mf.p3d";
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=75;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"set_legion\set\Helmet\data\bolti.rvmat",
								"set_legion\set\Helmet\data\helmet.rvmat",
								"set_legion\set\Helmet\data\krep_helmet.rvmat",
								"set_legion\set\Helmet\data\krep_xvost.rvmat",
								"set_legion\set\Helmet\data\naushnik.rvmat",
								"set_legion\set\Helmet\data\xvost.rvmat",
								"set_legion\set\Helmet\data\zalypa1.rvmat",
								"set_legion\set\Helmet\data\zalypa2.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"set_legion\set\Helmet\data\bolti.rvmat",
								"set_legion\set\Helmet\data\helmet.rvmat",
								"set_legion\set\Helmet\data\krep_helmet.rvmat",
								"set_legion\set\Helmet\data\krep_xvost.rvmat",
								"set_legion\set\Helmet\data\naushnik.rvmat",
								"set_legion\set\Helmet\data\xvost.rvmat",
								"set_legion\set\Helmet\data\zalypa1.rvmat",
								"set_legion\set\Helmet\data\zalypa2.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"set_legion\set\Helmet\data\bolti_damage.rvmat",
								"set_legion\set\Helmet\data\helmet_damage.rvmat",
								"set_legion\set\Helmet\data\krep_helmet_damage.rvmat",
								"set_legion\set\Helmet\data\krep_xvost_damage.rvmat",
								"set_legion\set\Helmet\data\naushnik_damage.rvmat",
								"set_legion\set\Helmet\data\xvost_damage.rvmat",
								"set_legion\set\Helmet\data\zalypa1_damage.rvmat",
								"set_legion\set\Helmet\data\zalypa2_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"set_legion\set\Helmet\data\bolti_damage.rvmat",
								"set_legion\set\Helmet\data\helmet_damage.rvmat",
								"set_legion\set\Helmet\data\krep_helmet_damage.rvmat",
								"set_legion\set\Helmet\data\krep_xvost_damage.rvmat",
								"set_legion\set\Helmet\data\naushnik_damage.rvmat",
								"set_legion\set\Helmet\data\xvost_damage.rvmat",
								"set_legion\set\Helmet\data\zalypa1_damage.rvmat",
								"set_legion\set\Helmet\data\zalypa2_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"set_legion\set\Helmet\data\bolti_destruct.rvmat",
								"set_legion\set\Helmet\data\helmet_destruct.rvmat",
								"set_legion\set\Helmet\data\krep_helmet_destruct.rvmat",
								"set_legion\set\Helmet\data\krep_xvost_destruct.rvmat",
								"set_legion\set\Helmet\data\naushnik_destruct.rvmat",
								"set_legion\set\Helmet\data\xvost_destruct.rvmat",
								"set_legion\set\Helmet\data\zalypa1_destruct.rvmat",
								"set_legion\set\Helmet\data\zalypa2_destruct.rvmat"
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
						damage=0.25;
					};
					class Blood
					{
						damage=0;
					};
					class Shock
					{
						damage=0.5;
					};
				};
				class Melee
				{
					class Health
					{
						damage=0.55000001;
					};
					class Blood
					{
						damage=0.2;
					};
					class Shock
					{
						damage=0.5;
					};
				};
				class Infected
				{
					class Health
					{
						damage=0.55000001;
					};
					class Blood
					{
						damage=0.2;
					};
					class Shock
					{
						damage=0.5;
					};
				};
				class FragGrenade
				{
					class Health
					{
						damage=0.5;
					};
					class Blood
					{
						damage=0;
					};
					class Shock
					{
						damage=0.25999999;
					};
				};
			};
		};
		soundImpactType="metal";
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
	};
	class Vp_SET20_Helmet: Vp_SET20_Helmet_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"set_legion\set\Helmet\data\bolti_co.paa",
			"set_legion\set\Helmet\data\helmet_co.paa",
			"set_legion\set\Helmet\data\krep_helmet_co.paa",
			"set_legion\set\Helmet\data\krep_xvost_co.paa",
			"set_legion\set\Helmet\data\naushnik_co.paa",
			"set_legion\set\Helmet\data\xvost_co.paa",
			"set_legion\set\Helmet\data\zalypa1_co.paa",
			"set_legion\set\Helmet\data\zalypa2_co.paa"
		};
	};
	class Vp_SET20_Helmet_No_Tail: Vp_SET20_Helmet_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"set_legion\set\Helmet\data\bolti_co.paa",
			"set_legion\set\Helmet\data\helmet_co.paa",
			"set_legion\set\Helmet\data\krep_helmet_co.paa",
			"set_legion\set\Helmet\data\krep_xvost_ca.paa",
			"set_legion\set\Helmet\data\naushnik_co.paa",
			"set_legion\set\Helmet\data\xvost_ca.paa",
			"set_legion\set\Helmet\data\zalypa1_co.paa",
			"set_legion\set\Helmet\data\zalypa2_co.paa"
		};
	};
};
