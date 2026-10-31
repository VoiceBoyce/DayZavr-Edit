// Отключение урона авто
modded class CarScript extends Car {
	override void OnContact( string zoneName, vector localPos, IEntity other, Contact data )
	{
	}
}