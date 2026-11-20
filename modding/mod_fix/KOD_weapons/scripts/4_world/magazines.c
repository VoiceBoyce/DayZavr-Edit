class Mag_KOD_AKALPHA_30rnd: MagazineStorage {};
class Mag_KOD_RSASS_20rnd: MagazineStorage {};
class Mag_KOD_Beowulf_10rnd: MagazineStorage {};
class Mag_KOD_AX50_5rnd: MagazineStorage {};
class Mag_KOD_AK47_20rnd: MagazineStorage {};
class Mag_KOD_AWM_5rnd: MagazineStorage {};
class Mag_KOD_M82A2_10rnd: MagazineStorage {};
class Mag_KOD_ASH12_20rnd: MagazineStorage {};
class Mag_KOD_Drum308_50rnd: MagazineStorage {};
class Mag_KOD_HK417_20rnd: MagazineStorage {};
class Mag_KOD_R700_7rnd: MagazineStorage {};
class Mag_KOD_DVL10_5rnd: MagazineStorage {};
class Mag_KOD_VR80_10rnd: MagazineStorage {};
class Mag_KOD_VR80_30rnd: MagazineStorage {};
class Mag_KOD_Ruger_5rnd: MagazineStorage {};
class Mag_KOD_RPK16_96rnd: MagazineStorage {};
class Mag_KOD_M110_10rnd: MagazineStorage {};
class Mag_KOD_M110_20rnd: MagazineStorage {};
class Mag_KOD_SCARH_20rnd: MagazineStorage {};
class Mag_KOD_SCARH_30rnd: MagazineStorage {};
class Mag_KOD_DVL10M1_5rnd: MagazineStorage {};
class Mag_KOD_PKM_100rnd: MagazineStorage {};
class Mag_KOD_SKS_20rnd: MagazineStorage {};
class Mag_KOD_DrumShotgun_30rnd: MagazineStorage {};
class Mag_KOD_SA58_20Rnd: MagazineStorage {};
class Mag_KOD_M200_7rnd: MagazineStorage {};
class Mag_KOD_PPSh_71rnd: MagazineStorage {};
class Mag_KOD_Scorpion_30rnd: MagazineStorage {};
class Mag_KOD_Raptr_30rnd: MagazineStorage {};
class Mag_KOD_VSS_20rnd: MagazineStorage {};
class Mag_KOD_VSS_40rnd: MagazineStorage {};
class Mag_KOD_HK21_50rnd: MagazineStorage {};
class Mag_KOD_Kivaari_10rnd: MagazineStorage {};
class Mag_KOD_PKP_150rnd: MagazineStorage {};
class Mag_KOD_M60_100rnd: MagazineStorage {};
class Mag_KOD_Alligator_5rnd: MagazineStorage {};
class Mag_KOD_VSK94_20rnd: MagazineStorage {};
class Mag_KOD_M1928A1_30rnd: MagazineStorage {};
class Mag_KOD_M1928A1_100rnd: MagazineStorage {};
class Mag_KOD_MDR_30rnd: MagazineStorage {};
class Mag_KOD_AK458_20rnd: MagazineStorage {};
class Mag_KOD_VSSK_5rnd: MagazineStorage {};
class Mag_KOD_Vector_30rnd: MagazineStorage {};
class Mag_KOD_KSVK_5rnd: MagazineStorage {};
class Mag_KOD_SUB2000_30rnd: MagazineStorage {};
class Mag_KOD_Glock17_Black_16rnd: MagazineStorage {};
class Mag_KOD_Glock17_Tan_16rnd: Mag_KOD_Glock17_Black_16rnd {};
class Mag_KOD_Glock17_Black_30rnd: MagazineStorage {};
class Mag_KOD_Glock17_Tan_30rnd: Mag_KOD_Glock17_Black_30rnd {};
class Mag_KOD_CS5_10rnd: MagazineStorage {};
class Mag_KOD_SV98_10rnd: MagazineStorage {};
class Mag_KOD_AS50_10Rnd: MagazineStorage {};
class Mag_KOD_GM6Lynx_5Rnd: MagazineStorage {};
class Mag_CMAG_100Rnd_Black: MagazineStorage {};

class Mag_KOD_M249_200rnd: MagazineStorage 
{
	void Mag_KOD_M249_200rnd()
	{
		HideAmmoBeltMagKOD(this);
	}	
	
	override void OnWasAttached(EntityAI parent, int slot_id)
	{
		super.OnWasAttached(parent,slot_id);
		
		if( parent && parent.IsKindOf("KOD_M249_Base"))
		{
			ShowAmmoBeltMagKOD(this);
		}	
	}

	override void OnWasDetached(EntityAI parent, int slot_id)
	{
		super.OnWasDetached(parent,slot_id);
		
		if( parent && parent.IsKindOf("KOD_M249_Base"))
		{
			HideAmmoBeltMagKOD(this);
		}	
	}			
};

class Mag_KOD_Evolys_100rnd: MagazineStorage 
{
	void Mag_KOD_Evolys_100rnd()
	{
		HideAmmoBeltMagKOD(this);
	}	
	
	override void OnWasAttached(EntityAI parent, int slot_id)
	{
		super.OnWasAttached(parent,slot_id);
		
		if( parent && parent.IsKindOf("KOD_Evolys_Base"))
		{
			ShowAmmoBeltMagKOD(this);
		}	
	}

	override void OnWasDetached(EntityAI parent, int slot_id)
	{
		super.OnWasDetached(parent,slot_id);
		
		if( parent && parent.IsKindOf("KOD_Evolys_Base"))
		{
			HideAmmoBeltMagKOD(this);
		}	
	}			
};

	void HideAmmoBeltMagKOD (EntityAI ParentItem)
	{
		protected int hideAmmoBeltId = -1;
		TStringArray selectionNames = new TStringArray;

		ParentItem.ConfigGetTextArray("simpleHiddenSelections",selectionNames);

		hideAmmoBeltId = selectionNames.Find("ammobelt");
			
		ParentItem.SetSimpleHiddenSelectionState(hideAmmoBeltId,false);
	};

	void ShowAmmoBeltMagKOD(EntityAI ParentItem)
	{
		protected int hideAmmoBeltId = -1;
		TStringArray selectionNames = new TStringArray;

		ParentItem.ConfigGetTextArray("simpleHiddenSelections",selectionNames);

		hideAmmoBeltId = selectionNames.Find("ammobelt");

		ParentItem.SetSimpleHiddenSelectionState(hideAmmoBeltId,true);
	};