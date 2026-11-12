// Скины боссов (ТЗ 4.5): модель x палитра. Генерирует tools/ab_skins.py, руками не править.
// vbABScale и vbABHealth — значения модели по умолчанию (ТЗ 4.4), bosses.json их переопределяет.
class VB_AB_Ent_Storm: VB_AB_BossBase
{
	scope = 2;
	displayName = "$STR_VB_AB_Boss_Ent_Storm";
	model = "VB_AB\m\vb_ent_storm.p3d";
	vbABScale = 1.6;
	vbABHealth = 8000;
};
class VB_AB_Ent_Acid: VB_AB_BossBase
{
	scope = 2;
	displayName = "$STR_VB_AB_Boss_Ent_Acid";
	model = "VB_AB\m\vb_ent_acid.p3d";
	vbABScale = 1.6;
	vbABHealth = 8000;
};
class VB_AB_MozgoTrasRed_Fire: VB_AB_BossBase
{
	scope = 2;
	displayName = "$STR_VB_AB_Boss_MozgoTrasRed_Fire";
	model = "VB_AB\m\vb_mozgotras_red_fire.p3d";
	vbABScale = 1.1;
	vbABHealth = 5000;
};
class VB_AB_MozgoTrasRed_Gravity: VB_AB_BossBase
{
	scope = 2;
	displayName = "$STR_VB_AB_Boss_MozgoTrasRed_Gravity";
	model = "VB_AB\m\vb_mozgotras_red_gravity.p3d";
	vbABScale = 1.1;
	vbABHealth = 5000;
};
class VB_AB_MozgoTras_Psi: VB_AB_BossBase
{
	scope = 2;
	displayName = "$STR_VB_AB_Boss_MozgoTras_Psi";
	model = "VB_AB\m\vb_mozgotras_psi.p3d";
	vbABScale = 1.1;
	vbABHealth = 4500;
};
class VB_AB_MozgoTras_Storm: VB_AB_BossBase
{
	scope = 2;
	displayName = "$STR_VB_AB_Boss_MozgoTras_Storm";
	model = "VB_AB\m\vb_mozgotras_storm.p3d";
	vbABScale = 1.1;
	vbABHealth = 4500;
};
class VB_AB_MutaWolf_Ice: VB_AB_BossBase
{
	scope = 2;
	displayName = "$STR_VB_AB_Boss_MutaWolf_Ice";
	model = "VB_AB\m\vb_mutawolf_ice.p3d";
	vbABScale = 1.1;
	vbABHealth = 3500;
};
class VB_AB_MutaWolf_Fire: VB_AB_BossBase
{
	scope = 2;
	displayName = "$STR_VB_AB_Boss_MutaWolf_Fire";
	model = "VB_AB\m\vb_mutawolf_fire.p3d";
	vbABScale = 1.1;
	vbABHealth = 3500;
};
class VB_AB_Shum_Gravity: VB_AB_BossBase
{
	scope = 2;
	displayName = "$STR_VB_AB_Boss_Shum_Gravity";
	model = "VB_AB\m\vb_shum_gravity.p3d";
	vbABScale = 1;
	vbABHealth = 3500;
};
class VB_AB_Shum_Ice: VB_AB_BossBase
{
	scope = 2;
	displayName = "$STR_VB_AB_Boss_Shum_Ice";
	model = "VB_AB\m\vb_shum_ice.p3d";
	vbABScale = 1;
	vbABHealth = 3500;
};
class VB_AB_Tenevnik_Psi: VB_AB_BossBase
{
	scope = 2;
	displayName = "$STR_VB_AB_Boss_Tenevnik_Psi";
	model = "VB_AB\m\vb_tenevnik_psi.p3d";
	vbABScale = 1;
	vbABHealth = 3000;
};
class VB_AB_Tenevnik_Rad: VB_AB_BossBase
{
	scope = 2;
	displayName = "$STR_VB_AB_Boss_Tenevnik_Rad";
	model = "VB_AB\m\vb_tenevnik_rad.p3d";
	vbABScale = 1;
	vbABHealth = 3000;
};
class VB_AB_Ricar_Acid: VB_AB_BossBase
{
	scope = 2;
	displayName = "$STR_VB_AB_Boss_Ricar_Acid";
	model = "VB_AB\m\vb_ricar_acid.p3d";
	vbABScale = 1;
	vbABHealth = 2500;
	// секция camo покрывает тело: свой материал со свечением прожилок (Stage8)
	hiddenSelections[] = {"camo"};
	hiddenSelectionsTextures[] = {"VB_AB\t\riacid\msfz_ricar_body___________________.paa"};
	hiddenSelectionsMaterials[] = {"VB_AB\t\riacid\vb_glow.rvmat"};
};
class VB_AB_Ricar_Rad: VB_AB_BossBase
{
	scope = 2;
	displayName = "$STR_VB_AB_Boss_Ricar_Rad";
	model = "VB_AB\m\vb_ricar_rad.p3d";
	vbABScale = 1;
	vbABHealth = 2500;
	// секция camo покрывает тело: свой материал со свечением прожилок (Stage8)
	hiddenSelections[] = {"camo"};
	hiddenSelectionsTextures[] = {"VB_AB\t\rirad\msfz_ricar_body____________________.paa"};
	hiddenSelectionsMaterials[] = {"VB_AB\t\rirad\vb_glow.rvmat"};
};
