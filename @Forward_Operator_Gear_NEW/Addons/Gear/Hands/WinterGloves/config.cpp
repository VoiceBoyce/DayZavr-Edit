class CfgPatches
{
	class FOG_Gear_Hands_WinterGloves
	{
		units[]=
		{
			"FOG_WinterGloves_Tan",
			"FOG_WinterGloves_White",
			"FOG_WinterGloves_Black",
			"FOG_WinterGloves_CB",
			"FOG_WinterGloves_RG",
			"FOG_WinterGloves_MC",
			"FOG_WinterGloves_MCB",
			"FOG_WinterGloves_MCAL",
			"FOG_WinterGloves_Red"
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
	class FOG_WinterGloves_Base: Clothing
	{
		scope=0;
		displayName="Winter Combat Gloves";
		descriptionShort="Winterized Combat Gloves — утеплённые тактические перчатки для экстремально низких температур, сочетающие ветрозащитную мембрану и термоизоляцию с сохранением подвижности пальцев. Усиленные ладони и антискользящее покрытие обеспечивают надёжный хват оружия даже в метель | The Winterized Combat Gloves are insulated tactical gloves for extreme cold, merging windproof membrane with thermal lining while maintaining finger dexterity. Reinforced palms and anti-slip texture guarantee secure weapon grip in blizzard conditions.";
		model="\FOG_MOD\Gear\Hands\WinterGloves\FOG_WinterGloves_G.p3d";
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
		varWetMax=0.249;
		heatIsolation=0.89999998;
		repairableWithKits[]={5,2};
		repairCosts[]={30,25};
		class ClothingTypes
		{
			male="\FOG_MOD\Gear\Hands\WinterGloves\FOG_WinterGloves_M.p3d";
			female="\FOG_MOD\Gear\Hands\WinterGloves\FOG_WinterGloves_F.p3d";
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=800;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"FOG_MOD\Gear\Hands\WinterGloves\data\WinterGloves.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"FOG_MOD\Gear\Hands\WinterGloves\data\WinterGloves.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"FOG_MOD\Gear\Hands\WinterGloves\data\WinterGloves.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"FOG_MOD\Gear\Hands\WinterGloves\data\WinterGloves_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"FOG_MOD\Gear\Hands\WinterGloves\data\WinterGloves_destruct.rvmat"
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
	class FOG_WinterGloves_Tan: FOG_WinterGloves_Base
	{
		scope=2;
		visibilityModifier=0.80000001;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Gear\Hands\WinterGloves\data\WinterGloves_Tan_co.paa"
		};
	};
	class FOG_WinterGloves_White: FOG_WinterGloves_Base
	{
		scope=2;
		visibilityModifier=0.80000001;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Gear\Hands\WinterGloves\data\WinterGloves_White_co.paa"
		};
	};
	class FOG_WinterGloves_Black: FOG_WinterGloves_Base
	{
		scope=2;
		visibilityModifier=0.80000001;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Gear\Hands\WinterGloves\data\WinterGloves_Black_co.paa"
		};
	};
	class FOG_WinterGloves_CB: FOG_WinterGloves_Base
	{
		scope=2;
		visibilityModifier=0.80000001;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Gear\Hands\WinterGloves\data\WinterGloves_CB_co.paa"
		};
	};
	class FOG_WinterGloves_RG: FOG_WinterGloves_Base
	{
		scope=2;
		visibilityModifier=0.80000001;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Gear\Hands\WinterGloves\data\WinterGloves_RG_co.paa"
		};
	};
	class FOG_WinterGloves_MC: FOG_WinterGloves_Base
	{
		scope=2;
		visibilityModifier=0.80000001;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Gear\Hands\WinterGloves\data\WinterGloves_MC_co.paa"
		};
	};
	class FOG_WinterGloves_MCB: FOG_WinterGloves_Base
	{
		scope=2;
		visibilityModifier=0.80000001;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Gear\Hands\WinterGloves\data\WinterGloves_MCB_co.paa"
		};
	};
	class FOG_WinterGloves_MCAL: FOG_WinterGloves_Base
	{
		scope=2;
		visibilityModifier=0.80000001;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Gear\Hands\WinterGloves\data\WinterGloves_MCAL_co.paa"
		};
	};
	class FOG_WinterGloves_Red: FOG_WinterGloves_Base
	{
		scope=2;
		visibilityModifier=0.80000001;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Gear\Hands\WinterGloves\data\WinterGloves_Red_co.paa"
		};
	};
};
