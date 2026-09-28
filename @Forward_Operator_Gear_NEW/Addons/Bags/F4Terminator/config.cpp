class CfgPatches
{
	class FOG_Bag_F4Terminator_shit
	{
		units[]=
		{
			"FOG_Bag_F4Terminator",
			"FOG_Bag_F4Terminator_BlackTiger",
			"FOG_Bag_F4Terminator_Black",
			"FOG_Bag_F4Terminator_RG",
			"FOG_Bag_F4Terminator_OD",
			"FOG_Bag_F4Terminator_CB"
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
	class FOG_Bag_F4Terminator_ColorBase: Clothing
	{
		scope=0;
		displayName="Terminator Backpack";
		descriptionShort="Рюкзак Терминатор — это монструозная 200-литровая модель с огромными боковыми карманами и усиленными молниями, способная выдержать экстремальные нагрузки и перевозку крупногабаритного снаряжения | The Terminator backpack is a monstrous 200-liter model with massive side pockets and reinforced zippers, designed to withstand extreme loads and transportation of bulky gear.";
		model="\FOG_MOD\Bags\F4Terminator\models\F4Terminator_g.p3d";
		inventorySlot[]=
		{
			"Back"
		};
		itemInfo[]=
		{
			"Clothing",
			"Back"
		};
		simulation="clothing";
		vehicleClass="Clothing";
		quickBarBonus=3;
		itemsCargoSize[]={10,20};
		weight=1600;
		varWetMax=0.249;
		heatIsolation=0.69999999;
		repairableWithKits[]={5,2};
		repairCosts[]={30,25};
		soundAttType="Outdoor";
		rotationFlags=16;
		randomQuantity=4;
		canBeDigged=0;
		attachments[]=
		{
			"Chemlight",
			"WalkieTalkie",
			"Backpack_1"
		};
		hiddenSelections[]=
		{
			"camoflage"
		};
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Bags\F4Terminator\data\F4Terminator_co.paa"
		};
		class ClothingTypes
		{
			male="\FOG_MOD\Bags\F4Terminator\models\F4Terminator_m.p3d";
			female="\FOG_MOD\Bags\F4Terminator\models\F4Terminator_m.p3d";
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=2100;
					transferToAttachmentsCoef=0.5;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"FOG_MOD\Bags\F4Terminator\data\F4Terminator.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"FOG_MOD\Bags\F4Terminator\data\F4Terminator.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"FOG_MOD\Bags\F4Terminator\data\F4Terminator_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"FOG_MOD\Bags\F4Terminator\data\F4Terminator_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"FOG_MOD\Bags\F4Terminator\data\F4Terminator_destruct.rvmat"
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
				class pickUpItem_Light
				{
					soundSet="pickUpBackPack_Plastic_Light_SoundSet";
					id=796;
				};
				class pickUpItem
				{
					soundSet="pickUpBackPack_Plastic_SoundSet";
					id=797;
				};
				class drop
				{
					soundset="taloonbag_drop_SoundSet";
					id=898;
				};
			};
		};
	};
	class FOG_Bag_F4Terminator: FOG_Bag_F4Terminator_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Bags\F4Terminator\data\F4Terminator_co.paa"
		};
	};
	class FOG_Bag_F4Terminator_BlackTiger: FOG_Bag_F4Terminator_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Bags\F4Terminator\data\F4Terminator_black_Tiger_co.paa"
		};
	};
	class FOG_Bag_F4Terminator_Black: FOG_Bag_F4Terminator_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Bags\F4Terminator\data\F4Terminator_black_co.paa"
		};
	};
	class FOG_Bag_F4Terminator_RG: FOG_Bag_F4Terminator_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Bags\F4Terminator\data\F4Terminator_RG_co.paa"
		};
	};
	class FOG_Bag_F4Terminator_OD: FOG_Bag_F4Terminator_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Bags\F4Terminator\data\F4Terminator_OD_co.paa"
		};
	};
	class FOG_Bag_F4Terminator_CB: FOG_Bag_F4Terminator_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Bags\F4Terminator\data\F4Terminator_CB_co.paa"
		};
	};
};
