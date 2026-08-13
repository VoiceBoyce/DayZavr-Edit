modded class ActionSkinning: ActionContinuousBase
{
    float Spider_Spawn_radius = 15;
    int Spider_Spawn_Count = 6;
    override void OnFinishProgressServer(ActionData action_data)
    {
        super.OnFinishProgressServer(action_data);
        Object targetObject = action_data.m_Target.GetObject();

        if (targetObject && targetObject.IsKindOf("MSFZ_Mutant_Spider3"))
        {
            vector Spider_death_place = targetObject.GetPosition();
            for (int l = 0; l < Spider_Spawn_Count; l++)
                {
                    vector Spider_spawnPosAll = MSFZ_GetRandomSpawnPosition(Spider_death_place, Spider_Spawn_radius);
                    GetGame().CreateObject("MSFZ_Mutant_Spider1", Spider_spawnPosAll, false, true);
                }
        }
    }
    vector MSFZ_GetRandomSpawnPosition(vector Spider_death_place, float radSpider_Spawn_radiusius)
    {
        float x = Spider_death_place[0] + Math.RandomFloat(-Spider_Spawn_radius, Spider_Spawn_radius);
        float z = Spider_death_place[2] + Math.RandomFloat(-Spider_Spawn_radius, Spider_Spawn_radius);
        float y = GetGame().SurfaceY(x, z);
        return Vector(x, y, z);
    }
}
class MSFZ_Mutant_Spider_Base extends Animal_CanisLupus{};
class MSFZ_Mutant_Spider1 extends MSFZ_Mutant_Spider_Base{};
class MSFZ_Mutant_Spider2 extends MSFZ_Mutant_Spider_Base{};
class MSFZ_Mutant_Spider3 extends MSFZ_Mutant_Spider_Base{};
modded class MSFZ_Mutant_Spider1
{
    void MSFZ_Mutant_Spider1()
    {
        SetEventMask(EntityEvent.POSTFRAME);
    }
    override void EOnPostFrame(IEntity other, int extra)
    {
        float scale = 0.3; // Коэффициент увеличения изначально 1
        vector transform[4];
        GetTransform(transform);
       
        transform[0] = transform[0] * scale;
        transform[1] = transform[1] * scale;
        transform[2] = transform[2] * scale;
       
        SetTransform(transform);
        Update();
    }
    override void EOnFrame(IEntity other, float timeSlice)
    {
        vector rotation = GetYawPitchRoll();
        SetYawPitchRoll(Vector(rotation[0], 0, 0));
    }
};
modded class MSFZ_Mutant_Spider2
{
    void MSFZ_Mutant_Spider2()
    {
        SetEventMask(EntityEvent.POSTFRAME);
    }
    override void EOnPostFrame(IEntity other, int extra)
    {
        float scale = 0.6; // Коэффициент увеличения изначально 1
        vector transform[4];
        GetTransform(transform);
       
        transform[0] = transform[0] * scale;
        transform[1] = transform[1] * scale;
        transform[2] = transform[2] * scale;
       
        SetTransform(transform);
        Update();
    }
    override void EOnFrame(IEntity other, float timeSlice)
    {
        vector rotation = GetYawPitchRoll();
        SetYawPitchRoll(Vector(rotation[0], 0, 0));
    }
};
modded class MSFZ_Mutant_Spider3
{
    void MSFZ_Mutant_Spider3()
    {
        SetEventMask(EntityEvent.POSTFRAME);
    }
    override void EOnPostFrame(IEntity other, int extra)
    {
        float scale = 0.9; // Коэффициент увеличения изначально 1
        vector transform[4];
        GetTransform(transform);
       
        transform[0] = transform[0] * scale;
        transform[1] = transform[1] * scale;
        transform[2] = transform[2] * scale;
       
        SetTransform(transform);
        Update();
    }
    override void EOnFrame(IEntity other, float timeSlice)
    {
        vector rotation = GetYawPitchRoll();
        SetYawPitchRoll(Vector(rotation[0], 0, 0));
    }
};