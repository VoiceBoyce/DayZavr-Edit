modded class ActionSkinningCB
{
    override void CreateActionComponent()
    {
        float time = UATimeSpent.SKIN;

        if (m_ActionData.m_MainItem && m_ActionData.m_MainItem.ConfigIsExisting("SkinningDurationSpeed"))
            time = m_ActionData.m_MainItem.ConfigGetFloat("SkinningDurationSpeed");

        m_ActionData.m_ActionComponent = new CAContinuousTime( time );
    }
};