// DayZavr: улучшенные версии предметов сета (классы *_vb).
// Каждый X_vb наследует X целиком: вид, название, описание, скрипты. Параметры меняем только здесь.

class CfgPatches
{
	class SetComplete_EOD_Specialist_vb
	{
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"SetComplete_EOD_Specialist","TGK_SetComplete_EOD_Specialist"};
	};
};

class CfgVehicles
{
	class TGK_EOD_Specialist_Backpack_Black;
	class TGK_EOD_Specialist_Backpack_Green;
	class TGK_EOD_Specialist_Belt_Black;
	class TGK_EOD_Specialist_Belt_Green;
	class TGK_EOD_Specialist_Boots;
	class TGK_EOD_Specialist_Gloves_Black;
	class TGK_EOD_Specialist_Gloves_Green;
	class TGK_EOD_Specialist_Helmet_Black;
	class TGK_EOD_Specialist_Helmet_Green;
	class TGK_EOD_Specialist_Top_Black;
	class TGK_EOD_Specialist_Top_Green;
	class TGK_EOD_Specialist_Mask_Black;
	class TGK_EOD_Specialist_Mask_Orange;
	class TGK_EOD_Specialist_Pants_Black;
	class TGK_EOD_Specialist_Pants_Green;
	class TGK_EOD_Specialist_Vest_Black;
	class TGK_EOD_Specialist_Vest_Green;

	class TGK_EOD_Specialist_Backpack_Black_vb: TGK_EOD_Specialist_Backpack_Black
	{
		scope = 2;
		descriptionShort = "$STR_VBC_Alpha_Backpack_DescVB";
		vbSet = "SetComplete_EOD_Specialist"; // метка сета для скриптовых баффов (vb_bonuses.c)
		visibilityModifier = 0.8; // незаметность для заражённых
		repairableWithKits[] = {2,3,5,8}; // швейный, кожаный, скотч, эпоксидка
		repairCosts[] = {25,25,25,25};
		class Protection
		{
			biological = 1;
			chemical = 1;
		};
		itemsCargoSize[] = {10,7};
	};
	class TGK_EOD_Specialist_Backpack_Green_vb: TGK_EOD_Specialist_Backpack_Green
	{
		scope = 2;
		descriptionShort = "$STR_VBC_Alpha_Backpack_DescVB";
		vbSet = "SetComplete_EOD_Specialist"; // метка сета для скриптовых баффов (vb_bonuses.c)
		visibilityModifier = 0.8; // незаметность для заражённых
		repairableWithKits[] = {2,3,5,8}; // швейный, кожаный, скотч, эпоксидка
		repairCosts[] = {25,25,25,25};
		class Protection
		{
			biological = 1;
			chemical = 1;
		};
		itemsCargoSize[] = {10,7};
	};
	class TGK_EOD_Specialist_Belt_Black_vb: TGK_EOD_Specialist_Belt_Black
	{
		scope = 2;
		descriptionShort = "$STR_VBC_Alpha_Belt_DescVB";
		vbSet = "SetComplete_EOD_Specialist"; // метка сета для скриптовых баффов (vb_bonuses.c)
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
	class TGK_EOD_Specialist_Belt_Green_vb: TGK_EOD_Specialist_Belt_Green
	{
		scope = 2;
		descriptionShort = "$STR_VBC_Alpha_Belt_DescVB";
		vbSet = "SetComplete_EOD_Specialist"; // метка сета для скриптовых баффов (vb_bonuses.c)
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
	class TGK_EOD_Specialist_Boots_vb: TGK_EOD_Specialist_Boots
	{
		scope = 2;
		descriptionShort = "$STR_VBC_Alpha_Boots_DescVB";
		vbSet = "SetComplete_EOD_Specialist"; // метка сета для скриптовых баффов (vb_bonuses.c)
		visibilityModifier = 0.8; // незаметность для заражённых
		repairableWithKits[] = {2,3,5,8}; // швейный, кожаный, скотч, эпоксидка
		repairCosts[] = {25,25,25,25};
		soundAttType = "Sneakers"; // тихий шаг для заражённых
		class Protection
		{
			biological = 1;
			chemical = 1;
		};
		// DamageSystem скопирован из TGK_SetComplete_EOD_Specialist_Boots_Colorbase целиком (частичное переопределение заменило бы его), броня усилена
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 500;
					healthLevels[] = {{1.0,{"SetComplete_EOD_Specialist\set\data\boots\Boots.rvmat"}},{0.7,{"SetComplete_EOD_Specialist\set\data\boots\Boots.rvmat"}},{0.5,{"SetComplete_EOD_Specialist\set\data\boots\Boots_damage.rvmat"}},{0.3,{"SetComplete_EOD_Specialist\set\data\boots\Boots_damage.rvmat"}},{0.0,{"SetComplete_EOD_Specialist\set\data\boots\Boots_destruct.rvmat"}}};
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
	class TGK_EOD_Specialist_Gloves_Black_vb: TGK_EOD_Specialist_Gloves_Black
	{
		scope = 2;
		descriptionShort = "$STR_VBC_Alpha_Gloves_DescVB";
		vbSet = "SetComplete_EOD_Specialist"; // метка сета для скриптовых баффов (vb_bonuses.c)
		visibilityModifier = 0.8; // незаметность для заражённых
		repairableWithKits[] = {2,3,5,8}; // швейный, кожаный, скотч, эпоксидка
		repairCosts[] = {25,25,25,25};
		class Protection
		{
			biological = 1;
			chemical = 1;
		};
		// DamageSystem скопирован из TGK_SetComplete_EOD_Specialist_Gloves_Colorbase целиком (частичное переопределение заменило бы его), броня усилена
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 400;
					healthLevels[] = {{1.0,{"SetComplete_EOD_Specialist\set\data\gloves\gloves.rvmat"}},{0.7,{"SetComplete_EOD_Specialist\set\data\gloves\gloves.rvmat"}},{0.5,{"SetComplete_EOD_Specialist\set\data\gloves\gloves_damage.rvmat"}},{0.3,{"SetComplete_EOD_Specialist\set\data\gloves\gloves_damage.rvmat"}},{0.0,{"SetComplete_EOD_Specialist\set\data\gloves\gloves_destruct.rvmat"}}};
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
	class TGK_EOD_Specialist_Gloves_Green_vb: TGK_EOD_Specialist_Gloves_Green
	{
		scope = 2;
		descriptionShort = "$STR_VBC_Alpha_Gloves_DescVB";
		vbSet = "SetComplete_EOD_Specialist"; // метка сета для скриптовых баффов (vb_bonuses.c)
		visibilityModifier = 0.8; // незаметность для заражённых
		repairableWithKits[] = {2,3,5,8}; // швейный, кожаный, скотч, эпоксидка
		repairCosts[] = {25,25,25,25};
		class Protection
		{
			biological = 1;
			chemical = 1;
		};
		// DamageSystem скопирован из TGK_SetComplete_EOD_Specialist_Gloves_Colorbase целиком (частичное переопределение заменило бы его), броня усилена
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 400;
					healthLevels[] = {{1.0,{"SetComplete_EOD_Specialist\set\data\gloves\gloves.rvmat"}},{0.7,{"SetComplete_EOD_Specialist\set\data\gloves\gloves.rvmat"}},{0.5,{"SetComplete_EOD_Specialist\set\data\gloves\gloves_damage.rvmat"}},{0.3,{"SetComplete_EOD_Specialist\set\data\gloves\gloves_damage.rvmat"}},{0.0,{"SetComplete_EOD_Specialist\set\data\gloves\gloves_destruct.rvmat"}}};
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
	class TGK_EOD_Specialist_Helmet_Black_vb: TGK_EOD_Specialist_Helmet_Black
	{
		scope = 2;
		descriptionShort = "$STR_VBC_Alpha_Helmet_DescVB";
		vbSet = "SetComplete_EOD_Specialist"; // метка сета для скриптовых баффов (vb_bonuses.c)
		visibilityModifier = 0.8; // незаметность для заражённых
		repairableWithKits[] = {2,3,5,8}; // швейный, кожаный, скотч, эпоксидка
		repairCosts[] = {25,25,25,25};
		class Protection
		{
			biological = 1;
			chemical = 1;
		};
		// DamageSystem скопирован из TGK_SetComplete_EOD_Specialist_Helmet_Colorbase целиком (частичное переопределение заменило бы его), броня усилена
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 400;
					healthLevels[] = {{1.0,{"SetComplete_EOD_Specialist\set\data\belt\Belt.rvmat","SetComplete_EOD_Specialist\set\data\helmet_strap\helmet_strap.rvmat","SetComplete_EOD_Specialist\set\data\clips\clips.rvmat","SetComplete_EOD_Specialist\set\data\def_2_ear\def_2_ear.rvmat","SetComplete_EOD_Specialist\set\data\glass\glass.rvmat","SetComplete_EOD_Specialist\set\data\helm\helm.rvmat","SetComplete_EOD_Specialist\set\data\helm1\helm1.rvmat","SetComplete_EOD_Specialist\set\data\manda\manda.rvmat","SetComplete_EOD_Specialist\set\data\patch_golova\patch_golova.rvmat","SetComplete_EOD_Specialist\set\data\patch_golova2\patch_golova2.rvmat","SetComplete_EOD_Specialist\set\data\pnv\pnv.rvmat"}},{0.7,{"SetComplete_EOD_Specialist\set\data\belt\Belt.rvmat","SetComplete_EOD_Specialist\set\data\helmet_strap\helmet_strap.rvmat","SetComplete_EOD_Specialist\set\data\clips\clips.rvmat","SetComplete_EOD_Specialist\set\data\def_2_ear\def_2_ear.rvmat","SetComplete_EOD_Specialist\set\data\glass\glass.rvmat","SetComplete_EOD_Specialist\set\data\helm\helm.rvmat","SetComplete_EOD_Specialist\set\data\helm1\helm1.rvmat","SetComplete_EOD_Specialist\set\data\manda\manda.rvmat","SetComplete_EOD_Specialist\set\data\patch_golova\patch_golova.rvmat","SetComplete_EOD_Specialist\set\data\patch_golova2\patch_golova2.rvmat","SetComplete_EOD_Specialist\set\data\pnv\pnv.rvmat"}},{0.5,{"SetComplete_EOD_Specialist\set\data\belt\Belt_damage.rvmat","SetComplete_EOD_Specialist\set\data\helmet_strap\helmet_strap_damage.rvmat","SetComplete_EOD_Specialist\set\data\clips\clips_damage.rvmat","SetComplete_EOD_Specialist\set\data\def_2_ear\def_2_ear_damage.rvmat","SetComplete_EOD_Specialist\set\data\glass\glass_damage.rvmat","SetComplete_EOD_Specialist\set\data\helm\helm_damage.rvmat","SetComplete_EOD_Specialist\set\data\helm1\helm1_damage.rvmat","SetComplete_EOD_Specialist\set\data\manda\manda_damage.rvmat","SetComplete_EOD_Specialist\set\data\patch_golova\patch_golova_damage.rvmat","SetComplete_EOD_Specialist\set\data\patch_golova2\patch_golova2_damage.rvmat","SetComplete_EOD_Specialist\set\data\pnv\pnv_damage.rvmat"}},{0.3,{"SetComplete_EOD_Specialist\set\data\belt\Belt_damage.rvmat","SetComplete_EOD_Specialist\set\data\helmet_strap\helmet_strap_damage.rvmat","SetComplete_EOD_Specialist\set\data\clips\clips_damage.rvmat","SetComplete_EOD_Specialist\set\data\def_2_ear\def_2_ear_damage.rvmat","SetComplete_EOD_Specialist\set\data\glass\glass_damage.rvmat","SetComplete_EOD_Specialist\set\data\helm\helm_damage.rvmat","SetComplete_EOD_Specialist\set\data\helm1\helm1_damage.rvmat","SetComplete_EOD_Specialist\set\data\manda\manda_damage.rvmat","SetComplete_EOD_Specialist\set\data\patch_golova\patch_golova_damage.rvmat","SetComplete_EOD_Specialist\set\data\patch_golova2\patch_golova2_damage.rvmat","SetComplete_EOD_Specialist\set\data\pnv\pnv_damage.rvmat"}},{0.0,{"SetComplete_EOD_Specialist\set\data\belt\Belt_destruct.rvmat","SetComplete_EOD_Specialist\set\data\helmet_strap\helmet_strap_destruct.rvmat","SetComplete_EOD_Specialist\set\data\clips\clips_destruct.rvmat","SetComplete_EOD_Specialist\set\data\def_2_ear\def_2_ear_destruct.rvmat","SetComplete_EOD_Specialist\set\data\glass\glass_destruct.rvmat","SetComplete_EOD_Specialist\set\data\helm\helm_destruct.rvmat","SetComplete_EOD_Specialist\set\data\helm1\helm1_destruct.rvmat","SetComplete_EOD_Specialist\set\data\manda\manda_destruct.rvmat","SetComplete_EOD_Specialist\set\data\patch_golova\patch_golova_destruct.rvmat","SetComplete_EOD_Specialist\set\data\patch_golova2\patch_golova2_destruct.rvmat","SetComplete_EOD_Specialist\set\data\pnv\pnv_destruct.rvmat"}}};
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
	class TGK_EOD_Specialist_Helmet_Green_vb: TGK_EOD_Specialist_Helmet_Green
	{
		scope = 2;
		descriptionShort = "$STR_VBC_Alpha_Helmet_DescVB";
		vbSet = "SetComplete_EOD_Specialist"; // метка сета для скриптовых баффов (vb_bonuses.c)
		visibilityModifier = 0.8; // незаметность для заражённых
		repairableWithKits[] = {2,3,5,8}; // швейный, кожаный, скотч, эпоксидка
		repairCosts[] = {25,25,25,25};
		class Protection
		{
			biological = 1;
			chemical = 1;
		};
		// DamageSystem скопирован из TGK_SetComplete_EOD_Specialist_Helmet_Colorbase целиком (частичное переопределение заменило бы его), броня усилена
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 400;
					healthLevels[] = {{1.0,{"SetComplete_EOD_Specialist\set\data\belt\Belt.rvmat","SetComplete_EOD_Specialist\set\data\helmet_strap\helmet_strap.rvmat","SetComplete_EOD_Specialist\set\data\clips\clips.rvmat","SetComplete_EOD_Specialist\set\data\def_2_ear\def_2_ear.rvmat","SetComplete_EOD_Specialist\set\data\glass\glass.rvmat","SetComplete_EOD_Specialist\set\data\helm\helm.rvmat","SetComplete_EOD_Specialist\set\data\helm1\helm1.rvmat","SetComplete_EOD_Specialist\set\data\manda\manda.rvmat","SetComplete_EOD_Specialist\set\data\patch_golova\patch_golova.rvmat","SetComplete_EOD_Specialist\set\data\patch_golova2\patch_golova2.rvmat","SetComplete_EOD_Specialist\set\data\pnv\pnv.rvmat"}},{0.7,{"SetComplete_EOD_Specialist\set\data\belt\Belt.rvmat","SetComplete_EOD_Specialist\set\data\helmet_strap\helmet_strap.rvmat","SetComplete_EOD_Specialist\set\data\clips\clips.rvmat","SetComplete_EOD_Specialist\set\data\def_2_ear\def_2_ear.rvmat","SetComplete_EOD_Specialist\set\data\glass\glass.rvmat","SetComplete_EOD_Specialist\set\data\helm\helm.rvmat","SetComplete_EOD_Specialist\set\data\helm1\helm1.rvmat","SetComplete_EOD_Specialist\set\data\manda\manda.rvmat","SetComplete_EOD_Specialist\set\data\patch_golova\patch_golova.rvmat","SetComplete_EOD_Specialist\set\data\patch_golova2\patch_golova2.rvmat","SetComplete_EOD_Specialist\set\data\pnv\pnv.rvmat"}},{0.5,{"SetComplete_EOD_Specialist\set\data\belt\Belt_damage.rvmat","SetComplete_EOD_Specialist\set\data\helmet_strap\helmet_strap_damage.rvmat","SetComplete_EOD_Specialist\set\data\clips\clips_damage.rvmat","SetComplete_EOD_Specialist\set\data\def_2_ear\def_2_ear_damage.rvmat","SetComplete_EOD_Specialist\set\data\glass\glass_damage.rvmat","SetComplete_EOD_Specialist\set\data\helm\helm_damage.rvmat","SetComplete_EOD_Specialist\set\data\helm1\helm1_damage.rvmat","SetComplete_EOD_Specialist\set\data\manda\manda_damage.rvmat","SetComplete_EOD_Specialist\set\data\patch_golova\patch_golova_damage.rvmat","SetComplete_EOD_Specialist\set\data\patch_golova2\patch_golova2_damage.rvmat","SetComplete_EOD_Specialist\set\data\pnv\pnv_damage.rvmat"}},{0.3,{"SetComplete_EOD_Specialist\set\data\belt\Belt_damage.rvmat","SetComplete_EOD_Specialist\set\data\helmet_strap\helmet_strap_damage.rvmat","SetComplete_EOD_Specialist\set\data\clips\clips_damage.rvmat","SetComplete_EOD_Specialist\set\data\def_2_ear\def_2_ear_damage.rvmat","SetComplete_EOD_Specialist\set\data\glass\glass_damage.rvmat","SetComplete_EOD_Specialist\set\data\helm\helm_damage.rvmat","SetComplete_EOD_Specialist\set\data\helm1\helm1_damage.rvmat","SetComplete_EOD_Specialist\set\data\manda\manda_damage.rvmat","SetComplete_EOD_Specialist\set\data\patch_golova\patch_golova_damage.rvmat","SetComplete_EOD_Specialist\set\data\patch_golova2\patch_golova2_damage.rvmat","SetComplete_EOD_Specialist\set\data\pnv\pnv_damage.rvmat"}},{0.0,{"SetComplete_EOD_Specialist\set\data\belt\Belt_destruct.rvmat","SetComplete_EOD_Specialist\set\data\helmet_strap\helmet_strap_destruct.rvmat","SetComplete_EOD_Specialist\set\data\clips\clips_destruct.rvmat","SetComplete_EOD_Specialist\set\data\def_2_ear\def_2_ear_destruct.rvmat","SetComplete_EOD_Specialist\set\data\glass\glass_destruct.rvmat","SetComplete_EOD_Specialist\set\data\helm\helm_destruct.rvmat","SetComplete_EOD_Specialist\set\data\helm1\helm1_destruct.rvmat","SetComplete_EOD_Specialist\set\data\manda\manda_destruct.rvmat","SetComplete_EOD_Specialist\set\data\patch_golova\patch_golova_destruct.rvmat","SetComplete_EOD_Specialist\set\data\patch_golova2\patch_golova2_destruct.rvmat","SetComplete_EOD_Specialist\set\data\pnv\pnv_destruct.rvmat"}}};
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
	class TGK_EOD_Specialist_Top_Black_vb: TGK_EOD_Specialist_Top_Black
	{
		scope = 2;
		descriptionShort = "$STR_VBC_Alpha_Top_DescVB";
		vbSet = "SetComplete_EOD_Specialist"; // метка сета для скриптовых баффов (vb_bonuses.c)
		visibilityModifier = 0.8; // незаметность для заражённых
		repairableWithKits[] = {2,3,5,8}; // швейный, кожаный, скотч, эпоксидка
		repairCosts[] = {25,25,25,25};
		class Protection
		{
			biological = 1;
			chemical = 1;
		};
		itemsCargoSize[] = {10,8};
		// DamageSystem скопирован из TGK_SetComplete_EOD_Specialist_Top_Colorbase целиком (частичное переопределение заменило бы его), броня усилена
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 500;
					transferToAttachmentsCoef = 0.5;
					healthLevels[] = {{1,{"SetComplete_EOD_Specialist\set\data\top\top.rvmat"}},{0.7,{"SetComplete_EOD_Specialist\set\data\top\top.rvmat"}},{0.5,{"SetComplete_EOD_Specialist\set\data\top\top_damage.rvmat"}},{0.3,{"SetComplete_EOD_Specialist\set\data\top\top_damage.rvmat"}},{0,{"SetComplete_EOD_Specialist\set\data\top\top_destruct.rvmat"}}};
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
	class TGK_EOD_Specialist_Top_Green_vb: TGK_EOD_Specialist_Top_Green
	{
		scope = 2;
		descriptionShort = "$STR_VBC_Alpha_Top_DescVB";
		vbSet = "SetComplete_EOD_Specialist"; // метка сета для скриптовых баффов (vb_bonuses.c)
		visibilityModifier = 0.8; // незаметность для заражённых
		repairableWithKits[] = {2,3,5,8}; // швейный, кожаный, скотч, эпоксидка
		repairCosts[] = {25,25,25,25};
		class Protection
		{
			biological = 1;
			chemical = 1;
		};
		itemsCargoSize[] = {10,8};
		// DamageSystem скопирован из TGK_SetComplete_EOD_Specialist_Top_Colorbase целиком (частичное переопределение заменило бы его), броня усилена
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 500;
					transferToAttachmentsCoef = 0.5;
					healthLevels[] = {{1,{"SetComplete_EOD_Specialist\set\data\top\top.rvmat"}},{0.7,{"SetComplete_EOD_Specialist\set\data\top\top.rvmat"}},{0.5,{"SetComplete_EOD_Specialist\set\data\top\top_damage.rvmat"}},{0.3,{"SetComplete_EOD_Specialist\set\data\top\top_damage.rvmat"}},{0,{"SetComplete_EOD_Specialist\set\data\top\top_destruct.rvmat"}}};
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
	class TGK_EOD_Specialist_Mask_Black_vb: TGK_EOD_Specialist_Mask_Black
	{
		scope = 2;
		descriptionShort = "$STR_VBC_Alpha_Mask_DescVB";
		vbSet = "SetComplete_EOD_Specialist"; // метка сета для скриптовых баффов (vb_bonuses.c)
		visibilityModifier = 0.8; // незаметность для заражённых
		repairableWithKits[] = {2,3,5,8}; // швейный, кожаный, скотч, эпоксидка
		repairCosts[] = {25,25,25,25};
		class Protection
		{
			biological = 1;
			chemical = 1;
		};
	};
	class TGK_EOD_Specialist_Mask_Orange_vb: TGK_EOD_Specialist_Mask_Orange
	{
		scope = 2;
		descriptionShort = "$STR_VBC_Alpha_Mask_DescVB";
		vbSet = "SetComplete_EOD_Specialist"; // метка сета для скриптовых баффов (vb_bonuses.c)
		visibilityModifier = 0.8; // незаметность для заражённых
		repairableWithKits[] = {2,3,5,8}; // швейный, кожаный, скотч, эпоксидка
		repairCosts[] = {25,25,25,25};
		class Protection
		{
			biological = 1;
			chemical = 1;
		};
	};
	class TGK_EOD_Specialist_Pants_Black_vb: TGK_EOD_Specialist_Pants_Black
	{
		scope = 2;
		descriptionShort = "$STR_VBC_Alpha_Pants_DescVB";
		vbSet = "SetComplete_EOD_Specialist"; // метка сета для скриптовых баффов (vb_bonuses.c)
		visibilityModifier = 0.8; // незаметность для заражённых
		repairableWithKits[] = {2,3,5,8}; // швейный, кожаный, скотч, эпоксидка
		repairCosts[] = {25,25,25,25};
		class Protection
		{
			biological = 1;
			chemical = 1;
		};
		itemsCargoSize[] = {10,7};
		// DamageSystem скопирован из TGK_SetComplete_EOD_Specialist_Pants_Colorbase целиком (частичное переопределение заменило бы его), броня усилена
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 500;
					transferToAttachmentsCoef = 0.5;
					healthLevels[] = {{1,{"SetComplete_EOD_Specialist\set\data\pants\pants.rvmat","SetComplete_EOD_Specialist\set\data\pads\pads.rvmat"}},{0.7,{"SetComplete_EOD_Specialist\set\data\pants\pants.rvmat","SetComplete_EOD_Specialist\set\data\pads\pads.rvmat"}},{0.5,{"SetComplete_EOD_Specialist\set\data\pants\pants_damage.rvmat","SetComplete_EOD_Specialist\set\data\pads\pads_damage.rvmat"}},{0.3,{"SetComplete_EOD_Specialist\set\data\pants\pants_damage.rvmat","SetComplete_EOD_Specialist\set\data\pads\pads_damage.rvmat"}},{0,{"SetComplete_EOD_Specialist\set\data\pants\pants_destruct.rvmat","SetComplete_EOD_Specialist\set\data\pads\pads_destruct.rvmat"}}};
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
	class TGK_EOD_Specialist_Pants_Green_vb: TGK_EOD_Specialist_Pants_Green
	{
		scope = 2;
		descriptionShort = "$STR_VBC_Alpha_Pants_DescVB";
		vbSet = "SetComplete_EOD_Specialist"; // метка сета для скриптовых баффов (vb_bonuses.c)
		visibilityModifier = 0.8; // незаметность для заражённых
		repairableWithKits[] = {2,3,5,8}; // швейный, кожаный, скотч, эпоксидка
		repairCosts[] = {25,25,25,25};
		class Protection
		{
			biological = 1;
			chemical = 1;
		};
		itemsCargoSize[] = {10,7};
		// DamageSystem скопирован из TGK_SetComplete_EOD_Specialist_Pants_Colorbase целиком (частичное переопределение заменило бы его), броня усилена
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 500;
					transferToAttachmentsCoef = 0.5;
					healthLevels[] = {{1,{"SetComplete_EOD_Specialist\set\data\pants\pants.rvmat","SetComplete_EOD_Specialist\set\data\pads\pads.rvmat"}},{0.7,{"SetComplete_EOD_Specialist\set\data\pants\pants.rvmat","SetComplete_EOD_Specialist\set\data\pads\pads.rvmat"}},{0.5,{"SetComplete_EOD_Specialist\set\data\pants\pants_damage.rvmat","SetComplete_EOD_Specialist\set\data\pads\pads_damage.rvmat"}},{0.3,{"SetComplete_EOD_Specialist\set\data\pants\pants_damage.rvmat","SetComplete_EOD_Specialist\set\data\pads\pads_damage.rvmat"}},{0,{"SetComplete_EOD_Specialist\set\data\pants\pants_destruct.rvmat","SetComplete_EOD_Specialist\set\data\pads\pads_destruct.rvmat"}}};
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
	class TGK_EOD_Specialist_Vest_Black_vb: TGK_EOD_Specialist_Vest_Black
	{
		scope = 2;
		descriptionShort = "$STR_VBC_Alpha_Vest_DescVB";
		vbSet = "SetComplete_EOD_Specialist"; // метка сета для скриптовых баффов (vb_bonuses.c)
		visibilityModifier = 0.8; // незаметность для заражённых
		repairableWithKits[] = {2,3,5,8}; // швейный, кожаный, скотч, эпоксидка
		repairCosts[] = {25,25,25,25};
		class Protection
		{
			biological = 1;
			chemical = 1;
		};
		itemsCargoSize[] = {10,6};
		// DamageSystem скопирован из TGK_SetComplete_EOD_Specialist_Vest_Colorbase целиком (частичное переопределение заменило бы его), броня усилена
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 800;
					transferToAttachmentsCoef = 0.5;
					healthLevels[] = {{1.0,{"SetComplete_EOD_Specialist\set\data\belt\Belt.rvmat","SetComplete_EOD_Specialist\set\data\belt_granads\Belt_granads.rvmat","SetComplete_EOD_Specialist\set\data\bundle\bundle.rvmat","SetComplete_EOD_Specialist\set\data\clips\clips.rvmat","SetComplete_EOD_Specialist\set\data\def_1\def_1.rvmat","SetComplete_EOD_Specialist\set\data\def_4_neck\def_4_neck.rvmat","SetComplete_EOD_Specialist\set\data\def_5_ruka\def_5_ruka.rvmat","SetComplete_EOD_Specialist\set\data\granad\granad.rvmat","SetComplete_EOD_Specialist\set\data\granade_strap\granade_strap.rvmat","SetComplete_EOD_Specialist\set\data\helmet_strap\helmet_strap.rvmat","SetComplete_EOD_Specialist\set\data\knife_holster\knife_holster.rvmat","SetComplete_EOD_Specialist\set\data\karm2\karm2.rvmat","SetComplete_EOD_Specialist\set\data\knife\knife.rvmat","SetComplete_EOD_Specialist\set\data\knife_holster1\knife_holster1.rvmat","SetComplete_EOD_Specialist\set\data\magazine\magazine.rvmat","SetComplete_EOD_Specialist\set\data\magazine_pouch1\magazine_pouch1.rvmat","SetComplete_EOD_Specialist\set\data\magazine_pouch2\magazine_pouch2.rvmat","SetComplete_EOD_Specialist\set\data\patch_ruka\patch_ruka.rvmat","SetComplete_EOD_Specialist\set\data\pocket\pocket.rvmat","SetComplete_EOD_Specialist\set\data\racia_scotch\racia_scotch.rvmat","SetComplete_EOD_Specialist\set\data\racia_strap\racia_strap.rvmat","SetComplete_EOD_Specialist\set\data\raciya\raciya.rvmat","SetComplete_EOD_Specialist\set\data\raciya1_2\raciya1_2.rvmat","SetComplete_EOD_Specialist\set\data\raciya1_3\raciya1_3.rvmat","SetComplete_EOD_Specialist\set\data\raciya1_4\raciya1_4.rvmat","SetComplete_EOD_Specialist\set\data\radio\radio.rvmat","SetComplete_EOD_Specialist\set\data\radio_holster\radio_holster.rvmat","SetComplete_EOD_Specialist\set\data\radio_pocket\radio_pocket.rvmat","SetComplete_EOD_Specialist\set\data\rope\rope.rvmat","SetComplete_EOD_Specialist\set\data\smoke_granad\smoke_granad.rvmat","SetComplete_EOD_Specialist\set\data\vest\vest.rvmat"}},{0.7,{"SetComplete_EOD_Specialist\set\data\belt\Belt.rvmat","SetComplete_EOD_Specialist\set\data\belt_granads\Belt_granads.rvmat","SetComplete_EOD_Specialist\set\data\bundle\bundle.rvmat","SetComplete_EOD_Specialist\set\data\clips\clips.rvmat","SetComplete_EOD_Specialist\set\data\def_1\def_1.rvmat","SetComplete_EOD_Specialist\set\data\def_4_neck\def_4_neck.rvmat","SetComplete_EOD_Specialist\set\data\def_5_ruka\def_5_ruka.rvmat","SetComplete_EOD_Specialist\set\data\granad\granad.rvmat","SetComplete_EOD_Specialist\set\data\granade_strap\granade_strap.rvmat","SetComplete_EOD_Specialist\set\data\helmet_strap\helmet_strap.rvmat","SetComplete_EOD_Specialist\set\data\knife_holster\knife_holster.rvmat","SetComplete_EOD_Specialist\set\data\karm2\karm2.rvmat","SetComplete_EOD_Specialist\set\data\knife\knife.rvmat","SetComplete_EOD_Specialist\set\data\knife_holster1\knife_holster1.rvmat","SetComplete_EOD_Specialist\set\data\magazine\magazine.rvmat","SetComplete_EOD_Specialist\set\data\magazine_pouch1\magazine_pouch1.rvmat","SetComplete_EOD_Specialist\set\data\magazine_pouch2\magazine_pouch2.rvmat","SetComplete_EOD_Specialist\set\data\patch_ruka\patch_ruka.rvmat","SetComplete_EOD_Specialist\set\data\pocket\pocket.rvmat","SetComplete_EOD_Specialist\set\data\racia_scotch\racia_scotch.rvmat","SetComplete_EOD_Specialist\set\data\racia_strap\racia_strap.rvmat","SetComplete_EOD_Specialist\set\data\raciya\raciya.rvmat","SetComplete_EOD_Specialist\set\data\raciya1_2\raciya1_2.rvmat","SetComplete_EOD_Specialist\set\data\raciya1_3\raciya1_3.rvmat","SetComplete_EOD_Specialist\set\data\raciya1_4\raciya1_4.rvmat","SetComplete_EOD_Specialist\set\data\radio\radio.rvmat","SetComplete_EOD_Specialist\set\data\radio_holster\radio_holster.rvmat","SetComplete_EOD_Specialist\set\data\radio_pocket\radio_pocket.rvmat","SetComplete_EOD_Specialist\set\data\rope\rope.rvmat","SetComplete_EOD_Specialist\set\data\smoke_granad\smoke_granad.rvmat","SetComplete_EOD_Specialist\set\data\vest\vest.rvmat"}},{0.5,{"SetComplete_EOD_Specialist\set\data\belt\Belt_damage.rvmat","SetComplete_EOD_Specialist\set\data\belt_granads\Belt_granads_damage.rvmat","SetComplete_EOD_Specialist\set\data\bundle\bundle_damage.rvmat","SetComplete_EOD_Specialist\set\data\clips\clips.rvmat","SetComplete_EOD_Specialist\set\data\def_1\def_1_damage.rvmat","SetComplete_EOD_Specialist\set\data\def_4_neck\def_4_neck_damage.rvmat","SetComplete_EOD_Specialist\set\data\def_5_ruka\def_5_ruka_damage.rvmat","SetComplete_EOD_Specialist\set\data\granad\granad_damage.rvmat","SetComplete_EOD_Specialist\set\data\granade_strap\granade_strap_damage.rvmat","SetComplete_EOD_Specialist\set\data\helmet_strap\helmet_strap_damage.rvmat","SetComplete_EOD_Specialist\set\data\knife_holster\knife_holster_damage.rvmat","SetComplete_EOD_Specialist\set\data\karm2\karm2_damage.rvmat","SetComplete_EOD_Specialist\set\data\knife\knife_damage.rvmat","SetComplete_EOD_Specialist\set\data\knife_holster1\knife_holster1_damage.rvmat","SetComplete_EOD_Specialist\set\data\magazine\magazine_damage.rvmat","SetComplete_EOD_Specialist\set\data\magazine_pouch1\magazine_pouch1_damage.rvmat","SetComplete_EOD_Specialist\set\data\magazine_pouch2\magazine_pouch2_damage.rvmat","SetComplete_EOD_Specialist\set\data\patch_ruka\patch_ruka_damage.rvmat","SetComplete_EOD_Specialist\set\data\pocket\pocket_damage.rvmat","SetComplete_EOD_Specialist\set\data\racia_scotch\racia_scotch_damage.rvmat","SetComplete_EOD_Specialist\set\data\racia_strap\racia_strap_damage.rvmat","SetComplete_EOD_Specialist\set\data\raciya\raciya_damage.rvmat","SetComplete_EOD_Specialist\set\data\raciya1_2\raciya1_2_damage.rvmat","SetComplete_EOD_Specialist\set\data\raciya1_3\raciya1_3_damage.rvmat","SetComplete_EOD_Specialist\set\data\raciya1_4\raciya1_4_damage.rvmat","SetComplete_EOD_Specialist\set\data\radio\radio_damage.rvmat","SetComplete_EOD_Specialist\set\data\radio_holster\radio_holster_damage.rvmat","SetComplete_EOD_Specialist\set\data\radio_pocket\radio_pocket_damage.rvmat","SetComplete_EOD_Specialist\set\data\rope\rope_damage.rvmat","SetComplete_EOD_Specialist\set\data\smoke_granad\smoke_granad_damage.rvmat","SetComplete_EOD_Specialist\set\data\vest\vest_damage.rvmat"}},{0.3,{"SetComplete_EOD_Specialist\set\data\belt\Belt_damage.rvmat","SetComplete_EOD_Specialist\set\data\belt_granads\Belt_granads_damage.rvmat","SetComplete_EOD_Specialist\set\data\bundle\bundle_damage.rvmat","SetComplete_EOD_Specialist\set\data\clips\clips.rvmat","SetComplete_EOD_Specialist\set\data\def_1\def_1_damage.rvmat","SetComplete_EOD_Specialist\set\data\def_4_neck\def_4_neck_damage.rvmat","SetComplete_EOD_Specialist\set\data\def_5_ruka\def_5_ruka_damage.rvmat","SetComplete_EOD_Specialist\set\data\granad\granad_damage.rvmat","SetComplete_EOD_Specialist\set\data\granade_strap\granade_strap_damage.rvmat","SetComplete_EOD_Specialist\set\data\helmet_strap\helmet_strap_damage.rvmat","SetComplete_EOD_Specialist\set\data\knife_holster\knife_holster_damage.rvmat","SetComplete_EOD_Specialist\set\data\karm2\karm2_damage.rvmat","SetComplete_EOD_Specialist\set\data\knife\knife_damage.rvmat","SetComplete_EOD_Specialist\set\data\knife_holster1\knife_holster1_damage.rvmat","SetComplete_EOD_Specialist\set\data\magazine\magazine_damage.rvmat","SetComplete_EOD_Specialist\set\data\magazine_pouch1\magazine_pouch1_damage.rvmat","SetComplete_EOD_Specialist\set\data\magazine_pouch2\magazine_pouch2_damage.rvmat","SetComplete_EOD_Specialist\set\data\patch_ruka\patch_ruka_damage.rvmat","SetComplete_EOD_Specialist\set\data\pocket\pocket_damage.rvmat","SetComplete_EOD_Specialist\set\data\racia_scotch\racia_scotch_damage.rvmat","SetComplete_EOD_Specialist\set\data\racia_strap\racia_strap_damage.rvmat","SetComplete_EOD_Specialist\set\data\raciya\raciya_damage.rvmat","SetComplete_EOD_Specialist\set\data\raciya1_2\raciya1_2_damage.rvmat","SetComplete_EOD_Specialist\set\data\raciya1_3\raciya1_3_damage.rvmat","SetComplete_EOD_Specialist\set\data\raciya1_4\raciya1_4_damage.rvmat","SetComplete_EOD_Specialist\set\data\radio\radio_damage.rvmat","SetComplete_EOD_Specialist\set\data\radio_holster\radio_holster_damage.rvmat","SetComplete_EOD_Specialist\set\data\radio_pocket\radio_pocket_damage.rvmat","SetComplete_EOD_Specialist\set\data\rope\rope_damage.rvmat","SetComplete_EOD_Specialist\set\data\smoke_granad\smoke_granad_damage.rvmat","SetComplete_EOD_Specialist\set\data\vest\vest_damage.rvmat"}},{0.0,{"SetComplete_EOD_Specialist\set\data\belt\Belt_destruct.rvmat","SetComplete_EOD_Specialist\set\data\belt_granads\Belt_granads_destruct.rvmat","SetComplete_EOD_Specialist\set\data\bundle\bundle_destruct.rvmat","SetComplete_EOD_Specialist\set\data\clips\clips.rvmat","SetComplete_EOD_Specialist\set\data\def_1\def_1_destruct.rvmat","SetComplete_EOD_Specialist\set\data\def_4_neck\def_4_neck_destruct.rvmat","SetComplete_EOD_Specialist\set\data\def_5_ruka\def_5_ruka_destruct.rvmat","SetComplete_EOD_Specialist\set\data\granad\granad_destruct.rvmat","SetComplete_EOD_Specialist\set\data\granade_strap\granade_strap_destruct.rvmat","SetComplete_EOD_Specialist\set\data\helmet_strap\helmet_strap_destruct.rvmat","SetComplete_EOD_Specialist\set\data\knife_holster\knife_holster_destruct.rvmat","SetComplete_EOD_Specialist\set\data\karm2\karm2_destruct.rvmat","SetComplete_EOD_Specialist\set\data\knife\knife_destruct.rvmat","SetComplete_EOD_Specialist\set\data\knife_holster1\knife_holster1_destruct.rvmat","SetComplete_EOD_Specialist\set\data\magazine\magazine_destruct.rvmat","SetComplete_EOD_Specialist\set\data\magazine_pouch1\magazine_pouch1_destruct.rvmat","SetComplete_EOD_Specialist\set\data\magazine_pouch2\magazine_pouch2_destruct.rvmat","SetComplete_EOD_Specialist\set\data\patch_ruka\patch_ruka_destruct.rvmat","SetComplete_EOD_Specialist\set\data\pocket\pocket_destruct.rvmat","SetComplete_EOD_Specialist\set\data\racia_scotch\racia_scotch_destruct.rvmat","SetComplete_EOD_Specialist\set\data\racia_strap\racia_strap_destruct.rvmat","SetComplete_EOD_Specialist\set\data\raciya\raciya_destruct.rvmat","SetComplete_EOD_Specialist\set\data\raciya1_2\raciya1_2_destruct.rvmat","SetComplete_EOD_Specialist\set\data\raciya1_3\raciya1_3_destruct.rvmat","SetComplete_EOD_Specialist\set\data\raciya1_4\raciya1_4_destruct.rvmat","SetComplete_EOD_Specialist\set\data\radio\radio_destruct.rvmat","SetComplete_EOD_Specialist\set\data\radio_holster\radio_holster_destruct.rvmat","SetComplete_EOD_Specialist\set\data\radio_pocket\radio_pocket_destruct.rvmat","SetComplete_EOD_Specialist\set\data\rope\rope_destruct.rvmat","SetComplete_EOD_Specialist\set\data\smoke_granad\smoke_granad_destruct.rvmat","SetComplete_EOD_Specialist\set\data\vest\vest_destruct.rvmat"}}};
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
	class TGK_EOD_Specialist_Vest_Green_vb: TGK_EOD_Specialist_Vest_Green
	{
		scope = 2;
		descriptionShort = "$STR_VBC_Alpha_Vest_DescVB";
		vbSet = "SetComplete_EOD_Specialist"; // метка сета для скриптовых баффов (vb_bonuses.c)
		visibilityModifier = 0.8; // незаметность для заражённых
		repairableWithKits[] = {2,3,5,8}; // швейный, кожаный, скотч, эпоксидка
		repairCosts[] = {25,25,25,25};
		class Protection
		{
			biological = 1;
			chemical = 1;
		};
		itemsCargoSize[] = {10,6};
		// DamageSystem скопирован из TGK_SetComplete_EOD_Specialist_Vest_Colorbase целиком (частичное переопределение заменило бы его), броня усилена
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 800;
					transferToAttachmentsCoef = 0.5;
					healthLevels[] = {{1.0,{"SetComplete_EOD_Specialist\set\data\belt\Belt.rvmat","SetComplete_EOD_Specialist\set\data\belt_granads\Belt_granads.rvmat","SetComplete_EOD_Specialist\set\data\bundle\bundle.rvmat","SetComplete_EOD_Specialist\set\data\clips\clips.rvmat","SetComplete_EOD_Specialist\set\data\def_1\def_1.rvmat","SetComplete_EOD_Specialist\set\data\def_4_neck\def_4_neck.rvmat","SetComplete_EOD_Specialist\set\data\def_5_ruka\def_5_ruka.rvmat","SetComplete_EOD_Specialist\set\data\granad\granad.rvmat","SetComplete_EOD_Specialist\set\data\granade_strap\granade_strap.rvmat","SetComplete_EOD_Specialist\set\data\helmet_strap\helmet_strap.rvmat","SetComplete_EOD_Specialist\set\data\knife_holster\knife_holster.rvmat","SetComplete_EOD_Specialist\set\data\karm2\karm2.rvmat","SetComplete_EOD_Specialist\set\data\knife\knife.rvmat","SetComplete_EOD_Specialist\set\data\knife_holster1\knife_holster1.rvmat","SetComplete_EOD_Specialist\set\data\magazine\magazine.rvmat","SetComplete_EOD_Specialist\set\data\magazine_pouch1\magazine_pouch1.rvmat","SetComplete_EOD_Specialist\set\data\magazine_pouch2\magazine_pouch2.rvmat","SetComplete_EOD_Specialist\set\data\patch_ruka\patch_ruka.rvmat","SetComplete_EOD_Specialist\set\data\pocket\pocket.rvmat","SetComplete_EOD_Specialist\set\data\racia_scotch\racia_scotch.rvmat","SetComplete_EOD_Specialist\set\data\racia_strap\racia_strap.rvmat","SetComplete_EOD_Specialist\set\data\raciya\raciya.rvmat","SetComplete_EOD_Specialist\set\data\raciya1_2\raciya1_2.rvmat","SetComplete_EOD_Specialist\set\data\raciya1_3\raciya1_3.rvmat","SetComplete_EOD_Specialist\set\data\raciya1_4\raciya1_4.rvmat","SetComplete_EOD_Specialist\set\data\radio\radio.rvmat","SetComplete_EOD_Specialist\set\data\radio_holster\radio_holster.rvmat","SetComplete_EOD_Specialist\set\data\radio_pocket\radio_pocket.rvmat","SetComplete_EOD_Specialist\set\data\rope\rope.rvmat","SetComplete_EOD_Specialist\set\data\smoke_granad\smoke_granad.rvmat","SetComplete_EOD_Specialist\set\data\vest\vest.rvmat"}},{0.7,{"SetComplete_EOD_Specialist\set\data\belt\Belt.rvmat","SetComplete_EOD_Specialist\set\data\belt_granads\Belt_granads.rvmat","SetComplete_EOD_Specialist\set\data\bundle\bundle.rvmat","SetComplete_EOD_Specialist\set\data\clips\clips.rvmat","SetComplete_EOD_Specialist\set\data\def_1\def_1.rvmat","SetComplete_EOD_Specialist\set\data\def_4_neck\def_4_neck.rvmat","SetComplete_EOD_Specialist\set\data\def_5_ruka\def_5_ruka.rvmat","SetComplete_EOD_Specialist\set\data\granad\granad.rvmat","SetComplete_EOD_Specialist\set\data\granade_strap\granade_strap.rvmat","SetComplete_EOD_Specialist\set\data\helmet_strap\helmet_strap.rvmat","SetComplete_EOD_Specialist\set\data\knife_holster\knife_holster.rvmat","SetComplete_EOD_Specialist\set\data\karm2\karm2.rvmat","SetComplete_EOD_Specialist\set\data\knife\knife.rvmat","SetComplete_EOD_Specialist\set\data\knife_holster1\knife_holster1.rvmat","SetComplete_EOD_Specialist\set\data\magazine\magazine.rvmat","SetComplete_EOD_Specialist\set\data\magazine_pouch1\magazine_pouch1.rvmat","SetComplete_EOD_Specialist\set\data\magazine_pouch2\magazine_pouch2.rvmat","SetComplete_EOD_Specialist\set\data\patch_ruka\patch_ruka.rvmat","SetComplete_EOD_Specialist\set\data\pocket\pocket.rvmat","SetComplete_EOD_Specialist\set\data\racia_scotch\racia_scotch.rvmat","SetComplete_EOD_Specialist\set\data\racia_strap\racia_strap.rvmat","SetComplete_EOD_Specialist\set\data\raciya\raciya.rvmat","SetComplete_EOD_Specialist\set\data\raciya1_2\raciya1_2.rvmat","SetComplete_EOD_Specialist\set\data\raciya1_3\raciya1_3.rvmat","SetComplete_EOD_Specialist\set\data\raciya1_4\raciya1_4.rvmat","SetComplete_EOD_Specialist\set\data\radio\radio.rvmat","SetComplete_EOD_Specialist\set\data\radio_holster\radio_holster.rvmat","SetComplete_EOD_Specialist\set\data\radio_pocket\radio_pocket.rvmat","SetComplete_EOD_Specialist\set\data\rope\rope.rvmat","SetComplete_EOD_Specialist\set\data\smoke_granad\smoke_granad.rvmat","SetComplete_EOD_Specialist\set\data\vest\vest.rvmat"}},{0.5,{"SetComplete_EOD_Specialist\set\data\belt\Belt_damage.rvmat","SetComplete_EOD_Specialist\set\data\belt_granads\Belt_granads_damage.rvmat","SetComplete_EOD_Specialist\set\data\bundle\bundle_damage.rvmat","SetComplete_EOD_Specialist\set\data\clips\clips.rvmat","SetComplete_EOD_Specialist\set\data\def_1\def_1_damage.rvmat","SetComplete_EOD_Specialist\set\data\def_4_neck\def_4_neck_damage.rvmat","SetComplete_EOD_Specialist\set\data\def_5_ruka\def_5_ruka_damage.rvmat","SetComplete_EOD_Specialist\set\data\granad\granad_damage.rvmat","SetComplete_EOD_Specialist\set\data\granade_strap\granade_strap_damage.rvmat","SetComplete_EOD_Specialist\set\data\helmet_strap\helmet_strap_damage.rvmat","SetComplete_EOD_Specialist\set\data\knife_holster\knife_holster_damage.rvmat","SetComplete_EOD_Specialist\set\data\karm2\karm2_damage.rvmat","SetComplete_EOD_Specialist\set\data\knife\knife_damage.rvmat","SetComplete_EOD_Specialist\set\data\knife_holster1\knife_holster1_damage.rvmat","SetComplete_EOD_Specialist\set\data\magazine\magazine_damage.rvmat","SetComplete_EOD_Specialist\set\data\magazine_pouch1\magazine_pouch1_damage.rvmat","SetComplete_EOD_Specialist\set\data\magazine_pouch2\magazine_pouch2_damage.rvmat","SetComplete_EOD_Specialist\set\data\patch_ruka\patch_ruka_damage.rvmat","SetComplete_EOD_Specialist\set\data\pocket\pocket_damage.rvmat","SetComplete_EOD_Specialist\set\data\racia_scotch\racia_scotch_damage.rvmat","SetComplete_EOD_Specialist\set\data\racia_strap\racia_strap_damage.rvmat","SetComplete_EOD_Specialist\set\data\raciya\raciya_damage.rvmat","SetComplete_EOD_Specialist\set\data\raciya1_2\raciya1_2_damage.rvmat","SetComplete_EOD_Specialist\set\data\raciya1_3\raciya1_3_damage.rvmat","SetComplete_EOD_Specialist\set\data\raciya1_4\raciya1_4_damage.rvmat","SetComplete_EOD_Specialist\set\data\radio\radio_damage.rvmat","SetComplete_EOD_Specialist\set\data\radio_holster\radio_holster_damage.rvmat","SetComplete_EOD_Specialist\set\data\radio_pocket\radio_pocket_damage.rvmat","SetComplete_EOD_Specialist\set\data\rope\rope_damage.rvmat","SetComplete_EOD_Specialist\set\data\smoke_granad\smoke_granad_damage.rvmat","SetComplete_EOD_Specialist\set\data\vest\vest_damage.rvmat"}},{0.3,{"SetComplete_EOD_Specialist\set\data\belt\Belt_damage.rvmat","SetComplete_EOD_Specialist\set\data\belt_granads\Belt_granads_damage.rvmat","SetComplete_EOD_Specialist\set\data\bundle\bundle_damage.rvmat","SetComplete_EOD_Specialist\set\data\clips\clips.rvmat","SetComplete_EOD_Specialist\set\data\def_1\def_1_damage.rvmat","SetComplete_EOD_Specialist\set\data\def_4_neck\def_4_neck_damage.rvmat","SetComplete_EOD_Specialist\set\data\def_5_ruka\def_5_ruka_damage.rvmat","SetComplete_EOD_Specialist\set\data\granad\granad_damage.rvmat","SetComplete_EOD_Specialist\set\data\granade_strap\granade_strap_damage.rvmat","SetComplete_EOD_Specialist\set\data\helmet_strap\helmet_strap_damage.rvmat","SetComplete_EOD_Specialist\set\data\knife_holster\knife_holster_damage.rvmat","SetComplete_EOD_Specialist\set\data\karm2\karm2_damage.rvmat","SetComplete_EOD_Specialist\set\data\knife\knife_damage.rvmat","SetComplete_EOD_Specialist\set\data\knife_holster1\knife_holster1_damage.rvmat","SetComplete_EOD_Specialist\set\data\magazine\magazine_damage.rvmat","SetComplete_EOD_Specialist\set\data\magazine_pouch1\magazine_pouch1_damage.rvmat","SetComplete_EOD_Specialist\set\data\magazine_pouch2\magazine_pouch2_damage.rvmat","SetComplete_EOD_Specialist\set\data\patch_ruka\patch_ruka_damage.rvmat","SetComplete_EOD_Specialist\set\data\pocket\pocket_damage.rvmat","SetComplete_EOD_Specialist\set\data\racia_scotch\racia_scotch_damage.rvmat","SetComplete_EOD_Specialist\set\data\racia_strap\racia_strap_damage.rvmat","SetComplete_EOD_Specialist\set\data\raciya\raciya_damage.rvmat","SetComplete_EOD_Specialist\set\data\raciya1_2\raciya1_2_damage.rvmat","SetComplete_EOD_Specialist\set\data\raciya1_3\raciya1_3_damage.rvmat","SetComplete_EOD_Specialist\set\data\raciya1_4\raciya1_4_damage.rvmat","SetComplete_EOD_Specialist\set\data\radio\radio_damage.rvmat","SetComplete_EOD_Specialist\set\data\radio_holster\radio_holster_damage.rvmat","SetComplete_EOD_Specialist\set\data\radio_pocket\radio_pocket_damage.rvmat","SetComplete_EOD_Specialist\set\data\rope\rope_damage.rvmat","SetComplete_EOD_Specialist\set\data\smoke_granad\smoke_granad_damage.rvmat","SetComplete_EOD_Specialist\set\data\vest\vest_damage.rvmat"}},{0.0,{"SetComplete_EOD_Specialist\set\data\belt\Belt_destruct.rvmat","SetComplete_EOD_Specialist\set\data\belt_granads\Belt_granads_destruct.rvmat","SetComplete_EOD_Specialist\set\data\bundle\bundle_destruct.rvmat","SetComplete_EOD_Specialist\set\data\clips\clips.rvmat","SetComplete_EOD_Specialist\set\data\def_1\def_1_destruct.rvmat","SetComplete_EOD_Specialist\set\data\def_4_neck\def_4_neck_destruct.rvmat","SetComplete_EOD_Specialist\set\data\def_5_ruka\def_5_ruka_destruct.rvmat","SetComplete_EOD_Specialist\set\data\granad\granad_destruct.rvmat","SetComplete_EOD_Specialist\set\data\granade_strap\granade_strap_destruct.rvmat","SetComplete_EOD_Specialist\set\data\helmet_strap\helmet_strap_destruct.rvmat","SetComplete_EOD_Specialist\set\data\knife_holster\knife_holster_destruct.rvmat","SetComplete_EOD_Specialist\set\data\karm2\karm2_destruct.rvmat","SetComplete_EOD_Specialist\set\data\knife\knife_destruct.rvmat","SetComplete_EOD_Specialist\set\data\knife_holster1\knife_holster1_destruct.rvmat","SetComplete_EOD_Specialist\set\data\magazine\magazine_destruct.rvmat","SetComplete_EOD_Specialist\set\data\magazine_pouch1\magazine_pouch1_destruct.rvmat","SetComplete_EOD_Specialist\set\data\magazine_pouch2\magazine_pouch2_destruct.rvmat","SetComplete_EOD_Specialist\set\data\patch_ruka\patch_ruka_destruct.rvmat","SetComplete_EOD_Specialist\set\data\pocket\pocket_destruct.rvmat","SetComplete_EOD_Specialist\set\data\racia_scotch\racia_scotch_destruct.rvmat","SetComplete_EOD_Specialist\set\data\racia_strap\racia_strap_destruct.rvmat","SetComplete_EOD_Specialist\set\data\raciya\raciya_destruct.rvmat","SetComplete_EOD_Specialist\set\data\raciya1_2\raciya1_2_destruct.rvmat","SetComplete_EOD_Specialist\set\data\raciya1_3\raciya1_3_destruct.rvmat","SetComplete_EOD_Specialist\set\data\raciya1_4\raciya1_4_destruct.rvmat","SetComplete_EOD_Specialist\set\data\radio\radio_destruct.rvmat","SetComplete_EOD_Specialist\set\data\radio_holster\radio_holster_destruct.rvmat","SetComplete_EOD_Specialist\set\data\radio_pocket\radio_pocket_destruct.rvmat","SetComplete_EOD_Specialist\set\data\rope\rope_destruct.rvmat","SetComplete_EOD_Specialist\set\data\smoke_granad\smoke_granad_destruct.rvmat","SetComplete_EOD_Specialist\set\data\vest\vest_destruct.rvmat"}}};
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
