// DayZavr: улучшенные версии предметов сета (классы *_vb).
// Каждый X_vb наследует X целиком: вид, название, описание, скрипты. Параметры меняем только здесь.

class CfgPatches
{
	class Set_Oni_vb
	{
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"Vp_SET13"};
	};
};

class CfgVehicles
{
	class Vp_SET13_Belt_Black;
	class VP_SET13_Shoes_Black;
	class Vp_SET13_Gloves_Black;
	class Vp_SET13_Mask_Black;
	class Vp_SET13_Pants_Black;
	class Vp_SET13_Top_Black;
	class Vp_SET13_Vest_Black;

	class Vp_SET13_Belt_Black_vb: Vp_SET13_Belt_Black
	{
		scope = 2;
		descriptionShort = "$STR_VBC_Samurai_Belt_DescVB";
		vbSet = "Set_Oni"; // метка сета для скриптовых баффов (vb_bonuses.c)
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
	class VP_SET13_Shoes_Black_vb: VP_SET13_Shoes_Black
	{
		scope = 2;
		descriptionShort = "$STR_VBC_Samurai_Shoes_DescVB";
		vbSet = "Set_Oni"; // метка сета для скриптовых баффов (vb_bonuses.c)
		visibilityModifier = 0.8; // незаметность для заражённых
		repairableWithKits[] = {2,3,5,8}; // швейный, кожаный, скотч, эпоксидка
		repairCosts[] = {25,25,25,25};
		soundAttType = "Sneakers"; // тихий шаг для заражённых
		class Protection
		{
			biological = 1;
			chemical = 1;
		};
		// DamageSystem скопирован из VP_SET13_Shoes_ColorBase целиком (частичное переопределение заменило бы его), броня усилена
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 500;
					healthLevels[] = {{1,{"Set_Oni\set\Boots\data\Boots.rvmat"}},{0.7,{"Set_Oni\set\Boots\data\Boots.rvmat"}},{0.5,{"Set_Oni\set\Boots\data\Boots_damage.rvmat"}},{0.3,{"Set_Oni\set\Boots\data\Boots_damage.rvmat"}},{0,{"Set_Oni\set\Boots\data\Boots_destruct.rvmat"}}};
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
	class Vp_SET13_Gloves_Black_vb: Vp_SET13_Gloves_Black
	{
		scope = 2;
		descriptionShort = "$STR_VBC_Samurai_Gloves_DescVB";
		vbSet = "Set_Oni"; // метка сета для скриптовых баффов (vb_bonuses.c)
		visibilityModifier = 0.8; // незаметность для заражённых
		repairableWithKits[] = {2,3,5,8}; // швейный, кожаный, скотч, эпоксидка
		repairCosts[] = {25,25,25,25};
		class Protection
		{
			biological = 1;
			chemical = 1;
		};
		// DamageSystem скопирован из Vp_SET13_Gloves_ColorBase целиком (частичное переопределение заменило бы его), броня усилена
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 400;
					healthLevels[] = {{1.0,{"Set_Oni\set\Gloves\data\Gloves.rvmat"}},{0.7,{"Set_Oni\set\Gloves\data\Gloves.rvmat"}},{0.5,{"Set_Oni\set\Gloves\data\Gloves_damage.rvmat"}},{0.3,{"Set_Oni\set\Gloves\data\Gloves_damage.rvmat"}},{0.0,{"Set_Oni\set\Gloves\data\Gloves_destruct.rvmat"}}};
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
	class Vp_SET13_Mask_Black_vb: Vp_SET13_Mask_Black
	{
		scope = 2;
		descriptionShort = "$STR_VBC_Samurai_Mask_DescVB";
		vbSet = "Set_Oni"; // метка сета для скриптовых баффов (vb_bonuses.c)
		visibilityModifier = 0.8; // незаметность для заражённых
		repairableWithKits[] = {2,3,5,8}; // швейный, кожаный, скотч, эпоксидка
		repairCosts[] = {25,25,25,25};
		class Protection
		{
			biological = 1;
			chemical = 1;
		};
	};
	class Vp_SET13_Pants_Black_vb: Vp_SET13_Pants_Black
	{
		scope = 2;
		descriptionShort = "$STR_VBC_Samurai_Pants_DescVB";
		vbSet = "Set_Oni"; // метка сета для скриптовых баффов (vb_bonuses.c)
		visibilityModifier = 0.8; // незаметность для заражённых
		repairableWithKits[] = {2,3,5,8}; // швейный, кожаный, скотч, эпоксидка
		repairCosts[] = {25,25,25,25};
		class Protection
		{
			biological = 1;
			chemical = 1;
		};
		itemsCargoSize[] = {10,7};
		// DamageSystem скопирован из Vp_SET13_Pants_ColorBase целиком (частичное переопределение заменило бы его), броня усилена
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 500;
					transferToAttachmentsCoef = 0.5;
					healthLevels[] = {{1.0,{"Set_Oni\set\Pants\data\Pants.rvmat"}},{0.7,{"Set_Oni\set\Pants\data\Pants.rvmat"}},{0.5,{"Set_Oni\set\Pants\data\Pants_damage.rvmat"}},{0.3,{"Set_Oni\set\Pants\data\Pants_damage.rvmat"}},{0.0,{"Set_Oni\set\Pants\data\Pants_destruct.rvmat"}}};
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
	class Vp_SET13_Top_Black_vb: Vp_SET13_Top_Black
	{
		scope = 2;
		descriptionShort = "$STR_VBC_Samurai_Top_DescVB";
		vbSet = "Set_Oni"; // метка сета для скриптовых баффов (vb_bonuses.c)
		visibilityModifier = 0.8; // незаметность для заражённых
		repairableWithKits[] = {2,3,5,8}; // швейный, кожаный, скотч, эпоксидка
		repairCosts[] = {25,25,25,25};
		class Protection
		{
			biological = 1;
			chemical = 1;
		};
		itemsCargoSize[] = {10,8};
		// DamageSystem скопирован из Vp_SET13_Top_ColorBase целиком (частичное переопределение заменило бы его), броня усилена
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 500;
					transferToAttachmentsCoef = 0.5;
					healthLevels[] = {{1.0,{"Set_Oni\set\Top\data\Top.rvmat"}},{0.7,{"Set_Oni\set\Top\data\Top.rvmat"}},{0.5,{"Set_Oni\set\Top\data\Top_damage.rvmat"}},{0.3,{"Set_Oni\set\Top\data\Top_damage.rvmat"}},{0.0,{"Set_Oni\set\Top\data\Top_destruct.rvmat"}}};
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
	class Vp_SET13_Vest_Black_vb: Vp_SET13_Vest_Black
	{
		scope = 2;
		descriptionShort = "$STR_VBC_Samurai_Vest_DescVB";
		vbSet = "Set_Oni"; // метка сета для скриптовых баффов (vb_bonuses.c)
		visibilityModifier = 0.8; // незаметность для заражённых
		repairableWithKits[] = {2,3,5,8}; // швейный, кожаный, скотч, эпоксидка
		repairCosts[] = {25,25,25,25};
		class Protection
		{
			biological = 1;
			chemical = 1;
		};
		itemsCargoSize[] = {10,6};
		// DamageSystem скопирован из Vp_SET13_Vest_ColorBase целиком (частичное переопределение заменило бы его), броня усилена
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 800;
					transferToAttachmentsCoef = 0.5;
					healthLevels[] = {{1.0,{"Set_Oni\set\Vest\data\Vest.rvmat","Set_Oni\set\Vest\data\Karman.rvmat","Set_Oni\set\Vest\data\Karman_ra.rvmat","Set_Oni\set\Vest\data\Mag.rvmat","Set_Oni\set\Vest\data\ra.rvmat","Set_Oni\set\Vest\data\zalypa_ra.rvmat"}},{0.7,{"Set_Oni\set\Vest\data\Vest.rvmat","Set_Oni\set\Vest\data\Karman.rvmat","Set_Oni\set\Vest\data\Karman_ra.rvmat","Set_Oni\set\Vest\data\Mag.rvmat","Set_Oni\set\Vest\data\ra.rvmat","Set_Oni\set\Vest\data\zalypa_ra.rvmat"}},{0.5,{"Set_Oni\set\Vest\data\Vest_damage.rvmat","Set_Oni\set\Vest\data\Karman_damage.rvmat","Set_Oni\set\Vest\data\Karman_ra_damage.rvmat","Set_Oni\set\Vest\data\Mag_damage.rvmat","Set_Oni\set\Vest\data\ra_damage.rvmat","Set_Oni\set\Vest\data\zalypa_ra_damage.rvmat"}},{0.3,{"Set_Oni\set\Vest\data\Vest_damage.rvmat","Set_Oni\set\Vest\data\Karman_damage.rvmat","Set_Oni\set\Vest\data\Karman_ra_damage.rvmat","Set_Oni\set\Vest\data\Mag_damage.rvmat","Set_Oni\set\Vest\data\ra_damage.rvmat","Set_Oni\set\Vest\data\zalypa_ra_damage.rvmat"}},{0.0,{"Set_Oni\set\Vest\data\Vest_destruct.rvmat","Set_Oni\set\Vest\data\Karman_destruct.rvmat","Set_Oni\set\Vest\data\Karman_ra_destruct.rvmat","Set_Oni\set\Vest\data\Mag_destruct.rvmat","Set_Oni\set\Vest\data\ra_destruct.rvmat","Set_Oni\set\Vest\data\zalypa_ra_destruct.rvmat"}}};
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
