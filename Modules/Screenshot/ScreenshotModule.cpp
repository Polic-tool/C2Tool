#include "ScreenshotModule.h"
#include <windows.h>

ScreenshotModule::ScreenshotModule() {}

ScreenshotModule::~ScreenshotModule() {}

ModuleInfo ScreenshotModule::GetInfo() const {
    ModuleInfo info;
    info.name = "Screenshot";
    info.version = "1.0.0";
    info.description = "Capture screen screenshots";
    info.type = ModuleType::SCREENSHOT;
    info.interfaceVersion = MODULE_INTERFACE_VERSION;
    return info;
}

bool ScreenshotModule::Initialize() {
    return true;
}

ModuleResult ScreenshotModule::Execute(const std::vector<uint8_t>& params) {
    ModuleResult result;
    result.success = false;

    try {
        result.data = CaptureScreen();
        result.success = !result.data.empty();
    }
    catch (const std::exception& e) {
        result.errorMessage = e.what();
    }

    return result;
}

void ScreenshotModule::Cleanup() {}

std::vector<uint8_t> ScreenshotModule::CaptureScreen() {
    
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    
    HDC hdcScreen = GetDC(NULL);
    HDC hdcMem = CreateCompatibleDC(hdcScreen);

    
    HBITMAP hBitmap = CreateCompatibleBitmap(hdcScreen, screenWidth, screenHeight);
    SelectObject(hdcMem, hBitmap);

    
    BitBlt(hdcMem, 0, 0, screenWidth, screenHeight, hdcScreen, 0, 0, SRCCOPY);

    
    BITMAPINFOHEADER bi;
    bi.biSize = sizeof(BITMAPINFOHEADER);
    bi.biWidth = screenWidth;
    bi.biHeight = -screenHeight; 
    bi.biPlanes = 1;
    bi.biBitCount = 24;
    bi.biCompression = BI_RGB;
    bi.biSizeImage = 0;
    bi.biXPelsPerMeter = 0;
    bi.biYPelsPerMeter = 0;
    bi.biClrUsed = 0;
    bi.biClrImportant = 0;

    DWORD dwBmpSize = ((screenWidth * bi.biBitCount + 31) / 32) * 4 * screenHeight;
    std::vector<uint8_t> bmpData(dwBmpSize);

    GetDIBits(hdcScreen, hBitmap, 0, screenHeight, bmpData.data(), (BITMAPINFO*)&bi, DIB_RGB_COLORS);

    
    DeleteObject(hBitmap);
    DeleteDC(hdcMem);
    ReleaseDC(NULL, hdcScreen);

    return bmpData;
}

EXPORT_MODULE(ScreenshotModule)
