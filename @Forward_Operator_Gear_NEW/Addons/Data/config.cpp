class CfgPatches
{
	class FOG_MOD_Data
	{
		units[]=
		{
			"FOG_TShirt_Novelty",
			"FOG_BDU_Pants_Black",
			"FOG_BDU_Pants_RG",
			"FOG_BDU_Pants_Tan",
			"FOG_GhillieBushrag_GrassMulti",
			"FOG_GhillieBushrag_GrassGreen",
			"FOG_GhillieBushrag_Grass",
			"FOG_GhillieBushrag_BushYellow",
			"FOG_GhillieBushrag_Urban",
			"FOG_GhillieBushrag_Snow",
			"FOG_GhillieBushrag_Rags",
			"FOG_GhillieBushrag_RagsMCB",
			"FOG_GhillieBushrag_Pine",
			"FOG_GhillieBushrag_Green",
			"FOG_GhillieBushrag_MCBrowns",
			"FOG_GhillieBushrag_Leaf",
			"FOG_GhillieTop_GrassMulti",
			"FOG_GhillieTop_GrassGreen",
			"FOG_GhillieTop_Grass",
			"FOG_GhillieTop_BushYellow",
			"FOG_GhillieTop_Urban",
			"FOG_GhillieTop_Snow",
			"FOG_GhillieTop_Rags",
			"FOG_GhillieTop_RagsMCB",
			"FOG_GhillieTop_Pine",
			"FOG_GhillieTop_Green",
			"FOG_GhillieTop_MCBrowns",
			"FOG_GhillieTop_Leaf",
			"FOG_GhillieSuit_GrassMulti",
			"FOG_GhillieSuit_GrassGreen",
			"FOG_GhillieSuit_Grass",
			"FOG_GhillieSuit_BushYellow",
			"FOG_GhillieSuit_Urban",
			"FOG_GhillieSuit_Snow",
			"FOG_GhillieSuit_Rags",
			"FOG_GhillieSuit_RagsMCB",
			"FOG_GhillieSuit_Pine",
			"FOG_GhillieSuit_Livonia",
			"FOG_GhillieSuit_MCBrowns",
			"FOG_GhillieSuit_Leaf",
			"FOG_GhillieAtt_GrassMulti",
			"FOG_GhillieAtt_GrassGreen",
			"FOG_GhillieAtt_Grass",
			"FOG_GhillieAtt_BushYellow",
			"FOG_GhillieAtt_Urban",
			"FOG_GhillieAtt_Snow",
			"FOG_GhillieAtt_Rags",
			"FOG_GhillieAtt_RagsMCB",
			"FOG_GhillieAtt_Pine",
			"FOG_GhillieAtt_Green",
			"FOG_GhillieAtt_MCBrowns",
			"FOG_GhillieAtt_Leaf",
			"FOG_GhillieHood_GrassMulti",
			"FOG_GhillieHood_GrassGreen",
			"FOG_GhillieHood_Grass",
			"FOG_GhillieHood_BushYellow",
			"FOG_GhillieHood_Urban",
			"FOG_GhillieHood_Snow",
			"FOG_GhillieHood_Rags",
			"FOG_GhillieHood_RagsMCB",
			"FOG_GhillieHood_Pine",
			"FOG_GhillieHood_Green",
			"FOG_GhillieHood_MCBrowns",
			"FOG_GhillieHood_Leaf"
		};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Data",
			"DZ_Characters",
			"DZ_Characters_Headgear",
			"DZ_Characters_Backpacks",
			"DZ_Characters_Pants",
			"DZ_Characters_Tops",
			"DZ_Scripts"
		};
	};
};
class CfgVehicles
{
	class TShirt_ColorBase;
	class FOG_TShirt_Novelty: TShirt_ColorBase
	{
		scope=2;
		visibilityModifier=0.85000002;
		hiddenSelectionsTextures[]=
		{
			"\DZ\characters\tops\Data\tshirt_ground_black_co.paa",
			"\FOG_MOD\Data\Retextures\Tops\tshirt_novelty_co.paa",
			"\FOG_MOD\Data\Retextures\Tops\tshirt_novelty_co.paa"
		};
	};
	class BDUPants;
	class FOG_BDU_Pants_Black: BDUPants
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Data\Retextures\Pants\BDU_blk_co.paa",
			"FOG_MOD\Data\Retextures\Pants\BDU_blk_co.paa",
			"FOG_MOD\Data\Retextures\Pants\BDU_blk_co.paa"
		};
	};
	class FOG_BDU_Pants_RG: BDUPants
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Data\Retextures\Pants\BDU_rgr_co.paa",
			"FOG_MOD\Data\Retextures\Pants\BDU_rgr_co.paa",
			"FOG_MOD\Data\Retextures\Pants\BDU_rgr_co.paa"
		};
	};
	class FOG_BDU_Pants_Tan: BDUPants
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Data\Retextures\Pants\BDU_Tan_co.paa",
			"FOG_MOD\Data\Retextures\Pants\BDU_Tan_co.paa",
			"FOG_MOD\Data\Retextures\Pants\BDU_Tan_co.paa"
		};
	};
	class GhillieBushrag_ColorBase;
	class FOG_GhillieBushrag_GrassMulti: GhillieBushrag_ColorBase
	{
		scope=2;
		visibilityModifier=0.99000001;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_FOG_grass_multi_co.paa",
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_FOG_grass_multi_co.paa",
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_FOG_grass_multi_co.paa"
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=900;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								""
							}
						},
						
						{
							0.69999999,
							
							{
								""
							}
						},
						
						{
							0.5,
							
							{
								""
							}
						},
						
						{
							0.30000001,
							
							{
								""
							}
						},
						
						{
							0,
							
							{
								""
							}
						}
					};
				};
			};
		};
	};
	class FOG_GhillieBushrag_GrassGreen: GhillieBushrag_ColorBase
	{
		scope=2;
		visibilityModifier=0.99000001;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_FOG_grass_green_co.paa",
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_FOG_grass_green_co.paa",
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_FOG_grass_green_co.paa"
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=900;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								""
							}
						},
						
						{
							0.69999999,
							
							{
								""
							}
						},
						
						{
							0.5,
							
							{
								""
							}
						},
						
						{
							0.30000001,
							
							{
								""
							}
						},
						
						{
							0,
							
							{
								""
							}
						}
					};
				};
			};
		};
	};
	class FOG_GhillieBushrag_Grass: GhillieBushrag_ColorBase
	{
		scope=2;
		visibilityModifier=0.99000001;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_FOG_grass_co.paa",
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_FOG_grass_co.paa",
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_FOG_grass_co.paa"
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=900;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								""
							}
						},
						
						{
							0.69999999,
							
							{
								""
							}
						},
						
						{
							0.5,
							
							{
								""
							}
						},
						
						{
							0.30000001,
							
							{
								""
							}
						},
						
						{
							0,
							
							{
								""
							}
						}
					};
				};
			};
		};
	};
	class FOG_GhillieBushrag_BushYellow: GhillieBushrag_ColorBase
	{
		scope=2;
		visibilityModifier=0.99000001;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_yellow_bush_co.paa",
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_yellow_bush_co.paa",
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_yellow_bush_co.paa"
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=900;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								""
							}
						},
						
						{
							0.69999999,
							
							{
								""
							}
						},
						
						{
							0.5,
							
							{
								""
							}
						},
						
						{
							0.30000001,
							
							{
								""
							}
						},
						
						{
							0,
							
							{
								""
							}
						}
					};
				};
			};
		};
	};
	class FOG_GhillieBushrag_Urban: GhillieBushrag_ColorBase
	{
		scope=2;
		visibilityModifier=0.99000001;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_urban_trash_co.paa",
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_urban_trash_co.paa",
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_urban_trash_co.paa"
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=900;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								""
							}
						},
						
						{
							0.69999999,
							
							{
								""
							}
						},
						
						{
							0.5,
							
							{
								""
							}
						},
						
						{
							0.30000001,
							
							{
								""
							}
						},
						
						{
							0,
							
							{
								""
							}
						}
					};
				};
			};
		};
	};
	class FOG_GhillieBushrag_Snow: GhillieBushrag_ColorBase
	{
		scope=2;
		visibilityModifier=0.99000001;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_snow_foliage_co.paa",
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_snow_foliage_co.paa",
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_snow_foliage_co.paa"
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=900;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								""
							}
						},
						
						{
							0.69999999,
							
							{
								""
							}
						},
						
						{
							0.5,
							
							{
								""
							}
						},
						
						{
							0.30000001,
							
							{
								""
							}
						},
						
						{
							0,
							
							{
								""
							}
						}
					};
				};
			};
		};
	};
	class FOG_GhillieBushrag_Rags: GhillieBushrag_ColorBase
	{
		scope=2;
		visibilityModifier=0.99000001;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_rags_brown_co.paa",
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_rags_brown_co.paa",
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_rags_brown_co.paa"
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=900;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								""
							}
						},
						
						{
							0.69999999,
							
							{
								""
							}
						},
						
						{
							0.5,
							
							{
								""
							}
						},
						
						{
							0.30000001,
							
							{
								""
							}
						},
						
						{
							0,
							
							{
								""
							}
						}
					};
				};
			};
		};
	};
	class FOG_GhillieBushrag_RagsMCB: GhillieBushrag_ColorBase
	{
		scope=2;
		visibilityModifier=0.99000001;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_rags_mcb_co.paa",
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_rags_mcb_co.paa",
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_rags_mcb_co.paa"
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=900;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								""
							}
						},
						
						{
							0.69999999,
							
							{
								""
							}
						},
						
						{
							0.5,
							
							{
								""
							}
						},
						
						{
							0.30000001,
							
							{
								""
							}
						},
						
						{
							0,
							
							{
								""
							}
						}
					};
				};
			};
		};
	};
	class FOG_GhillieBushrag_Pine: GhillieBushrag_ColorBase
	{
		scope=2;
		visibilityModifier=0.99000001;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_pine_co.paa",
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_pine_co.paa",
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_pine_co.paa"
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=900;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								""
							}
						},
						
						{
							0.69999999,
							
							{
								""
							}
						},
						
						{
							0.5,
							
							{
								""
							}
						},
						
						{
							0.30000001,
							
							{
								""
							}
						},
						
						{
							0,
							
							{
								""
							}
						}
					};
				};
			};
		};
	};
	class FOG_GhillieBushrag_Green: GhillieBushrag_ColorBase
	{
		scope=2;
		visibilityModifier=0.99000001;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_new_green_co.paa",
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_new_green_co.paa",
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_new_green_co.paa"
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=900;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								""
							}
						},
						
						{
							0.69999999,
							
							{
								""
							}
						},
						
						{
							0.5,
							
							{
								""
							}
						},
						
						{
							0.30000001,
							
							{
								""
							}
						},
						
						{
							0,
							
							{
								""
							}
						}
					};
				};
			};
		};
	};
	class FOG_GhillieBushrag_MCBrowns: GhillieBushrag_ColorBase
	{
		scope=2;
		visibilityModifier=0.99000001;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_multicam_browns_co.paa",
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_multicam_browns_co.paa",
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_multicam_browns_co.paa"
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=900;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								""
							}
						},
						
						{
							0.69999999,
							
							{
								""
							}
						},
						
						{
							0.5,
							
							{
								""
							}
						},
						
						{
							0.30000001,
							
							{
								""
							}
						},
						
						{
							0,
							
							{
								""
							}
						}
					};
				};
			};
		};
	};
	class FOG_GhillieBushrag_Leaf: GhillieBushrag_ColorBase
	{
		scope=2;
		visibilityModifier=0.99000001;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_leaf_1_co.paa",
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_leaf_1_co.paa",
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_leaf_1_co.paa"
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=900;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								""
							}
						},
						
						{
							0.69999999,
							
							{
								""
							}
						},
						
						{
							0.5,
							
							{
								""
							}
						},
						
						{
							0.30000001,
							
							{
								""
							}
						},
						
						{
							0,
							
							{
								""
							}
						}
					};
				};
			};
		};
	};
	class GhillieTop_ColorBase;
	class FOG_GhillieTop_GrassMulti: GhillieTop_ColorBase
	{
		scope=2;
		visibilityModifier=0.99000001;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_FOG_grass_multi_co.paa",
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_FOG_grass_multi_co.paa",
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_FOG_grass_multi_co.paa"
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=900;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								""
							}
						},
						
						{
							0.69999999,
							
							{
								""
							}
						},
						
						{
							0.5,
							
							{
								""
							}
						},
						
						{
							0.30000001,
							
							{
								""
							}
						},
						
						{
							0,
							
							{
								""
							}
						}
					};
				};
			};
		};
	};
	class FOG_GhillieTop_GrassGreen: GhillieTop_ColorBase
	{
		scope=2;
		visibilityModifier=0.99000001;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_FOG_grass_green_co.paa",
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_FOG_grass_green_co.paa",
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_FOG_grass_green_co.paa"
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=900;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								""
							}
						},
						
						{
							0.69999999,
							
							{
								""
							}
						},
						
						{
							0.5,
							
							{
								""
							}
						},
						
						{
							0.30000001,
							
							{
								""
							}
						},
						
						{
							0,
							
							{
								""
							}
						}
					};
				};
			};
		};
	};
	class FOG_GhillieTop_Grass: GhillieTop_ColorBase
	{
		scope=2;
		visibilityModifier=0.99000001;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_FOG_grass_co.paa",
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_FOG_grass_co.paa",
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_FOG_grass_co.paa"
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=900;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								""
							}
						},
						
						{
							0.69999999,
							
							{
								""
							}
						},
						
						{
							0.5,
							
							{
								""
							}
						},
						
						{
							0.30000001,
							
							{
								""
							}
						},
						
						{
							0,
							
							{
								""
							}
						}
					};
				};
			};
		};
	};
	class FOG_GhillieTop_BushYellow: GhillieTop_ColorBase
	{
		scope=2;
		visibilityModifier=0.99000001;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_yellow_bush_co.paa",
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_yellow_bush_co.paa",
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_yellow_bush_co.paa"
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=900;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								""
							}
						},
						
						{
							0.69999999,
							
							{
								""
							}
						},
						
						{
							0.5,
							
							{
								""
							}
						},
						
						{
							0.30000001,
							
							{
								""
							}
						},
						
						{
							0,
							
							{
								""
							}
						}
					};
				};
			};
		};
	};
	class FOG_GhillieTop_Urban: GhillieTop_ColorBase
	{
		scope=2;
		visibilityModifier=0.99000001;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_urban_trash_co.paa",
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_urban_trash_co.paa",
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_urban_trash_co.paa"
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=900;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								""
							}
						},
						
						{
							0.69999999,
							
							{
								""
							}
						},
						
						{
							0.5,
							
							{
								""
							}
						},
						
						{
							0.30000001,
							
							{
								""
							}
						},
						
						{
							0,
							
							{
								""
							}
						}
					};
				};
			};
		};
	};
	class FOG_GhillieTop_Snow: GhillieTop_ColorBase
	{
		scope=2;
		visibilityModifier=0.99000001;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_snow_foliage_co.paa",
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_snow_foliage_co.paa",
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_snow_foliage_co.paa"
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=900;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								""
							}
						},
						
						{
							0.69999999,
							
							{
								""
							}
						},
						
						{
							0.5,
							
							{
								""
							}
						},
						
						{
							0.30000001,
							
							{
								""
							}
						},
						
						{
							0,
							
							{
								""
							}
						}
					};
				};
			};
		};
	};
	class FOG_GhillieTop_Rags: GhillieTop_ColorBase
	{
		scope=2;
		visibilityModifier=0.99000001;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_rags_brown_co.paa",
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_rags_brown_co.paa",
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_rags_brown_co.paa"
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=900;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								""
							}
						},
						
						{
							0.69999999,
							
							{
								""
							}
						},
						
						{
							0.5,
							
							{
								""
							}
						},
						
						{
							0.30000001,
							
							{
								""
							}
						},
						
						{
							0,
							
							{
								""
							}
						}
					};
				};
			};
		};
	};
	class FOG_GhillieTop_RagsMCB: GhillieTop_ColorBase
	{
		scope=2;
		visibilityModifier=0.99000001;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_rags_mcb_co.paa",
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_rags_mcb_co.paa",
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_rags_mcb_co.paa"
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=900;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								""
							}
						},
						
						{
							0.69999999,
							
							{
								""
							}
						},
						
						{
							0.5,
							
							{
								""
							}
						},
						
						{
							0.30000001,
							
							{
								""
							}
						},
						
						{
							0,
							
							{
								""
							}
						}
					};
				};
			};
		};
	};
	class FOG_GhillieTop_Pine: GhillieTop_ColorBase
	{
		scope=2;
		visibilityModifier=0.99000001;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_pine_co.paa",
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_pine_co.paa",
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_pine_co.paa"
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=900;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								""
							}
						},
						
						{
							0.69999999,
							
							{
								""
							}
						},
						
						{
							0.5,
							
							{
								""
							}
						},
						
						{
							0.30000001,
							
							{
								""
							}
						},
						
						{
							0,
							
							{
								""
							}
						}
					};
				};
			};
		};
	};
	class FOG_GhillieTop_Green: GhillieTop_ColorBase
	{
		scope=2;
		visibilityModifier=0.99000001;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_new_green_co.paa",
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_new_green_co.paa",
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_new_green_co.paa"
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=900;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								""
							}
						},
						
						{
							0.69999999,
							
							{
								""
							}
						},
						
						{
							0.5,
							
							{
								""
							}
						},
						
						{
							0.30000001,
							
							{
								""
							}
						},
						
						{
							0,
							
							{
								""
							}
						}
					};
				};
			};
		};
	};
	class FOG_GhillieTop_MCBrowns: GhillieTop_ColorBase
	{
		scope=2;
		visibilityModifier=0.99000001;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_multicam_browns_co.paa",
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_multicam_browns_co.paa",
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_multicam_browns_co.paa"
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=900;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								""
							}
						},
						
						{
							0.69999999,
							
							{
								""
							}
						},
						
						{
							0.5,
							
							{
								""
							}
						},
						
						{
							0.30000001,
							
							{
								""
							}
						},
						
						{
							0,
							
							{
								""
							}
						}
					};
				};
			};
		};
	};
	class FOG_GhillieTop_Leaf: GhillieTop_ColorBase
	{
		scope=2;
		visibilityModifier=0.99000001;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_leaf_1_co.paa",
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_leaf_1_co.paa",
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_leaf_1_co.paa"
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=900;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								""
							}
						},
						
						{
							0.69999999,
							
							{
								""
							}
						},
						
						{
							0.5,
							
							{
								""
							}
						},
						
						{
							0.30000001,
							
							{
								""
							}
						},
						
						{
							0,
							
							{
								""
							}
						}
					};
				};
			};
		};
	};
	class GhillieSuit_ColorBase;
	class FOG_GhillieSuit_GrassMulti: GhillieSuit_ColorBase
	{
		scope=2;
		visibilityModifier=0.99000001;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_FOG_grass_multi_co.paa",
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_FOG_grass_multi_co.paa",
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_FOG_grass_multi_co.paa"
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=900;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								""
							}
						},
						
						{
							0.69999999,
							
							{
								""
							}
						},
						
						{
							0.5,
							
							{
								""
							}
						},
						
						{
							0.30000001,
							
							{
								""
							}
						},
						
						{
							0,
							
							{
								""
							}
						}
					};
				};
			};
		};
	};
	class FOG_GhillieSuit_GrassGreen: GhillieSuit_ColorBase
	{
		scope=2;
		visibilityModifier=0.99000001;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_FOG_grass_green_co.paa",
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_FOG_grass_green_co.paa",
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_FOG_grass_green_co.paa"
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=900;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								""
							}
						},
						
						{
							0.69999999,
							
							{
								""
							}
						},
						
						{
							0.5,
							
							{
								""
							}
						},
						
						{
							0.30000001,
							
							{
								""
							}
						},
						
						{
							0,
							
							{
								""
							}
						}
					};
				};
			};
		};
	};
	class FOG_GhillieSuit_Grass: GhillieSuit_ColorBase
	{
		scope=2;
		visibilityModifier=0.99000001;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_FOG_grass_co.paa",
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_FOG_grass_co.paa",
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_FOG_grass_co.paa"
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=900;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								""
							}
						},
						
						{
							0.69999999,
							
							{
								""
							}
						},
						
						{
							0.5,
							
							{
								""
							}
						},
						
						{
							0.30000001,
							
							{
								""
							}
						},
						
						{
							0,
							
							{
								""
							}
						}
					};
				};
			};
		};
	};
	class FOG_GhillieSuit_BushYellow: GhillieSuit_ColorBase
	{
		scope=2;
		visibilityModifier=0.99000001;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_yellow_bush_co.paa",
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_yellow_bush_co.paa",
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_yellow_bush_co.paa"
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=900;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								""
							}
						},
						
						{
							0.69999999,
							
							{
								""
							}
						},
						
						{
							0.5,
							
							{
								""
							}
						},
						
						{
							0.30000001,
							
							{
								""
							}
						},
						
						{
							0,
							
							{
								""
							}
						}
					};
				};
			};
		};
	};
	class FOG_GhillieSuit_Urban: GhillieSuit_ColorBase
	{
		scope=2;
		visibilityModifier=0.99000001;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_urban_trash_co.paa",
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_urban_trash_co.paa",
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_urban_trash_co.paa"
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=900;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								""
							}
						},
						
						{
							0.69999999,
							
							{
								""
							}
						},
						
						{
							0.5,
							
							{
								""
							}
						},
						
						{
							0.30000001,
							
							{
								""
							}
						},
						
						{
							0,
							
							{
								""
							}
						}
					};
				};
			};
		};
	};
	class FOG_GhillieSuit_Snow: GhillieSuit_ColorBase
	{
		scope=2;
		visibilityModifier=0.99000001;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_snow_foliage_co.paa",
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_snow_foliage_co.paa",
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_snow_foliage_co.paa"
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=900;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								""
							}
						},
						
						{
							0.69999999,
							
							{
								""
							}
						},
						
						{
							0.5,
							
							{
								""
							}
						},
						
						{
							0.30000001,
							
							{
								""
							}
						},
						
						{
							0,
							
							{
								""
							}
						}
					};
				};
			};
		};
	};
	class FOG_GhillieSuit_Rags: GhillieSuit_ColorBase
	{
		scope=2;
		visibilityModifier=0.99000001;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_rags_brown_co.paa",
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_rags_brown_co.paa",
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_rags_brown_co.paa"
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=900;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								""
							}
						},
						
						{
							0.69999999,
							
							{
								""
							}
						},
						
						{
							0.5,
							
							{
								""
							}
						},
						
						{
							0.30000001,
							
							{
								""
							}
						},
						
						{
							0,
							
							{
								""
							}
						}
					};
				};
			};
		};
	};
	class FOG_GhillieSuit_RagsMCB: GhillieSuit_ColorBase
	{
		scope=2;
		visibilityModifier=0.99000001;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_rags_mcb_co.paa",
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_rags_mcb_co.paa",
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_rags_mcb_co.paa"
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=900;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								""
							}
						},
						
						{
							0.69999999,
							
							{
								""
							}
						},
						
						{
							0.5,
							
							{
								""
							}
						},
						
						{
							0.30000001,
							
							{
								""
							}
						},
						
						{
							0,
							
							{
								""
							}
						}
					};
				};
			};
		};
	};
	class FOG_GhillieSuit_Pine: GhillieSuit_ColorBase
	{
		scope=2;
		visibilityModifier=0.99000001;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_pine_co.paa",
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_pine_co.paa",
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_pine_co.paa"
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=900;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								""
							}
						},
						
						{
							0.69999999,
							
							{
								""
							}
						},
						
						{
							0.5,
							
							{
								""
							}
						},
						
						{
							0.30000001,
							
							{
								""
							}
						},
						
						{
							0,
							
							{
								""
							}
						}
					};
				};
			};
		};
	};
	class FOG_GhillieSuit_Livonia: GhillieSuit_ColorBase
	{
		scope=2;
		visibilityModifier=0.99000001;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_new_green_co.paa",
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_new_green_co.paa",
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_new_green_co.paa"
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=900;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								""
							}
						},
						
						{
							0.69999999,
							
							{
								""
							}
						},
						
						{
							0.5,
							
							{
								""
							}
						},
						
						{
							0.30000001,
							
							{
								""
							}
						},
						
						{
							0,
							
							{
								""
							}
						}
					};
				};
			};
		};
	};
	class FOG_GhillieSuit_MCBrowns: GhillieSuit_ColorBase
	{
		scope=2;
		visibilityModifier=0.99000001;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_multicam_browns_co.paa",
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_multicam_browns_co.paa",
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_multicam_browns_co.paa"
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=900;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								""
							}
						},
						
						{
							0.69999999,
							
							{
								""
							}
						},
						
						{
							0.5,
							
							{
								""
							}
						},
						
						{
							0.30000001,
							
							{
								""
							}
						},
						
						{
							0,
							
							{
								""
							}
						}
					};
				};
			};
		};
	};
	class FOG_GhillieSuit_Leaf: GhillieSuit_ColorBase
	{
		scope=2;
		visibilityModifier=0.99000001;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_leaf_1_co.paa",
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_leaf_1_co.paa",
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_leaf_1_co.paa"
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=900;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								""
							}
						},
						
						{
							0.69999999,
							
							{
								""
							}
						},
						
						{
							0.5,
							
							{
								""
							}
						},
						
						{
							0.30000001,
							
							{
								""
							}
						},
						
						{
							0,
							
							{
								""
							}
						}
					};
				};
			};
		};
	};
	class GhillieAtt_ColorBase;
	class FOG_GhillieAtt_GrassMulti: GhillieAtt_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_FOG_grass_multi_co.paa"
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=900;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								""
							}
						},
						
						{
							0.69999999,
							
							{
								""
							}
						},
						
						{
							0.5,
							
							{
								""
							}
						},
						
						{
							0.30000001,
							
							{
								""
							}
						},
						
						{
							0,
							
							{
								""
							}
						}
					};
				};
			};
		};
	};
	class FOG_GhillieAtt_GrassGreen: GhillieAtt_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_FOG_grass_green_co.paa"
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=900;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								""
							}
						},
						
						{
							0.69999999,
							
							{
								""
							}
						},
						
						{
							0.5,
							
							{
								""
							}
						},
						
						{
							0.30000001,
							
							{
								""
							}
						},
						
						{
							0,
							
							{
								""
							}
						}
					};
				};
			};
		};
	};
	class FOG_GhillieAtt_Grass: GhillieAtt_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_FOG_grass_co.paa"
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=900;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								""
							}
						},
						
						{
							0.69999999,
							
							{
								""
							}
						},
						
						{
							0.5,
							
							{
								""
							}
						},
						
						{
							0.30000001,
							
							{
								""
							}
						},
						
						{
							0,
							
							{
								""
							}
						}
					};
				};
			};
		};
	};
	class FOG_GhillieAtt_BushYellow: GhillieAtt_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_yellow_bush_co.paa"
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=900;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								""
							}
						},
						
						{
							0.69999999,
							
							{
								""
							}
						},
						
						{
							0.5,
							
							{
								""
							}
						},
						
						{
							0.30000001,
							
							{
								""
							}
						},
						
						{
							0,
							
							{
								""
							}
						}
					};
				};
			};
		};
	};
	class FOG_GhillieAtt_Urban: GhillieAtt_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_urban_trash_co.paa"
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=900;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								""
							}
						},
						
						{
							0.69999999,
							
							{
								""
							}
						},
						
						{
							0.5,
							
							{
								""
							}
						},
						
						{
							0.30000001,
							
							{
								""
							}
						},
						
						{
							0,
							
							{
								""
							}
						}
					};
				};
			};
		};
	};
	class FOG_GhillieAtt_Snow: GhillieAtt_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_snow_foliage_co.paa"
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=900;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								""
							}
						},
						
						{
							0.69999999,
							
							{
								""
							}
						},
						
						{
							0.5,
							
							{
								""
							}
						},
						
						{
							0.30000001,
							
							{
								""
							}
						},
						
						{
							0,
							
							{
								""
							}
						}
					};
				};
			};
		};
	};
	class FOG_GhillieAtt_Rags: GhillieAtt_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_rags_brown_co.paa"
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=900;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								""
							}
						},
						
						{
							0.69999999,
							
							{
								""
							}
						},
						
						{
							0.5,
							
							{
								""
							}
						},
						
						{
							0.30000001,
							
							{
								""
							}
						},
						
						{
							0,
							
							{
								""
							}
						}
					};
				};
			};
		};
	};
	class FOG_GhillieAtt_RagsMCB: GhillieAtt_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_rags_mcb_co.paa"
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=900;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								""
							}
						},
						
						{
							0.69999999,
							
							{
								""
							}
						},
						
						{
							0.5,
							
							{
								""
							}
						},
						
						{
							0.30000001,
							
							{
								""
							}
						},
						
						{
							0,
							
							{
								""
							}
						}
					};
				};
			};
		};
	};
	class FOG_GhillieAtt_Pine: GhillieAtt_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_pine_co.paa"
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=900;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								""
							}
						},
						
						{
							0.69999999,
							
							{
								""
							}
						},
						
						{
							0.5,
							
							{
								""
							}
						},
						
						{
							0.30000001,
							
							{
								""
							}
						},
						
						{
							0,
							
							{
								""
							}
						}
					};
				};
			};
		};
	};
	class FOG_GhillieAtt_Green: GhillieAtt_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_new_green_co.paa"
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=900;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								""
							}
						},
						
						{
							0.69999999,
							
							{
								""
							}
						},
						
						{
							0.5,
							
							{
								""
							}
						},
						
						{
							0.30000001,
							
							{
								""
							}
						},
						
						{
							0,
							
							{
								""
							}
						}
					};
				};
			};
		};
	};
	class FOG_GhillieAtt_MCBrowns: GhillieAtt_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_multicam_browns_co.paa"
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=900;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								""
							}
						},
						
						{
							0.69999999,
							
							{
								""
							}
						},
						
						{
							0.5,
							
							{
								""
							}
						},
						
						{
							0.30000001,
							
							{
								""
							}
						},
						
						{
							0,
							
							{
								""
							}
						}
					};
				};
			};
		};
	};
	class FOG_GhillieAtt_Leaf: GhillieAtt_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_leaf_1_co.paa"
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=900;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								""
							}
						},
						
						{
							0.69999999,
							
							{
								""
							}
						},
						
						{
							0.5,
							
							{
								""
							}
						},
						
						{
							0.30000001,
							
							{
								""
							}
						},
						
						{
							0,
							
							{
								""
							}
						}
					};
				};
			};
		};
	};
	class GhillieHood_ColorBase;
	class FOG_GhillieHood_GrassMulti: GhillieHood_ColorBase
	{
		scope=2;
		visibilityModifier=0.99000001;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_FOG_grass_multi_co.paa",
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_FOG_grass_multi_co.paa",
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_FOG_grass_multi_co.paa"
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=900;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								""
							}
						},
						
						{
							0.69999999,
							
							{
								""
							}
						},
						
						{
							0.5,
							
							{
								""
							}
						},
						
						{
							0.30000001,
							
							{
								""
							}
						},
						
						{
							0,
							
							{
								""
							}
						}
					};
				};
			};
		};
	};
	class FOG_GhillieHood_GrassGreen: GhillieHood_ColorBase
	{
		scope=2;
		visibilityModifier=0.99000001;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_FOG_grass_green_co.paa",
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_FOG_grass_green_co.paa",
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_FOG_grass_green_co.paa"
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=900;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								""
							}
						},
						
						{
							0.69999999,
							
							{
								""
							}
						},
						
						{
							0.5,
							
							{
								""
							}
						},
						
						{
							0.30000001,
							
							{
								""
							}
						},
						
						{
							0,
							
							{
								""
							}
						}
					};
				};
			};
		};
	};
	class FOG_GhillieHood_Grass: GhillieHood_ColorBase
	{
		scope=2;
		visibilityModifier=0.99000001;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_FOG_grass_co.paa",
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_FOG_grass_co.paa",
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_FOG_grass_co.paa"
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=900;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								""
							}
						},
						
						{
							0.69999999,
							
							{
								""
							}
						},
						
						{
							0.5,
							
							{
								""
							}
						},
						
						{
							0.30000001,
							
							{
								""
							}
						},
						
						{
							0,
							
							{
								""
							}
						}
					};
				};
			};
		};
	};
	class FOG_GhillieHood_BushYellow: GhillieHood_ColorBase
	{
		scope=2;
		visibilityModifier=0.99000001;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_yellow_bush_co.paa",
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_yellow_bush_co.paa",
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_yellow_bush_co.paa"
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=900;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								""
							}
						},
						
						{
							0.69999999,
							
							{
								""
							}
						},
						
						{
							0.5,
							
							{
								""
							}
						},
						
						{
							0.30000001,
							
							{
								""
							}
						},
						
						{
							0,
							
							{
								""
							}
						}
					};
				};
			};
		};
	};
	class FOG_GhillieHood_Urban: GhillieHood_ColorBase
	{
		scope=2;
		visibilityModifier=0.99000001;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_urban_trash_co.paa",
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_urban_trash_co.paa",
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_urban_trash_co.paa"
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=900;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								""
							}
						},
						
						{
							0.69999999,
							
							{
								""
							}
						},
						
						{
							0.5,
							
							{
								""
							}
						},
						
						{
							0.30000001,
							
							{
								""
							}
						},
						
						{
							0,
							
							{
								""
							}
						}
					};
				};
			};
		};
	};
	class FOG_GhillieHood_Snow: GhillieHood_ColorBase
	{
		scope=2;
		visibilityModifier=0.99000001;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_snow_foliage_co.paa",
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_snow_foliage_co.paa",
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_snow_foliage_co.paa"
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=900;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								""
							}
						},
						
						{
							0.69999999,
							
							{
								""
							}
						},
						
						{
							0.5,
							
							{
								""
							}
						},
						
						{
							0.30000001,
							
							{
								""
							}
						},
						
						{
							0,
							
							{
								""
							}
						}
					};
				};
			};
		};
	};
	class FOG_GhillieHood_Rags: GhillieHood_ColorBase
	{
		scope=2;
		visibilityModifier=0.99000001;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_rags_brown_co.paa",
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_rags_brown_co.paa",
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_rags_brown_co.paa"
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=900;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								""
							}
						},
						
						{
							0.69999999,
							
							{
								""
							}
						},
						
						{
							0.5,
							
							{
								""
							}
						},
						
						{
							0.30000001,
							
							{
								""
							}
						},
						
						{
							0,
							
							{
								""
							}
						}
					};
				};
			};
		};
	};
	class FOG_GhillieHood_RagsMCB: GhillieHood_ColorBase
	{
		scope=2;
		visibilityModifier=0.99000001;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_rags_mcb_co.paa",
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_rags_mcb_co.paa",
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_rags_mcb_co.paa"
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=900;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								""
							}
						},
						
						{
							0.69999999,
							
							{
								""
							}
						},
						
						{
							0.5,
							
							{
								""
							}
						},
						
						{
							0.30000001,
							
							{
								""
							}
						},
						
						{
							0,
							
							{
								""
							}
						}
					};
				};
			};
		};
	};
	class FOG_GhillieHood_Pine: GhillieHood_ColorBase
	{
		scope=2;
		visibilityModifier=0.99000001;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_pine_co.paa",
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_pine_co.paa",
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_pine_co.paa"
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=900;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								""
							}
						},
						
						{
							0.69999999,
							
							{
								""
							}
						},
						
						{
							0.5,
							
							{
								""
							}
						},
						
						{
							0.30000001,
							
							{
								""
							}
						},
						
						{
							0,
							
							{
								""
							}
						}
					};
				};
			};
		};
	};
	class FOG_GhillieHood_Green: GhillieHood_ColorBase
	{
		scope=2;
		visibilityModifier=0.99000001;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_new_green_co.paa",
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_new_green_co.paa",
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_new_green_co.paa"
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=900;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								""
							}
						},
						
						{
							0.69999999,
							
							{
								""
							}
						},
						
						{
							0.5,
							
							{
								""
							}
						},
						
						{
							0.30000001,
							
							{
								""
							}
						},
						
						{
							0,
							
							{
								""
							}
						}
					};
				};
			};
		};
	};
	class FOG_GhillieHood_MCBrowns: GhillieHood_ColorBase
	{
		scope=2;
		visibilityModifier=0.99000001;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_multicam_browns_co.paa",
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_multicam_browns_co.paa",
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_multicam_browns_co.paa"
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=900;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								""
							}
						},
						
						{
							0.69999999,
							
							{
								""
							}
						},
						
						{
							0.5,
							
							{
								""
							}
						},
						
						{
							0.30000001,
							
							{
								""
							}
						},
						
						{
							0,
							
							{
								""
							}
						}
					};
				};
			};
		};
	};
	class FOG_GhillieHood_Leaf: GhillieHood_ColorBase
	{
		scope=2;
		visibilityModifier=0.99000001;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_leaf_1_co.paa",
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_leaf_1_co.paa",
			"FOG_MOD\Data\Retextures\Ghillies\Ghillie_leaf_1_co.paa"
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=900;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								""
							}
						},
						
						{
							0.69999999,
							
							{
								""
							}
						},
						
						{
							0.5,
							
							{
								""
							}
						},
						
						{
							0.30000001,
							
							{
								""
							}
						},
						
						{
							0,
							
							{
								""
							}
						}
					};
				};
			};
		};
	};
};
