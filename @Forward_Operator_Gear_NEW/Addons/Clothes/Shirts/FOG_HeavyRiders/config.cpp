class CfgPatches
{
	class FOG_HeavyRiders_Jacket_Shit
	{
		units[]=
		{
			"FOG_HeavyRiders_Jacket_Black",
			"FOG_HeavyRiders_Jacket_Teal",
			"FOG_HeavyRiders_Jacket_Red",
			"FOG_HeavyRiders_Jacket_Green",
			"FOG_HeavyRiders_Jacket_RB"
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
	class FOG_HeavyRiders_Jacket_ColorBase: Clothing
	{
		displayName="Heavy Riders Jacket";
		descriptionShort="Heavy Riders Jacket — эта легендарная байкерская куртка с 63 слотами хранит историю в каждой царапине: ходят слухи, что её либо вырвали из холодных рук лидера мёртвого мотоклуба, либо нашли в заброшенном ангаре среди ржавых запчастей и пустых бутылок виски | The Heavy Riders Jacket is this battle-scarred 63-slot biker relic where every scratch tells a story - rumors say it was either pried from a dead MC president's grip or found in some derelict garage amid rusted parts and empty whiskey bottles.";
		model="\FOG_MOD\Clothes\Shirts\FOG_HeavyRiders\FOG_HeavyRiders_G.p3d";
		inventorySlot[]=
		{
			"Body"
		};
		itemInfo[]=
		{
			"Clothing",
			"Body"
		};
		weight=870;
		itemSize[]={5,3};
		itemsCargoSize[]={9,7};
		quickBarBonus=1;
		varWetMax=0.30000001;
		heatIsolation=0.49999998;
		ragQuantity=4;
		repairableWithKits[]={5,2};
		repairCosts[]={30,25};
		soundAttType="WoolShirt";
		hiddenSelections[]=
		{
			"camo"
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=1500;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"FOG_MOD\Clothes\Shirts\FOG_HeavyRiders\data\FOG_HeavyRiders.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"FOG_MOD\Clothes\Shirts\FOG_HeavyRiders\data\FOG_HeavyRiders.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"FOG_MOD\Clothes\Shirts\FOG_HeavyRiders\data\FOG_HeavyRiders_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"FOG_MOD\Clothes\Shirts\FOG_HeavyRiders\data\FOG_HeavyRiders_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"FOG_MOD\Clothes\Shirts\FOG_HeavyRiders\data\FOG_HeavyRiders_destruct.rvmat"
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
		class ClothingTypes
		{
			male="\FOG_MOD\Clothes\Shirts\FOG_HeavyRiders\FOG_HeavyRiders_M.p3d";
			female="\FOG_MOD\Clothes\Shirts\FOG_HeavyRiders\FOG_HeavyRiders_F.p3d";
		};
		class AnimEvents
		{
			class SoundWeapon
			{
				class pickUpItem
				{
					soundSet="Shirt_pickup_SoundSet";
					id=797;
				};
				class drop
				{
					soundset="Shirt_drop_SoundSet";
					id=898;
				};
			};
		};
	};
	class FOG_HeavyRiders_Jacket_Black: FOG_HeavyRiders_Jacket_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\FOG_HeavyRiders\data\FOG_HeavyRider_co.paa"
		};
	};
	class FOG_HeavyRiders_Jacket_Teal: FOG_HeavyRiders_Jacket_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\FOG_HeavyRiders\data\FOG_HeavyRider_Teal_co.paa"
		};
	};
	class FOG_HeavyRiders_Jacket_Red: FOG_HeavyRiders_Jacket_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\FOG_HeavyRiders\data\FOG_HeavyRider_Red_co.paa"
		};
	};
	class FOG_HeavyRiders_Jacket_Green: FOG_HeavyRiders_Jacket_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\FOG_HeavyRiders\data\FOG_HeavyRider_Green_co.paa"
		};
	};
	class FOG_HeavyRiders_Jacket_RB: FOG_HeavyRiders_Jacket_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\FOG_HeavyRiders\data\FOG_HeavyRider_Rainbow_co.paa"
		};
	};
};
