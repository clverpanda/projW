// projW.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "projW.h"

#include<windows.h>
#include<time.h>

#define MAX_LOADSTRING 100


#define KEY_NORMAL 0
#define KEY_DOWN  VK_DOWN
#define KEY_UP    VK_UP
#define KEY_LEFT  VK_LEFT
#define KEY_RIGHT VK_RIGHT
#define KEY_PAUSE VK_SPACE
#define ID_START_BTN  1
#define ID_END_BTN    2
#define ID_BEATIMER 101
#define KEY_W 87
#define KEY_A 65
#define KEY_S 83
#define KEY_D 68



// 全局变量: 
HINSTANCE hInst;                                // 当前实例
WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名

BOOL Compare();//判定是否碰撞
BOOL TransBlock(int);//转换函数(包含变形，加速，左右移动。
BOOL EchoBeat(int);

void HandleTable();//得分时处理游戏区域
				   //定义24种方块
POINT ptBlock[28][4] = { -1, 0, 0, 0, 1, 0, 0, 1, -1, 0, 0, 0, 0, -1, 0, 1, -1, 0, 1, 0, 0, 0, 0, -1, 0, 0, 1, 0, 0, 1, 0, -1,//山形
-2, 0, -1, 0, 0, 0, 1, 0, 0, 2, 0, 1, 0, 0, 0, -1, -2, 0, -1, 0, 0, 0, 1, 0, 0, 2, 0, 1, 0, 0, 0, -1,//一字形
-1, 0, 0, 0, 0, 1, 1, 1, -1, 1, -1, 0, 0, 0, 0, -1, -1, 0, 0, 0, 0, 1, 1, 1, -1, 1, -1, 0, 0, 0, 0, -1,//右折
-1, 1, 0, 1, 0, 0, 1, 0, 0, -1, 0, 0, 1, 0, 1, 1, -1, 1, 0, 1, 0, 0, 1, 0, 0, -1, 0, 0, 1, 0, 1, 1,//左折
0, 1, 0, 0, 1, 0, 2, 0, -1, 0, 0, 0, 0, 1, 0, 2, -2, 0, -1, 0, 0, 0, 0, -1, 0, -2, 0, -1, 0, 0, 1, 0,//左L
-2, 0, -1, 0, 0, 0, 0, -1, -1, 0, 0, 0, 0, 1, 0, 2, 0, 1, 0, 0, 1, 0, 2, 0, 0, -2, 0, -1, 0, 0, 1, 0,//又L
-1, 0, 0, 0, -1, -1, 0, -1, -1, 0, 0, 0, -1, -1, 0, -1, -1, 0, 0, 0, -1, -1, 0, -1, -1, 0, 0, 0, -1, -1, 0, -1 };//田

 //全局变量主要用于全局记录
BOOL bStateForNext, bGameOver, bBlockState[10][24], bPause, bGameState, MisFlag;
POINT ptCurrentPos, ptCurrentShape, ptNextShape;
int iScore,MissCount,Beat;



// 此代码模块中包含的函数的前向声明: 
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    RecordDialog(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 在此放置代码。

    // 初始化全局字符串
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_PROJW, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 执行应用程序初始化: 
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_PROJW));

    MSG msg;

    // 主消息循环: 
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  函数: MyRegisterClass()
//
//  目的: 注册窗口类。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PROJW));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_PROJW);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   函数: InitInstance(HINSTANCE, int)
//
//   目的: 保存实例句柄并创建主窗口
//
//   注释: 
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 将实例句柄存储在全局变量中

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的:    处理主窗口的消息。
//
//  WM_COMMAND  - 处理应用程序菜单
//  WM_PAINT    - 绘制主窗口
//  WM_DESTROY  - 发送退出消息并返回
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int i, j, lBeat;
	PAINTSTRUCT ps;
	HDC hdc;
	HBITMAP hBitmap, hOldBitmap;
	HBITMAP BitA, BitW, BitS, BitD, BitA_p, BitW_p, BitS_p, BitD_p;
	HBRUSH hOldBrush;
	RECT rect;
	TCHAR wcScore[20],wcMiss[10];
	static HWND hStartBtn, hEndBtn;
	static int cxScreen, cyScreen, cxClient, cyClient, cxChar, cyChar;

    switch (message)
    {
	case WM_CREATE:
	    { 
			//创建按钮
			cyChar = HIWORD(GetDialogBaseUnits());
			cxChar = LOWORD(GetDialogBaseUnits());
			hStartBtn = CreateWindow(TEXT("button"), TEXT("Start"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 402 - 8 * cxChar, 250 - cyChar, 14 * cxChar, 2 * cyChar, hWnd, (HMENU)ID_START_BTN, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
			hEndBtn = CreateWindow(TEXT("button"), TEXT("Stop"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 402 - 8 * cxChar, 250 + 2 * cyChar, 14 * cxChar, 2 * cyChar, hWnd, (HMENU)ID_END_BTN, ((LPCREATESTRUCT)lParam)->hInstance, NULL);

			//初始化窗口
			srand(time(0));
			bGameState = FALSE;
			cxScreen = GetSystemMetrics(SM_CXSCREEN);
			cyScreen = GetSystemMetrics(SM_CYSCREEN);
			MoveWindow(hWnd, cxScreen / 2 - 350, cyScreen / 2 - 350, 800, 550, TRUE);
			GetClientRect(hWnd, &rect);
			cxClient = rect.right - rect.left;
			cyClient = rect.bottom - rect.top;
	    }
		break;
	/*case WM_SIZE:
	    {
			cxClient = LOWORD(lParam);
			cyClient = HIWORD(lParam);
	    }
	    break;*/
    case WM_COMMAND:
        {
			SetFocus(hWnd);
            int wmId = LOWORD(wParam);
            // 分析菜单选择: 
            switch (wmId)
            {
			case ID_START:
				//初始化参数，启动游戏
				iScore = 0;
				MissCount = 0;
				bGameState = TRUE;
				bPause = FALSE;
				bGameOver = FALSE;
				bStateForNext = TRUE;
				ptCurrentShape.x = rand() % 7;
				ptCurrentShape.y = rand() % 4;
				ptNextShape.x = rand() % 7;
				ptNextShape.y = rand() % 4;
				ptCurrentPos.x = 5;
				ptCurrentPos.y = 21;
				for (i = 0; i < 10; i++)
				{
					for (j = 0; j < 24; j++)
					{
						bBlockState[i][j] = FALSE;
					}
				}
				for (i = 0; i < 4; i++)
				{
					bBlockState[ptCurrentPos.x + ptBlock[ptCurrentShape.x * 4 + ptCurrentShape.y][i].x][ptCurrentPos.y + ptBlock[ptCurrentShape.x * 4 + ptCurrentShape.y][i].y] = TRUE;
				}
				SetTimer(hWnd, 1, 1000, NULL);
				SetTimer(hWnd, ID_BEATIMER, 750, NULL);
				EnableWindow(hStartBtn, FALSE);
				EnableWindow(hEndBtn, TRUE);
				return 0;
			case ID_STOP:
				KillTimer(hWnd, 1);
				KillTimer(hWnd, ID_BEATIMER);
				EnableWindow(hStartBtn, TRUE);
				EnableWindow(hEndBtn, FALSE);
				return 0;
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
			case ID_START_BTN:
				//初始化参数，启动游戏
				iScore = 0;
				MissCount = 0;
				bGameState = TRUE;
				bPause = FALSE;
				bGameOver = FALSE;
				bStateForNext = TRUE;
				ptCurrentShape.x = rand() % 7;
				ptCurrentShape.y = rand() % 4;
				ptNextShape.x = rand() % 7;
				ptNextShape.y = rand() % 4;
				ptCurrentPos.x = 5;
				ptCurrentPos.y = 21;
				for (i = 0; i < 10; i++)
				{
					for (j = 0; j < 24; j++)
					{
						bBlockState[i][j] = FALSE;
					}
				}
				for (i = 0; i < 4; i++)
				{
					bBlockState[ptCurrentPos.x + ptBlock[ptCurrentShape.x * 4 + ptCurrentShape.y][i].x][ptCurrentPos.y + ptBlock[ptCurrentShape.x * 4 + ptCurrentShape.y][i].y] = TRUE;
				}
				SetTimer(hWnd, 1, 1000, NULL);
				SetTimer(hWnd, ID_BEATIMER, 750, NULL);
				MisFlag = FALSE;
				EnableWindow(hStartBtn, FALSE);
				EnableWindow(hEndBtn, TRUE);
				return 0;
				//结束计时，结束游戏 
			case ID_END_BTN:
				KillTimer(hWnd, 1);
				KillTimer(hWnd, ID_BEATIMER);
				EnableWindow(hStartBtn, TRUE);
				EnableWindow(hEndBtn, FALSE);
				return 0;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;



    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: 在此处添加使用 hdc 的任何绘图代码...
			HDC hMemDC = CreateCompatibleDC(hdc);
			HDC hMemDC1 = CreateCompatibleDC(hdc);
			hBitmap = LoadBitmap(hInst, (LPCWSTR)IDB_BITMAP1);
			hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBitmap);
			FillRect(hMemDC, &rect, (HBRUSH)(GetStockObject(WHITE_BRUSH)));


			BitA = LoadBitmap(hInst, (LPCWSTR)IDB_BITA);
			BitW = LoadBitmap(hInst, (LPCWSTR)IDB_BITW);
			BitS = LoadBitmap(hInst, (LPCWSTR)IDB_BITS);
			BitD = LoadBitmap(hInst, (LPCWSTR)IDB_BITD);
			BitA_p = LoadBitmap(hInst, (LPCWSTR)IDB_BITA_P);
			BitW_p = LoadBitmap(hInst, (LPCWSTR)IDB_BITW_P);
			BitS_p = LoadBitmap(hInst, (LPCWSTR)IDB_BITS_P);
			BitD_p = LoadBitmap(hInst, (LPCWSTR)IDB_BITD_P);

			//SelectObject(hMemDC, GetStockObject(SYSTEM_FIXED_FONT));
			TextOut(hMemDC, 260, 125, TEXT("Next Block:"), 11);
			wsprintf(wcScore, TEXT("Score:%d"), iScore);
		    TextOut(hMemDC, 360, 200, wcScore, lstrlen(wcScore));
			//绘制边框
			SelectObject(hMemDC, GetStockObject(BLACK_PEN));
			Rectangle(hMemDC, 1, cyClient - 3 - 25 * 20, 2 + 10 * 25 + 1, cyClient - 1);
			//绘制提示区域
			
			SelectObject(hMemDC, GetStockObject(WHITE_PEN));
			hOldBrush = (HBRUSH)SelectObject(hMemDC, CreateSolidBrush(RGB(0, 0, 0)));
			if (bGameState)
			{
				for (i = 0; i < 4; i++)
					Rectangle(hMemDC, 420 + 25 * ptBlock[ptNextShape.x * 4 + ptNextShape.y][i].x,
						150 - 25 + 25 * ptBlock[ptNextShape.x * 4 + ptNextShape.y][i].y,
						420 + 25 + 25 * ptBlock[ptNextShape.x * 4 + ptNextShape.y][i].x,
						150 + 25 * ptBlock[ptNextShape.x * 4 + ptNextShape.y][i].y);
			}
			//绘制游戏区域
			for (i = 0; i < 10; i++)
				for (j = 0; j < 20; j++)
					if (bBlockState[i][j])
						Rectangle(hMemDC, 2 + 25 * i, cyClient - 2 - 25 * (j + 1), 2 + 25 * (i + 1), cyClient - 2 - 25 * j);
			//MISS
			wsprintf(wcMiss, TEXT("Miss:%d"), MissCount);
			TextOut(hMemDC, 525, 125, wcMiss, lstrlen(wcMiss));
			//绘制BEAT部分
			SelectObject(hMemDC1, BitA);
			BitBlt(hMemDC, 500, 300, cxClient, cyClient, hMemDC1, 0, 0, SRCCOPY);
			SelectObject(hMemDC1, BitS);
			BitBlt(hMemDC, 575, 300, cxClient, cyClient, hMemDC1, 0, 0, SRCCOPY);
			SelectObject(hMemDC1, BitD);
			BitBlt(hMemDC, 650, 300, cxClient, cyClient, hMemDC1, 0, 0, SRCCOPY);
			SelectObject(hMemDC1, BitW);
			BitBlt(hMemDC, 575, 225, cxClient, cyClient, hMemDC1, 0, 0, SRCCOPY);



			switch (Beat)
			{
			case 1:
				SelectObject(hMemDC1, BitW_p);
				BitBlt(hMemDC, 575, 225, cxClient, cyClient, hMemDC1, 0, 0, SRCCOPY);
				break;
			case 2:
				SelectObject(hMemDC1, BitA_p);
				BitBlt(hMemDC, 500, 300, cxClient, cyClient, hMemDC1, 0, 0, SRCCOPY);
				break;
			case 3:
				SelectObject(hMemDC1, BitS_p);
				BitBlt(hMemDC, 575, 300, cxClient, cyClient, hMemDC1, 0, 0, SRCCOPY);
				break;
			case 4:
				SelectObject(hMemDC1, BitD_p);
				BitBlt(hMemDC, 650, 300, cxClient, cyClient, hMemDC1, 0, 0, SRCCOPY);
				break;
			default:
				break;
			}


			BitBlt(hdc, 0, 0, cxClient, cyClient, hMemDC, 0, 0, SRCCOPY);
			SelectObject(hMemDC, hOldBrush);
			SelectObject(hMemDC, hOldBitmap);
			DeleteObject(hBitmap);
			DeleteObject(BitD_p);
			DeleteObject(BitS_p);
			DeleteObject(BitW_p);
			DeleteObject(BitA_p);
			DeleteObject(BitD);
			DeleteObject(BitS);
			DeleteObject(BitW);
			DeleteObject(BitA);
			DeleteDC(hMemDC);
            EndPaint(hWnd, &ps);
        }
        break;

	case WM_TIMER:
		switch (wParam)
		{
		case ID_BEATIMER:
			lBeat=Beat;
			Beat = (rand() % 4) + 1;//保证每次的Beat不同
			if (Beat == lBeat)
				Beat = (Beat + 1) % 4 + 1;
			if (MisFlag == TRUE)
				MissCount++;
			MisFlag = TRUE;
			if (MissCount >= 10)
				bGameOver = TRUE;
			break;
		case 1:
			if (bGameOver)
			{
				KillTimer(hWnd, 1);
				KillTimer(hWnd, ID_BEATIMER);
				DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG1), hWnd, RecordDialog);
				return 0;
			}
			TransBlock(KEY_NORMAL);
		}
		InvalidateRect(hWnd, NULL, FALSE);
		return 0;
	case WM_KEYDOWN:
		switch (wParam)
		{
		case KEY_LEFT:
			TransBlock(wParam);
			break;
		case KEY_RIGHT:
			TransBlock(wParam);
			break;
		case KEY_UP:
			TransBlock(wParam);
			break;
		case KEY_DOWN:
			TransBlock(wParam);
			break;
		case KEY_W:
			EchoBeat(wParam);
			break;
		case KEY_A:
			EchoBeat(wParam);
			break;
		case KEY_S:
			EchoBeat(wParam);
			break;
		case KEY_D:
			EchoBeat(wParam);
			break;
		default:
			TransBlock(wParam);
			break;
		}
		//TransBlock(wParam);
		//EchoBeat(wParam);
		InvalidateRect(hWnd, NULL, FALSE);
		return 0;

    case WM_DESTROY:
		KillTimer(hWnd, 1);
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// “关于”框的消息处理程序。
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

//最高纪录对话框消息处理程序
INT_PTR CALLBACK RecordDialog(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

//判断节拍是否正确
BOOL EchoBeat(int nIndex)
{
	switch (nIndex)
	{
	case KEY_W:
		if (MisFlag == TRUE)
		{
			if (Beat == 1)
				iScore++;
			else
				MissCount++;
			MisFlag = FALSE;
		}
		break;
	case KEY_A:
		if (MisFlag == TRUE)
		{
			if (Beat == 2)
				iScore++;
			else
				MissCount++;
			MisFlag = FALSE;
		}
		break;
	case KEY_S:
		if (MisFlag == TRUE)
		{
			if (Beat == 3)
				iScore++;
			else
				MissCount++;
			MisFlag = FALSE;
		}
		break;
	case KEY_D:
		if (MisFlag == TRUE)
		{
			if (Beat == 4)
				iScore++;
			else
				MissCount++;
			MisFlag = FALSE;
		}
		break;
	default:
		
		break;
	}
	return 0;
}




//变换判定，如果能变换则变换，不能则不变。
BOOL TransBlock(int nIndex)
{
	POINT ptTempCurrentPos = ptCurrentPos;
	POINT ptTempCurrentShape = ptCurrentShape;
	int i;
	if (bPause&&nIndex == KEY_PAUSE)
	{
		bPause = FALSE;
		return FALSE;
	}
	if (bPause) return FALSE;
	for (i = 0; i < 4; i++)
	{
		bBlockState[ptCurrentPos.x + ptBlock[ptCurrentShape.x * 4 + ptCurrentShape.y][i].x][ptCurrentPos.y + ptBlock[ptCurrentShape.x * 4 + ptCurrentShape.y][i].y] = FALSE;
	}
	switch (nIndex)
	{
		//左移
	case KEY_LEFT:
		ptCurrentPos.x--;
		if (Compare())
		{
			for (i = 0; i < 4; i++)
			{
				bBlockState[ptCurrentPos.x + ptBlock[ptCurrentShape.x * 4 + ptCurrentShape.y][i].x][ptCurrentPos.y + ptBlock[ptCurrentShape.x * 4 + ptCurrentShape.y][i].y] = TRUE;
			}
			return 0;
		}
		ptCurrentPos.x++;
		for (i = 0; i < 4; i++)
		{
			bBlockState[ptCurrentPos.x + ptBlock[ptCurrentShape.x * 4 + ptCurrentShape.y][i].x][ptCurrentPos.y + ptBlock[ptCurrentShape.x * 4 + ptCurrentShape.y][i].y] = TRUE;
		}
		return 0;
		//右移
	case KEY_RIGHT:
		ptCurrentPos.x++;
		if (Compare())
		{
			for (i = 0; i < 4; i++)
			{
				bBlockState[ptCurrentPos.x + ptBlock[ptCurrentShape.x * 4 + ptCurrentShape.y][i].x][ptCurrentPos.y + ptBlock[ptCurrentShape.x * 4 + ptCurrentShape.y][i].y] = TRUE;
			}
			return 0;
		}
		ptCurrentPos.x--;
		for (i = 0; i < 4; i++)
		{
			bBlockState[ptCurrentPos.x + ptBlock[ptCurrentShape.x * 4 + ptCurrentShape.y][i].x][ptCurrentPos.y + ptBlock[ptCurrentShape.x * 4 + ptCurrentShape.y][i].y] = TRUE;
		}
		return 0;
		//加速和正常下降
	case KEY_DOWN:
	case KEY_NORMAL:
		ptCurrentPos.y--;
		if (!Compare())
		{
			//遇到阻碍，进行判断。
			//恢复方块
			ptCurrentPos.y++;
			for (i = 0; i < 4; i++)
			{
				bBlockState[ptCurrentPos.x + ptBlock[ptCurrentShape.x * 4 + ptCurrentShape.y][i].x][ptCurrentPos.y + ptBlock[ptCurrentShape.x * 4 + ptCurrentShape.y][i].y] = TRUE;
			}
			//游戏结束
			for (i = 0; i < 10; i++)
				if (bBlockState[i][19]) { bGameOver = TRUE; return 0; }
			//得分消行
			HandleTable();
			//启动下一个
			ptCurrentPos.x = 5;
			ptCurrentPos.y = 21;
			ptCurrentShape = ptNextShape;
			ptNextShape.x = rand() % 7;
			ptNextShape.y = rand() % 4;
			return 0;
		}
		for (i = 0; i < 4; i++)
		{
			bBlockState[ptCurrentPos.x + ptBlock[ptCurrentShape.x * 4 + ptCurrentShape.y][i].x][ptCurrentPos.y + ptBlock[ptCurrentShape.x * 4 + ptCurrentShape.y][i].y] = TRUE;
		}
		return 0;
		//变形
	case KEY_UP:
		ptCurrentShape.y = ptCurrentShape.y + 1<4 ? ptCurrentShape.y + 1 : 0;
		if (!Compare())
		{
			ptCurrentShape.y = ptTempCurrentShape.y;
			for (i = 0; i < 4; i++)
			{
				bBlockState[ptCurrentPos.x + ptBlock[ptCurrentShape.x * 4 + ptCurrentShape.y][i].x][ptCurrentPos.y + ptBlock[ptCurrentShape.x * 4 + ptCurrentShape.y][i].y] = TRUE;
			}
			return 0;
		}
		for (i = 0; i < 4; i++)
		{
			bBlockState[ptCurrentPos.x + ptBlock[ptCurrentShape.x * 4 + ptCurrentShape.y][i].x][ptCurrentPos.y + ptBlock[ptCurrentShape.x * 4 + ptCurrentShape.y][i].y] = TRUE;
		}
		return 0;
		//暂停
	case KEY_PAUSE:
		for (i = 0; i < 4; i++)
		{
			bBlockState[ptCurrentPos.x + ptBlock[ptCurrentShape.x * 4 + ptCurrentShape.y][i].x][ptCurrentPos.y + ptBlock[ptCurrentShape.x * 4 + ptCurrentShape.y][i].y] = TRUE;
		}
		bPause = bPause ? FALSE : TRUE;
		return 0;
	}
	return 0;
}
//比较判定是否能继续变换
BOOL Compare()
{
	int i;
	for (i = 0; i < 4; i++)
		if (ptCurrentPos.x + ptBlock[ptCurrentShape.x * 4 + ptCurrentShape.y][i].x < 0 || ptCurrentPos.x + ptBlock[ptCurrentShape.x * 4 + ptCurrentShape.y][i].x>9 ||
			ptCurrentPos.y + ptBlock[ptCurrentShape.x * 4 + ptCurrentShape.y][i].y < 0)
			return FALSE;
		else if (bBlockState[ptCurrentPos.x + ptBlock[ptCurrentShape.x * 4 + ptCurrentShape.y][i].x][ptCurrentPos.y + ptBlock[ptCurrentShape.x * 4 + ptCurrentShape.y][i].y])
			return FALSE;
		else continue;
		return TRUE;
}
//得分处理函数
void HandleTable()
{
	int i, j, k = 0;
	BOOL tempState;
	BOOL bTempBlockState[10][24];
	for (i = 0; i < 10; i++)
		for (j = 0; j < 24; j++)
			bTempBlockState[i][j] = FALSE;
	for (j = 0; j < 24; j++)
	{
		tempState = TRUE;
		for (i = 0; i < 10; i++) { ; if (!bBlockState[i][j]) { tempState = FALSE; break; } }
		if (!tempState)
		{
			for (i = 0; i < 10; i++) { bTempBlockState[i][k] = bBlockState[i][j]; }
			k++;
		}
		else iScore+=10;
	}
	for (i = 0; i < 10; i++)
		for (j = 0; j < 24; j++)
			bBlockState[i][j] = bTempBlockState[i][j];
	return;
}