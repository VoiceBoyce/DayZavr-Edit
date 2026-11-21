class CfgPatches
{
	class KOD_Weapons_Grips
	{
		units[]=
		{
			"KOD_Grip_RK2_Black",
			"KOD_Grip_Angled_FDE",
			"KOD_Grip_Shift_Black",
			"KOD_Grip_Vertical_Black"
		};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Data"
		};
		magazines[]={};
	};
};
class cfgVehicles
{
	class Inventory_Base;
	class KOD_Grip_RK2_Base: Inventory_Base
	{
		scope=0;
		displayName="Рукоятка Зенит РК-2";
		descriptionShort="Полноразмерная тактическая рукоятка РК-2 для установки на цевье. Уменьшает вертикальную отдачу на 20%, горизонтальную на 30%.";
		model="KOD_weapons\attachments\Grips\grip_zenitco.p3d";
		rotationFlags=12;
		reversed=0;
		inventorySlot[]=
		{
			"KODRISBipod"
		};
		weight=200;
		itemSize[]={1,2};
		recoilModifier[]={0.69999999,0.80000001,1};
		swayModifier[]={0.80000001,0.80000001,0.89999998};
		hiddenSelections[]=
		{
			"color"
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=100;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"KOD_weapons\weapons\PKPTactical\data\pkp_5.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"KOD_weapons\weapons\PKPTactical\data\pkp_5.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"KOD_weapons\weapons\PKPTactical\data\pkp_5_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"KOD_weapons\weapons\PKPTactical\data\pkp_5_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"KOD_weapons\weapons\PKPTactical\data\pkp_5_destruct.rvmat"
							}
						}
					};
				};
			};
		};
		isMeleeWeapon=1;
		class MeleeModes
		{
			class Default
			{
				ammo="MeleeLightBlunt";
				range=1;
			};
			class Heavy
			{
				ammo="MeleeLightBlunt_Heavy";
				range=1;
			};
			class Sprint
			{
				ammo="MeleeLightBlunt_Heavy";
				range=2.8;
			};
		};
	};
	class KOD_Grip_RK2_Black: KOD_Grip_RK2_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\weapons\PKPTactical\data\pkp_5_black_co.paa"
		};
	};
	class KOD_Grip_Angled_Base: Inventory_Base
	{
		scope=0;
		displayName="Рукоятка Угловая";
		descriptionShort="Угловая рукоятка установки на цевье. Уменьшает вертикальную отдачу на 15%, горизонтальную на 15%.";
		model="KOD_weapons\attachments\Grips\grip_angled.p3d";
		rotationFlags=12;
		reversed=0;
		inventorySlot[]=
		{
			"KODRISBipod"
		};
		weight=200;
		itemSize[]={1,2};
		recoilModifier[]={0.85000002,0.85000002,1};
		swayModifier[]={0.80000001,0.80000001,0.89999998};
		hiddenSelections[]=
		{
			"color"
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=100;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"KOD_weapons\weapons\Mk18\data\mk18.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"KOD_weapons\weapons\Mk18\data\mk18.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"KOD_weapons\weapons\Mk18\data\mk18_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"KOD_weapons\weapons\Mk18\data\mk18_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"KOD_weapons\weapons\Mk18\data\mk18_destruct.rvmat"
							}
						}
					};
				};
			};
		};
		isMeleeWeapon=1;
		class MeleeModes
		{
			class Default
			{
				ammo="MeleeLightBlunt";
				range=1;
			};
			class Heavy
			{
				ammo="MeleeLightBlunt_Heavy";
				range=1;
			};
			class Sprint
			{
				ammo="MeleeLightBlunt_Heavy";
				range=2.8;
			};
		};
	};
	class KOD_Grip_Angled_FDE: KOD_Grip_Angled_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\weapons\Mk18\data\mk18_co.paa"
		};
	};
	class KOD_Grip_Shift_Base: Inventory_Base
	{
		scope=0;
		displayName="Рукоятка Fortis Shift";
		descriptionShort="Облегчённая рукоятка установки на цевье. Уменьшает вертикальную отдачу на 10%, горизонтальную на 10%.";
		model="KOD_weapons\attachments\Grips\grip_vertical_skeleton.p3d";
		rotationFlags=12;
		reversed=0;
		inventorySlot[]=
		{
			"KODRISBipod"
		};
		weight=200;
		itemSize[]={1,2};
		recoilModifier[]={0.89999998,0.89999998,1};
		swayModifier[]={0.80000001,0.80000001,0.89999998};
		hiddenSelections[]=
		{
			"color"
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=100;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"KOD_weapons\weapons\Raptr\data\raptr.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"KOD_weapons\weapons\Raptr\data\raptr.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"KOD_weapons\weapons\Raptr\data\raptr_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"KOD_weapons\weapons\Raptr\data\raptr_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"KOD_weapons\weapons\Raptr\data\raptr_destruct.rvmat"
							}
						}
					};
				};
			};
		};
		isMeleeWeapon=1;
		class MeleeModes
		{
			class Default
			{
				ammo="MeleeLightBlunt";
				range=1;
			};
			class Heavy
			{
				ammo="MeleeLightBlunt_Heavy";
				range=1;
			};
			class Sprint
			{
				ammo="MeleeLightBlunt_Heavy";
				range=2.8;
			};
		};
	};
	class KOD_Grip_Shift_Black: KOD_Grip_Shift_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\weapons\Raptr\data\raptr_co.paa"
		};
	};
	class KOD_Grip_Vertical_Base: Inventory_Base
	{
		scope=0;
		displayName="Рукоятка Вертикальная";
		descriptionShort="Полноразмерная вертикальная рукоятка установки на цевье. Уменьшает вертикальную отдачу на 20%, горизонтальную на 10%.";
		model="KOD_weapons\attachments\Grips\grip_vertical.p3d";
		rotationFlags=12;
		reversed=0;
		inventorySlot[]=
		{
			"KODRISBipod"
		};
		weight=200;
		itemSize[]={1,2};
		recoilModifier[]={0.89999998,0.80000001,1};
		swayModifier[]={0.80000001,0.80000001,0.89999998};
		hiddenSelections[]=
		{
			"color"
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=100;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"kod_weapons\weapons\hk21\data\verticalgrip.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"kod_weapons\weapons\hk21\data\verticalgrip.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"kod_weapons\weapons\hk21\data\verticalgrip_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"kod_weapons\weapons\hk21\data\verticalgrip_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"kod_weapons\weapons\hk21\data\verticalgrip_destruct.rvmat"
							}
						}
					};
				};
			};
		};
		isMeleeWeapon=1;
		class MeleeModes
		{
			class Default
			{
				ammo="MeleeLightBlunt";
				range=1;
			};
			class Heavy
			{
				ammo="MeleeLightBlunt_Heavy";
				range=1;
			};
			class Sprint
			{
				ammo="MeleeLightBlunt_Heavy";
				range=2.8;
			};
		};
	};
	class KOD_Grip_Vertical_Black: KOD_Grip_Vertical_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"kod_weapons\weapons\hk21\data\verticalgrip_co.paa"
		};
	};
};
