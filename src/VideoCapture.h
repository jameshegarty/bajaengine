// VFWImageProcessor.h: interface for the CVFWCapture class.
//
//////////////////////////////////////////////////////////////////////

#if !defined VFWIMAGEPROCESSOR_H
#define VFWIMAGEPROCESSOR_H

#ifdef _WIN32
	#include <windows.h>
	#include <vfw.h>
	#endif
#include <string.h>

#define MAX_VFW_DEVICES		10
#define ERROR_SIZE			100

class CVFWCapture{
public:
   CVFWCapture();
   virtual ~CVFWCapture();
   CVFWCapture(const CVFWCapture &CopyFrom);
   CVFWCapture &operator =(const CVFWCapture &CopyFrom);

   bool Initialize(short DriverIndex = 0);
   void Destroy();

   bool SetDriver(short DriverIndex);

	#ifdef _WIN32
   HWND GetCapWindow();
   bool CaptureDIB(PBITMAPINFO *Bitmap, ULONG BitmapLength, ULONG *RetBitmapLength);                            
   bool EnablePreviewVideo(HWND Parent, INT x, INT y, INT PreviewRate = 30);
   BOOL EnablePreviewVideo(HWND Parent, INT x, INT y, INT Width, INT Height, INT PreviewRate = 30);
   BOOL DisablePreviewVideo();
   BOOL DriverGetCaps(CAPDRIVERCAPS *Caps);
	bool AllocDIBImage(PBITMAPINFO *ppImageData, ULONG *AllocatedSize);
	
	BITMAPINFOHEADER GetBitmapInfoHeader();

   void GetPreviousError(int *ErrorID, char *ErrorString, bool ResetError = false);

   static ULONG CalcBitmapSize(const BITMAPINFOHEADER &bmiHeader);
   static ULONG CalcBitmapInfoSize(const BITMAPINFOHEADER &bmiHeader);

   friend LRESULT CALLBACK ErrorCallbackProc(HWND hWnd, int nErrID, LPSTR lpErrorText);
   friend LRESULT CALLBACK StatusCallbackProc(HWND hWnd, int nID, LPCSTR lpsz);
   friend LRESULT CALLBACK FrameCallbackProc(HWND hWnd, LPVIDEOHDR lpVHdr);
	#endif
	
   
   void CancelCapture();
   

   

private:    // Data
   #ifdef _WIN32
   HWND m_hWndVideo;
   BITMAPINFOHEADER m_BitmapInfoHeader;      // Used to store image dimensions.
   PBITMAPINFO m_TransferBitmapInfo;
   ULONG m_TransferBitmapInfoSize;
   INT m_DriverIndex;
   INT m_ErrorID;
   char m_ErrorText[ERROR_SIZE];


   static UINT m_ValidDriverIndex[MAX_VFW_DEVICES];
   static USHORT m_TotalVideoDrivers;
	#endif
	
private:    // Functions
   #ifdef _WIN32
   CVFWCapture &Copy(const CVFWCapture &CopyFrom);
	#endif
};

#endif // !defined VFWIMAGEPROCESSOR_H

