class CfgPatches
{
	class FOG_Operator_Boots_shit
	{
		units[]=
		{
			"FOG_Operator_Boots_CB",
			"FOG_Operator_Boots_Black",
			"FOG_Operator_Boots_RG"
		};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Characters",
			"DZ_Characters_Shoes"
		};
	};
};
class CfgVehicles
{
	class Clothing;
	class FOG_Operator_Boots_ColorBase: Clothing
	{
		scope=0;
		displayName="Operator Combat Boots";
		descriptionShort="Operator Combat Boots — сверхпрочные тактические ботинки с интегрированным слотом для скрытого ношения ножа, изготовленные из баллистического нейлона и термостойкой кожи. Усиленная стальным подноском и антипрокольной пластиной подошва выдерживает экстремальные боевые нагрузки | The Operator Combat Boots are heavy-duty tactical footwear with integrated concealed knife slot, constructed from ballistic nylon and heat-resistant leather. Steel toe and puncture-proof sole provide ultimate protection in combat conditions.";
		model="\FOG_MOD\Gear\Shoes\FOG_Operator_Boots\FOG_Operator_Boots_G.p3d";
		inventorySlot[]=
		{
			"Feet"
		};
		itemInfo[]=
		{
			"Clothing",
			"Feet"
		};
		itemSize[]={3,3};
		attachments[] = {"knife"};
		weight=400;
		varWetMax=0.10000001;
		heatIsolation=0.69999999;
		repairableWithKits[]={5,2};
		repairCosts[]={25,25};
		soundAttType="Boots";
		hiddenSelections[]=
		{
			"camo"
		};
		hiddenSelectionsMaterials[]=
		{
			"FOG_MOD\Gear\Shoes\FOG_Operator_Boots\data\operator_boots.rvmat"
		};
		class ClothingTypes
		{
			male="\FOG_MOD\Gear\Shoes\FOG_Operator_Boots\FOG_Operator_Boots_M.p3d";
			female="\FOG_MOD\Gear\Shoes\FOG_Operator_Boots\FOG_Operator_Boots_F.p3d";
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=1100;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"FOG_MOD\Gear\Shoes\FOG_Operator_Boots\data\operator_boots.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"FOG_MOD\Gear\Shoes\FOG_Operator_Boots\data\operator_boots.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"FOG_MOD\Gear\Shoes\FOG_Operator_Boots\data\operator_boots_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"FOG_MOD\Gear\Shoes\FOG_Operator_Boots\data\operator_boots_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"FOG_MOD\Gear\Shoes\FOG_Operator_Boots\data\operator_boots_destruct.rvmat"
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
                        damage=0.7;
                    };
                };
            };
		class AnimEvents
		{
			class SoundWeapon
			{
				class pickUpItem
				{
					soundSet="AthleticShoes_pickup_SoundSet";
					id=797;
				};
				class drop
				{
					soundset="AthleticShoes_drop_SoundSet";
					id=898;
				};
			};
		};
	};
	class FOG_Operator_Boots_CB: FOG_Operator_Boots_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Gear\Shoes\FOG_Operator_Boots\data\Operator_Boot_CB_co.paa"
		};
	};
	class FOG_Operator_Boots_Black: FOG_Operator_Boots_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Gear\Shoes\FOG_Operator_Boots\data\Operator_Boot_BLK_co.paa"
		};
	};
	class FOG_Operator_Boots_RG: FOG_Operator_Boots_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Gear\Shoes\FOG_Operator_Boots\data\Operator_Boot_RG_co.paa"
		};
	};
};
