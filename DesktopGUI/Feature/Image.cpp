#include "Image.h"
#include "LogGUI.h"
#include "Config.h"

std::vector<wxBitmap> Image::mImageVector;

void Image::FetchData()
{
    PROFILE_FUNC();

    AddImg( Config::sAppPath + IMG_NULL_FILEPATH );
    AddImg( Config::sAppPath + IMG_SPLASH_FILEPATH );
    AddImg( Config::sAppPath + IMG_SAVE_FILEPATH );
    AddImg( Config::sAppPath + IMG_UNSAVE_FILEPATH );
}

wxBitmap& Image::GetImg( size_t index )
{
    if ( index >= mImageVector.size() ) return mImageVector[0];
    return mImageVector[index];
}

size_t Image::AddImg( const wxString& filepath )
{
    wxBitmap bitmap;
    if ( !bitmap.LoadFile( filepath, wxBITMAP_TYPE_ANY ) )
    {
        LOG_ALL( LV_ERROR, "Image cannot be Loaded: " + std::string( filepath ) );
    }
    mImageVector.push_back( bitmap );
    return mImageVector.size() - 1;
}