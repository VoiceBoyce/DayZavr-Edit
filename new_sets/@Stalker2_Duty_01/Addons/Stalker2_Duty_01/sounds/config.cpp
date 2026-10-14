class CfgPatches
{
	class S2_Duty_01_Exo_Sounds
	{
		units[]={};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Data",
			"DZ_Sounds_Effects"
		};
	};
};
class CfgSoundSets
{
	class ExoStep_SoundSet
	{
		soundShaders[]=
		{
			"ExoStep_SoundShader"
		};
		volumeFactor=1;
		frequencyRandomizer=0.050000001;
		volumeCurve="InverseSquare2Curve";
		spatial=1;
		doppler=0;
		looping=0;
		sound3DProcessingType="default3D";
	};
};
class CfgSoundShaders
{
	class ExoStep_SoundShader
	{
		samples[]=
		{
			
			{
				"Stalker2_Duty_01\sounds\heavy_servo_exo_walk1.ogg",
				1
			},
			
			{
				"Stalker2_Duty_01\sounds\heavy_servo_exo_walk2.ogg",
				1
			},
			
			{
				"Stalker2_Duty_01\sounds\light_servo_exo_walk1.ogg",
				1
			},
			
			{
				"Stalker2_Duty_01\sounds\light_servo_exo_walk2.ogg",
				1
			},
			
			{
				"Stalker2_Duty_01\sounds\middle_servo_exo_walk1.ogg",
				1
			},
			
			{
				"Stalker2_Duty_01\sounds\middle_servo_exo_walk2.ogg",
				1
			},
			
			{
				"Stalker2_Duty_01\sounds\nosorog_servo_exo_walk1.ogg",
				1
			},
			
			{
				"Stalker2_Duty_01\sounds\nosorog_servo_exo_walk2.ogg",
				1
			},
			
			{
				"Stalker2_Duty_01\sounds\servo_exo_step.ogg",
				1
			}
		};
		volume=1;
		range=12;
		rangeCurve="close2far3";
		limitation=1;
	};
};
