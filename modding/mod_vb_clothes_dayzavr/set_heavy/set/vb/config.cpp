// DayZavr: улучшенные версии предметов сета (классы *_vb).
// Каждый X_vb наследует X целиком: вид, название, описание, скрипты. Параметры меняем только здесь.

class CfgPatches
{
	class set_heavy_vb
	{
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"Vp_SET16"};
	};
};

class CfgVehicles
{
	class VP_SET16_Boots_Black;
	class Vp_SET16_Gloves_Black;
	class Vp_Set16_Helmet_Black;
	class Vp_SET16_Pants_Black;
	class Vp_SET16_Top_Black;
	class Vp_SET16_Vest_Black;

	class VP_SET16_Boots_Black_vb: VP_SET16_Boots_Black
	{
		scope = 2;
		descriptionShort = "$STR_VBC_Assault_Boots_DescVB";
		vbSet = "set_heavy"; // метка сета для скриптовых баффов (vb_bonuses.c)
		visibilityModifier = 0.8; // незаметность для заражённых
		repairableWithKits[] = {2,3,5,8}; // швейный, кожаный, скотч, эпоксидка
		repairCosts[] = {25,25,25,25};
		soundAttType = "Sneakers"; // тихий шаг для заражённых
		class Protection
		{
			biological = 1;
			chemical = 1;
		};
		// DamageSystem скопирован из VP_SET16_Boots_ColorBase целиком (частичное переопределение заменило бы его), броня усилена
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 500;
					healthLevels[] = {{1,{"set_heavy\set\Boots\data\boots.rvmat"}},{0.7,{"set_heavy\set\Boots\data\boots.rvmat"}},{0.5,{"set_heavy\set\Boots\data\boots_damage.rvmat"}},{0.3,{"set_heavy\set\Boots\data\boots_damage.rvmat"}},{0,{"set_heavy\set\Boots\data\boots_destruct.rvmat"}}};
				};
			};
			class GlobalArmor
			{
				class Melee
				{
					class Health
					{
						damage = 0.6;
					};
					class Blood
					{
						damage = 0.6;
					};
					class Shock
					{
						damage = 0.6;
					};
				};
				class Infected
				{
					class Health
					{
						damage = 0.6;
					};
					class Blood
					{
						damage = 0.6;
					};
					class Shock
					{
						damage = 0.6;
					};
				};
			};
		};
	};
	class Vp_SET16_Gloves_Black_vb: Vp_SET16_Gloves_Black
	{
		scope = 2;
		descriptionShort = "$STR_VBC_Assault_Gloves_DescVB";
		vbSet = "set_heavy"; // метка сета для скриптовых баффов (vb_bonuses.c)
		visibilityModifier = 0.8; // незаметность для заражённых
		repairableWithKits[] = {2,3,5,8}; // швейный, кожаный, скотч, эпоксидка
		repairCosts[] = {25,25,25,25};
		class Protection
		{
			biological = 1;
			chemical = 1;
		};
		// DamageSystem скопирован из Vp_SET16_Gloves_ColorBase целиком (частичное переопределение заменило бы его), броня усилена
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 400;
					healthLevels[] = {{1.0,{"set_heavy\set\Gloves\data\gloves.rvmat"}},{0.7,{"set_heavy\set\Gloves\data\gloves.rvmat"}},{0.5,{"set_heavy\set\Gloves\data\gloves_damage.rvmat"}},{0.3,{"set_heavy\set\Gloves\data\gloves_damage.rvmat"}},{0.0,{"set_heavy\set\Gloves\data\gloves_destruct.rvmat"}}};
				};
			};
			class GlobalArmor
			{
				class Melee
				{
					class Health
					{
						damage = 0.6;
					};
					class Blood
					{
						damage = 0.6;
					};
					class Shock
					{
						damage = 0.6;
					};
				};
				class Infected
				{
					class Health
					{
						damage = 0.6;
					};
					class Blood
					{
						damage = 0.6;
					};
					class Shock
					{
						damage = 0.6;
					};
				};
			};
		};
	};
	class Vp_Set16_Helmet_Black_vb: Vp_Set16_Helmet_Black
	{
		scope = 2;
		descriptionShort = "$STR_VBC_Assault_Helmet_DescVB";
		vbSet = "set_heavy"; // метка сета для скриптовых баффов (vb_bonuses.c)
		visibilityModifier = 0.8; // незаметность для заражённых
		repairableWithKits[] = {2,3,5,8}; // швейный, кожаный, скотч, эпоксидка
		repairCosts[] = {25,25,25,25};
		class Protection
		{
			biological = 1;
			chemical = 1;
		};
		// DamageSystem скопирован из Vp_Set16_Helmet_ColorBase целиком (частичное переопределение заменило бы его), броня усилена
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 400;
					transferToAttachmentsCoef = 0.5;
					healthLevels[] = {{1.0,{"set_heavy\set\Helmet\data\helmet.rvmat","set_heavy\set\Helmet\data\krep_pnv.rvmat","set_heavy\set\Helmet\data\krep_fanar.rvmat","set_heavy\set\Helmet\data\mask.rvmat","set_heavy\set\Helmet\data\ochki.rvmat","set_heavy\set\Helmet\data\tkan.rvmat","set_heavy\set\Helmet\data\ushi.rvmat","set_heavy\set\Helmet\data\vizor.rvmat","set_heavy\set\Helmet\data\zad.rvmat"}},{0.7,{"set_heavy\set\Helmet\data\helmet.rvmat","set_heavy\set\Helmet\data\krep_pnv.rvmat","set_heavy\set\Helmet\data\krep_fanar.rvmat","set_heavy\set\Helmet\data\mask.rvmat","set_heavy\set\Helmet\data\ochki.rvmat","set_heavy\set\Helmet\data\tkan.rvmat","set_heavy\set\Helmet\data\ushi.rvmat","set_heavy\set\Helmet\data\vizor.rvmat","set_heavy\set\Helmet\data\zad.rvmat"}},{0.5,{"set_heavy\set\Helmet\data\helmet_damage.rvmat","set_heavy\set\Helmet\data\krep_pnv_damage.rvmat","set_heavy\set\Helmet\data\krep_fanar_damage.rvmat","set_heavy\set\Helmet\data\mask_damage.rvmat","set_heavy\set\Helmet\data\ochki_damage.rvmat","set_heavy\set\Helmet\data\tkan_damage.rvmat","set_heavy\set\Helmet\data\ushi_damage.rvmat","set_heavy\set\Helmet\data\vizor_damage.rvmat","set_heavy\set\Helmet\data\zad_damage.rvmat"}},{0.3,{"set_heavy\set\Helmet\data\helmet_damage.rvmat","set_heavy\set\Helmet\data\krep_pnv_damage.rvmat","set_heavy\set\Helmet\data\krep_fanar_damage.rvmat","set_heavy\set\Helmet\data\mask_damage.rvmat","set_heavy\set\Helmet\data\ochki_damage.rvmat","set_heavy\set\Helmet\data\tkan_damage.rvmat","set_heavy\set\Helmet\data\ushi_damage.rvmat","set_heavy\set\Helmet\data\vizor_damage.rvmat","set_heavy\set\Helmet\data\zad_damage.rvmat"}},{0.0,{"set_heavy\set\Helmet\data\helmet_destruct.rvmat","set_heavy\set\Helmet\data\krep_pnv_destruct.rvmat","set_heavy\set\Helmet\data\krep_fanar_destruct.rvmat","set_heavy\set\Helmet\data\mask_destruct.rvmat","set_heavy\set\Helmet\data\ochki_destruct.rvmat","set_heavy\set\Helmet\data\tkan_destruct.rvmat","set_heavy\set\Helmet\data\ushi_destruct.rvmat","set_heavy\set\Helmet\data\vizor_destruct.rvmat","set_heavy\set\Helmet\data\zad_destruct.rvmat"}}};
				};
			};
			class GlobalArmor
			{
				class Projectile
				{
					class Health
					{
						damage = 0.1;
					};
					class Blood
					{
						damage = 0;
					};
					class Shock
					{
						damage = 0.25;
					};
				};
				class Melee
				{
					class Health
					{
						damage = 0.25;
					};
					class Blood
					{
						damage = 0;
					};
					class Shock
					{
						damage = 0.1;
					};
				};
				class Infected
				{
					class Health
					{
						damage = 0.25;
					};
					class Blood
					{
						damage = 0;
					};
					class Shock
					{
						damage = 0.25;
					};
				};
				class FragGrenade
				{
					class Health
					{
						damage = 0.1;
					};
					class Blood
					{
						damage = 0;
					};
					class Shock
					{
						damage = 0.1;
					};
				};
			};
		};
	};
	class Vp_SET16_Pants_Black_vb: Vp_SET16_Pants_Black
	{
		scope = 2;
		descriptionShort = "$STR_VBC_Assault_Pants_DescVB";
		vbSet = "set_heavy"; // метка сета для скриптовых баффов (vb_bonuses.c)
		visibilityModifier = 0.8; // незаметность для заражённых
		repairableWithKits[] = {2,3,5,8}; // швейный, кожаный, скотч, эпоксидка
		repairCosts[] = {25,25,25,25};
		class Protection
		{
			biological = 1;
			chemical = 1;
		};
		itemsCargoSize[] = {10,7};
		// DamageSystem скопирован из Vp_SET16_Pants_ColorBase целиком (частичное переопределение заменило бы его), броня усилена
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 500;
					transferToAttachmentsCoef = 0.5;
					healthLevels[] = {{1.0,{"set_heavy\set\Pants\data\fast.rvmat","set_heavy\set\Pants\data\fast_krep.rvmat","set_heavy\set\Pants\data\fast2.rvmat","set_heavy\set\Pants\data\karmani_na.rvmat","set_heavy\set\Pants\data\kobura.rvmat","set_heavy\set\Pants\data\koleno.rvmat","set_heavy\set\Pants\data\krep_magaz.rvmat","set_heavy\set\Pants\data\magaz.rvmat","set_heavy\set\Pants\data\pants.rvmat","set_heavy\set\Pants\data\podkladka.rvmat"}},{0.7,{"set_heavy\set\Pants\data\fast.rvmat","set_heavy\set\Pants\data\fast_krep.rvmat","set_heavy\set\Pants\data\fast2.rvmat","set_heavy\set\Pants\data\karmani_na.rvmat","set_heavy\set\Pants\data\kobura.rvmat","set_heavy\set\Pants\data\koleno.rvmat","set_heavy\set\Pants\data\krep_magaz.rvmat","set_heavy\set\Pants\data\magaz.rvmat","set_heavy\set\Pants\data\pants.rvmat","set_heavy\set\Pants\data\podkladka.rvmat"}},{0.5,{"set_heavy\set\Pants\data\fast_damage.rvmat","set_heavy\set\Pants\data\fast_krep_damage.rvmat","set_heavy\set\Pants\data\fast2_damage.rvmat","set_heavy\set\Pants\data\karmani_na_damage.rvmat","set_heavy\set\Pants\data\kobura_damage.rvmat","set_heavy\set\Pants\data\koleno_damage.rvmat","set_heavy\set\Pants\data\krep_magaz_damage.rvmat","set_heavy\set\Pants\data\magaz_damage.rvmat","set_heavy\set\Pants\data\pants_damage.rvmat","set_heavy\set\Pants\data\podkladka_damage.rvmat"}},{0.3,{"set_heavy\set\Pants\data\fast_damage.rvmat","set_heavy\set\Pants\data\fast_krep_damage.rvmat","set_heavy\set\Pants\data\fast2_damage.rvmat","set_heavy\set\Pants\data\karmani_na_damage.rvmat","set_heavy\set\Pants\data\kobura_damage.rvmat","set_heavy\set\Pants\data\koleno_damage.rvmat","set_heavy\set\Pants\data\krep_magaz_damage.rvmat","set_heavy\set\Pants\data\magaz_damage.rvmat","set_heavy\set\Pants\data\pants_damage.rvmat","set_heavy\set\Pants\data\podkladka_damage.rvmat"}},{0.0,{"set_heavy\set\Pants\data\fast_destruct.rvmat","set_heavy\set\Pants\data\fast_krep_destruct.rvmat","set_heavy\set\Pants\data\fast2_destruct.rvmat","set_heavy\set\Pants\data\karmani_na_destruct.rvmat","set_heavy\set\Pants\data\kobura_destruct.rvmat","set_heavy\set\Pants\data\koleno_destruct.rvmat","set_heavy\set\Pants\data\krep_magaz_destruct.rvmat","set_heavy\set\Pants\data\magaz_destruct.rvmat","set_heavy\set\Pants\data\pants_destruct.rvmat","set_heavy\set\Pants\data\podkladka_destruct.rvmat"}}};
				};
			};
			class GlobalArmor
			{
				class Projectile
				{
					class Health
					{
						damage = 0.75;
					};
					class Blood
					{
						damage = 0.75;
					};
					class Shock
					{
						damage = 0.75;
					};
				};
				class Melee
				{
					class Health
					{
						damage = 0.6;
					};
					class Blood
					{
						damage = 0.6;
					};
					class Shock
					{
						damage = 0.6;
					};
				};
				class Infected
				{
					class Health
					{
						damage = 0.6;
					};
					class Blood
					{
						damage = 0.6;
					};
					class Shock
					{
						damage = 0.6;
					};
				};
				class FragGrenade
				{
					class Health
					{
						damage = 0.75;
					};
					class Blood
					{
						damage = 0.75;
					};
					class Shock
					{
						damage = 0.75;
					};
				};
			};
		};
	};
	class Vp_SET16_Top_Black_vb: Vp_SET16_Top_Black
	{
		scope = 2;
		descriptionShort = "$STR_VBC_Assault_Top_DescVB";
		vbSet = "set_heavy"; // метка сета для скриптовых баффов (vb_bonuses.c)
		visibilityModifier = 0.8; // незаметность для заражённых
		repairableWithKits[] = {2,3,5,8}; // швейный, кожаный, скотч, эпоксидка
		repairCosts[] = {25,25,25,25};
		class Protection
		{
			biological = 1;
			chemical = 1;
		};
		itemsCargoSize[] = {10,8};
		// DamageSystem скопирован из Vp_SET16_Top_ColorBase целиком (частичное переопределение заменило бы его), броня усилена
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 500;
					transferToAttachmentsCoef = 0.5;
					healthLevels[] = {{1.0,{"set_heavy\set\Top\data\lokot.rvmat","set_heavy\set\Top\data\naplechniki.rvmat","set_heavy\set\Top\data\predpleshie.rvmat","set_heavy\set\Top\data\top.rvmat"}},{0.7,{"set_heavy\set\Top\data\lokot.rvmat","set_heavy\set\Top\data\naplechniki.rvmat","set_heavy\set\Top\data\predpleshie.rvmat","set_heavy\set\Top\data\top.rvmat"}},{0.5,{"set_heavy\set\Top\data\lokot_damage.rvmat","set_heavy\set\Top\data\naplechniki_damage.rvmat","set_heavy\set\Top\data\predpleshie_damage.rvmat","set_heavy\set\Top\data\top_damage.rvmat"}},{0.3,{"set_heavy\set\Top\data\lokot_damage.rvmat","set_heavy\set\Top\data\naplechniki_damage.rvmat","set_heavy\set\Top\data\predpleshie_damage.rvmat","set_heavy\set\Top\data\top_damage.rvmat"}},{0.0,{"set_heavy\set\Top\data\lokot_destruct.rvmat","set_heavy\set\Top\data\naplechniki_destruct.rvmat","set_heavy\set\Top\data\predpleshie_destruct.rvmat","set_heavy\set\Top\data\top_destruct.rvmat"}}};
				};
			};
			class GlobalArmor
			{
				class Projectile
				{
					class Health
					{
						damage = 0.75;
					};
					class Blood
					{
						damage = 0.75;
					};
					class Shock
					{
						damage = 0.75;
					};
				};
				class Melee
				{
					class Health
					{
						damage = 0.6;
					};
					class Blood
					{
						damage = 0.6;
					};
					class Shock
					{
						damage = 0.6;
					};
				};
				class Infected
				{
					class Health
					{
						damage = 0.6;
					};
					class Blood
					{
						damage = 0.6;
					};
					class Shock
					{
						damage = 0.6;
					};
				};
				class FragGrenade
				{
					class Health
					{
						damage = 0.75;
					};
					class Blood
					{
						damage = 0.75;
					};
					class Shock
					{
						damage = 0.75;
					};
				};
			};
		};
	};
	class Vp_SET16_Vest_Black_vb: Vp_SET16_Vest_Black
	{
		scope = 2;
		descriptionShort = "$STR_VBC_Assault_Vest_DescVB";
		vbSet = "set_heavy"; // метка сета для скриптовых баффов (vb_bonuses.c)
		visibilityModifier = 0.8; // незаметность для заражённых
		repairableWithKits[] = {2,3,5,8}; // швейный, кожаный, скотч, эпоксидка
		repairCosts[] = {25,25,25,25};
		class Protection
		{
			biological = 1;
			chemical = 1;
		};
		itemsCargoSize[] = {10,6};
		// DamageSystem скопирован из Vp_SET16_Vest_ColorBase целиком (частичное переопределение заменило бы его), броня усилена
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 800;
					transferToAttachmentsCoef = 0.5;
					healthLevels[] = {{1.0,{"set_heavy\set\Vest\data\karabin.rvmat","set_heavy\set\Vest\data\pah.rvmat","set_heavy\set\Vest\data\vest.rvmat","set_heavy\set\Vest\data\vest_relief.rvmat","set_heavy\set\Vest\data\vorotnik.rvmat"}},{0.7,{"set_heavy\set\Vest\data\karabin.rvmat","set_heavy\set\Vest\data\pah.rvmat","set_heavy\set\Vest\data\vest.rvmat","set_heavy\set\Vest\data\vest_relief.rvmat","set_heavy\set\Vest\data\vorotnik.rvmat"}},{0.5,{"set_heavy\set\Vest\data\karabin_damage.rvmat","set_heavy\set\Vest\data\pah_damage.rvmat","set_heavy\set\Vest\data\vest_damage.rvmat","set_heavy\set\Vest\data\vest_relief_damage.rvmat","set_heavy\set\Vest\data\vorotnik_damage.rvmat"}},{0.3,{"set_heavy\set\Vest\data\karabin_damage.rvmat","set_heavy\set\Vest\data\pah_damage.rvmat","set_heavy\set\Vest\data\vest_damage.rvmat","set_heavy\set\Vest\data\vest_relief_damage.rvmat","set_heavy\set\Vest\data\vorotnik_damage.rvmat"}},{0.0,{"set_heavy\set\Vest\data\karabin_destruct.rvmat","set_heavy\set\Vest\data\pah_destruct.rvmat","set_heavy\set\Vest\data\vest_destruct.rvmat","set_heavy\set\Vest\data\vest_relief_destruct.rvmat","set_heavy\set\Vest\data\vorotnik_destruct.rvmat"}}};
				};
			};
			class GlobalArmor
			{
				class Projectile
				{
					class Health
					{
						damage = 0.15;
					};
					class Blood
					{
						damage = 0;
					};
					class Shock
					{
						damage = 0.45;
					};
				};
				class Melee
				{
					class Health
					{
						damage = 0.2;
					};
					class Blood
					{
						damage = 0;
					};
					class Shock
					{
						damage = 0.2;
					};
				};
				class Infected
				{
					class Health
					{
						damage = 0.2;
					};
					class Blood
					{
						damage = 0;
					};
					class Shock
					{
						damage = 0.2;
					};
				};
				class FragGrenade
				{
					class Health
					{
						damage = 0.35;
					};
					class Blood
					{
						damage = 0;
					};
					class Shock
					{
						damage = 0.35;
					};
				};
			};
		};
	};
};
