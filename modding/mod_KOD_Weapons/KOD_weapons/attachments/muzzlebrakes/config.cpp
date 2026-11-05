class CfgPatches
{
	class KOD_Weapons_MuzzleBrakes
	{
		units[]=
		{
			"KOD_Compensator_AR",
			"KOD_Compensator_AK"
		};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Weapons_Muzzles",
			"DZ_Scripts"
		};
		magazines[]={};
		ammo[]={};
	};
};
class cfgVehicles
{
	class ItemSuppressor;
	class KOD_Compensator_AR: ItemSuppressor
	{
		scope=2;
		displayName="Компенсатор AR";
		descriptionShort="Устройство, предназначенное для перенаправления части газов для борьбы с отдачей. Совместим с огнестрельным оружием калибра 5.56 мм и .308 Win.";
		model="KOD_weapons\attachments\MuzzleBrakes\m4_mb.p3d";
		rotationFlags=17;
		reversed=0;
		weight=80;
		itemSize[]={1,2};
		itemModelLength=0.02;
		inventorySlot[]=
		{
			"weaponMuzzleM4",
			"suppressorImpro"
		};
		dispersionModifier=-9.9999997e-05;
		dispersionCondition="true";
		barrelArmor=15000;
		recoilModifier[]={0.80000001,0.60000002,0.89999998};
		soundIndex=0;
		hiddenSelections[]=
		{
			"color"
		};
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\attachments\MuzzleBrakes\data\m4_compensator_co.paa"
		};
		class Particles
		{
			class OnFire
			{
				class MuzzleFlash
				{
					overrideParticle="weapon_shot_cz75_01";
					ignoreIfSuppressed=1;
					illuminateWorld=1;
				};
			};
			class OnOverheating
			{
				shotsToStartOverheating=5;
				maxOverheatingValue=30;
				overheatingDecayInterval=1;
				class SmokeTrail
				{
					overrideParticle="smoking_barrel_small";
					overrideDirectionVector[]={0,180,180};
					onlyWithinOverheatLimits[]={0,0.69999999};
				};
				class SmokeHot
				{
					overrideParticle="smoking_barrel_heavy";
					onlyWithinOverheatLimits[]={0.69999999,2};
				};
			};
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=7;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"KOD_weapons\attachments\MuzzleBrakes\data\m4_compensator.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"KOD_weapons\attachments\MuzzleBrakes\data\m4_compensator.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"KOD_weapons\attachments\MuzzleBrakes\data\m4_compensator_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"KOD_weapons\attachments\MuzzleBrakes\data\m4_compensator_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"KOD_weapons\attachments\MuzzleBrakes\data\m4_compensator_destruct.rvmat"
							}
						}
					};
				};
			};
		};
	};
	class KOD_Compensator_AK: ItemSuppressor
	{
		scope=2;
		displayName="Компенсатор АК";
		descriptionShort="Устройство, предназначенное для перенаправления части газов для борьбы с отдачей. Совместим с оружием на базе платформы АК.";
		model="KOD_weapons\attachments\MuzzleBrakes\ak_mb.p3d";
		rotationFlags=17;
		reversed=0;
		weight=80;
		itemSize[]={1,2};
		itemModelLength=0.02;
		inventorySlot[]=
		{
			"weaponMuzzleAK"
		};
		dispersionModifier=-9.9999997e-05;
		dispersionCondition="true";
		barrelArmor=15000;
		recoilModifier[]={0.80000001,0.60000002,0.89999998};
		soundIndex=0;
		hiddenSelections[]=
		{
			"color"
		};
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\attachments\MuzzleBrakes\data\ak_compensator_co.paa"
		};
		class Particles
		{
			class OnFire
			{
				class MuzzleFlash
				{
					overrideParticle="weapon_shot_cz75_01";
					ignoreIfSuppressed=1;
					illuminateWorld=1;
				};
			};
			class OnOverheating
			{
				shotsToStartOverheating=5;
				maxOverheatingValue=30;
				overheatingDecayInterval=1;
				class SmokeTrail
				{
					overrideParticle="smoking_barrel_small";
					overrideDirectionVector[]={0,180,180};
					onlyWithinOverheatLimits[]={0,0.69999999};
				};
				class SmokeHot
				{
					overrideParticle="smoking_barrel_heavy";
					onlyWithinOverheatLimits[]={0.69999999,2};
				};
			};
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=7;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"KOD_weapons\attachments\MuzzleBrakes\data\ak_compensator.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"KOD_weapons\attachments\MuzzleBrakes\data\ak_compensator.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"KOD_weapons\attachments\MuzzleBrakes\data\ak_compensator_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"KOD_weapons\attachments\MuzzleBrakes\data\ak_compensator_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"KOD_weapons\attachments\MuzzleBrakes\data\ak_compensator_destruct.rvmat"
							}
						}
					};
				};
			};
		};
	};
};
class CfgNonAIVehicles
{
	class ProxyAttachment;
	class Proxym4_mb: ProxyAttachment
	{
		scope=0;
		inventorySlot[]=
		{
			"weaponMuzzleM4"
		};
		model="KOD_weapons\attachments\MuzzleBrakes\m4_mb.p3d";
	};
	class Proxyak_mb: ProxyAttachment
	{
		scope=0;
		inventorySlot[]=
		{
			"weaponMuzzleAK"
		};
		model="KOD_weapons\attachments\MuzzleBrakes\ak_mb.p3d";
	};
};
