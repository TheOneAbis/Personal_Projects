#pragma once

float Clamp(float num, float min, float max)
{
	if (num < min) return min;
	if (num > max) return max;
	return num;
}