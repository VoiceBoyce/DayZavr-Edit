// DayZavr: улучшенные версии предметов сета (классы *_vb).
// Каждый X_vb наследует X целиком: вид, название, описание, скрипты. Параметры меняем только здесь.

class CfgPatches
{
	class set_legion_vb
	{
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"set_Legion","VP_SET20","set_Legion_Top"};
	};
};

class CfgVehicles
{
	class VP_SET20_Shoes;
	class Vp_SET20_Gloves;
	class Vp_SET20_Helmet;
	class Vp_SET20_Helmet_No_Tail;
	class Vp_SET20_Mask;
	class Vp_SET20_Pants;
	class Vp_SET20_Top;
	class Vp_SET20_Vest;

	class VP_SET20_Shoes_vb: VP_SET20_Shoes
	{
		scope = 2;
		descriptionShort = "$STR_VBC_Legion_Boots_DescVB";
		vbSet = "set_legion"; // метка сета для скриптовых баффов (vb_bonuses.c)
		visibilityModifier = 0.8; // незаметность для заражённых
		repairableWithKits[] = {2,3,5,8}; // швейный, кожаный, скотч, эпоксидка
		repairCosts[] = {25,25,25,25};
		soundAttType = "Sneakers"; // тихий шаг для заражённых
		class Protection
		{
			biological = 1;
			chemical = 1;
		};
		// DamageSystem скопирован из VP_SET20_Shoes_ColorBase целиком (частичное переопределение заменило бы его), броня усилена
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 500;
					healthLevels[] = {{1,{"set_legion\set\Boots\data\Boots.rvmat"}},{0.7,{"set_legion\set\Boots\data\Boots.rvmat"}},{0.5,{"set_legion\set\Boots\data\Boots_damage.rvmat"}},{0.3,{"set_legion\set\Boots\data\Boots_damage.rvmat"}},{0,{"set_legion\set\Boots\data\Boots_destruct.rvmat"}}};
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
	class Vp_SET20_Gloves_vb: Vp_SET20_Gloves
	{
		scope = 2;
		descriptionShort = "$STR_VBC_Legion_Gloves_DescVB";
		vbSet = "set_legion"; // метка сета для скриптовых баффов (vb_bonuses.c)
		visibilityModifier = 0.8; // незаметность для заражённых
		repairableWithKits[] = {2,3,5,8}; // швейный, кожаный, скотч, эпоксидка
		repairCosts[] = {25,25,25,25};
		class Protection
		{
			biological = 1;
			chemical = 1;
		};
		// DamageSystem скопирован из Vp_SET20_Gloves_ColorBase целиком (частичное переопределение заменило бы его), броня усилена
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 400;
					healthLevels[] = {{1,{"set_legion\set\Gloves\data\gloves.rvmat"}},{0.7,{"set_legion\set\Gloves\data\gloves.rvmat"}},{0.5,{"set_legion\set\Gloves\data\gloves_damage.rvmat"}},{0.3,{"set_legion\set\Gloves\data\gloves_damage.rvmat"}},{0,{"set_legion\set\Gloves\data\gloves_destruct.rvmat"}}};
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
	class Vp_SET20_Helmet_vb: Vp_SET20_Helmet
	{
		scope = 2;
		descriptionShort = "$STR_VBC_Legion_Helmet_DescVB";
		vbSet = "set_legion"; // метка сета для скриптовых баффов (vb_bonuses.c)
		visibilityModifier = 0.8; // незаметность для заражённых
		repairableWithKits[] = {2,3,5,8}; // швейный, кожаный, скотч, эпоксидка
		repairCosts[] = {25,25,25,25};
		class Protection
		{
			biological = 1;
			chemical = 1;
		};
		// DamageSystem скопирован из Vp_SET20_Helmet_ColorBase целиком (частичное переопределение заменило бы его), броня усилена
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 400;
					healthLevels[] = {{1,{"set_legion\set\Helmet\data\bolti.rvmat","set_legion\set\Helmet\data\helmet.rvmat","set_legion\set\Helmet\data\krep_helmet.rvmat","set_legion\set\Helmet\data\krep_xvost.rvmat","set_legion\set\Helmet\data\naushnik.rvmat","set_legion\set\Helmet\data\xvost.rvmat","set_legion\set\Helmet\data\zalypa1.rvmat","set_legion\set\Helmet\data\zalypa2.rvmat"}},{0.7,{"set_legion\set\Helmet\data\bolti.rvmat","set_legion\set\Helmet\data\helmet.rvmat","set_legion\set\Helmet\data\krep_helmet.rvmat","set_legion\set\Helmet\data\krep_xvost.rvmat","set_legion\set\Helmet\data\naushnik.rvmat","set_legion\set\Helmet\data\xvost.rvmat","set_legion\set\Helmet\data\zalypa1.rvmat","set_legion\set\Helmet\data\zalypa2.rvmat"}},{0.5,{"set_legion\set\Helmet\data\bolti_damage.rvmat","set_legion\set\Helmet\data\helmet_damage.rvmat","set_legion\set\Helmet\data\krep_helmet_damage.rvmat","set_legion\set\Helmet\data\krep_xvost_damage.rvmat","set_legion\set\Helmet\data\naushnik_damage.rvmat","set_legion\set\Helmet\data\xvost_damage.rvmat","set_legion\set\Helmet\data\zalypa1_damage.rvmat","set_legion\set\Helmet\data\zalypa2_damage.rvmat"}},{0.3,{"set_legion\set\Helmet\data\bolti_damage.rvmat","set_legion\set\Helmet\data\helmet_damage.rvmat","set_legion\set\Helmet\data\krep_helmet_damage.rvmat","set_legion\set\Helmet\data\krep_xvost_damage.rvmat","set_legion\set\Helmet\data\naushnik_damage.rvmat","set_legion\set\Helmet\data\xvost_damage.rvmat","set_legion\set\Helmet\data\zalypa1_damage.rvmat","set_legion\set\Helmet\data\zalypa2_damage.rvmat"}},{0,{"set_legion\set\Helmet\data\bolti_destruct.rvmat","set_legion\set\Helmet\data\helmet_destruct.rvmat","set_legion\set\Helmet\data\krep_helmet_destruct.rvmat","set_legion\set\Helmet\data\krep_xvost_destruct.rvmat","set_legion\set\Helmet\data\naushnik_destruct.rvmat","set_legion\set\Helmet\data\xvost_destruct.rvmat","set_legion\set\Helmet\data\zalypa1_destruct.rvmat","set_legion\set\Helmet\data\zalypa2_destruct.rvmat"}}};
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
	class Vp_SET20_Helmet_No_Tail_vb: Vp_SET20_Helmet_No_Tail
	{
		scope = 2;
		descriptionShort = "$STR_VBC_Legion_Helmet_DescVB";
		vbSet = "set_legion"; // метка сета для скриптовых баффов (vb_bonuses.c)
		visibilityModifier = 0.8; // незаметность для заражённых
		repairableWithKits[] = {2,3,5,8}; // швейный, кожаный, скотч, эпоксидка
		repairCosts[] = {25,25,25,25};
		class Protection
		{
			biological = 1;
			chemical = 1;
		};
		// DamageSystem скопирован из Vp_SET20_Helmet_ColorBase целиком (частичное переопределение заменило бы его), броня усилена
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 400;
					healthLevels[] = {{1,{"set_legion\set\Helmet\data\bolti.rvmat","set_legion\set\Helmet\data\helmet.rvmat","set_legion\set\Helmet\data\krep_helmet.rvmat","set_legion\set\Helmet\data\krep_xvost.rvmat","set_legion\set\Helmet\data\naushnik.rvmat","set_legion\set\Helmet\data\xvost.rvmat","set_legion\set\Helmet\data\zalypa1.rvmat","set_legion\set\Helmet\data\zalypa2.rvmat"}},{0.7,{"set_legion\set\Helmet\data\bolti.rvmat","set_legion\set\Helmet\data\helmet.rvmat","set_legion\set\Helmet\data\krep_helmet.rvmat","set_legion\set\Helmet\data\krep_xvost.rvmat","set_legion\set\Helmet\data\naushnik.rvmat","set_legion\set\Helmet\data\xvost.rvmat","set_legion\set\Helmet\data\zalypa1.rvmat","set_legion\set\Helmet\data\zalypa2.rvmat"}},{0.5,{"set_legion\set\Helmet\data\bolti_damage.rvmat","set_legion\set\Helmet\data\helmet_damage.rvmat","set_legion\set\Helmet\data\krep_helmet_damage.rvmat","set_legion\set\Helmet\data\krep_xvost_damage.rvmat","set_legion\set\Helmet\data\naushnik_damage.rvmat","set_legion\set\Helmet\data\xvost_damage.rvmat","set_legion\set\Helmet\data\zalypa1_damage.rvmat","set_legion\set\Helmet\data\zalypa2_damage.rvmat"}},{0.3,{"set_legion\set\Helmet\data\bolti_damage.rvmat","set_legion\set\Helmet\data\helmet_damage.rvmat","set_legion\set\Helmet\data\krep_helmet_damage.rvmat","set_legion\set\Helmet\data\krep_xvost_damage.rvmat","set_legion\set\Helmet\data\naushnik_damage.rvmat","set_legion\set\Helmet\data\xvost_damage.rvmat","set_legion\set\Helmet\data\zalypa1_damage.rvmat","set_legion\set\Helmet\data\zalypa2_damage.rvmat"}},{0,{"set_legion\set\Helmet\data\bolti_destruct.rvmat","set_legion\set\Helmet\data\helmet_destruct.rvmat","set_legion\set\Helmet\data\krep_helmet_destruct.rvmat","set_legion\set\Helmet\data\krep_xvost_destruct.rvmat","set_legion\set\Helmet\data\naushnik_destruct.rvmat","set_legion\set\Helmet\data\xvost_destruct.rvmat","set_legion\set\Helmet\data\zalypa1_destruct.rvmat","set_legion\set\Helmet\data\zalypa2_destruct.rvmat"}}};
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
	class Vp_SET20_Mask_vb: Vp_SET20_Mask
	{
		scope = 2;
		descriptionShort = "$STR_VBC_Legion_Mask_DescVB";
		vbSet = "set_legion"; // метка сета для скриптовых баффов (vb_bonuses.c)
		visibilityModifier = 0.8; // незаметность для заражённых
		repairableWithKits[] = {2,3,5,8}; // швейный, кожаный, скотч, эпоксидка
		repairCosts[] = {25,25,25,25};
		class Protection
		{
			biological = 1;
			chemical = 1;
		};
	};
	class Vp_SET20_Pants_vb: Vp_SET20_Pants
	{
		scope = 2;
		descriptionShort = "$STR_VBC_Legion_Pants_DescVB";
		vbSet = "set_legion"; // метка сета для скриптовых баффов (vb_bonuses.c)
		visibilityModifier = 0.8; // незаметность для заражённых
		repairableWithKits[] = {2,3,5,8}; // швейный, кожаный, скотч, эпоксидка
		repairCosts[] = {25,25,25,25};
		class Protection
		{
			biological = 1;
			chemical = 1;
		};
		itemsCargoSize[] = {10,7};
		// DamageSystem скопирован из Vp_SET20_Pants_ColorBase целиком (частичное переопределение заменило бы его), броня усилена
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 500;
					healthLevels[] = {{1,{"set_legion\set\Pants\data\koleno.rvmat","set_legion\set\Pants\data\pants.rvmat"}},{0.7,{"set_legion\set\Pants\data\koleno.rvmat","set_legion\set\Pants\data\pants.rvmat"}},{0.5,{"set_legion\set\Pants\data\koleno_damage.rvmat","set_legion\set\Pants\data\pants_damage.rvmat"}},{0.3,{"set_legion\set\Pants\data\koleno_damage.rvmat","set_legion\set\Pants\data\pants_damage.rvmat"}},{0,{"set_legion\set\Pants\data\koleno_destruct.rvmat","set_legion\set\Pants\data\pants_destruct.rvmat"}}};
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
	class Vp_SET20_Top_vb: Vp_SET20_Top
	{
		scope = 2;
		descriptionShort = "$STR_VBC_Legion_Top_DescVB";
		vbSet = "set_legion"; // метка сета для скриптовых баффов (vb_bonuses.c)
		visibilityModifier = 0.8; // незаметность для заражённых
		repairableWithKits[] = {2,3,5,8}; // швейный, кожаный, скотч, эпоксидка
		repairCosts[] = {25,25,25,25};
		class Protection
		{
			biological = 1;
			chemical = 1;
		};
		itemsCargoSize[] = {10,8};
		// DamageSystem скопирован из Vp_SET20_Top_ColorBase целиком (частичное переопределение заменило бы его), броня усилена
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 500;
					healthLevels[] = {{1,{"set_legion\set\Top\data\top.rvmat","set_legion\set\Top\data\top1.rvmat","set_legion\set\Top\data\top2.rvmat","set_legion\set\Top\data\znak.rvmat"}},{0.7,{"set_legion\set\Top\data\top.rvmat","set_legion\set\Top\data\top1.rvmat","set_legion\set\Top\data\top2.rvmat","set_legion\set\Top\data\znak.rvmat"}},{0.5,{"set_legion\set\Top\data\top_damage.rvmat","set_legion\set\Top\data\top1_damage.rvmat","set_legion\set\Top\data\top2_damage.rvmat","set_legion\set\Top\data\znak_damage.rvmat"}},{0.3,{"set_legion\set\Top\data\top_damage.rvmat","set_legion\set\Top\data\top1_damage.rvmat","set_legion\set\Top\data\top2_damage.rvmat","set_legion\set\Top\data\znak_damage.rvmat"}},{0,{"set_legion\set\Top\data\top_destruct.rvmat","set_legion\set\Top\data\top1_destruct.rvmat","set_legion\set\Top\data\top2_destruct.rvmat","set_legion\set\Top\data\znak_destruct.rvmat"}}};
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
	class Vp_SET20_Vest_vb: Vp_SET20_Vest
	{
		scope = 2;
		descriptionShort = "$STR_VBC_Legion_Vest_DescVB";
		vbSet = "set_legion"; // метка сета для скриптовых баффов (vb_bonuses.c)
		visibilityModifier = 0.8; // незаметность для заражённых
		repairableWithKits[] = {2,3,5,8}; // швейный, кожаный, скотч, эпоксидка
		repairCosts[] = {25,25,25,25};
		class Protection
		{
			biological = 1;
			chemical = 1;
		};
		itemsCargoSize[] = {10,6};
		// DamageSystem скопирован из Vp_SET20_Vest_ColorBase целиком (частичное переопределение заменило бы его), броня усилена
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 800;
					transferToAttachmentsCoef = 0.5;
					healthLevels[] = {{1,{"set_legion\set\Vest\data\fast_vest.rvmat","set_legion\set\Vest\data\vest.rvmat","set_legion\set\Vest\data\vest_belt.rvmat","set_legion\set\Vest\data\vest3.rvmat","set_legion\set\Vest\data\znak_vest.rvmat"}},{0.7,{"set_legion\set\Vest\data\fast_vest.rvmat","set_legion\set\Vest\data\vest.rvmat","set_legion\set\Vest\data\vest_belt.rvmat","set_legion\set\Vest\data\vest3.rvmat","set_legion\set\Vest\data\znak_vest.rvmat"}},{0.5,{"set_legion\set\Vest\data\fast_vest_damage.rvmat","set_legion\set\Vest\data\vest_damage.rvmat","set_legion\set\Vest\data\vest_belt_damage.rvmat","set_legion\set\Vest\data\vest3_damage.rvmat","set_legion\set\Vest\data\znak_vest_damage.rvmat"}},{0.3,{"set_legion\set\Vest\data\fast_vest_damage.rvmat","set_legion\set\Vest\data\vest_damage.rvmat","set_legion\set\Vest\data\vest_belt_damage.rvmat","set_legion\set\Vest\data\vest3_damage.rvmat","set_legion\set\Vest\data\znak_vest_damage.rvmat"}},{0,{"set_legion\set\Vest\data\fast_vest_destruct.rvmat","set_legion\set\Vest\data\vest_destruct.rvmat","set_legion\set\Vest\data\vest_belt_destruct.rvmat","set_legion\set\Vest\data\vest3_destruct.rvmat","set_legion\set\Vest\data\znak_vest_destruct.rvmat"}}};
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
