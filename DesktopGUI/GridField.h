#pragma once
#include "Feature/Dictionary.h"

struct sGridData
{
	bool isChanged;
	bool isTemporary;
	std::string FilePath;
};

class GridField : public wxFrame
{
public:
	GridField( wxWindow* parent );
	void OnNewDict();
	void OnOpenDict();
	void OnSaveDict();
	void OnClose( wxCloseEvent& event );

private:
	void AddNewGrid();
	void UpdateParentName();
	wxDECLARE_EVENT_TABLE();

private:
	wxWindow* mParent;
	wxNotebook* mNotebook;
	wxGrid* mGrid;

	uint32_t mIndex;
	std::vector<sGridData> mGridData;
};