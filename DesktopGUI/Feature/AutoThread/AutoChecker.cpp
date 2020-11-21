#include "AutoChecker.h"
#include "../../Frame/DictionaryFrame.h"
#include "../../TextField.h"
#include "../LogGUI.h"

void AutoChecker::Routine()
{
	auto page = TextField::GetActivePage();
	DictionaryFrame::ResetStyling( page );
	DictionaryFrame::StartStyling( page );
	LOG_CONSOLE_FORMAT( LEVEL_TRACE, "AutoChecker at Page(%d), Actions(%u)", page, this->GetTotalAction() );
}