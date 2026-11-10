modded class Weapon_Base

{

	override void OnRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
	{
		super.OnRPC(sender, rpc_type, ctx);

	
		if(!GetGame().IsDedicatedServer())
		{
			if(rpc_type == 8832135611)
			{		
				int mi = GetCurrentMuzzle();
				EEFired(mi, GetCurrentMode(mi), string.Empty);
    			WeaponForceFireSound();
			}
			if(rpc_type == 8832135612)
			{		
				
				ref Param1<vector> bulletpos = new Param1<vector>(vector.Zero);
				
				if (ctx.Read(bulletpos))
				{
					vector position = bulletpos.param1;
					//Print("bullet pos " + position);
					BanditBulletSound(position);
				}
    			
			}
		}
	}


	void WeaponForceFireSound()
	{
		ref BanditWeaponFireSound ssss = new BanditWeaponFireSound(GetType(), GetPosition());
	}
	
	void BanditBulletSound(vector position)
	{
		ref BanditBulletFlybyFireSound ssss = new BanditBulletFlybyFireSound(position);
	}

}

class BanditWeaponFireSound

{
	protected vector m_Position;

	void BanditWeaponFireSound(string wpn_type, vector pos)
	{

		m_Position = pos;

		

		auto soundSets = new array<string>;

		string semiAutoSoundPath = string.Format("CfgWeapons %1 SemiAuto soundSetShot", wpn_type);

		string singleSoundPath = string.Format("CfgWeapons %1 Single soundSetShot", wpn_type);

		string silencedSoundPath = string.Format("CfgWeapons %1 Single soundSetShot", wpn_type);

		

		if(GetGame().ConfigIsExisting(singleSoundPath))

			GetGame().ConfigGetTextArray(singleSoundPath, soundSets);

		else

			GetGame().ConfigGetTextArray(semiAutoSoundPath, soundSets);

		
		foreach(string soundSet : soundSets)
		{
			MakeSound(soundSet);
		}
	}

	

	void MakeSound(string sound_set)
	{

		SoundParams soundParams = new SoundParams(sound_set);
		if(soundParams.IsValid())
		{
			SoundObjectBuilder builder = new SoundObjectBuilder(soundParams);

			builder.AddVariable("forest", 1);
			builder.AddVariable("houses", 1);
			builder.AddVariable("meadows", 1);
			builder.AddVariable("sea", 1);
			builder.AddVariable("trees", 1);		

			if(GetGame().GetPlayer())

				builder.AddVariable("interior", GetGame().GetPlayer().IsSoundInsideBuilding()); //zmenit na infected (getparent?)
				

			SoundObject soundObject = builder.BuildSoundObject();



			if(soundObject)

			{

				AttenuateSoundIfNecessary(soundObject);

				PlaySound(soundObject, builder);

			}

		}

	}

	

	protected void AttenuateSoundIfNecessary(SoundObject soundObject)

	{

		Man pl = GetGame().GetPlayer();

		

		if(pl)

		{

			/*IsSoundInsideBuilding() != GetGame().GetPlayer().IsSoundInsideBuilding() || */

			if(pl.IsCameraInsideVehicle())

				soundObject.SetKind(WaveKind.WAVEATTALWAYS);

		}

		else

		{

			soundObject.SetKind(WaveKind.WAVEEFFECTEX);

		}

	}

	

	AbstractWave PlaySound(SoundObject so, SoundObjectBuilder sob)
	{
		if(so)
		{
			so.SetPosition(m_Position);
			AbstractWave wave = GetGame().GetSoundScene().Play3D(so, sob);
			return wave;
		}
		else
		{
			return null;
		}
	}		
}

//SuperSonicCrack_SoundSet
//bulletFlyBy_SoundSet

class BanditBulletFlybyFireSound

{
	protected vector m_Position;

	void BanditBulletFlybyFireSound(vector pos)
	{
		m_Position = pos;	
		MakeSound("bulletFlyBy_SoundSet");
		MakeSound("SuperSonicCrack_SoundSet");
	}

	

	void MakeSound(string sound_set)
	{

		SoundParams soundParams = new SoundParams(sound_set);
		if(soundParams.IsValid())
		{
			SoundObjectBuilder builder = new SoundObjectBuilder(soundParams);
			/*
			builder.AddVariable("forest", 1);
			builder.AddVariable("houses", 1);
			builder.AddVariable("meadows", 1);
			builder.AddVariable("sea", 1);
			builder.AddVariable("trees", 1); */
			if (sound_set == "SuperSonicCrack_SoundSet")
			{
				builder.AddVariable("Close", 1);		
			}
			if(GetGame().GetPlayer() && sound_set == "SuperSonicCrack_SoundSet")
			{
				builder.AddVariable("interior", GetGame().GetPlayer().IsSoundInsideBuilding()); 
			}
			SoundObject soundObject = builder.BuildSoundObject();


			if(soundObject)

			{

				AttenuateSoundIfNecessary(soundObject);

				PlaySound(soundObject, builder);

			}

		}

	}

	

	protected void AttenuateSoundIfNecessary(SoundObject soundObject)

	{

		Man pl = GetGame().GetPlayer();

		

		if(pl)

		{

			/*IsSoundInsideBuilding() != GetGame().GetPlayer().IsSoundInsideBuilding() || */

			if(pl.IsCameraInsideVehicle())

				soundObject.SetKind(WaveKind.WAVEATTALWAYS);

		}

		else

		{

			soundObject.SetKind(WaveKind.WAVEEFFECTEX);

		}

	}

	

	AbstractWave PlaySound(SoundObject so, SoundObjectBuilder sob)
	{
		if(so)
		{
			so.SetPosition(m_Position);
			AbstractWave wave = GetGame().GetSoundScene().Play3D(so, sob);
			return wave;
		}
		else
		{
			return null;
		}
	}		
}