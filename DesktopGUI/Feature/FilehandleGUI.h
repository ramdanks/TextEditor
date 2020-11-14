#include <wx/wxprec.h>

class FilehandleGUI
{
public:
	FilehandleGUI() = delete;
	FilehandleGUI( const FilehandleGUI& other ) = delete;

	static void LoadSupportedFormat();
	static std::string OpenDialog( wxWindow* parent, const wxString& defaultFilename );
	static std::string SaveDialog( wxWindow* parent, const wxString& defaultFilename );
	static wxArrayString GetDroppedFiles( wxString* dropped, uint32_t size );

	static std::vector<uint8_t> OpenFileFormat( const std::string& filepath );
	static void SaveFileFormat( const std::string& filepath, const char* pd, size_t size );

private:
	static std::string FileDialog( wxWindow* parent, const wxString& message, const wxString& defaultFilename, int style );

	static wxString mSupportedFormat;
};