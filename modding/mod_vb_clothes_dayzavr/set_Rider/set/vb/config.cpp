// DayZavr: улучшенные версии предметов сета (классы *_vb).
// Каждый X_vb наследует X целиком: вид, название, описание, скрипты. Параметры меняем только здесь.

class CfgPatches
{
	class set_Rider_vb
	{
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"Vp_SET12"};
	};
};

class CfgVehicles
{
	class VP_SET12_Belt_Black;
	class VP_SET12_Shoes_Black;
	class Vp_SET12_Gloves_Black;
	class Vp_SET12_MotoHelmet;
	class Vp_SET12_Pants_Black;
	class Vp_SET12_Top_Black;
	class Vp_SET12_Vest_Black;

	class VP_SET12_Belt_Black_vb: VP_SET12_Belt_Black
	{
		scope = 2;
		descriptionShort = "$STR_VBC_Rider_Belt_DescVB";
		vbSet = "set_Rider"; // метка сета для скриптовых баффов (vb_bonuses.c)
		visibilityModifier = 0.8; // незаметность для заражённых
		repairableWithKits[] = {2,3,5,8}; // швейный, кожаный, скотч, эпоксидка
		repairCosts[] = {25,25,25,25};
		class Protection
		{
			biological = 1;
			chemical = 1;
		};
		itemsCargoSize[] = {10,5};
	};
	class VP_SET12_Shoes_Black_vb: VP_SET12_Shoes_Black
	{
		scope = 2;
		descriptionShort = "$STR_VBC_Rider_Boots_DescVB";
		vbSet = "set_Rider"; // метка сета для скриптовых баффов (vb_bonuses.c)
		visibilityModifier = 0.8; // незаметность для заражённых
		repairableWithKits[] = {2,3,5,8}; // швейный, кожаный, скотч, эпоксидка
		repairCosts[] = {25,25,25,25};
		soundAttType = "Sneakers"; // тихий шаг для заражённых
		class Protection
		{
			biological = 1;
			chemical = 1;
		};
		// DamageSystem скопирован из VP_SET12_Shoes_ColorBase целиком (частичное переопределение заменило бы его), броня усилена
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 500;
					healthLevels[] = {{1,{"set_Rider\set\Boots\data\Boots.rvmat"}},{0.7,{"set_Rider\set\Boots\data\Boots.rvmat"}},{0.5,{"set_Rider\set\Boots\data\Boots_damage.rvmat"}},{0.3,{"set_Rider\set\Boots\data\Boots_damage.rvmat"}},{0,{"set_Rider\set\Boots\data\Boots_destruct.rvmat"}}};
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
	class Vp_SET12_Gloves_Black_vb: Vp_SET12_Gloves_Black
	{
		scope = 2;
		descriptionShort = "$STR_VBC_Rider_Gloves_DescVB";
		vbSet = "set_Rider"; // метка сета для скриптовых баффов (vb_bonuses.c)
		visibilityModifier = 0.8; // незаметность для заражённых
		repairableWithKits[] = {2,3,5,8}; // швейный, кожаный, скотч, эпоксидка
		repairCosts[] = {25,25,25,25};
		class Protection
		{
			biological = 1;
			chemical = 1;
		};
		// DamageSystem скопирован из Vp_SET12_Gloves_ColorBase целиком (частичное переопределение заменило бы его), броня усилена
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 400;
					healthLevels[] = {{1.0,{"set_Rider\set\Gloves\data\Gloves.rvmat"}},{0.7,{"set_Rider\set\Gloves\data\Gloves.rvmat"}},{0.5,{"set_Rider\set\Gloves\data\Gloves_damage.rvmat"}},{0.3,{"set_Rider\set\Gloves\data\Gloves_damage.rvmat"}},{0.0,{"set_Rider\set\Gloves\data\Gloves_destruct.rvmat"}}};
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
	class Vp_SET12_MotoHelmet_vb: Vp_SET12_MotoHelmet
	{
		scope = 2;
		descriptionShort = "$STR_VBC_Rider_Helmet_DescVB";
		vbSet = "set_Rider"; // метка сета для скриптовых баффов (vb_bonuses.c)
		visibilityModifier = 0.8; // незаметность для заражённых
		repairableWithKits[] = {2,3,5,8}; // швейный, кожаный, скотч, эпоксидка
		repairCosts[] = {25,25,25,25};
		class Protection
		{
			biological = 1;
			chemical = 1;
		};
		// DamageSystem скопирован из Vp_SET12_MotoHelmet_ColorBase целиком (частичное переопределение заменило бы его), броня усилена
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 400;
					transferToAttachmentsCoef = 0.5;
					healthLevels[] = {{1.0,{"set_Rider\set\helmet\data\Mask.rvmat","set_Rider\set\helmet\data\Helmet.rvmat","set_Rider\set\helmet\data\Vizor.rvmat","set_Rider\set\helmet\data\Vizor2.rvmat"}},{0.7,{"set_Rider\set\helmet\data\Mask.rvmat","set_Rider\set\helmet\data\Helmet.rvmat","set_Rider\set\helmet\data\Vizor.rvmat","set_Rider\set\helmet\data\Vizor2.rvmat"}},{0.5,{"set_Rider\set\helmet\data\Mask_damage.rvmat","set_Rider\set\helmet\data\Helmet_damage.rvmat","set_Rider\set\helmet\data\Vizor_damage.rvmat","set_Rider\set\helmet\data\Vizor2_damage.rvmat"}},{0.3,{"set_Rider\set\helmet\data\Mask_damage.rvmat","set_Rider\set\helmet\data\Helmet_damage.rvmat","set_Rider\set\helmet\data\Vizor_damage.rvmat","set_Rider\set\helmet\data\Vizor2_damage.rvmat"}},{0.0,{"set_Rider\set\helmet\data\Mask_destruct.rvmat","set_Rider\set\helmet\data\Helmet_destruct.rvmat","set_Rider\set\helmet\data\Vizor_destruct.rvmat","set_Rider\set\helmet\data\Vizor2_destruct.rvmat"}}};
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
	class Vp_SET12_Pants_Black_vb: Vp_SET12_Pants_Black
	{
		scope = 2;
		descriptionShort = "$STR_VBC_Rider_Pants_DescVB";
		vbSet = "set_Rider"; // метка сета для скриптовых баффов (vb_bonuses.c)
		visibilityModifier = 0.8; // незаметность для заражённых
		repairableWithKits[] = {2,3,5,8}; // швейный, кожаный, скотч, эпоксидка
		repairCosts[] = {25,25,25,25};
		class Protection
		{
			biological = 1;
			chemical = 1;
		};
		itemsCargoSize[] = {10,7};
		// DamageSystem скопирован из Vp_SET12_Pants_ColorBase целиком (частичное переопределение заменило бы его), броня усилена
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 500;
					transferToAttachmentsCoef = 0.5;
					healthLevels[] = {{1.0,{"set_Rider\set\Pants\data\Pants.rvmat","set_Rider\set\Pants\data\Koleno.rvmat","set_Rider\set\Pants\data\Rem_koleno.rvmat"}},{0.7,{"set_Rider\set\Pants\data\Pants.rvmat","set_Rider\set\Pants\data\Koleno.rvmat","set_Rider\set\Pants\data\Rem_koleno.rvmat"}},{0.5,{"set_Rider\set\Pants\data\Pants_damage.rvmat","set_Rider\set\Pants\data\Koleno_damage.rvmat","set_Rider\set\Pants\data\Rem_koleno_damage.rvmat"}},{0.3,{"set_Rider\set\Pants\data\Pants_damage.rvmat","set_Rider\set\Pants\data\Koleno_damage.rvmat","set_Rider\set\Pants\data\Rem_koleno_damage.rvmat"}},{0.0,{"set_Rider\set\Pants\data\Pants_destruct.rvmat","set_Rider\set\Pants\data\Koleno_destruct.rvmat","set_Rider\set\Pants\data\Rem_koleno_destruct.rvmat"}}};
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
	class Vp_SET12_Top_Black_vb: Vp_SET12_Top_Black
	{
		scope = 2;
		descriptionShort = "$STR_VBC_Rider_Top_DescVB";
		vbSet = "set_Rider"; // метка сета для скриптовых баффов (vb_bonuses.c)
		visibilityModifier = 0.8; // незаметность для заражённых
		repairableWithKits[] = {2,3,5,8}; // швейный, кожаный, скотч, эпоксидка
		repairCosts[] = {25,25,25,25};
		class Protection
		{
			biological = 1;
			chemical = 1;
		};
		itemsCargoSize[] = {10,8};
		// DamageSystem скопирован из Vp_SET12_Top_ColorBase целиком (частичное переопределение заменило бы его), броня усилена
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 500;
					transferToAttachmentsCoef = 0.5;
					healthLevels[] = {{1.0,{"set_Rider\set\Top\data\Top.rvmat","set_Rider\set\Top\data\Monset_top.rvmat"}},{0.7,{"set_Rider\set\Top\data\Top.rvmat","set_Rider\set\Top\data\Monset_top.rvmat"}},{0.5,{"set_Rider\set\Top\data\Top_damage.rvmat","set_Rider\set\Top\data\Monset_top_damage.rvmat"}},{0.3,{"set_Rider\set\Top\data\Top_damage.rvmat","set_Rider\set\Top\data\Monset_top_damage.rvmat"}},{0.0,{"set_Rider\set\Top\data\Top_destruct.rvmat","set_Rider\set\Top\data\Monset_top_destruct.rvmat"}}};
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
	class Vp_SET12_Vest_Black_vb: Vp_SET12_Vest_Black
	{
		scope = 2;
		descriptionShort = "$STR_VBC_Rider_Vest_DescVB";
		vbSet = "set_Rider"; // метка сета для скриптовых баффов (vb_bonuses.c)
		visibilityModifier = 0.8; // незаметность для заражённых
		repairableWithKits[] = {2,3,5,8}; // швейный, кожаный, скотч, эпоксидка
		repairCosts[] = {25,25,25,25};
		class Protection
		{
			biological = 1;
			chemical = 1;
		};
		itemsCargoSize[] = {10,6};
		// DamageSystem скопирован из Vp_SET12_Vest_ColorBase целиком (частичное переопределение заменило бы его), броня усилена
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 800;
					transferToAttachmentsCoef = 0.5;
					healthLevels[] = {{1.0,{"set_Rider\set\Vest\data\fast.rvmat","set_Rider\set\Vest\data\fast2.rvmat","set_Rider\set\Vest\data\Karman.rvmat","set_Rider\set\Vest\data\Karman_ra.rvmat","set_Rider\set\Vest\data\Monster2.rvmat","set_Rider\set\Vest\data\Plita.rvmat","set_Rider\set\Vest\data\ra.rvmat","set_Rider\set\Vest\data\Verevka.rvmat","set_Rider\set\Vest\data\Vest.rvmat"}},{0.7,{"set_Rider\set\Vest\data\fast.rvmat","set_Rider\set\Vest\data\fast2.rvmat","set_Rider\set\Vest\data\Karman.rvmat","set_Rider\set\Vest\data\Karman_ra.rvmat","set_Rider\set\Vest\data\Monster2.rvmat","set_Rider\set\Vest\data\Plita.rvmat","set_Rider\set\Vest\data\ra.rvmat","set_Rider\set\Vest\data\Verevka.rvmat","set_Rider\set\Vest\data\Vest.rvmat"}},{0.5,{"set_Rider\set\Vest\data\fast_damage.rvmat","set_Rider\set\Vest\data\fast2_damage.rvmat","set_Rider\set\Vest\data\Karman_damage.rvmat","set_Rider\set\Vest\data\Karman_ra_damage.rvmat","set_Rider\set\Vest\data\Monster2_damage.rvmat","set_Rider\set\Vest\data\Plita_damage.rvmat","set_Rider\set\Vest\data\ra_damage.rvmat","set_Rider\set\Vest\data\Verevka_damage.rvmat","set_Rider\set\Vest\data\Vest_damage.rvmat"}},{0.3,{"set_Rider\set\Vest\data\fast_damage.rvmat","set_Rider\set\Vest\data\fast2_damage.rvmat","set_Rider\set\Vest\data\Karman_damage.rvmat","set_Rider\set\Vest\data\Karman_ra_damage.rvmat","set_Rider\set\Vest\data\Monster2_damage.rvmat","set_Rider\set\Vest\data\Plita_damage.rvmat","set_Rider\set\Vest\data\ra_damage.rvmat","set_Rider\set\Vest\data\Verevka_damage.rvmat","set_Rider\set\Vest\data\Vest_damage.rvmat"}},{0.0,{"set_Rider\set\Vest\data\fast_destruct.rvmat","set_Rider\set\Vest\data\fast2_destruct.rvmat","set_Rider\set\Vest\data\Karman_destruct.rvmat","set_Rider\set\Vest\data\Karman_ra_destruct.rvmat","set_Rider\set\Vest\data\Monster2_destruct.rvmat","set_Rider\set\Vest\data\Plita_destruct.rvmat","set_Rider\set\Vest\data\ra_destruct.rvmat","set_Rider\set\Vest\data\Verevka_destruct.rvmat","set_Rider\set\Vest\data\Vest_destruct.rvmat"}},"_destruct"};
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
