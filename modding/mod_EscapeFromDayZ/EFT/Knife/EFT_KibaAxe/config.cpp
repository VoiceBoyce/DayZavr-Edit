class CfgPatches
{
	class CfgPatches_KibaAxe_Knife
	{
		units[]={};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]=
		{
            "DZ_Data",
            "DZ_Weapons_Projectiles",
            "DZ_Weapons_Firearms"
		};
	};
};
class CfgVehicles
{	
	class Inventory_Base;
	class CombatKnife;

class EFT_KibaAxe_ColorBase: CombatKnife
	{
		scope=0;
		displayName="Tomahawk";
		descriptionShort="Kiba Arms Tactical Tomahawk ";
		model="EFT\Knife\EFT_KibaAxe\EFT_KibaAxe.p3d";
		animClass="Knife";
		repairableWithKits[]={4};
		repairCosts[]={10};
		hiddenSelections[] = {"zbytek"};
		hiddenSelectionsTextures[] = {"EFT\Knife\EFT_KibaAxe\Data\knife.paa"};
		inventorySlot[]=
		{
			"Knife"
		};
		rotationFlags=17;
		canSkinBodies=1;
		RestrainUnlockType=1;
		weight=1861;
		itemSize[]={1,3};
		lootTag[]=
		{
			"Military_east",
			"Military_west"
		};
		lootCategory="Tools";
		itemInfo[]=
		{
			"Knife"
		};
		openItemSpillRange[]={10,20};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=100;
					healthLevels[]=
					{
						
						{1,{"EFT\Knife\EFT_KibaAxe\Data\Knife.rvmat"}},
						{0.69999999,{"EFT\Knife\EFT_KibaAxe\Data\Knife.rvmat"}},
						{0.5,{"EFT\Knife\EFT_KibaAxe\Data\Knife_damage.rvmat"}},
						{0.30000001,{"EFT\Knife\EFT_KibaAxe\Data\Knife_destruct.rvmat"}},
						{0,{"EFT\Knife\EFT_KibaAxe\Data\Knife_destruct.rvmat"}}
					};
				};
			};
		};
		isMeleeWeapon=1;
		suicideAnim="onehanded";
		class MeleeModes
		{
			class Default
			{
				ammo="MeleeMachete";
				range=1.2;
			};
			class Heavy
			{
				ammo="MeleeMachete_Heavy";
				range=1.2;
			};
			class Sprint
			{
				ammo="MeleeMachete_Heavy";
				range=3.3;
			};
		};
		class AnimEvents
		{
			class SoundWeapon
			{
				class animalSkinning_in
				{
					soundSet="animalSkinning_in_SoundSet";
					id=516;
				};
				class animalSkinning
				{
					soundSet="animalSkinning_SoundSet";
					id=517;
				};
				class animalSkinning_out
				{
					soundSet="animalSkinning_out_SoundSet";
					id=518;
				};
				class CutTies_end
				{
					soundSet="CutTies_end_SoundSet";
					id=519;
				};
				class CutTies_loop
				{
					soundSet="CutTies_loop_SoundSet";
					id=520;
				};
				class shaving_knife_clean
				{
					soundSet="shaving_knife_clean_SoundSet";
					id=600;
				};
				class shaving_knife1
				{
					soundSet="shaving_knife1_SoundSet";
					id=601;
				};
				class shaving_knife2
				{
					soundSet="shaving_knife2_SoundSet";
					id=602;
				};
				class shaving_knife3
				{
					soundSet="shaving_knife3_SoundSet";
					id=603;
				};
				class shaving_knife4
				{
					soundSet="shaving_knife4_SoundSet";
					id=604;
				};
				class shaving_knife5
				{
					soundSet="shaving_knife5_SoundSet";
					id=605;
				};
				class shaving_knife6
				{
					soundSet="shaving_knife6_SoundSet";
					id=606;
				};
				class shaving_knife7
				{
					soundSet="shaving_knife7_SoundSet";
					id=607;
				};
				class shaving_knife8
				{
					soundSet="shaving_knife8_SoundSet";
					id=608;
				};
				class shaving_knife9
				{
					soundSet="shaving_knife9_SoundSet";
					id=609;
				};
				class shaving_knife10
				{
					soundSet="shaving_knife10_SoundSet";
					id=610;
				};
				class shaving_knife11
				{
					soundSet="shaving_knife11_SoundSet";
					id=611;
				};
				class shaving_knife12
				{
					soundSet="shaving_knife12_SoundSet";
					id=612;
				};
				class shaving_knife13
				{
					soundSet="shaving_knife13_SoundSet";
					id=613;
				};
				class shaving_knife14
				{
					soundSet="shaving_knife14_SoundSet";
					id=614;
				};
				class shaving_knife15
				{
					soundSet="shaving_knife15_SoundSet";
					id=615;
				};
				class shaving_knife16
				{
					soundSet="shaving_knife16_SoundSet";
					id=616;
				};
				class shaving_knife17
				{
					soundSet="shaving_knife17_SoundSet";
					id=617;
				};
				class shaving_knife18
				{
					soundSet="shaving_knife18_SoundSet";
					id=618;
				};
				class shaving_knife19
				{
					soundSet="shaving_knife19_SoundSet";
					id=619;
				};
				class shaving_knife20
				{
					soundSet="shaving_knife20_SoundSet";
					id=620;
				};
				class shaving_knife21
				{
					soundSet="shaving_knife21_SoundSet";
					id=621;
				};
				class drop
				{
					soundset="combatknife_drop_SoundSet";
					id=898;
				};
			};
		};
	};
	
	class EFT_KibaAxe: EFT_KibaAxe_ColorBase
	{
		scope = 2;
		hiddenSelectionsTextures[] = {"EFT\Knife\EFT_KibaAxe\Data\knife.paa"};
	};
};

class CfgNonAIVehicles
	{
		class ProxyAttachment;
		class Proxycombatknife: ProxyAttachment
			{
				scope=2;
				inventorySlot="Knife";
				model="EFT\Knife\EFT_KibaAxe\EFT_KibaAxe.p3d";
			};
	};
	
