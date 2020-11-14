#include "DragDrop.h"
#include "Feature/LogGUI.h"

DragDrop::DragDrop( wxWindow* parent )
    : wxDropSource( parent )
{
    mParent = parent;
}

bool DragDrop::GiveFeedback( wxDragResult effect )
{
    switch ( effect )
    {
    case wxDragMove:
        LOG_CONSOLE( LEVEL_INFO, "Move" );
        return true;
    case wxDragNone:

        return true;

    case wxDragCopy:

        return true;
    default:
        return false;
    }
}
