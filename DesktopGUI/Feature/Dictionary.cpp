#include "Dictionary.h"
#include "../Feature/LogGUI.h"

Dictionary::Dictionary()
{
}

Dictionary::Dictionary( const std::string& filepath )
{
	Load( filepath );
}

std::unordered_map<std::string, std::string> Dictionary::GetContent() const
{
	return mContent;
}

bool Dictionary::Load( const std::string& filepath )
{
	if ( Filestream::FileExtension( filepath ) != "csv" ) return false;

	auto vRead = Filestream::Read_Bin( filepath );
	if ( vRead.empty() ) return false;
	auto readsize = vRead.size();

	std::vector<std::string> line;
	line = Filestream::ParseNewline( std::string( (char*) &vRead[0], vRead.size() ) );
	if ( line.empty() || line.size() < 2 ) return false;
	vRead.clear();

	// remove the identifier
	auto vSetting = Filestream::ParseString( line[1], ',' );
	if ( vSetting.empty() ) return false;
	mTitle = vSetting[0];
	mColour = vSetting[1];

	// get and insert the content
	for ( uint32_t i = 3; i < line.size(); i++  )
	{
		// we use csv format (comma as delimiter)
		auto vContent = Filestream::ParseString( line[i], ',', 2 );
		if ( vContent.empty() ) break;
		mContent.insert( { vContent[0], vContent[1] } );
	}
	return true;
}

void Dictionary::SetTitle( const std::string& title )
{
	mTitle = title;
}

std::string Dictionary::GetTitle() const
{
	return mTitle;
}

std::string Dictionary::GetColour() const
{
	return mColour;
}

void Dictionary::Insert( const sContent& content )
{
	//Util::Timer Tm( "Insert Unordered Map", ADJUST, true );
	mContent[content.Word] = content.Description;
}

void Dictionary::Remove( const std::string& word )
{
	//Util::Timer Tm( "Erase Unordered Map", ADJUST, true );
	mContent.erase( word );
}

bool Dictionary::Exist( const std::string& word )
{
	//Util::Timer Tm( "Lookup Unordered Map", ADJUST, true );
	return mContent.find( word ) != mContent.end();
}

void Dictionary::Clear()
{
	mContent.clear();
}

size_t Dictionary::Size() const
{
	return mContent.size();
}