class CfgPatches
{
	class MSFZ_Weapon_Crossbow_Sound
	{
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data"};
	};
};
class CfgSoundShaders
{
	class baseCharacter_SoundShader;
	class crossbow_SoundShaderClose
	{
		samples[] = {{"MSFZ_Mifical_Pack\Weapons\Crossbow\sounds\crossbow_shoot",1}};
		volume = 1;
		range = 10;
		rangeCurve = "closeShotCurve";
	};
	class crossbow_SoundShaderMid
	{
		samples[] = {{"MSFZ_Mifical_Pack\Weapons\Crossbow\sounds\crossbow_shoot",1}};
		volume = 0.56234133;
		range = 5;
		rangeCurve[] = {{0,0.2},{50,1},{75,0},{400,0}};
	};
	class crossbow_SoundShaderLong
	{
		samples[] = {{"MSFZ_Mifical_Pack\Weapons\Crossbow\sounds\crossbow_shoot",1}};
		volume = 0;
		range = 2;
		rangeCurve[] = {{0,0},{50,0},{75,1},{400,1}};
	};
	class crossbow_Silenced_SoundShaderClose
	{
		samples[] = {{"MSFZ_Mifical_Pack\Weapons\Crossbow\sounds\crossbow_suppressed",1}};
		volume = 1.3;
		range = 40;
		rangeCurve = "closeShotCurve";
	};
	class crossbow_Silenced_SoundShaderMid
	{
		samples[] = {{"MSFZ_Mifical_Pack\Weapons\Crossbow\sounds\crossbow_suppressed",1}};
		volume = 0.56234133;
		range = 100;
		rangeCurve[] = {{0,0.2},{50,1},{75,0},{400,0}};
	};
	class crossbow_Silenced_SoundShaderLong
	{
		samples[] = {{"MSFZ_Mifical_Pack\Weapons\Crossbow\sounds\crossbow_suppressed",1}};
		volume = 0.5;
		range = 200;
		rangeCurve[] = {{0,0},{50,0},{75,1},{400,1}};
	};
};
class CfgSoundSets
{
	class Rifle_Shot_Base_SoundSet;
	class crossbow_SoundSet: Rifle_Shot_Base_SoundSet
	{
		soundShaders[] = {"crossbow_SoundShaderClose","crossbow_SoundShaderMid","crossbow_SoundShaderLong"};
	};
	class crossbow_Silenced_SoundSet: Rifle_Shot_Base_SoundSet
	{
		soundShaders[] = {"crossbow_Silenced_SoundShaderClose","crossbow_Silenced_SoundShaderMid","crossbow_Silenced_SoundShaderLong"};
	};
};
