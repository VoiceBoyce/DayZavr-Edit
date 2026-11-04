// DayZavr: улучшенные версии предметов сета (классы *_vb).
// Каждый X_vb наследует X целиком: вид, название, описание, скрипты. Параметры меняем только здесь.

class CfgPatches
{
	class set_operator_vb
	{
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"set_operator","VP_SET23"};
	};
};

class CfgVehicles
{
	class VP_SET23_Belt_Black;
	class Vp_Set23_Helmet_Black;
	class Vp_SET23_Pants_Black;
	class VP_SET23_Shoes_Black;
	class Vp_SET23_Top_Black;
	class Vp_SET23_Vest_Black;

	class VP_SET23_Belt_Black_vb: VP_SET23_Belt_Black
	{
		scope = 2;
		descriptionShort = "$STR_VBC_Operator_Belt_DescVB";
		vbSet = "set_operator"; // метка сета для скриптовых баффов (vb_bonuses.c)
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
	class Vp_Set23_Helmet_Black_vb: Vp_Set23_Helmet_Black
	{
		scope = 2;
		descriptionShort = "$STR_VBC_Operator_Helmet_DescVB";
		vbSet = "set_operator"; // метка сета для скриптовых баффов (vb_bonuses.c)
		visibilityModifier = 0.8; // незаметность для заражённых
		repairableWithKits[] = {2,3,5,8}; // швейный, кожаный, скотч, эпоксидка
		repairCosts[] = {25,25,25,25};
		class Protection
		{
			biological = 1;
			chemical = 1;
		};
		// DamageSystem скопирован из Vp_Set23_Helmet_ColorBase целиком (частичное переопределение заменило бы его), броня усилена
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 400;
					transferToAttachmentsCoef = 0.5;
					healthLevels[] = {{1.0, {"\set_operator\set\helmet\data\head1.rvmat", "\set_operator\set\helmet\data\head2.rvmat", "\set_operator\set\helmet\data\head3.rvmat", "\set_operator\set\helmet\data\head4.rvmat", "\set_operator\set\helmet\data\head5.rvmat", "\set_operator\set\helmet\data\head6.rvmat", "\set_operator\set\helmet\data\head7.rvmat"}},
									 {0.7, {"\set_operator\set\helmet\data\head1.rvmat", "\set_operator\set\helmet\data\head2.rvmat", "\set_operator\set\helmet\data\head3.rvmat", "\set_operator\set\helmet\data\head4.rvmat", "\set_operator\set\helmet\data\head5.rvmat", "\set_operator\set\helmet\data\head6.rvmat", "\set_operator\set\helmet\data\head7.rvmat"}},
									 {0.5, {"\set_operator\set\helmet\data\head1_damage.rvmat", "\set_operator\set\helmet\data\head2_damage.rvmat", "\set_operator\set\helmet\data\head3_damage.rvmat", "\set_operator\set\helmet\data\head4_damage.rvmat", "\set_operator\set\helmet\data\head5_damage.rvmat", "\set_operator\set\helmet\data\head6_damage.rvmat", "\set_operator\set\helmet\data\head7_damage.rvmat"}},
									 {0.3, {"\set_operator\set\helmet\data\head1_damage.rvmat", "\set_operator\set\helmet\data\head2_damage.rvmat", "\set_operator\set\helmet\data\head3_damage.rvmat", "\set_operator\set\helmet\data\head4_damage.rvmat", "\set_operator\set\helmet\data\head5_damage.rvmat", "\set_operator\set\helmet\data\head6_damage.rvmat", "\set_operator\set\helmet\data\head7_damage.rvmat"}},
									 {0.0, {"\set_operator\set\helmet\data\head1_destruct.rvmat", "\set_operator\set\helmet\data\head2_destruct.rvmat", "\set_operator\set\helmet\data\head3_destruct.rvmat", "\set_operator\set\helmet\data\head4_destruct.rvmat", "\set_operator\set\helmet\data\head5_destruct.rvmat", "\set_operator\set\helmet\data\head6_destruct.rvmat", "\set_operator\set\helmet\data\head7_destruct.rvmat"}}};
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
	class Vp_SET23_Pants_Black_vb: Vp_SET23_Pants_Black
	{
		scope = 2;
		descriptionShort = "$STR_VBC_Operator_Pants_DescVB";
		vbSet = "set_operator"; // метка сета для скриптовых баффов (vb_bonuses.c)
		visibilityModifier = 0.8; // незаметность для заражённых
		repairableWithKits[] = {2,3,5,8}; // швейный, кожаный, скотч, эпоксидка
		repairCosts[] = {25,25,25,25};
		class Protection
		{
			biological = 1;
			chemical = 1;
		};
		itemsCargoSize[] = {10,7};
		// DamageSystem скопирован из Vp_SET23_Pants_ColorBase целиком (частичное переопределение заменило бы его), броня усилена
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 500;
					transferToAttachmentsCoef = 0.5;
					healthLevels[] = {{1.0, {"\set_operator\set\pants\data\pants.rvmat"}},
									 {0.7, {"\set_operator\set\pants\data\pants.rvmat"}},
									 {0.5, {"\set_operator\set\pants\data\pants_damage.rvmat"}},
									 {0.3, {"\set_operator\set\pants\data\pants_damage.rvmat"}},
									 {0.0, {"\set_operator\set\pants\data\pants_destruct.rvmat"}}};
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
	class VP_SET23_Shoes_Black_vb: VP_SET23_Shoes_Black
	{
		scope = 2;
		descriptionShort = "$STR_VBC_Operator_Shoes_DescVB";
		vbSet = "set_operator"; // метка сета для скриптовых баффов (vb_bonuses.c)
		visibilityModifier = 0.8; // незаметность для заражённых
		repairableWithKits[] = {2,3,5,8}; // швейный, кожаный, скотч, эпоксидка
		repairCosts[] = {25,25,25,25};
		soundAttType = "Sneakers"; // тихий шаг для заражённых
		class Protection
		{
			biological = 1;
			chemical = 1;
		};
		// DamageSystem скопирован из VP_SET23_Shoes_ColorBase целиком (частичное переопределение заменило бы его), броня усилена
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 500;
					healthLevels[] = {{1, {"\set_operator\set\shoes\data\shoes.rvmat"}},
									{0.7, {"\set_operator\set\shoes\data\shoes.rvmat"}},
									{0.5, {"\set_operator\set\shoes\data\shoes_damage.rvmat"}},
									{0.3, {"\set_operator\set\shoes\data\shoes_damage.rvmat"}},
									{0, {"\set_operator\set\shoes\data\shoes_destruct.rvmat"}}};
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
	class Vp_SET23_Top_Black_vb: Vp_SET23_Top_Black
	{
		scope = 2;
		descriptionShort = "$STR_VBC_Operator_Top_DescVB";
		vbSet = "set_operator"; // метка сета для скриптовых баффов (vb_bonuses.c)
		visibilityModifier = 0.8; // незаметность для заражённых
		repairableWithKits[] = {2,3,5,8}; // швейный, кожаный, скотч, эпоксидка
		repairCosts[] = {25,25,25,25};
		class Protection
		{
			biological = 1;
			chemical = 1;
		};
		itemsCargoSize[] = {10,8};
		// DamageSystem скопирован из Vp_SET23_Top_ColorBase целиком (частичное переопределение заменило бы его), броня усилена
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 500;
					transferToAttachmentsCoef = 0.5;
					healthLevels[] = {{1.0, {"\set_operator\set\top\data\top.rvmat", "\set_operator\set\top\data\top1.rvmat"}},
									 {0.7, {"\set_operator\set\top\data\top.rvmat", "\set_operator\set\top\data\top1.rvmat"}},
									 {0.5, {"\set_operator\set\top\data\top_damage.rvmat", "\set_operator\set\top\data\top1_damage.rvmat"}},
									 {0.3, {"\set_operator\set\top\data\top_damage.rvmat", "\set_operator\set\top\data\top1_damage.rvmat"}},
									 {0.0, {"\set_operator\set\top\data\top_destruct.rvmat", "\set_operator\set\top\data\top1_destruct.rvmat"}}};
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
	class Vp_SET23_Vest_Black_vb: Vp_SET23_Vest_Black
	{
		scope = 2;
		descriptionShort = "$STR_VBC_Operator_Vest_DescVB";
		vbSet = "set_operator"; // метка сета для скриптовых баффов (vb_bonuses.c)
		visibilityModifier = 0.8; // незаметность для заражённых
		repairableWithKits[] = {2,3,5,8}; // швейный, кожаный, скотч, эпоксидка
		repairCosts[] = {25,25,25,25};
		class Protection
		{
			biological = 1;
			chemical = 1;
		};
		itemsCargoSize[] = {10,6};
		// DamageSystem скопирован из Vp_SET23_Vest_ColorBase целиком (частичное переопределение заменило бы его), броня усилена
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 800;
					transferToAttachmentsCoef = 0.5;
					healthLevels[] = {{1.0, {"\set_operator\set\vest\data\vest.rvmat", "\set_operator\set\vest\data\vest2.rvmat", "\set_operator\set\vest\data\vest3.rvmat", "\set_operator\set\vest\data\vest4.rvmat"}},
									 {0.7, {"\set_operator\set\vest\data\vest.rvmat", "\set_operator\set\vest\data\vest2.rvmat", "\set_operator\set\vest\data\vest3.rvmat", "\set_operator\set\vest\data\vest4.rvmat"}},
									 {0.5, {"\set_operator\set\vest\data\vest_damage.rvmat", "\set_operator\set\vest\data\vest2_damage.rvmat", "\set_operator\set\vest\data\vest3_damage.rvmat", "\set_operator\set\vest\data\vest4_damage.rvmat"}},
									 {0.3, {"\set_operator\set\vest\data\vest_damage.rvmat", "\set_operator\set\vest\data\vest2_damage.rvmat", "\set_operator\set\vest\data\vest3_damage.rvmat", "\set_operator\set\vest\data\vest4_damage.rvmat"}},
									 {0.0, {"\set_operator\set\vest\data\vest_destruct.rvmat", "\set_operator\set\vest\data\vest2_destruct.rvmat", "\set_operator\set\vest\data\vest3_destruct.rvmat", "\set_operator\set\vest\data\vest4_destruct.rvmat"}}};
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
