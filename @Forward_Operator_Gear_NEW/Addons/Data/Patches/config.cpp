class CfgPatches
{
	class FOG_Data_Patches
	{
		units[]=
		{
			"FOG_Small_Patch_US_IR",
			"FOG_Small_Patch_US_IR_Damaged",
			"FOG_Small_Patch_US_Subdued",
			"FOG_Small_Patch_UK_Subdued",
			"FOG_Big_Patch_US_Colored",
			"FOG_Big_Patch_US_IR",
			"FOG_Big_Patch_US_MC_IR",
			"FOG_Big_Patch_UK_Colored",
			"FOG_Big_Patch_RedMan",
			"FOG_Big_Patch_FOG_cc",
			"FOG_Big_Patch_CrimeCo",
			"FOG_Big_Patch_FOG_BB",
			"FOG_Big_Patch_FOG_BB_EMB",
			"FOG_Big_Patch_JTAC",
			"FOG_Big_Patch_FBI",
			"FOG_Big_Patch_Medic",
			"FOG_Big_Patch_Shaka",
			"FOG_Big_Patch_SOG",
			"FOG_Big_Patch_TACP",
			"FOG_Big_Patch_FOGonia",
			"FOG_Big_Patch_KillCell",
			"FOG_Big_Patch_MeltyBoys",
			"FOG_Big_Patch_Buccees",
			"FOG_Big_Patch_TummyHurt",
			"FOG_Big_Patch_JFK"
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
	class Inventory_Base;
	class FOG_Small_Patch_Base: Inventory_Base
	{
		scope=0;
		displayName="Small Patch";
		descriptionShort="Небольшая нашивка для определённых бронежилетов. | A Small patch that fits certain plate carriers.";
		itemsize[]={2,2};
		allowOwnedCargoManipulation=1;
		Model="\FOG_MOD\Data\Patches\Patch_Small\FOG_Patch_Small.p3d";
		inventorySlot[]=
		{
			"FOG_small_patch"
		};
		weight=600;
		repairableWithKits[]={5,2};
		repairCosts[]={30,25};
		hiddenSelections[]=
		{
			"camo"
		};
		hiddenSelectionsMaterials[]=
		{
			"FOG_MOD\Data\Patches\Patch_Small\data\IR_US.rvmat"
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=450;
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
	class FOG_Small_Patch_US_IR: FOG_Small_Patch_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Data\Patches\Patch_Small\data\IR_US_co.paa"
		};
		hiddenSelectionsMaterials[]=
		{
			"FOG_MOD\Data\Patches\Patch_Small\data\IR_US.rvmat"
		};
	};
	class FOG_Small_Patch_US_IR_Damaged: FOG_Small_Patch_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Data\Patches\Patch_Small\data\IR_US_damaged_co.paa"
		};
		hiddenSelectionsMaterials[]=
		{
			"FOG_MOD\Data\Patches\Patch_Small\data\IR_US.rvmat"
		};
	};
	class FOG_Small_Patch_US_Subdued: FOG_Small_Patch_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Data\Patches\Patch_Small\data\US_subdued_co.paa"
		};
		hiddenSelectionsMaterials[]=
		{
			"FOG_MOD\Data\Patches\Patch_Small\data\US_Sub.rvmat"
		};
	};
	class FOG_Small_Patch_UK_Subdued: FOG_Small_Patch_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Data\Patches\Patch_Small\data\UK_Sub_co.paa"
		};
		hiddenSelectionsMaterials[]=
		{
			"FOG_MOD\Data\Patches\Patch_Small\data\UK_Sub.rvmat"
		};
	};
	class FOG_Big_Patch_Base: Inventory_Base
	{
		scope=0;
		displayName="Large Patch";
		descriptionShort="Большая нашивка для определённых бронежилетов. | A large patch that fits certain plate carriers.";
		itemsize[]={2,2};
		allowOwnedCargoManipulation=1;
		Model="\FOG_MOD\Data\Patches\Patch_Big\FOG_big_patch.p3d";
		inventorySlot[]=
		{
			"FOG_big_patch",
			"FOG_big_patch_only"
		};
		weight=600;
		repairableWithKits[]={5,2};
		repairCosts[]={30,25};
		hiddenSelections[]=
		{
			"camo"
		};
		hiddenSelectionsMaterials[]=
		{
			"FOG_MOD\Data\Patches\Patch_Big\data\Bigpatch_US.rvmat"
		};
		class AnimEvents
		{
			class SoundWeapon
			{
				class pickUpItem
				{
					soundSet="pickUpPot_SoundSet";
					id=797;
				};
				class drop
				{
					soundset="BallisticHelmet_drop_SoundSet";
					id=898;
				};
			};
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=450;
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
	class FOG_Big_Patch_US_Colored: FOG_Big_Patch_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Data\Patches\Patch_Big\data\Bigpatch_US_co.paa"
		};
		hiddenSelectionsMaterials[]=
		{
			"FOG_MOD\Data\Patches\Patch_Big\data\Bigpatch_US.rvmat"
		};
	};
	class FOG_Big_Patch_US_IR: FOG_Big_Patch_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Data\Patches\Patch_Big\data\Bigpatch_USIR_co.paa"
		};
		hiddenSelectionsMaterials[]=
		{
			"FOG_MOD\Data\Patches\Patch_Big\data\Bigpatch_US_IR.rvmat"
		};
	};
	class FOG_Big_Patch_US_MC_IR: FOG_Big_Patch_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Data\Patches\Patch_Big\data\Bigpatch_US_MCIR_co.paa"
		};
		hiddenSelectionsMaterials[]=
		{
			"FOG_MOD\Data\Patches\Patch_Big\data\Bigpatch_US_MC_IR.rvmat"
		};
	};
	class FOG_Big_Patch_UK_Colored: FOG_Big_Patch_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Data\Patches\Patch_Big\data\Bigpatch_UK_co.paa"
		};
		hiddenSelectionsMaterials[]=
		{
			"FOG_MOD\Data\Patches\Patch_Big\data\Bigpatch_UK.rvmat"
		};
	};
	class FOG_Big_Patch_RedMan: FOG_Big_Patch_Base
	{
		scope=2;
		descriptionShort="CAG GOON SQUAD. Голливудское дерьмо. | CAG GOON SQUAD. Hollywood special.";
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Data\Patches\Patch_Big\data\Bigpatch_RM_co.paa"
		};
		hiddenSelectionsMaterials[]=
		{
			"FOG_MOD\Data\Patches\Patch_Big\data\Bigpatch_RM.rvmat"
		};
	};
	class FOG_Big_Patch_FOG_cc: FOG_Big_Patch_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Data\Patches\Patch_Big\data\Bigpatch_fog_cc_co.paa"
		};
		hiddenSelectionsMaterials[]=
		{
			"FOG_MOD\Data\Patches\Patch_Big\data\Bigpatch_fog_cc.rvmat"
		};
	};
	class FOG_Big_Patch_CrimeCo: FOG_Big_Patch_Base
	{
		scope=2;
		descriptionShort="Нашивка Crime Co, переводится как 'Преступная компания'. Нашивка-однодневка от одного из основателей. | Crime Co Patch, translates to 'Crime Company'. Limited run patch from one of the founders.";
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Data\Patches\Patch_Big\data\Bigpatch_crimeco_co.paa"
		};
		hiddenSelectionsMaterials[]=
		{
			"FOG_MOD\Data\Patches\Patch_Big\data\Bigpatch_crimeco.rvmat"
		};
	};
	class FOG_Big_Patch_FOG_BB: FOG_Big_Patch_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Data\Patches\Patch_Big\data\Bigpatch_FOG_BB_co.paa"
		};
		hiddenSelectionsMaterials[]=
		{
			"FOG_MOD\Data\Patches\Patch_Big\data\Bigpatch_FOG_BB.rvmat"
		};
	};
	class FOG_Big_Patch_FOG_BB_EMB: FOG_Big_Patch_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Data\Patches\Patch_Big\data\Bigpatch_FOG_BB_EMB_co.paa"
		};
		hiddenSelectionsMaterials[]=
		{
			"FOG_MOD\Data\Patches\Patch_Big\data\Bigpatch_FOG_BB_EMB.rvmat"
		};
	};
	class FOG_Big_Patch_JTAC: FOG_Big_Patch_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Data\Patches\Patch_Big\data\Bigpatch_JTAC_co.paa"
		};
		hiddenSelectionsMaterials[]=
		{
			"FOG_MOD\Data\Patches\Patch_Big\data\Bigpatch_JTAC.rvmat"
		};
	};
	class FOG_Big_Patch_FBI: FOG_Big_Patch_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Data\Patches\Patch_Big\data\Bigpatch_MCIR_FBI_co.paa"
		};
		hiddenSelectionsMaterials[]=
		{
			"FOG_MOD\Data\Patches\Patch_Big\data\Bigpatch_MCIR_FBI.rvmat"
		};
	};
	class FOG_Big_Patch_Medic: FOG_Big_Patch_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Data\Patches\Patch_Big\data\Bigpatch_Medic_co.paa"
		};
		hiddenSelectionsMaterials[]=
		{
			"FOG_MOD\Data\Patches\Patch_Big\data\Bigpatch_Medic.rvmat"
		};
	};
	class FOG_Big_Patch_Shaka: FOG_Big_Patch_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Data\Patches\Patch_Big\data\Bigpatch_Shaka_skeleton_co.paa"
		};
		hiddenSelectionsMaterials[]=
		{
			"FOG_MOD\Data\Patches\Patch_Big\data\Bigpatch_Shaka_skeleton.rvmat"
		};
	};
	class FOG_Big_Patch_SOG: FOG_Big_Patch_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Data\Patches\Patch_Big\data\Bigpatch_SOG_co.paa"
		};
		hiddenSelectionsMaterials[]=
		{
			"FOG_MOD\Data\Patches\Patch_Big\data\Bigpatch_SOG.rvmat"
		};
	};
	class FOG_Big_Patch_TACP: FOG_Big_Patch_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Data\Patches\Patch_Big\data\Bigpatch_TACP_co.paa"
		};
		hiddenSelectionsMaterials[]=
		{
			"FOG_MOD\Data\Patches\Patch_Big\data\Bigpatch_TACP.rvmat"
		};
	};
	class FOG_Big_Patch_FOGonia: FOG_Big_Patch_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Data\Patches\Patch_Big\data\Bigpatch_FOGonia_co.paa"
		};
		hiddenSelectionsMaterials[]=
		{
			"FOG_MOD\Data\Patches\Patch_Big\data\Bigpatch_FOGonia.rvmat"
		};
	};
	class FOG_Big_Patch_KillCell: FOG_Big_Patch_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Data\Patches\Patch_Big\data\Bigpatch_KillCell_co.paa"
		};
		hiddenSelectionsMaterials[]=
		{
			"FOG_MOD\Data\Patches\Patch_Big\data\Bigpatch_KillCell.rvmat"
		};
	};
	class FOG_Big_Patch_MeltyBoys: FOG_Big_Patch_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Data\Patches\Patch_Big\data\Bigpatch_MeltyBoys_co.paa"
		};
		hiddenSelectionsMaterials[]=
		{
			"FOG_MOD\Data\Patches\Patch_Big\data\Bigpatch_MeltyBoys.rvmat"
		};
	};
	class FOG_Big_Patch_Buccees: FOG_Big_Patch_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Data\Patches\Patch_Big\data\Bigpatch_Buccees_co.paa"
		};
		hiddenSelectionsMaterials[]=
		{
			"FOG_MOD\Data\Patches\Patch_Big\data\Bigpatch_Buccees.rvmat"
		};
	};
	class FOG_Big_Patch_TummyHurt: FOG_Big_Patch_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Data\Patches\Patch_Big\data\Bigpatch_tummyhurts_co.paa"
		};
		hiddenSelectionsMaterials[]=
		{
			"FOG_MOD\Data\Patches\Patch_Big\data\Bigpatch_tummyhurts.rvmat"
		};
	};
	class FOG_Big_Patch_JFK: FOG_Big_Patch_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Data\Patches\Patch_Big\data\Bigpatch_JFK_co.paa"
		};
		hiddenSelectionsMaterials[]=
		{
			"FOG_MOD\Data\Patches\Patch_Big\data\Bigpatch_JFK.rvmat"
		};
	};
};
