// DayZavr: улучшенные версии предметов сета (классы *_vb).
// Каждый X_vb наследует X целиком: вид, название, описание, скрипты. Параметры меняем только здесь.

class CfgPatches
{
	class set_demon_vb
	{
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"set_demon","Vp_SET22"};
	};
};

class CfgVehicles
{
	class Vp_Set22_CAP_Red;
	class Vp_SET22_Gloves_Red;
	class Vp_Set22_Mask_Red;
	class Vp_SET22_Pants_Red;
	class VP_SET22_Shoes_Red;
	class Vp_SET22_Top_Red;
	class Vp_SET22_Vest_Red;

	class Vp_Set22_CAP_Red_vb: Vp_Set22_CAP_Red
	{
		scope = 2;
		descriptionShort = "$STR_VBC_Demon_Cap_DescVB";
		vbSet = "set_demon"; // метка сета для скриптовых баффов (vb_bonuses.c)
		visibilityModifier = 0.8; // незаметность для заражённых
		repairableWithKits[] = {2,3,5,8}; // швейный, кожаный, скотч, эпоксидка
		repairCosts[] = {25,25,25,25};
		class Protection
		{
			biological = 1;
			chemical = 1;
		};
		// DamageSystem скопирован из Vp_Set22_cap_ColorBase целиком (частичное переопределение заменило бы его), броня усилена
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 300;
					transferToAttachmentsCoef = 0.5;
					healthLevels[] = {{1.0, {"\set_demon\set\cap\data\cap.rvmat"}},
									 {0.7, {"\set_demon\set\cap\data\cap.rvmat"}},
									 {0.5, {"\set_demon\set\cap\data\cap_damage.rvmat"}},
									 {0.3, {"\set_demon\set\cap\data\cap_damage.rvmat"}},
									 {0.0, {"\set_demon\set\cap\data\cap_destruct.rvmat"}}};
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
	class Vp_SET22_Gloves_Red_vb: Vp_SET22_Gloves_Red
	{
		scope = 2;
		descriptionShort = "$STR_VBC_Demon_Gloves_DescVB";
		vbSet = "set_demon"; // метка сета для скриптовых баффов (vb_bonuses.c)
		visibilityModifier = 0.8; // незаметность для заражённых
		repairableWithKits[] = {2,3,5,8}; // швейный, кожаный, скотч, эпоксидка
		repairCosts[] = {25,25,25,25};
		class Protection
		{
			biological = 1;
			chemical = 1;
		};
		// DamageSystem скопирован из Vp_SET22_Gloves_ColorBase целиком (частичное переопределение заменило бы его), броня усилена
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 400;
					healthLevels[] = {{1.0, {"\set_demon\set\gloves\data\Gloves.rvmat"}},
									 {0.7, {"\set_demon\set\gloves\data\Gloves.rvmat"}},
									 {0.5, {"\set_demon\set\gloves\data\Gloves_damage.rvmat"}},
									 {0.3, {"\set_demon\set\gloves\data\Gloves_damage.rvmat"}},
									 {0.0, {"\set_demon\set\gloves\data\Gloves_destruct.rvmat"}}};
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
	class Vp_Set22_Mask_Red_vb: Vp_Set22_Mask_Red
	{
		scope = 2;
		descriptionShort = "$STR_VBC_Demon_Mask_DescVB";
		vbSet = "set_demon"; // метка сета для скриптовых баффов (vb_bonuses.c)
		visibilityModifier = 0.8; // незаметность для заражённых
		repairableWithKits[] = {2,3,5,8}; // швейный, кожаный, скотч, эпоксидка
		repairCosts[] = {25,25,25,25};
		class Protection
		{
			biological = 1;
			chemical = 1;
		};
	};
	class Vp_SET22_Pants_Red_vb: Vp_SET22_Pants_Red
	{
		scope = 2;
		descriptionShort = "$STR_VBC_Demon_Pants_DescVB";
		vbSet = "set_demon"; // метка сета для скриптовых баффов (vb_bonuses.c)
		visibilityModifier = 0.8; // незаметность для заражённых
		repairableWithKits[] = {2,3,5,8}; // швейный, кожаный, скотч, эпоксидка
		repairCosts[] = {25,25,25,25};
		class Protection
		{
			biological = 1;
			chemical = 1;
		};
		itemsCargoSize[] = {10,7};
		// DamageSystem скопирован из Vp_SET22_Pants_ColorBase целиком (частичное переопределение заменило бы его), броня усилена
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 500;
					transferToAttachmentsCoef = 0.5;
					healthLevels[] = {{1.0, {"\set_demon\set\pants\data\pants.rvmat", "\set_demon\set\pants\data\pants2.rvmat"}},
									 {0.7, {"\set_demon\set\pants\data\pants.rvmat", "\set_demon\set\pants\data\pants2.rvmat"}},
									 {0.5, {"\set_demon\set\pants\data\pants_damage.rvmat", "\set_demon\set\pants\data\pants2_damage.rvmat"}},
									 {0.3, {"\set_demon\set\pants\data\pants_damage.rvmat", "\set_demon\set\pants\data\pants2_damage.rvmat"}},
									 {0.0, {"\set_demon\set\pants\data\pants_destruct.rvmat", "\set_demon\set\pants\data\pants2_destruct.rvmat"}}};
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
	class VP_SET22_Shoes_Red_vb: VP_SET22_Shoes_Red
	{
		scope = 2;
		descriptionShort = "$STR_VBC_Demon_Shoes_DescVB";
		vbSet = "set_demon"; // метка сета для скриптовых баффов (vb_bonuses.c)
		visibilityModifier = 0.8; // незаметность для заражённых
		repairableWithKits[] = {2,3,5,8}; // швейный, кожаный, скотч, эпоксидка
		repairCosts[] = {25,25,25,25};
		soundAttType = "Sneakers"; // тихий шаг для заражённых
		class Protection
		{
			biological = 1;
			chemical = 1;
		};
		// DamageSystem скопирован из VP_SET22_Shoes_ColorBase целиком (частичное переопределение заменило бы его), броня усилена
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 500;
					healthLevels[] = {{1, {"\set_demon\set\shoes\data\shoes.rvmat"}},
									{0.7, {"\set_demon\set\shoes\data\shoes.rvmat"}},
									{0.5, {"\set_demon\set\shoes\data\shoes_damage.rvmat"}},
									{0.3, {"\set_demon\set\shoes\data\shoes_damage.rvmat"}},
									{0, {"\set_demon\set\shoes\data\shoes_destruct.rvmat"}}};
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
	class Vp_SET22_Top_Red_vb: Vp_SET22_Top_Red
	{
		scope = 2;
		descriptionShort = "$STR_VBC_Demon_Top_DescVB";
		vbSet = "set_demon"; // метка сета для скриптовых баффов (vb_bonuses.c)
		visibilityModifier = 0.8; // незаметность для заражённых
		repairableWithKits[] = {2,3,5,8}; // швейный, кожаный, скотч, эпоксидка
		repairCosts[] = {25,25,25,25};
		class Protection
		{
			biological = 1;
			chemical = 1;
		};
		itemsCargoSize[] = {10,8};
		// DamageSystem скопирован из Vp_SET22_Top_ColorBase целиком (частичное переопределение заменило бы его), броня усилена
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 500;
					transferToAttachmentsCoef = 0.5;
					healthLevels[] = {{1.0, {"\set_demon\set\top\data\top.rvmat", "\set_demon\set\top\data\top1.rvmat"}},
									 {0.7, {"\set_demon\set\top\data\top.rvmat", "\set_demon\set\top\data\top1.rvmat"}},
									 {0.5, {"\set_demon\set\top\data\top_damage.rvmat", "\set_demon\set\top\data\top1_damage.rvmat"}},
									 {0.3, {"\set_demon\set\top\data\top_damage.rvmat", "\set_demon\set\top\data\top1_damage.rvmat"}},
									 {0.0, {"\set_demon\set\top\data\top_destruct.rvmat", "\set_demon\set\top\data\top1_destruct.rvmat"}}};
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
	class Vp_SET22_Vest_Red_vb: Vp_SET22_Vest_Red
	{
		scope = 2;
		descriptionShort = "$STR_VBC_Demon_Vest_DescVB";
		vbSet = "set_demon"; // метка сета для скриптовых баффов (vb_bonuses.c)
		visibilityModifier = 0.8; // незаметность для заражённых
		repairableWithKits[] = {2,3,5,8}; // швейный, кожаный, скотч, эпоксидка
		repairCosts[] = {25,25,25,25};
		class Protection
		{
			biological = 1;
			chemical = 1;
		};
		itemsCargoSize[] = {10,6};
		// DamageSystem скопирован из Vp_SET22_Vest_ColorBase целиком (частичное переопределение заменило бы его), броня усилена
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 800;
					transferToAttachmentsCoef = 0.5;
					healthLevels[] = {{1.0, {"\set_demon\set\vest\data\vest.rvmat", "\set_demon\set\vest\data\vest1.rvmat", "\set_demon\set\vest\data\vest2.rvmat"}},
									 {0.7, {"\set_demon\set\vest\data\vest.rvmat", "\set_demon\set\vest\data\vest1.rvmat", "\set_demon\set\vest\data\vest2.rvmat"}},
									 {0.5, {"\set_demon\set\vest\data\vest_damage.rvmat", "\set_demon\set\vest\data\vest1_damage.rvmat", "\set_demon\set\vest\data\vest2_damage.rvmat"}},
									 {0.3, {"\set_demon\set\vest\data\vest_damage.rvmat", "\set_demon\set\vest\data\vest1_damage.rvmat", "\set_demon\set\vest\data\vest2_damage.rvmat"}},
									 {0.0, {"\set_demon\set\vest\data\vest_destruct.rvmat", "\set_demon\set\vest\data\vest1_destruct.rvmat", "\set_demon\set\vest\data\vest2_destruct.rvmat"}}};
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
