class CfgPatches
{
	class AI_Bandits_AI
	{
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data","DZ_data_aiconfigs","DZ_AI"};
	};
};
class AIParams
{
	class AgentTeams
	{
		TeamList[] += {"Bandits"};
		class Bandits
		{
			friends[] = {"AmbientLife"};
		};
	};
};
class GroupBehaviourTemplates
{
	class DZBanditsGroupBeh
	{
		type = "InfectedPack";
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
		endAttractionRange = 300;
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
		enragedRingDistanceMax = 10;
		enragedRingDistanceMin = 3;
		enragedReachDistanceMin = 0.8;
		enragedReachDistanceMax = 3;
		enragedOnReachCount = 3;
		maxEnrageTime = 10;
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
class PathGraphFilters
{
	class BanditOnHunt
	{
		class Flags
		{
			include[] = {"walk","door","inside","jump","climb","jumpover"};
			exclude[] = {"disabled","crawl","crouch","swim","swimsea"};
		};
		class Costs
		{
			jump0 = 3;
			jump1 = 0;
			jump2 = 0;
			jump3 = 0;
			jump4 = 0;
			water = 5;
		};
	};
};
class CfgAIBehaviours
{
	class Bandit
	{
		name = "bandits";
		HeadLookBoneName = "face_tongue";
		teamName = "Bandits";
		defaultGroupTemplateName = "DZBanditsGroupBeh";
		class BehaviourHLInfectedPack
		{
			instantAlertRangeMin = 10;
			instantAlertRangeMax = 200;
			instantAlertStrength = 50;
			proximityAttackRange = 2.5;
			proximityAttackPredictTime = 0.5;
			attackCooldown = 2.5;
			attractionChangeTargetCooldown = 5;
			changeAttractionTargetRatio = 1.5;
			attractionNoiseStrengthTreshold = 3;
			attractionStrengthCeiling = 100;
			attractionStrengthDropSpeed = 5;
			attractionNoiseWindowStrengthTreshold = 20;
			attractionNoiseWindowSize = 5;
			attractionVisionPostponeTime = 3.5;
			class SlotCalm
			{
				class BehaviourCalm
				{
					travelingMode = "true";
					grazeOnSpotWeight = 0;
					grazeWalkingWeight = 0;
					restWeight = 0;
					travelWeight = 1;
					grazeOnSpotDurationMin = 5;
					grazeOnSpotDurationMax = 10;
					grazeWalkingDurationMin = 50;
					grazeWalkingDurationMax = 50;
					restingDurationMin = 0;
					restingDurationMax = 0;
					travelingDurationMin = 150;
					travelingDurationMax = 300;
					grazeWalkingSpeed = 1.2;
					travelingWalkingSpeed = 3.36;
					safetyDurationMin = 10;
					safetyDurationMax = 10;
					safetyLookAngleMin = 0.3;
					safetyLookAngleMax = 0.7;
					safetyLookAngleChangeInterval = 3;
					class GrazeMovement
					{
						maxSpeed = 4.25;
						minSpeed = 1;
						acceleration = 5;
						maxAngleSpeed = 180;
						slowRadius = 2;
						stopRadius = 1;
						pathFilter = "BanditOnHunt";
					};
					class TravelingMovement
					{
						maxSpeed = 2.1499999;
						minSpeed = 1.78;
						acceleration = 5;
						maxAngleSpeed = 180;
						slowRadius = 2;
						stopRadius = 1;
						pathFilter = "BanditOnHunt";
						slowToTurn = "true";
						smoothAcceleration = "true";
					};
					class CatchUpMovement
					{
						maxSpeed = 6.38;
						minSpeed = 1.78;
						acceleration = 5;
						maxAngleSpeed = 120;
						slowRadius = 2;
						stopRadius = 1;
						pathFilter = "BanditOnHunt";
						slowToTurn = "true";
						smoothAcceleration = "true";
					};
				};
			};
			class SlotPreAttracted
			{
				class BehaviourInfectedPackInterested
				{
					class SoundsEntering
					{
						class Sound1{};
					};
					class SoundsDuring
					{
						class Sound1{};
						probability = 0.9;
						RepeatTimeMin = 2;
						RepeatTimeMax = 4;
						RepeatEnabled = 1;
					};
					introStateDurationMin = "8.0f";
					introStateDurationMax = "13.0f";
					lookAtStateDurationMin = "8.0f";
					lookAtStateDurationMax = "13.0f";
					lookAtStateNoiseWindowStrengthTreshold = "11.0f";
					lookAtStateNoiseWindowSize = "5.0f";
				};
			};
			class SlotAttracted
			{
				class BehaviourInfectedPackSearching
				{
					class SoundsEntering
					{
						class Sound1{};
					};
					class SoundsDuring
					{
						class Sound1{};
						probability = 0.9;
						RepeatTimeMin = 2;
						RepeatTimeMax = 4;
						RepeatEnabled = 1;
					};
					attractionFollowupDistance = 120;
					noiseStrengthTreshold = 2.5;
					searchStateDurationMax = 20;
					cooldownStateDurationMin = 5;
					cooldownStateDurationMax = 15;
					class Movement
					{
						maxSpeed = 3.2;
						minSpeed = 2.2;
						acceleration = 0.5;
						maxAngleSpeed = 180;
						slowRadius = 4;
						stopRadius = 1;
						slowToTurn = 1;
						smoothAcceleration = 1;
						pathFilter = "BanditOnHunt";
					};
				};
			};
			class SlotEnraged
			{
				class BehaviourInfectedPackEnraged
				{
					class SoundsEntering
					{
						class Sound1{};
					};
					class SoundsDuring
					{
						class Sound1{};
						probability = 0.9;
						RepeatTimeMin = 3;
						RepeatTimeMax = 6;
						RepeatEnabled = 1;
					};
					class Movement
					{
						maxSpeed = 4.4;
						minSpeed = 1.6;
						acceleration = 8;
						maxAngleSpeed = 180;
						slowRadius = 1.5;
						stopRadius = 0.8;
						slowToTurn = 1;
						smoothAcceleration = 1;
						pathFilter = "BanditOnHunt";
					};
					class MovementApproaching
					{
						maxSpeed = 4.4;
						maxSpeedRange = 2.5;
						optimalSpeed = 1.6;
						minSpeed = 0.4;
						acceleration = 1.5;
						maxAngleSpeed = 120;
						slowRadius = 1;
						stopRadius = 0.2;
						slowToTurn = 1;
						smoothAcceleration = 1;
						pathFilter = "BanditOnHunt";
					};
					class MovementReach
					{
						maxSpeed = 2.4;
						minSpeed = 1.1;
						acceleration = 2;
						maxAngleSpeed = 180;
						slowRadius = 1.6;
						stopRadius = 0.35;
						slowToTurn = 1;
						smoothAcceleration = 1;
						pathFilter = "BanditOnHunt";
					};
				};
			};
			class SlotEnragedTargetLost
			{
				class BehaviourInfectedPackSearching
				{
					class SoundsDuring
					{
						class Sound1{};
						probability = 0.9;
						RepeatTimeMin = 3;
						RepeatTimeMax = 6;
						RepeatEnabled = 1;
					};
					class SoundsExiting
					{
						class Sound1{};
					};
					attractionFollowupDistance = 80;
					noiseStrengthTreshold = 4;
					searchStateDurationMax = 8;
					cooldownStateDurationMin = 4;
					cooldownStateDurationMax = 8;
					class Movement
					{
						maxSpeed = 2.2;
						minSpeed = 1.2;
						acceleration = 2.5;
						maxAngleSpeed = 180;
						slowRadius = 1;
						stopRadius = 0.2;
						slowToTurn = 1;
						smoothAcceleration = 1;
						pathFilter = "BanditOnHunt";
					};
					class MovementAlerted
					{
						maxSpeed = 3.4;
						minSpeed = 1.2;
						acceleration = 4;
						maxAngleSpeed = 180;
						slowRadius = 1;
						stopRadius = 0.2;
						slowToTurn = 1;
						smoothAcceleration = 1;
						pathFilter = "BanditOnHunt";
					};
				};
			};
			class AlertSystem
			{
				visionToAlertMultiplier = 15;
				noiseToAlertMultiplier = 8;
				damageToAlertMultiplier = 10000;
				noiseShotToAlertMultiplier = 3.5;
				class Calm
				{
					DropSpeed = 1;
					DropDelay = 0;
					MaxAlertValue = 20;
				};
				class Alerted
				{
					DropSpeed = 3;
					DropDelay = 20;
					MaxAlertValue = 100;
				};
				class AlertedExtra
				{
					dropSpeed = 4;
					dropDelay = 60;
					maxAlertValue = 200;
				};
			};
		};
		class NoiseSystemParams
		{
			rangeMin = 30;
			rangeMax = 150;
			rangeShotMin = 10;
			rangeShotMax = 350;
			radiusMin = 50;
			radiusMax = 125;
			radiusShotMin = 15;
			radiusShotMax = 270;
			shotVolExponent = 1;
			class NoiseStrengthTeamMultipliers
			{
				BigGame = 0.8;
				Zombies = 1;
				Player = 5;
			};
		};
		class TargetSystemDZBase
		{
			class VisionTeamMultipliers
			{
				BigGame = 0.8;
				Zombies = 1;
				Player = 5;
			};
			visionManSizeStand = 1;
			visionManSizeCrouch = 0.5;
			visionManSizeProne = 0.3;
			visionAngularSpeedMin = 0.1;
			visionAngularSpeedMax = 0.5;
			visionAngularSpeedMaxMult = 1.5;
			visionRangeMin = 20;
			visionRangeMax = 300;
			visionFov = 1.9;
			visionPeripheralRangeMin = 1;
			visionPeripheralRangeMax = 3;
			visionPeripheralFov = 3.15;
			visionNightMinMult = 1;
			visionNightMaxMult = 0.75;
			visionRainMinMult = 1;
			visionRainMaxMult = 0.9;
			visionFogMinMult = 1;
			visionFogMaxMult = 0.7;
			visionProximityRange = 2.5;
			visionProximityStrengthMult = 2;
			visionCloseRange = 10;
			visionCloseHeight = 1.8;
			visionCloseStrengthMult = 1.5;
		};
	};
};
