// DayZavr: улучшенные версии предметов сета (классы *_vb).
// Каждый X_vb наследует X целиком: вид, название, описание, скрипты. Параметры меняем только здесь.

class CfgPatches
{
	class set_Elegant_vb
	{
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"Vp_SET15"};
	};
};

class CfgVehicles
{
	class VP_SET15_Belt_Black;
	class VP_SET15_Shoes_Black;
	class Vp_SET15_Gloves_Black;
	class Vp_Set15_Mask_Black;
	class Vp_SET15_Pants_Black;
	class Vp_SET15_Top_Black;
	class Vp_SET15_Vest_Black;

	class VP_SET15_Belt_Black_vb: VP_SET15_Belt_Black
	{
		scope = 2;
		descriptionShort = "$STR_VBC_Moriarty_Belt_DescVB";
		vbSet = "set_Elegant"; // метка сета для скриптовых баффов (vb_bonuses.c)
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
	class VP_SET15_Shoes_Black_vb: VP_SET15_Shoes_Black
	{
		scope = 2;
		descriptionShort = "$STR_VBC_Moriarty_Shoes_DescVB";
		vbSet = "set_Elegant"; // метка сета для скриптовых баффов (vb_bonuses.c)
		visibilityModifier = 0.8; // незаметность для заражённых
		repairableWithKits[] = {2,3,5,8}; // швейный, кожаный, скотч, эпоксидка
		repairCosts[] = {25,25,25,25};
		soundAttType = "Sneakers"; // тихий шаг для заражённых
		class Protection
		{
			biological = 1;
			chemical = 1;
		};
		// DamageSystem скопирован из VP_SET15_Shoes_ColorBase целиком (частичное переопределение заменило бы его), броня усилена
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 500;
					healthLevels[] = {{1,{"set_Elegant\set\Boots\data\Boots.rvmat"}},{0.7,{"set_Elegant\set\Boots\data\Boots.rvmat"}},{0.5,{"set_Elegant\set\Boots\data\Boots_damage.rvmat"}},{0.3,{"set_Elegant\set\Boots\data\Boots_damage.rvmat"}},{0,{"set_Elegant\set\Boots\data\Boots_destruct.rvmat"}}};
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
	class Vp_SET15_Gloves_Black_vb: Vp_SET15_Gloves_Black
	{
		scope = 2;
		descriptionShort = "$STR_VBC_Moriarty_Gloves_DescVB";
		vbSet = "set_Elegant"; // метка сета для скриптовых баффов (vb_bonuses.c)
		visibilityModifier = 0.8; // незаметность для заражённых
		repairableWithKits[] = {2,3,5,8}; // швейный, кожаный, скотч, эпоксидка
		repairCosts[] = {25,25,25,25};
		class Protection
		{
			biological = 1;
			chemical = 1;
		};
		// DamageSystem скопирован из Vp_SET15_Gloves_ColorBase целиком (частичное переопределение заменило бы его), броня усилена
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 400;
					healthLevels[] = {{1.0,{"set_Elegant\set\Gloves\data\Gloves.rvmat"}},{0.7,{"set_Elegant\set\Gloves\data\Gloves.rvmat"}},{0.5,{"set_Elegant\set\Gloves\data\Gloves_damage.rvmat"}},{0.3,{"set_Elegant\set\Gloves\data\Gloves_damage.rvmat"}},{0.0,{"set_Elegant\set\Gloves\data\Gloves_destruct.rvmat"}}};
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
	class Vp_Set15_Mask_Black_vb: Vp_Set15_Mask_Black
	{
		scope = 2;
		descriptionShort = "$STR_VBC_Moriarty_Mask_DescVB";
		vbSet = "set_Elegant"; // метка сета для скриптовых баффов (vb_bonuses.c)
		visibilityModifier = 0.8; // незаметность для заражённых
		repairableWithKits[] = {2,3,5,8}; // швейный, кожаный, скотч, эпоксидка
		repairCosts[] = {25,25,25,25};
		class Protection
		{
			biological = 1;
			chemical = 1;
		};
	};
	class Vp_SET15_Pants_Black_vb: Vp_SET15_Pants_Black
	{
		scope = 2;
		descriptionShort = "$STR_VBC_Moriarty_Pants_DescVB";
		vbSet = "set_Elegant"; // метка сета для скриптовых баффов (vb_bonuses.c)
		visibilityModifier = 0.8; // незаметность для заражённых
		repairableWithKits[] = {2,3,5,8}; // швейный, кожаный, скотч, эпоксидка
		repairCosts[] = {25,25,25,25};
		class Protection
		{
			biological = 1;
			chemical = 1;
		};
		itemsCargoSize[] = {10,7};
		// DamageSystem скопирован из Vp_SET15_Pants_ColorBase целиком (частичное переопределение заменило бы его), броня усилена
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 500;
					transferToAttachmentsCoef = 0.5;
					healthLevels[] = {{1.0,{"set_Elegant\set\Pants\data\koleno.rvmat","set_Elegant\set\Pants\data\Pants.rvmat"}},{0.7,{"set_Elegant\set\Pants\data\koleno.rvmat","set_Elegant\set\Pants\data\Pants.rvmat"}},{0.5,{"set_Elegant\set\Pants\data\koleno_damage.rvmat","set_Elegant\set\Pants\data\Pants_damage.rvmat"}},{0.3,{"set_Elegant\set\Pants\data\koleno_damage.rvmat","set_Elegant\set\Pants\data\Pants_damage.rvmat"}},{0.0,{"set_Elegant\set\Pants\data\koleno_destruct.rvmat","set_Elegant\set\Pants\data\Pants_destruct.rvmat"}}};
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
	class Vp_SET15_Top_Black_vb: Vp_SET15_Top_Black
	{
		scope = 2;
		descriptionShort = "$STR_VBC_Moriarty_Top_DescVB";
		vbSet = "set_Elegant"; // метка сета для скриптовых баффов (vb_bonuses.c)
		visibilityModifier = 0.8; // незаметность для заражённых
		repairableWithKits[] = {2,3,5,8}; // швейный, кожаный, скотч, эпоксидка
		repairCosts[] = {25,25,25,25};
		class Protection
		{
			biological = 1;
			chemical = 1;
		};
		itemsCargoSize[] = {10,8};
		// DamageSystem скопирован из Vp_SET15_Top_ColorBase целиком (частичное переопределение заменило бы его), броня усилена
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 500;
					transferToAttachmentsCoef = 0.5;
					healthLevels[] = {{1.0,{"set_Elegant\set\Top\data\galstuk.rvmat","set_Elegant\set\Top\data\lokot.rvmat","set_Elegant\set\Top\data\top1.rvmat","set_Elegant\set\Top\data\top2.rvmat"}},{0.7,{"set_Elegant\set\Top\data\galstuk.rvmat","set_Elegant\set\Top\data\lokot.rvmat","set_Elegant\set\Top\data\top1.rvmat","set_Elegant\set\Top\data\top2.rvmat"}},{0.5,{"set_Elegant\set\Top\data\galstuk_damage.rvmat","set_Elegant\set\Top\data\lokot_damage.rvmat","set_Elegant\set\Top\data\top1_damage.rvmat","set_Elegant\set\Top\data\top2_damage.rvmat"}},{0.3,{"set_Elegant\set\Top\data\galstuk_damage.rvmat","set_Elegant\set\Top\data\lokot_damage.rvmat","set_Elegant\set\Top\data\top1_damage.rvmat","set_Elegant\set\Top\data\top2_damage.rvmat"}},{0.0,{"set_Elegant\set\Top\data\galstuk_destruct.rvmat","set_Elegant\set\Top\data\lokot_destruct.rvmat","set_Elegant\set\Top\data\top1_destruct.rvmat","set_Elegant\set\Top\data\top2_destruct.rvmat"}}};
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
	class Vp_SET15_Vest_Black_vb: Vp_SET15_Vest_Black
	{
		scope = 2;
		descriptionShort = "$STR_VBC_Moriarty_Vest_DescVB";
		vbSet = "set_Elegant"; // метка сета для скриптовых баффов (vb_bonuses.c)
		visibilityModifier = 0.8; // незаметность для заражённых
		repairableWithKits[] = {2,3,5,8}; // швейный, кожаный, скотч, эпоксидка
		repairCosts[] = {25,25,25,25};
		class Protection
		{
			biological = 1;
			chemical = 1;
		};
		itemsCargoSize[] = {10,6};
		// DamageSystem скопирован из Vp_SET15_Vest_ColorBase целиком (частичное переопределение заменило бы его), броня усилена
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 800;
					transferToAttachmentsCoef = 0.5;
					healthLevels[] = {{1.0,{"set_Elegant\set\Vest\data\karman.rvmat","set_Elegant\set\Vest\data\Magaz.rvmat","set_Elegant\set\Vest\data\planshet.rvmat","set_Elegant\set\Vest\data\vest.rvmat"}},{0.7,{"set_Elegant\set\Vest\data\karman.rvmat","set_Elegant\set\Vest\data\Magaz.rvmat","set_Elegant\set\Vest\data\planshet.rvmat","set_Elegant\set\Vest\data\vest.rvmat"}},{0.5,{"set_Elegant\set\Vest\data\karman_damage.rvmat","set_Elegant\set\Vest\data\Magaz_damage.rvmat","set_Elegant\set\Vest\data\planshet_damage.rvmat","set_Elegant\set\Vest\data\vest_damage.rvmat"}},{0.3,{"set_Elegant\set\Vest\data\karman_damage.rvmat","set_Elegant\set\Vest\data\Magaz_damage.rvmat","set_Elegant\set\Vest\data\planshet_damage.rvmat","set_Elegant\set\Vest\data\vest_damage.rvmat"}},{0.0,{"set_Elegant\set\Vest\data\karman_destruct.rvmat","set_Elegant\set\Vest\data\Magaz_destruct.rvmat","set_Elegant\set\Vest\data\planshet_destruct.rvmat","set_Elegant\set\Vest\data\vest_destruct.rvmat"}}};
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
