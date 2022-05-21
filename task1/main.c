#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>

#define IDM_QUIT 3
#define IDM_TOGGLE_OPTIONS 5
#define IDM_RADIO_1 6
#define IDM_RADIO_2 7
#define IDM_RADIO_3 8
#define IDM_RADIO_4 9
#define IDM_RADIO_5 10
#define IDM_RADIO_6 11
#define IDM_RADIO_7 12
#define IDM_RADIO_8 13
#define IDM_DLG_COLOR 14
#define IDM_DLG_HINTS 15

#define SB_PID_RADIO_STATE 0
#define SB_PID_RADIO_CHOICE 1

#define RECTANGLE 1
#define ELLIPSE 2
#define CIRCLE 3
#define SECTOR 4
#define SEGMENT 5
#define POLYLINE 6
#define POLYGON 7
#define LINE 8

static int figure = 9;

HMENU NewMainMenu(HMENU* lphRadioSubMenu) {

  HMENU hRadioSubMenu = CreateMenu();
  AppendMenu(hRadioSubMenu, MF_STRING, IDM_RADIO_1, "&Rectangle");
  AppendMenu(hRadioSubMenu, MF_STRING, IDM_RADIO_2, "&Ellipse");
  AppendMenu(hRadioSubMenu, MF_STRING, IDM_RADIO_3, "&Circle");
  AppendMenu(hRadioSubMenu, MF_STRING, IDM_RADIO_4, "&Sector");
  AppendMenu(hRadioSubMenu, MF_STRING, IDM_RADIO_5, "&Segment");
  AppendMenu(hRadioSubMenu, MF_STRING, IDM_RADIO_6, "&Polyline");
  AppendMenu(hRadioSubMenu, MF_STRING, IDM_RADIO_7, "&Polygon");
  AppendMenu(hRadioSubMenu, MF_STRING, IDM_RADIO_8, "&Line");
  if (lphRadioSubMenu != NULL) {
    *lphRadioSubMenu = hRadioSubMenu;
  }

  HMENU hSettingsSubMenu = CreateMenu();
  AppendMenu(hSettingsSubMenu, MF_STRING, IDM_DLG_HINTS, "&Hints");
  AppendMenu(hSettingsSubMenu, MF_SEPARATOR, 0, NULL);
  AppendMenu(
      hSettingsSubMenu, MF_STRING, IDM_TOGGLE_OPTIONS, "&Enable choise");
  CheckMenuItem(hSettingsSubMenu, IDM_TOGGLE_OPTIONS, MF_CHECKED);
  AppendMenu(hSettingsSubMenu, MF_POPUP, (UINT_PTR) hRadioSubMenu, "&Figures");

  HMENU hMenuBar = CreateMenu();
  AppendMenu(hMenuBar, MF_STRING, IDM_DLG_COLOR, "&Choose Color");
  AppendMenu(hMenuBar, MF_POPUP, (UINT_PTR) hSettingsSubMenu, "&Settings");
  AppendMenu(hMenuBar, MF_STRING, IDM_QUIT, "&Quit");
  return hMenuBar;
}

LRESULT WndProc(HWND hWindow, UINT messageCode, WPARAM wParam, LPARAM lParam) {
  static HMENU hMainMenu = NULL;
  static HMENU hMainMenuRadioGroup = NULL;
  static HWND hStatusBar = NULL;
  static HWND hText = NULL;

  static HPEN pen_handle = NULL;

  static int sx = 0;
  static int sy = 0;

  static CHOOSECOLOR ccDialogData;
  static COLORREF crCustomColors[16];

  static DWORD dwBufferSize = 0;
  static LPSTR lpstrBuffer = NULL;

  switch (messageCode) {
    case WM_CREATE: {
      hMainMenu = NewMainMenu(&hMainMenuRadioGroup);
      SetMenu(hWindow, hMainMenu);
      hStatusBar = CreateWindow(
          STATUSCLASSNAME, NULL, WS_CHILD | WS_VISIBLE,
          0, 0, 0, 0, hWindow, (HMENU) 1, GetModuleHandle(NULL), NULL);

      const int iStatusWidths[] = {100, 300, -1};
      SendMessage(hStatusBar, SB_SETPARTS, 3, (LPARAM) iStatusWidths);
      SendMessage(hStatusBar, SB_SETTEXT, SB_PID_RADIO_STATE,
                  (LPARAM) "Choose enabled");

      ZeroMemory(&ccDialogData, sizeof(ccDialogData));
      ccDialogData.lStructSize = sizeof(ccDialogData);
      ccDialogData.hwndOwner = hWindow;
      ccDialogData.lpCustColors = (LPDWORD) crCustomColors;
      ccDialogData.rgbResult = RGB(0, 0, 0);
      ccDialogData.Flags = CC_FULLOPEN | CC_RGBINIT;
      break;
    }
    case WM_DESTROY: {
      if (lpstrBuffer != NULL) {
        HeapFree(GetProcessHeap(), 0, lpstrBuffer);
      }
      DestroyMenu(hMainMenu);
      PostQuitMessage(0);
      break;
    }
    case WM_SIZE: {
      SendMessage(hStatusBar, WM_SIZE, wParam, lParam);
      SetWindowPos(hText, 0, 0, 0,
                   LOWORD(lParam), HIWORD(lParam) - 20,
                   SWP_NOZORDER | SWP_NOREPOSITION);
      InvalidateRect(hWindow, NULL, TRUE);
      break;
    }
    case WM_CHAR: {
      if (wParam == 'd' || wParam == 'D') {
        figure = 0;
        InvalidateRect(hWindow, NULL, TRUE);
      } else if (wParam == 'r' || wParam == 'R') {
        figure = RECTANGLE;
        char buffer[] = "Selected Rectangle";
        SendMessage(hStatusBar, SB_SETTEXT, SB_PID_RADIO_CHOICE,
                    (LPARAM) buffer);
      }
       else if (wParam == 'e' || wParam == 'E') {
        figure = ELLIPSE;
        char buffer[] = "Selected Ellipse";
        SendMessage(hStatusBar, SB_SETTEXT, SB_PID_RADIO_CHOICE,
                    (LPARAM) buffer);
      }
       else if (wParam == 'c' || wParam == 'C') {
        figure = CIRCLE;
        char buffer[] = "Selected Circle";
        SendMessage(hStatusBar, SB_SETTEXT, SB_PID_RADIO_CHOICE,
                    (LPARAM) buffer);
      }
       else if (wParam == 's') {
        figure = SECTOR;
        char buffer[] = "Selected Sector";
        SendMessage(hStatusBar, SB_SETTEXT, SB_PID_RADIO_CHOICE,
                    (LPARAM) buffer);
      }
       else if (wParam == 'S') {
        figure = SEGMENT;
        char buffer[] = "Selected Segment";
        SendMessage(hStatusBar, SB_SETTEXT, SB_PID_RADIO_CHOICE,
                    (LPARAM) buffer);
      }
       else if (wParam == 'p') {
        figure = POLYGON;
        char buffer[] = "Selected Polygon";
        SendMessage(hStatusBar, SB_SETTEXT, SB_PID_RADIO_CHOICE,
                    (LPARAM) buffer);
      }
       else if (wParam == 'P') {
        figure = POLYLINE;
        char buffer[] = "Selected Polyline";
        SendMessage(hStatusBar, SB_SETTEXT, SB_PID_RADIO_CHOICE,
                    (LPARAM) buffer);
      }
       else if (wParam == 'l'||wParam == 'L') {
        figure = LINE;
        char buffer[] = "Selected Line";
        SendMessage(hStatusBar, SB_SETTEXT, SB_PID_RADIO_CHOICE,
                    (LPARAM) buffer);
      }
      else {
        return DefWindowProc(hWindow, messageCode, wParam, lParam);
      }
      break;
    }
    case WM_LBUTTONUP: {
      sx = LOWORD(lParam);
      sy = HIWORD(lParam);
      POINT point;
      point.x = sx;
      point.y = sy;
      ClientToScreen(hWindow, &point);
      InvalidateRect(hWindow, NULL, FALSE);
    }
    case WM_PAINT: {
      PAINTSTRUCT ps;
      HDC hdc = BeginPaint(hWindow, &ps);
      SelectObject(hdc, pen_handle);
      if (figure == RECTANGLE) {
        Rectangle(hdc, sx, sy, sx * 2, sy * 3 / 4);
        break;
      }
      if (figure == ELLIPSE) {
        Ellipse(hdc, sx, sy, sx * 3, sy * 2);
        break;
      }
      if (figure == CIRCLE) {
        Ellipse(hdc, sx, sy, 2 * sx, sx + sy);
        break;
      }
      if (figure == SEGMENT) {
        Pie(hdc, sx / 2, sy / 2, sx * 3 / 2, sy * 3 / 2, sx / 2 + 100, sy / 2,
            sx / 2, sy / 2 + 100);
        break;
      }
      if (figure == SECTOR) {
        Chord(hdc, sx / 2, sy / 2, sx * 3 / 2, sy * 3 / 2, sx / 2 + 100, sy / 2,
              sx / 2, sy / 2 + 100);
        break;
      }
      if (figure == POLYLINE) {
        MoveToEx(hdc, 50, 100, NULL);
        LineTo(hdc, 400, 200);
        LineTo(hdc, 100, 100);
        LineTo(hdc, 300, 100);
        LineTo(hdc, 400, 150);
        LineTo(hdc, 50, 50);
        EndPaint(hWindow, &ps);
      }
      if (figure == POLYGON) {
        POINT point;
        point.x = sx;
        point.y = sy;
        POINT points[7];
        points[0]=point;
        point.x+=sx/2;
        point.y+=sx/2;
        points[1]=point;
        point.y+=sx/4;
        points[2]=point;
        point.y+=sx/6;
        point.x-=sx/4;
        points[3]=point;
        point.y-=sx/6;
        point.x-=sx/5;
        points[4]=point;
        point.y-=sx/2;
        points[5]=point;
        point.x-=sx/10;
        point.y-=sx/30;
        points[6]=point;
        Polygon(hdc, &points, 7);
        break;
      }
      if (figure==LINE) {
        MoveToEx(hdc, sx, sy, NULL);
        LineTo(hdc, (sx+sy)/4, sx-sy);
        EndPaint(hWindow, &ps);
      }
    }
    case WM_COMMAND: {
      if (lParam == 0) {
        switch (LOWORD(wParam)) {
          case IDM_QUIT: {
            SendMessage(hWindow, WM_CLOSE, 0, 0);
            break;
          }
          case IDM_DLG_COLOR: {
            if (ChooseColor(&ccDialogData)) {
              InvalidateRect(hWindow, NULL, FALSE);
            }
            break;
          }
          case IDM_TOGGLE_OPTIONS: {
            UINT state =
                GetMenuState(hMainMenu, IDM_TOGGLE_OPTIONS, MF_BYCOMMAND);
            if (state == MF_CHECKED) {
              EnableMenuItem(
                  hMainMenu, (UINT) hMainMenuRadioGroup, MF_GRAYED);
              CheckMenuItem(hMainMenu, IDM_TOGGLE_OPTIONS, MF_UNCHECKED);
              SendMessage(hStatusBar, SB_SETTEXT, SB_PID_RADIO_STATE,
                          (LPARAM) "Choose disabled");
              char buffer[] = "Nothing is selected";
              SendMessage(hStatusBar, SB_SETTEXT, SB_PID_RADIO_CHOICE,
                          (LPARAM) buffer);
              figure = 9;
            } else {
              EnableMenuItem(
                  hMainMenu, (UINT) hMainMenuRadioGroup, MF_ENABLED);
              CheckMenuItem(hMainMenu, IDM_TOGGLE_OPTIONS, MF_CHECKED);
              SendMessage(hStatusBar, SB_SETTEXT, SB_PID_RADIO_STATE,
                          (LPARAM) "Choose enabled");
            }
            InvalidateRect(hWindow, NULL, TRUE);
            break;
          }
          case IDM_RADIO_1: {
            CheckMenuRadioItem(hMainMenu, IDM_RADIO_1, IDM_RADIO_7,
                               LOWORD(wParam), MF_BYCOMMAND);
            char buffer[] = "Selected Rectangle";
            SendMessage(hStatusBar, SB_SETTEXT, SB_PID_RADIO_CHOICE,
                        (LPARAM) buffer);
            figure = RECTANGLE;
            break;
          }
          case IDM_RADIO_2: {
            CheckMenuRadioItem(hMainMenu, IDM_RADIO_1, IDM_RADIO_7,
                               LOWORD(wParam), MF_BYCOMMAND);
            char buffer[] = "Selected Ellipse";
            SendMessage(hStatusBar, SB_SETTEXT, SB_PID_RADIO_CHOICE,
                        (LPARAM) buffer);
            figure = ELLIPSE;
            break;
          }
          case IDM_RADIO_3: {
            CheckMenuRadioItem(hMainMenu, IDM_RADIO_1, IDM_RADIO_7,
                               LOWORD(wParam), MF_BYCOMMAND);
            char buffer[] = "Selected Circle";
            SendMessage(hStatusBar, SB_SETTEXT, SB_PID_RADIO_CHOICE,
                        (LPARAM) buffer);
            figure = CIRCLE;
            break;
          }
          case IDM_RADIO_4: {
            CheckMenuRadioItem(hMainMenu, IDM_RADIO_1, IDM_RADIO_7,
                               LOWORD(wParam), MF_BYCOMMAND);
            char buffer[] = "Selected Sector";
            SendMessage(hStatusBar, SB_SETTEXT, SB_PID_RADIO_CHOICE,
                        (LPARAM) buffer);
            figure = SECTOR;
            break;
          }
          case IDM_RADIO_5: {
            CheckMenuRadioItem(hMainMenu, IDM_RADIO_1, IDM_RADIO_7,
                               LOWORD(wParam), MF_BYCOMMAND);
            char buffer[] = "Selected Segment";
            SendMessage(hStatusBar, SB_SETTEXT, SB_PID_RADIO_CHOICE,
                        (LPARAM) buffer);
            figure = SEGMENT;
            break;
          }
          case IDM_RADIO_6: {
            CheckMenuRadioItem(hMainMenu, IDM_RADIO_1, IDM_RADIO_7,
                               LOWORD(wParam), MF_BYCOMMAND);
            char buffer[] = "Selected Polyline";
            SendMessage(hStatusBar, SB_SETTEXT, SB_PID_RADIO_CHOICE,
                        (LPARAM) buffer);
            figure = POLYLINE;
            break;
          }
          case IDM_RADIO_7: {
            CheckMenuRadioItem(hMainMenu, IDM_RADIO_1, IDM_RADIO_7,
                               LOWORD(wParam), MF_BYCOMMAND);
            char buffer[] = "Selected Polygon";
            SendMessage(hStatusBar, SB_SETTEXT, SB_PID_RADIO_CHOICE,
                        (LPARAM) buffer);
            figure = POLYGON;
            break;
          }
          case IDM_RADIO_8: {
            CheckMenuRadioItem(hMainMenu, IDM_RADIO_1, IDM_RADIO_8,
                               LOWORD(wParam), MF_BYCOMMAND);
            char buffer[] = "Selected Line";
            SendMessage(hStatusBar, SB_SETTEXT, SB_PID_RADIO_CHOICE,
                        (LPARAM) buffer);
            figure = LINE;
            break;
          }
          case IDM_DLG_HINTS: {
            int mbResult = IDRETRY;
            while (mbResult == IDRETRY) {
              mbResult = MessageBox(
                  hWindow,
                  "Select the shape (to do this, allow access to the choose) and click the left mouse button to draw it.\nClick 'd' or 'D' to clear\nYou can also see below whether drawing is available to you and which shape is selected\nAlso the size of the shape depends on the coordinates of the point you clicked on (because why not? :) )\nAlso you can draw many shapes on the screen, not just one\nYou can choose the color with which your shapes will be drawn\npressing the \"r\" or \"R\" key selects the rectangle; pressing the \"e\" or \"E\" key selects the ellipse; pressing the \"l\" or \"L\" key selects the line; pressing the \"c\" or \"C\" key selects the circle; pressing the \"s\" key selects the sector; pressing the \"S\" key selects the segment; pressing the \"p\" key selects the polygon; pressing the \"P\" key selects the polyline",
                  "Reference",
                  MB_OK);
            }
            break;
          }
          default: {
            break;
          }
        }

      } else {
        return DefWindowProc(hWindow, messageCode, wParam, lParam);
      }
      break;
    }
    default: {
      return DefWindowProc(hWindow, messageCode, wParam, lParam);
    }
  }

  return 0;
}

INT WinMain(HINSTANCE hCurrentInstance, HINSTANCE haParentInstance,
            LPSTR lpstraCommandLine, int naCmdShow) {
  FreeConsole();

  const char WINDOW_CLASS_NAME[] = "MainWindowClass";
  WNDCLASS windowClass = {};
  windowClass.lpfnWndProc = WndProc;
  windowClass.hInstance = hCurrentInstance;
  windowClass.lpszClassName = WINDOW_CLASS_NAME;
  windowClass.hCursor = LoadCursor(0, IDC_ARROW);
  windowClass.hbrBackground = GetSysColorBrush(COLOR_WINDOW);
  RegisterClass(&windowClass);

  HWND hWindow = CreateWindowEx(
      /* dwExStyle =    */ 0,
      /* lpClassName =  */ WINDOW_CLASS_NAME,
      /* lpWindowName = */ "A part of geogebra",
      /* dwStyle =      */ WS_OVERLAPPEDWINDOW | WS_VISIBLE
                               | CS_HREDRAW | CS_VREDRAW,
      /* X =            */ CW_USEDEFAULT,
      /* Y =            */ CW_USEDEFAULT,
      /* nWidth =       */ 800,
      /* nHeight =      */ 600,
      /* hWndParent =   */ NULL,
      /* hMenu =        */ NULL,
      /* hInstance =    */ hCurrentInstance,
      /* lpParam =      */ NULL
  );
  if (hWindow == NULL) {
    return 1;
  }
  MSG message = {};
  while (GetMessage(&message, NULL, 0, 0)) {
    TranslateMessage(&message);
    DispatchMessage(&message);
  }

  return (int) message.wParam;
}