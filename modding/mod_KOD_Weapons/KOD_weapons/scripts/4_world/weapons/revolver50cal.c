const float REV50Cal_ROTATION_POSITION_M1 = -0.333;
const float REV50Cal_ROTATION_POSITION_0 = 0.0;
const float REV50Cal_ROTATION_POSITION_1 = 0.333;
const float REV50Cal_ROTATION_POSITION_2 = 0.666;
const float REV50Cal_ROTATION_POSITION_3 = 1.0;


enum Revolver50CalAnimState
{
	DEFAULT 			= 0, 	///< default weapon state
};

enum Revolver50CalStableStateID
{
	UNKNOWN				=  0,
	DEFAULT				=  1,
}

class Revolver50CalCylinderRotate extends WeaponStateBase
{
	override void OnEntry (WeaponEventBase e)
	{
		KOD_Revolver50Cal_Cylinder cylinder = KOD_Revolver50Cal_Cylinder.Cast(m_weapon.GetAttachmentByType(KOD_Revolver50Cal_Cylinder));
		KOD_Revolver50Cal_Ejector ejector = KOD_Revolver50Cal_Ejector.Cast(m_weapon.GetAttachmentByType(KOD_Revolver50Cal_Ejector));
		
		//Magnum_Base magnum = Magnum_Base.Cast(m_weapon);
		
		//Magazine mag = m_weapon.GetMagazine(0);
		if(cylinder && ejector)
		{
			float a;
			int mi = m_weapon.GetCurrentMuzzle();
			switch(mi)
			{
				case 2:
					a = REV50Cal_ROTATION_POSITION_2;
					break;
				case 0:
					a = REV50Cal_ROTATION_POSITION_1;
					break;
				case 1:
					a = REV50Cal_ROTATION_POSITION_0;
					cylinder.ResetAnimationPhase("Rotate_Cylinder", REV50Cal_ROTATION_POSITION_M1 );
					ejector.ResetAnimationPhase("Rotate_Ejector", REV50Cal_ROTATION_POSITION_M1 );
					break;
			}	
			cylinder.SetAnimationPhase("Rotate_Cylinder", a );
			ejector.SetAnimationPhase("Rotate_Ejector", a );
			
		}

		super.OnEntry(e); // @NOTE: super at the end (prevent override from submachine start)
	}

};

class WeaponChambering_MultiMuzzleRevolver50Cal extends WeaponChambering_Cartridge
{
	override bool IsWaitingForActionFinish () { return false; }
	override void OnEntry(WeaponEventBase e)
	{
		super.OnEntry(e);

		/*for(int i = 0; i < m_weapon.GetMuzzleCount(); i++ )
		{
			if(!m_weapon.IsChamberFull(i))
			{
				m_weapon.ShowBullet(i);
				m_weapon.EffectBulletShow(i, m_damage, m_type);
				return;
			}
		}*/
	}
	
	override void OnExit (WeaponEventBase e)
	{
		m_weapon.SelectionBulletHide();
		int muzzle = m_weapon.GetCurrentMuzzle();
		if(muzzle == 0)
			muzzle = 2;
		else
			muzzle--;
		
		if(!m_weapon.IsChamberFull(muzzle))
		{
			if (m_weapon.PushCartridgeToChamber(muzzle, m_damage, m_type))
			{
				KOD_Revolver50Cal_Cylinder cylinder = KOD_Revolver50Cal_Cylinder.Cast(m_weapon.GetAttachmentByType(KOD_Revolver50Cal_Cylinder));

				if(cylinder)
				{
					string bullet = "bullet";
					string bullet_nose = "bullet_nose";
					
					if (muzzle > 0)
					{
						bullet = string.Format("bullet_" + (muzzle + 1));
						bullet_nose = string.Format("bullet_nose_" + (muzzle + 1));
					}
					cylinder.ShowSelection(bullet);
					cylinder.ShowSelection(bullet_nose);
				}
				m_weapon.SetCurrentMuzzle(muzzle);
				wpnDebugPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " WeaponChambering_Cartridge, ok - loaded chamber");
			}
			else
				wpnDebugPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " WeaponChambering_Cartridge, error - cannot load chamber chamber!");
			m_type = string.Empty;
			return;
		}
		else
			wpnDebugPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " WeaponChambering_Cartridge, error - cannot load chamber chamber!");
		
		//super.OnExit(e);
	}
}

class WeaponEjectAllMuzzlesRevolver50Cal extends WeaponStateBase
{
	Magazine m_dstMagazine;
	void WeaponEjectAllMuzzlesRevolver50Cal (Weapon_Base w = NULL, WeaponStateBase parent = NULL) { }

	override void OnEntry (WeaponEventBase e)
	{
		super.OnEntry(e);
		if (e)
		{
			KOD_Revolver50Cal_Cylinder cylinder = KOD_Revolver50Cal_Cylinder.Cast(m_weapon.GetAttachmentByType(KOD_Revolver50Cal_Cylinder));
			if(cylinder)
			{
				cylinder.HideSelection("bullet");
				cylinder.HideSelection("bullet_2");
				cylinder.HideSelection("bullet_3");
				
				cylinder.HideSelection("bullet_nose");
				cylinder.HideSelection("bullet_nose_2");
				cylinder.HideSelection("bullet_nose_3");
			}
			
			
			wpnDebugPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " ejected bullets from all muzzles");
			for( int i = 0; i < m_weapon.GetMuzzleCount(); i++ )
			{
				if(m_weapon.IsChamberFiredOut(i))
				{
					m_weapon.EjectCasing(i);
					m_weapon.EffectBulletHide(i);
					m_weapon.HideBullet(i);
					continue;
				}
				
				if(!m_weapon.IsChamberEmpty(i))
				{
					DayZPlayer p = e.m_player;
					if(m_dstMagazine && m_dstMagazine.GetAmmoCount() < m_dstMagazine.GetAmmoMax() )
					{
						m_weapon.CreateRound(i);
						ejectBulletAndStoreInMagazine(m_weapon,i,m_dstMagazine,p);
					}
					else
					{
						m_weapon.CreateRound(i);
						ejectBulletAndStoreInMagazine(m_weapon,i,null,p);
					}
					m_weapon.EffectBulletHide(i);
					m_weapon.HideBullet(i);
				}
			}
		}
		//m_weapon.SelectionBulletHide();
	}
	
	override bool IsWaitingForActionFinish () 
	{ 
		return false; 
	}
};

class WeaponRevolver50CalChambering extends WeaponStateBase
{
	WeaponActions m_action;
	int m_startActionType;
	int m_endActionType;
	Magazine m_srcMagazine; /// source of the cartridge
	ref InventoryLocation m_srcMagazinePrevLocation;

	ref WeaponStateBase m_start;
	ref WeaponEjectAllMuzzlesRevolver50Cal m_eject;
	ref Revolver50CalCylinderRotate m_rotate;
	ref WeaponChambering_Base m_chamber;
	ref LoopedChambering_Wait4ShowBullet2 m_w4sb2;
	ref WeaponStartAction m_endLoop;
	ref BulletHide_W4T m_hideB;

	void WeaponRevolver50CalChambering (Weapon_Base w = NULL, WeaponStateBase parent = NULL, WeaponActions action = WeaponActions.NONE, int startActionType = -1, int endActionType = -1)
	{
		m_action = action;
		m_startActionType = startActionType;
		m_endActionType = endActionType;

		// setup nested state machine
		m_start = new WeaponChambering_Start(m_weapon, this, m_action, m_startActionType);
		m_eject = new WeaponEjectAllMuzzlesRevolver50Cal(m_weapon, this);
		m_rotate = new Revolver50CalCylinderRotate(m_weapon, this);
		m_chamber = new WeaponChambering_MultiMuzzleRevolver50Cal(m_weapon, this);
		m_w4sb2 = LoopedChambering_Wait4ShowBullet2(m_weapon, this);
		m_hideB = new BulletHide_W4T(m_weapon, this);
		m_endLoop = new LoopedChambering_EndLoop(m_weapon, this, m_action, m_endActionType); // @NOTE: termination playing action - dummy?
		// events
		WeaponEventBase							_fin_ = new WeaponEventHumanCommandActionFinished;
		WeaponEventContinuousLoadBulletStart	__lS_ = new WeaponEventContinuousLoadBulletStart;
		WeaponEventContinuousLoadBulletEnd		__lE_ = new WeaponEventContinuousLoadBulletEnd;
		WeaponEventCylinderRotate				__cr_ = new WeaponEventCylinderRotate;
		WeaponEventAnimBulletShow				__bs_ = new WeaponEventAnimBulletShow;
		WeaponEventAnimBulletHide				__bh_ = new WeaponEventAnimBulletHide;
		WeaponEventAnimBulletEject				__be_ = new WeaponEventAnimBulletEject;
		WeaponEventAnimBulletInMagazine			__bM_ = new WeaponEventAnimBulletInMagazine;
		WeaponEventAnimBulletShow2				_bs2_ = new WeaponEventAnimBulletShow2;

		m_fsm = new WeaponFSM(this); // @NOTE: set owner of the submachine fsm
		m_fsm.AddTransition(new WeaponTransition(m_start,	__be_, m_eject));
		m_fsm.AddTransition(new WeaponTransition(m_start,	__cr_, m_rotate));
		m_fsm.AddTransition(new WeaponTransition(m_eject,	__cr_, m_rotate));
			
		m_fsm.AddTransition(new WeaponTransition(m_rotate,	__be_, m_eject));
		
		m_fsm.AddTransition(new WeaponTransition(m_eject, __bs_, m_chamber));
		

		m_fsm.AddTransition(new WeaponTransition(m_rotate, __bs_, m_chamber));	
		
		m_fsm.AddTransition(new WeaponTransition(m_chamber, __bM_, m_w4sb2, NULL, new GuardAnd(new GuardAnd(new WeaponGuardHasAmmoInLoopedState(m_chamber), new WeaponGuardChamberMultiHasRoomBulltetIgnoreLast(m_weapon)),new WeaponGuardWeaponManagerWantContinue())));
		m_fsm.AddTransition(new WeaponTransition(m_chamber, __bM_, m_endLoop));
		//m_fsm.AddTransition(new WeaponTransition(m_rotate, __bh_, m_chamber));
		//m_fsm.AddTransition(new WeaponTransition(m_w4sb2, 	__bh_, m_hideB));
		m_fsm.AddTransition(new WeaponTransition(m_w4sb2, 	__cr_, m_rotate));
		
		m_fsm.AddTransition(new WeaponTransition(m_endLoop, _fin_, NULL));
		
		// Safety exits
		m_fsm.AddTransition(new WeaponTransition(m_w4sb2, 	_fin_, null));
		m_fsm.AddTransition(new WeaponTransition(m_chamber, _fin_, null));
		m_fsm.AddTransition(new WeaponTransition(m_rotate,  _fin_, null));
		m_fsm.AddTransition(new WeaponTransition(m_eject  , _fin_, null));
		m_fsm.AddTransition(new WeaponTransition(m_start  , _fin_, null));		

		m_fsm.SetInitialState(m_start);
	}

	override void OnEntry (WeaponEventBase e)
	{
		if (e != NULL)
		{
			
			m_srcMagazine = e.m_magazine;
			if (m_srcMagazine != NULL)
			{
				InventoryLocation newSrc = new InventoryLocation;
				m_srcMagazine.GetInventory().GetCurrentInventoryLocation(newSrc);
				
				m_srcMagazinePrevLocation = newSrc;
		
				// move to LH
				InventoryLocation lhand = new InventoryLocation;
				lhand.SetAttachment(e.m_player, m_srcMagazine, InventorySlots.LEFTHAND);
				if (GameInventory.LocationSyncMoveEntity(newSrc, lhand))
				{
					wpnDebugPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " LoopedChambering, ok - ammo pile removed from inv (inv->LHand)");
				}
				else
					Error("[wpnfsm] " + Object.GetDebugName(m_weapon) + " LoopedChambering, error - cannot remove ammo pile from inv");
				
				m_chamber.m_srcMagazine = m_srcMagazine;
			} 
			else
			{
				Print("[wpnfsm] " + Object.GetDebugName(m_weapon) + " LoopedChambering m_srcMagazine = NULL");
			}
		}
		else
		{
			Print("[wpnfsm] " + Object.GetDebugName(m_weapon) + " LoopedChambering (e=NULL), m_srcMagazine=" + m_srcMagazine.ToString());
		}

		super.OnEntry(e); // @NOTE: super at the end (prevent override from submachine start)
	}
	
    #ifdef DAYZ_1_11
	override void OnExit (WeaponEventBase e)
	{
		bool done = false;
		if (m_srcMagazine)
		{
			e.m_player.GetInventory().ClearInventoryReservation( m_srcMagazine , m_srcMagazinePrevLocation );
			
			InventoryLocation leftHandIl = new InventoryLocation;
			m_srcMagazine.GetInventory().GetCurrentInventoryLocation(leftHandIl);
			if(leftHandIl.IsValid())
			{
				if(m_srcMagazinePrevLocation && m_srcMagazinePrevLocation.IsValid())
				{
					if(vector.DistanceSq(m_srcMagazinePrevLocation.GetPos(), leftHandIl.GetPos()) < WeaponManager.MAX_DROP_MAGAZINE_DISTANCE_SQ )
					{
						if( GameInventory.LocationCanMoveEntity(leftHandIl,m_srcMagazinePrevLocation) )
						{
							if( GameInventory.LocationSyncMoveEntity(leftHandIl,m_srcMagazinePrevLocation) )
							{
								wpnDebugPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " LoopedChambering, ok - ammo pile removed from left hand to previous location (LHand->inv) - exit");
								done = true;
							}
						}
					}
				}
				
				if( !done)
				{
					InventoryLocation il = new InventoryLocation;
					e.m_player.GetInventory().FindFreeLocationFor( m_srcMagazine, FindInventoryLocationType.CARGO, il );
			
					if(!il || !il.IsValid())
					{
						if (DayZPlayerUtils.HandleDropMagazine(e.m_player, m_srcMagazine))
							wpnDebugPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " LoopedChambering, ok - no inventory space for ammo pile - dropped to ground - exit");
						else
							Error("[wpnfsm] " + Object.GetDebugName(m_weapon) + " LoopedChambering, error - cannot drop ammo pile from left hand after not found inventory space for ammo pile - exit");
					
					}
					else
					{
						if (GameInventory.LocationSyncMoveEntity(leftHandIl, il))
						{
							wpnDebugPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " LoopedChambering, ok - ammo pile removed from left hand (LHand->inv) - exit");
						}
						else
							Error("[wpnfsm] " + Object.GetDebugName(m_weapon) + " LoopedChambering, error - cannot remove ammo pile from wpn - exit");
					}
				}
			}
		}
		
		super.OnExit(e);
		m_srcMagazine = NULL;
		m_chamber.m_srcMagazine = NULL;
		m_srcMagazinePrevLocation = NULL;
	}
    #else	
	override void OnExit (WeaponEventBase e)
	{
		bool done = false;
		if (m_srcMagazine)
		{
			e.m_player.GetInventory().ClearInventoryReservationEx( m_srcMagazine , m_srcMagazinePrevLocation );
			
			InventoryLocation leftHandIl = new InventoryLocation;
			m_srcMagazine.GetInventory().GetCurrentInventoryLocation(leftHandIl);
			if(leftHandIl.IsValid())
			{
				if(m_srcMagazinePrevLocation && m_srcMagazinePrevLocation.IsValid())
				{
					if(vector.DistanceSq(m_srcMagazinePrevLocation.GetPos(), leftHandIl.GetPos()) < WeaponManager.MAX_DROP_MAGAZINE_DISTANCE_SQ )
					{
						if( GameInventory.LocationCanMoveEntity(leftHandIl,m_srcMagazinePrevLocation) )
						{
							if( GameInventory.LocationSyncMoveEntity(leftHandIl,m_srcMagazinePrevLocation) )
							{
								wpnDebugPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " LoopedChambering, ok - ammo pile removed from left hand to previous location (LHand->inv) - exit");
								done = true;
							}
						}
					}
				}
				
				if( !done)
				{
					InventoryLocation il = new InventoryLocation;
					e.m_player.GetInventory().FindFreeLocationFor( m_srcMagazine, FindInventoryLocationType.CARGO, il );
			
					if(!il || !il.IsValid())
					{
						if (DayZPlayerUtils.HandleDropMagazine(e.m_player, m_srcMagazine))
							wpnDebugPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " LoopedChambering, ok - no inventory space for ammo pile - dropped to ground - exit");
						else
							Error("[wpnfsm] " + Object.GetDebugName(m_weapon) + " LoopedChambering, error - cannot drop ammo pile from left hand after not found inventory space for ammo pile - exit");
					
					}
					else
					{
						if (GameInventory.LocationSyncMoveEntity(leftHandIl, il))
						{
							wpnDebugPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " LoopedChambering, ok - ammo pile removed from left hand (LHand->inv) - exit");
						}
						else
							Error("[wpnfsm] " + Object.GetDebugName(m_weapon) + " LoopedChambering, error - cannot remove ammo pile from wpn - exit");
					}
				}
			}
		}
		
		super.OnExit(e);
		m_srcMagazine = NULL;
		m_chamber.m_srcMagazine = NULL;
		m_srcMagazinePrevLocation = NULL;
	}
    #endif	
	
    #ifdef DAYZ_1_11
	override void OnAbort (WeaponEventBase e)
	{
		bool done = false;
		if (m_srcMagazine)
		{
			e.m_player.GetInventory().ClearInventoryReservation( m_srcMagazine , m_srcMagazinePrevLocation );
			
			InventoryLocation leftHandIl = new InventoryLocation;
			m_srcMagazine.GetInventory().GetCurrentInventoryLocation(leftHandIl);
			if(leftHandIl.IsValid())
			{
				if(m_srcMagazinePrevLocation && m_srcMagazinePrevLocation.IsValid())
				{
					if(vector.DistanceSq(m_srcMagazinePrevLocation.GetPos(), leftHandIl.GetPos()) < WeaponManager.MAX_DROP_MAGAZINE_DISTANCE_SQ )
					{
						if( GameInventory.LocationCanMoveEntity(leftHandIl,m_srcMagazinePrevLocation) )
						{
							if( GameInventory.LocationSyncMoveEntity(leftHandIl,m_srcMagazinePrevLocation) )
							{
								wpnDebugPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " LoopedChambering, ok - ammo pile removed from left hand to previous location (LHand->inv) - abort");
								done = true;
							}
						}
					}
				}
				
				if( !done)
				{
					InventoryLocation il = new InventoryLocation;
					e.m_player.GetInventory().FindFreeLocationFor( m_srcMagazine, FindInventoryLocationType.CARGO, il );
			
					if(!il || !il.IsValid())
					{
						if (DayZPlayerUtils.HandleDropMagazine(e.m_player, m_srcMagazine))
							wpnDebugPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " LoopedChambering, ok - no inventory space for ammo pile - dropped to ground - abort");
						else
							Error("[wpnfsm] " + Object.GetDebugName(m_weapon) + " LoopedChambering, error - cannot drop ammo pile from left hand after not found inventory space for ammo pile - abort");
					
					}
					else
					{
						if (GameInventory.LocationSyncMoveEntity(leftHandIl, il))
						{
							wpnDebugPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " LoopedChambering, ok - ammo pile removed from left hand (LHand->inv) - abort");
						}
						else
							Error("[wpnfsm] " + Object.GetDebugName(m_weapon) + " LoopedChambering, error - cannot remove ammo pile from wpn - abort");
					}
				}
			}
		}
		
		super.OnAbort(e);
		m_srcMagazine = NULL;
		m_chamber.m_srcMagazine = NULL;
		m_srcMagazinePrevLocation = NULL;
	}
    #else	
	override void OnAbort (WeaponEventBase e)
	{
		bool done = false;
		if (m_srcMagazine)
		{
			e.m_player.GetInventory().ClearInventoryReservationEx( m_srcMagazine , m_srcMagazinePrevLocation );
			
			InventoryLocation leftHandIl = new InventoryLocation;
			m_srcMagazine.GetInventory().GetCurrentInventoryLocation(leftHandIl);
			if(leftHandIl.IsValid())
			{
				if(m_srcMagazinePrevLocation && m_srcMagazinePrevLocation.IsValid())
				{
					if(vector.DistanceSq(m_srcMagazinePrevLocation.GetPos(), leftHandIl.GetPos()) < WeaponManager.MAX_DROP_MAGAZINE_DISTANCE_SQ )
					{
						if( GameInventory.LocationCanMoveEntity(leftHandIl,m_srcMagazinePrevLocation) )
						{
							if( GameInventory.LocationSyncMoveEntity(leftHandIl,m_srcMagazinePrevLocation) )
							{
								wpnDebugPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " LoopedChambering, ok - ammo pile removed from left hand to previous location (LHand->inv) - abort");
								done = true;
							}
						}
					}
				}
				
				if( !done)
				{
					InventoryLocation il = new InventoryLocation;
					e.m_player.GetInventory().FindFreeLocationFor( m_srcMagazine, FindInventoryLocationType.CARGO, il );
			
					if(!il || !il.IsValid())
					{
						if (DayZPlayerUtils.HandleDropMagazine(e.m_player, m_srcMagazine))
							wpnDebugPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " LoopedChambering, ok - no inventory space for ammo pile - dropped to ground - abort");
						else
							Error("[wpnfsm] " + Object.GetDebugName(m_weapon) + " LoopedChambering, error - cannot drop ammo pile from left hand after not found inventory space for ammo pile - abort");
					
					}
					else
					{
						if (GameInventory.LocationSyncMoveEntity(leftHandIl, il))
						{
							wpnDebugPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " LoopedChambering, ok - ammo pile removed from left hand (LHand->inv) - abort");
						}
						else
							Error("[wpnfsm] " + Object.GetDebugName(m_weapon) + " LoopedChambering, error - cannot remove ammo pile from wpn - abort");
					}
				}
			}
		}
		
		super.OnAbort(e);
		m_srcMagazine = NULL;
		m_chamber.m_srcMagazine = NULL;
		m_srcMagazinePrevLocation = NULL;
	}
    #endif	

	override bool SaveCurrentFSMState (ParamsWriteContext ctx)
	{
		if (!super.SaveCurrentFSMState(ctx))
			return false;

		if (!ctx.Write(m_srcMagazine))
		{
			Error("[wpnfsm] " + Object.GetDebugName(m_weapon) + " LoopedChambering.SaveCurrentFSMState: cannot save m_srcMagazine for weapon=" + m_weapon);
			return false;
		}
		
		if (!OptionalLocationWriteToContext(m_srcMagazinePrevLocation, ctx))
		{
			Error("[wpnfsm] " + Object.GetDebugName(m_weapon) + " LoopedChambering.SaveCurrentFSMState: cannot write m_srcMagazinePrevLocation for weapon=" + m_weapon);
			return false;
		}
		
		return true;
	}

	override bool LoadCurrentFSMState (ParamsReadContext ctx, int version)
	{
		if (!super.LoadCurrentFSMState(ctx, version))
			return false;

		if (!ctx.Read(m_srcMagazine))
		{
			Error("[wpnfsm] " + Object.GetDebugName(m_weapon) + " LoopedChambering.LoadCurrentFSMState: cannot read m_srcMagazine for weapon=" + m_weapon);
			return false;
		}
		
		if (!OptionalLocationReadFromContext(m_srcMagazinePrevLocation, ctx))
		{
			Error("[wpnfsm] " + Object.GetDebugName(m_weapon) + " LoopedChambering.LoadCurrentFSMState: cannot read m_srcMagazinePrevLocation for weapon=" + m_weapon);
			return false;
		}
		return true;
	}
};

class WeaponFireRevolver50Cal extends WeaponFireMultiMuzzle
{
	override void OnEntry (WeaponEventBase e)
	{
		super.OnEntry(e);
		if (e)
		{
			KOD_Revolver50Cal_Cylinder cylinder = KOD_Revolver50Cal_Cylinder.Cast(m_weapon.GetAttachmentByType(KOD_Revolver50Cal_Cylinder));
			KOD_Revolver50Cal_Ejector ejector = KOD_Revolver50Cal_Ejector.Cast(m_weapon.GetAttachmentByType(KOD_Revolver50Cal_Ejector));
			if(cylinder && ejector)
			{
				float a;
				int mi = m_weapon.GetCurrentMuzzle();
				switch(mi)
				{
					case 0:
						a = REV50Cal_ROTATION_POSITION_0;
						break;
					case 1:
						a = REV50Cal_ROTATION_POSITION_2;
						cylinder.ResetAnimationPhase("Rotate_Cylinder", REV50Cal_ROTATION_POSITION_3 );
						ejector.ResetAnimationPhase("Rotate_Ejector", REV50Cal_ROTATION_POSITION_3 );
						break;
					case 2:
						a = REV50Cal_ROTATION_POSITION_1;
						break;
				}
				cylinder.SetAnimationPhase("Rotate_Cylinder", a );
				ejector.SetAnimationPhase("Rotate_Ejector", a );
			}
		}
	}

	override void OnUpdate (float dt)
	{
		m_dtAccumulator += dt;

		DayZPlayer p;
		Class.CastTo(p, m_weapon.GetHierarchyParent());

		int muzzleIndex = m_weapon.GetCurrentMuzzle();
		float reloadTime = m_weapon.GetReloadTime(muzzleIndex);
		if (m_dtAccumulator >= reloadTime)
			if (m_weapon.CanProcessWeaponEvents())
				m_weapon.ProcessWeaponEvent(new WeaponEventReloadTimeout(p));
	}

	override void OnExit (WeaponEventBase e)
	{
		if (e)
			m_dtAccumulator = 0;
		super.OnExit(e);
	}
};

class Revolver50Cal_Static_State extends WeaponStableState
{
	bool init = false;
	override void OnEntry(WeaponEventBase e) 
	{		
		if (LogManager.IsWeaponLogEnable()) { wpnPrint("[wpnstate] { Magnum stable state"); } 
		super.OnEntry(e);
		if (init)
		{
			KOD_Revolver50Cal_Base magnum;
			if (CastTo(magnum, m_weapon))
			{
				magnum.SyncCylinderRotation();
				//magnum.SyncSelectionStateFromState();
			}
		}
		init = true;
	}
	override void OnExit (WeaponEventBase e) { super.OnExit(e); if (LogManager.IsWeaponLogEnable()) { wpnPrint("[wpnstate] }  Magnum stable state"); } }
	override int GetCurrentStateID () { return MagnumStableStateID.DEFAULT; }
	override bool HasBullet() { return m_weapon.IsChamberFull(m_weapon.GetCurrentMuzzle()); }
	override bool HasMagazine() { return false; }
	override bool IsJammed() { return m_weapon.IsJammed(); }
	override bool IsSingleState() { return true; }
};

/*
class Revolver50Cal_Static_State extends WeaponStableState
{
	bool init = false;
	override void OnEntry(WeaponEventBase e) 
	{		
		wpnPrint("[wpnstate] { Revolver 50Cal stable state"); 
		super.OnEntry(e);
		if(init)
		{
			KOD_Revolver50Cal_Cylinder cylinder = KOD_Revolver50Cal_Cylinder.Cast(m_weapon.GetAttachmentByType(KOD_Revolver50Cal_Cylinder));
			KOD_Revolver50Cal_Ejector ejector = KOD_Revolver50Cal_Ejector.Cast(m_weapon.GetAttachmentByType(KOD_Revolver50Cal_Ejector));
			
			if(cylinder && ejector)
			{
				float rot;
				int mi = m_weapon.GetCurrentMuzzle();
				switch(mi)
				{
	
					case 0:
						rot = REV50Cal_ROTATION_POSITION_0;
						break;
					case 1:
						rot = REV50Cal_ROTATION_POSITION_2;
						break;	
					case 2:
						rot = REV50Cal_ROTATION_POSITION_1;
						break;	
				}
				float anim_phase = cylinder.GetAnimationPhase("Rotate_Cylinder");
				if(anim_phase + 0.1 < rot || anim_phase - 0.1 > rot)
				{
					cylinder.ResetAnimationPhase("Rotate_Cylinder", rot );
					cylinder.SetAnimationPhase("Rotate_Cylinder", rot );
					ejector.ResetAnimationPhase("Rotate_Ejector", rot );
					ejector.SetAnimationPhase("Rotate_Ejector", rot );
				}
			}
		}
		init = true;
	}
	override void OnExit (WeaponEventBase e) { super.OnExit(e); wpnPrint("[wpnstate] }  Revolver 50Cal stable state"); }
	override int GetCurrentStateID () { return Revolver50CalStableStateID.DEFAULT; }
	override bool HasBullet () { return false; }
	override bool HasMagazine () { return false; }
	override bool IsJammed () { return false; }
};
*/
class WeaponChargingRevolver50Cal extends WeaponStateBase
{
	WeaponActions m_action;
	int m_actionType;

	ref WeaponCharging_Start m_start;
	ref WeaponCharging_CK m_onCK;
	ref WeaponChamberFromAttMag_W4T m_chamber;
	ref WeaponEjectAllMuzzlesRevolver50Cal m_eject;

	void WeaponChargingRevolver50Cal (Weapon_Base w = NULL, WeaponStateBase parent = NULL, WeaponActions action = WeaponActions.NONE, int actionType = -1)
	{
		m_action = action;
		m_actionType = actionType;

		// setup nested state machine
		m_start = new WeaponCharging_Start(m_weapon, this, m_action, m_actionType);
		m_onCK = new WeaponCharging_CK(m_weapon, this);
		m_chamber = new WeaponChamberFromAttMag_W4T(m_weapon, this);
		m_eject = new WeaponEjectAllMuzzlesRevolver50Cal(m_weapon, this);

		// events
		WeaponEventBase __be_ = new WeaponEventAnimBulletEject;
		WeaponEventBase __bh_ = new WeaponEventAnimBulletHide;
		WeaponEventBase __ck_ = new WeaponEventAnimCocked;
		WeaponEventBase _fin_ = new WeaponEventHumanCommandActionFinished;

		m_fsm = new WeaponFSM(this); // @NOTE: set owner of the submachine fsm
		// transitions
		m_fsm.AddTransition(new WeaponTransition(  m_start, __be_, m_eject ));
		m_fsm.AddTransition(new WeaponTransition(  m_start, _fin_, NULL ));
		
		m_fsm.AddTransition(new WeaponTransition(  m_start, __ck_, m_eject, NULL, new GuardNot(new WeaponGuardCurrentChamberEmpty(m_weapon))));
		m_fsm.AddTransition(new WeaponTransition(  m_start, __ck_, m_chamber, NULL, new WeaponGuardHasAmmo(m_weapon))); // some anims do not send BE event
		m_fsm.AddTransition(new WeaponTransition(  m_start, __ck_, m_onCK)); // some anims do not send BE event
		
		m_fsm.AddTransition(new WeaponTransition(   m_eject, __ck_, m_chamber, NULL, new WeaponGuardHasAmmo(m_weapon)));
		m_fsm.AddTransition(new WeaponTransition(   m_eject, __ck_, m_onCK));
		m_fsm.AddTransition(new WeaponTransition(   m_eject, _fin_, NULL));
		
		m_fsm.AddTransition(new WeaponTransition(   m_onCK, _fin_, NULL));
		
		m_fsm.AddTransition(new WeaponTransition(m_chamber, _fin_, NULL));

		m_fsm.SetInitialState(m_start);
	}
};

class KOD_Revolver50Cal_Base extends Weapon_Base
{
	ref WeaponStateBase C;
	int m_LastMuzzleloaded;
	int m_ActiveMuzzle;
	const string ATT_SLOT_CYLINDER = "RevolverCylinder50Cal";
	const string ATT_SLOT_EJECTOR = "RevolverEjector50Cal";
	
	override void SetActions()
	{
		super.SetActions();
		AddAction(FirearmActionLoadMultiBullet);

		RemoveAction(FirearmActionLoadBulletQuick); // Easy reload
		AddAction(FirearmActionLoadMultiBulletQuick); // Easy reload

	}
	
	override RecoilBase SpawnRecoilObject()
	{
		return new MagnumRecoil(this);
	}
	
	void KOD_Revolver50Cal_Base ()
	{
		m_LastMuzzleloaded = 0;
		m_ActiveMuzzle = 0;
		
		/*m_abilities.Insert(new AbilityRecord(WeaponActions.CHAMBERING, WeaponActionTypes.CHAMBERING_STARTLOOPABLE_CLOSED));
		m_abilities.Insert(new AbilityRecord(WeaponActions.CHAMBERING, WeaponActionTypes.CHAMBERING_ENDLOOPABLE));
		m_abilities.Insert(new AbilityRecord(WeaponActions.MECHANISM, WeaponActionTypes.MECHANISM_CLOSED));
		m_abilities.Insert(new AbilityRecord(WeaponActions.MECHANISM, WeaponActionTypes.MECHANISM_SPECIAL));
		m_abilities.Insert(new AbilityRecord(WeaponActions.UNJAMMING, WeaponActionTypes.UNJAMMING_START));
		m_abilities.Insert(new AbilityRecord(WeaponActions.UNJAMMING, WeaponActionTypes.UNJAMMING_END));
		m_abilities.Insert(new AbilityRecord(WeaponActions.FIRE, WeaponActionTypes.FIRE_NORMAL));
		m_abilities.Insert(new AbilityRecord(WeaponActions.FIRE, WeaponActionTypes.FIRE_COCKED));*/
	}
	
	override void EEInit()
	{
		super.EEInit();
		
		GetGame().GetCallQueue( CALL_CATEGORY_GAMEPLAY ).Call( AssembleGun );
	}
	
	override void InitStateMachine ()
	{
		m_abilities.Insert(new AbilityRecord(WeaponActions.CHAMBERING, WeaponActionChamberingTypes.CHAMBERING_STARTLOOPABLE_CLOSED));
		m_abilities.Insert(new AbilityRecord(WeaponActions.CHAMBERING, WeaponActionChamberingTypes.CHAMBERING_STARTLOOPABLE_CLOSED_KEEP));
		m_abilities.Insert(new AbilityRecord(WeaponActions.CHAMBERING, WeaponActionChamberingTypes.CHAMBERING_ENDLOOPABLE));
		m_abilities.Insert(new AbilityRecord(WeaponActions.MECHANISM, WeaponActionMechanismTypes.MECHANISM_CLOSED));
		m_abilities.Insert(new AbilityRecord(WeaponActions.MECHANISM, WeaponActionMechanismTypes.MECHANISM_SPECIAL));
		m_abilities.Insert(new AbilityRecord(WeaponActions.FIRE, WeaponActionFireTypes.FIRE_NORMAL));
		m_abilities.Insert(new AbilityRecord(WeaponActions.FIRE, WeaponActionFireTypes.FIRE_COCKED));
		m_abilities.Insert(new AbilityRecord(WeaponActions.FIRE, WeaponActionFireTypes.FIRE_UNCOCKED));


		// setup state machine
		// basic weapon state
		C = new Revolver50Cal_Static_State(this, NULL, Revolver50CalAnimState.DEFAULT);


		WeaponRevolver50CalChambering Chamber = new WeaponRevolver50CalChambering(this, NULL, WeaponActions.CHAMBERING, WeaponActionChamberingTypes.CHAMBERING_STARTLOOPABLE_CLOSED_KEEP,WeaponActionChamberingTypes.CHAMBERING_ENDLOOPABLE);
		WeaponRevolver50CalChambering Chamber_E = new WeaponRevolver50CalChambering(this, NULL, WeaponActions.CHAMBERING, WeaponActionChamberingTypes.CHAMBERING_STARTLOOPABLE_CLOSED,WeaponActionChamberingTypes.CHAMBERING_ENDLOOPABLE);
		//WeaponRevolver50CalChambering Chamber_CC00 = new WeaponRevolver50CalChambering(this, NULL, WeaponActions.CHAMBERING, WeaponActionChamberingTypes.CHAMBERING_END);

		WeaponChargingRevolver50Cal Mech = new WeaponChargingRevolver50Cal(this, NULL, WeaponActions.MECHANISM, WeaponActionMechanismTypes.MECHANISM_CLOSED);
		
		WeaponStateBase	Trigger_normal = new WeaponFireRevolver50Cal(this, NULL, WeaponActions.FIRE, WeaponActionFireTypes.FIRE_NORMAL);
		WeaponStateBase	Trigger_dry = new WeaponFireRevolver50Cal(this, NULL, WeaponActions.FIRE, WeaponActionFireTypes.FIRE_COCKED);
		
		// events

		WeaponEventBase __L__ = new WeaponEventLoad1Bullet;
		WeaponEventBase _fin_ = new WeaponEventHumanCommandActionFinished;
		WeaponEventBase _abt_ = new WeaponEventHumanCommandActionAborted;
		WeaponEventBase __M__ = new WeaponEventMechanism;
		WeaponEventBase __T__ = new WeaponEventTrigger;
		

		WeaponEventBase _rto_ = new WeaponEventReloadTimeout;
		WeaponEventBase _dto_ = new WeaponEventDryFireTimeout;

	
		m_fsm = new WeaponFSM();



		m_fsm.AddTransition(new WeaponTransition( C,				__L__,	Chamber, null, new WeaponGuardAnyChamberFiredOut(this))); // chamber from closed charged
		m_fsm.AddTransition(new WeaponTransition(  Chamber,	_fin_,	C));
		m_fsm.AddTransition(new WeaponTransition(  Chamber,	_abt_,	C));

		
		m_fsm.AddTransition(new WeaponTransition( C,				__L__,	Chamber_E)); // chamber from closed charged
		m_fsm.AddTransition(new WeaponTransition(  Chamber_E,	_fin_,	C));
		m_fsm.AddTransition(new WeaponTransition(  Chamber_E,	_abt_,	C));

		
		m_fsm.AddTransition(new WeaponTransition( C,				__M__,	Mech)); // charge from closed
		m_fsm.AddTransition(new WeaponTransition(  Mech,		_fin_,	C));
		m_fsm.AddTransition(new WeaponTransition(  Mech,		_abt_,	C));

		
		
		m_fsm.AddTransition(new WeaponTransition( C,				__T__,	Trigger_normal, null, new GuardAnd (new WeaponGuardCurrentChamberFull(this), new GuardNot(new WeaponGuardCurrentChamberFiredOut(this))) )); // fire.cocked
		m_fsm.AddTransition(new WeaponTransition(  Trigger_normal,	_fin_,	C));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_normal, 	_dto_,	C));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_normal, 	_abt_,	C));
		
		m_fsm.AddTransition(new WeaponTransition( C,				__T__,	Trigger_dry)); // fire.cocked
		m_fsm.AddTransition(new WeaponTransition(  Trigger_dry,	_fin_,	C));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_dry, 	_dto_,	C));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_dry, 	_abt_,	C));


		m_fsm.SetInitialState(C);
		SelectionBulletHide();
		SetCurrentMuzzle(0);
		//HideMagazine();
		m_fsm.Start();
	}
	
	override bool CanChamberBullet (int muzzleIndex, Magazine mag)
	{
		for(int i = 0; i < GetMuzzleCount(); i++)
		{
			if( CanChamberFromMag(i, mag) )
			{
				if(IsChamberEmpty(i))
					return true;
				
				if(IsChamberFiredOut(i))
					return true;
			} 
		}
		return false;
	}
	
	void AssembleGun()
	{
		if ( !FindAttachmentBySlotName(ATT_SLOT_EJECTOR) && !FindAttachmentBySlotName(ATT_SLOT_CYLINDER) )
		{
			GetInventory().CreateAttachment("KOD_Revolver50Cal_Ejector");
			GetInventory().CreateAttachment("KOD_Revolver50Cal_Cylinder");
		}

		ForceSyncSelectionState();
		SyncCylinderRotation();
	}
	
	void SyncCylinderRotation(bool reset = true)
	{
		SetCylinderRotationAnimationPhase(GetCylinderRotation(GetCurrentMuzzle()), reset);
	}
	
	void SetCylinderRotationAnimationPhase(float rot, bool reset = false)
	{
		KOD_Revolver50Cal_Cylinder cylinder = KOD_Revolver50Cal_Cylinder.Cast(GetAttachmentByType(KOD_Revolver50Cal_Cylinder));
		KOD_Revolver50Cal_Ejector ejector = KOD_Revolver50Cal_Ejector.Cast(GetAttachmentByType(KOD_Revolver50Cal_Ejector));
		if (cylinder && ejector)
		{
			float anim_phase = cylinder.GetAnimationPhase("Rotate_Cylinder");
			if ( Math.AbsFloat(anim_phase - rot) > 0.1 )
			{
				if (reset)
				{
					cylinder.ResetAnimationPhase("Rotate_Cylinder", rot );
					ejector.ResetAnimationPhase("Rotate_Ejector", rot );				
				}				
				else if (rot == REV50Cal_ROTATION_POSITION_0)
				{
					cylinder.ResetAnimationPhase("Rotate_Cylinder", REV50Cal_ROTATION_POSITION_M1 );
					ejector.ResetAnimationPhase("Rotate_Ejector", REV50Cal_ROTATION_POSITION_M1 );
				}
				
				cylinder.SetAnimationPhase("Rotate_Cylinder", rot );
				ejector.SetAnimationPhase("Rotate_Ejector", rot );
			}
		}
	}
	
	static float GetCylinderRotation(int muzzleIndex)
	{
		switch (muzzleIndex)
		{
			case 0:
				return REV50Cal_ROTATION_POSITION_0;
			case 1:
				return REV50Cal_ROTATION_POSITION_2;
			case 2:
				return REV50Cal_ROTATION_POSITION_1;
		}
		
		ErrorEx(string.Format("Invalid muzzle index: %1", muzzleIndex));
		
		return REV50Cal_ROTATION_POSITION_0;
	}
	
	/*
	void AssembleGun()
	{
		if ( !FindAttachmentBySlotName(ATT_SLOT_EJECTOR) && !FindAttachmentBySlotName(ATT_SLOT_CYLINDER) )
		{
			GetInventory().CreateAttachment("KOD_Revolver50Cal_Ejector");
			GetInventory().CreateAttachment("KOD_Revolver50Cal_Cylinder");
		}
		else
		{
			KOD_Revolver50Cal_Cylinder cylinder = KOD_Revolver50Cal_Cylinder.Cast(GetAttachmentByType(KOD_Revolver50Cal_Cylinder));
			KOD_Revolver50Cal_Ejector ejector = KOD_Revolver50Cal_Ejector.Cast(GetAttachmentByType(KOD_Revolver50Cal_Ejector));

			if(cylinder && ejector)
			{			
				for (int i = 0; i < GetMuzzleCount(); i++ )
				{
					if (IsChamberFull(i))
					{
						string bullet = "bullet";
						if (i > 0)
							bullet = string.Format("bullet_" + (i + 1));
						cylinder.ShowSelection(bullet);
						
						if (!IsChamberFiredOut(i))
						{
							string bullet_nose = "bullet_nose";
							if (i > 0)
								bullet_nose = string.Format("bullet_nose_" + (i + 1));
							cylinder.ShowSelection(bullet_nose);
						}
					}
				}
				
				float rot;
				int mi = GetCurrentMuzzle();
				switch(mi)
				{
	
					case 0:
						rot = REV50Cal_ROTATION_POSITION_0;
						break;
					case 1:
						rot = REV50Cal_ROTATION_POSITION_2;
						break;	
					case 2:
						rot = REV50Cal_ROTATION_POSITION_1;
						break;	
				}
				float anim_phase = cylinder.GetAnimationPhase("Rotate_Cylinder");
				if(anim_phase + 0.1 < rot || anim_phase - 0.1 > rot)
				{
					cylinder.ResetAnimationPhase("Rotate_Cylinder", rot );
					cylinder.SetAnimationPhase("Rotate_Cylinder", rot );
					ejector.ResetAnimationPhase("Rotate_Ejector", rot );
					ejector.SetAnimationPhase("Rotate_Ejector", rot );
				}
			}
		}
	}
	*/
	
	override void EEHealthLevelChanged(int oldLevel, int newLevel, string zone)
	{
		super.EEHealthLevelChanged(oldLevel,newLevel,zone);
		
		if (GetGame().IsClient())
			return;
		
		SetAttachmentsHealth();
	}
	
	//sets health of cylinder and ejector objects
	void SetAttachmentsHealth()
	{
		EntityAI entity;
		ItemBase attachment;
		for (int i = 0; i < GetInventory().AttachmentCount(); i++)
		{
			entity = GetInventory().GetAttachmentFromIndex(i);
			if ( Class.CastTo(attachment,entity) )
			{
				attachment.SetHealth01("","Health",GetHealth01());
			}
		}
	}
	
	override void OnFire(int muzzle_index)
	{
		KOD_Revolver50Cal_Cylinder cylinder = KOD_Revolver50Cal_Cylinder.Cast(GetAttachmentByType(KOD_Revolver50Cal_Cylinder));
		if (cylinder)
		{
			string bullet_nose = "bullet_nose";
			if (muzzle_index > 0)
				bullet_nose = string.Format("bullet_nose_" + (muzzle_index + 1));
			cylinder.HideSelection(bullet_nose);
		}
	}
	
	override bool IsShowingChamberedBullet()
	{
		return false;
	}
	
	override bool CanEjectBullet() 
	{
		for(int i = 0; i < GetMuzzleCount(); i++)
		{
			if(IsChamberFull(i))
				return true;
		}
		return false;
	}
	
	override void ShowBullet(int muzzleIndex)
	{
		super.ShowBullet(muzzleIndex);
		
		KOD_Revolver50Cal_Cylinder cylinder = KOD_Revolver50Cal_Cylinder.Cast(GetAttachmentByType(KOD_Revolver50Cal_Cylinder));
		if (cylinder)
		{		
			string bullet = "bullet";
			if (muzzleIndex > 0)
				bullet = string.Format("bullet_" + (muzzleIndex + 1));
			
			cylinder.ShowSelection(bullet);
			
			if (!IsChamberFiredOut(muzzleIndex))
			{
				string bullet_nose = "bullet_nose";
				if (muzzleIndex > 0)
					bullet_nose = string.Format("bullet_nose_" + (muzzleIndex + 1));
				cylinder.ShowSelection(bullet_nose);
			}
		}
	}
	
	override void HideBullet(int muzzleIndex)
	{
		super.HideBullet(muzzleIndex);
		
		KOD_Revolver50Cal_Cylinder cylinder = KOD_Revolver50Cal_Cylinder.Cast(GetAttachmentByType(KOD_Revolver50Cal_Cylinder));
		if (cylinder)
		{		
			string bullet = "bullet";
			if (muzzleIndex > 0)
				bullet = string.Format("bullet_" + (muzzleIndex + 1));
			
			cylinder.HideSelection(bullet);
			
			string bullet_nose = "bullet_nose";
			if (muzzleIndex > 0)
				bullet_nose = string.Format("bullet_nose_" + (muzzleIndex + 1));
			cylinder.HideSelection(bullet_nose);
		}
	}
};

class KOD_Revolver50Cal extends KOD_Revolver50Cal_Base {};
class KOD_Revolver50Cal_Cylinder extends DummyItem {};
class KOD_Revolver50Cal_Ejector extends DummyItem {};