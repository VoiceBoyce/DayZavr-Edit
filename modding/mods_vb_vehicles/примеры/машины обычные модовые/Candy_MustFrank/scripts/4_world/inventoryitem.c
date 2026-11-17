class Candy_MustFrank_Wheel : CarWheel 
{
	override int GetMeleeTargetType()
	{
		return EMeleeTargetType.NONALIGNABLE;
	}
	
	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionDetach);
		AddAction(ActionAttachOnSelection);
	}
	
};


class Candy_MustFrank_Doors_Driver : CarDoor {};
class Candy_MustFrank_Doors_CoDriver :Candy_MustFrank_Doors_Driver {};
class Candy_MustFrank_Doors_Trunk :Candy_MustFrank_Doors_Driver {};
class Candy_MustFrank_Doors_Trunk0 :Inventory_Base  {};
class Candy_MustFrank_Doors_Hood :Candy_MustFrank_Doors_Driver
{};
