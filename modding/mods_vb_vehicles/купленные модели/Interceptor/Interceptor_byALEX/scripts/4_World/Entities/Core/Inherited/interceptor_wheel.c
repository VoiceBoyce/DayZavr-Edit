class Interceptor_Wheel extends CarWheel
{
	override void EEKilled(Object killer)
	{
		string newWheel = "";
		switch( GetType() )
		{
			case "Interceptor_Wheel":
				newWheel = "Interceptor_Wheel_destroyed";
			break;
		}
		if ( newWheel != "" )
		{
			ReplaceWheelLambda lambda = new ReplaceWheelLambda ( this, newWheel, NULL);
			lambda.SetTransferParams(true, true, true);
			GetInventory().ReplaceItemWithNew(InventoryMode.SERVER, lambda);
		}
	}

	override int GetMeleeTargetType()
	{
		return EMeleeTargetType.NONALIGNABLE;
	}
	
}

class Interceptor_Wheel_destroyed extends CarWheel {};

class Interceptor_Doors_Driver extends CarDoor {};
class Interceptor_Doors_coDriver extends CarDoor {};
class Interceptor_Doors_hood extends CarDoor {};
