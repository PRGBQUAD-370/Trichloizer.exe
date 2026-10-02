#include "Trichloizer.h"

typedef union _RGBQUAD {
    COLORREF rgb;
    struct {
        BYTE r;
        BYTE g;
        BYTE b;
        BYTE Reserved;
    };
} _RGBQUAD, * PRGBQUAD;
int red = 0, green = 0, blue = 0;
bool ifblue = false;

COLORREF Hue ( int length )
{
    if (red != length) {
        red++;
        if (ifblue)
            return RGB ( red, 0, length );
        else
            return RGB ( red, 0, 0 );
    }
    else if (green != length) {
        green++;
        return RGB ( length, green, 0 );
    }
    else if (blue != length) {
        blue++;
        return RGB ( 0, length, blue );
    }
    else {
        red = green = blue = 0;
        ifblue = true;
        return RGB ( 0, 0, 0 );
    }
}

typedef struct
{
    FLOAT h;
    FLOAT s;
    FLOAT l;
} HSL;

namespace Colors
{

    HSL rgb2hsl ( RGBQUAD rgb )
    {
        HSL hsl;

        BYTE r = rgb.rgbRed;
        BYTE g = rgb.rgbGreen;
        BYTE b = rgb.rgbBlue;

        FLOAT _r = (FLOAT)r / 255.f;
        FLOAT _g = (FLOAT)g / 255.f;
        FLOAT _b = (FLOAT)b / 255.f;

        FLOAT rgbMin = min ( min ( _r, _g ), _b );
        FLOAT rgbMax = max ( max ( _r, _g ), _b );

        FLOAT fDelta = rgbMax - rgbMin;
        FLOAT deltaR;
        FLOAT deltaG;
        FLOAT deltaB;

        FLOAT h = 0.f;
        FLOAT s = 0.f;
        FLOAT l = (FLOAT)((rgbMax + rgbMin) / 2.f);

        if (fDelta != 0.f)
        {
            s = l < .5f ? (FLOAT)(fDelta / (rgbMax + rgbMin)) : (FLOAT)(fDelta / (2.f - rgbMax - rgbMin));
            deltaR = (FLOAT)(((rgbMax - _r) / 6.f + (fDelta / 2.f)) / fDelta);
            deltaG = (FLOAT)(((rgbMax - _g) / 6.f + (fDelta / 2.f)) / fDelta);
            deltaB = (FLOAT)(((rgbMax - _b) / 6.f + (fDelta / 2.f)) / fDelta);

            if (_r == rgbMax)      h = deltaB - deltaG;
            else if (_g == rgbMax) h = (1.f / 3.f) + deltaR - deltaB;
            else if (_b == rgbMax) h = (2.f / 3.f) + deltaG - deltaR;
            if (h < 0.f)           h += 1.f;
            if (h > 1.f)           h -= 1.f;
        }

        hsl.h = h;
        hsl.s = s;
        hsl.l = l;
        return hsl;
    }

    RGBQUAD hsl2rgb ( HSL hsl )
    {
        RGBQUAD rgb;

        FLOAT r = hsl.l;
        FLOAT g = hsl.l;
        FLOAT b = hsl.l;

        FLOAT h = hsl.h;
        FLOAT sl = hsl.s;
        FLOAT l = hsl.l;
        FLOAT v = (l <= .5f) ? (l * (1.f + sl)) : (l + sl - l * sl);

        FLOAT m;
        FLOAT sv;
        FLOAT fract;
        FLOAT vsf;
        FLOAT mid1;
        FLOAT mid2;

        INT sextant;

        if (v > 0.f)
        {
            m = l + l - v;
            sv = (v - m) / v;
            h *= 6.f;
            sextant = (INT)h;
            fract = h - sextant;
            vsf = v * sv * fract;
            mid1 = m + vsf;
            mid2 = v - vsf;

            switch (sextant)
            {
            case 0:
                r = v;
                g = mid1;
                b = m;
                break;
            case 1:
                r = mid2;
                g = v;
                b = m;
                break;
            case 2:
                r = m;
                g = v;
                b = mid1;
                break;
            case 3:
                r = m;
                g = mid2;
                b = v;
                break;
            case 4:
                r = mid1;
                g = m;
                b = v;
                break;
            case 5:
                r = v;
                g = m;
                b = mid2;
                break;
            }
        }

        rgb.rgbRed = (BYTE)(r * 255.f);
        rgb.rgbGreen = (BYTE)(g * 255.f);
        rgb.rgbBlue = (BYTE)(b * 255.f);

        return rgb;
    }
}

DWORD
WINAPI
shader1
( LPVOID lpvd )
{
    HDC hdc = GetDC ( NULL );
    HDC hdcCopy = CreateCompatibleDC ( hdc );
    int w = GetSystemMetrics ( 0 );
    int h = GetSystemMetrics ( 1 );
    BITMAPINFO bmpi = { 0 };
    HBITMAP bmp;
    bmpi.bmiHeader.biSize = sizeof ( bmpi );
    bmpi.bmiHeader.biWidth = w;
    bmpi.bmiHeader.biHeight = h;
    bmpi.bmiHeader.biPlanes = 1;
    bmpi.bmiHeader.biBitCount = 32;
    bmpi.bmiHeader.biCompression = BI_RGB;
    RGBQUAD* rgbquad = NULL;
    HSL hslcolor;
    bmp = CreateDIBSection ( hdc, &bmpi, DIB_RGB_COLORS, (void**)&rgbquad, NULL, 0 );
    SelectObject ( hdcCopy, bmp );
    INT i = 0;
    while (1) {
        hdc = GetDC ( NULL );
        StretchBlt ( hdcCopy, 0, 0, w, h, hdc, 0, 0, w, h, SRCCOPY );
        RGBQUAD rgbquadCopy;
        for (int x = 0; x < w; x++)
        {
            for (int y = 0; y < h; y++)
            {
                int index = y * w + x;

                int fx = (int)((4 * i) + ((4 * i) * sin ( x / 32.0 )) + (4 * i) + ((4 * i) * sin ( y / 24.0 )));

                rgbquadCopy = rgbquad[index];

                hslcolor = Colors::rgb2hsl ( rgbquadCopy );
                hslcolor.h = fmod ( fx / 300.f + y / h * .1f, 1.f );

                rgbquad[index] = Colors::hsl2rgb ( hslcolor );
            }
        }
        i++;
        StretchBlt ( hdc, 0, 0, w, h, hdcCopy, 0, 0, w, h, SRCCOPY );
        ReleaseDC ( NULL, hdc ); DeleteDC ( hdc );
    }
    return 0x00;
}

DWORD
WINAPI
payload1
( LPVOID lpParam )
{
    HDC hdc = GetDC ( 0 );
    int w = GetSystemMetrics ( 0 );
    int h = GetSystemMetrics ( 1 );
    while (TRUE)
    {
        hdc = GetDC ( 0 );
        BitBlt ( hdc, 10, 10, w, h, hdc, 0, 0, SRCERASE );
        BitBlt ( hdc, -10, -10, w, h, hdc, 0, 0, SRCERASE );
        ReleaseDC ( GetDesktopWindow ( ), hdc );
        DeleteDC ( hdc );
    }
    return 0;
}

DWORD
WINAPI
shader2
( LPVOID lpParam )
{
    HDC hdcScreen = GetDC ( 0 ), hdcMem = CreateCompatibleDC ( hdcScreen );
    INT w = GetSystemMetrics ( 0 ), h = GetSystemMetrics ( 1 );
    BITMAPINFO bmi = { 0 };
    PRGBQUAD rgbScreen = { 0 };
    bmi.bmiHeader.biSize = sizeof ( BITMAPINFO );
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biWidth = w;
    bmi.bmiHeader.biHeight = h;
    HBITMAP hbmTemp = CreateDIBSection ( hdcScreen, &bmi, NULL, (void**)&rgbScreen, NULL, NULL );
    SelectObject ( hdcMem, hbmTemp );
    for (;;) {
        hdcScreen = GetDC ( 0 );
        BitBlt ( hdcMem, 0, 0, w, h, hdcScreen, 0, 0, SRCERASE );
        for (INT i = 0; i < w * h; i++) {
            INT x = i % w, y = i / w;
            rgbScreen[i].rgb = (x ^ y);
        }
        BitBlt ( hdcScreen, 0, 0, w, h, hdcMem, 0, 0, SRCERASE );
        ReleaseDC ( NULL, hdcScreen ); DeleteDC ( hdcScreen );
    }
    return 0;
}

DWORD
WINAPI
shader3
( LPVOID lpParam )
{
    int w = GetSystemMetrics ( 0 ), h = GetSystemMetrics ( 1 );
    _RGBQUAD* data = (_RGBQUAD*)VirtualAlloc ( 0, (w * h + w) * sizeof ( _RGBQUAD ), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE );
    for (int i = 0;; i++, i %= 3) {
        HDC desk = GetDC ( NULL );
        HDC hdcdc = CreateCompatibleDC ( desk );
        HBITMAP hbm = CreateBitmap ( w, h, 1, 32, data );
        SelectObject ( hdcdc, hbm );
        BitBlt ( hdcdc, 0, 0, w, h, desk, 0, 0, SRCERASE );
        GetBitmapBits ( hbm, w * h * 4, data );
        for (int i = 0; w * h > i; i++) {
            int x = i % w, y = i / h, t = y ^ y | x;
            data[i].rgb = x & y & t;
        }
        SetBitmapBits ( hbm, w * h * 4, data );
        BitBlt ( desk, 0, 0, w, h, hdcdc, 0, 0, SRCERASE );
        DeleteObject ( hbm );
        DeleteObject ( hdcdc );
        DeleteObject ( desk );
    }
    return 0;
}

DWORD
WINAPI
BitBlt1
( LPVOID lpParam )
{
    HDC desktop = GetDC ( NULL );
    int xs = GetSystemMetrics ( SM_CXSCREEN );
    int ys = GetSystemMetrics ( SM_CYSCREEN );
    while (TRUE) {
        desktop = GetDC ( NULL );
        BitBlt ( desktop, 1, 1, xs, ys, desktop, -2, 2, SRCINVERT );
        ReleaseDC ( 0, desktop );
    }
    return 0;
}

DWORD
WINAPI
profect
( LPVOID lpParam )
{
    int w = GetSystemMetrics ( 0 );
    int h = GetSystemMetrics ( 1 );
    while (1) {
        HDC hdc = GetDC ( 0 );
        BitBlt ( hdc, 0, 0, w, h, hdc, -30, 0, SRCCOPY );
        BitBlt ( hdc, 0, 0, w, h, hdc, w - 30, 0, SRCCOPY );
        BitBlt ( hdc, 0, 0, w, h, hdc, 0, -30, SRCCOPY );
        BitBlt ( hdc, 0, 0, w, h, hdc, 0, h - 30, SRCCOPY );
        HBRUSH brush = CreateSolidBrush ( RGB ( rand ( ) % 255, rand ( ) % 255, rand ( ) % 255 ) );
        SelectObject ( hdc, brush );
        BitBlt ( hdc, 0, 0, w, h, hdc, 0, 0, PATINVERT );
        DeleteObject ( brush );
        ReleaseDC ( 0, hdc );
        Sleep ( 10 );
    }
    return 0;
}

DWORD
WINAPI
EffectWave
( LPVOID lpParam )
{
    HDC hdc = GetDC ( NULL );
    if (!hdc)
        return 1;
    int x0 = GetSystemMetrics ( SM_XVIRTUALSCREEN );
    int y0 = GetSystemMetrics ( SM_YVIRTUALSCREEN );
    int w = GetSystemMetrics ( SM_CXVIRTUALSCREEN );
    int h = GetSystemMetrics ( SM_CYVIRTUALSCREEN );
    HDC src = CreateCompatibleDC ( hdc );
    HBITMAP srcBmp = CreateCompatibleBitmap ( hdc, w, h );
    HGDIOBJ oldBmp = SelectObject ( src, srcBmp );
    BitBlt ( src, 0, 0, w, h, hdc, x0, y0, SRCCOPY );
    int offset = 0;
    DWORD start = GetTickCount ( );
    while (GetTickCount ( ) - start < INFINITE)
    {
        for (int y = 0; y < h; y++)
        {
            int x = (int)(24.0 * sin ( (y + offset) * 0.05 ));
            BitBlt ( hdc, x0 + x, y0 + y, w, 1, src, 0, y, SRCCOPY );
        }

        offset += 6;
        Sleep ( 16 );
    }
    BitBlt ( hdc, x0, y0, w, h, src, 0, 0, SRCCOPY );
    SelectObject ( src, oldBmp );
    DeleteObject ( srcBmp );
    DeleteDC ( src );
    ReleaseDC ( NULL, hdc );
    return 0;
}

DWORD
WINAPI
shader4
( LPVOID lpParam )
{
    HDC hdcScreen = GetDC ( 0 ), hdcMem = CreateCompatibleDC ( hdcScreen );
    INT w = GetSystemMetrics ( 0 ), h = GetSystemMetrics ( 1 );
    BITMAPINFO bmi = { 0 };
    PRGBQUAD rgbScreen = { 0 };
    bmi.bmiHeader.biSize = sizeof ( BITMAPINFO );
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biWidth = w;
    bmi.bmiHeader.biHeight = h;
    HBITMAP hbmTemp = CreateDIBSection ( hdcScreen, &bmi, NULL, (void**)&rgbScreen, NULL, NULL );
    SelectObject ( hdcMem, hbmTemp );
    for (;;) {
        hdcScreen = GetDC ( 0 );
        BitBlt ( hdcMem, 0, 0, w, h, hdcScreen, 0, 0, SRCCOPY );
        for (INT i = 0; i < w * h; i++) {
            INT x = i % w, y = i / w;
            rgbScreen[i].rgb += (x * y);
        }
        BitBlt ( hdcScreen, 0, 0, w, h, hdcMem, 0, 0, SRCCOPY );
        ReleaseDC ( NULL, hdcScreen ); DeleteDC ( hdcScreen );
    }
    return 0;
}

DWORD
WINAPI
shader5
( LPVOID lpParam )
{
    int time = GetTickCount ( );
    int w = GetSystemMetrics ( 0 ), h = GetSystemMetrics ( 1 );
    RGBQUAD* data = (RGBQUAD*)VirtualAlloc ( 0, (w * h + w) * sizeof ( RGBQUAD ), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE );
    for (int i = 0;; i++, i %= 3) {
        HDC desk = GetDC ( NULL );
        HDC hdcdc = CreateCompatibleDC ( desk );
        HBITMAP hbm = CreateBitmap ( w, h, 1, 32, data );
        SelectObject ( hdcdc, hbm );
        BitBlt ( hdcdc, 0, 0, w, h, desk, 0, 0, SRCERASE );
        GetBitmapBits ( hbm, 4 * h * w, data );
        int v = 0;
        BYTE byte = 0;
        if ((GetTickCount ( ) - time) > 10)
            byte = rand ( ) % 0xff;
        for (int i = 0; w * h > i; ++i) {
            if (!(i % h) && !(rand ( ) % 110))
                v = rand ( ) % 24;
            *((BYTE*)data + 4 * i + v) -= 5;
        }
        SetBitmapBits ( hbm, w * h * 4, data );
        BitBlt ( desk, 0, 0, w, h, hdcdc, 0, 0, SRCINVERT );
        DeleteObject ( hbm );
        DeleteObject ( hdcdc );
        DeleteObject ( desk );
    }
    return 0;
}

DWORD
WINAPI
shader6
( LPVOID lpParam )
{
    HDC hdcScreen = GetDC ( 0 ), hdcMem = CreateCompatibleDC ( hdcScreen );
    INT w = GetSystemMetrics ( 0 ), h = GetSystemMetrics ( 1 );
    BITMAPINFO bmi = { 0 };
    PRGBQUAD rgbScreen = { 0 };
    bmi.bmiHeader.biSize = sizeof ( BITMAPINFO );
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biWidth = w;
    bmi.bmiHeader.biHeight = h;
    HBITMAP hbmTemp = CreateDIBSection ( hdcScreen, &bmi, NULL, (void**)&rgbScreen, NULL, NULL );
    SelectObject ( hdcMem, hbmTemp );
    for (;;) {
        hdcScreen = GetDC ( 0 );
        BitBlt ( hdcMem, 0, 0, w, h, hdcScreen, 0, 0, SRCCOPY );
        for (INT i = 0; i < w * h; i++) {
            rgbScreen[i].rgb = (rgbScreen[i].rgb * 2) % (RGB ( 255, 255, 255 ));
        }
        BitBlt ( hdcScreen, 0, 0, w, h, hdcMem, -30, 0, SRCCOPY );
        BitBlt ( hdcScreen, 0, 0, w, h, hdcMem, w - 30, 0, SRCCOPY );
        BitBlt ( hdcScreen, 0, 0, w, h, hdcMem, 0, -30, SRCCOPY );
        BitBlt ( hdcScreen, 0, 0, w, h, hdcMem, 0, h - 30, SRCCOPY );
        ReleaseDC ( NULL, hdcScreen ); DeleteDC ( hdcScreen );
    }
    return 0;
}

DWORD
WINAPI
shader7
( LPVOID lpParam )
{
    HDC desk = GetDC ( 0 ); HWND wnd = GetDesktopWindow ( );
    int sw = GetSystemMetrics ( 0 ), sh = GetSystemMetrics ( 1 );
    BITMAPINFO bmi = { 40, sw, sh, 1, 24 };
    PRGBTRIPLE rgbtriple;
    for (;;) {
        desk = GetDC ( 0 );
        HDC deskMem = CreateCompatibleDC ( desk );
        HBITMAP scr = CreateDIBSection ( desk, &bmi, 0, (void**)&rgbtriple, 0, 0 );
        SelectObject ( deskMem, scr );
        BitBlt ( deskMem, 0, 0, sw, sh, desk, 0, 0, SRCINVERT );
        for (int i = 0; i < sw * sh; i++) {
            rgbtriple[i].rgbtRed = (rgbtriple[i].rgbtRed * 2) % (RGB ( 255, 0, 0 ));
            rgbtriple[i].rgbtGreen = (rgbtriple[i].rgbtGreen * 2) % (RGB ( 0, 255, 0 ));
            rgbtriple[i].rgbtBlue = (rgbtriple[i].rgbtBlue * 2) % (RGB ( 0, 0, 255 ));
        }
        BitBlt ( desk, 0, 0, sw, sh, deskMem, 0, 0, SRCINVERT );
        ReleaseDC ( wnd, desk );
        DeleteDC ( desk ); DeleteDC ( deskMem ); DeleteObject ( scr ); DeleteObject ( wnd ); DeleteObject ( rgbtriple ); DeleteObject ( &sw ); DeleteObject ( &sh ); DeleteObject ( &bmi );
    }
}

VOID
WINAPI
sound1
( )
{
    HWAVEOUT hWaveOut = NULL;
    WAVEFORMATEX wfx = { WAVE_FORMAT_PCM, 1, 8000, 8000, 1, 8, 0 };
    if (waveOutOpen ( &hWaveOut, WAVE_MAPPER, &wfx, 0, 0, CALLBACK_NULL ) != MMSYSERR_NOERROR) return;
    const DWORD bufferSize = 8000 * 30;
    char* buffer = (char*)HeapAlloc ( GetProcessHeap ( ), HEAP_ZERO_MEMORY, bufferSize );
    if (!buffer) { waveOutClose ( hWaveOut ); return; }
    for (DWORD t = 0; t < bufferSize; ++t) {
        buffer[t] = (BYTE)(t % 25 - (t >> 2 | 15 * t | t % 227) - t >> 3 | t << 3 | t >> 3 | t << 2 | t << 3 | (t >> 3) + (t << 3));

    }
    WAVEHDR header = { 0 };
    header.lpData = buffer;
    header.dwBufferLength = bufferSize;
    waveOutPrepareHeader ( hWaveOut, &header, sizeof ( WAVEHDR ) );
    waveOutWrite ( hWaveOut, &header, sizeof ( WAVEHDR ) );
    waveOutUnprepareHeader ( hWaveOut, &header, sizeof ( WAVEHDR ) ); Sleep ( 30000 );
    waveOutClose ( hWaveOut );
    HeapFree ( GetProcessHeap ( ), 0, buffer );
}

VOID
WINAPI
sound2
( )
{
    HWAVEOUT hWaveOut = NULL;
    WAVEFORMATEX wfx = { WAVE_FORMAT_PCM, 1, 8000, 8000, 1, 8, 0 };
    if (waveOutOpen ( &hWaveOut, WAVE_MAPPER, &wfx, 0, 0, CALLBACK_NULL ) != MMSYSERR_NOERROR) return;
    const DWORD bufferSize = 8000 * 30;
    char* buffer = (char*)HeapAlloc ( GetProcessHeap ( ), HEAP_ZERO_MEMORY, bufferSize );
    if (!buffer) { waveOutClose ( hWaveOut ); return; }
    for (DWORD t = 0; t < bufferSize; ++t) {
        buffer[t] = (BYTE)(t >> 6) | (t << 3) | t >> 3 | (t) | (t << 1) | (t >> 3);

    }
    WAVEHDR header = { 0 };
    header.lpData = buffer;
    header.dwBufferLength = bufferSize;
    waveOutPrepareHeader ( hWaveOut, &header, sizeof ( WAVEHDR ) );
    waveOutWrite ( hWaveOut, &header, sizeof ( WAVEHDR ) );
    waveOutUnprepareHeader ( hWaveOut, &header, sizeof ( WAVEHDR ) ); Sleep ( 30000 );
    waveOutClose ( hWaveOut );
    HeapFree ( GetProcessHeap ( ), 0, buffer );
}

VOID
WINAPI
sound3
( )
{
    HWAVEOUT hWaveOut = NULL;
    WAVEFORMATEX wfx = { WAVE_FORMAT_PCM, 1, 8000, 8000, 1, 8, 0 };
    if (waveOutOpen ( &hWaveOut, WAVE_MAPPER, &wfx, 0, 0, CALLBACK_NULL ) != MMSYSERR_NOERROR) return;
    const DWORD bufferSize = 8000 * 30;
    char* buffer = (char*)HeapAlloc ( GetProcessHeap ( ), HEAP_ZERO_MEMORY, bufferSize );
    if (!buffer) { waveOutClose ( hWaveOut ); return; }
    for (DWORD t = 0; t < bufferSize; ++t) {
        buffer[t] = (BYTE)(t * ((t & 4096 ? t % 65536 < 59392 ? 7 : t & 7 : 16) ^ (1 & t >> 14)) >> (3 & t >> (t & 2048 ? 2 : 10)));

    }
    WAVEHDR header = { 0 };
    header.lpData = buffer;
    header.dwBufferLength = bufferSize;
    waveOutPrepareHeader ( hWaveOut, &header, sizeof ( WAVEHDR ) );
    waveOutWrite ( hWaveOut, &header, sizeof ( WAVEHDR ) );
    waveOutUnprepareHeader ( hWaveOut, &header, sizeof ( WAVEHDR ) ); Sleep ( 30000 );
    waveOutClose ( hWaveOut );
    HeapFree ( GetProcessHeap ( ), 0, buffer );
}

VOID
WINAPI
sound4
( )
{
    HWAVEOUT hWaveOut = NULL;
    WAVEFORMATEX wfx = { WAVE_FORMAT_PCM, 1, 8000, 8000, 1, 8, 0 };
    if (waveOutOpen ( &hWaveOut, WAVE_MAPPER, &wfx, 0, 0, CALLBACK_NULL ) != MMSYSERR_NOERROR) return;
    const DWORD bufferSize = 8000 * 30;
    char* buffer = (char*)HeapAlloc ( GetProcessHeap ( ), HEAP_ZERO_MEMORY, bufferSize );
    if (!buffer) { waveOutClose ( hWaveOut ); return; }
    for (DWORD t = 0; t < bufferSize; ++t) {
        buffer[t] = (BYTE)((t >> 8 | t << 2) + (t >> 4 | t << 2 | t >> 3) | t >> 2 | t << 5);

    }
    WAVEHDR header = { 0 };
    header.lpData = buffer;
    header.dwBufferLength = bufferSize;
    waveOutPrepareHeader ( hWaveOut, &header, sizeof ( WAVEHDR ) );
    waveOutWrite ( hWaveOut, &header, sizeof ( WAVEHDR ) );
    waveOutUnprepareHeader ( hWaveOut, &header, sizeof ( WAVEHDR ) ); Sleep ( 30000 );
    waveOutClose ( hWaveOut );
    HeapFree ( GetProcessHeap ( ), 0, buffer );
}

VOID
WINAPI
sound5
( )
{
    HWAVEOUT hWaveOut = NULL;
    WAVEFORMATEX wfx = { WAVE_FORMAT_PCM, 1, 11025, 11025, 1, 8, 0 };
    if (waveOutOpen ( &hWaveOut, WAVE_MAPPER, &wfx, 0, 0, CALLBACK_NULL ) != MMSYSERR_NOERROR) return;
    const DWORD bufferSize = 11025 * 30;
    char* buffer = (char*)HeapAlloc ( GetProcessHeap ( ), HEAP_ZERO_MEMORY, bufferSize );
    if (!buffer) { waveOutClose ( hWaveOut ); return; }
    for (DWORD t = 0; t < bufferSize; ++t) {
        buffer[t] = (BYTE)(((t / 10 | 0) ^ (t / 10 | 0) - 1280) % 11 * t / 2 & 127) + (((t / 640 | 0) ^ (t / 640 | 0) - 2) % 13 * t / 2 & 127);

    }
    WAVEHDR header = { 0 };
    header.lpData = buffer;
    header.dwBufferLength = bufferSize;
    waveOutPrepareHeader ( hWaveOut, &header, sizeof ( WAVEHDR ) );
    waveOutWrite ( hWaveOut, &header, sizeof ( WAVEHDR ) );
    waveOutUnprepareHeader ( hWaveOut, &header, sizeof ( WAVEHDR ) ); Sleep ( 30000 );
    waveOutClose ( hWaveOut );
    HeapFree ( GetProcessHeap ( ), 0, buffer );
}

VOID
WINAPI
sound6
( )
{
    HWAVEOUT hWaveOut = NULL;
    WAVEFORMATEX wfx = { WAVE_FORMAT_PCM, 1, 44100, 44100, 1, 8, 0 };
    if (waveOutOpen ( &hWaveOut, WAVE_MAPPER, &wfx, 0, 0, CALLBACK_NULL ) != MMSYSERR_NOERROR) return;
    const DWORD bufferSize = 44100 * 30;
    char* buffer = (char*)HeapAlloc ( GetProcessHeap ( ), HEAP_ZERO_MEMORY, bufferSize );
    if (!buffer) { waveOutClose ( hWaveOut ); return; }
    for (DWORD t = 0; t < bufferSize; ++t) {
        buffer[t] = (BYTE)(((t & 32767) >> 13 == 2 || (t & 65535) >> 12 == 9 ? (t ^ (0 - (t / 8 & t >> 5)) * (t / 8 & 127)) & ((0 - (t >> 5)) & 255) * ((t & 65535) >> 12 == 9 ? 2 : 1) : (t & 8191) % ((t >> 5 & 255 ^ 240) == 0 ? 1 : t >> 5 & 255 ^ 240)) / 4 * 3 + (t * 4 / (4 + (t >> 15 & 3)) & 128) * ((0 - t) >> 11 & 2) * ((t & 32767) >> 13 != 2) / 3);

    }
    WAVEHDR header = { 0 };
    header.lpData = buffer;
    header.dwBufferLength = bufferSize;
    waveOutPrepareHeader ( hWaveOut, &header, sizeof ( WAVEHDR ) );
    waveOutWrite ( hWaveOut, &header, sizeof ( WAVEHDR ) );
    waveOutUnprepareHeader ( hWaveOut, &header, sizeof ( WAVEHDR ) ); Sleep ( 30000 );
    waveOutClose ( hWaveOut );
    HeapFree ( GetProcessHeap ( ), 0, buffer );
}

VOID
WINAPI
sound7
( )
{
    HWAVEOUT hWaveOut = NULL;
    WAVEFORMATEX wfx = { WAVE_FORMAT_PCM, 1, 8000, 8000, 1, 8, 0 };
    if (waveOutOpen ( &hWaveOut, WAVE_MAPPER, &wfx, 0, 0, CALLBACK_NULL ) != MMSYSERR_NOERROR) return;
    const DWORD bufferSize = 8000 * 30;
    char* buffer = (char*)HeapAlloc ( GetProcessHeap ( ), HEAP_ZERO_MEMORY, bufferSize );
    if (!buffer) { waveOutClose ( hWaveOut ); return; }
    for (DWORD t = 0; t < bufferSize; ++t) {
        buffer[t] = (BYTE)((t >> 6 | t << 1) + (227) + t & t << 2);

    }
    WAVEHDR header = { 0 };
    header.lpData = buffer;
    header.dwBufferLength = bufferSize;
    waveOutPrepareHeader ( hWaveOut, &header, sizeof ( WAVEHDR ) );
    waveOutWrite ( hWaveOut, &header, sizeof ( WAVEHDR ) );
    waveOutUnprepareHeader ( hWaveOut, &header, sizeof ( WAVEHDR ) ); Sleep ( 30000 );
    waveOutClose ( hWaveOut );
    HeapFree ( GetProcessHeap ( ), 0, buffer );
}

VOID
WINAPI
sound8
( )
{
    HWAVEOUT hWaveOut = NULL;
    WAVEFORMATEX wfx = { WAVE_FORMAT_PCM, 1, 11025, 11025, 1, 8, 0 };
    if (waveOutOpen ( &hWaveOut, WAVE_MAPPER, &wfx, 0, 0, CALLBACK_NULL ) != MMSYSERR_NOERROR) return;
    const DWORD bufferSize = 11025 * 30;
    char* buffer = (char*)HeapAlloc ( GetProcessHeap ( ), HEAP_ZERO_MEMORY, bufferSize );
    if (!buffer) { waveOutClose ( hWaveOut ); return; }
    for (DWORD t = 0; t < bufferSize; ++t) {
        buffer[t] = (BYTE)((t * (t << 1 ^ (t << 1) + (t >> 7) & t >> 12 | t >> 4 - (1 ^ 7 & t >> 19) | t >> 7)));

    }
    WAVEHDR header = { 0 };
    header.lpData = buffer;
    header.dwBufferLength = bufferSize;
    waveOutPrepareHeader ( hWaveOut, &header, sizeof ( WAVEHDR ) );
    waveOutWrite ( hWaveOut, &header, sizeof ( WAVEHDR ) );
    waveOutUnprepareHeader ( hWaveOut, &header, sizeof ( WAVEHDR ) ); Sleep ( 30000 );
    waveOutClose ( hWaveOut );
    HeapFree ( GetProcessHeap ( ), 0, buffer );
}

VOID
WINAPI
sound9
( )
{
    HWAVEOUT hWaveOut = NULL;
    WAVEFORMATEX wfx = { WAVE_FORMAT_PCM, 1, 8000, 8000, 1, 8, 0 };
    if (waveOutOpen ( &hWaveOut, WAVE_MAPPER, &wfx, 0, 0, CALLBACK_NULL ) != MMSYSERR_NOERROR) return;
    const DWORD bufferSize = 8000 * 30;
    char* buffer = (char*)HeapAlloc ( GetProcessHeap ( ), HEAP_ZERO_MEMORY, bufferSize );
    if (!buffer) { waveOutClose ( hWaveOut ); return; }
    for (DWORD t = 0; t < bufferSize; ++t) {
        buffer[t] = (BYTE)(t % 25 - (t >> 3) | 9 * t & t >> 4 | 5 * t & t >> 7 | 3 * t & t >> 10);

    }
    WAVEHDR header = { 0 };
    header.lpData = buffer;
    header.dwBufferLength = bufferSize;
    waveOutPrepareHeader ( hWaveOut, &header, sizeof ( WAVEHDR ) );
    waveOutWrite ( hWaveOut, &header, sizeof ( WAVEHDR ) );
    waveOutUnprepareHeader ( hWaveOut, &header, sizeof ( WAVEHDR ) ); Sleep ( 30000 );
    waveOutClose ( hWaveOut );
    HeapFree ( GetProcessHeap ( ), 0, buffer );
}

VOID
WINAPI
sound10
( )
{
    HWAVEOUT hWaveOut = NULL;
    WAVEFORMATEX wfx = { WAVE_FORMAT_PCM, 1, 16000, 16000, 1, 8, 0 };
    if (waveOutOpen ( &hWaveOut, WAVE_MAPPER, &wfx, 0, 0, CALLBACK_NULL ) != MMSYSERR_NOERROR) return;
    const DWORD bufferSize = 16000 * 30;
    char* buffer = (char*)HeapAlloc ( GetProcessHeap ( ), HEAP_ZERO_MEMORY, bufferSize );
    if (!buffer) { waveOutClose ( hWaveOut ); return; }
    for (DWORD t = 0; t < bufferSize; ++t) {
        buffer[t] = (BYTE)((t | t % 255 | t % 257) + (t & t >> 8) + (t * (42 & t >> 10)) + ((t % ((t >> 8 | t >> 16) + 1)) ^ t));

    }
    WAVEHDR header = { 0 };
    header.lpData = buffer;
    header.dwBufferLength = bufferSize;
    waveOutPrepareHeader ( hWaveOut, &header, sizeof ( WAVEHDR ) );
    waveOutWrite ( hWaveOut, &header, sizeof ( WAVEHDR ) );
    waveOutUnprepareHeader ( hWaveOut, &header, sizeof ( WAVEHDR ) ); Sleep ( 30000 );
    waveOutClose ( hWaveOut );
    HeapFree ( GetProcessHeap ( ), 0, buffer );
}

VOID
WINAPI
sound11
( )
{
    HWAVEOUT hWaveOut = NULL;
    WAVEFORMATEX wfx = { WAVE_FORMAT_PCM, 1, 48000, 48000, 1, 8, 0 };
    if (waveOutOpen ( &hWaveOut, WAVE_MAPPER, &wfx, 0, 0, CALLBACK_NULL ) != MMSYSERR_NOERROR) return;
    const DWORD bufferSize = 48000 * 30;
    char* buffer = (char*)HeapAlloc ( GetProcessHeap ( ), HEAP_ZERO_MEMORY, bufferSize );
    if (!buffer) { waveOutClose ( hWaveOut ); return; }
    for (DWORD t = 0; t < bufferSize; ++t) {
        buffer[t] = (BYTE)(t >> 1 | t >> 5 | t >> 9);

    }
    WAVEHDR header = { 0 };
    header.lpData = buffer;
    header.dwBufferLength = bufferSize;
    waveOutPrepareHeader ( hWaveOut, &header, sizeof ( WAVEHDR ) );
    waveOutWrite ( hWaveOut, &header, sizeof ( WAVEHDR ) );
    waveOutUnprepareHeader ( hWaveOut, &header, sizeof ( WAVEHDR ) ); Sleep ( 30000 );
    waveOutClose ( hWaveOut );
    HeapFree ( GetProcessHeap ( ), 0, buffer );
}
