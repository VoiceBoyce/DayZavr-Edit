class CfgPatches
{
	class MSFZ_MSFZ_Mifical_PackSound
	{
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data","DZ_Vehicles_Wheeled"};
	};
};
class CfgSoundShaders
{
	class baseAnimal_SoundSet;
	class Japan_light_SoundShader
	{
		samples[] = {{"MSFZ_Mifical_Pack\Sound\MSFZ_Japan_light.ogg",1}};
		volume = 0.4;
		range = 5;
	};
};
class CfgSoundSets
{
	class Japan_light_SoundSet
	{
		soundShaders[] = {"Japan_light_SoundShader"};
		sound3DProcessingType = "smallItem3DProcessingType";
		volumeCurve = "LinearCurve";
		volumeFactor = 0.5;
		spatial = 1;
		loop = 1;
	};
};
