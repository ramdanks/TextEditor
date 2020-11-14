#pragma once
#include <wx/dnd.h>

class DragDrop : public wxDropSource
{
public:
	DragDrop( wxWindow* parent );
	bool GiveFeedback( wxDragResult effect ) override;

private:
	wxWindow* mParent;
};

