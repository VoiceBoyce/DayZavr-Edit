class CfgPatches
{
	class Vp_SET14
	{
		units[]=
		{
			"Vp_SET14_Gloves_Black"
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
	class Clothing;
	class Vp_SET14_Gloves_ColorBase: Clothing
	{
		scope=0;
		displayName="$STR_Ghost_Gloves";
		descriptionShort="";
		model="set_Ghost\set\Gloves\model\Gloves_g.p3d";
		inventorySlot[]=
		{
			"Gloves"
		};
		simulation="clothing";
		itemInfo[]=
		{
			"Clothing",
			"Gloves"
		};
		rotationFlags=34;
		weight=454;
		itemSize[]={2,2};
		varWetMax=0.048999999;
		heatIsolation=1;
		repairableWithKits[]={5,2};
		repairCosts[]={30,25};
		hiddenSelections[]=
		{
			"camo",
			"personality"
		};
		class ClothingTypes
		{
			male="set_Ghost\set\Gloves\model\Gloves_m.p3d";
			female="set_Ghost\set\Gloves\model\Gloves_f.p3d";
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
								"set_Ghost\set\Gloves\data\Gloves.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"set_Ghost\set\Gloves\data\Gloves.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"set_Ghost\set\Gloves\data\Gloves_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"set_Ghost\set\Gloves\data\Gloves_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"set_Ghost\set\Gloves\data\Gloves_destruct.rvmat"
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
						damage=0.85000002;
					};
					class Blood
					{
						damage=0.80000001;
					};
					class Shock
					{
						damage=0.80000001;
					};
				};
				class Melee
				{
					class Health
					{
						damage=0.85000002;
					};
					class Blood
					{
						damage=0.80000001;
					};
					class Shock
					{
						damage=0.80000001;
					};
				};
				class Infected
				{
					class Health
					{
						damage=0.85000002;
					};
					class Blood
					{
						damage=0.80000001;
					};
					class Shock
					{
						damage=0.80000001;
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
		class AnimEvents
		{
			class SoundWeapon
			{
				class pickUpItem
				{
					soundSet="WorkingGloves_pickup_SoundSet";
					id=797;
				};
				class drop
				{
					soundset="WorkingGloves_drop_SoundSet";
					id=898;
				};
			};
		};
	};
	class VP_SET14_Gloves_Black: Vp_SET14_Gloves_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"set_Ghost\set\Gloves\data\Gloves_co.paa"
		};
	};
};
