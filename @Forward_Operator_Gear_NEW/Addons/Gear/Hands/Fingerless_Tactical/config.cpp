class CfgPatches
{
	class FOG_Tactical_Fingerless_Gloves_stuff
	{
		units[]=
		{
			"FOG_Tactical_Fingerless_Gloves_Green",
			"FOG_Tactical_Fingerless_Gloves_Black",
			"FOG_Tactical_Fingerless_Gloves_Tan",
			"FOG_Tactical_Fingerless_Gloves_MC",
			"FOG_Tactical_Fingerless_Gloves_Red"
		};
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
	class FOG_Tactical_Fingerless_Gloves_ColorBase: Clothing
	{
		scope=0;
		displayName="Tactical Fingerless Gloves";
		descriptionShort="Tactical Fingerless Gloves — гибридные перчатки с открытыми пальцами для точных операций, сочетающие защиту ладоней и полную свободу движений. Усиленные зоны из термостойкого материала и перфорированные вставки обеспечивают вентиляцию при длительном ношении | The Tactical Fingerless Gloves are hybrid gloves with exposed fingertips for precision tasks, blending palm protection with unrestricted dexterity. Reinforced heat-resistant panels and perforated inserts provide airflow during extended wear.";
		model="\FOG_MOD\Gear\Hands\Fingerless_Tactical\Tactical_Fingerless_G.p3d";
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
		hiddenSelections[]=
		{
			"camo",
			"personality"
		};
		rotationFlags=34;
		weight=50;
		itemSize[]={2,2};
		varWetMax=0.29000002;
		heatIsolation=0.30000001;
		repairableWithKits[]={5,2};
		repairCosts[]={30,25};
		class ClothingTypes
		{
			male="\FOG_MOD\Gear\Hands\Fingerless_Tactical\Tactical_Fingerless_M.p3d";
			female="\FOG_MOD\Gear\Hands\Fingerless_Tactical\Tactical_Fingerless_F.p3d";
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=550;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"FOG_MOD\Gear\Hands\Fingerless_Tactical\data\fingerless_tactical.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"FOG_MOD\Gear\Hands\Fingerless_Tactical\data\fingerless_tactical.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"FOG_MOD\Gear\Hands\Fingerless_Tactical\data\fingerless_tactical_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"FOG_MOD\Gear\Hands\Fingerless_Tactical\data\fingerless_tactical_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"FOG_MOD\Gear\Hands\Fingerless_Tactical\data\fingerless_tactical_destruct.rvmat"
							}
						}
					};
				};
			};
		};
		class GlobalArmor
            {
                class Melee
                {
                    class Health
                    {
                        damage=0.89999998;
                    };
                    class Blood
                    {
                        damage=0.89999998;
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
                        damage=0.89999998;
                    };
                    class Shock
                    {
                        damage=1;
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
	class FOG_Tactical_Fingerless_Gloves_Green: FOG_Tactical_Fingerless_Gloves_ColorBase
	{
		scope=2;
		visibilityModifier=0.80000001;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Gear\Hands\Fingerless_Tactical\data\fingerless_tactical_green_co.paa",
			""
		};
	};
	class FOG_Tactical_Fingerless_Gloves_Black: FOG_Tactical_Fingerless_Gloves_ColorBase
	{
		scope=2;
		visibilityModifier=0.80000001;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Gear\Hands\Fingerless_Tactical\data\fingerless_tactical_black_co.paa",
			""
		};
	};
	class FOG_Tactical_Fingerless_Gloves_Tan: FOG_Tactical_Fingerless_Gloves_ColorBase
	{
		scope=2;
		visibilityModifier=0.80000001;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Gear\Hands\Fingerless_Tactical\data\fingerless_tactical_tan_co.paa",
			""
		};
	};
	class FOG_Tactical_Fingerless_Gloves_MC: FOG_Tactical_Fingerless_Gloves_ColorBase
	{
		scope=2;
		visibilityModifier=0.80000001;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Gear\Hands\Fingerless_Tactical\data\fingerless_tactical_MC_co.paa",
			""
		};
	};
	class FOG_Tactical_Fingerless_Gloves_Red: FOG_Tactical_Fingerless_Gloves_ColorBase
	{
		scope=2;
		visibilityModifier=0.80000001;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Gear\Hands\Fingerless_Tactical\data\fingerless_tactical_red_co.paa",
			""
		};
	};
};
