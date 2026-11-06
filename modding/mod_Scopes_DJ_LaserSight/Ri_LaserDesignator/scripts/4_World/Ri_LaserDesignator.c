enum modelaser
{
	RayOnly = 0
	IrRay = 1
	Flaslight = 2
	RayFlashlight = 3
	AllOfff = 4
}
class Ri_LaserDesignator_Base : Switchable_Base
{
    void Ri_LaserDesignator_Base()
    {
		RegisterNetSyncVariableBool("WorkNonRaised");
		RegisterNetSyncVariableBool("ScreenCenterMode");
		RegisterNetSyncVariableFloat("Ralpha");
		RegisterNetSyncVariableFloat("Rred");
		RegisterNetSyncVariableFloat("Rgreen");
		RegisterNetSyncVariableFloat("Rblue");
		RegisterNetSyncVariableInt("Rdot");
		RegisterNetSyncVariableInt("MODE");
		RegisterNetSyncVariableBool("RAISE");
        if(GetGame().IsDedicatedServer())
        {
			foreach(ref LaserDesignatorConfig lcu : GetLaserDesignatorPlugin().LaserSettings)
			{
				if(this.IsKindOf(lcu.classname))
				{
					WorkNonRaised = GetLaserDesignatorPlugin().WorkNonRaised;
					ScreenCenterMode = GetLaserDesignatorPlugin().ScreenCenterMode;
					Ralpha = lcu.alpha;
					Rred = lcu.red;
					Rgreen = lcu.green;
					Rblue = lcu.blue;

        			if(lcu.dotptc)
        			Rdot = ParticleList.GetParticleIDByName(lcu.dotptc);
					else
					Rdot = 0;
					SetSynchDirty();
				}

        	}
		}
		InitAttachmentsSlotsToCheck(m_AttachmentSlotsCheck);
    }
	// Следующий режим по кругу. Вызывается только на сервере.
	void NextMode()
	{
		if (modes.Count() == 0)
			return;

		int index = modes.Find(MODE);
		MODE = modes.Get((index + 1) % modes.Count());
		SetSynchDirty();
	}
	void Raise()
	{
		RAISE = true;
		SetSynchDirty();
	}
	void Unraise()
	{
		RAISE = false;
		SetSynchDirty();
	}
    void ~Ri_LaserDesignator_Base()
    {
		EndaLL();
    }

    EntityAI 				Beam;
    EntityAI 				wpn;
    Particle 				p;
	UniversallightLight 	m_Light;
	bool 	RAISE = false;
    bool 	WorkNonRaised;
    bool 	ScreenCenterMode;
    float  	Ralpha;
    float 	Rred;
    float  	Rgreen;
    float 	Rblue;
	int 	Rdot;
	int 	MODE = 0;
	int 	oldMODE = -1;
	string colosr;
	ref array<int> modes = {};

	override void OnVariablesSynchronized()
	{
		super.OnVariablesSynchronized();

		colosr = string.Format("#(argb,8,8,3)color(%1,%2,%3,%4,CO)", Rred, Rgreen, Rblue, Ralpha);
		if(oldMODE!=MODE)
		{
			oldMODE = MODE;
			SetMode(MODE);
		}
	}
	void InitAttachmentsSlotsToCheck(out array<int> AttSlots)
	{
		if (!AttSlots)
		{
			AttSlots = new array<int>;
			AttSlots.Insert(InventorySlots.GetSlotIdFromString("weaponHandguardM4"));
			AttSlots.Insert(InventorySlots.GetSlotIdFromString("weaponHandguardAK"));
			AttSlots.Insert(InventorySlots.GetSlotIdFromString("weaponHandguardMP5"));
		}
	}

	ref array<int> 		m_AttachmentSlotsCheck;

	override bool CanPutAsAttachment( EntityAI parent )
	{
		if ( !super.CanPutAsAttachment(parent) ) {return false;}

		bool req_attachment 		= false;
		bool rail_attachment_found   = false;
		int slot_id;
		ItemBase attachment;
		for ( int i = 0; i < parent.GetInventory().GetAttachmentSlotsCount(); i++ )
		{
			slot_id = parent.GetInventory().GetAttachmentSlotId(i);
			if ( m_AttachmentSlotsCheck.Find(slot_id) != -1 )
			{
				req_attachment = true;
				attachment = ItemBase.Cast(parent.GetInventory().FindAttachment(slot_id));
				if ( attachment && attachment.ConfigIsExisting("hasRailFunctionality") && attachment.ConfigGetBool("hasRailFunctionality") )
					rail_attachment_found = true;
			}
		}
		return !req_attachment || (req_attachment && rail_attachment_found);
	}

	override void OnWasAttached( EntityAI parent, int slot_id )
	{
			EndaLL();
			MODE = modelaser.AllOfff;
			if (GetCompEM().IsSwitchedOn() )
			{
				GetCompEM().SwitchOff();
			}
		super.OnWasAttached( parent, slot_id );
	}
	override void OnWasDetached( EntityAI parent, int slot_id )
	{
			EndaLL();
			MODE = modelaser.AllOfff;
			if (GetCompEM().IsSwitchedOn() )
			{
				GetCompEM().SwitchOff();
			}
		super.OnWasDetached( parent, slot_id );
	}
	override void OnInventoryExit(Man player)
	{
			EndaLL();
			MODE = modelaser.AllOfff;
			if (GetCompEM().IsSwitchedOn() )
			{
				GetCompEM().SwitchOff();
			}
		super.OnInventoryExit(player);
	}
	override void OnItemLocationChanged(EntityAI old_owner, EntityAI new_owner)
	{
			EndaLL();
			MODE = modelaser.AllOfff;
			if (GetCompEM().IsSwitchedOn() )
			{
				GetCompEM().SwitchOff();
			}
		super.OnItemLocationChanged(old_owner, new_owner);
	}


	void SetMode(int mod = 0)
	{
		EndaLL();

		switch (mod)
		{
			case modelaser.RayOnly:
			InitLaser(colosr,Rdot);
				break;

			case modelaser.IrRay:
			InitLaser();
				break;

			case modelaser.Flaslight:
				initflash();
				break;

			case modelaser.RayFlashlight:
				{
					InitLaser(colosr,Rdot);
					initflash();
				}
				break;

			case modelaser.AllOfff:
				//initflash();
				break;

			default:
				break;
		}
	}

	void initflash()
	{
		if(!wpn)
			wpn = this.GetHierarchyParent();

		if(wpn)
		{
			if(!mplayer)
        		mplayer = PlayerBase.Cast(wpn.GetHierarchyParent());

			if(mplayer)
				GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(flashhandler, 15, true);
		}
	}

	bool beRaised = false;

	void flashhandler()
	{
		if(MODE==modelaser.Flaslight)
		if(((mplayer.IsRaised() || WorkNonRaised) && !beRaised)&& mplayer.GetHumanInventory().GetEntityInHands() == wpn)
		{
			beRaised = true;
			GetGame().RPCSingleParam(this, -18921973, NULL, false, NULL);
		}
		else if((!mplayer.IsRaised()&&!WorkNonRaised) && beRaised)
		{
			beRaised = false;
			GetGame().RPCSingleParam(this, -18921972, NULL, false, NULL);
		}

		if((RAISE || WorkNonRaised)&& !m_Light)
		{
			m_Light = UniversallightLight.Cast(  ScriptedLightBase.CreateLight(UniversallightLight, "0 0 0", 0.08)  );
			m_Light.AttachOnMemoryPoint(this, "beamStart", "beamEnd");

		}
		else if((!RAISE&&!WorkNonRaised) && m_Light)
		{
			m_Light.FadeOut();
		}
	}


	vector contact_point;
	vector contact_dir;
	int contact_component;
    PlayerBase mplayer;
	bool IsMe = false;

	vector AA[4];   vector BB[4];   vector AB;  vector AC;  vector AD; vector VI;

	void InitLaser(string colori = "#(argb,8,8,3)color(0.99,0.99,0.99,1,CO)",int dots = ParticleList.RDOT_W)
	{
		if(!wpn)
		wpn = this.GetHierarchyParent();
		if(wpn)
		{

		if(!mplayer)
        mplayer = PlayerBase.Cast(wpn.GetHierarchyParent());
		if(mplayer)
		{
			if(mplayer.IsControlledPlayer())
			IsMe = true;
        	Beam = EntityAI.Cast(GetGame().CreateObjectEx("Ri_Laser", "0 0 0", ECE_LOCAL));
			Beam.SetInvisible(true);
			Beam.SetObjectTexture(0,colori);
			if(dots!=0)
        	p = ParticleManager.GetInstance().PlayOnObject(dots,Beam,"0 1 0");
			if(p)
			p.SetInvisible(true);

			Beam.DisableSimulation(true);
        	Beam.SetOrientation("0 0 90");

			this.AddChild( Beam, -1);
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(LaserHandler, 15, true);


			AC = wpn.GetSelectionPositionLS("usti hlavne");
			if(colori != "#(argb,8,8,3)color(0.99,0.99,0.99,1,CO)")
			{
				VI = this.GetMemoryPointPos("startbeam");
				nonvg = true;
				this.SetObjectTexture(1,colori);
				this.SetSimpleHiddenSelectionState(0,false);
			}
			else
			{
				VI = this.GetMemoryPointPos("startbeam0");
				nonvg = false;
				this.SetObjectTexture(2,colori);
				Beam.SetObjectMaterial(0,"dz/gear/consumables/data/chemlight_white_on.rvmat");
				this.SetSimpleHiddenSelectionState(1,false);
			}
			}
		}
	}

	override void OnRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
	{
		// На 1.30 звуки предметов приходят через OnRPC, без super они терялись
		super.OnRPC(sender, rpc_type, ctx);

		if(rpc_type == -18921973)
		{
			if(this)
			Raise();
		}
		if(rpc_type == -18921972)
		{
			if(this)
			Unraise();
		}
	}

	bool nonvg = false;
	bool next = false;
	int zaderzh = 0;
	void LaserHandler()
	{
		if((nonvg || (PlayerBase.Cast(GetGame().GetPlayer()).IsNVGWorking() && Camera.GetCurrentFOV()>=0.35)) && mplayer.GetHumanInventory().GetEntityInHands() == wpn)
		{
			if(IsMe)
			{
				if(((mplayer.IsRaised() && mplayer.IsWeaponRaiseCompleted()) || WorkNonRaised) && !beRaised)
				{
					beRaised = true;
					Beam.SetInvisible(false);
					if(p)
					p.SetInvisible(false);

					if(nonvg)
					this.SetSimpleHiddenSelectionState(0,true);
					else
					this.SetSimpleHiddenSelectionState(1,true);


					next = true;
					GetGame().RPCSingleParam(this, -18921973, NULL, false, NULL);

				}
				else if(!mplayer.IsRaised() && beRaised && !WorkNonRaised)
				{
					beRaised = false;
					Beam.SetInvisible(true);
					if(p)
					p.SetInvisible(true);

					if(nonvg)
					this.SetSimpleHiddenSelectionState(0,false);
					else
					this.SetSimpleHiddenSelectionState(1,false);

					next = false;
					zaderzh=0;
					GetGame().RPCSingleParam(this, -18921972, NULL, false, NULL);
				}
			}
			else
			{
				if(RAISE || WorkNonRaised)
				{
					Beam.SetInvisible(false);
					if(p)
					p.SetInvisible(false);

					if(nonvg)
					this.SetSimpleHiddenSelectionState(0,true);
					else
					this.SetSimpleHiddenSelectionState(1,true);

					next = true;
				}
				else
				{
					Beam.SetInvisible(true);
					if(p)
					p.SetInvisible(true);

					if(nonvg)
					this.SetSimpleHiddenSelectionState(0,false);
					else
					this.SetSimpleHiddenSelectionState(1,false);

					next = false;
				}
			}
		}
		else if(beRaised)
		{
			Beam.SetInvisible(true);
			if(p)
			p.SetInvisible(true);

			this.SetSimpleHiddenSelectionState(1,false);
			next = false;
			beRaised = false;
		}

		if(next)
		{

        	wpn.GetTransform( BB );
			AD = this.ModelToWorld(VI);
        	if(!(ScreenCenterMode && zaderzh>=30) || !IsMe || (!mplayer.IsRaised() && !mplayer.IsWeaponRaiseCompleted() || mplayer.IsLiftWeapon() || mplayer.GetInputController().CameraIsFreeLook() || mplayer.GetInputController().IsReloadOrMechanismContinuousUse() || mplayer.GetInputController().IsReloadOrMechanismSingleUse() || mplayer.IsInIronsights() || mplayer.IsInOptics()))
        	{
				if(zaderzh<=30 && !mplayer.GetInputController().CameraIsFreeLook())
				zaderzh++;
				else if(zaderzh > 30)
				zaderzh=0;

				if(mplayer.GetInputController().IsReloadOrMechanismSingleUse())
				{
					zaderzh = -80;
				}
				if(mplayer.GetInputController().IsReloadOrMechanismContinuousUse())
				{
					zaderzh = -200;
				}


			    if(!DayZPhysics.RaycastRV((BB[3] +BB[0]*AC[0]*1 + BB[1]*AC[1]+ BB[2]*AC[2]),(BB[3] +BB[0]*AC[0]*235 + BB[1]*AC[1]+ BB[2]*AC[2]), contact_point, contact_dir, contact_component, null, null, wpn.GetHierarchyParent(), false, false, ObjIntersectFire))
			    {
        	        contact_point = BB[3] +BB[0]*AC[0]*235 + BB[1]*AC[1]+ BB[2]*AC[2];
        	    }
        	}
       		else
       		{
				if(zaderzh<40)
				zaderzh++;

	    	    vector contact_pointa;
				vector dir = GetGame().GetCurrentCameraDirection().VectorToAngles();
				float axi;

				axi = dir[0]+DayZPlayerImplement.Cast(GetGame().GetPlayer()).GetAimingModel().Getix();
				dir[0] = axi;
				axi = dir[1]+DayZPlayerImplement.Cast(GetGame().GetPlayer()).GetAimingModel().Getiy();
				dir[1] = axi;

				dir = dir.AnglesToVector();
			    vector to = GetGame().GetCurrentCameraPosition() + dir*100;
			   if (!DayZPhysics.RaycastRV((GetGame().GetCurrentCameraPosition()), to, contact_point, contact_dir, contact_component, null, null, mplayer, false, false, ObjIntersectFire) )
			   {
        	    if(!DayZPhysics.RaycastRV((BB[3] +BB[0]*AC[0] + BB[1]*AC[1]+ BB[2]*AC[2]),contact_point+BB[0]*AC[0]*2, contact_pointa, contact_dir, contact_component, null, null, mplayer, false, false, ObjIntersectFire))
			    {
        	        contact_point = BB[3] +BB[0]*AC[0]*235 + BB[1]*AC[1]+ BB[2]*AC[2];
        	    }
        	    contact_point = contact_pointa-BB[0]*AC[0]*0.5;
			   }
        	   else
        	   {
	    	    contact_pointa;
        	    if(!DayZPhysics.RaycastRV((BB[3] +BB[0]*AC[0] + BB[1]*AC[1]+ BB[2]*AC[2]),contact_point+BB[0]*AC[0]*1, contact_pointa, contact_dir, contact_component, null, null, mplayer, false, false, ObjIntersectFire))
			    {
        	        contact_point = BB[3] +BB[0]*AC[0]*235 + BB[1]*AC[1]+ BB[2]*AC[2];
        	    }
        	    contact_point = contact_pointa-BB[0]*AC[0]*0.5;
        	   }
      		}

			AB[0] = vector.Distance(contact_point, AD)+(-0.035);
			this.RemoveChild( Beam );
        	Beam.GetTransform( AA );
			AA[3] = VI;
			AB[1] = this.CoordToLocal(contact_point)[1]-VI[1];
			AB[2] = this.CoordToLocal(contact_point)[2]-VI[2];

			AA[1] = AB;
        	Beam.SetTransform(AA);

			this.AddChild( Beam, -1);
		}
	}
	override void OnWorkStart()
	{
		if (GetGame().IsDedicatedServer())
		{
			MODE = modelaser.RayOnly;
			SetSynchDirty();
		}
	}

	override void OnWorkStop()
	{
		oldMODE = -1;
		MODE = modelaser.AllOfff;
		if ( !GetGame().IsDedicatedServer())
		{
			EndaLL();
		}
		else
		SetSynchDirty();
	}
	void EndaLL()
	{
		// При закрытии сервера очереди вызовов уже нет, отсюда был NULL pointer в логах
		ScriptCallQueue queue;
		if (GetGame())
			queue = GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM);

		if (queue)
		{
			queue.Remove(LaserHandler);
			queue.Remove(flashhandler);
		}

		if (m_Light)
			m_Light.FadeOut();

		if(p)
			p.SetInvisible(true);

       	if(p)
       		p.Stop();

       	if(Beam)
       		GetGame().ObjectDelete(Beam);
		beRaised = false;
		wpn = NULL;
		m_Light = NULL;
		mplayer = NULL;
		nonvg = false;
		next = false;
		IsMe = false;
		zaderzh = 0;

		this.SetSimpleHiddenSelectionState(1,false);
		this.SetSimpleHiddenSelectionState(0,false);
	}


	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionTuneLaserModule);
	}

}
class Ri_Laser_Anpeq2 : Ri_LaserDesignator_Base
{
	void Ri_Laser_Anpeq2()
	{
		modes.Insert(modelaser.RayOnly);
		modes.Insert(modelaser.Flaslight);
		modes.Insert(modelaser.RayFlashlight);
		modes.Insert(modelaser.AllOfff);
	}
};
class Ri_Laser_Anpeq15 : Ri_LaserDesignator_Base
{
	void Ri_Laser_Anpeq15()
	{
		modes.Insert(modelaser.RayOnly);
		modes.Insert(modelaser.IrRay);
		modes.Insert(modelaser.Flaslight);
		modes.Insert(modelaser.RayFlashlight);
		modes.Insert(modelaser.AllOfff);
	}
};
class Ri_Laser_dbEbal : Ri_LaserDesignator_Base
{
	void Ri_Laser_dbEbal()
	{
		modes.Insert(modelaser.RayOnly);
		modes.Insert(modelaser.IrRay);
		modes.Insert(modelaser.Flaslight);
		modes.Insert(modelaser.RayFlashlight);
		modes.Insert(modelaser.AllOfff);
	}
};
class Ri_Laser_Ncstar : Ri_LaserDesignator_Base
{
	void Ri_Laser_Ncstar()
	{
		modes.Insert(modelaser.RayOnly);
		modes.Insert(modelaser.AllOfff);
	}
};
class Ri_Laser_Perst3 : Ri_LaserDesignator_Base
{
	void Ri_Laser_Perst3()
	{
		modes.Insert(modelaser.RayOnly);
		modes.Insert(modelaser.IrRay);
		modes.Insert(modelaser.Flaslight);
		modes.Insert(modelaser.RayFlashlight);
		modes.Insert(modelaser.AllOfff);
	}
};
class Ri_Laser_Surefire_x400 : Ri_LaserDesignator_Base
{
	void Ri_Laser_Surefire_x400()
	{
		modes.Insert(modelaser.RayOnly);
		modes.Insert(modelaser.Flaslight);
		modes.Insert(modelaser.RayFlashlight);
		modes.Insert(modelaser.AllOfff);
	}
};
class Ri_Laser_GL21 : Ri_LaserDesignator_Base
{
	void Ri_Laser_GL21()
	{
		modes.Insert(modelaser.RayOnly);
		modes.Insert(modelaser.Flaslight);
		modes.Insert(modelaser.RayFlashlight);
		modes.Insert(modelaser.AllOfff);
	}
	override bool CanPutAsAttachment( EntityAI parent )
	{
		if(IsRuined())
		return false;
		return true;
	}
}