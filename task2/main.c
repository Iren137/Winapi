#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <math.h>

LRESULT WndProc(HWND hWindow, UINT messageCode, WPARAM wParam, LPARAM lParam);

HINSTANCE ghCurrentInstance;

static POINT ballState;

HBITMAP bitmap;

INT WinMain(HINSTANCE haCurrentInstance, HINSTANCE haParentInstance,
            LPSTR lpstraCommandLine, int naCmdShow) {
  ghCurrentInstance = haCurrentInstance;

  const char WINDOW_CLASS_NAME[] = "ClassNameHere";
  WNDCLASS windowClass = {};
  windowClass.lpfnWndProc = WndProc;
  windowClass.hInstance = ghCurrentInstance;
  windowClass.lpszClassName = WINDOW_CLASS_NAME;
  windowClass.hCursor = LoadCursor(0, IDC_ARROW);
  windowClass.hbrBackground = GetStockBrush(WHITE_BRUSH);
  RegisterClass(&windowClass);

  bitmap = LoadBitmap(GetModuleHandle(0), MAKEINTRESOURCE(1));

  HWND hWindow = CreateWindowEx(
      0,
      WINDOW_CLASS_NAME,
      "Ball Jump",
      WS_CAPTION | WS_SYSMENU
          | WS_MINIMIZEBOX & ~WS_THICKFRAME,
      CW_USEDEFAULT,
      CW_USEDEFAULT,
      500,
      450,
      NULL,
      NULL,
      ghCurrentInstance,
      NULL
  );
  ballState.x = 20;
  ballState.y = 300;
  if (hWindow == NULL) {
    return 1;
  }

  ShowWindow(hWindow, naCmdShow);

  MSG message = {};
  while (GetMessage(&message, NULL, 0, 0)) {
    TranslateMessage(&message);
    DispatchMessage(&message);
  }

  return (int) message.wParam;
}

#define WM_SUSPEND_ANIMATION (WM_USER+1)
#define WM_RESUME_ANIMATION (WM_USER+2)

#define STEP_TIMER_ID 1
#define ANIMATION_TIMER_ID 2

void SuspendAnimation(
    HWND hWindow, UINT messageCode, UINT_PTR idTimer, DWORD nTicks) {
  SendMessage(hWindow, WM_SUSPEND_ANIMATION, 0, 0);
  KillTimer(hWindow, idTimer);
}

void Paint(HWND hWindow, LPPAINTSTRUCT lpPaintStruct) {

  HINSTANCE hInstance;

  RECT rcClientArea;
  HDC hdcBuffer;
  HBITMAP hbmBuffer, hbmOld;

  GetClientRect(hWindow, &rcClientArea);

  hdcBuffer = CreateCompatibleDC(lpPaintStruct->hdc);

  hbmBuffer = CreateCompatibleBitmap(
      lpPaintStruct->hdc,
      rcClientArea.right - rcClientArea.left,
      rcClientArea.bottom - rcClientArea.top);

  hbmOld = SelectObject(hdcBuffer, hbmBuffer);

  HBRUSH hbrBkGnd = CreatePatternBrush(bitmap);
  FillRect(hdcBuffer, &rcClientArea, hbrBkGnd);
  DeleteObject(hbrBkGnd);

  HBRUSH hBR = CreateHatchBrush(BS_3STATE, RGB(255, 165, 0));
  HPEN hPen = CreatePen(PS_DASHDOTDOT, 1, RGB(0, 0, 0));
  SelectBrush(hdcBuffer, hBR);
  SelectPen(hdcBuffer, hPen);
  Ellipse(hdcBuffer,
          ballState.x,
          ballState.y,
          ballState.x + 60,
          ballState.y + 60);

  BitBlt(lpPaintStruct->hdc,
         rcClientArea.left,
         rcClientArea.top,
         rcClientArea.right - rcClientArea.left,
         rcClientArea.bottom - rcClientArea.top,
         hdcBuffer,
         0,
         0,
         SRCCOPY);

  SelectObject(hdcBuffer, hbmOld);
  DeleteObject(hbmBuffer);
  DeleteDC(hdcBuffer);
}

LRESULT WndProc(HWND hWindow, UINT messageCode, WPARAM wParam, LPARAM lParam) {
  static HWND hSuspendButton;
  static HWND hResumeButton;

  switch (messageCode) {
    case WM_CREATE: {
      hResumeButton = CreateWindow(
          "button", "Start animation",
          WS_CHILD | WS_VISIBLE | WS_BORDER | BS_PUSHBUTTON,
          100, 30, 120, 30, hWindow, NULL, ghCurrentInstance, NULL);
      hSuspendButton = CreateWindow(
          "button", "Stop animation",
          WS_CHILD | WS_VISIBLE | WS_BORDER | BS_PUSHBUTTON,
          300, 30, 120, 30, hWindow, NULL, ghCurrentInstance, NULL);
      break;
    }
    case WM_DESTROY: {
      PostQuitMessage(0);
      break;
    }
    case WM_PAINT: {
      PAINTSTRUCT paintStruct;
      HDC hDC = BeginPaint(hWindow, &paintStruct);

      Paint(hWindow, &paintStruct);
      EndPaint(hWindow, &paintStruct);
      break;
    }
    case WM_COMMAND: {
      if ((HWND) (lParam) == hSuspendButton) {
        SendMessage(hWindow, WM_SUSPEND_ANIMATION, 0, 0);
      } else if ((HWND) (lParam) == hResumeButton) {
        SendMessage(hWindow, WM_RESUME_ANIMATION, 0, 0);
      } else {
        return DefWindowProc(hWindow, messageCode, wParam, lParam);
      }
      break;
    }
    case WM_RESUME_ANIMATION: {
      SetTimer(hWindow, STEP_TIMER_ID, 40, NULL);
      SetTimer(hWindow, ANIMATION_TIMER_ID, 12200, SuspendAnimation);
      break;
    }
    case WM_SUSPEND_ANIMATION: {
      KillTimer(hWindow, STEP_TIMER_ID);
      KillTimer(hWindow, ANIMATION_TIMER_ID);
      break;
    }
    case WM_TIMER: {
      int new_x = (ballState.x < 500) ? ballState.x : -2;
      ballState.x = new_x + 2;
      ballState.y = 300 + 40 * sin(ballState.x / 4);
      InvalidateRect(hWindow, NULL, FALSE);
      break;
    }
    default: {
      return DefWindowProc(hWindow, messageCode, wParam, lParam);
    }
  }
  return 0;
}
