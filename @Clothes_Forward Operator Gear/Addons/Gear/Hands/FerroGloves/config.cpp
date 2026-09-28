class CfgPatches
{
	class FOG_MOD_Hands_FerroGloves
	{
		units[]=
		{
			"FOG_FerroGloves_Tan",
			"FOG_FerroGloves_Grey",
			"FOG_FerroGloves_Red",
			"FOG_FerroGloves_Blue",
			"FOG_FerroGloves_White",
			"FOG_FerroGloves_White_TanTrim",
			"FOG_FerroGloves_CB",
			"FOG_FerroGloves_RG",
			"FOG_FerroGloves_Black",
			"FOG_FerroGloves_Black_WhiteTrim",
			"FOG_FerroGloves_Black_BlueTrim",
			"FOG_FerroGloves_Black_RedTrim",
			"FOG_FerroGloves_Black_TanTrim",
			"FOG_FerroGloves_MC",
			"FOG_FerroGloves_MCB",
			"FOG_FerroGloves_M81",
			"FOG_FerroGloves_AJ"
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
	class FOG_FerroGloves_ColorBase: Clothing
	{
		scope=0;
		displayName="Ferro Concepts Gloves";
		descriptionShort="Ferro Concepts Gloves — тактические перчатки с усиленными зонами на ладонях и пальцах, обеспечивающие надежный хват и защиту при работе с оружием и снаряжением. Изготовлены из дышащего износостойкого материала с улучшенной тактильной чувствительностью | The Ferro Concepts Gloves are tactical gloves with reinforced palm and finger areas, offering secure grip and protection when handling weapons and gear. Made from breathable durable material with enhanced tactile sensitivity.";
		model="\FOG_MOD\Gear\Hands\FerroGloves\FOG_FerroGloves_G.p3d";
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
		varWetMax=0.149;
		heatIsolation=0.5;
		repairableWithKits[]={5,2};
		repairCosts[]={30,25};
		visibilityModifier=0.80000001;
		class ClothingTypes
		{
			male="\FOG_MOD\Gear\Hands\FerroGloves\FOG_FerroGloves_M.p3d";
			female="\FOG_MOD\Gear\Hands\FerroGloves\FOG_FerroGloves_F.p3d";
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=700;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"FOG_MOD\Gear\Hands\FerroGloves\data\Ferro_Gloves.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"FOG_MOD\Gear\Hands\FerroGloves\data\Ferro_Gloves.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"FOG_MOD\Gear\Hands\FerroGloves\data\Ferro_Gloves.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"FOG_MOD\Gear\Hands\FerroGloves\data\Ferro_Gloves.rvmat"
							}
						},
						
						{
							0,
							
							{
								"FOG_MOD\Gear\Hands\FerroGloves\data\Ferro_Gloves.rvmat"
							}
						}
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
	class FOG_FerroGloves_Tan: FOG_FerroGloves_ColorBase
	{
		scope=2;
		visibilityModifier=0.80000001;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Gear\Hands\FerroGloves\data\Ferro_Gloves_Tan_co.paa"
		};
	};
	class FOG_FerroGloves_Grey: FOG_FerroGloves_ColorBase
	{
		scope=2;
		visibilityModifier=0.80000001;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Gear\Hands\FerroGloves\data\Ferro_Gloves_Grey_co.paa"
		};
	};
	class FOG_FerroGloves_Red: FOG_FerroGloves_ColorBase
	{
		scope=2;
		visibilityModifier=0.80000001;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Gear\Hands\FerroGloves\data\Ferro_Gloves_Red_co.paa"
		};
	};
	class FOG_FerroGloves_Blue: FOG_FerroGloves_ColorBase
	{
		scope=2;
		visibilityModifier=0.80000001;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Gear\Hands\FerroGloves\data\Ferro_Gloves_Blue_co.paa"
		};
	};
	class FOG_FerroGloves_White: FOG_FerroGloves_ColorBase
	{
		scope=2;
		visibilityModifier=0.80000001;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Gear\Hands\FerroGloves\data\Ferro_Gloves_White_Trim_Black_co.paa"
		};
	};
	class FOG_FerroGloves_White_TanTrim: FOG_FerroGloves_ColorBase
	{
		scope=2;
		visibilityModifier=0.80000001;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Gear\Hands\FerroGloves\data\Ferro_Gloves_GunGlove_co.paa"
		};
	};
	class FOG_FerroGloves_CB: FOG_FerroGloves_ColorBase
	{
		scope=2;
		visibilityModifier=0.80000001;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Gear\Hands\FerroGloves\data\Ferro_Gloves_CB_co.paa"
		};
	};
	class FOG_FerroGloves_RG: FOG_FerroGloves_ColorBase
	{
		scope=2;
		visibilityModifier=0.80000001;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Gear\Hands\FerroGloves\data\Ferro_Gloves_RG_co.paa"
		};
	};
	class FOG_FerroGloves_Black: FOG_FerroGloves_ColorBase
	{
		scope=2;
		visibilityModifier=0.80000001;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Gear\Hands\FerroGloves\data\Ferro_Gloves_Black_co.paa"
		};
	};
	class FOG_FerroGloves_Black_WhiteTrim: FOG_FerroGloves_ColorBase
	{
		scope=2;
		visibilityModifier=0.80000001;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Gear\Hands\FerroGloves\data\Ferro_Gloves_Black_Trim_White_co.paa"
		};
	};
	class FOG_FerroGloves_Black_BlueTrim: FOG_FerroGloves_ColorBase
	{
		scope=2;
		visibilityModifier=0.80000001;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Gear\Hands\FerroGloves\data\Ferro_Gloves_Black_Trim_Blue_co.paa"
		};
	};
	class FOG_FerroGloves_Black_RedTrim: FOG_FerroGloves_ColorBase
	{
		scope=2;
		visibilityModifier=0.80000001;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Gear\Hands\FerroGloves\data\Ferro_Gloves_Black_Trim_Red_co.paa"
		};
	};
	class FOG_FerroGloves_Black_TanTrim: FOG_FerroGloves_ColorBase
	{
		scope=2;
		visibilityModifier=0.80000001;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Gear\Hands\FerroGloves\data\Ferro_Gloves_Black_Trim_Tan_co.paa"
		};
	};
	class FOG_FerroGloves_MC: FOG_FerroGloves_ColorBase
	{
		scope=2;
		visibilityModifier=0.80000001;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Gear\Hands\FerroGloves\data\Ferro_Gloves_MC_co.paa"
		};
	};
	class FOG_FerroGloves_MCB: FOG_FerroGloves_ColorBase
	{
		scope=2;
		visibilityModifier=0.80000001;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Gear\Hands\FerroGloves\data\Ferro_Gloves_MCB_co.paa"
		};
	};
	class FOG_FerroGloves_M81: FOG_FerroGloves_ColorBase
	{
		scope=2;
		visibilityModifier=0.80000001;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Gear\Hands\FerroGloves\data\Ferro_Gloves_M81_co.paa"
		};
	};
	class FOG_FerroGloves_AJ: FOG_FerroGloves_ColorBase
	{
		scope=2;
		visibilityModifier=0.80000001;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Gear\Hands\FerroGloves\data\Ferro_Gloves_AJ_co.paa"
		};
	};
};
