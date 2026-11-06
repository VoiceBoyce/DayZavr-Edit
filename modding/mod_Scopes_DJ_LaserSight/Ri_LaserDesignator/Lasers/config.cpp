class CfgPatches
{
	class Ri_Lasers
	{
		units[]={};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Data",
			"Ri_LaserDesignator"
		};
		worlds[]={};
	};
};
class cfgVehicles
{
	class Switchable_Base;
	class Inventory_Base;
	class Ri_LaserDesignator_Base;
	class Ri_Laser_Anpeq2: Ri_LaserDesignator_Base
	{
		scope=2;
		displayName="#Anpeq2";
		descriptionShort="#Anpeq2desc";
		model="Ri_LaserDesignator\Lasers\model\Anpeq2.p3d";
		hiddenSelectionsTextures[]=
		{
			"Ri_LaserDesignator\Lasers\data\Anpeq2_co.paa",
			"#(argb,8,8,3)color(0,0,0,0,co)",
			"dz\gear\tools\data\flashlight_glass.paa"
		};
		hiddenSelectionsMaterials[]=
		{
			"Ri_LaserDesignator\Lasers\data\Anpeq2.rvmat",
			"",
			"dz\gear\tools\data\flashlight_glass.rvmat"
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
								"Ri_LaserDesignator\Lasers\data\Anpeq2.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"Ri_LaserDesignator\Lasers\data\Anpeq2.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"Ri_LaserDesignator\Lasers\data\Anpeq2_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"Ri_LaserDesignator\Lasers\data\Anpeq2_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"Ri_LaserDesignator\Lasers\data\Anpeq2_destruct.rvmat"
							}
						}
					};
				};
			};
		};
	};
	class Ri_Laser_Surefire_x400: Ri_LaserDesignator_Base
	{
		scope=2;
		displayName="#Surefire_x400";
		descriptionShort="#Surefire_x400desc";
		model="Ri_LaserDesignator\Lasers\model\Surefire_x400.p3d";
		hiddenSelectionsTextures[]=
		{
			"Ri_LaserDesignator\Lasers\data\Surefire_x400_co.paa",
			"#(argb,8,8,3)color(0,0,0,0,co)",
			"dz\gear\tools\data\flashlight_glass.paa"
		};
		hiddenSelectionsMaterials[]=
		{
			"Ri_LaserDesignator\Lasers\data\Surefire_x400.rvmat",
			"",
			"dz\gear\tools\data\flashlight_glass.rvmat"
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
								"Ri_LaserDesignator\Lasers\data\Surefire_x400.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"Ri_LaserDesignator\Lasers\data\Surefire_x400.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"Ri_LaserDesignator\Lasers\data\Surefire_x400_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"Ri_LaserDesignator\Lasers\data\Surefire_x400_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"Ri_LaserDesignator\Lasers\data\Surefire_x400_destruct.rvmat"
							}
						}
					};
				};
			};
		};
	};
	class Ri_Laser_GL21: Ri_LaserDesignator_Base
	{
		scope=2;
		displayName="#GL21";
		descriptionShort="#GL21desc";
		inventorySlot[]=
		{
			"pistolFlashlight"
		};
		model="Ri_LaserDesignator\Lasers\model\gl21.p3d";
		hiddenSelectionsTextures[]=
		{
			"Ri_LaserDesignator\Lasers\data\gl21_co.paa",
			"#(argb,8,8,3)color(0,0,0,0,co)",
			"dz\gear\tools\data\flashlight_glass.paa"
		};
		hiddenSelectionsMaterials[]=
		{
			"Ri_LaserDesignator\Lasers\data\gl21.rvmat",
			"",
			"dz\gear\tools\data\flashlight_glass.rvmat"
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
								"Ri_LaserDesignator\Lasers\data\gl21.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"Ri_LaserDesignator\Lasers\data\gl21.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"Ri_LaserDesignator\Lasers\data\gl21_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"Ri_LaserDesignator\Lasers\data\gl21_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"Ri_LaserDesignator\Lasers\data\gl21_destruct.rvmat"
							}
						}
					};
				};
			};
		};
	};
	class Ri_Laser_Perst3: Ri_LaserDesignator_Base
	{
		scope=2;
		displayName="#Perst3";
		descriptionShort="#Perst3desc";
		model="Ri_LaserDesignator\Lasers\model\Perst3.p3d";
		hiddenSelections[]=
		{
			"zbytek",
			"Beam1",
			"Beam2",
			"glass"
		};
		simpleHiddenSelections[]=
		{
			"Beam1",
			"Beam2"
		};
		hiddenSelectionsTextures[]=
		{
			"Ri_LaserDesignator\Lasers\data\Perst3_co.paa",
			"#(argb,8,8,3)color(0,0,0,0,co)",
			"#(argb,8,8,3)color(0,0,0,0,co)",
			"dz\gear\tools\data\flashlight_glass.paa"
		};
		hiddenSelectionsMaterials[]=
		{
			"Ri_LaserDesignator\Lasers\data\Perst3.rvmat",
			"",
			"",
			"dz\gear\tools\data\flashlight_glass.rvmat"
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
								"Ri_LaserDesignator\Lasers\data\Perst3.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"Ri_LaserDesignator\Lasers\data\Perst3.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"Ri_LaserDesignator\Lasers\data\Perst3_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"Ri_LaserDesignator\Lasers\data\Perst3_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"Ri_LaserDesignator\Lasers\data\Perst3_destruct.rvmat"
							}
						}
					};
				};
			};
		};
	};
	class Ri_Laser_Ncstar: Ri_LaserDesignator_Base
	{
		scope=2;
		displayName="#Ncstar";
		descriptionShort="#Ncstardesc";
		model="Ri_LaserDesignator\Lasers\model\Ncstar.p3d";
		hiddenSelections[]=
		{
			"zbytek",
			"Beam1"
		};
		simpleHiddenSelections[]=
		{
			"Beam1"
		};
		hiddenSelectionsTextures[]=
		{
			"Ri_LaserDesignator\Lasers\data\Ncstar_co.paa",
			"#(argb,8,8,3)color(0,0,0,0,co)"
		};
		hiddenSelectionsMaterials[]=
		{
			"Ri_LaserDesignator\Lasers\data\Ncstar.rvmat",
			""
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
								"Ri_LaserDesignator\Lasers\data\Ncstar.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"Ri_LaserDesignator\Lasers\data\Ncstar.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"Ri_LaserDesignator\Lasers\data\Ncstar_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"Ri_LaserDesignator\Lasers\data\Ncstar_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"Ri_LaserDesignator\Lasers\data\Ncstar_destruct.rvmat"
							}
						}
					};
				};
			};
		};
	};
	class Ri_Laser_dbEbal: Ri_LaserDesignator_Base
	{
		scope=2;
		displayName="#dbEbal";
		descriptionShort="#dbEbaldesc";
		model="Ri_LaserDesignator\Lasers\model\dbEbal.p3d";
		hiddenSelections[]=
		{
			"zbytek",
			"Beam1",
			"Beam2",
			"glass"
		};
		simpleHiddenSelections[]=
		{
			"Beam1",
			"Beam2"
		};
		hiddenSelectionsTextures[]=
		{
			"Ri_LaserDesignator\Lasers\data\dbEbal_co.paa",
			"#(argb,8,8,3)color(0,0,0,0,co)",
			"#(argb,8,8,3)color(0,0,0,0,co)",
			"dz\gear\tools\data\flashlight_glass.paa"
		};
		hiddenSelectionsMaterials[]=
		{
			"Ri_LaserDesignator\Lasers\data\dbEbal.rvmat",
			"",
			"",
			"dz\gear\tools\data\flashlight_glass.rvmat"
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
								"Ri_LaserDesignator\Lasers\data\dbEbal.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"Ri_LaserDesignator\Lasers\data\dbEbal.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"Ri_LaserDesignator\Lasers\data\dbEbal_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"Ri_LaserDesignator\Lasers\data\dbEbal_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"Ri_LaserDesignator\Lasers\data\dbEbal_destruct.rvmat"
							}
						}
					};
				};
			};
		};
	};
	class Ri_Laser_Anpeq15: Ri_LaserDesignator_Base
	{
		scope=2;
		displayName="#Anpeq15";
		descriptionShort="#Anpeq15desc";
		model="Ri_LaserDesignator\Lasers\model\Anpeq15.p3d";
		hiddenSelections[]=
		{
			"zbytek",
			"Beam1",
			"Beam2",
			"glass"
		};
		simpleHiddenSelections[]=
		{
			"Beam1",
			"Beam2"
		};
		hiddenSelectionsTextures[]=
		{
			"Ri_LaserDesignator\Lasers\data\Anpeq15_co.paa",
			"#(argb,8,8,3)color(0,0,0,0,co)",
			"#(argb,8,8,3)color(0,0,0,0,co)",
			"dz\gear\tools\data\flashlight_glass.paa"
		};
		hiddenSelectionsMaterials[]=
		{
			"Ri_LaserDesignator\Lasers\data\Anpeq15.rvmat",
			"",
			"",
			"dz\gear\tools\data\flashlight_glass.rvmat"
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
								"Ri_LaserDesignator\Lasers\data\Anpeq15.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"Ri_LaserDesignator\Lasers\data\Anpeq15.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"Ri_LaserDesignator\Lasers\data\Anpeq15_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"Ri_LaserDesignator\Lasers\data\Anpeq15_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"Ri_LaserDesignator\Lasers\data\Anpeq15_destruct.rvmat"
							}
						}
					};
				};
			};
		};
	};
};
