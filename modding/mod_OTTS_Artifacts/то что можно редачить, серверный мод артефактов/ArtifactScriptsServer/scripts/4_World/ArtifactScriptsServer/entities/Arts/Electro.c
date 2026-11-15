modded class DD_Artifact_Lens
{
	override void Init()
	{
		super.Init();
		
		m_Absolutes.Set("Health", -0.25);
		//m_Relatives.Set("Health", 0.25);
		
		m_Absolutes.Set("Water", -2);
		//m_Relatives.Set("Water", 0.1);
		
		m_Absolutes.Set("Energy", -1);
		//m_Relatives.Set("Energy", -0.8);
		
		//m_Absolutes.Set("Shock", -15);
		
		m_Absolutes.Set("Blood", -1);
		//m_Relatives.Set("Blood", 5.15);
		
		//m_Absolutes.Set("Bleeds", 0.5); // >0 шанс закрыть кровотечение, <0 открыть (раз в 10 с)
		
		
		//m_Absolutes.Set("Stamina", 0.2); //this is stamina percent coef, e.g. -50% per tick
		
		m_Absolutes.Set("Radiation", 3);
	}
}
modded class DD_Artifact_SnowFlake
{
	override void Init()
	{
		super.Init();
		
		m_Absolutes.Set("Health", -0.5); // DayZavr 2026-11-14: было -3
		//m_Relatives.Set("Health", -1.2);
		
		m_Absolutes.Set("Water", -10);
		//m_Relatives.Set("Water", -1.5);
		
		m_Absolutes.Set("Energy", -10);
		//m_Relatives.Set("Energy", -0.8);
		
		//m_Absolutes.Set("Shock", -20);
		
		//m_Absolutes.Set("Blood", -3);
		//m_Relatives.Set("Blood", 5.15);
		
		m_Absolutes.Set("Bleeds", -0.2); // >0 шанс закрыть кровотечение, <0 открыть (раз в 10 с)
		
		
		m_Absolutes.Set("Stamina", 0.20); //this is stamina percent coef, e.g. -50% per tick
		
		m_Absolutes.Set("Radiation", 2);
	}
}
modded class DD_Artifact_Flash
{
	override void Init()
	{
		super.Init();
		
		m_Absolutes.Set("Health", -0.5); // DayZavr 2026-11-14: было -2.5
		//m_Relatives.Set("Health", 0.2);
		
		m_Absolutes.Set("Water", -10);
		//m_Relatives.Set("Water", 0.2);
		
		m_Absolutes.Set("Energy", -10);
		//m_Relatives.Set("Energy", 0.2);
		
		//m_Absolutes.Set("Shock", -20);
		
		m_Absolutes.Set("Blood", -3);
		//m_Relatives.Set("Blood", 0.2);
		
		m_Absolutes.Set("Bleeds", -0.1); // >0 шанс закрыть кровотечение, <0 открыть (раз в 10 с)
		
		
		//m_Absolutes.Set("Stamina", 0.15); //this is stamina percent coef, e.g. -50% per tick
		
		//m_Absolutes.Set("Radiation", 2);
	}
}
modded class DD_Artifact_AngelFool
{
	override void Init()
	{
		super.Init();
		
		m_Absolutes.Set("Health", -0.5); // DayZavr 2026-11-14: было -3
		//m_Relatives.Set("Health", 0.25);
		
		m_Absolutes.Set("Water", -10);
		//m_Relatives.Set("Water", 0.25);
		
		m_Absolutes.Set("Energy", -10);
		//m_Relatives.Set("Energy", 0.25);
		
		//m_Absolutes.Set("Shock", -20);
		
		m_Absolutes.Set("Blood", -2.5);
		//m_Relatives.Set("Blood", -2);
		
		//m_Absolutes.Set("Bleeds", -0.3); // >0 шанс закрыть кровотечение, <0 открыть (раз в 10 с)
		
		
		//m_Absolutes.Set("Stamina", 0.15); //this is stamina percent coef, e.g. -50% per tick
		
		//m_Absolutes.Set("Radiation", 2);
	}
}
modded class DD_Artifact_MoonLight
{
	override void Init()
	{
		super.Init();
		
		m_Absolutes.Set("Health", -0.5); // DayZavr 2026-11-14: было -1
		//m_Relatives.Set("Health", -1.3);
		
		m_Absolutes.Set("Water", -25); // DayZavr 2026-11-14: было -30
		//m_Relatives.Set("Water", -8);
		
		m_Absolutes.Set("Energy", 30);
		//m_Relatives.Set("Energy", -8);
		
		m_Absolutes.Set("Shock", -15);
		
		m_Absolutes.Set("Blood", -2.5);
		//m_Relatives.Set("Blood", 0.1);
		
		//m_Absolutes.Set("Bleeds", -0.3); // >0 шанс закрыть кровотечение, <0 открыть (раз в 10 с)
		
		
		//m_Absolutes.Set("Stamina", 0.15); //this is stamina percent coef, e.g. -50% per tick
		
		m_Absolutes.Set("Radiation", 1);
	}
}
modded class DD_Artifact_Soul
{
	override void Init()
	{
		super.Init();
		
		m_Absolutes.Set("Health", 2);
		//m_Relatives.Set("Health", -1.3);
		
		m_Absolutes.Set("Water", 5);
		//m_Relatives.Set("Water", -8);
		
		//m_Absolutes.Set("Energy", 30);
		//m_Relatives.Set("Energy", -8);
		
		m_Absolutes.Set("Shock", 10);
		
		m_Absolutes.Set("Blood", -1.75);
		//m_Relatives.Set("Blood", -2);
		
		m_Absolutes.Set("Bleeds", -0.1); // >0 шанс закрыть кровотечение, <0 открыть (раз в 10 с)
		
		
		//m_Absolutes.Set("Stamina", 0.15); //this is stamina percent coef, e.g. -50% per tick
		
		m_Absolutes.Set("Radiation", 6);
	}
}
modded class DD_Artifact_Battery
{
	override void Init()
	{
		super.Init();
		
		//m_Absolutes.Set("Health", 2);
		//m_Relatives.Set("Health", -1.3);
		
		m_Absolutes.Set("Water", -15);
		//m_Relatives.Set("Water", -8);
		
		m_Absolutes.Set("Energy", -15);
		//m_Relatives.Set("Energy", -8);
		
		//m_Absolutes.Set("Shock", 10);
		
		//m_Absolutes.Set("Blood", -1.75);
		//m_Relatives.Set("Blood", -2);
		
		//m_Absolutes.Set("Bleeds", -0.1); // >0 шанс закрыть кровотечение, <0 открыть (раз в 10 с)
		
		
		m_Absolutes.Set("Stamina", 0.05); //this is stamina percent coef, e.g. -50% per tick
		
		m_Absolutes.Set("Radiation", 4);
	}
}
modded class DD_Artifact_Sparkler
{
	override void Init()
	{
		super.Init();
		
		m_Absolutes.Set("Health", -0.5); // DayZavr 2026-11-14: было -1
		//m_Relatives.Set("Health", -1.3);
		
		m_Absolutes.Set("Water", -15);
		//m_Relatives.Set("Water", 0.5);
		
		m_Absolutes.Set("Energy", -15);
		//m_Relatives.Set("Energy", 0.5);
		
		m_Absolutes.Set("Shock", 20);
		
		m_Absolutes.Set("Blood", 1);
		//m_Relatives.Set("Blood", -2);
		
		//m_Absolutes.Set("Bleeds", -0.1); // >0 шанс закрыть кровотечение, <0 открыть (раз в 10 с)
		
		
		//m_Absolutes.Set("Stamina", 0.05); //this is stamina percent coef, e.g. -50% per tick
		
		m_Absolutes.Set("Radiation", 1);
	}
}
modded class DD_Artifact_DemonEye
{
	override void Init()
	{
		super.Init();
		
		m_Absolutes.Set("Health", 0.6);
		//m_Relatives.Set("Health", -1.3);
		
		//m_Absolutes.Set("Water", -15);
		//m_Relatives.Set("Water", 5);
		
		//m_Absolutes.Set("Energy", -15);
		//m_Relatives.Set("Energy", 5);
		
		m_Absolutes.Set("Shock", -25);
		
		//m_Absolutes.Set("Blood", 1);
		//m_Relatives.Set("Blood", -2);
		
		m_Absolutes.Set("Bleeds", 0.5); // >0 шанс закрыть кровотечение, <0 открыть (раз в 10 с)
		
		
		m_Absolutes.Set("Stamina", -0.2); //this is stamina percent coef, e.g. -50% per tick
		
		m_Absolutes.Set("Radiation", 3);
	}
}
modded class DD_Artifact_DarkSoul
{
	override void Init()
	{
		super.Init();
		
		m_Absolutes.Set("Health", 0.5);
		//m_Relatives.Set("Health", 1.25);
		
		m_Absolutes.Set("Water", -25); // DayZavr 2026-11-14: было -250
		//m_Relatives.Set("Water", 5);
		
		m_Absolutes.Set("Energy", -25); // DayZavr 2026-11-14: было -250
		//m_Relatives.Set("Energy", 5);
		
		m_Absolutes.Set("Shock", -15);
		
		m_Absolutes.Set("Blood", -5); // DayZavr 2026-11-14: было -25
		//m_Relatives.Set("Blood", -2);
		
		//m_Absolutes.Set("Bleeds", 0.15); // >0 шанс закрыть кровотечение, <0 открыть (раз в 10 с)
		
		
		//m_Absolutes.Set("Stamina", -0.2); //this is stamina percent coef, e.g. -50% per tick
		
		m_Absolutes.Set("Radiation", 5);
	}
}
modded class DD_Artifact_DevelHert
{
	override void Init()
	{
		super.Init();
		
		m_Absolutes.Set("Health", -0.5); // DayZavr 2026-11-14: было -10
		//m_Relatives.Set("Health", 1.25);
		
		m_Absolutes.Set("Water", 5); // DayZavr 2026-11-14: было 100
		//m_Relatives.Set("Water", 5);
		
		m_Absolutes.Set("Energy", 5); // DayZavr 2026-11-14: было 100
		//m_Relatives.Set("Energy", 5);
		
		m_Absolutes.Set("Shock", 15);
		
		m_Absolutes.Set("Blood", 5); // DayZavr 2026-11-14: было 100
		//m_Relatives.Set("Blood", -2);
		
		//m_Absolutes.Set("Bleeds", 0.15); // >0 шанс закрыть кровотечение, <0 открыть (раз в 10 с)
		
		
		//m_Absolutes.Set("Stamina", -0.2); //this is stamina percent coef, e.g. -50% per tick
		
		m_Absolutes.Set("Radiation", 5);
	}
}
modded class DD_Artifact_BatteryBlue
{
	override void Init()
	{
		super.Init();
		
		//m_Absolutes.Set("Health", 2);
		//m_Relatives.Set("Health", -1.3);
		
		m_Absolutes.Set("Water", -10);
		//m_Relatives.Set("Water", -8);
		
		m_Absolutes.Set("Energy", -10);
		//m_Relatives.Set("Energy", -8);
		
		//m_Absolutes.Set("Shock", 10);
		
		//m_Absolutes.Set("Blood", -1.75);
		//m_Relatives.Set("Blood", -2);
		
		//m_Absolutes.Set("Bleeds", -0.1); // >0 шанс закрыть кровотечение, <0 открыть (раз в 10 с)
		
		
		m_Absolutes.Set("Stamina", 0.1); //this is stamina percent coef, e.g. -50% per tick
		
		m_Absolutes.Set("Radiation", 2);
	}
}
modded class OOTS_ART_Battery3
{
	override void Init()
	{
		super.Init();
		
		//m_Absolutes.Set("Health", 2);
		//m_Relatives.Set("Health", -1.3);
		
		m_Absolutes.Set("Water", -5);
		//m_Relatives.Set("Water", -8);
		
		m_Absolutes.Set("Energy", -5);
		//m_Relatives.Set("Energy", -8);
		
		m_Absolutes.Set("Shock", -10);
		
		//m_Absolutes.Set("Blood", -1.75);
		m_Relatives.Set("Blood", 0.25);
		
		m_Absolutes.Set("Bleeds", -0.05); // >0 шанс закрыть кровотечение, <0 открыть (раз в 10 с)
		
		
		m_Absolutes.Set("Stamina", 0.15); //this is stamina percent coef, e.g. -50% per tick
		
		//m_Absolutes.Set("Radiation", 2);
	}
}