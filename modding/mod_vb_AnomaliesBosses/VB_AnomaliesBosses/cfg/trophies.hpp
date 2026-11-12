// Трофеи боссов (ТЗ 2.7): копии трофеев SFP/MSFZ на текстурах скина босса.
// Генерирует tools/ab_skins.py, руками не править. Не еда, не портятся; мелкие — в стопку до 10.
class VB_AB_TrophyBase: Inventory_Base
{
	scope = 0;
	class DamageSystem
	{
		class GlobalHealth
		{
			class Health
			{
				hitpoints = 100;
				healthLevels[] = {{1.0,{}},{0.7,{}},{0.5,{}},{0.3,{}},{0.0,{}}};
			};
		};
	};
};
// стопка как у ванильного BurlapStrip/Bone: количество = число штук, вес = weight x количество
class VB_AB_TrophyStackBase: VB_AB_TrophyBase
{
	scope = 0;
	canBeSplit = 1;
	varQuantityInit = 1;
	varQuantityMin = 0;
	varQuantityMax = 10;
	varQuantityDestroyOnMin = 1;
};
class VB_AB_Trophy_Ent_Storm: VB_AB_TrophyBase
{
	scope = 2;
	displayName = "$STR_VB_AB_Trophy_Ent_Storm";
	descriptionShort = "$STR_VB_AB_Trophy_Ent_Storm_Desc";
	model = "VB_AB\m\vb_ent_head_storm.p3d";
	weight = 1200;
	itemSize[] = {2,2};
};
class VB_AB_Trophy_Ent_Acid: VB_AB_TrophyBase
{
	scope = 2;
	displayName = "$STR_VB_AB_Trophy_Ent_Acid";
	descriptionShort = "$STR_VB_AB_Trophy_Ent_Acid_Desc";
	model = "VB_AB\m\vb_ent_head_acid.p3d";
	weight = 1200;
	itemSize[] = {2,2};
};
class VB_AB_Trophy_MozgoTrasRed_Fire: VB_AB_TrophyStackBase
{
	scope = 2;
	displayName = "$STR_VB_AB_Trophy_MozgoTrasRed_Fire";
	descriptionShort = "$STR_VB_AB_Trophy_MozgoTrasRed_Fire_Desc";
	model = "VB_AB\m\vb_mozgotras_red_ear_fire.p3d";
	weight = 93;
	itemSize[] = {1,1};
	rotationFlags = 63;
};
class VB_AB_Trophy_MozgoTrasRed_Gravity: VB_AB_TrophyStackBase
{
	scope = 2;
	displayName = "$STR_VB_AB_Trophy_MozgoTrasRed_Gravity";
	descriptionShort = "$STR_VB_AB_Trophy_MozgoTrasRed_Gravity_Desc";
	model = "VB_AB\m\vb_mozgotras_red_ear_gravity.p3d";
	weight = 93;
	itemSize[] = {1,1};
	rotationFlags = 63;
};
class VB_AB_Trophy_MozgoTras_Psi: VB_AB_TrophyStackBase
{
	scope = 2;
	displayName = "$STR_VB_AB_Trophy_MozgoTras_Psi";
	descriptionShort = "$STR_VB_AB_Trophy_MozgoTras_Psi_Desc";
	model = "VB_AB\m\vb_mozgotras_ear_psi.p3d";
	weight = 93;
	itemSize[] = {1,1};
	rotationFlags = 63;
};
class VB_AB_Trophy_MozgoTras_Storm: VB_AB_TrophyStackBase
{
	scope = 2;
	displayName = "$STR_VB_AB_Trophy_MozgoTras_Storm";
	descriptionShort = "$STR_VB_AB_Trophy_MozgoTras_Storm_Desc";
	model = "VB_AB\m\vb_mozgotras_ear_storm.p3d";
	weight = 93;
	itemSize[] = {1,1};
	rotationFlags = 63;
};
class VB_AB_Trophy_MutaWolf_Ice: VB_AB_TrophyStackBase
{
	scope = 2;
	displayName = "$STR_VB_AB_Trophy_MutaWolf_Ice";
	descriptionShort = "$STR_VB_AB_Trophy_MutaWolf_Ice_Desc";
	model = "VB_AB\m\vb_mutawolf_claw_ice.p3d";
	weight = 113;
	itemSize[] = {1,1};
	rotationFlags = 63;
};
class VB_AB_Trophy_MutaWolf_Fire: VB_AB_TrophyStackBase
{
	scope = 2;
	displayName = "$STR_VB_AB_Trophy_MutaWolf_Fire";
	descriptionShort = "$STR_VB_AB_Trophy_MutaWolf_Fire_Desc";
	model = "VB_AB\m\vb_mutawolf_claw_fire.p3d";
	weight = 113;
	itemSize[] = {1,1};
	rotationFlags = 63;
};
class VB_AB_Trophy_Shum_Gravity: VB_AB_TrophyStackBase
{
	scope = 2;
	displayName = "$STR_VB_AB_Trophy_Shum_Gravity";
	descriptionShort = "$STR_VB_AB_Trophy_Shum_Gravity_Desc";
	model = "VB_AB\m\vb_shum_jaw_gravity.p3d";
	weight = 11;
	itemSize[] = {1,1};
};
class VB_AB_Trophy_Shum_Ice: VB_AB_TrophyStackBase
{
	scope = 2;
	displayName = "$STR_VB_AB_Trophy_Shum_Ice";
	descriptionShort = "$STR_VB_AB_Trophy_Shum_Ice_Desc";
	model = "VB_AB\m\vb_shum_jaw_ice.p3d";
	weight = 11;
	itemSize[] = {1,1};
};
class VB_AB_Trophy_Tenevnik_Psi: VB_AB_TrophyStackBase
{
	scope = 2;
	displayName = "$STR_VB_AB_Trophy_Tenevnik_Psi";
	descriptionShort = "$STR_VB_AB_Trophy_Tenevnik_Psi_Desc";
	model = "VB_AB\m\vb_tenevnik_eye_psi.p3d";
	weight = 11;
	itemSize[] = {1,1};
};
class VB_AB_Trophy_Tenevnik_Rad: VB_AB_TrophyStackBase
{
	scope = 2;
	displayName = "$STR_VB_AB_Trophy_Tenevnik_Rad";
	descriptionShort = "$STR_VB_AB_Trophy_Tenevnik_Rad_Desc";
	model = "VB_AB\m\vb_tenevnik_eye_rad.p3d";
	weight = 11;
	itemSize[] = {1,1};
};
class VB_AB_Trophy_Ricar_Acid: VB_AB_TrophyBase
{
	scope = 2;
	displayName = "$STR_VB_AB_Trophy_Ricar_Acid";
	descriptionShort = "$STR_VB_AB_Trophy_Ricar_Acid_Desc";
	model = "VB_AB\m\vb_ricar_head_acid.p3d";
	weight = 1200;
	itemSize[] = {2,2};
};
class VB_AB_Trophy_Ricar_Rad: VB_AB_TrophyBase
{
	scope = 2;
	displayName = "$STR_VB_AB_Trophy_Ricar_Rad";
	descriptionShort = "$STR_VB_AB_Trophy_Ricar_Rad_Desc";
	model = "VB_AB\m\vb_ricar_head_rad.p3d";
	weight = 1200;
	itemSize[] = {2,2};
};
