// ProCave.cpp : Defines the entry point for the application.




//#include "flecs/flecs.h"
#define STB_IMAGE_IMPLEMENTATION
#include <chrono>
//#include <iostream>
//#include <sys/time.h>
#include <ctime>

#include "SH.h"
#include "ProCave.h"
#include "Base/BaseApp.h"
#include "Graphics/Renderer.h"
#include "Base/ECS.h"

#include "User/Input.h"
#include "Base/Setup.h"

#include "Rendering/DevUIDriver.h"
#include "imgui/backends/imgui_impl_win32.h"
#include "Noise/NoiseTool.h"
#include "Tools/MarchCubeSettings.h"
#include "Tools/DebugSettings.h"

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
float Deltatime = 0.f;
//flecs::world ECS;

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
   
    //SetupSystems(ECS);
    SetupSpdlog();
    Game.Init();
    Game.HandleWindow = hWnd;
    Graphics->InitializeDirect3d11App(hInstance, hWnd);
    Graphics->InitializeRenderer();

    gainput::InputManager* GaInput = Input::get()->GetManager();
    GaInput->SetDisplaySize(Graphics->Width, Graphics->Height);

    DevUIDriver::get()->SetupWindow(hWnd, Graphics->d3d11Device, Graphics->d3d11DevCon);

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_PROCAVE));

    MSG msg;

    DevUIDriver::get()->Windows.push_back(new NoiseTool());
    DevUIDriver::get()->Windows.push_back(MarchCubeSettings::get());
    DevUIDriver::get()->Windows.push_back(DebugSettings::get());

    spdlog::info("Window is {0:d} by {1:d}", Graphics->Width, Graphics->Height);

    //#LOADRESOURCES 
    Game.Load();
    

    long long LastTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    long long Calcer;
    //#TICK
    while (!Game.Shutdown) {
        Calcer = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
        Deltatime = fmin(float(Calcer - LastTime) / 1000.f, 0.016f);
        LastTime = Calcer;

        GaInput->Update();
        // Main message loop:
        PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE);
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

            GaInput->HandleMessage(msg);
		}
        

        Game.Tick(Deltatime);
        DevUIDriver::get()->Update(Deltatime);

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
      CW_USEDEFAULT, 0, /*CW_USEDEFAULT*/ Graphics->Width, Graphics->Height + 30, nullptr, nullptr, hInstance, nullptr);


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
    ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam);

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
    case WM_PAINT:
		{
		    //PAINTSTRUCT ps;
		    //HDC hdc = BeginPaint(hWnd, &ps);
		    //// TODO: Add any drawing code that uses hdc here...
		    //EndPaint(hWnd, &ps);
	    }
		break;
    case WM_SIZE:
       // if (!sizemove) {
        Graphics->Width = LOWORD(lParam);
        Graphics->Height = HIWORD(lParam);
        Graphics->Resize();
      //  }
        break;
    case WM_DESTROY:
        //ECS.quit();
        Game.Shutdown = true;
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
