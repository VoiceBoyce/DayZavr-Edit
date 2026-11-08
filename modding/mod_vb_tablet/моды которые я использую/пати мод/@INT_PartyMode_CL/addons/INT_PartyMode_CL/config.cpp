// INT PartyMode — client + shared part. Loaded with -mod on the server and on clients (spec.md §3).
// Script module paths use forward slashes: the 1.30 Windows server ignores backslash paths.

class CfgPatches
{
	class INT_PartyMode_CL
	{
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Scripts", "DZ_Sounds_Effects", "INT_UIFramework"};
	};
};

class CfgMods
{
	class INT_PartyMode_CL
	{
		type = "mod";
		dir = "INT_PartyMode_CL";
		name = "INT PartyMode";
		author = "Intenso";
		version = "1.1.0";
		inputs = "INT_PartyMode_CL/data/inputs.xml";
		dependencies[] = {"Game", "Mission"};
		defines[] = {"INT_PARTYMODE_CL"};
		class defs
		{
			class gameScriptModule
			{
				value = "";
				files[] = {"INT_PartyMode_CL/scripts/3_Game"};
			};
			class missionScriptModule
			{
				value = "";
				files[] = {"INT_PartyMode_CL/scripts/5_Mission"};
			};
		};
	};
};

// 2D interface / ping / notification sounds (tools/gen_pty_sfx.py -> data/sound/int_pty_*.ogg, mono 44.1 kHz).
// Played by INT_PTY_Sfx (WAVEUI, Play2D). Sample paths have no extension.
class CfgSoundShaders
{
	class INT_PTY_Ui_SoundShader
	{
		volume = 0.6;
		range = 50;
		limitation = 0;
	};
	class INT_PTY_Click_SoundShader : INT_PTY_Ui_SoundShader
	{
		samples[] = {{"INT_PartyMode_CL\data\sound\int_pty_click", 1}};
	};
	class INT_PTY_Hover_SoundShader : INT_PTY_Ui_SoundShader
	{
		samples[] = {{"INT_PartyMode_CL\data\sound\int_pty_hover", 1}};
	};
	class INT_PTY_Tab_SoundShader : INT_PTY_Ui_SoundShader
	{
		samples[] = {{"INT_PartyMode_CL\data\sound\int_pty_tab", 1}};
	};
	class INT_PTY_Toggle_SoundShader : INT_PTY_Ui_SoundShader
	{
		samples[] = {{"INT_PartyMode_CL\data\sound\int_pty_toggle", 1}};
	};
	class INT_PTY_Open_SoundShader : INT_PTY_Ui_SoundShader
	{
		samples[] = {{"INT_PartyMode_CL\data\sound\int_pty_open", 1}};
	};
	class INT_PTY_Close_SoundShader : INT_PTY_Ui_SoundShader
	{
		samples[] = {{"INT_PartyMode_CL\data\sound\int_pty_close", 1}};
	};
	class INT_PTY_PingIn_SoundShader : INT_PTY_Ui_SoundShader
	{
		samples[] = {{"INT_PartyMode_CL\data\sound\int_pty_ping_in", 1}};
	};
	class INT_PTY_PingMine_SoundShader : INT_PTY_Ui_SoundShader
	{
		samples[] = {{"INT_PartyMode_CL\data\sound\int_pty_ping_mine", 1}};
	};
	class INT_PTY_PingOut_SoundShader : INT_PTY_Ui_SoundShader
	{
		samples[] = {{"INT_PartyMode_CL\data\sound\int_pty_ping_out", 1}};
	};
	class INT_PTY_NOnline_SoundShader : INT_PTY_Ui_SoundShader
	{
		samples[] = {{"INT_PartyMode_CL\data\sound\int_pty_n_online", 1}};
	};
	class INT_PTY_NInvite_SoundShader : INT_PTY_Ui_SoundShader
	{
		samples[] = {{"INT_PartyMode_CL\data\sound\int_pty_n_invite", 1}};
	};
	class INT_PTY_NAccept_SoundShader : INT_PTY_Ui_SoundShader
	{
		samples[] = {{"INT_PartyMode_CL\data\sound\int_pty_n_accept", 1}};
	};
	class INT_PTY_NWarn_SoundShader : INT_PTY_Ui_SoundShader
	{
		samples[] = {{"INT_PartyMode_CL\data\sound\int_pty_n_warn", 1}};
	};
	class INT_PTY_NDead_SoundShader : INT_PTY_Ui_SoundShader
	{
		samples[] = {{"INT_PartyMode_CL\data\sound\int_pty_n_dead", 1}};
	};
};

class CfgSoundSets
{
	class INT_PTY_Ui_SoundSet
	{
		volumeFactor = 1.0;
		frequencyFactor = 1.0;
		spatial = 0;
		doppler = 0;
		loop = 0;
	};
	class INT_PTY_Click_SoundSet : INT_PTY_Ui_SoundSet
	{
		soundShaders[] = {"INT_PTY_Click_SoundShader"};
	};
	class INT_PTY_Hover_SoundSet : INT_PTY_Ui_SoundSet
	{
		soundShaders[] = {"INT_PTY_Hover_SoundShader"};
	};
	class INT_PTY_Tab_SoundSet : INT_PTY_Ui_SoundSet
	{
		soundShaders[] = {"INT_PTY_Tab_SoundShader"};
	};
	class INT_PTY_Toggle_SoundSet : INT_PTY_Ui_SoundSet
	{
		soundShaders[] = {"INT_PTY_Toggle_SoundShader"};
	};
	class INT_PTY_Open_SoundSet : INT_PTY_Ui_SoundSet
	{
		soundShaders[] = {"INT_PTY_Open_SoundShader"};
	};
	class INT_PTY_Close_SoundSet : INT_PTY_Ui_SoundSet
	{
		soundShaders[] = {"INT_PTY_Close_SoundShader"};
	};
	class INT_PTY_PingIn_SoundSet : INT_PTY_Ui_SoundSet
	{
		soundShaders[] = {"INT_PTY_PingIn_SoundShader"};
	};
	class INT_PTY_PingMine_SoundSet : INT_PTY_Ui_SoundSet
	{
		soundShaders[] = {"INT_PTY_PingMine_SoundShader"};
	};
	class INT_PTY_PingOut_SoundSet : INT_PTY_Ui_SoundSet
	{
		soundShaders[] = {"INT_PTY_PingOut_SoundShader"};
	};
	class INT_PTY_NOnline_SoundSet : INT_PTY_Ui_SoundSet
	{
		soundShaders[] = {"INT_PTY_NOnline_SoundShader"};
	};
	class INT_PTY_NInvite_SoundSet : INT_PTY_Ui_SoundSet
	{
		soundShaders[] = {"INT_PTY_NInvite_SoundShader"};
	};
	class INT_PTY_NAccept_SoundSet : INT_PTY_Ui_SoundSet
	{
		soundShaders[] = {"INT_PTY_NAccept_SoundShader"};
	};
	class INT_PTY_NWarn_SoundSet : INT_PTY_Ui_SoundSet
	{
		soundShaders[] = {"INT_PTY_NWarn_SoundShader"};
	};
	class INT_PTY_NDead_SoundSet : INT_PTY_Ui_SoundSet
	{
		soundShaders[] = {"INT_PTY_NDead_SoundShader"};
	};
};
