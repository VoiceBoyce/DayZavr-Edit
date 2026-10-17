class DRL_CraftNotificationWidget
{
	protected Widget m_Root;
	protected ImageWidget m_IconWidget;
	protected ItemPreviewWidget m_ItemPreview;
	protected ImageWidget m_NotifiIcon;
	protected Widget m_ImagePanel;
	protected TextWidget m_TitleWidget;
	protected TextWidget m_TextWidget;
	protected MultilineTextWidget m_MultiTextWidget;
	protected EntityAI m_PreviewDummy;
	protected bool m_IsDestroyed;

	void DRL_CraftNotificationWidget(string title, string text, string itemClass = "", string statusIcon = "", string bigIcon = "", float duration = 5.0)
	{
		m_Root = GetGame().GetWorkspace().CreateWidgets("DRL_CraftMenu/UI/layout/CraftNotifications.layout");
		if (!m_Root)
		{
			return;
		}

		m_IconWidget = ImageWidget.Cast(m_Root.FindAnyWidget("NotifiIconWidget"));
		m_TitleWidget = TextWidget.Cast(m_Root.FindAnyWidget("NotifiTitle"));
		Widget wText = m_Root.FindAnyWidget("NotifiMultilineText");
		m_TextWidget = TextWidget.Cast(wText);
		m_MultiTextWidget = MultilineTextWidget.Cast(wText);
		m_ImagePanel = m_Root.FindAnyWidget("NotifiImagePanelWidget");
		m_ItemPreview = ItemPreviewWidget.Cast(m_Root.FindAnyWidget("NotifiItemPreview"));
		m_NotifiIcon = ImageWidget.Cast(m_Root.FindAnyWidget("NotifiIcon"));

		if (m_TitleWidget)
		{
			string displayTitle = title;
			if (displayTitle.IndexOf("#") == 0)
			{
				displayTitle = Widget.TranslateString(displayTitle);
			}
			m_TitleWidget.SetText(displayTitle);
		}

		string displayText = text;
		if (displayText.IndexOf("#") == 0)
		{
			displayText = Widget.TranslateString(displayText);
		}

		if (m_TextWidget)
		{
			m_TextWidget.SetText(displayText);
		}
		else if (m_MultiTextWidget)
		{
			m_MultiTextWidget.SetText(displayText);
		}

		if (m_IconWidget)
		{
			if (statusIcon != "")
			{
				m_IconWidget.LoadImageFile(0, statusIcon);
			}
			m_IconWidget.Show(true);
		}

		bool hasPreview = false;
		if (itemClass != "" && m_ItemPreview)
		{
			m_PreviewDummy = EntityAI.Cast(GetGame().CreateObjectEx(itemClass, Vector(0, 0, 0), ECE_LOCAL | ECE_NOLIFETIME));
			if (m_PreviewDummy)
			{
				m_ItemPreview.SetItem(m_PreviewDummy);
				int vIdx = m_PreviewDummy.GetViewIndex();
				if (vIdx < 0)
				{
					vIdx = 0;
				}
				m_ItemPreview.SetView(vIdx);
				m_ItemPreview.SetModelPosition(Vector(0, 0, 0));
				m_ItemPreview.SetModelOrientation(Vector(0, 0, 0));
				m_ItemPreview.Show(true);

				if (m_NotifiIcon)
				{
					m_NotifiIcon.Show(false);
				}
				hasPreview = true;
			}
		}

		if (!hasPreview)
		{
			if (m_ItemPreview)
			{
				m_ItemPreview.Show(false);
			}

			if (m_NotifiIcon)
			{
				string iconToLoad = bigIcon;
				if (iconToLoad == "" && statusIcon != "")
				{
					iconToLoad = statusIcon;
				}
				if (iconToLoad != "")
				{
					m_NotifiIcon.LoadImageFile(0, iconToLoad);
				}
				m_NotifiIcon.Show(true);
			}
		}

		int delayMs = Math.Max(1000, Math.Round(duration * 1000));
		GetGame().GetCallQueue(CALL_CATEGORY_GUI).CallLater(this.DestroyNotification, delayMs, false);
	}

	void ~DRL_CraftNotificationWidget()
	{
		DestroyNotification();
	}

	void SetStackIndex(int index)
	{
		if (!m_Root)
		{
			return;
		}

		float targetY = 40.0 + (index * 88.0);
		m_Root.SetPos(20.0, targetY);
	}

	void DestroyNotification()
	{
		if (m_IsDestroyed)
		{
			return;
		}
		m_IsDestroyed = true;

		DRL_CraftNotificationManager.RemoveNotification(this);

		if (m_ItemPreview)
		{
			m_ItemPreview.SetItem(null);
			m_ItemPreview = null;
		}

		if (m_PreviewDummy)
		{
			GetGame().ObjectDelete(m_PreviewDummy);
			m_PreviewDummy = null;
		}

		if (m_Root)
		{
			m_Root.Unlink();
			m_Root = null;
		}
	}
}

class DRL_CraftNotificationManager
{
	protected static ref array<ref DRL_CraftNotificationWidget> s_ActiveNotifications = new array<ref DRL_CraftNotificationWidget>();

	static void Show(string title, string text, string itemClass = "", string statusIcon = "", string bigIcon = "", float duration = 5.0)
	{
		if (!GetGame() || !GetGame().GetWorkspace())
		{
			return;
		}

		ref DRL_CraftNotificationWidget notif = new DRL_CraftNotificationWidget(title, text, itemClass, statusIcon, bigIcon, duration);
		AddNotification(notif);
	}

	static void ShowByType(int notifType, string itemClass = "", string extraParam = "", string customTitle = "", string customText = "")
	{
		if (!GetGame() || !GetGame().GetWorkspace())
		{
			return;
		}

		string dName = GetItemDisplayName(itemClass);
		if (dName == "" && itemClass != "")
		{
			dName = itemClass;
		}

		string title = customTitle;
		string text = customText;
		string statusIcon = "DRL_CraftMenu/UI/data/craft_icon.edds";
		string bigIcon = "";

		switch (notifType)
		{
			case EDRLCraftNotifType.STARTED:
			{
				if (title == "") title = Widget.TranslateString("#STR_DRL_NOTIF_TITLE_STARTED");
				if (text == "") text = string.Format(Widget.TranslateString("#STR_DRL_NOTIF_MSG_STARTED"), dName);
				statusIcon = "DRL_CraftMenu/UI/data/craft_icon.edds";
				break;
			}

			case EDRLCraftNotifType.QUEUED:
			{
				if (title == "") title = Widget.TranslateString("#STR_DRL_NOTIF_TITLE_QUEUED");
				if (text == "") text = string.Format(Widget.TranslateString("#STR_DRL_NOTIF_MSG_QUEUED"), dName, extraParam);
				statusIcon = "DRL_CraftMenu/UI/data/watch.edds";
				break;
			}

			case EDRLCraftNotifType.PROMOTED_FROM_QUEUE:
			{
				if (title == "") title = Widget.TranslateString("#STR_DRL_NOTIF_TITLE_STARTED");
				if (text == "") text = string.Format(Widget.TranslateString("#STR_DRL_NOTIF_MSG_PROMOTED"), dName);
				statusIcon = "DRL_CraftMenu/UI/data/craft_icon.edds";
				break;
			}

			case EDRLCraftNotifType.SUCCESS:
			{
				if (title == "") title = Widget.TranslateString("#STR_DRL_NOTIF_TITLE_SUCCESS");
				if (text == "") text = string.Format(Widget.TranslateString("#STR_DRL_NOTIF_MSG_SUCCESS"), dName);
				statusIcon = "DRL_CraftMenu/UI/data/craft_icon.edds";
				break;
			}

			case EDRLCraftNotifType.CANCELLED_DISTANCE:
			{
				if (title == "") title = Widget.TranslateString("#STR_DRL_NOTIF_TITLE_CANCELLED");
				if (text == "") text = string.Format(Widget.TranslateString("#STR_DRL_NOTIF_MSG_DISTANCE"), dName);
				statusIcon = "DRL_CraftMenu/UI/data/error.edds";
				break;
			}

			case EDRLCraftNotifType.NO_INGREDIENTS:
			{
				if (title == "") title = Widget.TranslateString("#STR_DRL_NOTIF_TITLE_NO_INGREDIENTS");
				if (text == "") text = string.Format(Widget.TranslateString("#STR_DRL_NOTIF_MSG_NO_INGR"), dName);
				statusIcon = "DRL_CraftMenu/UI/data/error.edds";
				break;
			}

			case EDRLCraftNotifType.SPACE_BLOCKED:
			{
				if (title == "") title = Widget.TranslateString("#STR_DRL_NOTIF_TITLE_SPACE_BLOCKED");
				if (text == "") text = string.Format(Widget.TranslateString("#STR_DRL_NOTIF_MSG_BLOCKED"), dName);
				statusIcon = "DRL_CraftMenu/UI/data/error.edds";
				break;
			}

			case EDRLCraftNotifType.QUEUE_FULL:
			{
				if (title == "") title = Widget.TranslateString("#STR_DRL_NOTIF_TITLE_QUEUE_FULL");
				if (text == "") text = string.Format(Widget.TranslateString("#STR_DRL_NOTIF_MSG_QUEUE_FULL"), extraParam, extraParam);
				statusIcon = "DRL_CraftMenu/UI/data/watch.edds";
				break;
			}

			case EDRLCraftNotifType.CANCELLED_MANUAL:
			{
				if (title == "") title = Widget.TranslateString("#STR_DRL_NOTIF_TITLE_CANCELLED");
				if (text == "") text = string.Format(Widget.TranslateString("#STR_DRL_NOTIF_MSG_CANCELLED"), dName);
				statusIcon = "DRL_CraftMenu/UI/data/cross-mark.edds";
				break;
			}

			case EDRLCraftNotifType.MISSING_TOOLS:
			{
				if (title == "") title = Widget.TranslateString("#STR_DRL_NOTIF_TITLE_MISSING_TOOLS");
				if (text == "") text = string.Format(Widget.TranslateString("#STR_DRL_NOTIF_MSG_TOOLS"), dName);
				statusIcon = "DRL_CraftMenu/UI/data/anvil-impact.edds";
				break;
			}

			case EDRLCraftNotifType.NOT_LEARNED:
			{
				if (title == "") title = Widget.TranslateString("#STR_DRL_NOTIF_TITLE_NOT_LEARNED");
				if (text == "") text = string.Format(Widget.TranslateString("#STR_DRL_NOTIF_MSG_NOT_LEARNED"), dName);
				statusIcon = "DRL_CraftMenu/UI/data/lock.edds";
				break;
			}

			case EDRLCraftNotifType.RECIPE_LEARNED:
			{
				if (title == "") title = Widget.TranslateString("#STR_DRL_NOTIF_TITLE_LEARNED");
				if (text == "") text = string.Format(Widget.TranslateString("#STR_DRL_NOTIF_MSG_LEARNED"), dName);
				statusIcon = "DRL_CraftMenu/UI/data/unlock.edds";
				break;
			}

			case EDRLCraftNotifType.SYSTEM_INFO:
			{
				if (title == "") title = Widget.TranslateString("#STR_DRL_NOTIF_TITLE_SYSTEM");
				if (text == "") text = customText;
				statusIcon = "DRL_CraftMenu/UI/data/info.edds";
				bigIcon = "DRL_CraftMenu/UI/data/info.edds";
				break;
			}
		}

		Show(title, text, itemClass, statusIcon, bigIcon, 5.0);
	}

	static void AddNotification(DRL_CraftNotificationWidget notif)
	{
		if (!notif)
		{
			return;
		}

		if (!s_ActiveNotifications)
		{
			s_ActiveNotifications = new array<ref DRL_CraftNotificationWidget>();
		}

		s_ActiveNotifications.Insert(notif);
		if (s_ActiveNotifications.Count() > 4)
		{
			DRL_CraftNotificationWidget oldest = s_ActiveNotifications[0];
			s_ActiveNotifications.Remove(0);
			if (oldest)
			{
				oldest.DestroyNotification();
			}
		}

		UpdateStackPositions();
	}

	static void RemoveNotification(DRL_CraftNotificationWidget notif)
	{
		if (!s_ActiveNotifications || !notif)
		{
			return;
		}

		int idx = s_ActiveNotifications.Find(notif);
		if (idx > -1)
		{
			s_ActiveNotifications.Remove(idx);
			UpdateStackPositions();
		}
	}

	static void UpdateStackPositions()
	{
		if (!s_ActiveNotifications)
		{
			return;
		}

		for (int i = 0; i < s_ActiveNotifications.Count(); i++)
		{
			DRL_CraftNotificationWidget notif = s_ActiveNotifications[i];
			if (notif)
			{
				notif.SetStackIndex(i);
			}
		}
	}

	static string GetItemDisplayName(string itemClass)
	{
		if (itemClass == "")
		{
			return "";
		}

		string cfgPath = "CfgVehicles " + itemClass + " displayName";
		if (GetGame().ConfigIsExisting(cfgPath))
		{
			return Widget.TranslateString(GetGame().ConfigGetTextOut(cfgPath));
		}

		cfgPath = "CfgWeapons " + itemClass + " displayName";
		if (GetGame().ConfigIsExisting(cfgPath))
		{
			return Widget.TranslateString(GetGame().ConfigGetTextOut(cfgPath));
		}

		cfgPath = "CfgMagazines " + itemClass + " displayName";
		if (GetGame().ConfigIsExisting(cfgPath))
		{
			return Widget.TranslateString(GetGame().ConfigGetTextOut(cfgPath));
		}

		return itemClass;
	}
}
