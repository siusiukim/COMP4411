#pragma once

template <class TA, class TB>
bool is_in_range(TA a, TB lower_bound, TB upper_bound) {
	return a >= lower_bound && a <= upper_bound;
}
