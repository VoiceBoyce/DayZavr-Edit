// INT GlobalChat — client + shared part. Loaded with -mod on the server and on clients (spec.md §3.1).
// Script module paths use forward slashes: the 1.30 Windows server ignores backslash paths.

class CfgPatches
{
	class INT_GlobalChat_CL
	{
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Scripts", "DZ_Sounds_Effects", "INT_UIFramework"};
	};
};

class CfgMods
{
	class INT_GlobalChat_CL
	{
		type = "mod";
		dir = "INT_GlobalChat_CL";
		name = "INT GlobalChat";
		author = "Intenso";
		version = "1.0.0";
		inputs = "INT_GlobalChat_CL/data/inputs.xml";
		dependencies[] = {"Game", "Mission"};
		defines[] = {"INT_GLOBALCHAT_CL"};
		class defs
		{
			class gameScriptModule
			{
				value = "";
				files[] = {"INT_GlobalChat_CL/scripts/3_Game"};
			};
			class missionScriptModule
			{
				value = "";
				files[] = {"INT_GlobalChat_CL/scripts/5_Mission"};
			};
		};
	};
};

// UI cues (2D, mono 44.1 kHz Vorbis; tools/gen_gc_sfx.py). Played by INT_GC_Sfx on the UI channel.
class CfgSoundShaders
{
	class INT_GC_Ui_SoundShader
	{
		volume = 0.7;
		range = 50;
		limitation = 0;
	};
	class INT_GC_Message_SoundShader : INT_GC_Ui_SoundShader
	{
		samples[] = {{"INT_GlobalChat_CL\data\sound\int_gc_message", 1}};
		volume = 0.45;
	};
	class INT_GC_Mention_SoundShader : INT_GC_Ui_SoundShader
	{
		samples[] = {{"INT_GlobalChat_CL\data\sound\int_gc_mention", 1}};
		volume = 0.8;
	};
	class INT_GC_Send_SoundShader : INT_GC_Ui_SoundShader
	{
		samples[] = {{"INT_GlobalChat_CL\data\sound\int_gc_send", 1}};
		volume = 0.5;
	};
	class INT_GC_Error_SoundShader : INT_GC_Ui_SoundShader
	{
		samples[] = {{"INT_GlobalChat_CL\data\sound\int_gc_error", 1}};
		volume = 0.6;
	};
	class INT_GC_Click_SoundShader : INT_GC_Ui_SoundShader
	{
		samples[] = {{"INT_GlobalChat_CL\data\sound\int_gc_click", 1}};
		volume = 0.5;
	};
};

class CfgSoundSets
{
	class INT_GC_Ui_SoundSet
	{
		volumeFactor = 1.0;
		frequencyFactor = 1.0;
		spatial = 0;
		doppler = 0;
		loop = 0;
	};
	class INT_GC_Message_SoundSet : INT_GC_Ui_SoundSet
	{
		soundShaders[] = {"INT_GC_Message_SoundShader"};
	};
	class INT_GC_Mention_SoundSet : INT_GC_Ui_SoundSet
	{
		soundShaders[] = {"INT_GC_Mention_SoundShader"};
	};
	class INT_GC_Send_SoundSet : INT_GC_Ui_SoundSet
	{
		soundShaders[] = {"INT_GC_Send_SoundShader"};
	};
	class INT_GC_Error_SoundSet : INT_GC_Ui_SoundSet
	{
		soundShaders[] = {"INT_GC_Error_SoundShader"};
	};
	class INT_GC_Click_SoundSet : INT_GC_Ui_SoundSet
	{
		soundShaders[] = {"INT_GC_Click_SoundShader"};
	};
};
