// Пояс с баночками для артефактов экзо Эхо (VB_EchoClothes, ТЗ_VB_EchoClothes.md раздел 8.4; концепт одобрен 11-24).
// Гнездо VB_EC_ArtCase сервопривода (кость pelvis). В баночке N — слот VB_EC_ArtJarN без прокси: сам артефакт не виден,
// клиент рисует в баночке его уменьшенную копию (модель из config артефакта, CreateStaticObjectUsingP3D + AddChild,
// каталог E126: масштаб вложения движок сбрасывает). Копия ставится по точкам памяти модели: vbartN — центр баночки,
// vbartNu — вверх, vbartNf — наружу; габарит модели артефакта (vbArtBox[] из PBO VB_EchoClothes_Art) вписан в
// vbArtFit (м), центр габарита — в центр баночки.
// Копий нет, если пояс (или любой его родитель) лежит в грузе; проверка раз в секунду, пока в поясе есть артефакт.
// Кольцо vbjarN горит цветом Эхо, когда баночка занята. Действие артефактов — их серверный мод (по цепочке родителей).

class VB_EC_ArtBelt_Base extends ItemBase
{
	protected ref array<int> m_VB_SlotIds;
	protected ref array<Object> m_VB_Shows;
	protected ref TStringArray m_VB_ShowTypes;
	protected ref array<int> m_VB_Glow;			// -1 неизвестно, 0 погашено, 1 горит
	protected bool m_VB_Ticking;

	protected void VB_InitSlots()
	{
		if (m_VB_SlotIds)
			return;
		m_VB_SlotIds = new array<int>;
		m_VB_Shows = new array<Object>;
		m_VB_ShowTypes = new TStringArray;
		m_VB_Glow = new array<int>;
		TStringArray atts = new TStringArray;
		GetGame().ConfigGetTextArray("CfgVehicles " + GetType() + " attachments", atts);
		for (int i = 0; i < atts.Count(); i++)
		{
			m_VB_SlotIds.Insert(InventorySlots.GetSlotIdFromString("VB_EC_ArtJar" + (i + 1)));
			m_VB_Shows.Insert(null);
			m_VB_ShowTypes.Insert("");
			m_VB_Glow.Insert(-1);
		}
	}

	// копии видны, если ни пояс, ни его родители не лежат в грузе (рюкзак, ящик, карман)
	protected bool VB_ShowAllowed()
	{
		EntityAI e = this;
		while (e)
		{
			if (e.GetInventory() && e.GetInventory().IsInCargo())
				return false;
			e = e.GetHierarchyParent();
		}
		return true;
	}

	protected void VB_SetGlow(int i, bool on)
	{
		int want = 0;
		if (on)
			want = 1;
		if (m_VB_Glow[i] == want)
			return;
		m_VB_Glow[i] = want;
		int idx = GetHiddenSelectionIndex("vbjar" + (i + 1));
		if (idx < 0)
			return;
		if (on)
		{
			SetObjectTexture(idx, VB_EC_Exo_Servo_Base.GLOW_TEX_ECHO);
			SetObjectMaterial(idx, VB_EC_Exo_Servo_Base.GLOW_MAT_ON);
		}
		else
		{
			SetObjectTexture(idx, VB_EC_Exo_Servo_Base.GLOW_TEX_OFF);
			SetObjectMaterial(idx, VB_EC_Exo_Servo_Base.GLOW_MAT_OFF);
		}
	}

	protected void VB_DropShow(int i)
	{
		Object o = m_VB_Shows[i];
		if (o)
			GetGame().ObjectDelete(o);
		m_VB_Shows[i] = null;
		m_VB_ShowTypes[i] = "";
	}

	protected void VB_MakeShow(int i, EntityAI art)
	{
		string model = GetGame().ConfigGetTextOut("CfgVehicles " + art.GetType() + " model");
		if (model == "")
			return;
		if (model.IndexOf("\\") == 0)
			model = model.Substring(1, model.Length() - 1);
		string pc = "vbart" + (i + 1);
		if (!MemoryPointExists(pc))
			return;
		vector c = GetMemoryPointPos(pc);
		vector up = vector.Direction(c, GetMemoryPointPos(pc + "u")).Normalized();
		vector fw = vector.Direction(c, GetMemoryPointPos(pc + "f")).Normalized();
		vector side = (up * fw).Normalized();			// векторное произведение (E25): ось x копии — вдоль пояса
		fw = (side * up).Normalized();

		Object o = GetGame().CreateStaticObjectUsingP3D(model, GetPosition(), "0 0 0", 1.0, true);
		if (!o)
			return;
		o.ClearFlags(EntityFlags.SOLID, false);			// не ловит лучи (наведение действий, стрельба)
		if (dBodyIsSet(o))
			dBodyDestroy(o);							// без физики: копия у пояса не толкает игрока
		// габарит видимой модели: vbArtBox[] = {центр x, y, z, наибольший размер} из PBO VB_EchoClothes_Art (замер LOD0
		// генератором ec_art_slots.py). ClippingInfo — запасной путь: это габарит заголовка модели (все LOD) с запасом
		// ~2 см с каждой стороны, по нему копии выходили вдвое мельче (стенд 11-24) — берём 0,6 размера
		vector bc;
		float big;
		TFloatArray box = new TFloatArray;
		GetGame().ConfigGetFloatArray("CfgVehicles " + art.GetType() + " vbArtBox", box);
		if (box.Count() >= 4)
		{
			bc = Vector(box[0], box[1], box[2]);
			big = box[3];
		}
		else
		{
			vector mm[2];
			o.ClippingInfo(mm);
			bc = (mm[0] + mm[1]) * 0.5;
			vector ext = mm[1] - mm[0];
			big = Math.Max(ext[0], Math.Max(ext[1], ext[2])) * 0.6;
		}
		float fit = GetGame().ConfigGetFloat("CfgVehicles " + GetType() + " vbArtFit");
		if (fit <= 0)
			fit = 0.05;
		float s = 1.0;
		if (big > 0.001)
			s = Math.Min(fit / big, 1.0);
		vector m[4];
		m[0] = side * s;
		m[1] = up * s;
		m[2] = fw * s;
		m[3] = c - (side * bc[0] + up * bc[1] + fw * bc[2]) * s;
		AddChild(o, -1);
		o.SetTransform(m);			// после AddChild — локально и с масштабом (SetOrientation/SetPosition его сбросили бы)
		m_VB_Shows[i] = o;
		m_VB_ShowTypes[i] = art.GetType();
	}

	void VB_Refresh()
	{
		if (GetGame().IsDedicatedServer() || IsSetForDeletion())
			return;
		VB_InitSlots();
		bool show = VB_ShowAllowed();
		bool any = false;
		int n = m_VB_SlotIds.Count();
		for (int g = 0; g < n; g++)							// свет — отдельным проходом, до копий
		{
			bool occupied = GetInventory().FindAttachment(m_VB_SlotIds[g]) != null;
			VB_SetGlow(g, occupied);
			if (occupied)
				any = true;
		}
		for (int i = 0; i < n; i++)
		{
			EntityAI art = GetInventory().FindAttachment(m_VB_SlotIds[i]);
			if (art && show)
			{
				if (!m_VB_Shows[i] || m_VB_ShowTypes[i] != art.GetType())
				{
					VB_DropShow(i);
					VB_MakeShow(i, art);
				}
			}
			else if (m_VB_Shows[i])
			{
				VB_DropShow(i);
			}
		}
		if (any)
			Update();
		// пока в поясе есть артефакт — раз в секунду: пояс могли убрать в груз вместе с сервоприводом (нам не сообщают)
		if (any && !m_VB_Ticking)
		{
			m_VB_Ticking = true;
			GetGame().GetCallQueue(CALL_CATEGORY_GUI).CallLater(VB_Refresh, 1000, true);
		}
		else if (!any && m_VB_Ticking)
		{
			m_VB_Ticking = false;
			GetGame().GetCallQueue(CALL_CATEGORY_GUI).Remove(VB_Refresh);
		}
	}

	protected void VB_RefreshSoon()
	{
		if (GetGame().IsDedicatedServer())
			return;
		GetGame().GetCallQueue(CALL_CATEGORY_GUI).CallLater(VB_RefreshOnce, 50, false);
	}

	void VB_RefreshOnce()
	{
		VB_Refresh();
	}

	override void EEInit()
	{
		super.EEInit();
		if (!GetGame().IsDedicatedServer())
			GetGame().GetCallQueue(CALL_CATEGORY_GUI).CallLater(VB_RefreshOnce, 500, false);	// вложения приходят после
	}

	override void EEItemAttached(EntityAI item, string slot_name)
	{
		super.EEItemAttached(item, slot_name);
		VB_RefreshSoon();
	}

	override void EEItemDetached(EntityAI item, string slot_name)
	{
		super.EEItemDetached(item, slot_name);
		VB_RefreshSoon();
	}

	override void EEItemLocationChanged(notnull InventoryLocation oldLoc, notnull InventoryLocation newLoc)
	{
		super.EEItemLocationChanged(oldLoc, newLoc);
		VB_RefreshSoon();
	}

	override void EEHealthLevelChanged(int oldLevel, int newLevel, string zone)
	{
		super.EEHealthLevelChanged(oldLevel, newLevel, zone);
		if (m_VB_Glow)
		{
			for (int i = 0; i < m_VB_Glow.Count(); i++)
				m_VB_Glow[i] = -1;
		}
		VB_RefreshSoon();
	}

	override void EEDelete(EntityAI parent)
	{
		super.EEDelete(parent);
		if (GetGame().IsDedicatedServer())
			return;
		GetGame().GetCallQueue(CALL_CATEGORY_GUI).Remove(VB_Refresh);
		GetGame().GetCallQueue(CALL_CATEGORY_GUI).Remove(VB_RefreshOnce);
		if (m_VB_Shows)
		{
			for (int i = 0; i < m_VB_Shows.Count(); i++)
				VB_DropShow(i);
		}
	}
};
