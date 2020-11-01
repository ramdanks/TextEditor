#pragma once
#include <wx/wxprec.h>

class Image
{
public:
	static void Init();
	static wxBitmap& GetImg( size_t index );
	static size_t AddImg( const std::string& filepath );

private:
	static std::vector<wxBitmap> mImageVector;
};

#define IMG_NULL      Image::GetImg(0)
#define IMG_SPLASH    Image::GetImg(1)
#define IMG_SAVE      Image::GetImg(2)
#define IMG_UNSAVE    Image::GetImg(3)

#define IMG_NULL_FILEPATH      "resource/img/gallery.bmp"
#define IMG_SPLASH_FILEPATH    "resource/splash.bmp"
#define IMG_SAVE_FILEPATH      "resource/img/save.bmp"
#define IMG_UNSAVE_FILEPATH    "resource/img/unsave.bmp"