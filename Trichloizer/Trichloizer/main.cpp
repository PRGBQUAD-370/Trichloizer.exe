#include "Trichloizer.h"

VOID 
WINAPI 
Initialize 
( VOID ) 
{
    HMODULE hModUser32 = LoadLibraryW ( L"user32.dll" );
    BOOL ( WINAPI * SetProcessDPIAware )(VOID) = (BOOL ( WINAPI* )(VOID))GetProcAddress ( hModUser32, "SetProcessDPIAware" );
    if ( SetProcessDPIAware ) SetProcessDPIAware ( );
    FreeLibrary ( hModUser32 );
}

INT
WINAPI
wWinMain (
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ PWSTR pszCmdLine,
    _In_ INT nShowCmd
)
{
    UNREFERENCED_PARAMETER ( hInstance );
    UNREFERENCED_PARAMETER ( hPrevInstance );
    UNREFERENCED_PARAMETER ( pszCmdLine );
    UNREFERENCED_PARAMETER ( nShowCmd );

    Initialize ( );

    if (MessageBoxW ( NULL, L"Do you want to run GDI?", L"Trichloizer.exe", MB_YESNO | MB_ICONEXCLAMATION ) == IDNO)
    {
        ExitProcess ( 0 );
    }

    if (MessageBoxW ( NULL, L"Are you sure?\r\nFinal Chance of stoping it.", L"Trichloizer.exe", MB_YESNO | MB_ICONEXCLAMATION ) == IDNO)
    {
        ExitProcess ( 0 );
    }

    Sleep ( 1000 );

    HANDLE thread1 = CreateThread ( 0, 0, shader1, 0, 0, 0 );
    sound1 ( );
    TerminateThread ( thread1, 0 );
    CloseHandle ( thread1 );
    RedrawWindow ( NULL, NULL, NULL, RDW_INVALIDATE | RDW_ERASE | RDW_ALLCHILDREN | RDW_UPDATENOW );
    Sleep ( 100 );
    HANDLE thread2 = CreateThread ( 0, 0, payload1, 0, 0, 0 );
    sound2 ( );
    TerminateThread ( thread2, 0 );
    CloseHandle ( thread2 );
    RedrawWindow ( NULL, NULL, NULL, RDW_INVALIDATE | RDW_ERASE | RDW_ALLCHILDREN | RDW_UPDATENOW );
    Sleep ( 100 );
    HANDLE thread3 = CreateThread ( 0, 0, shader2, 0, 0, 0 );
    sound3 ( );
    TerminateThread ( thread3, 0 );
    CloseHandle ( thread3 );
    RedrawWindow ( NULL, NULL, NULL, RDW_INVALIDATE | RDW_ERASE | RDW_ALLCHILDREN | RDW_UPDATENOW );
    Sleep ( 100 );
    HANDLE thread4 = CreateThread ( 0, 0, shader3, 0, 0, 0 );
    sound4 ( );
    TerminateThread ( thread4, 0 );
    CloseHandle ( thread4 );
    RedrawWindow ( NULL, NULL, NULL, RDW_INVALIDATE | RDW_ERASE | RDW_ALLCHILDREN | RDW_UPDATENOW );
    Sleep ( 100 );
    HANDLE thread5 = CreateThread ( 0, 0, BitBlt1, 0, 0, 0 );
    sound5 ( );
    TerminateThread ( thread5, 0 );
    CloseHandle ( thread5 );
    RedrawWindow ( NULL, NULL, NULL, RDW_INVALIDATE | RDW_ERASE | RDW_ALLCHILDREN | RDW_UPDATENOW );
    Sleep ( 100 );
    HANDLE thread6 = CreateThread ( 0, 0, profect, 0, 0, 0 );
    sound6 ( );
    TerminateThread ( thread6, 0 );
    CloseHandle ( thread6 );
    RedrawWindow ( NULL, NULL, NULL, RDW_INVALIDATE | RDW_ERASE | RDW_ALLCHILDREN | RDW_UPDATENOW );
    Sleep ( 100 );
    HANDLE thread7 = CreateThread ( 0, 0, EffectWave, 0, 0, 0 );
    sound7 ( );
    TerminateThread ( thread7, 0 );
    CloseHandle ( thread7 );
    RedrawWindow ( NULL, NULL, NULL, RDW_INVALIDATE | RDW_ERASE | RDW_ALLCHILDREN | RDW_UPDATENOW );
    Sleep ( 100 );
    HANDLE thread8 = CreateThread ( 0, 0, shader4, 0, 0, 0 );
    sound8 ( );
    TerminateThread ( thread8, 0 );
    CloseHandle ( thread8 );
    RedrawWindow ( NULL, NULL, NULL, RDW_INVALIDATE | RDW_ERASE | RDW_ALLCHILDREN | RDW_UPDATENOW );
    Sleep ( 100 );
    HANDLE thread9 = CreateThread ( 0, 0, shader5, 0, 0, 0 );
    sound9 ( );
    TerminateThread ( thread9, 0 );
    CloseHandle ( thread9 );
    RedrawWindow ( NULL, NULL, NULL, RDW_INVALIDATE | RDW_ERASE | RDW_ALLCHILDREN | RDW_UPDATENOW );
    Sleep ( 100 );
    HANDLE thread10 = CreateThread ( 0, 0, shader6, 0, 0, 0 );
    sound10 ( );
    TerminateThread ( thread10, 0 );
    CloseHandle ( thread10 );
    RedrawWindow ( NULL, NULL, NULL, RDW_INVALIDATE | RDW_ERASE | RDW_ALLCHILDREN | RDW_UPDATENOW );
    Sleep ( 100 );
    HANDLE thread11 = CreateThread ( 0, 0, shader7, 0, 0, 0 );
    sound11 ( );
    TerminateThread ( thread11, 0 );
    CloseHandle ( thread11 );
    RedrawWindow ( NULL, NULL, NULL, RDW_INVALIDATE | RDW_ERASE | RDW_ALLCHILDREN | RDW_UPDATENOW );
    Sleep ( 100 );

    return 0;

}