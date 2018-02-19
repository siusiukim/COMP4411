#include <FL/fl_ask.h>

#include <math.h>

#include "impressionistUI.h"
#include "impressionistDoc.h"

#include "MathUtil.hpp"

void ImpressionistUI::autoDrawAction(Fl_Widget* o, void*) {
	ImpressionistUI *pUI = ((ImpressionistUI*)(o->user_data()));
	ImpressionistDoc *pDoc = pUI->getDocument();

	int spacing = pUI->getSpacing();
}
