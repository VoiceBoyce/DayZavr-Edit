// INT_UIFramework - easing curves. The maths is vanilla (3_game/static/easing.c);
// this only maps an int id to the function so tweens can store the curve.
enum INT_UIF_Ease
{
	LINEAR,
	IN_SINE,
	OUT_SINE,
	IN_OUT_SINE,
	IN_QUAD,
	OUT_QUAD,
	IN_OUT_QUAD,
	IN_CUBIC,
	OUT_CUBIC,
	IN_OUT_CUBIC,
	IN_QUART,
	OUT_QUART,
	IN_OUT_QUART,
	IN_EXPO,
	OUT_EXPO,
	IN_OUT_EXPO,
	IN_BACK,
	OUT_BACK,
	IN_OUT_BACK,
	OUT_ELASTIC,
	OUT_BOUNCE
}

class INT_UIF_Easing
{
	// if-chain on purpose: no switch/default-return construct (enforce pitfall).
	static float Apply(int ease, float t)
	{
		t = Math.Clamp(t, 0, 1);
		if (ease == INT_UIF_Ease.IN_SINE)
			return Easing.EaseInSine(t);
		if (ease == INT_UIF_Ease.OUT_SINE)
			return Easing.EaseOutSine(t);
		if (ease == INT_UIF_Ease.IN_OUT_SINE)
			return Easing.EaseInOutSine(t);
		if (ease == INT_UIF_Ease.IN_QUAD)
			return Easing.EaseInQuad(t);
		if (ease == INT_UIF_Ease.OUT_QUAD)
			return Easing.EaseOutQuad(t);
		if (ease == INT_UIF_Ease.IN_OUT_QUAD)
			return Easing.EaseInOutQuad(t);
		if (ease == INT_UIF_Ease.IN_CUBIC)
			return Easing.EaseInCubic(t);
		if (ease == INT_UIF_Ease.OUT_CUBIC)
			return Easing.EaseOutCubic(t);
		if (ease == INT_UIF_Ease.IN_OUT_CUBIC)
			return Easing.EaseInOutCubic(t);
		if (ease == INT_UIF_Ease.IN_QUART)
			return Easing.EaseInQuart(t);
		if (ease == INT_UIF_Ease.OUT_QUART)
			return Easing.EaseOutQuart(t);
		if (ease == INT_UIF_Ease.IN_OUT_QUART)
			return Easing.EaseInOutQuart(t);
		if (ease == INT_UIF_Ease.IN_EXPO)
			return Easing.EaseInExpo(t);
		if (ease == INT_UIF_Ease.OUT_EXPO)
			return Easing.EaseOutExpo(t);
		if (ease == INT_UIF_Ease.IN_OUT_EXPO)
			return Easing.EaseInOutExpo(t);
		if (ease == INT_UIF_Ease.IN_BACK)
			return Easing.EaseInBack(t);
		if (ease == INT_UIF_Ease.OUT_BACK)
			return Easing.EaseOutBack(t);
		if (ease == INT_UIF_Ease.IN_OUT_BACK)
			return Easing.EaseInOutBack(t);
		if (ease == INT_UIF_Ease.OUT_ELASTIC)
			return Easing.EaseOutElastic(t);
		if (ease == INT_UIF_Ease.OUT_BOUNCE)
			return Easing.EaseOutBounce(t);
		return t;
	}
}
