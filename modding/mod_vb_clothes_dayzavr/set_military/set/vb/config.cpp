// DayZavr: улучшенные версии предметов сета (классы *_vb).
// Каждый X_vb наследует X целиком: вид, название, описание, скрипты. Параметры меняем только здесь.

class CfgPatches
{
	class set_military_vb
	{
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"set_military","Vp_Set19"};
	};
};

class CfgVehicles
{
	class Vp_Set19_Mask_Black;
	class Vp_Set19_Mask_Black_Pink;
	class Vp_Set19_Mask_Black_Pixel;
	class Vp_Set19_Mask_Desert;
	class VP_SET19_Boots_Black;
	class VP_SET19_Boots_Black_Pink;
	class VP_SET19_Boots_Black_Pixel;
	class VP_SET19_Boots_Desert;
	class Vp_SET19_cap_Black;
	class Vp_SET19_cap_Black_Pink;
	class Vp_SET19_cap_Black_Pixel;
	class Vp_SET19_cap_Desert;
	class Vp_SET19_Gloves_Black;
	class Vp_SET19_Gloves_Black_Pink;
	class Vp_SET19_Gloves_Black_Pixel;
	class Vp_SET19_Gloves_Desert;
	class Vp_SET19_Pants_Black;
	class Vp_SET19_Pants_Black_Pink;
	class Vp_SET19_Pants_Black_Pixel;
	class Vp_SET19_Pants_Desert;
	class Vp_SET19_Top_Black;
	class Vp_SET19_Top_Black_Pink;
	class Vp_SET19_Top_Black_Pixel;
	class Vp_SET19_Top_Desert;
	class Vp_SET19_Vest_Black;
	class Vp_SET19_Vest_Black_Pink;
	class Vp_SET19_Vest_Black_Pixel;
	class Vp_SET19_Vest_Desert;

	class Vp_Set19_Mask_Black_vb: Vp_Set19_Mask_Black
	{
		scope = 2;
		descriptionShort = "$STR_VBC_Spetsnaz_Mask_DescVB";
		vbSet = "set_military"; // метка сета для скриптовых баффов (vb_bonuses.c)
		visibilityModifier = 0.8; // незаметность для заражённых
		repairableWithKits[] = {2,3,5,8}; // швейный, кожаный, скотч, эпоксидка
		repairCosts[] = {25,25,25,25};
		class Protection
		{
			biological = 1;
			chemical = 1;
		};
	};
	class Vp_Set19_Mask_Black_Pink_vb: Vp_Set19_Mask_Black_Pink
	{
		scope = 2;
		descriptionShort = "$STR_VBC_Spetsnaz_Mask_DescVB";
		vbSet = "set_military"; // метка сета для скриптовых баффов (vb_bonuses.c)
		visibilityModifier = 0.8; // незаметность для заражённых
		repairableWithKits[] = {2,3,5,8}; // швейный, кожаный, скотч, эпоксидка
		repairCosts[] = {25,25,25,25};
		class Protection
		{
			biological = 1;
			chemical = 1;
		};
	};
	class Vp_Set19_Mask_Black_Pixel_vb: Vp_Set19_Mask_Black_Pixel
	{
		scope = 2;
		descriptionShort = "$STR_VBC_Spetsnaz_Mask_DescVB";
		vbSet = "set_military"; // метка сета для скриптовых баффов (vb_bonuses.c)
		visibilityModifier = 0.8; // незаметность для заражённых
		repairableWithKits[] = {2,3,5,8}; // швейный, кожаный, скотч, эпоксидка
		repairCosts[] = {25,25,25,25};
		class Protection
		{
			biological = 1;
			chemical = 1;
		};
	};
	class Vp_Set19_Mask_Desert_vb: Vp_Set19_Mask_Desert
	{
		scope = 2;
		descriptionShort = "$STR_VBC_Spetsnaz_Mask_DescVB";
		vbSet = "set_military"; // метка сета для скриптовых баффов (vb_bonuses.c)
		visibilityModifier = 0.8; // незаметность для заражённых
		repairableWithKits[] = {2,3,5,8}; // швейный, кожаный, скотч, эпоксидка
		repairCosts[] = {25,25,25,25};
		class Protection
		{
			biological = 1;
			chemical = 1;
		};
	};
	class VP_SET19_Boots_Black_vb: VP_SET19_Boots_Black
	{
		scope = 2;
		descriptionShort = "$STR_VBC_Spetsnaz_Boots_DescVB";
		vbSet = "set_military"; // метка сета для скриптовых баффов (vb_bonuses.c)
		visibilityModifier = 0.8; // незаметность для заражённых
		repairableWithKits[] = {2,3,5,8}; // швейный, кожаный, скотч, эпоксидка
		repairCosts[] = {25,25,25,25};
		soundAttType = "Sneakers"; // тихий шаг для заражённых
		class Protection
		{
			biological = 1;
			chemical = 1;
		};
		// DamageSystem скопирован из VP_SET19_Boots_ColorBase целиком (частичное переопределение заменило бы его), броня усилена
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 500;
					healthLevels[] = {{1,{"\set_military\set\boots\data\Boots.rvmat"}},{0.7,{"\set_military\set\boots\data\Boots.rvmat"}},{0.5,{"\set_military\set\boots\data\Boots_damage.rvmat"}},{0.3,{"\set_military\set\boots\data\Boots_damage.rvmat"}},{0,{"\set_military\set\boots\data\Boots_destruct.rvmat"}}};
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
	class VP_SET19_Boots_Black_Pink_vb: VP_SET19_Boots_Black_Pink
	{
		scope = 2;
		descriptionShort = "$STR_VBC_Spetsnaz_Boots_DescVB";
		vbSet = "set_military"; // метка сета для скриптовых баффов (vb_bonuses.c)
		visibilityModifier = 0.8; // незаметность для заражённых
		repairableWithKits[] = {2,3,5,8}; // швейный, кожаный, скотч, эпоксидка
		repairCosts[] = {25,25,25,25};
		soundAttType = "Sneakers"; // тихий шаг для заражённых
		class Protection
		{
			biological = 1;
			chemical = 1;
		};
		// DamageSystem скопирован из VP_SET19_Boots_ColorBase целиком (частичное переопределение заменило бы его), броня усилена
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 500;
					healthLevels[] = {{1,{"\set_military\set\boots\data\Boots.rvmat"}},{0.7,{"\set_military\set\boots\data\Boots.rvmat"}},{0.5,{"\set_military\set\boots\data\Boots_damage.rvmat"}},{0.3,{"\set_military\set\boots\data\Boots_damage.rvmat"}},{0,{"\set_military\set\boots\data\Boots_destruct.rvmat"}}};
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
	class VP_SET19_Boots_Black_Pixel_vb: VP_SET19_Boots_Black_Pixel
	{
		scope = 2;
		descriptionShort = "$STR_VBC_Spetsnaz_Boots_DescVB";
		vbSet = "set_military"; // метка сета для скриптовых баффов (vb_bonuses.c)
		visibilityModifier = 0.8; // незаметность для заражённых
		repairableWithKits[] = {2,3,5,8}; // швейный, кожаный, скотч, эпоксидка
		repairCosts[] = {25,25,25,25};
		soundAttType = "Sneakers"; // тихий шаг для заражённых
		class Protection
		{
			biological = 1;
			chemical = 1;
		};
		// DamageSystem скопирован из VP_SET19_Boots_ColorBase целиком (частичное переопределение заменило бы его), броня усилена
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 500;
					healthLevels[] = {{1,{"\set_military\set\boots\data\Boots.rvmat"}},{0.7,{"\set_military\set\boots\data\Boots.rvmat"}},{0.5,{"\set_military\set\boots\data\Boots_damage.rvmat"}},{0.3,{"\set_military\set\boots\data\Boots_damage.rvmat"}},{0,{"\set_military\set\boots\data\Boots_destruct.rvmat"}}};
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
	class VP_SET19_Boots_Desert_vb: VP_SET19_Boots_Desert
	{
		scope = 2;
		descriptionShort = "$STR_VBC_Spetsnaz_Boots_DescVB";
		vbSet = "set_military"; // метка сета для скриптовых баффов (vb_bonuses.c)
		visibilityModifier = 0.8; // незаметность для заражённых
		repairableWithKits[] = {2,3,5,8}; // швейный, кожаный, скотч, эпоксидка
		repairCosts[] = {25,25,25,25};
		soundAttType = "Sneakers"; // тихий шаг для заражённых
		class Protection
		{
			biological = 1;
			chemical = 1;
		};
		// DamageSystem скопирован из VP_SET19_Boots_ColorBase целиком (частичное переопределение заменило бы его), броня усилена
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 500;
					healthLevels[] = {{1,{"\set_military\set\boots\data\Boots.rvmat"}},{0.7,{"\set_military\set\boots\data\Boots.rvmat"}},{0.5,{"\set_military\set\boots\data\Boots_damage.rvmat"}},{0.3,{"\set_military\set\boots\data\Boots_damage.rvmat"}},{0,{"\set_military\set\boots\data\Boots_destruct.rvmat"}}};
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
	class Vp_SET19_cap_Black_vb: Vp_SET19_cap_Black
	{
		scope = 2;
		descriptionShort = "$STR_VBC_Spetsnaz_Cap_DescVB";
		vbSet = "set_military"; // метка сета для скриптовых баффов (vb_bonuses.c)
		visibilityModifier = 0.8; // незаметность для заражённых
		repairableWithKits[] = {2,3,5,8}; // швейный, кожаный, скотч, эпоксидка
		repairCosts[] = {25,25,25,25};
		class Protection
		{
			biological = 1;
			chemical = 1;
		};
		// DamageSystem скопирован из Vp_SET19_cap_ColorBase целиком (частичное переопределение заменило бы его), броня усилена
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 300;
					transferToAttachmentsCoef = 0.5;
					healthLevels[] = {{1.0,{"\set_military\set\cap\data\cap.rvmat","\set_military\set\cap\data\Headgears.rvmat"}},{0.7,{"\set_military\set\cap\data\cap.rvmat","\set_military\set\cap\data\Headgears.rvmat"}},{0.5,{"\set_military\set\cap\data\cap_damage.rvmat","\set_military\set\cap\data\Headgears_damage.rvmat"}},{0.3,{"\set_military\set\cap\data\cap_damage.rvmat","\set_military\set\cap\data\Headgears_damage.rvmat"}},{0.0,{"\set_military\set\cap\data\cap_destruct.rvmat","\set_military\set\cap\data\Headgears_damage.rvmat"}}};
				};
			};
			class GlobalArmor
			{
				class Melee
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
				class Infected
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
	class Vp_SET19_cap_Black_Pink_vb: Vp_SET19_cap_Black_Pink
	{
		scope = 2;
		descriptionShort = "$STR_VBC_Spetsnaz_Cap_DescVB";
		vbSet = "set_military"; // метка сета для скриптовых баффов (vb_bonuses.c)
		visibilityModifier = 0.8; // незаметность для заражённых
		repairableWithKits[] = {2,3,5,8}; // швейный, кожаный, скотч, эпоксидка
		repairCosts[] = {25,25,25,25};
		class Protection
		{
			biological = 1;
			chemical = 1;
		};
		// DamageSystem скопирован из Vp_SET19_cap_ColorBase целиком (частичное переопределение заменило бы его), броня усилена
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 300;
					transferToAttachmentsCoef = 0.5;
					healthLevels[] = {{1.0,{"\set_military\set\cap\data\cap.rvmat","\set_military\set\cap\data\Headgears.rvmat"}},{0.7,{"\set_military\set\cap\data\cap.rvmat","\set_military\set\cap\data\Headgears.rvmat"}},{0.5,{"\set_military\set\cap\data\cap_damage.rvmat","\set_military\set\cap\data\Headgears_damage.rvmat"}},{0.3,{"\set_military\set\cap\data\cap_damage.rvmat","\set_military\set\cap\data\Headgears_damage.rvmat"}},{0.0,{"\set_military\set\cap\data\cap_destruct.rvmat","\set_military\set\cap\data\Headgears_damage.rvmat"}}};
				};
			};
			class GlobalArmor
			{
				class Melee
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
				class Infected
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
	class Vp_SET19_cap_Black_Pixel_vb: Vp_SET19_cap_Black_Pixel
	{
		scope = 2;
		descriptionShort = "$STR_VBC_Spetsnaz_Cap_DescVB";
		vbSet = "set_military"; // метка сета для скриптовых баффов (vb_bonuses.c)
		visibilityModifier = 0.8; // незаметность для заражённых
		repairableWithKits[] = {2,3,5,8}; // швейный, кожаный, скотч, эпоксидка
		repairCosts[] = {25,25,25,25};
		class Protection
		{
			biological = 1;
			chemical = 1;
		};
		// DamageSystem скопирован из Vp_SET19_cap_ColorBase целиком (частичное переопределение заменило бы его), броня усилена
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 300;
					transferToAttachmentsCoef = 0.5;
					healthLevels[] = {{1.0,{"\set_military\set\cap\data\cap.rvmat","\set_military\set\cap\data\Headgears.rvmat"}},{0.7,{"\set_military\set\cap\data\cap.rvmat","\set_military\set\cap\data\Headgears.rvmat"}},{0.5,{"\set_military\set\cap\data\cap_damage.rvmat","\set_military\set\cap\data\Headgears_damage.rvmat"}},{0.3,{"\set_military\set\cap\data\cap_damage.rvmat","\set_military\set\cap\data\Headgears_damage.rvmat"}},{0.0,{"\set_military\set\cap\data\cap_destruct.rvmat","\set_military\set\cap\data\Headgears_damage.rvmat"}}};
				};
			};
			class GlobalArmor
			{
				class Melee
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
				class Infected
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
	class Vp_SET19_cap_Desert_vb: Vp_SET19_cap_Desert
	{
		scope = 2;
		descriptionShort = "$STR_VBC_Spetsnaz_Cap_DescVB";
		vbSet = "set_military"; // метка сета для скриптовых баффов (vb_bonuses.c)
		visibilityModifier = 0.8; // незаметность для заражённых
		repairableWithKits[] = {2,3,5,8}; // швейный, кожаный, скотч, эпоксидка
		repairCosts[] = {25,25,25,25};
		class Protection
		{
			biological = 1;
			chemical = 1;
		};
		// DamageSystem скопирован из Vp_SET19_cap_ColorBase целиком (частичное переопределение заменило бы его), броня усилена
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 300;
					transferToAttachmentsCoef = 0.5;
					healthLevels[] = {{1.0,{"\set_military\set\cap\data\cap.rvmat","\set_military\set\cap\data\Headgears.rvmat"}},{0.7,{"\set_military\set\cap\data\cap.rvmat","\set_military\set\cap\data\Headgears.rvmat"}},{0.5,{"\set_military\set\cap\data\cap_damage.rvmat","\set_military\set\cap\data\Headgears_damage.rvmat"}},{0.3,{"\set_military\set\cap\data\cap_damage.rvmat","\set_military\set\cap\data\Headgears_damage.rvmat"}},{0.0,{"\set_military\set\cap\data\cap_destruct.rvmat","\set_military\set\cap\data\Headgears_damage.rvmat"}}};
				};
			};
			class GlobalArmor
			{
				class Melee
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
				class Infected
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
	class Vp_SET19_Gloves_Black_vb: Vp_SET19_Gloves_Black
	{
		scope = 2;
		descriptionShort = "$STR_VBC_Spetsnaz_Gloves_DescVB";
		vbSet = "set_military"; // метка сета для скриптовых баффов (vb_bonuses.c)
		visibilityModifier = 0.8; // незаметность для заражённых
		repairableWithKits[] = {2,3,5,8}; // швейный, кожаный, скотч, эпоксидка
		repairCosts[] = {25,25,25,25};
		class Protection
		{
			biological = 1;
			chemical = 1;
		};
		// DamageSystem скопирован из Vp_SET19_Gloves_ColorBase целиком (частичное переопределение заменило бы его), броня усилена
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 400;
					healthLevels[] = {{1.0,{"\set_military\set\gloves\data\Gloves.rvmat"}},{0.7,{"\set_military\set\gloves\data\Gloves.rvmat"}},{0.5,{"\set_military\set\gloves\data\Gloves_damage.rvmat"}},{0.3,{"\set_military\set\gloves\data\Gloves_damage.rvmat"}},{0.0,{"\set_military\set\gloves\data\Gloves_destruct.rvmat"}}};
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
	class Vp_SET19_Gloves_Black_Pink_vb: Vp_SET19_Gloves_Black_Pink
	{
		scope = 2;
		descriptionShort = "$STR_VBC_Spetsnaz_Gloves_DescVB";
		vbSet = "set_military"; // метка сета для скриптовых баффов (vb_bonuses.c)
		visibilityModifier = 0.8; // незаметность для заражённых
		repairableWithKits[] = {2,3,5,8}; // швейный, кожаный, скотч, эпоксидка
		repairCosts[] = {25,25,25,25};
		class Protection
		{
			biological = 1;
			chemical = 1;
		};
		// DamageSystem скопирован из Vp_SET19_Gloves_ColorBase целиком (частичное переопределение заменило бы его), броня усилена
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 400;
					healthLevels[] = {{1.0,{"\set_military\set\gloves\data\Gloves.rvmat"}},{0.7,{"\set_military\set\gloves\data\Gloves.rvmat"}},{0.5,{"\set_military\set\gloves\data\Gloves_damage.rvmat"}},{0.3,{"\set_military\set\gloves\data\Gloves_damage.rvmat"}},{0.0,{"\set_military\set\gloves\data\Gloves_destruct.rvmat"}}};
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
	class Vp_SET19_Gloves_Black_Pixel_vb: Vp_SET19_Gloves_Black_Pixel
	{
		scope = 2;
		descriptionShort = "$STR_VBC_Spetsnaz_Gloves_DescVB";
		vbSet = "set_military"; // метка сета для скриптовых баффов (vb_bonuses.c)
		visibilityModifier = 0.8; // незаметность для заражённых
		repairableWithKits[] = {2,3,5,8}; // швейный, кожаный, скотч, эпоксидка
		repairCosts[] = {25,25,25,25};
		class Protection
		{
			biological = 1;
			chemical = 1;
		};
		// DamageSystem скопирован из Vp_SET19_Gloves_ColorBase целиком (частичное переопределение заменило бы его), броня усилена
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 400;
					healthLevels[] = {{1.0,{"\set_military\set\gloves\data\Gloves.rvmat"}},{0.7,{"\set_military\set\gloves\data\Gloves.rvmat"}},{0.5,{"\set_military\set\gloves\data\Gloves_damage.rvmat"}},{0.3,{"\set_military\set\gloves\data\Gloves_damage.rvmat"}},{0.0,{"\set_military\set\gloves\data\Gloves_destruct.rvmat"}}};
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
	class Vp_SET19_Gloves_Desert_vb: Vp_SET19_Gloves_Desert
	{
		scope = 2;
		descriptionShort = "$STR_VBC_Spetsnaz_Gloves_DescVB";
		vbSet = "set_military"; // метка сета для скриптовых баффов (vb_bonuses.c)
		visibilityModifier = 0.8; // незаметность для заражённых
		repairableWithKits[] = {2,3,5,8}; // швейный, кожаный, скотч, эпоксидка
		repairCosts[] = {25,25,25,25};
		class Protection
		{
			biological = 1;
			chemical = 1;
		};
		// DamageSystem скопирован из Vp_SET19_Gloves_ColorBase целиком (частичное переопределение заменило бы его), броня усилена
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 400;
					healthLevels[] = {{1.0,{"\set_military\set\gloves\data\Gloves.rvmat"}},{0.7,{"\set_military\set\gloves\data\Gloves.rvmat"}},{0.5,{"\set_military\set\gloves\data\Gloves_damage.rvmat"}},{0.3,{"\set_military\set\gloves\data\Gloves_damage.rvmat"}},{0.0,{"\set_military\set\gloves\data\Gloves_destruct.rvmat"}}};
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
	class Vp_SET19_Pants_Black_vb: Vp_SET19_Pants_Black
	{
		scope = 2;
		descriptionShort = "$STR_VBC_Spetsnaz_Pants_DescVB";
		vbSet = "set_military"; // метка сета для скриптовых баффов (vb_bonuses.c)
		visibilityModifier = 0.8; // незаметность для заражённых
		repairableWithKits[] = {2,3,5,8}; // швейный, кожаный, скотч, эпоксидка
		repairCosts[] = {25,25,25,25};
		class Protection
		{
			biological = 1;
			chemical = 1;
		};
		itemsCargoSize[] = {10,7};
		// DamageSystem скопирован из Vp_SET19_Pants_ColorBase целиком (частичное переопределение заменило бы его), броня усилена
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 500;
					transferToAttachmentsCoef = 0.5;
					healthLevels[] = {{1.0,{"\set_military\set\pants\data\Pants.rvmat"}},{0.7,{"\set_military\set\pants\data\Pants.rvmat"}},{0.5,{"\set_military\set\pants\data\Pants_damage.rvmat"}},{0.3,{"\set_military\set\pants\data\Pants_damage.rvmat"}},{0.0,{"\set_military\set\pants\data\Pants_destruct.rvmat"}}};
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
	class Vp_SET19_Pants_Black_Pink_vb: Vp_SET19_Pants_Black_Pink
	{
		scope = 2;
		descriptionShort = "$STR_VBC_Spetsnaz_Pants_DescVB";
		vbSet = "set_military"; // метка сета для скриптовых баффов (vb_bonuses.c)
		visibilityModifier = 0.8; // незаметность для заражённых
		repairableWithKits[] = {2,3,5,8}; // швейный, кожаный, скотч, эпоксидка
		repairCosts[] = {25,25,25,25};
		class Protection
		{
			biological = 1;
			chemical = 1;
		};
		itemsCargoSize[] = {10,7};
		// DamageSystem скопирован из Vp_SET19_Pants_ColorBase целиком (частичное переопределение заменило бы его), броня усилена
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 500;
					transferToAttachmentsCoef = 0.5;
					healthLevels[] = {{1.0,{"\set_military\set\pants\data\Pants.rvmat"}},{0.7,{"\set_military\set\pants\data\Pants.rvmat"}},{0.5,{"\set_military\set\pants\data\Pants_damage.rvmat"}},{0.3,{"\set_military\set\pants\data\Pants_damage.rvmat"}},{0.0,{"\set_military\set\pants\data\Pants_destruct.rvmat"}}};
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
	class Vp_SET19_Pants_Black_Pixel_vb: Vp_SET19_Pants_Black_Pixel
	{
		scope = 2;
		descriptionShort = "$STR_VBC_Spetsnaz_Pants_DescVB";
		vbSet = "set_military"; // метка сета для скриптовых баффов (vb_bonuses.c)
		visibilityModifier = 0.8; // незаметность для заражённых
		repairableWithKits[] = {2,3,5,8}; // швейный, кожаный, скотч, эпоксидка
		repairCosts[] = {25,25,25,25};
		class Protection
		{
			biological = 1;
			chemical = 1;
		};
		itemsCargoSize[] = {10,7};
		// DamageSystem скопирован из Vp_SET19_Pants_ColorBase целиком (частичное переопределение заменило бы его), броня усилена
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 500;
					transferToAttachmentsCoef = 0.5;
					healthLevels[] = {{1.0,{"\set_military\set\pants\data\Pants.rvmat"}},{0.7,{"\set_military\set\pants\data\Pants.rvmat"}},{0.5,{"\set_military\set\pants\data\Pants_damage.rvmat"}},{0.3,{"\set_military\set\pants\data\Pants_damage.rvmat"}},{0.0,{"\set_military\set\pants\data\Pants_destruct.rvmat"}}};
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
	class Vp_SET19_Pants_Desert_vb: Vp_SET19_Pants_Desert
	{
		scope = 2;
		descriptionShort = "$STR_VBC_Spetsnaz_Pants_DescVB";
		vbSet = "set_military"; // метка сета для скриптовых баффов (vb_bonuses.c)
		visibilityModifier = 0.8; // незаметность для заражённых
		repairableWithKits[] = {2,3,5,8}; // швейный, кожаный, скотч, эпоксидка
		repairCosts[] = {25,25,25,25};
		class Protection
		{
			biological = 1;
			chemical = 1;
		};
		itemsCargoSize[] = {10,7};
		// DamageSystem скопирован из Vp_SET19_Pants_ColorBase целиком (частичное переопределение заменило бы его), броня усилена
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 500;
					transferToAttachmentsCoef = 0.5;
					healthLevels[] = {{1.0,{"\set_military\set\pants\data\Pants.rvmat"}},{0.7,{"\set_military\set\pants\data\Pants.rvmat"}},{0.5,{"\set_military\set\pants\data\Pants_damage.rvmat"}},{0.3,{"\set_military\set\pants\data\Pants_damage.rvmat"}},{0.0,{"\set_military\set\pants\data\Pants_destruct.rvmat"}}};
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
	class Vp_SET19_Top_Black_vb: Vp_SET19_Top_Black
	{
		scope = 2;
		descriptionShort = "$STR_VBC_Spetsnaz_Top_DescVB";
		vbSet = "set_military"; // метка сета для скриптовых баффов (vb_bonuses.c)
		visibilityModifier = 0.8; // незаметность для заражённых
		repairableWithKits[] = {2,3,5,8}; // швейный, кожаный, скотч, эпоксидка
		repairCosts[] = {25,25,25,25};
		class Protection
		{
			biological = 1;
			chemical = 1;
		};
		itemsCargoSize[] = {10,8};
		// DamageSystem скопирован из Vp_SET19_Top_ColorBase целиком (частичное переопределение заменило бы его), броня усилена
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 500;
					transferToAttachmentsCoef = 0.5;
					healthLevels[] = {{1.0,{"\set_military\set\top\data\top.rvmat"}},{0.7,{"\set_military\set\top\data\top.rvmat"}},{0.5,{"\set_military\set\top\data\top_damage.rvmat"}},{0.3,{"\set_military\set\top\data\top_damage.rvmat"}},{0.0,{"\set_military\set\top\data\top_destruct.rvmat"}}};
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
	class Vp_SET19_Top_Black_Pink_vb: Vp_SET19_Top_Black_Pink
	{
		scope = 2;
		descriptionShort = "$STR_VBC_Spetsnaz_Top_DescVB";
		vbSet = "set_military"; // метка сета для скриптовых баффов (vb_bonuses.c)
		visibilityModifier = 0.8; // незаметность для заражённых
		repairableWithKits[] = {2,3,5,8}; // швейный, кожаный, скотч, эпоксидка
		repairCosts[] = {25,25,25,25};
		class Protection
		{
			biological = 1;
			chemical = 1;
		};
		itemsCargoSize[] = {10,8};
		// DamageSystem скопирован из Vp_SET19_Top_ColorBase целиком (частичное переопределение заменило бы его), броня усилена
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 500;
					transferToAttachmentsCoef = 0.5;
					healthLevels[] = {{1.0,{"\set_military\set\top\data\top.rvmat"}},{0.7,{"\set_military\set\top\data\top.rvmat"}},{0.5,{"\set_military\set\top\data\top_damage.rvmat"}},{0.3,{"\set_military\set\top\data\top_damage.rvmat"}},{0.0,{"\set_military\set\top\data\top_destruct.rvmat"}}};
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
	class Vp_SET19_Top_Black_Pixel_vb: Vp_SET19_Top_Black_Pixel
	{
		scope = 2;
		descriptionShort = "$STR_VBC_Spetsnaz_Top_DescVB";
		vbSet = "set_military"; // метка сета для скриптовых баффов (vb_bonuses.c)
		visibilityModifier = 0.8; // незаметность для заражённых
		repairableWithKits[] = {2,3,5,8}; // швейный, кожаный, скотч, эпоксидка
		repairCosts[] = {25,25,25,25};
		class Protection
		{
			biological = 1;
			chemical = 1;
		};
		itemsCargoSize[] = {10,8};
		// DamageSystem скопирован из Vp_SET19_Top_ColorBase целиком (частичное переопределение заменило бы его), броня усилена
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 500;
					transferToAttachmentsCoef = 0.5;
					healthLevels[] = {{1.0,{"\set_military\set\top\data\top.rvmat"}},{0.7,{"\set_military\set\top\data\top.rvmat"}},{0.5,{"\set_military\set\top\data\top_damage.rvmat"}},{0.3,{"\set_military\set\top\data\top_damage.rvmat"}},{0.0,{"\set_military\set\top\data\top_destruct.rvmat"}}};
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
	class Vp_SET19_Top_Desert_vb: Vp_SET19_Top_Desert
	{
		scope = 2;
		descriptionShort = "$STR_VBC_Spetsnaz_Top_DescVB";
		vbSet = "set_military"; // метка сета для скриптовых баффов (vb_bonuses.c)
		visibilityModifier = 0.8; // незаметность для заражённых
		repairableWithKits[] = {2,3,5,8}; // швейный, кожаный, скотч, эпоксидка
		repairCosts[] = {25,25,25,25};
		class Protection
		{
			biological = 1;
			chemical = 1;
		};
		itemsCargoSize[] = {10,8};
		// DamageSystem скопирован из Vp_SET19_Top_ColorBase целиком (частичное переопределение заменило бы его), броня усилена
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 500;
					transferToAttachmentsCoef = 0.5;
					healthLevels[] = {{1.0,{"\set_military\set\top\data\top.rvmat"}},{0.7,{"\set_military\set\top\data\top.rvmat"}},{0.5,{"\set_military\set\top\data\top_damage.rvmat"}},{0.3,{"\set_military\set\top\data\top_damage.rvmat"}},{0.0,{"\set_military\set\top\data\top_destruct.rvmat"}}};
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
	class Vp_SET19_Vest_Black_vb: Vp_SET19_Vest_Black
	{
		scope = 2;
		descriptionShort = "$STR_VBC_Spetsnaz_Vest_DescVB";
		vbSet = "set_military"; // метка сета для скриптовых баффов (vb_bonuses.c)
		visibilityModifier = 0.8; // незаметность для заражённых
		repairableWithKits[] = {2,3,5,8}; // швейный, кожаный, скотч, эпоксидка
		repairCosts[] = {25,25,25,25};
		class Protection
		{
			biological = 1;
			chemical = 1;
		};
		itemsCargoSize[] = {10,6};
		// DamageSystem скопирован из Vp_SET19_Vest_ColorBase целиком (частичное переопределение заменило бы его), броня усилена
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 800;
					transferToAttachmentsCoef = 0.5;
					healthLevels[] = {{1.0,{"\set_military\set\vest\data\napleshniki.rvmat","\set_military\set\vest\data\vest.rvmat"}},{0.7,{"\set_military\set\vest\data\napleshniki.rvmat","\set_military\set\vest\data\vest.rvmat"}},{0.5,{"\set_military\set\vest\data\napleshniki_damage.rvmat","\set_military\set\vest\data\vest_damage.rvmat"}},{0.3,{"\set_military\set\vest\data\napleshniki_damage.rvmat","\set_military\set\vest\data\vest_damage.rvmat"}},{0.0,{"\set_military\set\vest\data\napleshniki_destruct.rvmat","\set_military\set\vest\data\vest_destruct.rvmat"}}};
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
	class Vp_SET19_Vest_Black_Pink_vb: Vp_SET19_Vest_Black_Pink
	{
		scope = 2;
		descriptionShort = "$STR_VBC_Spetsnaz_Vest_DescVB";
		vbSet = "set_military"; // метка сета для скриптовых баффов (vb_bonuses.c)
		visibilityModifier = 0.8; // незаметность для заражённых
		repairableWithKits[] = {2,3,5,8}; // швейный, кожаный, скотч, эпоксидка
		repairCosts[] = {25,25,25,25};
		class Protection
		{
			biological = 1;
			chemical = 1;
		};
		itemsCargoSize[] = {10,6};
		// DamageSystem скопирован из Vp_SET19_Vest_ColorBase целиком (частичное переопределение заменило бы его), броня усилена
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 800;
					transferToAttachmentsCoef = 0.5;
					healthLevels[] = {{1.0,{"\set_military\set\vest\data\napleshniki.rvmat","\set_military\set\vest\data\vest.rvmat"}},{0.7,{"\set_military\set\vest\data\napleshniki.rvmat","\set_military\set\vest\data\vest.rvmat"}},{0.5,{"\set_military\set\vest\data\napleshniki_damage.rvmat","\set_military\set\vest\data\vest_damage.rvmat"}},{0.3,{"\set_military\set\vest\data\napleshniki_damage.rvmat","\set_military\set\vest\data\vest_damage.rvmat"}},{0.0,{"\set_military\set\vest\data\napleshniki_destruct.rvmat","\set_military\set\vest\data\vest_destruct.rvmat"}}};
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
	class Vp_SET19_Vest_Black_Pixel_vb: Vp_SET19_Vest_Black_Pixel
	{
		scope = 2;
		descriptionShort = "$STR_VBC_Spetsnaz_Vest_DescVB";
		vbSet = "set_military"; // метка сета для скриптовых баффов (vb_bonuses.c)
		visibilityModifier = 0.8; // незаметность для заражённых
		repairableWithKits[] = {2,3,5,8}; // швейный, кожаный, скотч, эпоксидка
		repairCosts[] = {25,25,25,25};
		class Protection
		{
			biological = 1;
			chemical = 1;
		};
		itemsCargoSize[] = {10,6};
		// DamageSystem скопирован из Vp_SET19_Vest_ColorBase целиком (частичное переопределение заменило бы его), броня усилена
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 800;
					transferToAttachmentsCoef = 0.5;
					healthLevels[] = {{1.0,{"\set_military\set\vest\data\napleshniki.rvmat","\set_military\set\vest\data\vest.rvmat"}},{0.7,{"\set_military\set\vest\data\napleshniki.rvmat","\set_military\set\vest\data\vest.rvmat"}},{0.5,{"\set_military\set\vest\data\napleshniki_damage.rvmat","\set_military\set\vest\data\vest_damage.rvmat"}},{0.3,{"\set_military\set\vest\data\napleshniki_damage.rvmat","\set_military\set\vest\data\vest_damage.rvmat"}},{0.0,{"\set_military\set\vest\data\napleshniki_destruct.rvmat","\set_military\set\vest\data\vest_destruct.rvmat"}}};
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
	class Vp_SET19_Vest_Desert_vb: Vp_SET19_Vest_Desert
	{
		scope = 2;
		descriptionShort = "$STR_VBC_Spetsnaz_Vest_DescVB";
		vbSet = "set_military"; // метка сета для скриптовых баффов (vb_bonuses.c)
		visibilityModifier = 0.8; // незаметность для заражённых
		repairableWithKits[] = {2,3,5,8}; // швейный, кожаный, скотч, эпоксидка
		repairCosts[] = {25,25,25,25};
		class Protection
		{
			biological = 1;
			chemical = 1;
		};
		itemsCargoSize[] = {10,6};
		// DamageSystem скопирован из Vp_SET19_Vest_ColorBase целиком (частичное переопределение заменило бы его), броня усилена
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 800;
					transferToAttachmentsCoef = 0.5;
					healthLevels[] = {{1.0,{"\set_military\set\vest\data\napleshniki.rvmat","\set_military\set\vest\data\vest.rvmat"}},{0.7,{"\set_military\set\vest\data\napleshniki.rvmat","\set_military\set\vest\data\vest.rvmat"}},{0.5,{"\set_military\set\vest\data\napleshniki_damage.rvmat","\set_military\set\vest\data\vest_damage.rvmat"}},{0.3,{"\set_military\set\vest\data\napleshniki_damage.rvmat","\set_military\set\vest\data\vest_damage.rvmat"}},{0.0,{"\set_military\set\vest\data\napleshniki_destruct.rvmat","\set_military\set\vest\data\vest_destruct.rvmat"}}};
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
