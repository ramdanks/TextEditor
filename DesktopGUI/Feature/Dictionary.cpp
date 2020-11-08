#include "Dictionary.h"
#include "../../Utilities/Filestream.h"
#include "../../Utilities/Timer.h"

std::unordered_set<std::string> Dictionary::ParseWord( const std::string& str )
{
	return std::unordered_set<std::string>();
}

Dictionary::Dictionary()
{
}

Dictionary::Dictionary( const std::string& filepath )
{
	Load( filepath );
}

bool Dictionary::Load( const std::string& filepath )
{
	//Util::Timer Tm( "Load CSV", ADJUST, true );

	auto vRead = Filestream::Read_Bin( filepath );
	if ( vRead.empty() ) return false;

	std::vector<std::string> line;
	line = Filestream::ParseString( std::string( (const char*) &vRead[0], vRead.size() ), '\n' );
	if ( line.empty() || line.size() < 2 ) return false;
	vRead.clear();

	// clear CR-LF format used by DOS and Windows
	if ( line[0].back() == '\r' )
		for ( auto& l : line ) l.pop_back();
	
	// remove the identifier
	auto vSetting = Filestream::ParseString( line[1], ',' );
	mTitle = vSetting[0];
	//mColour = vSetting[1];

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