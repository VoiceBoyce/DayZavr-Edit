class CfgPatches
{
	class MSFZ_Troll
	{
		units[] = {"MSFZ_Troll"};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Characters"};
	};
};
class CfgAIBehaviours
{
	class MSFZ_Troll
	{
		name = "zombie";
		HeadLookBoneName = "lookat";
		teamName = "Zombies";
		class BehaviourHLZombie
		{
			class MovementWalk
			{
				maxSpeed = 1.5;
				minSpeed = 0;
				acceleration = 5;
				maxAngleSpeed = 180;
				slowRadius = 0;
				stopRadius = 0.5;
				pathFilter = "ZombieCalm";
			};
			class MovementRun
			{
				maxSpeed = 2;
				minSpeed = 0;
				acceleration = 10;
				maxAngleSpeed = 120;
				slowRadius = 0;
				goalRadius = 1.5;
				stopRadius = 1.7;
				useStartAnimation = "false";
				pathFilter = "ZombieAlerted";
			};
			class MovementSprint
			{
				maxSpeed = 4;
				minSpeed = 0;
				acceleration = 27;
				maxAngleSpeed = 180;
				maxSpeedRange = 0;
				slowRadius = 3.2;
				goalRadius = 1.5;
				goalSpeed = 7.2;
				stopRadius = 1.7;
				waypointRadius = 1.5;
				useStartAnimation = "false";
				startAnimationMaxSpeed = 0;
				slowToTurn = "false";
				smoothAcceleration = "true";
				pathFilter = "ZombieAlerted";
			};
			class SlotCalm
			{
				class BehaviourZombieCalm
				{
					StandingDurationMin = 4;
					StandingDurationMax = 8;
					WalkingDurationMin = 4;
					WalkingDurationMax = 12;
					MinLookTime = 1;
					MaxLookTime = 2;
					IsAttractMode = "false";
					class SoundsDuring
					{
						class Sound1
						{
							sounds[] = {"TrollWalk_SoundSet"};
						};
						probability = 0.8;
						RepeatTimeMin = 9;
						RepeatTimeMax = 15;
						RepeatEnabled = "true";
					};
				};
			};
			class SlotAlerted
			{
				class BehaviourZombieAlerted
				{
					maxTimeInDisturbedState = 5;
					alertToAttract = 0.8;
					fightStateEnterDistance = 2;
					fightStateExitDistance = 3;
					fightStateEnterOrientAngleDiff = 160;
					fightStateExitOrientAngleDiff = 160;
					disturbedTargetHistoryLength = 10;
					disturbedVisionUtilityWeight = 1;
					disturbedNoiseUtilityWeight = 1;
					disturbedDamageUtilityWeight = 1;
					attractedTargetHistoryLength = 10;
					attractedVisionUtilityWeight = 1;
					attractedNoiseUtilityWeight = 1;
					attractedDamageUtilityWeight = 1;
					chaseTargetHistoryLength = 10;
					chaseVisionUtilityWeight = 1;
					chaseNoiseUtilityWeight = 1;
					chaseDamageUtilityWeight = 1;
					class SoundsEntering
					{
						class Sound1
						{
							sounds[] = {"TrollPrepare_SoundSet"};
						};
						probability = 1;
					};
					class SoundsDuring
					{
						class Sound1
						{
							sounds[] = {"TrollСontrol_SoundSet"};
						};
						class Sound2
						{
							sounds[] = {"TrollAura_SoundSet"};
						};
						class Sound3
						{
							sounds[] = {"TrollHit_SoundSet"};
						};
						probability = 1;
						RepeatTimeMin = 3.9;
						RepeatTimeMax = 4.1;
						RepeatEnabled = "true";
					};
					noiseMakeAlertPeriod = 3;
					class NoiseMakeAlert
					{
						strength = 40;
						type = "sound";
					};
				};
			};
			class AlertSystem
			{
				visionToAlertMultiplier = 20;
				noiseToAlertMultiplier = 1;
				damageToAlertMultiplier = 10000;
				noiseShotToAlertMultiplier = 1.7;
				class Calm
				{
					DropSpeed = 1;
					DropDelay = 0;
					MaxAlertValue = 20;
				};
				class Alerted
				{
					DropSpeed = 3;
					DropDelay = 1;
					MaxAlertValue = 100;
				};
			};
			staminaDepletionSpeed = 5;
			staminaRechargeSpeed = 10;
		};
		class TargetSystemDZBase
		{
			visionProximityRange = 2.5;
			visionProximityStrengthMult = 2;
			visionCloseRange = 10;
			visionCloseHeight = 1.8;
			visionCloseStrengthMult = 1.5;
			visionRangeMin = 20;
			visionRangeMax = 40;
			visionFov = 1;
			visionPeripheralRangeMin = 2.5;
			visionPeripheralRangeMax = 20;
			visionPeripheralFov = 2.3;
			visionAngularSpeedMin = 0.1;
			visionAngularSpeedMax = 0.5;
			visionAngularSpeedMaxMult = 1;
			visionNightMinMult = 1;
			visionNightMaxMult = 0.3;
			visionRainMinMult = 1;
			visionRainMaxMult = 1;
			visionFogMinMult = 1;
			visionFogMaxMult = 1;
		};
		class NoiseSystemParams
		{
			rangeMin = 0;
			rangeMax = 35;
			rangeShotMin = 0;
			rangeShotMax = 200;
			radiusMin = 5;
			radiusMax = 20;
			radiusShotMin = 25;
			radiusShotMax = 50;
			class NoiseStrengthTeamMultipliers
			{
				BigGame = 1;
				Zombies = 1;
				Player = 1;
			};
		};
	};
};
class CfgVehicles
{
	class ZmbM_HermitSkinny_Base;
	class MSFZ_Troll_Base: ZmbM_HermitSkinny_Base
	{
		scope = 0;
		displayName = "$STR_MSFZ_Troll";
		descriptionShort = "$STR_MSFZ_Troll";
		hiddenSelections[] = {"Troll"};
		model = "MSFZ_Mifical_Pack\Troll\MSFZ_Troll.p3d";
		aiAgentTemplate = "MSFZ_Troll";
		simulation = "dayzinfected";
		attachments[] = {};
		class InventoryEquipment
		{
			playerSlots[] = {};
		};
		class Cargo
		{
			itemsCargoSize[] = {};
			allowOwnedCargoManipulation = 0;
			openable = 0;
		};
		class AttackActions
		{
			class AttackLong
			{
				attackName = "attackLong";
				ammoType = "MeleeZombieMale";
				stanceName = "erect";
				moveAnimNames[] = {"run","sprint"};
				minDistance = 0.75;
				distance = 1.75;
				time = 2.5;
				yawAngle = 0;
				pitchAngle = 0;
				attackWidth = 2.2;
				repeatable = 0;
				cooldown = 2.75;
			};
			class AttackRun
			{
				attackName = "attackRun";
				ammoType = "MeleeZombieMale";
				stanceName = "erect";
				moveAnimNames[] = {"run","sprint"};
				minDistance = 0.5;
				distance = 1.3;
				time = 1;
				yawAngle = 0;
				pitchAngle = 0;
				attackWidth = 1.5;
				repeatable = 1;
				cooldown = 1.25;
			};
			class AttackShort
			{
				attackName = "attackShort";
				ammoType = "MeleeZombieMale";
				stanceName = "erect";
				moveAnimNames[] = {"idle","walk"};
				minDistance = 0.5;
				distance = 1.3;
				time = 1;
				yawAngle = 0;
				pitchAngle = 0;
				attackWidth = 1.5;
				repeatable = 1;
				cooldown = 1.25;
			};
			class AttackShortLow
			{
				attackName = "attackShortLow";
				ammoType = "MeleeZombieMale";
				stanceName = "erect";
				moveAnimNames[] = {"idle","walk","run"};
				minDistance = 0;
				distance = 2;
				time = 1;
				yawAngle = 0;
				pitchAngle = -45;
				attackWidth = 2;
				repeatable = 1;
				cooldown = 1.25;
			};
			class CrawlAttackMove
			{
				attackName = "crawlAttackMove";
				ammoType = "MeleeZombieMale";
				stanceName = "crawl";
				moveAnimNames[] = {"walk"};
				distance = 2;
				time = 1.5;
				yawAngle = 0;
				pitchAngle = 45;
				attackWidth = 2;
				cooldown = 1.25;
			};
			class CrawlAttackStill
			{
				attackName = "crawlAttackStill";
				ammoType = "MeleeZombieMale";
				stanceName = "crawl";
				moveAnimNames[] = {"idle"};
				distance = 2;
				time = 1.1;
				yawAngle = 0;
				pitchAngle = 45;
				attackWidth = 2;
				cooldown = 1.25;
			};
		};
		class AnimEvents
		{
			class Steps
			{
				class Walk1
				{
					soundLookupTable = "walkErc_Boots_Zmb_LookupTable";
					noise = "ZombieStepNoise";
					id = 1;
				};
				class Walk2
				{
					soundLookupTable = "walkErc_Boots_Zmb_LookupTable";
					noise = "ZombieStepNoise";
					id = 2;
				};
				class Walk3
				{
					soundLookupTable = "walkErc_Boots_Zmb_LookupTable";
					noise = "ZombieStepNoise";
					id = 3;
				};
				class Walk4
				{
					soundLookupTable = "walkErc_Boots_Zmb_LookupTable";
					noise = "ZombieStepNoise";
					id = 4;
				};
				class Run1
				{
					soundLookupTable = "runErc_Boots_Zmb_LookupTable";
					noise = "ZombieStepNoise";
					id = 5;
				};
				class Run2
				{
					soundLookupTable = "runErc_Boots_Zmb_LookupTable";
					noise = "ZombieStepNoise";
					id = 6;
				};
				class Run3
				{
					soundLookupTable = "runErc_Boots_Zmb_LookupTable";
					noise = "ZombieStepNoise";
					id = 7;
				};
				class Run4
				{
					soundLookupTable = "runErc_Boots_Zmb_LookupTable";
					noise = "ZombieStepNoise";
					id = 8;
				};
				class Sprint1
				{
					soundLookupTable = "sprintErc_Boots_Zmb_LookupTable";
					noise = "ZombieStepNoise";
					id = 9;
				};
				class Sprint2
				{
					soundLookupTable = "sprintErc_Boots_Zmb_LookupTable";
					noise = "ZombieStepNoise";
					id = 10;
				};
				class Sprint3
				{
					soundLookupTable = "sprintErc_Boots_Zmb_LookupTable";
					noise = "ZombieStepNoise";
					id = 11;
				};
				class Sprint4
				{
					soundLookupTable = "sprintErc_Boots_Zmb_LookupTable";
					noise = "ZombieStepNoise";
					id = 12;
				};
				class Scuff1
				{
					soundLookupTable = "scuffErc_Boots_Zmb_LookupTable";
					noise = "ZombieStepNoise";
					id = 17;
				};
				class Scuff2
				{
					soundLookupTable = "scuffErc_Boots_Zmb_LookupTable";
					noise = "ZombieStepNoise";
					id = 18;
				};
				class Sccuff3
				{
					soundLookupTable = "scuffErc_Boots_Zmb_LookupTable";
					noise = "ZombieStepNoise";
					id = 19;
				};
				class Scuff4
				{
					soundLookupTable = "scuffErc_Boots_Zmb_LookupTable";
					noise = "ZombieStepNoise";
					id = 20;
				};
				class landFeetErc
				{
					soundLookupTable = "landFeetErc_Boots_Zmb_LookupTable";
					noise = "ZombieStepNoise";
					id = 21;
				};
				class landFootErc
				{
					soundLookupTable = "landFootErc_Boots_Zmb_LookupTable";
					noise = "ZombieStepNoise";
					id = 22;
				};
				class Bodyfall
				{
					soundLookupTable = "bodyfall_Zmb_LookupTable";
					noise = "ZombieStepNoise";
					id = 23;
				};
				class Bodyfall_Hand
				{
					soundLookupTable = "bodyfall_hand_Zmb_LookupTable";
					noise = "ZombieStepNoise";
					id = 24;
				};
				class Bodyfall_Slide
				{
					soundLookupTable = "bodyfall_slide_Zmb_LookupTable";
					noise = "ZombieStepNoise";
					id = 25;
				};
				class Prone_Walk_L
				{
					soundLookupTable = "walkProne_Zmb_LookupTable";
					noise = "ZombieStepNoise";
					id = 27;
				};
				class Prone_Walk_R
				{
					soundLookupTable = "walkProne_Zmb_LookupTable";
					noise = "ZombieStepNoise";
					id = 28;
				};
				class Prone_Run_L
				{
					soundLookupTable = "runProne_Zmb_LookupTable";
					noise = "ZombieStepNoise";
					id = 29;
				};
				class Prone_Run_R
				{
					soundLookupTable = "runProne_Zmb_LookupTable";
					noise = "ZombieStepNoise";
					id = 30;
				};
			};
			class Sounds
			{
				class Attack_Light1
				{
					soundSet = "TrollHandHit_SoundSet";
					id = 1;
				};
				class Attack_Light2
				{
					soundSet = "TrollHandHit_SoundSet";
					id = 2;
				};
				class Attack_Heavy1
				{
					soundSet = "TrollHandHit_SoundSet";
					id = 3;
				};
				class Attack_Heavy2
				{
					soundSet = "TrollHandHit_SoundSet";
					id = 4;
				};
				class TwoHands
				{
					soundSet = "TrollHandHit_SoundSet";
					id = 5;
				};
			};
			class SoundVoice
			{
				class LightHit
				{
					soundSet = "TrollHit_SoundSet";
					id = 1;
				};
				class HeavyHit
				{
					soundSet = "TrollHit_SoundSet";
					id = 2;
				};
				class Attack
				{
					soundSet = "TrollAttack_SoundSet";
					id = 5;
				};
				class Jump
				{
					soundSet = "ZmbF_Normal_Jump_Soundset";
					id = 10;
				};
				class Land
				{
					soundSet = "ZmbF_Normal_Land_Soundset";
					id = 11;
				};
				class CallToArmsShort
				{
					soundSet = "TrollRun_SoundSet";
					id = 20;
				};
			};
		};
		class Skinning
		{
			class ObtainedSteaks
			{
				item = "MutantTrollSteakMeat";
				count = 2;
				itemZones[] = {"Torso"};
				countByZone[] = {1,1};
				quantityMinMaxCoef[] = {0.3,1};
			};
			class ObtainedPelt
			{
				item = "MSFZ_Troll_Hand";
				count = 1;
				itemZones[] = {"LeftArm","RightArm"};
				quantityCoef = 1;
			};
		};
	};
	class MSFZ_Troll: MSFZ_Troll_Base
	{
		scope = 2;
		hiddenSelectionsTextures[] = {"\MSFZ_Mifical_Pack\Troll\data\MSFZ_Troll.paa"};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 12000;
					healthLevels[] = {{1,{"MSFZ_Mifical_Pack\Troll\data\MSFZ_Troll.rvmat"}},{0.7,{"MSFZ_Mifical_Pack\Troll\data\MSFZ_Troll.rvmat"}},{0.5,{"MSFZ_Mifical_Pack\Troll\data\MSFZ_Troll_damage.rvmat"}},{0.3,{"MSFZ_Mifical_Pack\Troll\data\MSFZ_Troll_damage.rvmat"}},{0,{"MSFZ_Mifical_Pack\Troll\data\MSFZ_Troll_destruct.rvmat"}}};
				};
			};
			class DamageZones
			{
				class Head
				{
					class Health
					{
						hitpoints = 6000;
						transferToGlobalCoef = 1;
					};
					class ArmorType
					{
						class Projectile
						{
							class Health
							{
								damage = 0.1;
							};
						};
					};
					componentNames[] = {"Head"};
					fatalInjuryCoef = 0.1;
					inventorySlots[] = {"Headgear","Mask"};
				};
				class Torso
				{
					class Health
					{
						hitpoints = 12000;
						transferToGlobalCoef = 1;
					};
					class ArmorType
					{
						class Projectile
						{
							class Health
							{
								damage = 0.1;
							};
						};
					};
					componentNames[] = {"Spine1","Spine3"};
					fatalInjuryCoef = -1;
					inventorySlots[] = {"Vest","Body","Back"};
				};
				class LeftArm
				{
					class Health
					{
						hitpoints = 12000;
						transferToGlobalCoef = 0.33;
					};
					class ArmorType
					{
						class Projectile
						{
							class Health
							{
								damage = 0.1;
							};
						};
					};
					componentNames[] = {"LeftArm","LeftForeArm"};
					fatalInjuryCoef = -1;
				};
				class RightArm
				{
					class Health
					{
						hitpoints = 12000;
						transferToGlobalCoef = 0.33;
					};
					class ArmorType
					{
						class Projectile
						{
							class Health
							{
								damage = 0.1;
							};
						};
					};
					componentNames[] = {"RightArm","RightForeArm"};
					fatalInjuryCoef = -1;
				};
				class LeftLeg
				{
					class Health
					{
						hitpoints = 12000;
						transferToGlobalCoef = 0.33;
					};
					class ArmorType
					{
						class Projectile
						{
							class Health
							{
								damage = 0.1;
							};
						};
					};
					componentNames[] = {"LeftLeg","LeftUpLeg"};
					fatalInjuryCoef = -1;
					inventorySlots[] = {"Legs"};
				};
				class RightLeg
				{
					class Health
					{
						hitpoints = 12000;
						transferToGlobalCoef = 0.33;
					};
					class ArmorType
					{
						class Projectile
						{
							class Health
							{
								damage = 0.1;
							};
						};
					};
					componentNames[] = {"RightLeg","RightUpLeg"};
					fatalInjuryCoef = -1;
					inventorySlots[] = {"Legs"};
				};
				class LeftFoot
				{
					class Health
					{
						hitpoints = 12000;
						transferToGlobalCoef = 0.1;
					};
					class ArmorType
					{
						class Projectile
						{
							class Health
							{
								damage = 0.1;
							};
						};
					};
					transferToZonesNames[] = {"LeftLeg"};
					transferToZonesCoefs[] = {0.1};
					componentNames[] = {"LeftFoot"};
					fatalInjuryCoef = -1;
					inventorySlots[] = {"Feet"};
				};
				class RightFoot
				{
					class Health
					{
						hitpoints = 12000;
						transferToGlobalCoef = 0.1;
					};
					class ArmorType
					{
						class Projectile
						{
							class Health
							{
								damage = 0.1;
							};
						};
					};
					transferToZonesNames[] = {"RightLeg"};
					transferToZonesCoefs[] = {0.1};
					componentNames[] = {"RightFoot"};
					fatalInjuryCoef = -1;
					inventorySlots[] = {"Feet"};
				};
			};
		};
	};
};
class CfgSoundShaders
{
	class baseCharacter_SoundShader
	{
		range = 50;
	};
	class TrollAttack_SoundShader
	{
		samples[] = {{"MSFZ_Mifical_Pack\Troll\sound\Troll_attack_0",1},{"MSFZ_Mifical_Pack\Troll\sound\Troll_attack_1",1}};
		volume = 1.78;
		range = 80;
		rangeCurve = "defaultAnimalAttenuationCurve";
	};
	class TrollHit_SoundShader
	{
		samples[] = {{"MSFZ_Mifical_Pack\Troll\sound\Troll_hit_0",1},{"MSFZ_Mifical_Pack\Troll\sound\Troll_hit_1",1},{"MSFZ_Mifical_Pack\Troll\sound\Troll_hit_2",1}};
		volume = 1.78;
		range = 80;
		rangeCurve = "defaultAnimalAttenuationCurve";
	};
	class TrollHandHit_SoundShader
	{
		samples[] = {{"MSFZ_Mifical_Pack\Troll\sound\Troll_hand_hit_0",1}};
		volume = 1.78;
		range = 80;
		rangeCurve = "defaultAnimalAttenuationCurve";
	};
	class TrollPrepare_SoundShader
	{
		samples[] = {{"MSFZ_Mifical_Pack\Troll\sound\Troll_prepare_1",1}};
		volume = 2.05;
		range = 80;
		rangeCurve = "defaultAnimalAttenuationCurve";
	};
	class TrollControl_SoundShader
	{
		samples[] = {{"MSFZ_Mifical_Pack\Troll\sound\Troll_control_1",1},{"MSFZ_Mifical_Pack\Troll\sound\Troll_control_2",1}};
		volume = 2.05;
		range = 80;
		rangeCurve = "defaultAnimalAttenuationCurve";
	};
	class TrollAura_SoundShader
	{
		samples[] = {{"MSFZ_Mifical_Pack\Troll\sound\Troll_aura_l",1},{"MSFZ_Mifical_Pack\Troll\sound\Troll_aura_r",1}};
		volume = 3.05;
		range = 80;
		rangeCurve = "defaultAnimalAttenuationCurve";
	};
	class TrollHit2_SoundShader
	{
		samples[] = {{"MSFZ_Mifical_Pack\Troll\sound\Troll_hit_l",1},{"MSFZ_Mifical_Pack\Troll\sound\Troll_hit_r",1}};
		volume = 3.05;
		range = 80;
		rangeCurve = "defaultAnimalAttenuationCurve";
	};
	class TrollWalk_SoundShader
	{
		samples[] = {{"MSFZ_Mifical_Pack\Troll\sound\Troll_scan_1",1},{"MSFZ_Mifical_Pack\Troll\sound\Troll_scan_2",1},{"MSFZ_Mifical_Pack\Troll\sound\Troll_scan_3",1},{"MSFZ_Mifical_Pack\Troll\sound\Troll_scan_4",1}};
		volume = 4.55;
		range = 120;
		rangeCurve = "defaultAnimalAttenuationCurve";
	};
	class TrollRun_SoundShader
	{
		samples[] = {{"MSFZ_Mifical_Pack\Troll\sound\Troll_run_0",1}};
		volume = 1.78;
		range = 100;
		rangeCurve = "defaultAnimalAttenuationCurve";
	};
};
class CfgSoundSets
{
	class TrollAttack_SoundSet
	{
		soundShaders[] = {"TrollAttack_SoundShader"};
	};
	class TrollHit_SoundSet
	{
		soundShaders[] = {"TrollHit_SoundShader"};
	};
	class TrollHandHit_SoundSet
	{
		soundShaders[] = {"TrollHandHit_SoundShader"};
	};
	class TrollPrepare_SoundSet
	{
		soundShaders[] = {"TrollPrepare_SoundShader"};
	};
	class TrollControl_SoundSet
	{
		soundShaders[] = {"TrollControl_SoundShader"};
	};
	class TrollAura_SoundSet
	{
		soundShaders[] = {"TrollAura_SoundShader"};
	};
	class TrollHit2_SoundSet
	{
		soundShaders[] = {"TrollHit_SoundShader"};
	};
	class TrollWalk_SoundSet
	{
		soundShaders[] = {"TrollWalk_SoundShader"};
	};
	class TrollRun_SoundSet
	{
		soundShaders[] = {"TrollRun_SoundShader"};
	};
};
