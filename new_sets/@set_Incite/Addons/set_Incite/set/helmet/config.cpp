class CfgPatches
{
	class Vp_Set17
	{
		units[]=
		{
			"Vp_Set17_Helmet_Black"
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
class cfgVehicles
{
	class Clothing_Base;
	class Clothing: Clothing_Base
	{
	};
	class Vp_Set17_Helmet_ColorBase: Clothing
	{
		scope=0;
		displayName="$STR_set_Incite_Helmet";
		descriptionShort="";
		model="\set_Incite\set\helmet\model\helmet_g.p3d";
		inventorySlot[]=
		{
			"Headgear",
			"Helmet"
		};
		simulation="clothing";
		vehicleClass="Clothing";
		itemInfo[]=
		{
			"Clothing",
			"Headgear"
		};
		ragQuantity=6;
		quickBarBonus=2;
		weight=310;
		itemSize[]={4,4};
		varWetMax=0.050000001;
		heatIsolation=1;
		repairableWithKits[]={3,8};
		repairCosts[]={25};
		hiddenSelections[]=
		{
			"helmet",
			"konektori",
			"nakladki_shlem",
			"trubki",
			"vizor"
		};
		headSelectionsToHide[]=
		{
			"Clipping_Balaclava"
		};
		class ClothingTypes
		{
			male="\set_Incite\set\helmet\model\helmet_mf.p3d";
			female="\set_Incite\set\helmet\model\helmet_mf.p3d";
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=500;
					transferToAttachmentsCoef=0.5;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"\set_Incite\set\helmet\data\helmet.rvmat",
								"\set_Incite\set\helmet\data\konektori.rvmat",
								"\set_Incite\set\helmet\data\nakladki_shlem.rvmat",
								"\set_Incite\set\helmet\data\trubki.rvmat",
								"\set_Incite\set\helmet\data\vizor.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"\set_Incite\set\helmet\data\helmet.rvmat",
								"\set_Incite\set\helmet\data\konektori.rvmat",
								"\set_Incite\set\helmet\data\nakladki_shlem.rvmat",
								"\set_Incite\set\helmet\data\trubki.rvmat",
								"\set_Incite\set\helmet\data\vizor.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"\set_Incite\set\helmet\data\helmet_damage.rvmat",
								"\set_Incite\set\helmet\data\konektori_damage.rvmat",
								"\set_Incite\set\helmet\data\nakladki_shlem_damage.rvmat",
								"\set_Incite\set\helmet\data\trubki_damage.rvmat",
								"\set_Incite\set\helmet\data\vizor_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"\set_Incite\set\helmet\data\helmet_damage.rvmat",
								"\set_Incite\set\helmet\data\konektori_damage.rvmat",
								"\set_Incite\set\helmet\data\nakladki_shlem_damage.rvmat",
								"\set_Incite\set\helmet\data\trubki_damage.rvmat",
								"\set_Incite\set\helmet\data\vizor_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"\set_Incite\set\helmet\data\helmet_destruct.rvmat",
								"\set_Incite\set\helmet\data\konektori_destruct.rvmat",
								"\set_Incite\set\helmet\data\nakladki_shlem_destruct.rvmat",
								"\set_Incite\set\helmet\data\trubki_destruct.rvmat",
								"\set_Incite\set\helmet\data\vizor_destruct.rvmat"
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
						damage=0;
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
						damage=0;
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
	class Vp_Set17_Helmet_Black: Vp_Set17_Helmet_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"\set_Incite\set\helmet\data\helmet_co.paa",
			"\set_Incite\set\helmet\data\konektori_co.paa",
			"\set_Incite\set\helmet\data\nakladki_shlem_co.paa",
			"\set_Incite\set\helmet\data\trubki_co.paa",
			"\set_Incite\set\helmet\data\vizor_ca.paa"
		};
	};
};
