//	АБВ
class VD_StaticFnc
{
	static float Distance2D( vector v_PointA, vector v_PointB )
	{
		return Math.Sqrt( Math.Pow( ( v_PointA[ 0 ] - v_PointB[ 0 ] ), 2 ) + Math.Pow( ( v_PointA[ 2 ] - v_PointB[ 2 ] ), 2 ) );
	}

	static bool IsPlayerNear( array<Man> v_Players, vector v_Position, float v_Distance )
	{
		foreach ( Man v_Player : v_Players )
		{
			if ( v_Player && Distance2D( v_Player.GetPosition(), v_Position ) <= v_Distance )
			{
				return true;
			}
		}
		return false;
	}
}
