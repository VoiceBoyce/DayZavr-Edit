// VB_MiningResources_Veles: «Велесова кирка» (ТЗ_Велесова_кирка.md). Отдельный PBO в @VB_MiningResources\Addons:
// кирка наследует Магическую кирку MSFZ, поэтому зависит от MSFZ_Mifical_Pack; основной мод руды от MSFZ не зависит.
// Модель и текстуры — в основном PBO (VB_MiningResources\data\veles, генератор tools\gen_veles_pickaxe.py).
// Свойства добычи (время, износ, шанс, «Дар Велеса») — в settings.json сервера, запись MineToolsData.
class CfgPatches
{
	class VB_MiningResources_Veles
	{
		units[] = {"VB_MR_Pickaxe_Veles"};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data","DZ_Scripts","DZ_Weapons_Melee","VB_MiningResources","MSFZ_Magic_Kirka"};
	};
};
class CfgMods
{
	class VB_MiningResources_Veles
	{
		dir = "VB_MiningResources_Veles";
		name = "VB_MiningResources_Veles";
		author = "DayZavr";
		type = "mod";
		dependencies[] = {"World"};
		class defs
		{
			class worldScriptModule
			{
				value = "";
				files[] = {"VB_MiningResources_Veles/scripts/4_World"};
			};
		};
	};
};
class CfgVehicles
{
	class MSFZ_Magic_Kirka;
	class VB_MR_Pickaxe_Veles: MSFZ_Magic_Kirka
	{
		scope = 2;
		displayName = "$STR_VB_MR_Pickaxe_Veles_N";
		descriptionShort = "$STR_VB_MR_Pickaxe_Veles_D";
		model = "VB_MiningResources\data\veles\vb_mr_pickaxe_veles.p3d";
		// секция золотых прожилок и руны: «чует жилу» меняет её материал на светящийся (VB_MR_Veles.c).
		// Пути к .rvmat БЕЗ ведущего «\»: с ним движок материал не загружает, свечения нет (стенд 2026-11-20, E75)
		hiddenSelections[] = {"vbmr_glow"};
		hiddenSelectionsTextures[] = {"VB_MiningResources\data\veles\vb_mr_veles_glow_co.paa"};
		hiddenSelectionsMaterials[] = {"VB_MiningResources\data\veles\vb_mr_veles_glow.rvmat"};
		vbmrGlowOff = "VB_MiningResources\data\veles\vb_mr_veles_glow.rvmat";
		vbmrGlowOn = "VB_MiningResources\data\veles\vb_mr_veles_glow_on.rvmat";
		vbmrSenseRadius = 30;
		// износ: свои материалы металла, рукояти и янтаря (у родителя — материалы модели Магической кирки)
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 1500;
					healthLevels[] = {
						{1.0,{"VB_MiningResources\data\veles\vb_mr_veles_metal.rvmat","VB_MiningResources\data\veles\vb_mr_veles_wood.rvmat","VB_MiningResources\data\veles\vb_mr_veles_amber.rvmat"}},
						{0.7,{"VB_MiningResources\data\veles\vb_mr_veles_metal.rvmat","VB_MiningResources\data\veles\vb_mr_veles_wood.rvmat","VB_MiningResources\data\veles\vb_mr_veles_amber.rvmat"}},
						{0.5,{"VB_MiningResources\data\veles\vb_mr_veles_metal_damage.rvmat","VB_MiningResources\data\veles\vb_mr_veles_wood_damage.rvmat","VB_MiningResources\data\veles\vb_mr_veles_amber_damage.rvmat"}},
						{0.3,{"VB_MiningResources\data\veles\vb_mr_veles_metal_damage.rvmat","VB_MiningResources\data\veles\vb_mr_veles_wood_damage.rvmat","VB_MiningResources\data\veles\vb_mr_veles_amber_damage.rvmat"}},
						{0.0,{"VB_MiningResources\data\veles\vb_mr_veles_metal_destruct.rvmat","VB_MiningResources\data\veles\vb_mr_veles_wood_destruct.rvmat","VB_MiningResources\data\veles\vb_mr_veles_amber_destruct.rvmat"}}
					};
				};
			};
		};
	};
};
class CfgSoundShaders
{
	// гул рощи «Древнего корня» (тот же звук, что у живой рощи): кирка «гудит, как роща», когда рядом руда.
	// Файл из мода VB_AncientRoot (на 18 стоит вместе с рудой); без него кирка просто молчит.
	class VB_MR_Veles_Hum_SoundShader
	{
		samples[] = {{"VB_AncientRoot\sounds\vb_ar_hum",1}};
		volume = 0.45;
		range = 8;
		rangeCurve[] = {{0,1},{3,0.6},{8,0}};
	};
};
class CfgSoundSets
{
	class VB_MR_Veles_Hum_SoundSet
	{
		soundShaders[] = {"VB_MR_Veles_Hum_SoundShader"};
		spatial = 1;
		doppler = 0;
		loop = 1;
		volumeCurve = "LinearCurve";
		sound3DProcessingType = "ObjectsInner13DProcessingType";
	};
};
