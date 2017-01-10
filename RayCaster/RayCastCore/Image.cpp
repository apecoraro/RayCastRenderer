#include "Image.h"

#include <windows.h>
#include <gdiplus.h>

using namespace Gdiplus;

cs500::Image::Image(int horizRes, int vertRes) :
    _pBitmap(new Bitmap((int)horizRes, (int)vertRes, PixelFormat24bppRGB))
{
}

cs500::Image::~Image()
{
    if(_pBitmap != NULL)
        delete _pBitmap;
}

void cs500::Image::setPixel(int row, int col,
                            unsigned char r,
                            unsigned char g,
                            unsigned char b)
{
    Color gdiColor(r, g, b);
    _pBitmap->SetPixel(col, row, gdiColor);
}

int cs500::Image::horizontalResolution() const
{
    return (int)_pBitmap->GetWidth();
}

int cs500::Image::verticalResolution() const
{
    return (int)_pBitmap->GetHeight();
}

int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
   UINT  num = 0;          // number of image encoders
   UINT  size = 0;         // size of the image encoder array in bytes

   Gdiplus::ImageCodecInfo* pImageCodecInfo = NULL;

   Gdiplus::GetImageEncodersSize(&num, &size);
   if(size == 0)
      return -1;  // Failure

   pImageCodecInfo = (Gdiplus::ImageCodecInfo*)(malloc(size));
   if(pImageCodecInfo == NULL)
      return -1;  // Failure

   Gdiplus::GetImageEncoders(num, size, pImageCodecInfo);

   for(UINT j = 0; j < num; ++j)
   {
      if( wcscmp(pImageCodecInfo[j].MimeType, format) == 0 )
      {
         *pClsid = pImageCodecInfo[j].Clsid;
         free(pImageCodecInfo);
         return j;  // Success
      }    
   }

   free(pImageCodecInfo);
   return -1;  // Failure
}

void cs500::Image::saveBitmap(const std::string& outputFile)
{
    CLSID bmpClsid;
    GetEncoderClsid(L"image/bmp", &bmpClsid);
    
    std::string out = outputFile;
    out +=".bmp";

    // Convert to a wchar_t*
    size_t convertedChars = 0;
    wchar_t *wcstring = (wchar_t*)malloc(sizeof(wchar_t) * (out.length() + 1));
    mbstowcs_s(&convertedChars, wcstring, out.length()+1, out.c_str(), out.length());

    _pBitmap->Save(wcstring, &bmpClsid);

    free(wcstring);
}