
class SFP_Rifle_Case_woden_Base01 : SFP_OpenableCase_Base
{
    // [НАСТРАИВАЕТСЯ] Какая крышка у деревянного кейса
    override protected string GetLidClassName()
    {
        return "SFP_Rifle_Case_woden_lid";
    }

    // [НАСТРАИВАЕТСЯ] Привязка лут-листа к другому кейсу
    override protected string GetLootConfigKey()
    {
        return "SFP_Rifle_Case_Base01";
    }
};