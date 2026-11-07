// INT_PDA — client + shared part. Loaded with -mod on the server and on clients (spec.md §1).
// Script module paths use forward slashes: the 1.30 Windows server ignores backslash paths.
// Items, slots and models are in INT_PDA_Data (prefix INT_PDA_Data, shipped in the same @ folder).
// Sounds: tools/gen_pda_sfx.py (mono 44.1 kHz ogg); art: tools/gen_pda_art.py, tools/gen_pda_avatars.py.

class CfgPatches
{
	class INT_PDA_CL
	{
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Scripts", "DZ_Sounds_Effects", "INT_UIFramework", "INT_PDA_Data"};
	};
};

class CfgMods
{
	class INT_PDA_CL
	{
		type = "mod";
		dir = "INT_PDA_CL";
		name = "INT PDA";
		author = "Intenso";
		version = "1.0.0";
		inputs = "INT_PDA_CL/data/inputs.xml";
		dependencies[] = {"Game", "World", "Mission"};
		defines[] = {"INT_PDA_CL"};
		class defs
		{
			class imageSets
			{
				files[] = {"INT_PDA_CL/gui/imagesets/int_pda_slots.imageset", "INT_PDA_CL/gui/imagesets/int_pda_dev.imageset", "INT_PDA_CL/gui/imagesets/int_pda_icons.imageset", "INT_PDA_CL/gui/imagesets/int_pda_avatars.imageset", "INT_PDA_CL/gui/imagesets/int_pda_emblems.imageset", "INT_PDA_CL/gui/imagesets/int_pda_emblemsq.imageset", "INT_PDA_CL/gui/imagesets/int_pda_logo.imageset", "INT_PDA_CL/gui/imagesets/int_pda_nomap.imageset"};
			};
			class gameScriptModule
			{
				value = "";
				files[] = {"INT_PDA_CL/scripts/3_Game"};
			};
			class worldScriptModule
			{
				value = "";
				files[] = {"INT_PDA_CL/scripts/4_World"};
			};
			class missionScriptModule
			{
				value = "";
				files[] = {"INT_PDA_CL/scripts/5_Mission"};
			};
		};
	};
};

class CfgSoundShaders
{
	class INT_PDA_Ui_SoundShader
	{
		volume = 0.7;
		range = 50;
		limitation = 0;
	};
	class INT_PDA_Click_SoundShader : INT_PDA_Ui_SoundShader
	{
		samples[] = {{"INT_PDA_CL\sound\int_pda_click", 1}};
		volume = 0.8;
	};
	class INT_PDA_Tab_SoundShader : INT_PDA_Ui_SoundShader
	{
		samples[] = {{"INT_PDA_CL\sound\int_pda_tab", 1}};
		volume = 0.6;
	};
	class INT_PDA_Key1_SoundShader : INT_PDA_Ui_SoundShader
	{
		samples[] = {{"INT_PDA_CL\sound\int_pda_key1", 1}};
		volume = 0.6;
	};
	class INT_PDA_Key2_SoundShader : INT_PDA_Ui_SoundShader
	{
		samples[] = {{"INT_PDA_CL\sound\int_pda_key2", 1}};
		volume = 0.6;
	};
	class INT_PDA_Key3_SoundShader : INT_PDA_Ui_SoundShader
	{
		samples[] = {{"INT_PDA_CL\sound\int_pda_key3", 1}};
		volume = 0.6;
	};
	class INT_PDA_Toggle_SoundShader : INT_PDA_Ui_SoundShader
	{
		samples[] = {{"INT_PDA_CL\sound\int_pda_toggle", 1}};
		volume = 0.7;
	};
	class INT_PDA_Boot_SoundShader : INT_PDA_Ui_SoundShader
	{
		samples[] = {{"INT_PDA_CL\sound\int_pda_boot", 1}};
		volume = 0.7;
	};
	class INT_PDA_Off_SoundShader : INT_PDA_Ui_SoundShader
	{
		samples[] = {{"INT_PDA_CL\sound\int_pda_off", 1}};
		volume = 0.7;
	};
	class INT_PDA_LoginOk_SoundShader : INT_PDA_Ui_SoundShader
	{
		samples[] = {{"INT_PDA_CL\sound\int_pda_login_ok", 1}};
		volume = 0.7;
	};
	class INT_PDA_Error_SoundShader : INT_PDA_Ui_SoundShader
	{
		samples[] = {{"INT_PDA_CL\sound\int_pda_error", 1}};
		volume = 0.55;
	};
	class INT_PDA_Msg_SoundShader : INT_PDA_Ui_SoundShader
	{
		samples[] = {{"INT_PDA_CL\sound\int_pda_msg", 1}};
		volume = 0.6;
	};
	class INT_PDA_Pm_SoundShader : INT_PDA_Ui_SoundShader
	{
		samples[] = {{"INT_PDA_CL\sound\int_pda_pm", 1}};
		volume = 0.8;
	};
	class INT_PDA_News_SoundShader : INT_PDA_Ui_SoundShader
	{
		samples[] = {{"INT_PDA_CL\sound\int_pda_news", 1}};
		volume = 0.7;
	};
	class INT_PDA_Sos_SoundShader : INT_PDA_Ui_SoundShader
	{
		samples[] = {{"INT_PDA_CL\sound\int_pda_sos", 1}};
		volume = 0.6;
	};
	class INT_PDA_Invite_SoundShader : INT_PDA_Ui_SoundShader
	{
		samples[] = {{"INT_PDA_CL\sound\int_pda_invite", 1}};
		volume = 0.7;
	};
	class INT_PDA_LowBat_SoundShader : INT_PDA_Ui_SoundShader
	{
		samples[] = {{"INT_PDA_CL\sound\int_pda_lowbat", 1}};
		volume = 0.6;
	};
	class INT_PDA_MapPing_SoundShader : INT_PDA_Ui_SoundShader
	{
		samples[] = {{"INT_PDA_CL\sound\int_pda_map_ping", 1}};
		volume = 0.7;
	};
	class INT_PDA_Send_SoundShader : INT_PDA_Ui_SoundShader
	{
		samples[] = {{"INT_PDA_CL\sound\int_pda_send", 1}};
		volume = 0.6;
	};
	// World sound: attenuates to silence at 15 m
	class INT_PDA_BeepWorld_SoundShader
	{
		samples[] = {{"INT_PDA_CL\sound\int_pda_beep_world", 1}};
		volume = 1.0;
		range = 15;
		rangeCurve[] = {{0, 1.0}, {3, 0.85}, {8, 0.45}, {15, 0}};
		limitation = 0;
	};
};

class CfgSoundSets
{
	class INT_PDA_Ui_SoundSet
	{
		volumeFactor = 1.0;
		frequencyFactor = 1.0;
		spatial = 0;
		doppler = 0;
		loop = 0;
	};
	class INT_PDA_Click_SoundSet : INT_PDA_Ui_SoundSet
	{
		soundShaders[] = {"INT_PDA_Click_SoundShader"};
	};
	class INT_PDA_Tab_SoundSet : INT_PDA_Ui_SoundSet
	{
		soundShaders[] = {"INT_PDA_Tab_SoundShader"};
	};
	class INT_PDA_Key1_SoundSet : INT_PDA_Ui_SoundSet
	{
		soundShaders[] = {"INT_PDA_Key1_SoundShader"};
	};
	class INT_PDA_Key2_SoundSet : INT_PDA_Ui_SoundSet
	{
		soundShaders[] = {"INT_PDA_Key2_SoundShader"};
	};
	class INT_PDA_Key3_SoundSet : INT_PDA_Ui_SoundSet
	{
		soundShaders[] = {"INT_PDA_Key3_SoundShader"};
	};
	class INT_PDA_Toggle_SoundSet : INT_PDA_Ui_SoundSet
	{
		soundShaders[] = {"INT_PDA_Toggle_SoundShader"};
	};
	class INT_PDA_Boot_SoundSet : INT_PDA_Ui_SoundSet
	{
		soundShaders[] = {"INT_PDA_Boot_SoundShader"};
	};
	class INT_PDA_Off_SoundSet : INT_PDA_Ui_SoundSet
	{
		soundShaders[] = {"INT_PDA_Off_SoundShader"};
	};
	class INT_PDA_LoginOk_SoundSet : INT_PDA_Ui_SoundSet
	{
		soundShaders[] = {"INT_PDA_LoginOk_SoundShader"};
	};
	class INT_PDA_Error_SoundSet : INT_PDA_Ui_SoundSet
	{
		soundShaders[] = {"INT_PDA_Error_SoundShader"};
	};
	class INT_PDA_Msg_SoundSet : INT_PDA_Ui_SoundSet
	{
		soundShaders[] = {"INT_PDA_Msg_SoundShader"};
	};
	class INT_PDA_Pm_SoundSet : INT_PDA_Ui_SoundSet
	{
		soundShaders[] = {"INT_PDA_Pm_SoundShader"};
	};
	class INT_PDA_News_SoundSet : INT_PDA_Ui_SoundSet
	{
		soundShaders[] = {"INT_PDA_News_SoundShader"};
	};
	class INT_PDA_Sos_SoundSet : INT_PDA_Ui_SoundSet
	{
		soundShaders[] = {"INT_PDA_Sos_SoundShader"};
	};
	class INT_PDA_Invite_SoundSet : INT_PDA_Ui_SoundSet
	{
		soundShaders[] = {"INT_PDA_Invite_SoundShader"};
	};
	class INT_PDA_LowBat_SoundSet : INT_PDA_Ui_SoundSet
	{
		soundShaders[] = {"INT_PDA_LowBat_SoundShader"};
	};
	class INT_PDA_MapPing_SoundSet : INT_PDA_Ui_SoundSet
	{
		soundShaders[] = {"INT_PDA_MapPing_SoundShader"};
	};
	class INT_PDA_Send_SoundSet : INT_PDA_Ui_SoundSet
	{
		soundShaders[] = {"INT_PDA_Send_SoundShader"};
	};
	// Optional: all three keystroke variants in one set (engine picks one at random per play)
	class INT_PDA_Key_SoundSet : INT_PDA_Ui_SoundSet
	{
		soundShaders[] = {"INT_PDA_Key1_SoundShader", "INT_PDA_Key2_SoundShader", "INT_PDA_Key3_SoundShader"};
	};
	class INT_PDA_BeepWorld_SoundSet
	{
		soundShaders[] = {"INT_PDA_BeepWorld_SoundShader"};
		volumeFactor = 1.0;
		frequencyFactor = 1.0;
		spatial = 1;
		doppler = 0;
		loop = 0;
	};
};
