// ProCave.cpp : Defines the entry point for the application.

#include "Flecs/flecs.h"

#include "SH.h"
#include "ProCave.h"
#include "Base/BaseApp.h"
#include "Graphics/Renderer.h"
#include "Base/ECS.h"

#include "User/Input.h"


#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance   

LPWSTR szWindowClass;            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                RegisterWindow(HINSTANCE hInstance);
BOOL                InitWindow(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
HWND hWnd; //Handle to our now one window, this needs to be changed if I want more windows

BaseApp Game;
Renderer* Graphics = Renderer::get();

flecs::world ECS;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    
    // Initialize global strings
    LoadStringW(hInstance, IDC_PROCAVE, szWindowClass, MAX_LOADSTRING);
    RegisterWindow(hInstance);
    
    // Perform application initialization:
    if (!InitWindow (hInstance, nCmdShow))
    {
        return FALSE;
    }

    //#INITIALIZATION
   
    SetupSystems(ECS);
    Game.Init();
    Graphics->InitializeDirect3d11App(hInstance, hWnd);
    Graphics->InitializeRenderer();

    gainput::InputManager* GaInput = Input::get()->GetManager();
    GaInput->SetDisplaySize(Graphics->Width, Graphics->Height);

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_PROCAVE));

    MSG msg;

    //#LOADRESOURCES 
    Game.Load();


    //#TICK
    while (ECS.progress()) {
        GaInput->Update();
        // Main message loop:
        PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE);
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

            GaInput->HandleMessage(msg);
		}
        

        Game.Tick();
        Graphics->Draw();
    }
    
    //#UNLOAD
    Game.UnLoad();

    //#SHUTDOWN
    Game.Exit();

    return (int) msg.wParam;
}


ATOM RegisterWindow(HINSTANCE hInstance)
{
    //Windows conversion from c style string to widestring, to easy expose the name
	int wchars_num = MultiByteToWideChar(CP_UTF8, 0, "WindowClass", -1, NULL, 0);

	wchar_t* wstr = new wchar_t[wchars_num];

	MultiByteToWideChar(CP_UTF8, 0, "WindowClass", -1, wstr, wchars_num);

    szWindowClass = wstr;


    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(NULL, IDI_QUESTION);//LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PROCAVE));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = NULL;//MAKEINTRESOURCEW(IDC_PROCAVE);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(NULL, IDI_QUESTION);

    return RegisterClassExW(&wcex);
}

BOOL InitWindow(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   //Windows conversion from c style string to widestring
   int wchars_num = MultiByteToWideChar(CP_UTF8, 0, "ProCave", -1, NULL, 0);

   wchar_t* wstr = new wchar_t[wchars_num];

   MultiByteToWideChar(CP_UTF8, 0, "ProCave", -1, wstr, wchars_num);

   hWnd = CreateWindowW(szWindowClass, wstr, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);


   delete[] wstr;
   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
   //case WM_COMMAND:
   //    {
   //        int wmId = LOWORD(wParam);
   //        // Parse the menu selections:
   //        switch (wmId)
   //        {
   //        case IDM_EXIT:
   //            DestroyWindow(hWnd);
   //            break;
   //        default:
   //            return DefWindowProc(hWnd, message, wParam, lParam);
   //        }
   //    }
   // break;
		//  case WM_PAINT:
		//      {
		//	  //PAINTSTRUCT ps;
		//	  //HDC hdc = BeginPaint(hWnd, &ps);
		//	  //// TODO: Add any drawing code that uses hdc here...
		//	  //EndPaint(hWnd, &ps);
		//      }
       //break;
        
    case WM_DESTROY:
        ECS.quit();
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
