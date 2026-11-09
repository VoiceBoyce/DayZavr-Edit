class CfgPatches
{
	class Mutant_Spider
	{
		units[] = {"MSFZ_Mutant_Spider1","MSFZ_Mutant_Spider2","MSFZ_Mutant_Spider3"};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Animals"};
		ammo[] = {"MeleeSpider","MeleeSpider_Hight"};
	};
};
class CfgAmmo
{
	class MeleeWolf;
	class MeleeSpider: MeleeWolf
	{
		class DamageApplied
		{
			type = "Melee";
			bleedThreshold = 1;
			class Health
			{
				damage = 18;
			};
			class Blood
			{
				damage = 150;
			};
			class Shock
			{
				damage = 8;
			};
			additionAnimalMeleeMultiplier = 5;
		};
	};
	class MeleeSpider_Hight: MeleeWolf
	{
		class DamageApplied
		{
			type = "Melee";
			bleedThreshold = 1;
			class Health
			{
				damage = 32;
			};
			class Blood
			{
				damage = 220;
			};
			class Shock
			{
				damage = 10;
			};
			additionAnimalMeleeMultiplier = 5;
		};
	};
};
class CfgDamages
{
	class SpiderBiteDamage
	{
		bone = "chest";
		ammo = "MeleeSpider";
		radius = 0.7;
		duration = 0.2;
	};
	class SpiderLowBiteLDamage
	{
		bone = "lfleg3";
		ammo = "MeleeSpider_Hight";
		radius = 0.7;
		duration = 0.5;
	};
	class SpiderLowBiteRDamage
	{
		bone = "rfleg3";
		ammo = "MeleeSpider_Hight";
		radius = 0.7;
		duration = 0.5;
	};
};
class GroupBehaviourTemplates
{
	class SpiderGroupBeh
	{
		type = "Predators";
		alertDistributionSpeed = 20;
		catchUpTestDelay = 4;
		catchUpStartRadius = 80;
		catchUpTargetRadius = 7;
		groupRadius = 10;
		spawnMinDistanceBetweenAgents = 1;
		agentPathLength = 100;
		atNeedMinDuration = 60;
		atNeedMaxDuration = 120;
		singleAgentSafeKeeperDelayMin = 100;
		singleAgentSafeKeeperDelayMax = 100;
		preyAttractionRange = 350;
		innerOuterCircleRatio = 0.6;
		endAttractionRange = 10;
		attractionCooldown = 60;
		endAttractionTime = 300;
		targetEscapingSpeed = 1.5;
		killAddFear = 34;
		eatingTime = 600;
		huntingCooldown = 120;
		maxHuntingTime = 60;
		safeKeeperIntervalMin = 20;
		safeKeeperIntervalMax = 40;
		siegeAttackCountdownMin = 1;
		siegeAttackCountdownMax = 12;
		huntAttackCountdownMin = 1;
		huntAttackCountdownMax = 10;
		changeTargetCooldown = 5;
		changeTargetAlertRatio = 1.3;
		changeTargetEffectRadius = 6;
		subgroupSpacingMax = 500;
		class LifeCycleDayTime
		{
			class Activity1
			{
				endTimeMin = 8.5;
				endTimeMax = 8.5;
				zoneType = "Rest";
			};
			class Activity2
			{
				endTimeMin = 16;
				endTimeMax = 16;
				zoneType = "HuntingGround";
				stayInZone = "false";
			};
			class Activity3
			{
				endTimeMin = 18.5;
				endTimeMax = 19.5;
				zoneType = "Rest";
			};
			class Activity4
			{
				endTimeMin = 22.5;
				endTimeMax = 23.5;
				zoneType = "HuntingGround";
				stayInZone = "false";
			};
		};
	};
};
class CfgAIBehaviours
{
	class Predators_Spider
	{
		HeadLookBoneName = "pin_lookat";
		teamName = "Predator";
		defaultGroupTemplateName = "SpiderGroupBeh";
		class PathAgent
		{
			radius = 0.3;
			height = 1;
			lengthRadius = 0.7;
		};
		class BehaviourHLPredator
		{
			instantAlertRangeMin = 0;
			instantAlertRangeMax = 0;
			instantAlertStrength = 0;
			proximityAttackRange = 2.5;
			attackCooldown = 3;
			class SlotCalmGrazing
			{
				class BehaviourCalm
				{
					travelingMode = "true";
					grazeOnSpotWeight = 20;
					grazeWalkingWeight = 20;
					restWeight = 0;
					travelWeight = 0;
					grazeOnSpotDurationMin = 5;
					grazeOnSpotDurationMax = 10;
					grazeWalkingDurationMin = 500;
					grazeWalkingDurationMax = 500;
					restingDurationMin = 5;
					restingDurationMax = 10;
					travelingDurationMin = 15;
					travelingDurationMax = 30;
					grazeWalkingSpeed = 0.2;
					travelingWalkingSpeed = 1.36;
					safetyDurationMin = 10;
					safetyDurationMax = 10;
					safetyLookAngleMin = 0.3;
					safetyLookAngleMax = 0.7;
					safetyLookAngleChangeInterval = 3;
					class GrazeMovement
					{
						maxSpeed = 0.25;
						minSpeed = 0.25;
						acceleration = 5;
						maxAngleSpeed = 90;
						slowRadius = 2;
						stopRadius = 1;
						pathFilter = "WolfOnHunt";
					};
					class TravelingMovement
					{
						maxSpeed = 2.85;
						minSpeed = 0.78;
						acceleration = 5;
						maxAngleSpeed = 90;
						slowRadius = 2;
						stopRadius = 1;
						pathFilter = "WolfOnHunt";
						slowToTurn = "true";
						smoothAcceleration = "true";
					};
					class CatchUpMovement
					{
						maxSpeed = 6.38;
						minSpeed = 0.78;
						acceleration = 5;
						maxAngleSpeed = 90;
						slowRadius = 2;
						stopRadius = 1;
						pathFilter = "WolfOnHunt";
						slowToTurn = "true";
						smoothAcceleration = "true";
					};
				};
			};
			class SlotSiege
			{
				class BehaviourSiege
				{
					class SoundsEntering
					{
						class Sound1
						{
							sounds[] = {"SpiderHurt_SoundSet"};
						};
						class Sound2
						{
							sounds[] = {"SpiderHurt_SoundSet"};
						};
						class Sound3
						{
							sounds[] = {"SpiderHurt_SoundSet"};
						};
						probability = 1;
					};
					class SoundsDuring
					{
						class Sound1
						{
							sounds[] = {"SpiderHurt_SoundSet"};
						};
						class Sound2
						{
							sounds[] = {"SpiderHurt_SoundSet"};
						};
						class Sound3
						{
							sounds[] = {"SpiderHurt_SoundSet"};
						};
						class Sound4
						{
							sounds[] = {"SpiderHurt_SoundSet"};
						};
						class Sound5
						{
							sounds[] = {"SpiderHurt_SoundSet"};
						};
						class Sound6
						{
							sounds[] = {"SpiderGrowl_SoundSet"};
						};
						class Sound7
						{
							sounds[] = {"SpiderGrowl_SoundSet"};
						};
						class Sound8
						{
							sounds[] = {"SpiderGrowl_SoundSet"};
						};
						probability = 0.4;
						RepeatTimeMin = 5;
						RepeatTimeMax = 15;
						RepeatEnabled = "true";
					};
					innerRadius = 7;
					innerRadiusMin = 4.5;
					innerRadiusMax = 10;
					outerRadius = 16;
					directionChangeTimeMin = 7;
					directionChangeTimeMax = 25;
					PlayerFOV = 1.4;
					preferPlayerFOVCooldown = 1;
					attackDistance = 3.5;
					class InnerCircleMovement
					{
						maxSpeed = 6.3;
						optimalSpeed = 6.3;
						optimalSpeedRange = 1;
						minSpeed = 1;
						acceleration = 7;
						maxAngleSpeed = 90;
						slowRadius = 0;
						stopRadius = 2;
						maxSpeedRange = 2;
						pathFilter = "WolfOnHunt";
						startAnimationMaxSpeed = 0.54;
					};
					class Movement
					{
						maxSpeed = 9;
						optimalSpeed = 6.3;
						optimalSpeedRange = 15;
						minSpeed = 1;
						acceleration = 10;
						maxAngleSpeed = 180;
						slowRadius = 0;
						stopRadius = 2;
						maxSpeedRange = 20;
						pathFilter = "WolfOnOuterCircle";
						startAnimationMaxSpeed = 0.54;
					};
					class AttackMovement
					{
						maxSpeed = 12.175;
						optimalSpeed = 12;
						optimalSpeedRange = 6;
						minSpeed = 0.8;
						acceleration = 10;
						maxAngleSpeed = 180;
						slowRadius = 2;
						stopRadius = 3;
						maxSpeedRange = 30;
						pathFilter = "WolfOnHunt";
					};
				};
			};
			class SlotEating
			{
				class BehaviourCalm
				{
					grazeOnSpotWeight = 20;
					grazeWalkingWeight = 20;
					restWeight = 0;
					travelWeight = 20;
					eatingWeight = 20;
					grazeOnSpotDurationMin = 5;
					grazeOnSpotDurationMax = 10;
					grazeWalkingDurationMin = 10;
					grazeWalkingDurationMax = 15;
					restingDurationMin = 15;
					restingDurationMax = 25;
					travelingDurationMin = 15;
					travelingDurationMax = 30;
					eatingDurationMin = 15;
					eatingDurationMax = 25;
					safetyDurationMin = 10;
					safetyDurationMax = 20;
					safetyLookAngleMin = 0.1;
					safetyLookAngleMax = 1.5;
					safetyLookAngleChangeInterval = 7;
					class GrazeMovement
					{
						maxSpeed = 0.25;
						minSpeed = 0.25;
						acceleration = 5;
						maxAngleSpeed = 90;
						slowRadius = 2;
						stopRadius = 1;
						pathFilter = "WolfOnHunt";
					};
					class TravelingMovement
					{
						maxSpeed = 2.85;
						minSpeed = 0.78;
						acceleration = 5;
						maxAngleSpeed = 90;
						slowRadius = 2;
						stopRadius = 1;
						pathFilter = "WolfOnHunt";
						slowToTurn = "true";
						smoothAcceleration = "true";
					};
					class CatchUpMovement
					{
						maxSpeed = 6.38;
						minSpeed = 0.78;
						acceleration = 5;
						maxAngleSpeed = 90;
						slowRadius = 2;
						stopRadius = 1;
						pathFilter = "WolfOnHunt";
						slowToTurn = "true";
						smoothAcceleration = "true";
					};
				};
			};
			class SlotHunting
			{
				class BehaviourHunt
				{
					attackRange = 3.5;
					followingRadius = 15;
					followingRadiusReqroupCooldownMin = 5;
					followingRadiusReqroupCooldownMax = 15;
					followingRadiusRegroupMinSpeed = 1.5;
					predictFollowingMinDistance = 10;
					minDistanceToTargetRatio = 0.3;
					class SoundsEntering
					{
						class Sound1
						{
							sounds[] = {"SpiderDie_SoundSet"};
						};
						probability = 0.3;
					};
					class SoundsDuring
					{
						class Sound1
						{
							sounds[] = {"SpiderDie_SoundSet"};
						};
						class Sound2
						{
							sounds[] = {"SpiderDie_SoundSet"};
						};
						class Sound3
						{
							sounds[] = {"SpiderDie_SoundSet"};
						};
						class Sound4
						{
							sounds[] = {"SpiderDie_SoundSet"};
						};
						class Sound5
						{
							sounds[] = {"SpiderDie_SoundSet"};
						};
						probability = 1;
						RepeatTimeMin = 10;
						RepeatTimeMax = 30;
						RepeatEnabled = "true";
					};
					class Movement
					{
						maxSpeed = 10;
						optimalSpeed = 9.5;
						minSpeed = 1;
						acceleration = 5;
						maxAngleSpeed = 90;
						slowRadius = 2;
						stopRadius = 1;
						maxSpeedRange = 15;
						optimalSpeedRange = 5;
						pathFilter = "WolfOnHunt";
					};
					class MovementAttack
					{
						maxSpeed = 12.175;
						optimalSpeed = 12.175;
						minSpeed = 6;
						acceleration = 20;
						maxAngleSpeed = 360;
						slowRadius = 0;
						stopRadius = 0;
						maxSpeedRange = 3;
						optimalSpeedRange = 1;
						pathFilter = "WolfOnHunt";
					};
				};
			};
			class SlotCalmResting
			{
				class BehaviourCalm
				{
					travelingMode = "true";
					grazeOnSpotWeight = 50;
					grazeWalkingWeight = 50;
					restWeight = 50;
					travelWeight = 0;
					grazeOnSpotDurationMin = 5;
					grazeOnSpotDurationMax = 10;
					grazeWalkingDurationMin = 15;
					grazeWalkingDurationMax = 25;
					restingDurationMin = 25;
					restingDurationMax = 35;
					travelingDurationMin = 15;
					travelingDurationMax = 30;
					safetyDurationMin = 10;
					safetyDurationMax = 10;
					safetyLookAngleMin = 0.1;
					safetyLookAngleMax = 1.5;
					safetyLookAngleChangeInterval = 7;
					class GrazeMovement
					{
						maxSpeed = 0.25;
						minSpeed = 0.25;
						acceleration = 5;
						maxAngleSpeed = 90;
						slowRadius = 2;
						stopRadius = 1;
						pathFilter = "WolfOnHunt";
					};
					class TravelingMovement
					{
						maxSpeed = 2.85;
						minSpeed = 0.78;
						acceleration = 5;
						maxAngleSpeed = 90;
						slowRadius = 2;
						stopRadius = 1;
						pathFilter = "WolfOnHunt";
						slowToTurn = "true";
						smoothAcceleration = "true";
					};
					class CatchUpMovement
					{
						maxSpeed = 6.38;
						minSpeed = 0.78;
						acceleration = 5;
						maxAngleSpeed = 90;
						slowRadius = 2;
						stopRadius = 1;
						pathFilter = "WolfOnHunt";
						slowToTurn = "true";
						smoothAcceleration = "true";
					};
				};
			};
			class SlotCalmTravelling
			{
				class BehaviourCalm
				{
					travelingMode = "true";
					grazeOnSpotWeight = 20;
					grazeWalkingWeight = 20;
					restWeight = 0;
					travelWeight = 50;
					grazeOnSpotDurationMin = 10;
					grazeOnSpotDurationMax = 20;
					grazeWalkingDurationMin = 25;
					grazeWalkingDurationMax = 35;
					restingDurationMin = 5;
					restingDurationMax = 10;
					travelingDurationMin = 10;
					travelingDurationMax = 50;
					safetyDurationMin = 10;
					safetyDurationMax = 10;
					safetyLookAngleMin = 0.3;
					safetyLookAngleMax = 0.7;
					safetyLookAngleChangeInterval = 3;
					class TravelingMovement
					{
						maxSpeed = 2.85;
						minSpeed = 0.78;
						acceleration = 5;
						maxAngleSpeed = 90;
						slowRadius = 4;
						stopRadius = 2;
						slowToTurn = "true";
						smoothAcceleration = "true";
					};
					class CatchUpMovement
					{
						maxSpeed = 6.38;
						minSpeed = 0.78;
						acceleration = 5;
						maxAngleSpeed = 90;
						slowRadius = 2;
						stopRadius = 1;
						pathFilter = "WolfOnHunt";
						slowToTurn = "true";
						smoothAcceleration = "true";
					};
					class GrazeMovement
					{
						maxSpeed = 0.25;
						minSpeed = 0.25;
						acceleration = 5;
						maxAngleSpeed = 90;
						slowRadius = 2;
						stopRadius = 1;
						pathFilter = "WolfOnHunt";
					};
				};
			};
			class SlotAttracted
			{
				class BehaviourCalm
				{
					class SoundsEntering
					{
						class Sound1
						{
							sounds[] = {"SpiderEmpty_SoundSet","SpiderEmpty_SoundSet"};
						};
						class Sound2
						{
							sounds[] = {"SpiderEmpty_SoundSet","SpiderEmpty_SoundSet"};
						};
						class Sound3
						{
							sounds[] = {"SpiderEmpty_SoundSet","SpiderEmpty_SoundSet"};
						};
						class Sound4
						{
							sounds[] = {"SpiderEmpty_SoundSet","SpiderEmpty_SoundSet"};
						};
						class Sound5
						{
							sounds[] = {"SpiderEmpty_SoundSet","SpiderEmpty_SoundSet"};
						};
						class Sound6
						{
							sounds[] = {"SpiderEmpty_SoundSet","SpiderEmpty_SoundSet"};
						};
						class Sound7
						{
							sounds[] = {"SpiderEmpty_SoundSet","SpiderEmpty_SoundSet"};
						};
						class Sound8
						{
							sounds[] = {"SpiderEmpty_SoundSet","SpiderEmpty_SoundSet"};
						};
						probability = 1;
					};
					class SoundsDuring
					{
						class Sound1
						{
							sounds[] = {"SpiderEmpty_SoundSet","SpiderEmpty_SoundSet"};
						};
						class Sound2
						{
							sounds[] = {"SpiderEmpty_SoundSet","SpiderEmpty_SoundSet"};
						};
						class Sound3
						{
							sounds[] = {"SpiderEmpty_SoundSet","SpiderEmpty_SoundSet"};
						};
						class Sound4
						{
							sounds[] = {"SpiderEmpty_SoundSet","SpiderEmpty_SoundSet"};
						};
						class Sound5
						{
							sounds[] = {"SpiderEmpty_SoundSet","SpiderEmpty_SoundSet"};
						};
						class Sound6
						{
							sounds[] = {"SpiderEmpty_SoundSet","SpiderEmpty_SoundSet"};
						};
						class Sound7
						{
							sounds[] = {"SpiderEmpty_SoundSet","SpiderEmpty_SoundSet"};
						};
						class Sound8
						{
							sounds[] = {"SpiderEmpty_SoundSet","SpiderEmpty_SoundSet"};
						};
						probability = 0.5;
						RepeatTimeMin = 5;
						RepeatTimeMax = 25;
						RepeatEnabled = "true";
					};
					grazeOnSpotWeight = 20;
					grazeWalkingWeight = 20;
					restWeight = 0;
					travelWeight = 50;
					grazeOnSpotDurationMin = 5;
					grazeOnSpotDurationMax = 10;
					grazeWalkingDurationMin = 5;
					grazeWalkingDurationMax = 10;
					restingDurationMin = 5;
					restingDurationMax = 10;
					travelingDurationMin = 50;
					travelingDurationMax = 100;
					grazeWalkingSpeed = 0.54;
					travelingWalkingSpeed = 0.78;
					safetyDurationMin = 10;
					safetyDurationMax = 10;
					safetyLookAngleMin = 0.3;
					safetyLookAngleMax = 0.7;
					safetyLookAngleChangeInterval = 3;
					class TravelingMovement
					{
						maxSpeed = 2.85;
						minSpeed = 0.78;
						acceleration = 5;
						maxAngleSpeed = 90;
						slowRadius = 5;
						stopRadius = 3;
						slowToTurn = "true";
						smoothAcceleration = "true";
						pathFilter = "WolfOnHunt";
					};
					class CatchUpMovement
					{
						maxSpeed = 6.38;
						minSpeed = 0.78;
						acceleration = 5;
						maxAngleSpeed = 90;
						slowRadius = 0;
						stopRadius = 0.5;
						slowToTurn = "true";
						smoothAcceleration = "true";
						pathFilter = "WolfOnHunt";
					};
					class GrazeMovement
					{
						maxSpeed = 0.25;
						minSpeed = 0.25;
						acceleration = 5;
						maxAngleSpeed = 90;
						slowRadius = 0;
						stopRadius = 0.5;
					};
				};
			};
			class SlotFireplace
			{
				class BehaviourCalm
				{
					grazeOnSpotWeight = 20;
					grazeWalkingWeight = 20;
					restWeight = 20;
					travelWeight = 0;
					grazeOnSpotDurationMin = 50;
					grazeOnSpotDurationMax = 100;
					grazeWalkingDurationMin = 50;
					grazeWalkingDurationMax = 100;
					restingDurationMin = 50;
					restingDurationMax = 150;
					travelingDurationMin = 0;
					travelingDurationMax = 0;
					grazeWalkingSpeed = 0.2;
					travelingWalkingSpeed = 1.36;
					safetyIntervalMin = 30;
					safetyIntervalMax = 40;
					safetyDurationMin = 5;
					safetyDurationMax = 10;
					safetyLookAngleMin = 0.3;
					safetyLookAngleMax = 0.7;
					safetyLookAngleChangeInterval = 3;
					class GrazeMovement
					{
						maxSpeed = 0.25;
						minSpeed = 0.25;
						acceleration = 5;
						maxAngleSpeed = 90;
						slowRadius = 0;
						stopRadius = 0.5;
					};
					class TravelingMovement
					{
						maxSpeed = 2.85;
						minSpeed = 0.78;
						acceleration = 5;
						maxAngleSpeed = 90;
						slowRadius = 0;
						stopRadius = 0.5;
						slowToTurn = "true";
						smoothAcceleration = "true";
					};
				};
			};
			class SlotScared
			{
				class BehaviourFleeFromTargets
				{
					class SoundsEntering
					{
						class Sound1
						{
							sounds[] = {"SpiderGroan_SoundSet"};
						};
						class Sound2
						{
							sounds[] = {"SpiderGroan_SoundSet"};
						};
						class Sound3
						{
							sounds[] = {"SpiderGroan_SoundSet"};
						};
						probability = 1;
					};
					class SoundsDuring
					{
						class Sound1
						{
							sounds[] = {"SpiderGroan_SoundSet"};
						};
						class Sound2
						{
							sounds[] = {"SpiderGroan_SoundSet"};
						};
						class Sound3
						{
							sounds[] = {"SpiderGroan_SoundSet"};
						};
						probability = 0.3;
						RepeatTimeMin = 5;
						RepeatTimeMax = 25;
						RepeatEnabled = "true";
					};
					class RunMovement
					{
						maxSpeed = 12.175;
						optimalSpeed = 6.39;
						minSpeed = 1;
						acceleration = 2;
						maxAngleSpeed = 360;
						slowRadius = 4;
						stopRadius = 0;
						maxSpeedRange = 15;
						optimalSpeedRange = 10;
						pathFilter = "DeerOnRun";
					};
				};
			};
			class AlertSystem
			{
				visionToAlertMultiplier = 20;
				noiseToAlertMultiplier = 1.5;
				damageToAlertMultiplier = 200;
				class Calm
				{
					dropSpeed = 3;
					dropDelay = 2;
					maxAlertValue = 30;
				};
				class Alerted
				{
					dropSpeed = 11;
					dropDelay = 0;
					maxAlertValue = 100;
				};
				class AlertedExtra
				{
					dropSpeed = 25;
					dropDelay = 10;
					maxAlertValue = 500;
				};
			};
		};
		class NoiseSystemParams
		{
			rangeMin = 25;
			rangeMax = 100;
			rangeShotMin = 0;
			rangeShotMax = 50;
			class NoiseStrengthTeamMultipliers
			{
				BigGame = 0.4;
				Zombies = 0.6;
				Player = 1;
			};
		};
		class TargetSystemDZBase
		{
			class VisionTeamMultipliers
			{
				BigGame = 0.6;
				Zombies = 1;
				Player = 1;
			};
			visionManSizeStand = 1;
			visionManSizeCrouch = 0.8;
			visionManSizeProne = 0.6;
			visionAngularSpeedMin = 0.1;
			visionAngularSpeedMax = 0.5;
			visionAngularSpeedMaxMult = 1.5;
			visionRangeMin = 100;
			visionRangeMax = 200;
			visionFov = 1.8;
			visionPeripheralRangeMin = 1;
			visionPeripheralRangeMax = 16;
			visionPeripheralFov = 6.2;
			visionNightMinMult = 1;
			visionNightMaxMult = 0.75;
			visionRainMinMult = 1;
			visionRainMaxMult = 0.9;
			visionFogMinMult = 1;
			visionFogMaxMult = 0.7;
		};
	};
};
class CfgVehicles
{
	class Animal_CanisLupus;
	class MSFZ_Mutant_Spider_Base: Animal_CanisLupus
	{
		scope = 0;
		displayName = "$STR_Mutant_Spider";
		descriptionShort = "$STR_Mutant_Spider";
		hiddenSelections[] = {"Spider"};
		DamageSphereAmmos[] = {"MeleeSpider","MeleeSpider_Hight"};
		aiAgentTemplate = "Predators_Spider";
		simulation = "dayzanimal";
		injuryLevels[] = {1,0.5,0.2,0};
		class enfanimsys
		{
			meshObject = "dz\animals\canis_lupus\Data\canis_lupus_skeleton.xob";
			graphname = "dz\animals\animations\!graph_files\Wolf\Wolf_Graph.agr";
			defaultinstance = "dz\animals\animations\!graph_files\Wolf\Wolf_AnimInstance.asi";
			startnode = "AlignToTerrain_Rot";
			skeletonName = "canis_lupus_skeleton.xob";
		};
		class AnimEvents
		{
			class Steps
			{
				class Walk1
				{
					soundLookupTable = "PawMediumWalk_LookupTable";
					noise = "WolfStepNoise";
					effectSet[] = {"WolfStepEffect1","WolfStepEffect2"};
					id = 1;
				};
				class Walk2
				{
					soundLookupTable = "PawMediumWalk_LookupTable";
					noise = "WolfStepNoise";
					effectSet[] = {"WolfStepEffect1","WolfStepEffect2"};
					id = 2;
				};
				class Walk3
				{
					soundLookupTable = "PawMediumWalk_LookupTable";
					noise = "WolfStepNoise";
					effectSet[] = {"WolfStepEffect1","WolfStepEffect2"};
					id = 3;
				};
				class Walk4
				{
					soundLookupTable = "PawMediumWalk_LookupTable";
					noise = "WolfStepNoise";
					effectSet[] = {"WolfStepEffect1","WolfStepEffect2"};
					id = 4;
				};
				class Run1
				{
					soundLookupTable = "PawMediumRun_LookupTable";
					noise = "WolfStepNoise";
					effectSet[] = {"WolfStepEffect1","WolfStepEffect2"};
					id = 5;
				};
				class Run2
				{
					soundLookupTable = "PawMediumRun_LookupTable";
					noise = "WolfStepNoise";
					effectSet[] = {"WolfStepEffect1","WolfStepEffect2"};
					id = 6;
				};
				class Run3
				{
					soundLookupTable = "PawMediumRun_LookupTable";
					noise = "WolfStepNoise";
					effectSet[] = {"WolfStepEffect1","WolfStepEffect2"};
					id = 7;
				};
				class Run4
				{
					soundLookupTable = "PawMediumRun_LookupTable";
					noise = "WolfStepNoise";
					effectSet[] = {"WolfStepEffect1","WolfStepEffect2"};
					id = 8;
				};
				class Bodyfall
				{
					soundLookupTable = "PawMediumBodyfall_LookupTable";
					noise = "WolfStepNoise";
					effectSet[] = {"WolfStepEffect1","WolfStepEffect2"};
					id = 11;
				};
				class Settle
				{
					soundLookupTable = "PawMediumSettle_LookupTable";
					noise = "WolfStepNoise";
					effectSet[] = {"WolfStepEffect1","WolfStepEffect2"};
					id = 12;
				};
				class Rest2standA
				{
					soundLookupTable = "PawMediumRest2standA_LookupTable";
					noise = "WolfStepNoise";
					effectSet[] = {"WolfStepEffect1","WolfStepEffect2"};
					id = 13;
				};
				class Rest2standB
				{
					soundLookupTable = "PawMediumRest2standB_LookupTable";
					noise = "WolfStepNoise";
					effectSet[] = {"WolfStepEffect1","WolfStepEffect2"};
					id = 14;
				};
				class Stand2restA
				{
					soundLookupTable = "PawMediumStand2restA_LookupTable";
					noise = "WolfStepNoise";
					effectSet[] = {"WolfStepEffect1","WolfStepEffect2"};
					id = 15;
				};
				class Stand2restB
				{
					soundLookupTable = "PawMediumStand2restB_LookupTable";
					noise = "WolfStepNoise";
					effectSet[] = {"WolfStepEffect1","WolfStepEffect2"};
					id = 16;
				};
				class Stand2restC
				{
					soundLookupTable = "PawMediumStand2restC_LookupTable";
					noise = "WolfStepNoise";
					effectSet[] = {"WolfStepEffect1","WolfStepEffect2"};
					id = 17;
				};
				class Jump
				{
					soundLookupTable = "PawMediumJump_LookupTable";
					noise = "WolfStepNoise";
					effectSet[] = {"WolfStepEffect1","WolfStepEffect2"};
					id = 18;
				};
				class Impact
				{
					soundLookupTable = "PawMediumImpact_LookupTable";
					noise = "WolfStepNoise";
					effectSet[] = {"WolfStepEffect1","WolfStepEffect2"};
					id = 19;
				};
			};
			class Sounds
			{
				class WolfBark
				{
					soundSet = "SpiderAttack_SoundSet";
					noise = "WolfRoarNoise";
					id = 1;
				};
				class WolfBark_1
				{
					soundSet = "SpiderAttack_SoundSet";
					noise = "WolfRoarNoise";
					id = 61;
				};
				class WolfBark_2
				{
					soundSet = "SpiderAttack_SoundSet";
					noise = "WolfRoarNoise";
					id = 71;
				};
				class WolfBark2
				{
					soundSet = "SpiderAttack2_SoundSet";
					noise = "WolfRoarNoise";
					id = 2;
				};
				class WolfBark3
				{
					soundSet = "SpiderAttack2_SoundSet";
					noise = "WolfRoarNoise";
					id = 3;
				};
				class WolfBreath
				{
					soundSet = "WolfBreath_SoundSet";
					noise = "WolfRoarNoise";
					id = 4;
				};
				class WolfGroans
				{
					soundSet = "SpiderGroan_SoundSet";
					noise = "WolfRoarNoise";
					id = 5;
				};
				class WolfGrowl_A
				{
					soundSet = "SpiderGrowl_SoundSet";
					noise = "WolfRoarNoise";
					id = 6;
				};
				class WolfGrowl_B
				{
					soundSet = "SpiderGrowl_SoundSet";
					noise = "WolfRoarNoise";
					id = 7;
				};
				class WolfGrowl
				{
					soundSet = "SpiderGrowl_SoundSet";
					noise = "WolfRoarNoise";
					id = 8;
				};
				class WolfPant
				{
					soundSet = "SpiderIdle_SoundSet";
					noise = "WolfRoarNoise";
					id = 9;
				};
				class WolfPantShort
				{
					soundSet = "SpiderIdle_SoundSet";
					noise = "WolfRoarNoise";
					id = 10;
				};
				class WolfPantLong
				{
					soundSet = "SpiderIdle_SoundSet";
					noise = "WolfRoarNoise";
					id = 18;
				};
				class WolfSnarl
				{
					soundSet = "SpiderAttack2_SoundSet";
					noise = "WolfRoarNoise";
					id = 11;
				};
				class WolfSnarlShort
				{
					soundSet = "SpiderAttack_SoundSet";
					noise = "WolfRoarNoise";
					id = 12;
				};
				class WolfWhimper
				{
					soundSet = "SpiderDie_SoundSet";
					noise = "WolfRoarNoise";
					id = 13;
				};
				class WolfYelp
				{
					soundSet = "SpiderDie_SoundSet";
					noise = "WolfRoarNoise";
					id = 14;
				};
				class WolfYawn
				{
					soundSet = "SpiderDie_SoundSet";
					noise = "WolfRoarNoise";
					id = 15;
				};
				class WolfDeath
				{
					soundSet = "SpiderHurt_SoundSet";
					noise = "WolfRoarNoise";
					id = 20;
				};
				class WolfHowl
				{
					soundSet = "SpiderHowl_SoundSet";
					noise = "WolfRoarNoise";
					id = 16;
				};
				class WolfHowls
				{
					soundSet = "SpiderDistant_SoundSet";
					noise = "WolfRoarNoise";
					id = 17;
				};
			};
			class Damages
			{
				class Bite
				{
					damage = "SpiderBiteDamage";
					id = 1;
				};
				class BiteLLow
				{
					damage = "SpiderLowBiteLDamage";
					id = 2;
				};
				class BiteRLow
				{
					damage = "SpiderLowBiteRDamage";
					id = 3;
				};
			};
		};
		class CommandMoveSettings
		{
			useSpeedMapping = 1;
			movementSpeedMapping[] = {0,0.25,0.5,1.2,4.5,12.2};
		};
		class CommandLookAtSettings
		{
			lookAtFilterTimeout = 0.5;
			lookAtFilterSpeed = 1.57;
		};
		class Skinning
		{
			class ObtainedPelt
			{
				item = "MSFZ_Spider_Lapka";
				count = 1;
				quantityMinMaxCoef[] = {1,1};
			};
		};
	};
	class MSFZ_Mutant_Spider1: MSFZ_Mutant_Spider_Base
	{
		scope = 2;
		model = "MSFZ_Mifical_Pack\Spider\MSFZ_Spider.p3d";
		displayName = "$STR_Mutant_Spider1";
		descriptionShort = "$STR_Mutant_Spider1";
		hiddenSelectionsTextures[] = {"MSFZ_Mifical_Pack\Spider\data\MSFZ_Spider.paa"};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 500;
					healthLevels[] = {{1,{"MSFZ_Mifical_Pack\Spider\data\msfz_Spider.rvmat"}},{0.7,{"MSFZ_Mifical_Pack\Spider\data\msfz_Spider.rvmat"}},{0.5,{"MSFZ_Mifical_Pack\Spider\data\msfz_Spider_damage.rvmat"}},{0.3,{"MSFZ_Mifical_Pack\Spider\data\msfz_Spider_damage.rvmat"}},{0,{"MSFZ_Mifical_Pack\Spider\data\msfz_Spider_destruct.rvmat"}}};
				};
				class Blood
				{
					hitpoints = 500;
				};
				class Shock
				{
					hitpoints = 500;
				};
			};
			class DamageZones
			{
				class Zone_Head
				{
					componentNames[] = {"Zone_Head"};
					transferToZonesNames[] = {};
					transferToZonesCoefs[] = {};
					fatalInjuryCoef = 0.15;
					canBleed = 0;
					class Health
					{
						hitpoints = 500;
						transferToGlobalCoef = 1;
					};
					class Blood: Health
					{
						hitpoints = 0;
					};
					class Shock: Health
					{
						hitpoints = 0;
					};
				};
				class Zone_Neck: Zone_Head
				{
					componentNames[] = {"Zone_Neck"};
					transferToZonesNames[] = {};
					transferToZonesCoefs[] = {};
					fatalInjuryCoef = 0.05;
					class Health: Health
					{
						hitpoints = 500;
					};
				};
				class Zone_Chest: Zone_Head
				{
					componentNames[] = {"Zone_Chest"};
					transferToZonesNames[] = {};
					transferToZonesCoefs[] = {};
					fatalInjuryCoef = 0.05;
					class Health: Health
					{
						hitpoints = 500;
					};
				};
				class Zone_Belly: Zone_Head
				{
					componentNames[] = {"Zone_Belly"};
					transferToZonesNames[] = {};
					transferToZonesCoefs[] = {};
					fatalInjuryCoef = 0.05;
					class Health: Health
					{
						hitpoints = 500;
					};
				};
				class Zone_Spine: Zone_Head
				{
					componentNames[] = {"Zone_Spine_Front","Zone_Spine_Back"};
					transferToZonesNames[] = {};
					transferToZonesCoefs[] = {};
					fatalInjuryCoef = 0.05;
					class Health: Health
					{
						hitpoints = 500;
					};
				};
				class Zone_Pelvis: Zone_Head
				{
					componentNames[] = {"Zone_Pelvis"};
					transferToZonesNames[] = {"Zone_Spine"};
					transferToZonesCoefs[] = {0.5};
					fatalInjuryCoef = 0.05;
					class Health: Health
					{
						hitpoints = 500;
					};
				};
				class Zone_Legs: Zone_Head
				{
					componentNames[] = {"Zone_Legs_Front","Zone_Legs_Back"};
					transferToZonesNames[] = {};
					transferToZonesCoefs[] = {};
					fatalInjuryCoef = 0;
					class Health: Health
					{
						hitpoints = 500;
					};
				};
			};
		};
		class Skinning
		{
			class ObtainedPelt
			{
				item = "MSFZ_Spider_Lapka";
				count = 1;
				quantityMinMaxCoef[] = {1,1};
			};
		};
	};
	class MSFZ_Mutant_Spider2: MSFZ_Mutant_Spider_Base
	{
		scope = 2;
		model = "MSFZ_Mifical_Pack\Spider\MSFZ_Spider.p3d";
		displayName = "$STR_Mutant_Spider2";
		descriptionShort = "$STR_Mutant_Spider2";
		hiddenSelectionsTextures[] = {"MSFZ_Mifical_Pack\Spider\data\MSFZ_Spider.paa"};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 750;
					healthLevels[] = {{1,{"MSFZ_Mifical_Pack\Spider\data\msfz_Spider.rvmat"}},{0.7,{"MSFZ_Mifical_Pack\Spider\data\msfz_Spider.rvmat"}},{0.5,{"MSFZ_Mifical_Pack\Spider\data\msfz_Spider_damage.rvmat"}},{0.3,{"MSFZ_Mifical_Pack\Spider\data\msfz_Spider_damage.rvmat"}},{0,{"MSFZ_Mifical_Pack\Spider\data\msfz_Spider_destruct.rvmat"}}};
				};
				class Blood
				{
					hitpoints = 750;
				};
				class Shock
				{
					hitpoints = 750;
				};
			};
			class DamageZones
			{
				class Zone_Head
				{
					componentNames[] = {"Zone_Head"};
					transferToZonesNames[] = {};
					transferToZonesCoefs[] = {};
					fatalInjuryCoef = 0.15;
					canBleed = 0;
					class Health
					{
						hitpoints = 750;
						transferToGlobalCoef = 1;
					};
					class Blood: Health
					{
						hitpoints = 0;
					};
					class Shock: Health
					{
						hitpoints = 0;
					};
				};
				class Zone_Neck: Zone_Head
				{
					componentNames[] = {"Zone_Neck"};
					transferToZonesNames[] = {};
					transferToZonesCoefs[] = {};
					fatalInjuryCoef = 0.05;
					class Health: Health
					{
						hitpoints = 750;
					};
				};
				class Zone_Chest: Zone_Head
				{
					componentNames[] = {"Zone_Chest"};
					transferToZonesNames[] = {};
					transferToZonesCoefs[] = {};
					fatalInjuryCoef = 0.05;
					class Health: Health
					{
						hitpoints = 750;
					};
				};
				class Zone_Belly: Zone_Head
				{
					componentNames[] = {"Zone_Belly"};
					transferToZonesNames[] = {};
					transferToZonesCoefs[] = {};
					fatalInjuryCoef = 0.05;
					class Health: Health
					{
						hitpoints = 750;
					};
				};
				class Zone_Spine: Zone_Head
				{
					componentNames[] = {"Zone_Spine_Front","Zone_Spine_Back"};
					transferToZonesNames[] = {};
					transferToZonesCoefs[] = {};
					fatalInjuryCoef = 0.05;
					class Health: Health
					{
						hitpoints = 750;
					};
				};
				class Zone_Pelvis: Zone_Head
				{
					componentNames[] = {"Zone_Pelvis"};
					transferToZonesNames[] = {"Zone_Spine"};
					transferToZonesCoefs[] = {0.5};
					fatalInjuryCoef = 0.05;
					class Health: Health
					{
						hitpoints = 750;
					};
				};
				class Zone_Legs: Zone_Head
				{
					componentNames[] = {"Zone_Legs_Front","Zone_Legs_Back"};
					transferToZonesNames[] = {};
					transferToZonesCoefs[] = {};
					fatalInjuryCoef = 0;
					class Health: Health
					{
						hitpoints = 750;
					};
				};
			};
		};
		class Skinning
		{
			class ObtainedPelt
			{
				item = "MSFZ_Spider_Lapka";
				count = 3;
				quantityMinMaxCoef[] = {1,1};
			};
		};
	};
	class MSFZ_Mutant_Spider3: MSFZ_Mutant_Spider_Base
	{
		scope = 2;
		model = "MSFZ_Mifical_Pack\Spider\MSFZ_Spider.p3d";
		displayName = "$STR_Mutant_Spider3";
		descriptionShort = "$STR_Mutant_Spider3";
		hiddenSelectionsTextures[] = {"MSFZ_Mifical_Pack\Spider\data\MSFZ_Spider.paa"};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 1000;
					healthLevels[] = {{1,{"MSFZ_Mifical_Pack\Spider\data\msfz_Spider.rvmat"}},{0.7,{"MSFZ_Mifical_Pack\Spider\data\msfz_Spider.rvmat"}},{0.5,{"MSFZ_Mifical_Pack\Spider\data\msfz_Spider_damage.rvmat"}},{0.3,{"MSFZ_Mifical_Pack\Spider\data\msfz_Spider_damage.rvmat"}},{0,{"MSFZ_Mifical_Pack\Spider\data\msfz_Spider_destruct.rvmat"}}};
				};
				class Blood
				{
					hitpoints = 1000;
				};
				class Shock
				{
					hitpoints = 1000;
				};
			};
			class DamageZones
			{
				class Zone_Head
				{
					componentNames[] = {"Zone_Head"};
					transferToZonesNames[] = {};
					transferToZonesCoefs[] = {};
					fatalInjuryCoef = 0.15;
					canBleed = 0;
					class Health
					{
						hitpoints = 1000;
						transferToGlobalCoef = 1;
					};
					class Blood: Health
					{
						hitpoints = 0;
					};
					class Shock: Health
					{
						hitpoints = 0;
					};
				};
				class Zone_Neck: Zone_Head
				{
					componentNames[] = {"Zone_Neck"};
					transferToZonesNames[] = {};
					transferToZonesCoefs[] = {};
					fatalInjuryCoef = 0.05;
					class Health: Health
					{
						hitpoints = 1000;
					};
				};
				class Zone_Chest: Zone_Head
				{
					componentNames[] = {"Zone_Chest"};
					transferToZonesNames[] = {};
					transferToZonesCoefs[] = {};
					fatalInjuryCoef = 0.05;
					class Health: Health
					{
						hitpoints = 1000;
					};
				};
				class Zone_Belly: Zone_Head
				{
					componentNames[] = {"Zone_Belly"};
					transferToZonesNames[] = {};
					transferToZonesCoefs[] = {};
					fatalInjuryCoef = 0.05;
					class Health: Health
					{
						hitpoints = 1000;
					};
				};
				class Zone_Spine: Zone_Head
				{
					componentNames[] = {"Zone_Spine_Front","Zone_Spine_Back"};
					transferToZonesNames[] = {};
					transferToZonesCoefs[] = {};
					fatalInjuryCoef = 0.05;
					class Health: Health
					{
						hitpoints = 1000;
					};
				};
				class Zone_Pelvis: Zone_Head
				{
					componentNames[] = {"Zone_Pelvis"};
					transferToZonesNames[] = {"Zone_Spine"};
					transferToZonesCoefs[] = {0.5};
					fatalInjuryCoef = 0.05;
					class Health: Health
					{
						hitpoints = 1000;
					};
				};
				class Zone_Legs: Zone_Head
				{
					componentNames[] = {"Zone_Legs_Front","Zone_Legs_Back"};
					transferToZonesNames[] = {};
					transferToZonesCoefs[] = {};
					fatalInjuryCoef = 0;
					class Health: Health
					{
						hitpoints = 1000;
					};
				};
			};
		};
		class Skinning
		{
			class ObtainedPelt
			{
				item = "MSFZ_Spider_Lapka";
				count = 2;
				quantityMinMaxCoef[] = {1,1};
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
	class SpiderAttack_SoundShader
	{
		samples[] = {{"MSFZ_Mifical_Pack\Spider\sound\Spider_attack\Spider_attack_0",1},{"MSFZ_Mifical_Pack\Spider\sound\Spider_attack\Spider_attack_1",1},{"MSFZ_Mifical_Pack\Spider\sound\Spider_attack\Spider_attack_2",1}};
		volume = 1.78;
		range = 80;
		rangeCurve = "defaultAnimalAttenuationCurve";
	};
	class SpiderAttack2_SoundShader
	{
		samples[] = {{"MSFZ_Mifical_Pack\Spider\sound\Spider_attack2\Spider_attack2_0",1},{"MSFZ_Mifical_Pack\Spider\sound\Spider_attack2\Spider_attack2_1",1},{"MSFZ_Mifical_Pack\Spider\sound\Spider_attack2\Spider_attack2_2",1}};
		volume = 1.78;
		range = 80;
		rangeCurve = "defaultAnimalAttenuationCurve";
	};
	class SpiderDie_SoundShader
	{
		samples[] = {{"MSFZ_Mifical_Pack\Spider\sound\Spider_die\Spider_dle_0",1},{"MSFZ_Mifical_Pack\Spider\sound\Spider_die\Spider_dle_1",1},{"MSFZ_Mifical_Pack\Spider\sound\Spider_die\Spider_dle_2",1},{"MSFZ_Mifical_Pack\Spider\sound\Spider_die\Spider_dle_3",1}};
		volume = 1.78;
		range = 80;
		rangeCurve = "defaultAnimalAttenuationCurve";
	};
	class SpiderEat_SoundShader
	{
		samples[] = {{"MSFZ_Mifical_Pack\Spider\sound\Spider_eat\Spider_eat_0",1},{"MSFZ_Mifical_Pack\Spider\sound\Spider_eat\Spider_eat_1",1},{"MSFZ_Mifical_Pack\Spider\sound\Spider_eat\Spider_eat_2",1},{"MSFZ_Mifical_Pack\Spider\sound\Spider_eat\Spider_eat_3",1},{"MSFZ_Mifical_Pack\Spider\sound\Spider_eat\Spider_eat_4",1},{"MSFZ_Mifical_Pack\Spider\sound\Spider_eat\Spider_eat_5",1}};
		volume = 1.78;
		range = 80;
		rangeCurve = "defaultAnimalAttenuationCurve";
	};
	class SpiderEmpty_SoundShader
	{
		samples[] = {{"MSFZ_Mifical_Pack\Spider\sound\Spider_empty\Spider_empty_0",1}};
		volume = 1.78;
		range = 80;
		rangeCurve = "defaultAnimalAttenuationCurve";
	};
	class SpiderGroan_SoundShader
	{
		samples[] = {{"MSFZ_Mifical_Pack\Spider\sound\Spider_groan\Spider_groan_0",1},{"MSFZ_Mifical_Pack\Spider\sound\Spider_groan\Spider_groan_1",1},{"MSFZ_Mifical_Pack\Spider\sound\Spider_groan\Spider_groan_2",1}};
		volume = 1.78;
		range = 80;
		rangeCurve = "defaultAnimalAttenuationCurve";
	};
	class SpiderGrowl_SoundShader
	{
		samples[] = {{"MSFZ_Mifical_Pack\Spider\sound\Spider_growl\Spider_growl_0",1},{"MSFZ_Mifical_Pack\Spider\sound\Spider_growl\Spider_growl_1",1},{"MSFZ_Mifical_Pack\Spider\sound\Spider_growl\Spider_growl_2",1}};
		volume = 1.78;
		range = 80;
		rangeCurve = "defaultAnimalAttenuationCurve";
	};
	class SpiderHurt_SoundShader
	{
		samples[] = {{"MSFZ_Mifical_Pack\Spider\sound\Spider_hurt\Spider_hurt_0",1},{"MSFZ_Mifical_Pack\Spider\sound\Spider_hurt\Spider_hurt_1",1},{"MSFZ_Mifical_Pack\Spider\sound\Spider_hurt\Spider_hurt_2",1},{"MSFZ_Mifical_Pack\Spider\sound\Spider_hurt\Spider_hurt_3",1}};
		volume = 1.78;
		range = 80;
		rangeCurve = "defaultAnimalAttenuationCurve";
	};
	class SpiderIdle_SoundShader
	{
		samples[] = {{"MSFZ_Mifical_Pack\Spider\sound\Spider_idle\Spider_idle_0",1},{"MSFZ_Mifical_Pack\Spider\sound\Spider_idle\Spider_idle_1",1},{"MSFZ_Mifical_Pack\Spider\sound\Spider_idle\Spider_idle_2",1},{"MSFZ_Mifical_Pack\Spider\sound\Spider_idle\Spider_idle_3",1},{"MSFZ_Mifical_Pack\Spider\sound\Spider_idle\Spider_idle_4",1}};
		volume = 1.78;
		range = 80;
		rangeCurve = "defaultAnimalAttenuationCurve";
	};
	class SpiderPanic_SoundShader
	{
		samples[] = {{"MSFZ_Mifical_Pack\Spider\sound\Spider_panic\Spider_panic_0",1},{"MSFZ_Mifical_Pack\Spider\sound\Spider_panic\Spider_panic_1",1},{"MSFZ_Mifical_Pack\Spider\sound\Spider_panic\Spider_panic_2",1}};
		volume = 1.78;
		range = 80;
		rangeCurve = "defaultAnimalAttenuationCurve";
	};
	class SpiderHowl_SoundShader
	{
		samples[] = {{"MSFZ_Mifical_Pack\Spider\sound\Spider_howl\Spider_howl_0",1}};
		volume = 5.0118723;
		range = 900;
		rangeCurve = "defaultAnimalAttenuationCurve";
	};
	class SpiderDistant_SoundShader
	{
		samples[] = {{"MSFZ_Mifical_Pack\Spider\sound\Spider_distant\Spider_distant_0",1}};
		volume = 5.0118723;
		range = 900;
		rangeCurve = "defaultAnimalAttenuationCurve";
	};
};
class CfgSoundSets
{
	class SpiderAttack_SoundSet
	{
		soundShaders[] = {"SpiderAttack_SoundShader"};
	};
	class SpiderAttack2_SoundSet
	{
		soundShaders[] = {"SpiderAttack2_SoundShader"};
	};
	class SpiderDie_SoundSet
	{
		soundShaders[] = {"SpiderDie_SoundShader"};
	};
	class SpiderEat_SoundSet
	{
		soundShaders[] = {"SpiderEat_SoundShader"};
	};
	class SpiderEmpty_SoundSet
	{
		soundShaders[] = {"SpiderEmpty_SoundShader"};
	};
	class SpiderGroan_SoundSet
	{
		soundShaders[] = {"SpiderGroan_SoundShader"};
	};
	class SpiderGrowl_SoundSet
	{
		soundShaders[] = {"SpiderGrowl_SoundShader"};
	};
	class SpiderHurt_SoundSet
	{
		soundShaders[] = {"SpiderHurt_SoundShader"};
	};
	class SpiderIdle_SoundSet
	{
		soundShaders[] = {"SpiderIdle_SoundShader"};
	};
	class SpiderHowl_SoundSet
	{
		soundShaders[] = {"SpiderHowl_SoundShader"};
	};
	class SpiderDistant_SoundSet
	{
		soundShaders[] = {"SpiderDistant_SoundShader"};
	};
};
