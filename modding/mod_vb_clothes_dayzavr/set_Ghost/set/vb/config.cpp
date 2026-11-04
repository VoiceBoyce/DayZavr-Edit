// DayZavr: улучшенные версии предметов сета (классы *_vb).
// Каждый X_vb наследует X целиком: вид, название, описание, скрипты. Параметры меняем только здесь.

class CfgPatches
{
	class set_Ghost_vb
	{
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"Vp_SET14","Vp_Set11"};
	};
};

class CfgVehicles
{
	class VP_SET14_Boots_Black;
	class VP_SET14_Gloves_Black;
	class Vp_Set14_Helmet;
	class Vp_SET14_Pants_Black;
	class Vp_SET14_Top_Black;
	class Vp_SET14_Vest_Black;

	class VP_SET14_Boots_Black_vb: VP_SET14_Boots_Black
	{
		scope = 2;
		descriptionShort = "$STR_VBC_Ghost_Boots_DescVB";
		vbSet = "set_Ghost"; // метка сета для скриптовых баффов (vb_bonuses.c)
		visibilityModifier = 0.8; // незаметность для заражённых
		repairableWithKits[] = {2,3,5,8}; // швейный, кожаный, скотч, эпоксидка
		repairCosts[] = {25,25,25,25};
		soundAttType = "Sneakers"; // тихий шаг для заражённых
		class Protection
		{
			biological = 1;
			chemical = 1;
		};
		// DamageSystem скопирован из VP_SET14_Shoes_ColorBase целиком (частичное переопределение заменило бы его), броня усилена
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 500;
					healthLevels[] = {{1,{"set_Ghost\set\Boots\data\Boots.rvmat"}},{0.7,{"set_Ghost\set\Boots\data\Boots.rvmat"}},{0.5,{"set_Ghost\set\Boots\data\Boots_damage.rvmat"}},{0.3,{"set_Ghost\set\Boots\data\Boots_damage.rvmat"}},{0,{"set_Ghost\set\Boots\data\Boots_destruct.rvmat"}}};
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
	class VP_SET14_Gloves_Black_vb: VP_SET14_Gloves_Black
	{
		scope = 2;
		descriptionShort = "$STR_VBC_Ghost_Gloves_DescVB";
		vbSet = "set_Ghost"; // метка сета для скриптовых баффов (vb_bonuses.c)
		visibilityModifier = 0.8; // незаметность для заражённых
		repairableWithKits[] = {2,3,5,8}; // швейный, кожаный, скотч, эпоксидка
		repairCosts[] = {25,25,25,25};
		class Protection
		{
			biological = 1;
			chemical = 1;
		};
		// DamageSystem скопирован из Vp_SET14_Gloves_ColorBase целиком (частичное переопределение заменило бы его), броня усилена
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 400;
					healthLevels[] = {{1.0,{"set_Ghost\set\Gloves\data\Gloves.rvmat"}},{0.7,{"set_Ghost\set\Gloves\data\Gloves.rvmat"}},{0.5,{"set_Ghost\set\Gloves\data\Gloves_damage.rvmat"}},{0.3,{"set_Ghost\set\Gloves\data\Gloves_damage.rvmat"}},{0.0,{"set_Ghost\set\Gloves\data\Gloves_destruct.rvmat"}}};
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
	class Vp_Set14_Helmet_vb: Vp_Set14_Helmet
	{
		scope = 2;
		descriptionShort = "$STR_VBC_Ghost_Helmet_DescVB";
		vbSet = "set_Ghost"; // метка сета для скриптовых баффов (vb_bonuses.c)
		visibilityModifier = 0.8; // незаметность для заражённых
		repairableWithKits[] = {2,3,5,8}; // швейный, кожаный, скотч, эпоксидка
		repairCosts[] = {25,25,25,25};
		class Protection
		{
			biological = 1;
			chemical = 1;
		};
		// DamageSystem скопирован из Vp_Set14_Helmet_ColorBase целиком (частичное переопределение заменило бы его), броня усилена
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 400;
					transferToAttachmentsCoef = 0.5;
					healthLevels[] = {{1.0,{"set_Ghost\set\Helmet\data\Akb_helmet.rvmat","set_Ghost\set\Helmet\data\Ebalo.rvmat","set_Ghost\set\Helmet\data\Fast_helmet.rvmat","set_Ghost\set\Helmet\data\Flag_helmet.rvmat","set_Ghost\set\Helmet\data\Helmet.rvmat","set_Ghost\set\Helmet\data\Mask.rvmat","set_Ghost\set\Helmet\data\Meshok_helmet.rvmat","set_Ghost\set\Helmet\data\Mikro_helmet.rvmat","set_Ghost\set\Helmet\data\Haushnik.rvmat","set_Ghost\set\Helmet\data\PNV_kper.rvmat","set_Ghost\set\Helmet\data\Truba_helmet.rvmat","set_Ghost\set\Helmet\data\Zalypa_Helmet.rvmat","set_Ghost\set\Helmet\data\Zalypa2_helmet.rvmat","set_Ghost\set\Helmet\data\Zapladka_helmet.rvmat"}},{0.7,{"set_Ghost\set\Helmet\data\Akb_helmet.rvmat","set_Ghost\set\Helmet\data\Ebalo.rvmat","set_Ghost\set\Helmet\data\Fast_helmet.rvmat","set_Ghost\set\Helmet\data\Flag_helmet.rvmat","set_Ghost\set\Helmet\data\Helmet.rvmat","set_Ghost\set\Helmet\data\Mask.rvmat","set_Ghost\set\Helmet\data\Meshok_helmet.rvmat","set_Ghost\set\Helmet\data\Mikro_helmet.rvmat","set_Ghost\set\Helmet\data\Haushnik.rvmat","set_Ghost\set\Helmet\data\PNV_kper.rvmat","set_Ghost\set\Helmet\data\Truba_helmet.rvmat","set_Ghost\set\Helmet\data\Zalypa_Helmet.rvmat","set_Ghost\set\Helmet\data\Zalypa2_helmet.rvmat","set_Ghost\set\Helmet\data\Zapladka_helmet.rvmat"}},{0.5,{"set_Ghost\set\Helmet\data\Akb_helmet_damage.rvmat","set_Ghost\set\Helmet\data\Ebalo_damage.rvmat","set_Ghost\set\Helmet\data\Fast_helmet_damage.rvmat","set_Ghost\set\Helmet\data\Flag_helmet_damage.rvmat","set_Ghost\set\Helmet\data\Helmet_damage.rvmat","set_Ghost\set\Helmet\data\Mask_damage.rvmat","set_Ghost\set\Helmet\data\Meshok_helmet_damage.rvmat","set_Ghost\set\Helmet\data\Haushnik_damage.rvmat","set_Ghost\set\Helmet\data\Mikro_helmet_damage.rvmat","set_Ghost\set\Helmet\data\PNV_kper_damage.rvmat","set_Ghost\set\Helmet\data\Truba_helmet_damage.rvmat","set_Ghost\set\Helmet\data\Zalypa_Helmet_damage.rvmat","set_Ghost\set\Helmet\data\Zalypa2_helmet_damage.rvmat","set_Ghost\set\Helmet\data\Zapladka_helmet_damage.rvmat"}},{0.3,{"set_Ghost\set\Helmet\data\Akb_helmet_damage.rvmat","set_Ghost\set\Helmet\data\Ebalo_damage.rvmat","set_Ghost\set\Helmet\data\Fast_helmet_damage.rvmat","set_Ghost\set\Helmet\data\Flag_helmet_damage.rvmat","set_Ghost\set\Helmet\data\Haushnik_damage.rvmat","set_Ghost\set\Helmet\data\Helmet_damage.rvmat","set_Ghost\set\Helmet\data\Mask_damage.rvmat","set_Ghost\set\Helmet\data\Meshok_helmet_damage.rvmat","set_Ghost\set\Helmet\data\Haushnik_damage.rvmat","set_Ghost\set\Helmet\data\Mikro_helmet_damage.rvmat","set_Ghost\set\Helmet\data\PNV_kper_damage.rvmat","set_Ghost\set\Helmet\data\Truba_helmet_damage.rvmat","set_Ghost\set\Helmet\data\Zalypa_Helmet_damage.rvmat","set_Ghost\set\Helmet\data\Zalypa2_helmet_damage.rvmat","set_Ghost\set\Helmet\data\Zapladka_helmet_damage.rvmat"}},{0.0,{"set_Ghost\set\Helmet\data\Akb_helmet_destruct.rvmat","set_Ghost\set\Helmet\data\Ebalo_destruct.rvmat","set_Ghost\set\Helmet\data\Fast_helmet_destruct.rvmat","set_Ghost\set\Helmet\data\Flag_helmet_destruct.rvmat","set_Ghost\set\Helmet\data\Helmet_destruct.rvmat","set_Ghost\set\Helmet\data\Mask_destruct.rvmat","set_Ghost\set\Helmet\data\Meshok_helmet_destruct.rvmat","set_Ghost\set\Helmet\data\Mikro_helmet_destruct.rvmat","set_Ghost\set\Helmet\data\Haushnik_destruct.rvmat","set_Ghost\set\Helmet\data\PNV_kper_destruct.rvmat","set_Ghost\set\Helmet\data\Truba_helmet_destruct.rvmat","set_Ghost\set\Helmet\data\Zalypa_Helmet_destruct.rvmat","set_Ghost\set\Helmet\data\Zalypa2_helmet_destruct.rvmat","set_Ghost\set\Helmet\data\Zapladka_helmet_destruct.rvmat"}}};
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
	class Vp_SET14_Pants_Black_vb: Vp_SET14_Pants_Black
	{
		scope = 2;
		descriptionShort = "$STR_VBC_Ghost_Pants_DescVB";
		vbSet = "set_Ghost"; // метка сета для скриптовых баффов (vb_bonuses.c)
		visibilityModifier = 0.8; // незаметность для заражённых
		repairableWithKits[] = {2,3,5,8}; // швейный, кожаный, скотч, эпоксидка
		repairCosts[] = {25,25,25,25};
		class Protection
		{
			biological = 1;
			chemical = 1;
		};
		itemsCargoSize[] = {10,7};
		// DamageSystem скопирован из Vp_SET14_Pants_ColorBase целиком (частичное переопределение заменило бы его), броня усилена
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 500;
					transferToAttachmentsCoef = 0.5;
					healthLevels[] = {{1.0,{"set_Ghost\set\Pants\data\Pants.rvmat","set_Ghost\set\Pants\data\Fast.rvmat","set_Ghost\set\Pants\data\Kobura.rvmat","set_Ghost\set\Pants\data\Koleno.rvmat"}},{0.7,{"set_Ghost\set\Pants\data\Pants.rvmat","set_Ghost\set\Pants\data\Fast.rvmat","set_Ghost\set\Pants\data\Kobura.rvmat","set_Ghost\set\Pants\data\Koleno.rvmat"}},{0.5,{"set_Ghost\set\Pants\data\Pants_damage.rvmat","set_Ghost\set\Pants\data\Fast_damage.rvmat","set_Ghost\set\Pants\data\Kobura_damage.rvmat","set_Ghost\set\Pants\data\Koleno_damage.rvmat"}},{0.3,{"set_Ghost\set\Pants\data\Pants_damage.rvmat","set_Ghost\set\Pants\data\Fast_damage.rvmat","set_Ghost\set\Pants\data\Kobura_damage.rvmat","set_Ghost\set\Pants\data\Koleno_damage.rvmat"}},{0.0,{"set_Ghost\set\Pants\data\Pants_destruct.rvmat","set_Ghost\set\Pants\data\Fast_destruct.rvmat","set_Ghost\set\Pants\data\Kobura_destruct.rvmat","set_Ghost\set\Pants\data\Koleno_destruct.rvmat"}}};
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
	class Vp_SET14_Top_Black_vb: Vp_SET14_Top_Black
	{
		scope = 2;
		descriptionShort = "$STR_VBC_Ghost_Top_DescVB";
		vbSet = "set_Ghost"; // метка сета для скриптовых баффов (vb_bonuses.c)
		visibilityModifier = 0.8; // незаметность для заражённых
		repairableWithKits[] = {2,3,5,8}; // швейный, кожаный, скотч, эпоксидка
		repairCosts[] = {25,25,25,25};
		class Protection
		{
			biological = 1;
			chemical = 1;
		};
		itemsCargoSize[] = {10,8};
		// DamageSystem скопирован из Vp_SET14_Top_ColorBase целиком (частичное переопределение заменило бы его), броня усилена
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 500;
					transferToAttachmentsCoef = 0.5;
					healthLevels[] = {{1.0,{"set_Ghost\set\Top\data\Top.rvmat"}},{0.7,{"set_Ghost\set\Top\data\Top.rvmat"}},{0.5,{"set_Ghost\set\Top\data\Top_damage.rvmat"}},{0.3,{"set_Ghost\set\Top\data\Top_damage.rvmat"}},{0.0,{"set_Ghost\set\Top\data\Top_destruct.rvmat"}}};
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
	class Vp_SET14_Vest_Black_vb: Vp_SET14_Vest_Black
	{
		scope = 2;
		descriptionShort = "$STR_VBC_Ghost_Vest_DescVB";
		vbSet = "set_Ghost"; // метка сета для скриптовых баффов (vb_bonuses.c)
		visibilityModifier = 0.8; // незаметность для заражённых
		repairableWithKits[] = {2,3,5,8}; // швейный, кожаный, скотч, эпоксидка
		repairCosts[] = {25,25,25,25};
		class Protection
		{
			biological = 1;
			chemical = 1;
		};
		itemsCargoSize[] = {10,6};
		// DamageSystem скопирован из Vp_SET14_Vest_ColorBase целиком (частичное переопределение заменило бы его), броня усилена
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 800;
					transferToAttachmentsCoef = 0.5;
					healthLevels[] = {{1.0,{"set_Ghost\set\Vest\data\Vest1.rvmat","set_Ghost\set\Vest\data\Vest2.rvmat","set_Ghost\set\Vest\data\Vest3.rvmat","set_Ghost\set\Vest\data\Vest4.rvmat","set_Ghost\set\Vest\data\vest6.rvmat","set_Ghost\set\Vest\data\vest7.rvmat","set_Ghost\set\Vest\data\Vest10.rvmat","set_Ghost\set\Vest\data\Vest11.rvmat","set_Ghost\set\Vest\data\Vest12.rvmat","set_Ghost\set\Vest\data\Vest13.rvmat","set_Ghost\set\Vest\data\Vest14.rvmat","set_Ghost\set\Vest\data\Vest15.rvmat","set_Ghost\set\Vest\data\Vest16.rvmat","set_Ghost\set\Vest\data\Vest17.rvmat","set_Ghost\set\Vest\data\Vest19.rvmat","set_Ghost\set\Vest\data\Vest20.rvmat","set_Ghost\set\Vest\data\Vest21.rvmat","set_Ghost\set\Vest\data\Vest22.rvmat","set_Ghost\set\Vest\data\Vest23.rvmat","set_Ghost\set\Vest\data\Vest24.rvmat"}},{0.7,{"set_Ghost\set\Vest\data\Vest1.rvmat","set_Ghost\set\Vest\data\Vest2.rvmat","set_Ghost\set\Vest\data\Vest3.rvmat","set_Ghost\set\Vest\data\Vest4.rvmat","set_Ghost\set\Vest\data\vest6.rvmat","set_Ghost\set\Vest\data\vest7.rvmat","set_Ghost\set\Vest\data\Vest10.rvmat","set_Ghost\set\Vest\data\Vest11.rvmat","set_Ghost\set\Vest\data\Vest12.rvmat","set_Ghost\set\Vest\data\Vest13.rvmat","set_Ghost\set\Vest\data\Vest14.rvmat","set_Ghost\set\Vest\data\Vest15.rvmat","set_Ghost\set\Vest\data\Vest16.rvmat","set_Ghost\set\Vest\data\Vest17.rvmat","set_Ghost\set\Vest\data\Vest19.rvmat","set_Ghost\set\Vest\data\Vest20.rvmat","set_Ghost\set\Vest\data\Vest21.rvmat","set_Ghost\set\Vest\data\Vest22.rvmat","set_Ghost\set\Vest\data\Vest23.rvmat","set_Ghost\set\Vest\data\Vest24.rvmat"}},{0.5,{"set_Ghost\set\Vest\data\Vest1_damage.rvmat","set_Ghost\set\Vest\data\Vest2_damage.rvmat","set_Ghost\set\Vest\data\Vest3_damage.rvmat","set_Ghost\set\Vest\data\Vest4_damage.rvmat","set_Ghost\set\Vest\data\vest6_damage.rvmat","set_Ghost\set\Vest\data\vest7_damage.rvmat","set_Ghost\set\Vest\data\Vest10_damage.rvmat","set_Ghost\set\Vest\data\Vest11_damage.rvmat","set_Ghost\set\Vest\data\Vest12_damage.rvmat","set_Ghost\set\Vest\data\Vest13_damage.rvmat","set_Ghost\set\Vest\data\Vest14_damage.rvmat","set_Ghost\set\Vest\data\Vest15_damage.rvmat","set_Ghost\set\Vest\data\Vest16_damage.rvmat","set_Ghost\set\Vest\data\Vest17_damage.rvmat","set_Ghost\set\Vest\data\Vest19_damage.rvmat","set_Ghost\set\Vest\data\Vest20_damage.rvmat","set_Ghost\set\Vest\data\Vest21_damage.rvmat","set_Ghost\set\Vest\data\Vest22_damage.rvmat","set_Ghost\set\Vest\data\Vest23_damage.rvmat","set_Ghost\set\Vest\data\Vest24_damage.rvmat"}},{0.3,{"set_Ghost\set\Vest\data\Vest1_damage.rvmat","set_Ghost\set\Vest\data\Vest2_damage.rvmat","set_Ghost\set\Vest\data\Vest3_damage.rvmat","set_Ghost\set\Vest\data\Vest4_damage.rvmat","set_Ghost\set\Vest\data\vest6_damage.rvmat","set_Ghost\set\Vest\data\vest7_damage.rvmat","set_Ghost\set\Vest\data\Vest10_damage.rvmat","set_Ghost\set\Vest\data\Vest11_damage.rvmat","set_Ghost\set\Vest\data\Vest12_damage.rvmat","set_Ghost\set\Vest\data\Vest13_damage.rvmat","set_Ghost\set\Vest\data\Vest14_damage.rvmat","set_Ghost\set\Vest\data\Vest15_damage.rvmat","set_Ghost\set\Vest\data\Vest16_damage.rvmat","set_Ghost\set\Vest\data\Vest17_damage.rvmat","set_Ghost\set\Vest\data\Vest19_damage.rvmat","set_Ghost\set\Vest\data\Vest20_damage.rvmat","set_Ghost\set\Vest\data\Vest21_damage.rvmat","set_Ghost\set\Vest\data\Vest22_damage.rvmat","set_Ghost\set\Vest\data\Vest23_damage.rvmat","set_Ghost\set\Vest\data\Vest24_damage.rvmat"}},{0.0,{"set_Ghost\set\Vest\data\Vest1_destruct.rvmat","set_Ghost\set\Vest\data\Vest2_destruct.rvmat","set_Ghost\set\Vest\data\Vest3_destruct.rvmat","set_Ghost\set\Vest\data\Vest4_destruct.rvmat","set_Ghost\set\Vest\data\vest6_destruct.rvmat","set_Ghost\set\Vest\data\vest7_destruct.rvmat","set_Ghost\set\Vest\data\Vest10_destruct.rvmat","set_Ghost\set\Vest\data\Vest11_destruct.rvmat","set_Ghost\set\Vest\data\Vest12_destruct.rvmat","set_Ghost\set\Vest\data\Vest13_destruct.rvmat","set_Ghost\set\Vest\data\Vest14_destruct.rvmat","set_Ghost\set\Vest\data\Vest15_destruct.rvmat","set_Ghost\set\Vest\data\Vest16_destruct.rvmat","set_Ghost\set\Vest\data\Vest17_destruct.rvmat","set_Ghost\set\Vest\data\Vest19_destruct.rvmat","set_Ghost\set\Vest\data\Vest20_destruct.rvmat","set_Ghost\set\Vest\data\Vest21_destruct.rvmat","set_Ghost\set\Vest\data\Vest22_destruct.rvmat","set_Ghost\set\Vest\data\Vest23_destruct.rvmat","set_Ghost\set\Vest\data\Vest24_destruct.rvmat"}}};
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
