class RF_ESCAPE_FROM_DAYZ_AirstrikeDTO: Managed {
    
    // Позиция игрока по которому мы ебашим аирстрайком
    vector position;
    
    // Время отложенного запуска этой залупы
    int delayTime;

    // Задержка проигрывания финальной стадции взрыва
    float finalExplosionDelay;

    // Время полета снаряда в воздухе
    int midAirDetonationSoundLength;
}

class RF_ESCAPE_FROM_DAYZ_CL_AirstrikeService: Managed {

    // MARK: - Private Properties
    
	protected ShellLight explosionLight;
	protected EffectSound artillerySound;

    private float finalExplosionDelay;
    private int midAirDetonationSoundLength;

    // MARK: - Init

    void ConfigureWith(RF_ESCAPE_FROM_DAYZ_AirstrikeDTO config) {
        this.finalExplosionDelay = config.finalExplosionDelay;
        this.midAirDetonationSoundLength = config.midAirDetonationSoundLength;
    }

    // MARK: - Public Methods

    void StartAirstrikeDelayed(RF_ESCAPE_FROM_DAYZ_AirstrikeDTO config) {
        vector position = config.position;
        int delayTime = config.delayTime;
        if (delayTime <= 15) {
            delayTime = 15;
        }
        position[1] = position[1] + 50;

        this.playArtillerySound();
        RF_ESCAPE_FROM_DAYZ_Log().Info(string.Format("[AIRSTRIKE SERVICE]: Init airstrike delayed: %1, %2", delayTime, position.ToString(false)));
        GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(StartAirstrike, delayTime * 1000, false, position);
    }

    void StartAirstrike(vector position) {
        this.PlayMidAirDetonationSound(position);
        GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(PlayFinalExposion, (this.finalExplosionDelay + this.midAirDetonationSoundLength) * 1000, false, position);
    }

    void PlayMidAirDetonationSound(vector position) {
		RF_ESCAPE_FROM_DAYZ_Log().Info("[AIRSTRIKE SERVICE]: Play mid air sound.");
		EffectSound artilleryFallSound = SEffectManager.PlaySound("Artillery_Fall_SoundSet", position);
		artilleryFallSound.SetAutodestroy(true);
	}

	void PlayFinalExposion(vector position) {
		this.playExplosionSound(position);
        this.playExplosionParticles(position);
        this.playExplosionLight(position);
		this.shakeCameraDuringExplosion(position);
	}

    // MARK: - Private Methods
	
	private void playExplosionSound(vector position) {
		RF_ESCAPE_FROM_DAYZ_Log().Info("[AIRSTRIKE SERVICE]: Play final explosion");
		EffectSound closeArtySound = SEffectManager.PlaySound("Artillery_Close_SoundSet", position);
		closeArtySound.SetAutodestroy(true);
	}

	private void shakeCameraDuringExplosion(vector position) {
		RF_ESCAPE_FROM_DAYZ_Log().Info("[AIRSTRIKE SERVICE]: Shake Camera due to explosion");
        if (!GetGame() || !GetGame().GetPlayer()) return;
        
		float distanceToPlayer = vector.DistanceSq(position, GetGame().GetPlayer().GetPosition());
		if (distanceToPlayer > GameConstants.CAMERA_SHAKE_ARTILLERY_DISTANCE2) return;

		float strengthFactor = Math.InverseLerp(GameConstants.CAMERA_SHAKE_ARTILLERY_DISTANCE, 0, Math.Sqrt(distanceToPlayer));
		DayZPlayerCamera camera = GetGame().GetPlayer().GetCurrentCamera();
		if (!camera) return;
		
		camera.SpawnCameraShake(strengthFactor * 4);
	}

    private void playExplosionParticles(vector position) {
        ParticleManager.GetInstance().PlayInWorld(ParticleList.CONTAMINATED_AREA_GAS_SHELL, position);
    }
	
	// Нужно на клиенте
	private void playExplosionLight(vector position) {
		RF_ESCAPE_FROM_DAYZ_Log().Info("[AIRSTRIKE SERVICE]: Play explosion light");
		explosionLight = ShellLight.Cast(ScriptedLightBase.CreateLight(ShellLight, position));
	}

    private void playArtillerySound() {
        artillerySound = SEffectManager.PlaySound("Artillery_Distant_Barrage_SoundSet", Vector(3535, 508, 11814), 0.1, 0.1);
		artillerySound.SetAutodestroy(true);
    }
}