// СГЕНЕРИРОВАНО tools/ab_voices.py — руками не править
// Голоса боссов: наборы (внутри class CfgSoundSets в config.cpp). Звук — только у клиента, VB_AB_Voice.c
	// база: как ванильный Zmb_VoiceFX_Base_SoundSet, но кривая громкости «крик» (слышно в ~2,5 раза дальше зомби)
	class VB_AB_Voice_Base_SoundSet
	{
		sound3DProcessingType = "infected3DProcessingType";
		distanceFilter = "infectedDistanceFreqAttenuationFilter";
		volumeCurve = "infectedShoutingAttenuationCurve";
		spatial = 1;
		doppler = 0;
		loop = 0;
		occlusionFactor = 1;
		obstructionFactor = 1;
		volumeFactor = 1;
		frequencyFactor = 1;
	};
	class VB_AB_Voice_Ent_Idle_SoundSet: VB_AB_Voice_Base_SoundSet
	{
		soundShaders[] = {"VB_AB_Voice_Ent_Idle_SoundShader"};
		frequencyFactor = 1;
	};
	class VB_AB_Voice_Ent_Alert_SoundSet: VB_AB_Voice_Base_SoundSet
	{
		soundShaders[] = {"VB_AB_Voice_Ent_Alert_SoundShader"};
		frequencyFactor = 1;
	};
	class VB_AB_Voice_Ent_Chase_SoundSet: VB_AB_Voice_Base_SoundSet
	{
		soundShaders[] = {"VB_AB_Voice_Ent_Chase_SoundShader"};
		frequencyFactor = 1;
	};
	class VB_AB_Voice_Ent_Attack_SoundSet: VB_AB_Voice_Base_SoundSet
	{
		soundShaders[] = {"VB_AB_Voice_Ent_Attack_SoundShader"};
		frequencyFactor = 1;
	};
	class VB_AB_Voice_Ent_Scream_SoundSet: VB_AB_Voice_Base_SoundSet
	{
		soundShaders[] = {"VB_AB_Voice_Ent_Scream_SoundShader"};
		frequencyFactor = 1;
	};
	class VB_AB_Voice_Ent_Pain_SoundSet: VB_AB_Voice_Base_SoundSet
	{
		soundShaders[] = {"VB_AB_Voice_Ent_Pain_SoundShader"};
		frequencyFactor = 1;
	};
	class VB_AB_Voice_Ent_Death_SoundSet: VB_AB_Voice_Base_SoundSet
	{
		soundShaders[] = {"VB_AB_Voice_Ent_Death_SoundShader"};
		frequencyFactor = 1;
	};
	class VB_AB_Voice_Ricar_Idle_SoundSet: VB_AB_Voice_Base_SoundSet
	{
		soundShaders[] = {"VB_AB_Voice_Ricar_Idle_SoundShader"};
		frequencyFactor = 1;
	};
	class VB_AB_Voice_Ricar_Alert_SoundSet: VB_AB_Voice_Base_SoundSet
	{
		soundShaders[] = {"VB_AB_Voice_Ricar_Alert_SoundShader"};
		frequencyFactor = 1;
	};
	class VB_AB_Voice_Ricar_Chase_SoundSet: VB_AB_Voice_Base_SoundSet
	{
		soundShaders[] = {"VB_AB_Voice_Ricar_Chase_SoundShader"};
		frequencyFactor = 1;
	};
	class VB_AB_Voice_Ricar_Attack_SoundSet: VB_AB_Voice_Base_SoundSet
	{
		soundShaders[] = {"VB_AB_Voice_Ricar_Attack_SoundShader"};
		frequencyFactor = 1;
	};
	class VB_AB_Voice_Ricar_Scream_SoundSet: VB_AB_Voice_Base_SoundSet
	{
		soundShaders[] = {"VB_AB_Voice_Ricar_Scream_SoundShader"};
		frequencyFactor = 1;
	};
	class VB_AB_Voice_Ricar_Pain_SoundSet: VB_AB_Voice_Base_SoundSet
	{
		soundShaders[] = {"VB_AB_Voice_Ricar_Pain_SoundShader"};
		frequencyFactor = 1;
	};
	class VB_AB_Voice_Ricar_Death_SoundSet: VB_AB_Voice_Base_SoundSet
	{
		soundShaders[] = {"VB_AB_Voice_Ricar_Death_SoundShader"};
		frequencyFactor = 1;
	};
	class VB_AB_Voice_MozgoTras_Idle_SoundSet: VB_AB_Voice_Base_SoundSet
	{
		soundShaders[] = {"VB_AB_Voice_MozgoTras_Idle_SoundShader"};
		frequencyFactor = 1;
	};
	class VB_AB_Voice_MozgoTras_Alert_SoundSet: VB_AB_Voice_Base_SoundSet
	{
		soundShaders[] = {"VB_AB_Voice_MozgoTras_Alert_SoundShader"};
		frequencyFactor = 1;
	};
	class VB_AB_Voice_MozgoTras_Chase_SoundSet: VB_AB_Voice_Base_SoundSet
	{
		soundShaders[] = {"VB_AB_Voice_MozgoTras_Chase_SoundShader"};
		frequencyFactor = 1;
	};
	class VB_AB_Voice_MozgoTras_Attack_SoundSet: VB_AB_Voice_Base_SoundSet
	{
		soundShaders[] = {"VB_AB_Voice_MozgoTras_Attack_SoundShader"};
		frequencyFactor = 1;
	};
	class VB_AB_Voice_MozgoTras_Scream_SoundSet: VB_AB_Voice_Base_SoundSet
	{
		soundShaders[] = {"VB_AB_Voice_MozgoTras_Scream_SoundShader"};
		frequencyFactor = 1;
	};
	class VB_AB_Voice_MozgoTras_Pain_SoundSet: VB_AB_Voice_Base_SoundSet
	{
		soundShaders[] = {"VB_AB_Voice_MozgoTras_Pain_SoundShader"};
		frequencyFactor = 1;
	};
	class VB_AB_Voice_MozgoTras_Death_SoundSet: VB_AB_Voice_Base_SoundSet
	{
		soundShaders[] = {"VB_AB_Voice_MozgoTras_Death_SoundShader"};
		frequencyFactor = 1;
	};
	class VB_AB_Voice_MozgoTrasRed_Idle_SoundSet: VB_AB_Voice_Base_SoundSet
	{
		soundShaders[] = {"VB_AB_Voice_MozgoTrasRed_Idle_SoundShader"};
		frequencyFactor = 0.88;
	};
	class VB_AB_Voice_MozgoTrasRed_Alert_SoundSet: VB_AB_Voice_Base_SoundSet
	{
		soundShaders[] = {"VB_AB_Voice_MozgoTrasRed_Alert_SoundShader"};
		frequencyFactor = 0.88;
	};
	class VB_AB_Voice_MozgoTrasRed_Chase_SoundSet: VB_AB_Voice_Base_SoundSet
	{
		soundShaders[] = {"VB_AB_Voice_MozgoTrasRed_Chase_SoundShader"};
		frequencyFactor = 0.88;
	};
	class VB_AB_Voice_MozgoTrasRed_Attack_SoundSet: VB_AB_Voice_Base_SoundSet
	{
		soundShaders[] = {"VB_AB_Voice_MozgoTrasRed_Attack_SoundShader"};
		frequencyFactor = 0.88;
	};
	class VB_AB_Voice_MozgoTrasRed_Scream_SoundSet: VB_AB_Voice_Base_SoundSet
	{
		soundShaders[] = {"VB_AB_Voice_MozgoTrasRed_Scream_SoundShader"};
		frequencyFactor = 0.88;
	};
	class VB_AB_Voice_MozgoTrasRed_Pain_SoundSet: VB_AB_Voice_Base_SoundSet
	{
		soundShaders[] = {"VB_AB_Voice_MozgoTrasRed_Pain_SoundShader"};
		frequencyFactor = 0.88;
	};
	class VB_AB_Voice_MozgoTrasRed_Death_SoundSet: VB_AB_Voice_Base_SoundSet
	{
		soundShaders[] = {"VB_AB_Voice_MozgoTrasRed_Death_SoundShader"};
		frequencyFactor = 0.88;
	};
	class VB_AB_Voice_Tenevnik_Idle_SoundSet: VB_AB_Voice_Base_SoundSet
	{
		soundShaders[] = {"VB_AB_Voice_Tenevnik_Idle_SoundShader"};
		frequencyFactor = 0.85;
	};
	class VB_AB_Voice_Tenevnik_Alert_SoundSet: VB_AB_Voice_Base_SoundSet
	{
		soundShaders[] = {"VB_AB_Voice_Tenevnik_Alert_SoundShader"};
		frequencyFactor = 0.85;
	};
	class VB_AB_Voice_Tenevnik_Chase_SoundSet: VB_AB_Voice_Base_SoundSet
	{
		soundShaders[] = {"VB_AB_Voice_Tenevnik_Chase_SoundShader"};
		frequencyFactor = 0.85;
	};
	class VB_AB_Voice_Tenevnik_Attack_SoundSet: VB_AB_Voice_Base_SoundSet
	{
		soundShaders[] = {"VB_AB_Voice_Tenevnik_Attack_SoundShader"};
		frequencyFactor = 0.85;
	};
	class VB_AB_Voice_Tenevnik_Scream_SoundSet: VB_AB_Voice_Base_SoundSet
	{
		soundShaders[] = {"VB_AB_Voice_Tenevnik_Scream_SoundShader"};
		frequencyFactor = 0.85;
	};
	class VB_AB_Voice_Tenevnik_Pain_SoundSet: VB_AB_Voice_Base_SoundSet
	{
		soundShaders[] = {"VB_AB_Voice_Tenevnik_Pain_SoundShader"};
		frequencyFactor = 0.85;
	};
	class VB_AB_Voice_Tenevnik_Death_SoundSet: VB_AB_Voice_Base_SoundSet
	{
		soundShaders[] = {"VB_AB_Voice_Tenevnik_Death_SoundShader"};
		frequencyFactor = 0.85;
	};
	class VB_AB_Voice_Shum_Idle_SoundSet: VB_AB_Voice_Base_SoundSet
	{
		soundShaders[] = {"VB_AB_Voice_Shum_Idle_SoundShader"};
		frequencyFactor = 1.12;
	};
	class VB_AB_Voice_Shum_Alert_SoundSet: VB_AB_Voice_Base_SoundSet
	{
		soundShaders[] = {"VB_AB_Voice_Shum_Alert_SoundShader"};
		frequencyFactor = 1.12;
	};
	class VB_AB_Voice_Shum_Chase_SoundSet: VB_AB_Voice_Base_SoundSet
	{
		soundShaders[] = {"VB_AB_Voice_Shum_Chase_SoundShader"};
		frequencyFactor = 1.12;
	};
	class VB_AB_Voice_Shum_Attack_SoundSet: VB_AB_Voice_Base_SoundSet
	{
		soundShaders[] = {"VB_AB_Voice_Shum_Attack_SoundShader"};
		frequencyFactor = 1.12;
	};
	class VB_AB_Voice_Shum_Scream_SoundSet: VB_AB_Voice_Base_SoundSet
	{
		soundShaders[] = {"VB_AB_Voice_Shum_Scream_SoundShader"};
		frequencyFactor = 1.12;
	};
	class VB_AB_Voice_Shum_Pain_SoundSet: VB_AB_Voice_Base_SoundSet
	{
		soundShaders[] = {"VB_AB_Voice_Shum_Pain_SoundShader"};
		frequencyFactor = 1.12;
	};
	class VB_AB_Voice_Shum_Death_SoundSet: VB_AB_Voice_Base_SoundSet
	{
		soundShaders[] = {"VB_AB_Voice_Shum_Death_SoundShader"};
		frequencyFactor = 1.12;
	};
	class VB_AB_Voice_MutaWolf_Idle_SoundSet: VB_AB_Voice_Base_SoundSet
	{
		soundShaders[] = {"VB_AB_Voice_MutaWolf_Idle_SoundShader"};
		frequencyFactor = 1;
	};
	class VB_AB_Voice_MutaWolf_Alert_SoundSet: VB_AB_Voice_Base_SoundSet
	{
		soundShaders[] = {"VB_AB_Voice_MutaWolf_Alert_SoundShader"};
		frequencyFactor = 1;
	};
	class VB_AB_Voice_MutaWolf_Chase_SoundSet: VB_AB_Voice_Base_SoundSet
	{
		soundShaders[] = {"VB_AB_Voice_MutaWolf_Chase_SoundShader"};
		frequencyFactor = 1;
	};
	class VB_AB_Voice_MutaWolf_Attack_SoundSet: VB_AB_Voice_Base_SoundSet
	{
		soundShaders[] = {"VB_AB_Voice_MutaWolf_Attack_SoundShader"};
		frequencyFactor = 1;
	};
	class VB_AB_Voice_MutaWolf_Scream_SoundSet: VB_AB_Voice_Base_SoundSet
	{
		soundShaders[] = {"VB_AB_Voice_MutaWolf_Scream_SoundShader"};
		frequencyFactor = 1;
	};
	class VB_AB_Voice_MutaWolf_Pain_SoundSet: VB_AB_Voice_Base_SoundSet
	{
		soundShaders[] = {"VB_AB_Voice_MutaWolf_Pain_SoundShader"};
		frequencyFactor = 1;
	};
	class VB_AB_Voice_MutaWolf_Death_SoundSet: VB_AB_Voice_Base_SoundSet
	{
		soundShaders[] = {"VB_AB_Voice_MutaWolf_Death_SoundShader"};
		frequencyFactor = 1;
	};
