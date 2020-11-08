#include "GridField.h"
#include "Feature/Language.h"
#include "Feature/LogGUI.h"
#include "Feature/Image.h"
#include "Feature/Config.h"

wxBEGIN_EVENT_TABLE( GridField, wxFrame )
EVT_CLOSE( OnClose )
wxEND_EVENT_TABLE()

GridField::GridField( wxWindow* parent )
	: wxFrame(parent, wxID_ANY, DICT_NAME )
{
	mParent = parent;
	mNotebook = new wxNotebook( this, wxID_ANY, wxPoint( -1, -1 ), wxSize( -1, -1 ), wxNB_BOTTOM );
}

void GridField::OnNewDict()
{
	AddNewGrid();
}

void GridField::OnOpenDict()
{
}

void GridField::OnSaveDict()
{
}

void GridField::OnClose( wxCloseEvent& event )
{
	this->Show( false );
}

void GridField::AddNewGrid()
{
	auto grid = new wxGrid( mNotebook, wxID_ANY );
	grid->CreateGrid( 50, 3 );
	mNotebook->AddPage( grid, "Grid" );
}

void GridField::UpdateParentName()
{
}
