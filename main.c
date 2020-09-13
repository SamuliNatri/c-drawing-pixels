#include <windows.h>
#include <stdint.h>

#define i32 uint32_t

int globalRunning = 1;
int width = 0;
int height = 0;

void* memory;
BITMAPINFO bitmap_info;

LRESULT CALLBACK 
WindowProc(HWND window, 
           UINT message, 
           WPARAM wParam, 
           LPARAM lParam)
{
    LRESULT result;
    switch(message)
    {
        case WM_CLOSE:
        {
            globalRunning = 0;
        } break;
        
        default:
        {
            result = DefWindowProc(window, message, wParam, lParam);
        } break;
    }
    
    return result;
}

int WINAPI 
wWinMain(HINSTANCE instance, 
         HINSTANCE prevInstance, 
         PWSTR cmdLine, 
         int cmdShow)
{
    WNDCLASS window_class = {0};
    
    const wchar_t CLASS_NAME[] = L"GameWindowClass";
    
    window_class.lpfnWndProc = WindowProc;
    window_class.hInstance = instance;
    window_class.lpszClassName = CLASS_NAME;
    
    RegisterClass(&window_class);
    
    HWND window = CreateWindowEx(0,
                                 CLASS_NAME,
                                 L"Game",
                                 WS_OVERLAPPEDWINDOW|WS_VISIBLE,
                                 CW_USEDEFAULT,
                                 CW_USEDEFAULT,
                                 CW_USEDEFAULT,
                                 CW_USEDEFAULT,
                                 0,
                                 0,
                                 instance,
                                 0);
    
    RECT rect;
    GetClientRect(window, &rect);
    int width = rect.right - rect.left;
    int height = rect.bottom - rect.top;
    
    memory = VirtualAlloc(0,
                          width * height * 4,
                          MEM_RESERVE|MEM_COMMIT,
                          PAGE_READWRITE
                          );
    
    i32* pixel = (i32 *)memory; 
    
    for(int pixel_number = 0;
        pixel_number < width * height;
        ++pixel_number)
    {
        *pixel++ = 0xFF0000;
    }
    
    i32 red = 0xFF0000;
    i32 green = 0x00FF00;
    i32 blue = 0x0000FF;
    
    /*
    *pixel = 0xFF0000;
    ++pixel;
    *pixel = 0x0000FF;
    */
    bitmap_info.bmiHeader.biSize = sizeof(bitmap_info.bmiHeader);
    bitmap_info.bmiHeader.biWidth = width;
    bitmap_info.bmiHeader.biHeight = height;
    bitmap_info.bmiHeader.biPlanes = 1;
    bitmap_info.bmiHeader.biBitCount = 32;
    bitmap_info.bmiHeader.biCompression = BI_RGB;
    
    HDC hdc = GetDC(window);
    
    while(globalRunning)
    {
        MSG message;
        while(PeekMessage(&message, window, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&message);
            DispatchMessage(&message);
        }
        StretchDIBits(hdc,
                      0,
                      0,
                      width,
                      height,
                      0,
                      0,
                      width,
                      height,
                      memory,
                      &bitmap_info,
                      DIB_RGB_COLORS,
                      SRCCOPY
                      );
    }
    
    return 0;
}
