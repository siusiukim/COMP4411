#pragma once

template <class TA, class TB>
bool is_in_range(TA a, TB lower_bound, TB upper_bound) {
	return a >= lower_bound && a <= upper_bound;
}

template <class T>
T cap_range(T a, T lower_bound, T upper_bound) {
	if (a < lower_bound) return lower_bound;
	if (a > upper_bound) return upper_bound;
	return a;
}
