class CfgPatches
{
	class KOD_weapons_DZVR_sounds
	{
		units[]={};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Sounds_Effects",
			"DZ_Data"
		};
	};
};

class CfgSoundShaders
{
	class baseCharacter_SoundShader;
	class DZVR_KOD_Rifle_Closure_SoundShader
	{
		samples[]=
		{
			
			{
				"KOD_weapons\weapons\sounds\base\rifle_closure_01",
				1
			},
			
			{
				"KOD_weapons\weapons\sounds\base\rifle_closure_02",
				1
			}
		};
		range=5;
		volume=0.44668359;
	};
	
	class DZVR_KOD_AutoRifle_closeShot_SoundShader
	{
		samples[]=
		{
			
			{
				"KOD_weapons\weapons\sounds\base\autorifle_shot_close",
				1
			}
		};
		volume=2;
		range=100;
		rangeCurve="closeShotCurve";
	};
	class DZVR_KOD_AutoRifle_midShot_SoundShader
	{
		samples[]=
		{
			
			{
				"KOD_weapons\weapons\sounds\base\autorifle_mid_shot",
				1
			}
		};
		volume=0.56234133;
		range=1500;
		rangeCurve[]=
		{
			{0,0.2},
			{50,1},
			{300,0},
			{1500,0}
		};
	};
	class DZVR_KOD_AutoRifle_distShot_SoundShader
	{
		samples[]=
		{
			
			{
				"KOD_weapons\weapons\sounds\base\autorifle_dist_shot",
				1
			}
		};
		volume=1;
		range=3500;
		rangeCurve[]=
		{
			{0,0},
			{50,0},
			{300,1},
			{3500,1}
		};
	};
	class DZVR_KOD_Rifle_SilincerShot_SoundShader
	{
		samples[]=
		{
			
			{
				"KOD_weapons\weapons\sounds\base\rifle_silincer",
				1
			}
		};
		volume=1;
		range=100;
		rangeCurve="closeShotCurve";
	};
	class DZVR_KOD_Rifle_SilincerMidShot_SoundShader
	{
		samples[]=
		{
			
			{
				"KOD_weapons\weapons\sounds\base\rifle_silincer_mid",
				1
			}
		};
		volume=0.56234133;
		range=500;
		rangeCurve[]=
		{
			{0,0.2},
			{50,1},
			{140,0},
			{250,0}
		};
	};
	class DZVR_KOD_Rifle_SilincerDistShot_SoundShader
	{
		samples[]=
		{
			
			{
				"KOD_weapons\weapons\sounds\base\rifle_silincer_dist",
				1
			}
		};
		volume=1;
		range=800;
		rangeCurve[]=
		{
			{0,0},
			{50,0},
			{140,1},
			{250,1}
		};
	};
	
///////////////////звуки непосредственно пушек новые/////////////////////

	
	class DZVR_KOD_AKALPHA_closeShot_SoundShader
	{
		samples[]=
		{
			
			{
				"KOD_weapons\weapons\sounds\rifles\akalpha_shot",
				1
			}
		};
		volume=2;
		range=100;
		rangeCurve="closeShotCurve";
	};
	
	class DZVR_KOD_AKALPHA_SilincerShot_SoundShader
	{
		samples[]=
		{
			
			{
				"KOD_weapons\weapons\sounds\rifles\akalpha_shot_sil",
				1
			}
		};
		volume=1;
		range=100;
		rangeCurve="closeShotCurve";
	};
	
	
	class DZVR_KOD_AK47_closeShot_SoundShader
	{
		samples[]=
		{
			
			{
				"KOD_weapons\weapons\sounds\rifles\ak47_shot",
				1
			}
		};
		volume=1.1;
		range=100;
		rangeCurve="closeShotCurve";
	};
	
	class DZVR_KOD_AKM_closeShot_SoundShader
	{
		samples[]=
		{
			
			{
				"KOD_weapons\weapons\sounds\rifles\akm_shot",
				1
			}
		};
		volume=1.5;
		range=100;
		rangeCurve="closeShotCurve";
	};

	class DZVR_KOD_Beowulf_closeShot_SoundShader
	{
		samples[]=
		{
			
			{
				"KOD_weapons\weapons\sounds\rifles\beowulf_shot",
				1
			}
		};
		volume=0.3;
		range=100;
		rangeCurve="closeShotCurve";
	};
	
	
	class DZVR_KOD_AWM_SilincerShot_SoundShader
	{
		samples[]=
		{
			
			{
				"KOD_weapons\weapons\sounds\rifles\awm_shot_sil",
				1
			}
		};
		volume=1;
		range=60;
		rangeCurve="closeShotCurve";
	};
	
	class DZVR_KOD_AK12_closeShot_SoundShader
	{
		samples[]=
		{
			
			{
				"KOD_weapons\weapons\sounds\rifles\ak12_shot",
				1
			}
		};
		volume=0.9;
		range=100;
		rangeCurve="closeShotCurve";
	};
	
	class DZVR_KOD_DVL_closeShot_SoundShader
	{
		samples[]=
		{
			
			{
				"KOD_weapons\weapons\sounds\rifles\dvl_shot",
				1
			}
		};
		volume=2;
		range=300;
		rangeCurve="closeShotCurve";
	};

	class DZVR_KOD_HK21_closeShot_SoundShader
	{
		samples[]=
		{
			
			{
				"KOD_weapons\weapons\sounds\rifles\hk21_shot",
				1
			}
		};
		volume=0.7;
		range=100;
		rangeCurve="closeShotCurve";
	};
	
	class DZVR_KOD_FAL_SilincerShot_SoundShader
	{
		samples[]=
		{
			
			{
				"KOD_weapons\weapons\sounds\rifles\fal_shot_sil",
				1
			}
		};
		volume=1;
		range=70;
		rangeCurve="closeShotCurve";
	};
	
	class DZVR_KOD_HK417_closeShot_SoundShader
	{
		samples[]=
		{
			
			{
				"KOD_weapons\weapons\sounds\rifles\hk417_shot",
				1
			}
		};
		volume=1.6;
		range=100;
		rangeCurve="closeShotCurve";
	};
	
	class DZVR_KOD_M4_closeShot_SoundShader
	{
		samples[]=
		{
			
			{
				"KOD_weapons\weapons\sounds\rifles\m4_shot",
				1
			}
		};
		volume=1.5;
		range=100;
		rangeCurve="closeShotCurve";
	};
	
	class DZVR_KOD_M4_SilincerShot_SoundShader
	{
		samples[]=
		{
			
			{
				"KOD_weapons\weapons\sounds\rifles\m4_shot_sil",
				1
			}
		};
		volume=1;
		range=70;
		rangeCurve="closeShotCurve";
	};
	
	class DZVR_KOD_PKP_SilincerShot_SoundShader
	{
		samples[]=
		{
			
			{
				"KOD_weapons\weapons\sounds\rifles\pkp_shot_sil",
				1
			}
		};
		volume=1;
		range=70;
		rangeCurve="closeShotCurve";
	};
	
};
class CfgSoundSets
{
	class Rifle_Shot_Base_SoundSet;
	class Rifle_silencerHomemade_Base_SoundSet;
	class DZVR_KOD_Rifle_Shot_SoundSet: Rifle_Shot_Base_SoundSet
	{
		soundShaders[]=
		{
			"DZVR_KOD_AutoRifle_closeShot_SoundShader",
			"DZVR_KOD_AutoRifle_midShot_SoundShader",
			"DZVR_KOD_AutoRifle_distShot_SoundShader",
			"DZVR_KOD_Rifle_Closure_SoundShader"
		};
	};
	class DZVR_KOD_Rifle_Silencer_SoundSet: Rifle_silencerHomemade_Base_SoundSet
	{
		soundShaders[]=
		{
			"DZVR_KOD_Rifle_SilincerShot_SoundShader",
			"DZVR_KOD_Rifle_SilincerMidShot_SoundShader",
			"DZVR_KOD_Rifle_SilincerDistShot_SoundShader",
			"DZVR_KOD_Rifle_Closure_SoundShader"
		};
	};
//////////////////////////////////саундсеты непосредственно пушек/////////////////
	class DZVR_KOD_AKALPHA_Shot_SoundSet: Rifle_Shot_Base_SoundSet
	{
		soundShaders[]=
		{
			"DZVR_KOD_AKALPHA_closeShot_SoundShader",
			"DZVR_KOD_AutoRifle_midShot_SoundShader",
			"DZVR_KOD_AutoRifle_distShot_SoundShader",
			"DZVR_KOD_Rifle_Closure_SoundShader"
		};
	};
	class DZVR_KOD_AKALPHA_Silencer_SoundSet: Rifle_silencerHomemade_Base_SoundSet
	{
		soundShaders[]=
		{
			"DZVR_KOD_AKALPHA_SilincerShot_SoundShader",
			"DZVR_KOD_Rifle_SilincerMidShot_SoundShader",
			"DZVR_KOD_Rifle_SilincerDistShot_SoundShader",
			"DZVR_KOD_Rifle_Closure_SoundShader"
		};
	};
	
	class DZVR_KOD_AK47_Shot_SoundSet: Rifle_Shot_Base_SoundSet
	{
		soundShaders[]=
		{
			"DZVR_KOD_AK47_closeShot_SoundShader",
			"DZVR_KOD_AutoRifle_midShot_SoundShader",
			"DZVR_KOD_AutoRifle_distShot_SoundShader",
			"DZVR_KOD_Rifle_Closure_SoundShader"
		};
	};
	
	class DZVR_KOD_AKM_Shot_SoundSet: Rifle_Shot_Base_SoundSet
	{
		soundShaders[]=
		{
			"DZVR_KOD_AKM_closeShot_SoundShader",
			"DZVR_KOD_AutoRifle_midShot_SoundShader",
			"DZVR_KOD_AutoRifle_distShot_SoundShader",
			"DZVR_KOD_Rifle_Closure_SoundShader"
		};
	};
	
	class DZVR_KOD_Beowulf_Shot_SoundSet: Rifle_Shot_Base_SoundSet
	{
		soundShaders[]=
		{
			"DZVR_KOD_Beowulf_closeShot_SoundShader",
			"DZVR_KOD_AutoRifle_midShot_SoundShader",
			"DZVR_KOD_AutoRifle_distShot_SoundShader",
			"DZVR_KOD_Rifle_Closure_SoundShader"
		};
	};
	
	class DZVR_KOD_AWM_Silencer_SoundSet: Rifle_silencerHomemade_Base_SoundSet
	{
		soundShaders[]=
		{
			"DZVR_KOD_AWM_SilincerShot_SoundShader",
			"DZVR_KOD_Rifle_SilincerMidShot_SoundShader",
			"DZVR_KOD_Rifle_SilincerDistShot_SoundShader",
			"DZVR_KOD_Rifle_Closure_SoundShader"
		};
	};	
	
	class DZVR_KOD_AK12_Shot_SoundSet: Rifle_Shot_Base_SoundSet
	{
		soundShaders[]=
		{
			"DZVR_KOD_AK12_closeShot_SoundShader",
			"DZVR_KOD_AutoRifle_midShot_SoundShader",
			"DZVR_KOD_AutoRifle_distShot_SoundShader",
			"DZVR_KOD_Rifle_Closure_SoundShader"
		};
	};
	
	class DZVR_KOD_DVL_Shot_SoundSet: Rifle_Shot_Base_SoundSet
	{
		soundShaders[]=
		{
			"DZVR_KOD_DVL_closeShot_SoundShader",
			"DZVR_KOD_AutoRifle_midShot_SoundShader",
			"DZVR_KOD_AutoRifle_distShot_SoundShader",
			"DZVR_KOD_Rifle_Closure_SoundShader"
		};
	};
	
	class DZVR_KOD_HK21_Shot_SoundSet: Rifle_Shot_Base_SoundSet
	{
		soundShaders[]=
		{
			"DZVR_KOD_HK21_closeShot_SoundShader",
			"DZVR_KOD_AutoRifle_midShot_SoundShader",
			"DZVR_KOD_AutoRifle_distShot_SoundShader",
			"DZVR_KOD_Rifle_Closure_SoundShader"
		};
	};
	
	class DZVR_KOD_FAL_Silencer_SoundSet: Rifle_silencerHomemade_Base_SoundSet
	{
		soundShaders[]=
		{
			"DZVR_KOD_FAL_SilincerShot_SoundShader",
			"DZVR_KOD_Rifle_SilincerMidShot_SoundShader",
			"DZVR_KOD_Rifle_SilincerDistShot_SoundShader",
			"DZVR_KOD_Rifle_Closure_SoundShader"
		};
	};	
	
	
	class DZVR_KOD_HK417_Shot_SoundSet: Rifle_Shot_Base_SoundSet
	{
		soundShaders[]=
		{
			"DZVR_KOD_HK417_closeShot_SoundShader",
			"DZVR_KOD_AutoRifle_midShot_SoundShader",
			"DZVR_KOD_AutoRifle_distShot_SoundShader",
			"DZVR_KOD_Rifle_Closure_SoundShader"
		};
	};	
	
	class DZVR_KOD_M4_Shot_SoundSet: Rifle_Shot_Base_SoundSet
	{
		soundShaders[]=
		{
			"DZVR_KOD_M4_closeShot_SoundShader",
			"DZVR_KOD_AutoRifle_midShot_SoundShader",
			"DZVR_KOD_AutoRifle_distShot_SoundShader",
			"DZVR_KOD_Rifle_Closure_SoundShader"
		};
	};	
	
	class DZVR_KOD_M4_Silencer_SoundSet: Rifle_silencerHomemade_Base_SoundSet
	{
		soundShaders[]=
		{
			"DZVR_KOD_M4_SilincerShot_SoundShader",
			"DZVR_KOD_Rifle_SilincerMidShot_SoundShader",
			"DZVR_KOD_Rifle_SilincerDistShot_SoundShader",
			"DZVR_KOD_Rifle_Closure_SoundShader"
		};
	};	
	
	class DZVR_KOD_PKP_Silencer_SoundSet: Rifle_silencerHomemade_Base_SoundSet
	{
		soundShaders[]=
		{
			"DZVR_KOD_PKP_SilincerShot_SoundShader",
			"DZVR_KOD_Rifle_SilincerMidShot_SoundShader",
			"DZVR_KOD_Rifle_SilincerDistShot_SoundShader",
			"DZVR_KOD_Rifle_Closure_SoundShader"
		};
	};	
};
