// DayZavr: улучшенные версии предметов сета (классы *_vb).
// Каждый X_vb наследует X целиком: вид, название, описание, скрипты. Параметры меняем только здесь.

class CfgPatches
{
	class set_Incite_vb
	{
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"Vp_SET17","set_Incite_MagazineSlots"};
	};
};

class CfgVehicles
{
	class VP_SET17_Belt_Black;
	class VP_SET17_Boots_Black;
	class Vp_SET17_Gloves_Black;
	class Vp_Set17_Helmet_Black;
	class Vp_SET17_Pants_Black;
	class Vp_SET17_Top_Black;
	class Vp_SET17_Vest_Black;

	class VP_SET17_Belt_Black_vb: VP_SET17_Belt_Black
	{
		scope = 2;
		descriptionShort = "$STR_VBC_Exo_Belt_DescVB";
		vbSet = "set_Incite"; // метка сета для скриптовых баффов (vb_bonuses.c)
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
	class VP_SET17_Boots_Black_vb: VP_SET17_Boots_Black
	{
		scope = 2;
		descriptionShort = "$STR_VBC_Exo_Shoes_DescVB";
		vbSet = "set_Incite"; // метка сета для скриптовых баффов (vb_bonuses.c)
		visibilityModifier = 0.8; // незаметность для заражённых
		repairableWithKits[] = {2,3,5,8}; // швейный, кожаный, скотч, эпоксидка
		repairCosts[] = {25,25,25,25};
		soundAttType = "Sneakers"; // тихий шаг для заражённых
		class Protection
		{
			biological = 1;
			chemical = 1;
		};
		// DamageSystem скопирован из VP_SET17_Boots_ColorBase целиком (частичное переопределение заменило бы его), броня усилена
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 500;
					healthLevels[] = {{1,{"\set_Incite\set\boots\data\boots.rvmat"}},{0.7,{"\set_Incite\set\boots\data\boots.rvmat"}},{0.5,{"\set_Incite\set\boots\data\boots_damage.rvmat"}},{0.3,{"\set_Incite\set\boots\data\boots_damage.rvmat"}},{0,{"\set_Incite\set\boots\data\boots_destruct.rvmat"}}};
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
	class Vp_SET17_Gloves_Black_vb: Vp_SET17_Gloves_Black
	{
		scope = 2;
		descriptionShort = "$STR_VBC_Exo_Gloves_DescVB";
		vbSet = "set_Incite"; // метка сета для скриптовых баффов (vb_bonuses.c)
		visibilityModifier = 0.8; // незаметность для заражённых
		repairableWithKits[] = {2,3,5,8}; // швейный, кожаный, скотч, эпоксидка
		repairCosts[] = {25,25,25,25};
		class Protection
		{
			biological = 1;
			chemical = 1;
		};
		// DamageSystem скопирован из Vp_SET17_Gloves_ColorBase целиком (частичное переопределение заменило бы его), броня усилена
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 400;
					healthLevels[] = {{1.0,{"\set_Incite\set\gloves\data\gloves.rvmat"}},{0.7,{"\set_Incite\set\gloves\data\gloves.rvmat"}},{0.5,{"\set_Incite\set\gloves\data\gloves_damage.rvmat"}},{0.3,{"\set_Incite\set\gloves\data\gloves_damage.rvmat"}},{0.0,{"\set_Incite\set\gloves\data\gloves_destruct.rvmat"}}};
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
	class Vp_Set17_Helmet_Black_vb: Vp_Set17_Helmet_Black
	{
		scope = 2;
		descriptionShort = "$STR_VBC_Exo_Helmet_DescVB";
		vbSet = "set_Incite"; // метка сета для скриптовых баффов (vb_bonuses.c)
		visibilityModifier = 0.8; // незаметность для заражённых
		repairableWithKits[] = {2,3,5,8}; // швейный, кожаный, скотч, эпоксидка
		repairCosts[] = {25,25,25,25};
		class Protection
		{
			biological = 1;
			chemical = 1;
		};
		// DamageSystem скопирован из Vp_Set17_Helmet_ColorBase целиком (частичное переопределение заменило бы его), броня усилена
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 400;
					transferToAttachmentsCoef = 0.5;
					healthLevels[] = {{1.0,{"\set_Incite\set\helmet\data\helmet.rvmat","\set_Incite\set\helmet\data\konektori.rvmat","\set_Incite\set\helmet\data\nakladki_shlem.rvmat","\set_Incite\set\helmet\data\trubki.rvmat","\set_Incite\set\helmet\data\vizor.rvmat"}},{0.7,{"\set_Incite\set\helmet\data\helmet.rvmat","\set_Incite\set\helmet\data\konektori.rvmat","\set_Incite\set\helmet\data\nakladki_shlem.rvmat","\set_Incite\set\helmet\data\trubki.rvmat","\set_Incite\set\helmet\data\vizor.rvmat"}},{0.5,{"\set_Incite\set\helmet\data\helmet_damage.rvmat","\set_Incite\set\helmet\data\konektori_damage.rvmat","\set_Incite\set\helmet\data\nakladki_shlem_damage.rvmat","\set_Incite\set\helmet\data\trubki_damage.rvmat","\set_Incite\set\helmet\data\vizor_damage.rvmat"}},{0.3,{"\set_Incite\set\helmet\data\helmet_damage.rvmat","\set_Incite\set\helmet\data\konektori_damage.rvmat","\set_Incite\set\helmet\data\nakladki_shlem_damage.rvmat","\set_Incite\set\helmet\data\trubki_damage.rvmat","\set_Incite\set\helmet\data\vizor_damage.rvmat"}},{0.0,{"\set_Incite\set\helmet\data\helmet_destruct.rvmat","\set_Incite\set\helmet\data\konektori_destruct.rvmat","\set_Incite\set\helmet\data\nakladki_shlem_destruct.rvmat","\set_Incite\set\helmet\data\trubki_destruct.rvmat","\set_Incite\set\helmet\data\vizor_destruct.rvmat"}}};
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
	class Vp_SET17_Pants_Black_vb: Vp_SET17_Pants_Black
	{
		scope = 2;
		descriptionShort = "$STR_VBC_Exo_Pants_DescVB";
		vbSet = "set_Incite"; // метка сета для скриптовых баффов (vb_bonuses.c)
		visibilityModifier = 0.8; // незаметность для заражённых
		repairableWithKits[] = {2,3,5,8}; // швейный, кожаный, скотч, эпоксидка
		repairCosts[] = {25,25,25,25};
		class Protection
		{
			biological = 1;
			chemical = 1;
		};
		itemsCargoSize[] = {10,7};
		// DamageSystem скопирован из Vp_SET17_Pants_ColorBase целиком (частичное переопределение заменило бы его), броня усилена
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 500;
					transferToAttachmentsCoef = 0.5;
					healthLevels[] = {{1.0,{"\set_Incite\set\pants\data\Pants.rvmat","\set_Incite\set\pants\data\koleno.rvmat","\set_Incite\set\pants\data\krep_top_pants.rvmat"}},{0.7,{"\set_Incite\set\pants\data\Pants.rvmat","\set_Incite\set\pants\data\koleno.rvmat","\set_Incite\set\pants\data\krep_top_pants.rvmat"}},{0.5,{"\set_Incite\set\pants\data\Pants_damage.rvmat","\set_Incite\set\pants\data\koleno_damage.rvmat","\set_Incite\set\pants\data\krep_top_pants_damage.rvmat"}},{0.3,{"\set_Incite\set\pants\data\Pants_damage.rvmat","\set_Incite\set\pants\data\koleno_damage.rvmat","\set_Incite\set\pants\data\krep_top_pants_damage.rvmat"}},{0.0,{"\set_Incite\set\pants\data\Pants_destruct.rvmat","\set_Incite\set\pants\data\koleno_destruct.rvmat","\set_Incite\set\pants\data\krep_top_pants_destruct.rvmat"}}};
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
	class Vp_SET17_Top_Black_vb: Vp_SET17_Top_Black
	{
		scope = 2;
		descriptionShort = "$STR_VBC_Exo_Top_DescVB";
		vbSet = "set_Incite"; // метка сета для скриптовых баффов (vb_bonuses.c)
		visibilityModifier = 0.8; // незаметность для заражённых
		repairableWithKits[] = {2,3,5,8}; // швейный, кожаный, скотч, эпоксидка
		repairCosts[] = {25,25,25,25};
		class Protection
		{
			biological = 1;
			chemical = 1;
		};
		itemsCargoSize[] = {10,8};
		// DamageSystem скопирован из Vp_SET17_Top_ColorBase целиком (частичное переопределение заменило бы его), броня усилена
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 500;
					transferToAttachmentsCoef = 0.5;
					healthLevels[] = {{1.0,{"\set_Incite\set\top\data\top.rvmat","\set_Incite\set\top\data\gorlovina.rvmat","\set_Incite\set\top\data\krep_top.rvmat","\set_Incite\set\top\data\nadpleshniki.rvmat","\set_Incite\set\top\data\napleshniki_relief.rvmat","\set_Incite\set\top\data\predpleshie.rvmat"}},{0.7,{"\set_Incite\set\top\data\top.rvmat","\set_Incite\set\top\data\gorlovina.rvmat","\set_Incite\set\top\data\krep_top.rvmat","\set_Incite\set\top\data\nadpleshniki.rvmat","\set_Incite\set\top\data\napleshniki_relief.rvmat","\set_Incite\set\top\data\predpleshie.rvmat"}},{0.5,{"\set_Incite\set\top\data\top_damage.rvmat","\set_Incite\set\top\data\gorlovina_damage.rvmat","\set_Incite\set\top\data\krep_top_damage.rvmat","\set_Incite\set\top\data\nadpleshniki_damage.rvmat","\set_Incite\set\top\data\napleshniki_relief_damage.rvmat","\set_Incite\set\top\data\predpleshie_damage.rvmat"}},{0.3,{"\set_Incite\set\top\data\top_damage.rvmat","\set_Incite\set\top\data\gorlovina_damage.rvmat","\set_Incite\set\top\data\krep_top_damage.rvmat","\set_Incite\set\top\data\nadpleshniki_damage.rvmat","\set_Incite\set\top\data\napleshniki_relief_damage.rvmat","\set_Incite\set\top\data\predpleshie_damage.rvmat"}},{0.0,{"\set_Incite\set\top\data\top_destruct.rvmat","\set_Incite\set\top\data\gorlovina_destruct.rvmat","\set_Incite\set\top\data\krep_top_destruct.rvmat","\set_Incite\set\top\data\nadpleshniki_destruct.rvmat","\set_Incite\set\top\data\napleshniki_relief_destruct.rvmat","\set_Incite\set\top\data\predpleshie_destruct.rvmat"}}};
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
	class Vp_SET17_Vest_Black_vb: Vp_SET17_Vest_Black
	{
		scope = 2;
		descriptionShort = "$STR_VBC_Exo_Vest_DescVB";
		vbSet = "set_Incite"; // метка сета для скриптовых баффов (vb_bonuses.c)
		visibilityModifier = 0.8; // незаметность для заражённых
		repairableWithKits[] = {2,3,5,8}; // швейный, кожаный, скотч, эпоксидка
		repairCosts[] = {25,25,25,25};
		class Protection
		{
			biological = 1;
			chemical = 1;
		};
		itemsCargoSize[] = {10,6};
		// DamageSystem скопирован из Vp_SET17_Vest_ColorBase целиком (частичное переопределение заменило бы его), броня усилена
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 800;
					transferToAttachmentsCoef = 0.5;
					healthLevels[] = {{1.0,{"\set_Incite\set\vest\data\backpack.rvmat","\set_Incite\set\vest\data\karmani_vest.rvmat","\set_Incite\set\vest\data\krep_knife_vest.rvmat","\set_Incite\set\vest\data\ruka_knief_vest.rvmat","\set_Incite\set\vest\data\small_magazi_vest.rvmat","\set_Incite\set\vest\data\vest.rvmat","\set_Incite\set\vest\data\vest_podushki.rvmat","\set_Incite\set\vest\data\vest_relief.rvmat","\set_Incite\set\vest\data\vest1.rvmat"}},{0.7,{"\set_Incite\set\vest\data\backpack.rvmat","\set_Incite\set\vest\data\karmani_vest.rvmat","\set_Incite\set\vest\data\krep_knife_vest.rvmat","\set_Incite\set\vest\data\ruka_knief_vest.rvmat","\set_Incite\set\vest\data\small_magazi_vest.rvmat","\set_Incite\set\vest\data\vest.rvmat","\set_Incite\set\vest\data\vest_podushki.rvmat","\set_Incite\set\vest\data\vest_relief.rvmat","\set_Incite\set\vest\data\vest1.rvmat"}},{0.5,{"\set_Incite\set\vest\data\backpack_damage.rvmat","\set_Incite\set\vest\data\karmani_vest_damage.rvmat","\set_Incite\set\vest\data\krep_knife_vest_damage.rvmat","\set_Incite\set\vest\data\ruka_knief_vest_damage.rvmat","\set_Incite\set\vest\data\small_magazi_vest_damage.rvmat","\set_Incite\set\vest\data\vest_damage.rvmat","\set_Incite\set\vest\data\vest_podushki_damage.rvmat","\set_Incite\set\vest\data\vest_relief_damage.rvmat","\set_Incite\set\vest\data\vest1_damage.rvmat"}},{0.3,{"\set_Incite\set\vest\data\backpack_damage.rvmat","\set_Incite\set\vest\data\karmani_vest_damage.rvmat","\set_Incite\set\vest\data\krep_knife_vest_damage.rvmat","\set_Incite\set\vest\data\ruka_knief_vest_damage.rvmat","\set_Incite\set\vest\data\small_magazi_vest_damage.rvmat","\set_Incite\set\vest\data\vest_damage.rvmat","\set_Incite\set\vest\data\vest_podushki_damage.rvmat","\set_Incite\set\vest\data\vest_relief_damage.rvmat","\set_Incite\set\vest\data\vest1_damage.rvmat"}},{0.0,{"\set_Incite\set\vest\data\backpack_destruct.rvmat","\set_Incite\set\vest\data\karmani_vest_destruct.rvmat","\set_Incite\set\vest\data\krep_knife_vest_destruct.rvmat","\set_Incite\set\vest\data\ruka_knief_vest_destruct.rvmat","\set_Incite\set\vest\data\small_magazi_vest_destruct.rvmat","\set_Incite\set\vest\data\vest_destruct.rvmat","\set_Incite\set\vest\data\vest_podushki_destruct.rvmat","\set_Incite\set\vest\data\vest_relief_destruct.rvmat","\set_Incite\set\vest\data\vest1_destruct.rvmat"}}};
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
