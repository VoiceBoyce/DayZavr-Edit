// Fallback door action for door models without action selections.
// Dodge_Challenger_1970 / dodge_rust_Z door .p3d files expose no CarDoors/doors
// selection, so vanilla ActionCarDoorsOutside finds no anim source and offers
// nothing. This override keeps 100% vanilla behavior (super first) and only
// adds the missing case: resolve the animation source from the door item type.
modded class ActionCarDoorsOutside
{
	protected string GetFallbackDoorAnim(CarDoor door)
	{
		string t = door.GetType();
		t.ToLower();
		if (t.Contains("codriver")) return "DoorsCoDriver";
		if (t.Contains("driver")) return "DoorsDriver";
		if (t.Contains("cargo1")) return "DoorsCargo1";
		if (t.Contains("cargo2")) return "DoorsCargo2";
		if (t.Contains("hood")) return "DoorsHood";
		if (t.Contains("trunk")) return "DoorsTrunk";
		return "";
	}

	protected bool IsFallbackCar(CarScript car)
	{
		return car.IsKindOf("Dodge_Challenger_1970") || car.IsKindOf("dodge_rust_Z");
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		if (super.ActionCondition(player, target, item))
			return true;

		CarScript car = null;
		if (!IsInReach(player, target, UAMaxDistances.DEFAULT))
			return false;
		if (!Class.CastTo(car, target.GetParent()))
			return false;
		if (!IsFallbackCar(car))
			return false;
		CarDoor carDoor = CarDoor.Cast(target.GetObject());
		if (!carDoor)
			return false;
		string animSource = GetFallbackDoorAnim(carDoor);
		if (animSource == "")
			return false;
		int idx = car.GetSeatIndexFromDoor(animSource);
		if (idx != -1 && !car.IsAreaAtDoorFreeDiag(idx))
			return false;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_OPENDOORFW;
		float animationPhase = car.GetAnimationPhase(animSource);
		return (m_IsOpening && animationPhase <= 0.5) || (!m_IsOpening && animationPhase > 0.5);
	}

	override void OnStartServer(ActionData action_data)
	{
		super.OnStartServer(action_data);

		CarScript car = CarScript.Cast(action_data.m_Target.GetParent());
		CarDoor carDoor = null;
		if (car)
			carDoor = CarDoor.Cast(action_data.m_Target.GetObject());
		if (!car || !carDoor || !IsFallbackCar(car))
			return;
		string animSource = GetFallbackDoorAnim(carDoor);
		if (animSource == "")
			return;
		float phase;
		if (m_IsOpening)
			phase = 1.0;
		else
			phase = 0.0;
		car.SetAnimationPhase(animSource, phase);
	}
}
