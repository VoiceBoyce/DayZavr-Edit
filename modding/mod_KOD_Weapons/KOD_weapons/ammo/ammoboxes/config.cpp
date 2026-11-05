class CfgPatches
{
	class KOD_Weapons_AmmoBoxes
	{
		units[]=
		{
			"KOD_AmmoBox_458_30Rnd",
			"KOD_AmmoBox_50BMG_10Rnd",
			"KOD_AmmoBox_50calBW_20Rnd",
			"KOD_AmmoBox_127x55_20Rnd",
			"KOD_AmmoBox_127x108_10Rnd",
			"KOD_AmmoBox_145x114_10Rnd",
			"KOD_AmmoBox_338_20Rnd",
			"KOD_AmmoBox_408_10Rnd",
			"KOD_AmmoBox_20mm_5Rnd"
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
	class Box_Base;
	class Inventory_Base;
	class KOD_AmmoBox_458_30Rnd: Box_Base
	{
		scope=2;
		displayName="Коробка патронов .458 SOCOM";
		descriptionShort="Патроны .458 SOCOM в коробке.";
		model="\dz\weapons\ammunition\545x39_20RoundBox.p3d";
		rotationFlags=1;
		weight=300;
		hiddenSelections[]=
		{
			"zbytek"
		};
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\ammo\Ammoboxes\data\ammobox_458_co.paa"
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
								"DZ\weapons\ammunition\data\545x39box.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"DZ\weapons\ammunition\data\545x39box.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"DZ\weapons\ammunition\data\545x39box_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"DZ\weapons\ammunition\data\545x39box_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"DZ\weapons\ammunition\data\545x39box_destruct.rvmat"
							}
						}
					};
				};
			};
		};
		class Resources
		{
			class KOD_Ammo_458
			{
				value=30;
				variable="quantity";
			};
		};
		class AnimEvents
		{
			class SoundWeapon
			{
				class interact
				{
					soundset="ammoboxUnpack_SoundSet";
					id=70;
				};
			};
		};
	};
	class KOD_AmmoBox_50BMG_10Rnd: Box_Base
	{
		scope=2;
		displayName="Коробка патронов .50 BMG";
		descriptionShort="Патроны .50 BMG в коробке.";
		model="KOD_weapons\ammo\Ammoboxes\ammobox.p3d";
		itemSize[]={2,3};
		rotationFlags=1;
		weight=300;
		hiddenSelections[]=
		{
			"color"
		};
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\ammo\Ammoboxes\data\ammobox_50BMG_co.paa"
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
								"KOD_weapons\ammo\Ammoboxes\data\762_box.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"KOD_weapons\ammo\Ammoboxes\data\762_box.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"KOD_weapons\ammo\Ammoboxes\data\762_box_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"KOD_weapons\ammo\Ammoboxes\data\762_box_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"KOD_weapons\ammo\Ammoboxes\data\762_box_destruct.rvmat"
							}
						}
					};
				};
			};
		};
		class Resources
		{
			class KOD_Ammo_50BMG
			{
				value=10;
				variable="quantity";
			};
		};
		class AnimEvents
		{
			class SoundWeapon
			{
				class interact
				{
					soundset="ammoboxUnpack_SoundSet";
					id=70;
				};
			};
		};
	};
	class KOD_AmmoBox_50calBW_20Rnd: Box_Base
	{
		scope=2;
		displayName="Коробка патронов 12,7x42 мм";
		descriptionShort="Патроны 12,7x42 мм в коробке.";
		model="\dz\weapons\ammunition\762_20RoundBox.p3d";
		itemSize[]={3,2};
		rotationFlags=1;
		weight=500;
		hiddenSelections[]=
		{
			"zbytek",
			"camoground"
		};
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\ammo\Ammoboxes\data\ammobox_50BW_co.paa",
			"KOD_weapons\ammo\Ammoboxes\data\ammobox_50BW_co.paa"
		};
		hiddenSelectionsMaterials[]=
		{
			"KOD_weapons\ammo\Ammoboxes\data\762_box.rvmat",
			"KOD_weapons\ammo\Ammoboxes\data\762_box.rvmat"
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=100;
					RefTexsMats[]=
					{
						"DZ\weapons\ammunition\data\762_box.rvmat"
					};
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"KOD_weapons\ammo\Ammoboxes\data\762_box.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"KOD_weapons\ammo\Ammoboxes\data\762_box.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"KOD_weapons\ammo\Ammoboxes\data\762_box_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"KOD_weapons\ammo\Ammoboxes\data\762_box_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"KOD_weapons\ammo\Ammoboxes\data\762_box_destruct.rvmat"
							}
						}
					};
				};
			};
		};
		class Resources
		{
			class Ammo_50calBW
			{
				value=20;
				variable="quantity";
			};
		};
		class AnimEvents
		{
			class SoundWeapon
			{
				class interact
				{
					soundset="ammoboxUnpack_SoundSet";
					id=70;
				};
			};
		};
	};
	class KOD_AmmoBox_127x55_20Rnd: Box_Base
	{
		scope=2;
		displayName="Коробка патронов 12,7x55 мм";
		descriptionShort="Патроны 12,7x55 мм в коробке.";
		model="\dz\weapons\ammunition\762_20RoundBox.p3d";
		itemSize[]={2,2};
		rotationFlags=1;
		weight=800;
		hiddenSelections[]=
		{
			"zbytek",
			"camoground"
		};
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\ammo\Ammoboxes\data\ammobox_127x55_co.paa",
			"KOD_weapons\ammo\Ammoboxes\data\ammobox_127x55_co.paa"
		};
		hiddenSelectionsMaterials[]=
		{
			"KOD_weapons\ammo\Ammoboxes\data\762_box.rvmat",
			"KOD_weapons\ammo\Ammoboxes\data\762_box.rvmat"
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=100;
					RefTexsMats[]=
					{
						"DZ\weapons\ammunition\data\762_box.rvmat"
					};
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"KOD_weapons\ammo\Ammoboxes\data\762_box.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"KOD_weapons\ammo\Ammoboxes\data\762_box.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"KOD_weapons\ammo\Ammoboxes\data\762_box_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"KOD_weapons\ammo\Ammoboxes\data\762_box_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"KOD_weapons\ammo\Ammoboxes\data\762_box_destruct.rvmat"
							}
						}
					};
				};
			};
		};
		class Resources
		{
			class Ammo_127x55
			{
				value=20;
				variable="quantity";
			};
		};
		class AnimEvents
		{
			class SoundWeapon
			{
				class interact
				{
					soundset="ammoboxUnpack_SoundSet";
					id=70;
				};
			};
		};
	};
	class KOD_AmmoBox_127x108_10Rnd: Box_Base
	{
		scope=2;
		displayName="Коробка патронов 12,7х108 мм";
		descriptionShort="Патроны 12,7х108 мм в коробке.";
		model="KOD_weapons\ammo\Ammoboxes\ammobox.p3d";
		itemSize[]={3,2};
		rotationFlags=1;
		weight=300;
		hiddenSelections[]=
		{
			"color"
		};
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\ammo\Ammoboxes\data\ammobox_127x108_co.paa"
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
								"KOD_weapons\ammo\Ammoboxes\data\762_box.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"KOD_weapons\ammo\Ammoboxes\data\762_box.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"KOD_weapons\ammo\Ammoboxes\data\762_box_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"KOD_weapons\ammo\Ammoboxes\data\762_box_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"KOD_weapons\ammo\Ammoboxes\data\762_box_destruct.rvmat"
							}
						}
					};
				};
			};
		};
		class Resources
		{
			class KOD_Ammo_127x108
			{
				value=10;
				variable="quantity";
			};
		};
		class AnimEvents
		{
			class SoundWeapon
			{
				class interact
				{
					soundset="ammoboxUnpack_SoundSet";
					id=70;
				};
			};
		};
	};
	class KOD_AmmoBox_145x114_10Rnd: Box_Base
	{
		scope=2;
		displayName="Коробка патронов 14,5x114 мм";
		descriptionShort="Патроны 14,5x114 мм в коробке.";
		model="KOD_weapons\ammo\Ammoboxes\ammobox.p3d";
		itemSize[]={2,2};
		rotationFlags=1;
		weight=500;
		hiddenSelections[]=
		{
			"color"
		};
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\ammo\Ammoboxes\data\ammobox_145x114_co.paa"
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
								"KOD_weapons\ammo\Ammoboxes\data\762_box.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"KOD_weapons\ammo\Ammoboxes\data\762_box.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"KOD_weapons\ammo\Ammoboxes\data\762_box_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"KOD_weapons\ammo\Ammoboxes\data\762_box_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"KOD_weapons\ammo\Ammoboxes\data\762_box_destruct.rvmat"
							}
						}
					};
				};
			};
		};
		class Resources
		{
			class KOD_Ammo_145x114
			{
				value=10;
				variable="quantity";
			};
		};
		class AnimEvents
		{
			class SoundWeapon
			{
				class interact
				{
					soundset="ammoboxUnpack_SoundSet";
					id=70;
				};
			};
		};
	};
	class KOD_AmmoBox_338_20Rnd: Box_Base
	{
		scope=2;
		displayName="Коробка патронов .338 Lapua Magnum";
		descriptionShort="Патроны .338 Lapua Magnum в коробке.";
		model="\dz\weapons\ammunition\762_20RoundBox.p3d";
		itemSize[]={3,2};
		rotationFlags=1;
		weight=300;
		hiddenSelections[]=
		{
			"zbytek",
			"camoground"
		};
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\ammo\Ammoboxes\data\ammobox_338_co.paa",
			"KOD_weapons\ammo\Ammoboxes\data\ammobox_338_co.paa"
		};
		hiddenSelectionsMaterials[]=
		{
			"KOD_weapons\ammo\Ammoboxes\data\762_box.rvmat",
			"KOD_weapons\ammo\Ammoboxes\data\762_box.rvmat"
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=100;
					RefTexsMats[]=
					{
						"DZ\weapons\ammunition\data\762_box.rvmat"
					};
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"KOD_weapons\ammo\Ammoboxes\data\762_box.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"KOD_weapons\ammo\Ammoboxes\data\762_box.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"KOD_weapons\ammo\Ammoboxes\data\762_box_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"KOD_weapons\ammo\Ammoboxes\data\762_box_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"KOD_weapons\ammo\Ammoboxes\data\762_box_destruct.rvmat"
							}
						}
					};
				};
			};
		};
		class Resources
		{
			class KOD_Ammo_338
			{
				value=20;
				variable="quantity";
			};
		};
		class AnimEvents
		{
			class SoundWeapon
			{
				class interact
				{
					soundset="ammoboxUnpack_SoundSet";
					id=70;
				};
			};
		};
	};
	class KOD_AmmoBox_408_10Rnd: Box_Base
	{
		scope=2;
		displayName="Коробка патронов .408 CheyTac";
		descriptionShort="Патроны .408 CheyTac в коробке.";
		model="KOD_weapons\ammo\Ammoboxes\ammobox.p3d";
		itemSize[]={2,3};
		rotationFlags=1;
		weight=250;
		hiddenSelections[]=
		{
			"color"
		};
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\ammo\Ammoboxes\data\ammobox_408CT_co.paa"
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
								"KOD_weapons\ammo\Ammoboxes\data\762_box.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"KOD_weapons\ammo\Ammoboxes\data\762_box.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"KOD_weapons\ammo\Ammoboxes\data\762_box_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"KOD_weapons\ammo\Ammoboxes\data\762_box_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"KOD_weapons\ammo\Ammoboxes\data\762_box_destruct.rvmat"
							}
						}
					};
				};
			};
		};
		class Resources
		{
			class KOD_Ammo_408
			{
				value=10;
				variable="quantity";
			};
		};
		class AnimEvents
		{
			class SoundWeapon
			{
				class interact
				{
					soundset="ammoboxUnpack_SoundSet";
					id=70;
				};
			};
		};
	};
	class KOD_AmmoBox_20mm_5Rnd: Inventory_Base
	{
		scope=2;
		displayName="Цинк патронов 20x102 мм";
		descriptionShort="Снаряды 20x102 мм в цинке.";
		model="KOD_weapons\ammo\Ammoboxes\ammobox_big.p3d";
		weight=2250;
		itemSize[]={3,2};
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
								"KOD_weapons\ammo\Ammoboxes\data\ammobox.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"KOD_weapons\ammo\Ammoboxes\data\ammobox.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"KOD_weapons\ammo\Ammoboxes\data\ammobox_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"KOD_weapons\ammo\Ammoboxes\data\ammobox_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"KOD_weapons\ammo\Ammoboxes\data\ammobox_destruct.rvmat"
							}
						}
					};
				};
			};
		};
		soundImpactType="metal";
		class AnimEvents
		{
			class SoundWeapon
			{
				class pickup
				{
					soundSet="sewingkit_pickup_SoundSet";
					id=797;
				};
				class drop
				{
					soundset="ammobox_drop_SoundSet";
					id=898;
				};
			};
		};
	};
};
