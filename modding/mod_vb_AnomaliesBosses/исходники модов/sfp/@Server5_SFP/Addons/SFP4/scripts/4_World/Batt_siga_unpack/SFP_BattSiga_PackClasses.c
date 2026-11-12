// Battery 9V packs (2 batteries) + cigarette sample packs (2 cigarettes)
// The classes below are intentionally lightweight: they only add the unpack action.

/*class SFP_Battery9V_pack_Energ : Inventory_Base
{
	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionSFP_UnpackBattery9VPacks);
	}
};

class SFP_Battery9V_pack_GP : Inventory_Base
{
	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionSFP_UnpackBattery9VPacks);
	}
};

class SFP_Battery9V_pack_DW : Inventory_Base
{
	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionSFP_UnpackBattery9VPacks);
	}
};

class SFP_Battery9V_pack_Panason : Inventory_Base
{
	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionSFP_UnpackBattery9VPacks);
	}
};

class SFP_Battery9V_pack_Kosmos : Inventory_Base
{
	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionSFP_UnpackBattery9VPacks);
	}
};

class SFP_Battery9V_pack_Dur : Inventory_Base
{
	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionSFP_UnpackBattery9VPacks);
	}
};*/
class SFP_Battery9V_pack_Dur extends ItemBase
{
	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionSFP_UnpackBattery9VPacks);
		AddAction(ActionOsmotrItem);
	}
}
class SFP_Battery9V_pack_Energ: SFP_Battery9V_pack_Dur {};
class SFP_Battery9V_pack_GP: SFP_Battery9V_pack_Dur {};
class SFP_Battery9V_pack_DW: SFP_Battery9V_pack_Dur {};
class SFP_Battery9V_pack_Panason: SFP_Battery9V_pack_Dur {};
class SFP_Battery9V_pack_Kosmos: SFP_Battery9V_pack_Dur {};
class SFP_2cigarettes_pack_camel extends ItemBase
{
	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionSFP_Unpack2CigarettePacks);
		AddAction(ActionOsmotrItem);
	}
}
class SFP_2cigarettes_pack_kent: SFP_2cigarettes_pack_camel {};
class SFP_2cigarettes_pack_magna: SFP_2cigarettes_pack_camel {};
class SFP_2cigarettes_pack_marlboro: SFP_2cigarettes_pack_camel {};
class SFP_2cigarettes_pack_parliament: SFP_2cigarettes_pack_camel {};
class SFP_2cigarettes_pack_PhilipMor: SFP_2cigarettes_pack_camel {};
class SFP_2cigarettes_pack_RusSlyle: SFP_2cigarettes_pack_camel {};
class SFP_2cigarettes_pack_SouyzAppo: SFP_2cigarettes_pack_camel {};
class SFP_2cigarettes_pack_winston: SFP_2cigarettes_pack_camel {};
class SFP_2cigarettes_pack_goldYava: SFP_2cigarettes_pack_camel {};
/*
// 2-cigarette sample packs (universal cigarette output)
class SFP_2cigarettes_pack_camel : Inventory_Base
{
	override void SetActions() { super.SetActions(); AddAction(ActionSFP_Unpack2CigarettePacks); }
};
class SFP_2cigarettes_pack_kent : Inventory_Base
{
	override void SetActions() { super.SetActions(); AddAction(ActionSFP_Unpack2CigarettePacks); }
};
class SFP_2cigarettes_pack_magna : Inventory_Base
{
	override void SetActions() { super.SetActions(); AddAction(ActionSFP_Unpack2CigarettePacks); }
};
class SFP_2cigarettes_pack_marlboro : Inventory_Base
{
	override void SetActions() { super.SetActions(); AddAction(ActionSFP_Unpack2CigarettePacks); }
};
class SFP_2cigarettes_pack_parliament : Inventory_Base
{
	override void SetActions() { super.SetActions(); AddAction(ActionSFP_Unpack2CigarettePacks); }
};
class SFP_2cigarettes_pack_PhilipMor : Inventory_Base
{
	override void SetActions() { super.SetActions(); AddAction(ActionSFP_Unpack2CigarettePacks); }
};
class SFP_2cigarettes_pack_RusSlyle : Inventory_Base
{
	override void SetActions() { super.SetActions(); AddAction(ActionSFP_Unpack2CigarettePacks); }
};
class SFP_2cigarettes_pack_SouyzAppo : Inventory_Base
{
	override void SetActions() { super.SetActions(); AddAction(ActionSFP_Unpack2CigarettePacks); }
};
class SFP_2cigarettes_pack_winston : Inventory_Base
{
	override void SetActions() { super.SetActions(); AddAction(ActionSFP_Unpack2CigarettePacks); }
};
class SFP_2cigarettes_pack_goldYava : Inventory_Base
{
	override void SetActions() { super.SetActions(); AddAction(ActionSFP_Unpack2CigarettePacks); }
};
