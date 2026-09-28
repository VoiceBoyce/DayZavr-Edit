/*
modded class DayZPlayerCameraBase
{
    override void SetNVPostprocess(int NVtype)
    {
        // Vanilla binoculars overlay is suppressed via modelOptics="" in each FOG NVG config.
        // No script-level occluder manipulation needed.

        // TODO: uncomment when ready to apply custom FOG PPE (green tint, film grain, etc.)
        if (NVtype == NVTypes.NV_GOGGLES && FOG_IsActiveFogNvgOnClient())
        {
            PPERequesterBank.GetRequester(PPERequesterBank.REQ_CAMERANV).Stop();
            PPERequesterBank.GetRequester(PPERequesterBank.REQ_CAMERANV).Start(new Param1<int>(FOG_NV_CLEAN_GREEN));
            return;
        }

        super.SetNVPostprocess(NVtype);
    }
}
*/
