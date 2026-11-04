// DayZavr: улучшенные версии предметов сета (классы *_vb).
// Каждый X_vb наследует X целиком: вид, название, описание, скрипты. Параметры меняем только здесь.

class CfgPatches
{
	class set_cod2_vb
	{
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"Vp_Set6"};
	};
};

class CfgVehicles
{
	class Belt_SET6_Black;
	class Belt_SET6_Silver;
	class Belt_SET6_Silver_Away;
	class VP_Set6_Shoes_Away;
	class VP_Set6_Shoes_Black;
	class VP_Set6_Shoes_Silver;
	class Vp_Set6_Mask_Black;
	class Vp_Set6_Mask_Silver;
	class Vp_Set6_Mask_Silver_Away;
	class Vp_Set6_Pants_Black;
	class Vp_Set6_Pants_Silver;
	class Vp_Set6_Pants_Silver_Away;
	class Vp_Set6_top_Black;
	class Vp_Set6_top_Silver;
	class Vp_Set6_top_silver_Away;
	class Vp_Set6_vest2_Silver;
	class Vp_Set6_vest2_Silver_Away;
	class Vp_Set6_Vest_black;

	class Belt_SET6_Black_vb: Belt_SET6_Black
	{
		scope = 2;
		descriptionShort = "$STR_VBC_King_Belt_DescVB";
		vbSet = "set_cod2"; // метка сета для скриптовых баффов (vb_bonuses.c)
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
	class Belt_SET6_Silver_vb: Belt_SET6_Silver
	{
		scope = 2;
		descriptionShort = "$STR_VBC_King_Belt_DescVB";
		vbSet = "set_cod2"; // метка сета для скриптовых баффов (vb_bonuses.c)
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
	class Belt_SET6_Silver_Away_vb: Belt_SET6_Silver_Away
	{
		scope = 2;
		descriptionShort = "$STR_VBC_King_Belt_DescVB";
		vbSet = "set_cod2"; // метка сета для скриптовых баффов (vb_bonuses.c)
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
	class VP_Set6_Shoes_Away_vb: VP_Set6_Shoes_Away
	{
		scope = 2;
		descriptionShort = "$STR_VBC_King_Shoes_DescVB";
		vbSet = "set_cod2"; // метка сета для скриптовых баффов (vb_bonuses.c)
		visibilityModifier = 0.8; // незаметность для заражённых
		repairableWithKits[] = {2,3,5,8}; // швейный, кожаный, скотч, эпоксидка
		repairCosts[] = {25,25,25,25};
		soundAttType = "Sneakers"; // тихий шаг для заражённых
		class Protection
		{
			biological = 1;
			chemical = 1;
		};
		// DamageSystem скопирован из VP_Set6_Shoes_ColorBase целиком (частичное переопределение заменило бы его), броня усилена
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 500;
					healthLevels[] = {{1,{"set_cod2\set\boots\data\boots.rvmat"}},{0.7,{"set_cod2\set\boots\data\boots.rvmat"}},{0.5,{"set_cod2\set\boots\data\boots_damage.rvmat"}},{0.3,{"set_cod2\set\boots\data\boots_damage.rvmat"}},{0,{"set_cod2\set\boots\data\boots_destruct.rvmat"}}};
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
	class VP_Set6_Shoes_Black_vb: VP_Set6_Shoes_Black
	{
		scope = 2;
		descriptionShort = "$STR_VBC_King_Shoes_DescVB";
		vbSet = "set_cod2"; // метка сета для скриптовых баффов (vb_bonuses.c)
		visibilityModifier = 0.8; // незаметность для заражённых
		repairableWithKits[] = {2,3,5,8}; // швейный, кожаный, скотч, эпоксидка
		repairCosts[] = {25,25,25,25};
		soundAttType = "Sneakers"; // тихий шаг для заражённых
		class Protection
		{
			biological = 1;
			chemical = 1;
		};
		// DamageSystem скопирован из VP_Set6_Shoes_ColorBase целиком (частичное переопределение заменило бы его), броня усилена
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 500;
					healthLevels[] = {{1,{"set_cod2\set\boots\data\boots.rvmat"}},{0.7,{"set_cod2\set\boots\data\boots.rvmat"}},{0.5,{"set_cod2\set\boots\data\boots_damage.rvmat"}},{0.3,{"set_cod2\set\boots\data\boots_damage.rvmat"}},{0,{"set_cod2\set\boots\data\boots_destruct.rvmat"}}};
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
	class VP_Set6_Shoes_Silver_vb: VP_Set6_Shoes_Silver
	{
		scope = 2;
		descriptionShort = "$STR_VBC_King_Shoes_DescVB";
		vbSet = "set_cod2"; // метка сета для скриптовых баффов (vb_bonuses.c)
		visibilityModifier = 0.8; // незаметность для заражённых
		repairableWithKits[] = {2,3,5,8}; // швейный, кожаный, скотч, эпоксидка
		repairCosts[] = {25,25,25,25};
		soundAttType = "Sneakers"; // тихий шаг для заражённых
		class Protection
		{
			biological = 1;
			chemical = 1;
		};
		// DamageSystem скопирован из VP_Set6_Shoes_ColorBase целиком (частичное переопределение заменило бы его), броня усилена
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 500;
					healthLevels[] = {{1,{"set_cod2\set\boots\data\boots.rvmat"}},{0.7,{"set_cod2\set\boots\data\boots.rvmat"}},{0.5,{"set_cod2\set\boots\data\boots_damage.rvmat"}},{0.3,{"set_cod2\set\boots\data\boots_damage.rvmat"}},{0,{"set_cod2\set\boots\data\boots_destruct.rvmat"}}};
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
	class Vp_Set6_Mask_Black_vb: Vp_Set6_Mask_Black
	{
		scope = 2;
		descriptionShort = "$STR_VBC_King_Mask_DescVB";
		vbSet = "set_cod2"; // метка сета для скриптовых баффов (vb_bonuses.c)
		visibilityModifier = 0.8; // незаметность для заражённых
		repairableWithKits[] = {2,3,5,8}; // швейный, кожаный, скотч, эпоксидка
		repairCosts[] = {25,25,25,25};
		class Protection
		{
			biological = 1;
			chemical = 1;
		};
	};
	class Vp_Set6_Mask_Silver_vb: Vp_Set6_Mask_Silver
	{
		scope = 2;
		descriptionShort = "$STR_VBC_King_Mask_DescVB";
		vbSet = "set_cod2"; // метка сета для скриптовых баффов (vb_bonuses.c)
		visibilityModifier = 0.8; // незаметность для заражённых
		repairableWithKits[] = {2,3,5,8}; // швейный, кожаный, скотч, эпоксидка
		repairCosts[] = {25,25,25,25};
		class Protection
		{
			biological = 1;
			chemical = 1;
		};
	};
	class Vp_Set6_Mask_Silver_Away_vb: Vp_Set6_Mask_Silver_Away
	{
		scope = 2;
		descriptionShort = "$STR_VBC_King_Mask_DescVB";
		vbSet = "set_cod2"; // метка сета для скриптовых баффов (vb_bonuses.c)
		visibilityModifier = 0.8; // незаметность для заражённых
		repairableWithKits[] = {2,3,5,8}; // швейный, кожаный, скотч, эпоксидка
		repairCosts[] = {25,25,25,25};
		class Protection
		{
			biological = 1;
			chemical = 1;
		};
	};
	class Vp_Set6_Pants_Black_vb: Vp_Set6_Pants_Black
	{
		scope = 2;
		descriptionShort = "$STR_VBC_King_Pants_DescVB";
		vbSet = "set_cod2"; // метка сета для скриптовых баффов (vb_bonuses.c)
		visibilityModifier = 0.8; // незаметность для заражённых
		repairableWithKits[] = {2,3,5,8}; // швейный, кожаный, скотч, эпоксидка
		repairCosts[] = {25,25,25,25};
		class Protection
		{
			biological = 1;
			chemical = 1;
		};
		itemsCargoSize[] = {10,7};
		// DamageSystem скопирован из Vp_Set6_Pants_ColorBase целиком (частичное переопределение заменило бы его), броня усилена
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 500;
					transferToAttachmentsCoef = 0.5;
					healthLevels[] = {{1.0,{"set_cod2\set\pants\data\pants.rvmat"}},{0.7,{"set_cod2\set\pants\data\pants.rvmat"}},{0.5,{"set_cod2\set\pants\data\pants_damage.rvmat"}},{0.3,{"set_cod2\set\pants\data\pants_damage.rvmat"}},{0.0,{"set_cod2\set\pants\data\pants_destruct.rvmat"}}};
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
	class Vp_Set6_Pants_Silver_vb: Vp_Set6_Pants_Silver
	{
		scope = 2;
		descriptionShort = "$STR_VBC_King_Pants_DescVB";
		vbSet = "set_cod2"; // метка сета для скриптовых баффов (vb_bonuses.c)
		visibilityModifier = 0.8; // незаметность для заражённых
		repairableWithKits[] = {2,3,5,8}; // швейный, кожаный, скотч, эпоксидка
		repairCosts[] = {25,25,25,25};
		class Protection
		{
			biological = 1;
			chemical = 1;
		};
		itemsCargoSize[] = {10,7};
		// DamageSystem скопирован из Vp_Set6_Pants_ColorBase целиком (частичное переопределение заменило бы его), броня усилена
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 500;
					transferToAttachmentsCoef = 0.5;
					healthLevels[] = {{1.0,{"set_cod2\set\pants\data\pants.rvmat"}},{0.7,{"set_cod2\set\pants\data\pants.rvmat"}},{0.5,{"set_cod2\set\pants\data\pants_damage.rvmat"}},{0.3,{"set_cod2\set\pants\data\pants_damage.rvmat"}},{0.0,{"set_cod2\set\pants\data\pants_destruct.rvmat"}}};
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
	class Vp_Set6_Pants_Silver_Away_vb: Vp_Set6_Pants_Silver_Away
	{
		scope = 2;
		descriptionShort = "$STR_VBC_King_Pants_DescVB";
		vbSet = "set_cod2"; // метка сета для скриптовых баффов (vb_bonuses.c)
		visibilityModifier = 0.8; // незаметность для заражённых
		repairableWithKits[] = {2,3,5,8}; // швейный, кожаный, скотч, эпоксидка
		repairCosts[] = {25,25,25,25};
		class Protection
		{
			biological = 1;
			chemical = 1;
		};
		itemsCargoSize[] = {10,7};
		// DamageSystem скопирован из Vp_Set6_Pants_ColorBase целиком (частичное переопределение заменило бы его), броня усилена
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 500;
					transferToAttachmentsCoef = 0.5;
					healthLevels[] = {{1.0,{"set_cod2\set\pants\data\pants.rvmat"}},{0.7,{"set_cod2\set\pants\data\pants.rvmat"}},{0.5,{"set_cod2\set\pants\data\pants_damage.rvmat"}},{0.3,{"set_cod2\set\pants\data\pants_damage.rvmat"}},{0.0,{"set_cod2\set\pants\data\pants_destruct.rvmat"}}};
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
	class Vp_Set6_top_Black_vb: Vp_Set6_top_Black
	{
		scope = 2;
		descriptionShort = "$STR_VBC_King_Top_DescVB";
		vbSet = "set_cod2"; // метка сета для скриптовых баффов (vb_bonuses.c)
		visibilityModifier = 0.8; // незаметность для заражённых
		repairableWithKits[] = {2,3,5,8}; // швейный, кожаный, скотч, эпоксидка
		repairCosts[] = {25,25,25,25};
		class Protection
		{
			biological = 1;
			chemical = 1;
		};
		itemsCargoSize[] = {10,8};
		// DamageSystem скопирован из Vp_Set6_top_ColorBase целиком (частичное переопределение заменило бы его), броня усилена
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 500;
					transferToAttachmentsCoef = 0.5;
					healthLevels[] = {{1.0,{"set_cod2\set\top\data\top.rvmat"}},{0.7,{"set_cod2\set\top\data\top.rvmat"}},{0.5,{"set_cod2\set\top\data\top_damage.rvmat"}},{0.3,{"set_cod2\set\top\data\top_damage.rvmat"}},{0.0,{"set_cod2\set\top\data\top_destruct.rvmat"}}};
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
	class Vp_Set6_top_Silver_vb: Vp_Set6_top_Silver
	{
		scope = 2;
		descriptionShort = "$STR_VBC_King_Top_DescVB";
		vbSet = "set_cod2"; // метка сета для скриптовых баффов (vb_bonuses.c)
		visibilityModifier = 0.8; // незаметность для заражённых
		repairableWithKits[] = {2,3,5,8}; // швейный, кожаный, скотч, эпоксидка
		repairCosts[] = {25,25,25,25};
		class Protection
		{
			biological = 1;
			chemical = 1;
		};
		itemsCargoSize[] = {10,8};
		// DamageSystem скопирован из Vp_Set6_top_ColorBase целиком (частичное переопределение заменило бы его), броня усилена
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 500;
					transferToAttachmentsCoef = 0.5;
					healthLevels[] = {{1.0,{"set_cod2\set\top\data\top.rvmat"}},{0.7,{"set_cod2\set\top\data\top.rvmat"}},{0.5,{"set_cod2\set\top\data\top_damage.rvmat"}},{0.3,{"set_cod2\set\top\data\top_damage.rvmat"}},{0.0,{"set_cod2\set\top\data\top_destruct.rvmat"}}};
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
	class Vp_Set6_top_silver_Away_vb: Vp_Set6_top_silver_Away
	{
		scope = 2;
		descriptionShort = "$STR_VBC_King_Top_DescVB";
		vbSet = "set_cod2"; // метка сета для скриптовых баффов (vb_bonuses.c)
		visibilityModifier = 0.8; // незаметность для заражённых
		repairableWithKits[] = {2,3,5,8}; // швейный, кожаный, скотч, эпоксидка
		repairCosts[] = {25,25,25,25};
		class Protection
		{
			biological = 1;
			chemical = 1;
		};
		itemsCargoSize[] = {10,8};
		// DamageSystem скопирован из Vp_Set6_top_ColorBase целиком (частичное переопределение заменило бы его), броня усилена
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 500;
					transferToAttachmentsCoef = 0.5;
					healthLevels[] = {{1.0,{"set_cod2\set\top\data\top.rvmat"}},{0.7,{"set_cod2\set\top\data\top.rvmat"}},{0.5,{"set_cod2\set\top\data\top_damage.rvmat"}},{0.3,{"set_cod2\set\top\data\top_damage.rvmat"}},{0.0,{"set_cod2\set\top\data\top_destruct.rvmat"}}};
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
	class Vp_Set6_vest2_Silver_vb: Vp_Set6_vest2_Silver
	{
		scope = 2;
		descriptionShort = "$STR_VBC_King_Vest2_DescVB";
		vbSet = "set_cod2"; // метка сета для скриптовых баффов (vb_bonuses.c)
		visibilityModifier = 0.8; // незаметность для заражённых
		repairableWithKits[] = {2,3,5,8}; // швейный, кожаный, скотч, эпоксидка
		repairCosts[] = {25,25,25,25};
		class Protection
		{
			biological = 1;
			chemical = 1;
		};
		itemsCargoSize[] = {10,6};
		// DamageSystem скопирован из Vp_Set6_vest2_ColorBase целиком (частичное переопределение заменило бы его), броня усилена
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 800;
					transferToAttachmentsCoef = 0.5;
					healthLevels[] = {{1.0,{"set_cod2\set\vest2\data\vest2.rvmat","set_cod2\set\vest2\data\plita.rvmat"}},{0.7,{"set_cod2\set\vest2\data\vest2.rvmat","set_cod2\set\vest2\data\plita.rvmat"}},{0.5,{"set_cod2\set\vest2\data\vest2_damage.rvmat","set_cod2\set\vest2\data\plita_damage.rvmat"}},{0.3,{"set_cod2\set\vest2\data\vest2_damage.rvmat","set_cod2\set\vest2\data\plita_damage.rvmat"}},{0.0,{"set_cod2\set\vest2\data\vest2_destruct.rvmat","set_cod2\set\vest2\data\plita_destruct.rvmat"}}};
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
	class Vp_Set6_vest2_Silver_Away_vb: Vp_Set6_vest2_Silver_Away
	{
		scope = 2;
		descriptionShort = "$STR_VBC_King_Vest2_DescVB";
		vbSet = "set_cod2"; // метка сета для скриптовых баффов (vb_bonuses.c)
		visibilityModifier = 0.8; // незаметность для заражённых
		repairableWithKits[] = {2,3,5,8}; // швейный, кожаный, скотч, эпоксидка
		repairCosts[] = {25,25,25,25};
		class Protection
		{
			biological = 1;
			chemical = 1;
		};
		itemsCargoSize[] = {10,6};
		// DamageSystem скопирован из Vp_Set6_vest2_ColorBase целиком (частичное переопределение заменило бы его), броня усилена
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 800;
					transferToAttachmentsCoef = 0.5;
					healthLevels[] = {{1.0,{"set_cod2\set\vest2\data\vest2.rvmat","set_cod2\set\vest2\data\plita.rvmat"}},{0.7,{"set_cod2\set\vest2\data\vest2.rvmat","set_cod2\set\vest2\data\plita.rvmat"}},{0.5,{"set_cod2\set\vest2\data\vest2_damage.rvmat","set_cod2\set\vest2\data\plita_damage.rvmat"}},{0.3,{"set_cod2\set\vest2\data\vest2_damage.rvmat","set_cod2\set\vest2\data\plita_damage.rvmat"}},{0.0,{"set_cod2\set\vest2\data\vest2_destruct.rvmat","set_cod2\set\vest2\data\plita_destruct.rvmat"}}};
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
	class Vp_Set6_Vest_black_vb: Vp_Set6_Vest_black
	{
		scope = 2;
		descriptionShort = "$STR_VBC_King_Vest_DescVB";
		vbSet = "set_cod2"; // метка сета для скриптовых баффов (vb_bonuses.c)
		visibilityModifier = 0.8; // незаметность для заражённых
		repairableWithKits[] = {2,3,5,8}; // швейный, кожаный, скотч, эпоксидка
		repairCosts[] = {25,25,25,25};
		class Protection
		{
			biological = 1;
			chemical = 1;
		};
		itemsCargoSize[] = {10,6};
		// DamageSystem скопирован из Vp_Set6_Vest_ColorBase целиком (частичное переопределение заменило бы его), броня усилена
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 800;
					transferToAttachmentsCoef = 0.5;
					healthLevels[] = {{1.0,{"set_cod2\set\vest\data\vest.rvmat","set_cod2\set\vest\data\korona.rvmat"}},{0.7,{"set_cod2\set\vest\data\vest.rvmat","set_cod2\set\vest\data\korona.rvmat"}},{0.5,{"set_cod2\set\vest\data\vest_damage.rvmat","set_cod2\set\vest\data\korona_damage.rvmat"}},{0.3,{"set_cod2\set\vest\data\vest_damage.rvmat","set_cod2\set\vest\data\korona_damage.rvmat"}},{0.0,{"set_cod2\set\vest\data\vest_destruct.rvmat","set_cod2\set\vest\data\korona_destruct.rvmat"}}};
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
