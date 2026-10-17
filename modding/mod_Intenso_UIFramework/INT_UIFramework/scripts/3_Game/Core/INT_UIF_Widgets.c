// INT_UIFramework - widget helpers: creation from framework layouts, typed
// lookup, child cleanup, icons, labels, slots and reflection auto-binding.
class INT_UIF_Widgets
{
	// Layouts whose root came back wrong (must stay 0; checked by the self-test).
	static int s_RootMismatches;

	// Creates a framework layout (file name relative to gui/layouts/).
	// GetWorkspace() is null during early init (dayz-ui-development Rule 6).
	// Framework layouts go through INT_UIF_LayoutTable, which hands CreateWidgets
	// a LITERAL path: a path built at runtime made the engine return a different
	// layout's tree (measured in-game, see the table's header).
	static Widget Create(string layoutName, Widget parent)
	{
		if (!INT_UIF_LayoutTable.IsKnown(layoutName))
			return CreatePath(INT_UIF_LAYOUTS + layoutName, parent);
		if (!GetGame() || !GetGame().GetWorkspace())
			return null;
		INT_UIF_Assets.EnsureLoaded();
		Widget w = INT_UIF_LayoutTable.Create(layoutName, parent);
		if (!w)
		{
			INT_UIF_Log.Error("CreateWidgets failed: " + layoutName);
			return null;
		}
		string expected = INT_UIF_LayoutTable.RootName(layoutName);
		if (w.GetName() != expected)
		{
			s_RootMismatches++;
			INT_UIF_Log.Error("layout " + layoutName + " came back as " + w.GetName() + " (expected " + expected + ")");
		}
		INT_UIF_Log.Debug("CreateWidgets '" + layoutName + "' parent=" + NameOf(parent) + " -> " + NameOf(w));
		return w;
	}

	static Widget CreatePath(string layoutPath, Widget parent)
	{
		if (!GetGame() || !GetGame().GetWorkspace())
			return null;
		INT_UIF_Assets.EnsureLoaded();
		Widget w = GetGame().GetWorkspace().CreateWidgets(layoutPath, parent);
		if (!w)
			INT_UIF_Log.Error("CreateWidgets failed: " + layoutPath);
		INT_UIF_Log.Debug("CreateWidgets '" + layoutPath + "' parent=" + NameOf(parent) + " -> " + NameOf(w));
		return w;
	}

	// "null" for a missing widget, its name otherwise (diagnostics).
	static string NameOf(Widget w)
	{
		if (!w)
			return "null";
		return w.GetName();
	}

	static Widget Find(Widget root, string name)
	{
		if (!root)
			return null;
		return root.FindAnyWidget(name);
	}

	static TextWidget FindText(Widget root, string name)
	{
		return TextWidget.Cast(Find(root, name));
	}

	static RichTextWidget FindRich(Widget root, string name)
	{
		return RichTextWidget.Cast(Find(root, name));
	}

	static MultilineTextWidget FindMultiline(Widget root, string name)
	{
		return MultilineTextWidget.Cast(Find(root, name));
	}

	static ImageWidget FindImage(Widget root, string name)
	{
		return ImageWidget.Cast(Find(root, name));
	}

	static ButtonWidget FindButton(Widget root, string name)
	{
		return ButtonWidget.Cast(Find(root, name));
	}

	static EditBoxWidget FindEdit(Widget root, string name)
	{
		return EditBoxWidget.Cast(Find(root, name));
	}

	static ScrollWidget FindScroll(Widget root, string name)
	{
		return ScrollWidget.Cast(Find(root, name));
	}

	// Unlinks every child of parent (the parent itself stays).
	// The next sibling is read BEFORE the current child is destroyed.
	static void ClearChildren(Widget parent)
	{
		if (!parent)
			return;
		Widget child = parent.GetChildren();
		while (child)
		{
			Widget next = child.GetSibling();
			child.Unlink();
			child = next;
		}
	}

	// Shows an atlas icon ("check", "close", ...) on an ImageWidget.
	static void SetIcon(ImageWidget img, string icon)
	{
		if (!img)
			return;
		if (icon == "")
		{
			img.Show(false);
			return;
		}
		img.LoadImageFile(0, INT_UIF_SET + "iuf_icon_" + icon);
		img.SetImage(0);
		img.Show(true);
	}

	// RichText inline icon markup, e.g. for a button label: Icon("plus") + " Add".
	static string IconMarkup(string icon)
	{
		if (icon == "")
			return "";
		return "<image set=\"iuf_ui\" name=\"iuf_icon_" + icon + "\" />";
	}

	// True when the screen point (x, y) lies inside w.
	static bool Contains(Widget w, int x, int y)
	{
		if (!w || !w.IsVisibleHierarchy())
			return false;
		float px;
		float py;
		float sw;
		float sh;
		w.GetScreenPos(px, py);
		w.GetScreenSize(sw, sh);
		return x >= px && x <= px + sw && y >= py && y <= py + sh;
	}

	static bool IsDescendant(Widget w, Widget ancestor)
	{
		Widget cur = w;
		while (cur)
		{
			if (cur == ancestor)
				return true;
			cur = cur.GetParent();
		}
		return false;
	}

	// An empty proportional frame placed inside parent (all values 0..1 of parent).
	// This is how menus are composed: a slot per component.
	static Widget Slot(Widget parent, float x, float y, float w, float h)
	{
		// A null parent would create a workspace-level widget nobody unlinks.
		if (!parent)
			return null;
		Widget slot = Create("slot.layout", parent);
		if (!slot)
			return null;
		slot.SetPos(x, y);
		slot.SetSize(w, h);
		return slot;
	}

	// A themed text label in a proportional slot. Text may be a #STR_ key.
	static TextWidget Label(Widget parent, float x, float y, float w, float h, string text, int role = INT_UIF_TextRole.BODY)
	{
		if (!parent)
			return null;
		Widget root = Create("label.layout", parent);
		if (!root)
			return null;
		root.SetPos(x, y);
		root.SetSize(w, h);
		TextWidget tw = FindText(root, "Text");
		if (!tw)
			return null;
		tw.SetText(text);
		StyleText(tw, role);
		return tw;
	}

	// Applies a typography role: pixel size scaled from 1080p + theme colour.
	static void StyleText(TextWidget tw, int role)
	{
		if (!tw)
			return;
		INT_UIF_ThemeManager tm = INT_UIF_ThemeManager.Get();
		float size = 17;
		int color = tm.Color(INT_UIF_ThemeKeys.TEXT);
		if (role == INT_UIF_TextRole.TITLE)
		{
			size = 26;
		}
		else if (role == INT_UIF_TextRole.SUBTITLE)
		{
			size = 20;
		}
		else if (role == INT_UIF_TextRole.MUTED)
		{
			size = 16;
			color = tm.Color(INT_UIF_ThemeKeys.TEXT_MUTED);
		}
		else if (role == INT_UIF_TextRole.CAPTION)
		{
			size = 14;
			color = tm.Color(INT_UIF_ThemeKeys.TEXT_MUTED);
		}
		tw.SetTextExactSize(INT_UIF_Scale.TextPx(size));
		tw.SetColor(color);
	}

	// LBmaster-style auto-binding: every Widget-typed member variable of inst
	// is bound to the widget of the same name under root (dayz-ui-development,
	// AUTO WIDGET BINDING). Returns the number of bound fields.
	static int AutoBind(Class inst, Widget root)
	{
		if (!inst || !root)
			return 0;
		typename t = inst.Type();
		int bound = 0;
		int count = t.GetVariableCount();
		for (int i = 0; i < count; i++)
		{
			typename vt = t.GetVariableType(i);
			if (!vt.IsInherited(Widget))
				continue;
			string varName = t.GetVariableName(i);
			Widget w = root.FindAnyWidget(varName);
			if (w)
			{
				EnScript.SetClassVar(inst, varName, 0, w);
				bound++;
			}
		}
		return bound;
	}
}
