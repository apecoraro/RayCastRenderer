
#include <stdafx.h>
#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>
#include <string>
#include <algorithm>
#include <io.h>
#include <commdlg.h>
using namespace Gdiplus;
#pragma comment (lib,"Gdiplus.lib")
#include <time.h>
#include <sstream>

#include "Image.h"
#include "SmartPtr.h"
#include "RayCaster.h"
#include "HeightField.h"
#include "Sampler.h"
#include "UniformGridSampler.h"
#include "JitteredGridSampler.h"
#include "Random.h"
#include "Texture.h"

using namespace cs500;

SmartPtr<cs500::Image> g_spImage = NULL;
int g_imageSamplerType = 0;
int  g_antiAliasingGridSize = 3;
int g_softShadowRays = 0;
int g_blurRays = 0;
float g_transBlurFactor = 0.05f;
float g_reflBlurFactor = 0.5f;
bool g_renderTiles = true;
bool g_useSphereTree = false;
std::wstring g_file;

void ResizeWindow(HWND hWnd, int rectWidth, int rectHeight)
{
    RECT rect;
    SetRect(&rect, 0, 0, 
            rectWidth, rectHeight);

    AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, true);

    MoveWindow(hWnd, 50, 50, rect.right - rect.left, 
                             rect.bottom - rect.top, true);

    UpdateWindow(hWnd);
}

static void DrawInstructions(Graphics& graphics, float yStart)
{
    //Pen pen(Color(255, 0, 0, 255));
    //graphics.DrawLine(&pen, 0, 0, 200, 100);
    // Initialize arguments.
    float yIncr = 15.0f;

    Font font(L"Arial", 8);
    PointF origin(2.0f, yStart);
    StringFormat format;
    format.SetAlignment(StringAlignmentNear);
    SolidBrush blackBrush(Color(255, 0, 0, 0));
    SolidBrush redBrush(Color(255, 255, 0, 0));


    std::wstring open = L"Open File: o key";

    graphics.DrawString(open.c_str(), -1, &font, origin, &format, &redBrush);
    origin.Y += yIncr;

    std::wstring redraw = L"Re-draw: r key";

    graphics.DrawString(redraw.c_str(), -1, &font, origin, &format, &redBrush);
    origin.Y += yIncr;

    std::wstring samplerCtrls = L"Switch Sampler: left/right arrow keys";

    graphics.DrawString(samplerCtrls.c_str(), -1, &font, origin, &format, &redBrush);
    origin.Y += yIncr;

    std::wstring gridSize = L"Incr/Decr Grid Size: up/down arrow keys";

    graphics.DrawString(gridSize.c_str(), -1, &font, origin, &format, &redBrush);
    origin.Y += yIncr;

    std::wstring shadowRays = L"Incr/Decr Shadows Rays: + - (set to zero to disable)";  
    
    graphics.DrawString(shadowRays.c_str(), -1, &font, origin, &format, &redBrush);
    origin.Y += yIncr;
    
    std::wstring bRays = L"Incr/Decr Blur Rays: v key and b key (set to zero to disable)";

    graphics.DrawString(bRays.c_str(), -1, &font, origin, &format, &redBrush);
    origin.Y += yIncr;

    std::wstring reflF = L"Incr/Decr Reflection Blur Factor: ; ' (larger == more blurry)";

    graphics.DrawString(reflF.c_str(), -1, &font, origin, &format, &redBrush);
    origin.Y += yIncr;

    std::wstring transF = L"Incr/Decr Transmission Blur Factor: , . (larger == more blurry)";

    graphics.DrawString(transF.c_str(), -1, &font, origin, &format, &redBrush);
    origin.Y += yIncr;
    
    std::wstring tiles = L"Toggle Tile Rendering: t key";

    graphics.DrawString(tiles.c_str(), -1, &font, origin, &format, &redBrush);
    origin.Y += yIncr;

    std::wstring stree = L"Toggle Sphere Tree: s key";

    graphics.DrawString(stree.c_str(), -1, &font, origin, &format, &redBrush);
    origin.Y += yIncr;

    Pen pen(Color(255, 0, 0, 0));
    graphics.DrawLine(&pen, origin.X, origin.Y, origin.X + 400, origin.Y);
    origin.Y += yIncr*0.5f;

    std::wstring st = L"No Anti-Aliasing (disables soft shadow and blur)";
    if(g_imageSamplerType == 1)
        st = L"Uniform";
    else if(g_imageSamplerType == 2)
        st = L"Jittered";

    std::wstring sampleType = L"Sampler: ";
    sampleType += st;

    graphics.DrawString(sampleType.c_str(), -1, &font, origin, &format, &blackBrush);
    origin.Y += yIncr;

    if(g_imageSamplerType != 0)
    {
        std::wstringstream aaGridSize;
        aaGridSize << L"AA Grid Size: " << g_antiAliasingGridSize << "x" << g_antiAliasingGridSize;

        graphics.DrawString(aaGridSize.str().c_str(), -1, &font, origin, &format, &blackBrush);
        origin.Y += yIncr;

        std::wstringstream ssRays;
        ssRays << L"Soft Shadow Rays: " << g_softShadowRays;

        graphics.DrawString(ssRays.str().c_str(), -1, &font, origin,  &format, &blackBrush);
        origin.Y += yIncr;

        std::wstringstream blurRays;
        blurRays << L"Blur Rays: " << g_blurRays;

        graphics.DrawString(blurRays.str().c_str(), -1, &font, origin, &format, &blackBrush);
        origin.Y += yIncr;

        std::wstringstream reflBlurFactor;
        reflBlurFactor << L"Reflection Blur Factor: " << g_reflBlurFactor;

        graphics.DrawString(reflBlurFactor.str().c_str(), -1, &font, origin, &format, &blackBrush);
        origin.Y += yIncr;

        std::wstringstream transBlurFactor;
        transBlurFactor << L"Transmission Blur Factor: " << g_transBlurFactor;

        graphics.DrawString(transBlurFactor.str().c_str(), -1, &font, origin, &format, &blackBrush);
        origin.Y += yIncr;
    }

    std::wstringstream renTiles;
    renTiles << L"Render 32x32 Tiles: " << (g_renderTiles ? L"Yes" : L"No");

    graphics.DrawString(renTiles.str().c_str(), -1, &font, origin, &format, &blackBrush);
    origin.Y += yIncr;

    std::wstringstream useSTree;
    useSTree << L"Use Sphere Tree: " << (g_useSphereTree ? L"Yes" : L"No");

    graphics.DrawString(useSTree.str().c_str(), -1, &font, origin, &format, &blackBrush);
    origin.Y += yIncr;
}

VOID OnPaint(HDC hdc)
{
    if(g_spImage != NULL)
    {
        Graphics graphics(hdc);

        graphics.DrawImage(g_spImage->getBitmap(), 5, 5);

        DrawInstructions(graphics, g_spImage->verticalResolution() + 20.0f);
    }
    else
    {
        Graphics graphics(hdc);
        
        Color white(255, 255, 255, 255);
        graphics.Clear(white);

        DrawInstructions(graphics, 5.0f);
    }
}

static bool LoadFile(HWND hWnd, std::wstring& file)
{
	// Return buffer
    static wchar_t filename[4096];

    OPENFILENAME ofn;
    memset( &ofn, 0, sizeof(ofn) );

    ofn.lStructSize       = sizeof(ofn);
    ofn.hwndOwner         = hWnd;
    ofn.hInstance         = 0;
	ofn.lpstrFilter		  = L"Scene Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0\0";
    ofn.lpstrCustomFilter = NULL;
    ofn.nMaxCustFilter    = 0L;
    ofn.nFilterIndex      = 0L;
    ofn.lpstrFile         = filename;
    ofn.nMaxFile          = sizeof(filename);
    ofn.lpstrFileTitle    = 0;
    ofn.nMaxFileTitle     = 0;
	ofn.lpstrInitialDir   = 0;
    ofn.nFileOffset       = 0;
    ofn.nFileExtension    = 0;
    ofn.lpstrDefExt       = L"*.txt";
    ofn.lCustData         = 0;
	ofn.lpstrTitle		  = L"Select a scene file to open";
    ofn.Flags			  = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_NOCHANGEDIR | OFN_OVERWRITEPROMPT;

	if (GetOpenFileName(&ofn))
    {
        file = filename;
        return true;
    }

    return false;
}

char wide_to_narrow(WCHAR w)
{
    // simple typecast
    // works because UNICODE incorporates ASCII into itself
    return char(w);
}

static void GenerateImage(std::wstring file)
{
    RayCaster rayCaster;

    Random::Initialize((unsigned int)time(NULL));

    std::string inputFile(file.length(), 'a');
    std::transform(file.begin(), file.end(), inputFile.begin(), wide_to_narrow);

    int outputHorizRes = 400;
    if(rayCaster.loadScene(inputFile))
    {
        //AddHeightFieldToScene(rayCaster);
        //AddPolygonsToScene(rayCaster);
        //Sampler sampler;
        const Camera* pCamera = rayCaster.getCamera();
        if(pCamera != NULL)
        {
            Sampler* pSampler = NULL;
            UniformGridSampler gridSampler(g_antiAliasingGridSize, 
                                           g_antiAliasingGridSize);
            JitteredGridSampler jitteredSampler(g_antiAliasingGridSize, 
                                                g_antiAliasingGridSize);
            Sampler defaultSampler;

            if(g_imageSamplerType == 0)
            {
                pSampler = &defaultSampler;
                rayCaster.setDrawBlurredReflections(false);
                rayCaster.setDrawBlurredTransmissions(false);
                rayCaster.setDrawSoftShadows(false);
            }
            else
            {
                rayCaster.setDrawBlurredReflections(true, g_blurRays, g_reflBlurFactor);
                rayCaster.setDrawBlurredTransmissions(true, g_blurRays, g_transBlurFactor);
                rayCaster.setDrawSoftShadows(true, g_softShadowRays);
                if(g_imageSamplerType == 1)
                    pSampler = &gridSampler;
                else
                    pSampler = &jitteredSampler;
            }


            pSampler->setRenderTiles(g_renderTiles);
            //rayCaster.buildKDTree();
            if(g_useSphereTree)
                rayCaster.buildSphereTree();

            g_spImage = pSampler->generateImage(*pCamera, rayCaster, outputHorizRes, 10);
        }
    }
}

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, PSTR, INT iCmdShow)
{
    HWND                hWnd;
    MSG                 msg;
    WNDCLASS            wndClass;
    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR           gdiplusToken;
    // Initialize GDI+.
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    wndClass.style          = CS_HREDRAW | CS_VREDRAW;
    wndClass.lpfnWndProc    = WndProc;
    wndClass.cbClsExtra     = 0;
    wndClass.cbWndExtra     = 0;
    wndClass.hInstance      = hInstance;
    wndClass.hIcon          = LoadIcon(NULL, IDI_APPLICATION);
    wndClass.hCursor        = LoadCursor(NULL, IDC_ARROW);
    wndClass.hbrBackground  = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wndClass.lpszMenuName   = NULL;
    wndClass.lpszClassName  = TEXT("RayCast");

    RegisterClass(&wndClass);

    hWnd = CreateWindow(
      TEXT("RayCast"),   // window class name
      TEXT("Ray Cast"),  // window caption
      WS_OVERLAPPEDWINDOW,      // window style
      CW_USEDEFAULT,            // initial x position
      CW_USEDEFAULT,            // initial y position
      400,            // initial x size
      400,            // initial y size
      NULL,                     // parent window handle
      NULL,                     // window menu handle
      hInstance,                // program instance handle
      NULL);                    // creation parameters
      
    int rectWidthHeight = 400 + 10;

    ResizeWindow(hWnd, rectWidthHeight, rectWidthHeight);
    ShowWindow(hWnd, iCmdShow);

    while(GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    g_spImage = NULL;
    
    GdiplusShutdown(gdiplusToken);
    return (int)msg.wParam;
}  // WinMain

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, 
   WPARAM wParam, LPARAM lParam)
{
    HDC          hdc;
    PAINTSTRUCT  ps;

    switch(message)
    {
    case WM_PAINT:
        hdc = BeginPaint(hWnd, &ps);
        OnPaint(hdc);
        EndPaint(hWnd, &ps);
        return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    case WM_KEYDOWN:
        {
            switch(wParam)
            {
            case 'o':
            case 'O':
                {
                    if(LoadFile(hWnd, g_file))
                    {
                        g_spImage = NULL;
                        InvalidateRect(hWnd, NULL, TRUE);
                        UpdateWindow(hWnd);

                        GenerateImage(g_file);
                        InvalidateRect(hWnd, NULL, TRUE);
                        ResizeWindow(hWnd, 
                            g_spImage->horizontalResolution()+10, g_spImage->verticalResolution()+300);
                    }
                    break;
                }
            case 'r':
            case 'R':
                {
                    if(g_spImage.get() != NULL)
                    {
                        g_spImage = NULL;
                        InvalidateRect(hWnd, NULL, TRUE);
                        UpdateWindow(hWnd);
                        
                        GenerateImage(g_file);
                        InvalidateRect(hWnd, NULL, TRUE);
                        UpdateWindow(hWnd);
                    }
                    break;
                }
            case VK_LEFT:
                {
                    if(g_imageSamplerType != 0)
                        --g_imageSamplerType;

                    InvalidateRect(hWnd, NULL, TRUE);
                    UpdateWindow(hWnd);
                    break;
                }
            case VK_RIGHT:
                {
                    if(g_imageSamplerType != 2)
                        ++g_imageSamplerType;

                    InvalidateRect(hWnd, NULL, TRUE);
                    UpdateWindow(hWnd);
                    break;
                }
            case VK_UP:
                {
                    ++g_antiAliasingGridSize;

                    InvalidateRect(hWnd, NULL, TRUE);
                    UpdateWindow(hWnd);
                    break;
                }
            case VK_DOWN:
                {
                    if(g_antiAliasingGridSize != 0)
                        --g_antiAliasingGridSize;

                    InvalidateRect(hWnd, NULL, TRUE);
                    UpdateWindow(hWnd);
                    break;
                }
            case VK_OEM_PLUS:
                {
                    ++g_softShadowRays;

                    InvalidateRect(hWnd, NULL, TRUE);
                    UpdateWindow(hWnd);
                    break;
                }
            case VK_OEM_MINUS:
                {
                    if(g_softShadowRays != 0)
                        --g_softShadowRays;

                    InvalidateRect(hWnd, NULL, TRUE);
                    UpdateWindow(hWnd);
                    break;
                }
            case 'b':
            case 'B':
                {
                    ++g_blurRays;

                    InvalidateRect(hWnd, NULL, TRUE);
                    UpdateWindow(hWnd);
                    break;
                }
            case 'v':
            case 'V':
                {
                    if(g_blurRays != 0)
                        --g_blurRays;

                    InvalidateRect(hWnd, NULL, TRUE);
                    UpdateWindow(hWnd);
                    break;
                }
            case VK_OEM_COMMA:
                {
                    g_transBlurFactor-=0.05f;
                    if(g_transBlurFactor < 0.0f)
                        g_transBlurFactor = 0.0f;

                    InvalidateRect(hWnd, NULL, TRUE);
                    UpdateWindow(hWnd);
                    break;
                }
            case VK_OEM_PERIOD:
                {
                    g_transBlurFactor+=0.05f;

                    InvalidateRect(hWnd, NULL, TRUE);
                    UpdateWindow(hWnd);
                    break;
                }
            case VK_OEM_1: //; semi colon key
                {
                    g_reflBlurFactor-=0.05f;
                    if(g_reflBlurFactor < 0.0f)
                        g_reflBlurFactor = 0.0f;

                    InvalidateRect(hWnd, NULL, TRUE);
                    UpdateWindow(hWnd);
                    break;
                }
            case VK_OEM_7: //' single quote key
                {                    
                    g_reflBlurFactor+=0.05f;

                    InvalidateRect(hWnd, NULL, TRUE);
                    UpdateWindow(hWnd);
                    break;
                }
            case 't':
            case 'T':
                {
                    g_renderTiles = !g_renderTiles;

                    InvalidateRect(hWnd, NULL, TRUE);
                    UpdateWindow(hWnd);
                    break;
                }
            case 's':
            case 'S':
                {
                    g_useSphereTree = !g_useSphereTree;

                    InvalidateRect(hWnd, NULL, TRUE);
                    UpdateWindow(hWnd);
                    break;
                }
            default:
                break;
            }
        }
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
} // WndProc