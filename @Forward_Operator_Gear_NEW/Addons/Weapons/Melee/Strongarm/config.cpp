class CfgPatches
{
	class FOG_MOD_Knife_Strongarm
	{
		units[]=
		{
			"FOG_Strongarm_Knife"
		};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Weapons_Melee",
			"DZ_Weapons_Melee_Blade"
		};
	};
};
class CfgVehicles
{
	class HuntingKnife;
	class FOG_Strongarm_Knife_Base: HuntingKnife
	{
		scope=0;
		displayName="Gerber Strongarm Knife";
		descriptionShort="Gerber Strongarm Knife — это исключительно прочный тактический нож, изготовленный в Портленде из высокоуглеродистой стали 420HC с матовым антибликовым покрытием. Тонкое заточенное лезвие и усиленный хвостовик на всю длину клинка обеспечивают идеальный баланс режущих характеристик и ударной прочности | The Gerber Strongarm Knife is an ultra-durable tactical blade handmade in Portland from 420HC high-carbon steel with matte finish. Its full-tang construction and precision-ground edge deliver perfect balance between cutting performance and impact resistance.";
		model="\FOG_MOD\Weapons\Melee\Strongarm\FOG_Strongarm_Knife.p3d";
		debug_ItemCategory=2;
		inventorySlot[]=
		{
			"Knife"
		};
		repairableWithKits[]={4};
		repairCosts[]={10};
		rotationFlags=17;
		RestrainUnlockType=1;
		canSkinBodies=1;
		weight=200;
		itemSize[]={1,3};
		lootTag[]=
		{
			"Hunting",
			"Camping"
		};
		lootCategory="Tools";
		itemInfo[]=
		{
			"Knife"
		};
		openItemSpillRange[]={10,20};
		isMeleeWeapon=1;
		suicideAnim="onehanded";
		hiddenSelections[]=
		{
			"camo"
		};
		hiddenSelectionsMaterials[]=
		{
			"FOG_MOD\Weapons\Melee\Strongarm\data\GerberStrongarm.rvmat"
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
								"FOG_MOD\Weapons\Melee\Strongarm\data\GerberStrongarm.rvmat"
							}
						},
						
						{
							0.75,
							
							{
								"FOG_MOD\Weapons\Melee\Strongarm\data\GerberStrongarm.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"FOG_MOD\Weapons\Melee\Strongarm\data\GerberStrongarm.rvmat"
							}
						},
						
						{
							0.25,
							
							{
								"FOG_MOD\Weapons\Melee\Strongarm\data\GerberStrongarm_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"FOG_MOD\Weapons\Melee\Strongarm\data\GerberStrongarm_destruct.rvmat"
							}
						}
					};
				};
			};
		};
	};
	class FOG_Strongarm_Knife: FOG_Strongarm_Knife_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Weapons\Melee\Strongarm\data\GerberStrongarm_co.paa"
		};
	};
};
