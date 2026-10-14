class CfgPatches
{
	class SM_VP_Multi_Belt_Set
	{
		units[]={};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Data"
		};
	};
};
class cfgVehicles
{
	class Man;
	class Clothing;
	class SM_VP_Multi_Belt_ColorBase: Clothing
	{
		scope=0;
		displayName="Набедренная кобура Мульти";
		descriptionShort="Набедренная кабура от сета Мульти.";
		model="set_mulit\belt\model\belt_g.p3d";
		attachments[]=
		{
			"Pistol"
		};
		vehicleClass="Clothing";
		simulation="clothing";
		inventorySlot[]=
		{
			"Hips"
		};
		itemInfo[]=
		{
			"Clothing",
			"Hips"
		};
		itemSize[]={5,5};
		quickBarBonus=2;
		varWetMax=0.048999999;
		heatIsolation=0.5;
		weight=550;
		repairableWithKits[]={5,3};
		repairCosts[]={30,25};
		lootCategory="Crafted";
		hiddenSelections[]=
		{
			"fast",
			"kobura",
			"remen",
			"remen_noga"
		};
		class ClothingTypes
		{
			male="set_mulit\belt\model\belt_m.p3d";
			female="set_mulit\belt\model\belt_f.p3d";
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=500;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"set_mulit\belt\data\fast.rvmat",
								"set_mulit\belt\data\kobura.rvmat",
								"set_mulit\belt\data\remen.rvmat",
								"set_mulit\belt\data\remen_noga.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"set_mulit\belt\data\fast.rvmat",
								"set_mulit\belt\data\kobura.rvmat",
								"set_mulit\belt\data\remen.rvmat",
								"set_mulit\belt\data\remen_noga.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"set_mulit\belt\data\fast_damage.rvmat",
								"set_mulit\belt\data\kobura_damage.rvmat",
								"set_mulit\belt\data\remen_damage.rvmat",
								"set_mulit\belt\data\remen_noga_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"set_mulit\belt\data\fast_damage.rvmat",
								"set_mulit\belt\data\kobura_damage.rvmat",
								"set_mulit\belt\data\remen_damage.rvmat",
								"set_mulit\belt\data\remen_noga_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"set_mulit\belt\data\fast_destruct.rvmat",
								"set_mulit\belt\data\kobura_destruct.rvmat",
								"set_mulit\belt\data\remen_destruct.rvmat",
								"set_mulit\belt\data\remen_noga_destruct.rvmat"
							}
						}
					};
				};
			};
		};
		class AnimEvents
		{
			class SoundWeapon
			{
				class pickUpItem
				{
					soundSet="WorkingGloves_pickup_SoundSet";
					id=797;
				};
				class drop
				{
					soundset="WorkingGloves_drop_SoundSet";
					id=898;
				};
			};
		};
	};
	class SM_VP_Multi_Belt: SM_VP_Multi_Belt_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"set_mulit\belt\data\fast_co.paa",
			"set_mulit\belt\data\kobura_co.paa",
			"set_mulit\belt\data\remen_co.paa",
			"set_mulit\belt\data\remen_noga_co.paa"
		};
	};
	class SurvivorBase: Man
	{
		class DayZPlayer
		{
			class P3DAttachments
			{
				P3DProxies[]=
				{
					"spine1",
					"righthand_dummy",
					"spine2",
					"head",
					"rightlegroll",
					"spine3",
					"lefthand",
					"pelvis",
					"leftshoulder",
					"rightshoulder",
					"rightleg",
					"leftleg",
					"rightkneeextra",
					"rightupleg"
				};
				BoneNames[]=
				{
					"Spine1",
					"RightHand_Dummy",
					"Spine2",
					"Head",
					"RightLegRoll",
					"Spine3",
					"LeftHand",
					"Pelvis",
					"LeftShoulder",
					"RightShoulder",
					"RightLeg",
					"LeftLeg",
					"RightKneeExtra",
					"RightUpLeg"
				};
			};
		};
	};
};
