#include "Functions.h"


#pragma region declarations
// Global solid brush
static HBRUSH hBrush = CreateSolidBrush(RGB(240, 240, 240));
// Global declaration of windows, HWND - handle to a window
HWND mainWindow, mainWindowLbl1, mainWindowLbl2, mainWindowTxt, mainWindowBtn,  // Main window
	registerWindow, regWindowLbl1, regWindowLbl2, regFirstNameTxt, regLastNameTxt, registerBtn, regCancelBtn, // Register window
	adminWindow, adminOkBtn, adminCnclBtn, adminPrntReg, adminPrntAtten,//Admin window
	viewTextWin;

// wstring to store text field data
std::wstring idTextStr;
std::wstring* userArray; //dynamic array for textout

int numLines;
bool open = false;

// enums used to identify buttons
enum {ID_LOGINBTN, ID_REGISTER, ID_REGCANCEL, 
	ID_ARMYRADIO, ID_MARINESRADIO, ID_AIRFORCERADIO, ID_NAVYRADIO, ID_COASTGUARDRADIO, 
	ID_VIEWCLUB, ID_ADMINOK, ID_ADMINCNCL, ID_VIEWATTENDANCE, ID_BACKUP, ID_PRNTCLB, ID_PRNTATTEN};

//sqlite3 pointer
sqlite3* db;
#pragma endregion declarations

#pragma region Callback functions
// Callback functions define how a window processes commands and carries them out
LRESULT CALLBACK MainWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK RegisterWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK ViewTextWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK AdminWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
#pragma endregion Callback functions


int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstnace, PWSTR pCmdLine, int nCmdShow)
{
#pragma region RegClasses
	// wchar_t array stores name of the class, not necessary but allows for more than one use of same class
	const wchar_t MAIN_WINDOW[] = L"MainWindow";

	// Register main window
	// WNDCLASSEX - window class extended
	WNDCLASSEX mainWin = { };

	mainWin.cbSize = sizeof(WNDCLASSEX);
	mainWin.lpfnWndProc = MainWindowProc;
	mainWin.hInstance = hInstance;
	mainWin.lpszClassName = MAIN_WINDOW;

	RegisterClassEx(&mainWin);

#pragma endregion Register window classes
	// Create the window object
#pragma region MainWindow
	mainWindow = CreateWindowEx(
		0, // Extended window style
		MAIN_WINDOW, // class name
		L"OCCVC Login", // Title bar text
		WS_OVERLAPPEDWINDOW, // Window type
		650, // x co-ordinate
		400, // y co-ordinate
		500, // width
		300, // height
		NULL, // parent window  OPTIONAL
		NULL, // Handle to a menu  OPTIONAL
		hInstance, // instance handle  OPTIONAL
		NULL // value passed through CREATESTRUCT after WM_CREATE  OPTIONAL
	);

	EnableMenuItem(GetSystemMenu(mainWindow, FALSE), SC_CLOSE,
		MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);

	if (mainWindow == NULL)
	{
		return 0;
	}

	// How the window will be displayed
	// first param: handle to the window
	// second param: how the window will be shown
	ShowWindow(mainWindow, nCmdShow);

	// MSG struct to be passed to the window
	MSG mainWinMsg = { };

	// Loop to handle messages
	while (GetMessage(&mainWinMsg, NULL, 0, 0))
	{
		if (IsDialogMessage(mainWindow, &mainWinMsg) == 0)
		{
			TranslateMessage(&mainWinMsg);
			DispatchMessage(&mainWinMsg);
		}
	}
#pragma endregion Create main window
	return 0;
}

// Main WinProc
LRESULT CALLBACK MainWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_CREATE: // Executes when window is created
		{
#pragma region Main window
			// Create Txt box
			mainWindowTxt = CreateWindowEx(
				WS_EX_CLIENTEDGE,
				L"EDIT", // L"EDIT" Specifically defines window as a text field
				L"",
				WS_BORDER | WS_CHILD | WS_VISIBLE | WS_TABSTOP,
				40,
				150,
				400,
				25,
				hwnd,
				NULL,
				NULL,
				NULL
			);

			// Create gretting lbl
			mainWindowLbl1 = CreateWindowEx(
				WS_EX_TRANSPARENT,
				L"STATIC", // L"STATIC" Speifically defines window as a label
				L"",
				WS_CHILD | WS_VISIBLE ,
				175,
				60,
				126,
				16,
				hwnd,
				NULL,
				NULL,
				NULL
				);
			// create instruction lbl
			mainWindowLbl2 = CreateWindowEx(
				WS_EX_TRANSPARENT,
				L"STATIC",
				L"",
				WS_CHILD | WS_VISIBLE,
				146,
				85,
				185,
				16,
				hwnd,
				NULL,
				NULL,
				NULL
			);

			// Set texts for both labels
			SendMessage(mainWindowLbl2, WM_SETTEXT, NULL, (LPARAM)L"Please enter your student ID");
			SendMessage(mainWindowLbl1, WM_SETTEXT, NULL, (LPARAM)L"Welcome to OCCVC");

			// Create login button
			mainWindowBtn = CreateWindowEx(
				WS_EX_LEFT,
				L"BUTTON", // L"BUTTON" Specifically defines window as a button
				L"Login",
				WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
				205,
				200,
				65,
				25,
				hwnd,
				(HMENU)ID_LOGINBTN,
				(HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE),
				NULL
			);
#pragma endregion Create child windows to the main window
			break;
		}
		case WM_CLOSE: // Executes when the X button is clicked
		{
			DestroyWindow(mainWindow);
			break;
		}
		case WM_DESTROY: // Executes when DestroyWindow is called
		{
			PostQuitMessage(0);
			return 0;
		}
		case WM_PAINT: // Colors the window
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd, &ps);

			FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW));

			EndPaint(hwnd, &ps);

			break;
		}
		case WM_CTLCOLORSTATIC: // Sets the background for labels, otherwise they would have default background
		{
			if (mainWindowLbl1 == (HWND)lParam || mainWindowLbl2 == (HWND)lParam)
			{
				HDC hdcStatic = (HDC)wParam;
				SetTextColor(hdcStatic, RGB(0, 0, 0));
				SetBkColor(hdcStatic, RGB(240, 240, 240));
				return (INT_PTR)hBrush;
			}
		}
		case WM_COMMAND: // How the window handles user commands
		{
			switch (wParam) // The window that the user interacts with
			{
				case ID_LOGINBTN:
				{
#pragma region TextBox check
					int buffSize = 10;
					LPTSTR idText = new TCHAR[buffSize];
					// Getting the window text and store in std::wstring
					// Note: std::wstring has overloaded assignment operator to TCHAR[]
					GetWindowText(mainWindowTxt, idText, buffSize);
					idTextStr = idText;
					

					if (idTextStr == L"Close")
					{
						//DestroyWindow(registerWindow);
						DestroyWindow(mainWindow);
					}
					else if (idTextStr == L"password")
					{
						ShowWindow(mainWindow, SW_HIDE);

						static const wchar_t ADMIN_WINDOW[] = L"AdminWindow";

						WNDCLASSEX adminWin = { };

						adminWin.cbSize = sizeof(WNDCLASSEX);
						adminWin.lpfnWndProc = AdminWindowProc;
						adminWin.lpszClassName = ADMIN_WINDOW;

						RegisterClassEx(&adminWin);

						adminWindow = CreateWindowEx(
							0,
							ADMIN_WINDOW,
							L"OCCVC Admin",
							WS_OVERLAPPEDWINDOW,
							650,
							400,
							500,
							300,
							NULL,
							NULL,
							NULL,
							NULL
						);
				
						ShowWindow(adminWindow, SW_SHOW);

						MSG adminMsg = { };
						while (GetMessage(&adminMsg, NULL, 0, 0))
						{
							if (IsDialogMessage(adminWindow, &adminMsg) == 0)
							{
								TranslateMessage(&adminMsg);
								DispatchMessage(&adminMsg);
							}
						}

						SetWindowText(mainWindowTxt, L"");
						ShowWindow(mainWindow, SW_SHOW);
					}
					else if (!validID(idTextStr))
					{
						MessageBox(hwnd, L"Invalid ID, please try again", NULL, MB_OK);
					}
					else if (!checkForUser(idTextStr, db))
					{
						MessageBox(hwnd, L"ID not found, please register", NULL, MB_OK);
						
						// SW_HIDE - hides window the handle parameter points to
						ShowWindow(mainWindow, SW_HIDE);

						static const wchar_t REG_WINDOW[] = L"RegWindow";
						WNDCLASSEX registerWin = {};

						registerWin.cbSize = sizeof(WNDCLASSEX);
						registerWin.lpfnWndProc = RegisterWindowProc;
						registerWin.lpszClassName = REG_WINDOW;



						RegisterClassEx(&registerWin);

						// Create regWindow here
						registerWindow = CreateWindowEx(
							0,
							REG_WINDOW,
							L"OCCVC Login",
							WS_OVERLAPPEDWINDOW,
							650,
							400,
							600,
							300,
							NULL,
							NULL,
							NULL,
							NULL
						);

						EnableMenuItem(GetSystemMenu(registerWindow, FALSE), SC_CLOSE,
							MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);

						// Show register window
						ShowWindow(registerWindow, SW_SHOW);

						// Handle messages for the register window
						MSG regWinMsg = { };
						while (GetMessage(&regWinMsg, NULL, 0, 0))
						{
							if (IsDialogMessage(registerWindow, &regWinMsg) == 0)
							{
								TranslateMessage(&regWinMsg);
								DispatchMessage(&regWinMsg);
							}
						}

						// Reset the text in the text fields to blank
						SetWindowText(regFirstNameTxt, L"");
						SetWindowText(regLastNameTxt, L"");
						SetWindowText(mainWindowTxt, L"");
						ShowWindow(mainWindow, SW_SHOW);
					}
					else
					{
						MessageBox(hwnd, L"Welcome!", L"Success!", MB_OK);
						SetWindowText(mainWindowTxt, L"");
						timeStamp(idTextStr);
					}
#pragma endregion Check user entry in textbox
					delete[] idText;
					break;
				}
			}
			break;
		}
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

// Register WinProc
LRESULT CALLBACK RegisterWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_CREATE:
		{
#pragma region regwindow
			// Create label
			regWindowLbl1 = CreateWindowEx(
				WS_EX_TRANSPARENT,
				L"STATIC",
				L"",
				WS_CHILD | WS_VISIBLE,
				20,
				75,
				75,
				16,
				hwnd,
				NULL,
				NULL,
				NULL
			);

			// Create label
			regWindowLbl2 = CreateWindowEx(
				WS_EX_TRANSPARENT,
				L"STATIC",
				L"",
				WS_CHILD | WS_VISIBLE,
				20,
				150,
				75,
				16,
				hwnd,
				NULL,
				NULL,
				NULL
			);

			// Set label text
			SendMessage(regWindowLbl1, WM_SETTEXT, NULL, (LPARAM)L"First Name:");
			SendMessage(regWindowLbl2, WM_SETTEXT, NULL, (LPARAM)L"Last Name:");

			// Create text field
			regFirstNameTxt = CreateWindowEx(
				WS_EX_CLIENTEDGE,
				L"EDIT",
				L"",
				WS_BORDER | WS_CHILD | WS_VISIBLE | WS_TABSTOP,
				100,
				71,
				250,
				25,
				hwnd,
				NULL,
				NULL,
				NULL
			);

			// Create text field
			regLastNameTxt = CreateWindowEx(
				WS_EX_CLIENTEDGE,
				L"EDIT",
				L"",
				WS_BORDER | WS_CHILD | WS_VISIBLE | WS_TABSTOP,
				100,
				146,
				250,
				25,
				hwnd,
				NULL,
				NULL,
				NULL
			);

			// Create Air Force radio button
			CreateWindowEx(
				WS_EX_WINDOWEDGE,
				L"BUTTON",
				L"Air Force",
				WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON
				| WS_TABSTOP | WS_GROUP, // WS_GROUP notes the start of grouped windows
				420,												   // BS_AUTORADIOBUTTON defines the button as a radio
				30,
				100,
				20,
				hwnd,
				(HMENU)ID_AIRFORCERADIO,
				NULL,
				NULL
			);

			// Create Army radio button
			CreateWindowEx(
				WS_EX_WINDOWEDGE,
				L"BUTTON",
				L"Army",
				WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON, // Note: no WS_GROUP because button above defined beginning of group
				420,
				70,
				100,
				20,
				hwnd,
				(HMENU)ID_ARMYRADIO,
				NULL,
				NULL
			);

			//Create Coast Guard radio button
			CreateWindowEx(
				WS_EX_WINDOWEDGE,
				L"BUTTON",
				L"Coast Guard",
				WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON,
				420,
				110,
				100,
				20,
				hwnd,
				(HMENU)ID_COASTGUARDRADIO,
				NULL,
				NULL
				);

			// Create Marines radio button
			CreateWindowEx(
				WS_EX_WINDOWEDGE,
				L"BUTTON",
				L"Marines",
				WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON,
				420,
				150,
				100,
				20,
				hwnd,
				(HMENU)ID_MARINESRADIO,
				NULL,
				NULL
			);

			// Create Navy radio button
			CreateWindowEx(
				WS_EX_WINDOWEDGE,
				L"BUTTON",
				L"Navy",
				WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON,
				420,
				190,
				100,
				20,
				hwnd,
				(HMENU)ID_NAVYRADIO,
				NULL,
				NULL
			);

			

			// Create button
			registerBtn = CreateWindowEx(
				WS_EX_LEFT,
				L"BUTTON",
				L"Register",
				WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
				185,
				200,
				65,
				25,
				hwnd,
				(HMENU)ID_REGISTER,
				(HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE),
				NULL
			);

			// Create button
			regCancelBtn = CreateWindowEx(
				WS_EX_LEFT,
				L"BUTTON",
				L"Cancel",
				WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
				275,
				200,
				65,
				25,
				hwnd,
				(HMENU)ID_REGCANCEL,
				(HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE),
				NULL
			);

			// Create radio buttons: DOES NOT NEED TO BE ASSIGNED
			// Using enumed values to identify
			
#pragma endregion Child windows for regWindow
			break;
		}
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		}
		case WM_CLOSE:
		{
			DestroyWindow(registerWindow);
			break;
		}
		case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd, &ps);

			FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW));

			EndPaint(hwnd, &ps);

			break;
		}
		case WM_CTLCOLORSTATIC:
		{
			if (regWindowLbl1 == (HWND)lParam || regWindowLbl2 == (HWND)lParam)
			{
				HDC hdcStatic = (HDC)wParam;
				SetTextColor(hdcStatic, RGB(0, 0, 0));
				SetBkColor(hdcStatic, RGB(240, 240, 240));
				return (INT_PTR)hBrush;
			}
		}
		case WM_COMMAND:
		{
			switch (wParam)
			{
				case ID_REGCANCEL:
				{
					//ShowWindow(mainWindow, SW_SHOW);
					DestroyWindow(registerWindow);
					return 0;
				}
				case ID_REGISTER:
				{
#pragma region regTextCheck
					int buffSize = 20;
					LPTSTR firstTxt = new TCHAR[buffSize];

					int txtSize = GetWindowText(regFirstNameTxt, firstTxt, buffSize);
					std::wstring firstName = firstTxt;
					delete[] firstTxt;

					LPTSTR lastTxt = new TCHAR[buffSize];
					txtSize = GetWindowText(regLastNameTxt, lastTxt, buffSize);
					std::wstring lastName = lastTxt;
					delete[] lastTxt;

					if (firstName != L"" && lastName != L"")
					{
						bool valid = false;
						std::wstring idTemp = idTextStr;
						std::wstring branch;
						idTextStr += L" " + lastName + L", " + firstName;
						if (IsDlgButtonChecked(hwnd, ID_AIRFORCERADIO))
						{
							valid = true;
							idTextStr += L" Air Force";
							branch = L"Air Force";
						}
						else if (IsDlgButtonChecked(hwnd, ID_ARMYRADIO))
						{
							valid = true;
							idTextStr += L" Army";
							branch = L"Army";
						}
						else if (IsDlgButtonChecked(hwnd, ID_MARINESRADIO))
						{
							valid = true;
							idTextStr += L" Marines";
							branch = L"Marines";
						}
						else if (IsDlgButtonChecked(hwnd, ID_NAVYRADIO))
						{
							valid = true;
							idTextStr += L" Navy";
							branch = L"Navy";
						}
						else if (IsDlgButtonChecked(hwnd, ID_COASTGUARDRADIO))
						{
							valid = true;
							idTextStr += L" Coast Guard";
							branch = L"Coast Guard";
						}
						else
						{
							idTextStr = idTemp;
							MessageBox(hwnd, L"All fields must be complete", L"Error", MB_OK);
						}
						if (valid)
						{
							//addUserToRegistry(idTextStr);
							addUserToRegistry(idTemp, lastName, firstName, branch, db);
							timeStamp(idTextStr);
							MessageBox(hwnd, L"Welcome!", L"Success!", MB_OK);
							DestroyWindow(registerWindow);
						}
					}
					else
					{
						MessageBox(hwnd, L"All fields must be complete", L"Error",  MB_OK);
					}
#pragma endregion Procedures to check user entry in regWindow
				}
			}
		}
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

// Text WinProc
LRESULT CALLBACK ViewTextWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static int cxChar, cxCaps, cyChar, cxClient, cyClient, iMaxWidth;
	static int iDeltaPerLine, iAccumDelta;
	HDC hdc;
	int i, x, y, iVertPos, iHorzPos, iPaintBeg, iPaintEnd;
	PAINTSTRUCT ps;
	SCROLLINFO si;
	//TCHAR szBuffer[10];
	TEXTMETRIC tm;
	ULONG ulScrollLines;

	//numLines = getNumLines(nameFile);

	switch (uMsg)
	{
		case WM_CLOSE:
		{
			DestroyWindow(hwnd);
			break;
		}
		case WM_DESTROY:
		{
			delete[] userArray;
			PostQuitMessage(0);
			break;
		}
		case WM_CREATE:
		{
			hdc = GetDC(hwnd);
			GetTextMetrics(hdc, &tm);

			cxChar = tm.tmAveCharWidth;
			cxCaps = (tm.tmPitchAndFamily & 1 ? 3 : 2) * cxChar / 2;
			cyChar = tm.tmHeight + tm.tmExternalLeading;

			ReleaseDC(hwnd, hdc);
		}
		case WM_SETTINGCHANGE:
		{
			SystemParametersInfo(SPI_GETWHEELSCROLLLINES, 0, &ulScrollLines, 0);
			if (ulScrollLines)
			{
				iDeltaPerLine = WHEEL_DELTA / ulScrollLines;
			}
			else
			{
				iDeltaPerLine = 0;
			}
			return 0;
		}
		case WM_MOUSEWHEEL:
		{
			if (iDeltaPerLine == 0)
				break;
			iAccumDelta += (short)HIWORD(wParam);

			while (iAccumDelta >= iDeltaPerLine)
			{
				SendMessage(hwnd, WM_VSCROLL, SB_LINEUP, 0);
				iAccumDelta -= iDeltaPerLine;
			}

			while (iAccumDelta <= -iDeltaPerLine)
			{
				SendMessage(hwnd, WM_VSCROLL, SB_LINEDOWN, 0);
				iAccumDelta += iDeltaPerLine;
			}
			return 0;
		}
		case WM_SIZE:
		{
			cxClient = LOWORD(lParam);
			cyClient = HIWORD(lParam);

			//Vertical bar
			si.cbSize = sizeof(si);
			si.fMask = SIF_RANGE | SIF_PAGE;
			si.nMin = 0;
			si.nMax = numLines;
			si.nPage = cyClient / cyChar;
			SetScrollInfo(hwnd, SB_VERT, &si, TRUE);

			//Horizontal bar
			si.cbSize = sizeof(si);
			si.fMask = SIF_RANGE | SIF_PAGE;
			si.nMin = 0;
			si.nMax = 500;
			si.nPage = cxClient / cxChar;
			SetScrollInfo(hwnd, SB_HORZ, &si, TRUE);
			return 0;
		}
		case WM_VSCROLL:
		{
			si.cbSize = sizeof(si);
			si.fMask = SIF_ALL;
			GetScrollInfo(hwnd, SB_VERT, &si);

			iVertPos = si.nPos;

			switch (LOWORD(wParam))
			{
				case SB_TOP:
				{
					si.nPos = si.nMin;
					break;
				}
				case SB_BOTTOM:
				{
					si.nPos = si.nMax;
					break;
				}
				case SB_LINEUP:
				{
					si.nPos -= 1;
					break;
				}
				case SB_LINEDOWN:
				{
					si.nPos += 1;
					break;
				}
				case SB_PAGEUP:
				{
					si.nPos -= si.nPage;
					break;
				}
				case SB_PAGEDOWN:
				{
					si.nPos += si.nPage;
					break;
				}
				case SB_THUMBTRACK:
				{
					si.nPos = si.nTrackPos;
					break;
				}
				default:
					break;
			}
			si.fMask = SIF_POS;
			SetScrollInfo(hwnd, SB_VERT, &si, TRUE);
			GetScrollInfo(hwnd, SB_VERT, &si);

			if (si.nPos != iVertPos)
			{
				ScrollWindow(hwnd, 0, cyChar * (iVertPos - si.nPos), NULL, NULL);
				UpdateWindow(hwnd);
			}
			return 0;
		}
		case WM_HSCROLL :
		{
			si.cbSize = sizeof(si);
			si.fMask = SIF_ALL;
			GetScrollInfo(hwnd, SB_HORZ, &si);
			iHorzPos = si.nPos;

			switch (LOWORD(wParam))
			{
				case SB_LINELEFT:
				{
					si.nPos -= 1;
					break;
				}
				case SB_LINERIGHT:
				{
					si.nPos += 1;
					break;
				}
				case SB_PAGELEFT:
				{
					si.nPos -= si.nPage;
					break;
				}
				case SB_PAGERIGHT:
				{
					si.nPos += si.nPage;
					break;
				}
				case SB_THUMBPOSITION:
				{
					si.nPos = si.nTrackPos;
					break;
				}
				default:
					break;
			}

			si.fMask = SIF_POS;
			SetScrollInfo(hwnd, SB_HORZ, &si, TRUE);
			GetScrollInfo(hwnd, SB_HORZ, &si);

			if (si.nPos != iHorzPos)
			{
				ScrollWindow(hwnd, cxChar * (iHorzPos - si.nPos), 0, NULL, NULL);
			}
			return 0;
		}
		case WM_PAINT:
		{
			hdc = BeginPaint(hwnd, &ps);
			SetBkMode(hdc, TRANSPARENT);
			FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW));

			si.cbSize = sizeof(si);
			si.fMask = SIF_POS;
			GetScrollInfo(hwnd, SB_VERT, &si);
			iVertPos = si.nPos;

			GetScrollInfo(hwnd, SB_HORZ, &si);
			iHorzPos = si.nPos;

			iPaintBeg = max(0, iVertPos + ps.rcPaint.top / cyChar);
			iPaintEnd = min(numLines - 1, iVertPos + ps.rcPaint.bottom / cyChar);

			for(i = iPaintBeg; i <= iPaintEnd; i++)
			{
				x = cxChar * (1 - iHorzPos);
				y = cyChar * (i - iVertPos);

				TextOut(hdc, 0, y, userArray[i].c_str(), userArray[i].length());

			}
			EndPaint(hwnd, &ps);
			return 0;
		}
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

// Admin WinProc
LRESULT CALLBACK AdminWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_CREATE:
		{
			CreateWindowEx(
				WS_EX_WINDOWEDGE,
				L"BUTTON",
				L"View Club",
				WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON
				| WS_TABSTOP | WS_GROUP, // WS_GROUP notes the start of grouped windows
				0,												   // BS_AUTORADIOBUTTON defines the button as a radio
				30,
				100,
				20,
				hwnd,
				(HMENU)ID_VIEWCLUB,
				NULL,
				NULL
			);

			CreateWindowEx(
				WS_EX_WINDOWEDGE,
				L"BUTTON",
				L"View Attendance",
				WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON,
				0,
				60,
				130,
				20,
				hwnd,
				(HMENU)ID_VIEWATTENDANCE,
				NULL,
				NULL
			);

			CreateWindowEx(
				WS_EX_WINDOWEDGE,
				L"BUTTON",
				L"Backup Attendance",
				WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON,
				0,
				90,
				150,
				20,
				hwnd,
				(HMENU)ID_BACKUP,
				NULL,
				NULL
			);

			CreateWindowEx(
				WS_EX_WINDOWEDGE,
				L"BUTTON",
				L"Print Club",
				WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON,
				150,
				30,
				100,
				20,
				hwnd,
				(HMENU)ID_PRNTCLB,
				NULL,
				NULL
			);

			//CreateWindowEx(
			//	WS_EX_WINDOWEDGE,
			//	L"BUTTON",
			//	L"Print Attendance",
			//	WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON,
			//	150,
			//	60,
			//	130,
			//	20,
			//	hwnd,
			//	(HMENU)ID_PRNTATTEN,
			//	NULL,
			//	NULL
			//);

			adminOkBtn = CreateWindowEx(
				WS_EX_LEFT,
				L"BUTTON",
				L"OK",
				WS_CHILD | WS_VISIBLE,
				150,
				200,
				65,
				25,
				hwnd,
				(HMENU)ID_ADMINOK,
				NULL,
				NULL
			);

			adminCnclBtn = CreateWindowEx(
				WS_EX_LEFT,
				L"BUTTON",
				L"Cancel",
				WS_CHILD | WS_VISIBLE,
				250,
				200,
				65,
				25,
				hwnd,
				(HMENU)ID_ADMINCNCL,
				NULL,
				NULL
			);

			return 0;
		}
		case WM_CLOSE:
		{
			DestroyWindow(hwnd);
			return 0;
		}
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		}
		case WM_PAINT: // Colors the window
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd, &ps);

			FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW));

			EndPaint(hwnd, &ps);

			return 0;
		}
		case WM_COMMAND:
		{
			switch (wParam)
			{
				case ID_ADMINCNCL:
				{
					DestroyWindow(hwnd);
					return 0;
				}
				case ID_ADMINOK:
				{
					if (IsDlgButtonChecked(hwnd, ID_VIEWCLUB) || IsDlgButtonChecked(hwnd, ID_VIEWATTENDANCE))
					{
						if (IsDlgButtonChecked(hwnd, ID_VIEWCLUB))
						{
							numLines = getNumRows(db);
							userArray = new std::wstring[numLines];
							getRows(db, userArray);

						}
						else if (IsDlgButtonChecked(hwnd, ID_VIEWATTENDANCE))
						{
							numLines = getNumLines("Logins");
							userArray = new std::wstring[numLines];
							std::wifstream inFile;
							inFile.open("Logins.txt");
							std::wstring line;
							int j = 0;
							while (getline(inFile, line))
							{
								userArray[j] = line;
								j++;
							}
						}
						if (!open)
						{
							open = true;
							static const wchar_t VIEW_WINDOW[] = L"ViewWindow";
							ShowWindow(mainWindow, SW_HIDE);

							WNDCLASSEX viewWin = {};

							viewWin.cbSize = sizeof(WNDCLASSEX);
							viewWin.lpfnWndProc = ViewTextWindowProc;
							viewWin.lpszClassName = VIEW_WINDOW;

							RegisterClassEx(&viewWin);

							viewTextWin = CreateWindowEx(
								0,
								VIEW_WINDOW,
								L"View club roster/attendance",
								WS_OVERLAPPEDWINDOW | WS_VSCROLL | WS_HSCROLL,
								CW_USEDEFAULT,
								CW_USEDEFAULT,
								500,
								300,
								NULL,
								NULL,
								NULL,
								NULL
							);
							ShowWindow(viewTextWin, SW_SHOW);
							open = true;
							MSG viewMsg = {};
							while (GetMessage(&viewMsg, NULL, 0, 0))
							{
								if (IsDialogMessage(viewTextWin, &viewMsg) == 0)
								{
									TranslateMessage(&viewMsg);
									DispatchMessage(&viewMsg);
								}
							}
							open = false;
						}
					}
					else if (IsDlgButtonChecked(hwnd, ID_BACKUP))
					{
						std::time_t now = std::time(0);
						char realNow[50];
						ctime_s(realNow, 50, &now);

						std::string temp = realNow;
						std::string date = temp.substr(0, 3) + "_" + temp.substr(4, 3) + "_" + temp.substr(8, 2) + "_" + temp.substr(20, 4) + ".txt";
						const char* c = date.c_str();
						const char* old = "Logins.txt";
						int fail = rename(old, c);
						if (fail == 0)
						{
							std::ifstream in;
							in.open("Logins.txt");
							MessageBox(hwnd, L"Backup successful", NULL, MB_OK);

						}
						else
						{
							MessageBox(hwnd, L"Backup failed", NULL, MB_OK);
						}
					}
					else if (IsDlgButtonChecked(hwnd, ID_PRNTCLB))
					{
						if (tableToTxt(db))
						{
							// Send "temp.txt" to printer
						}
					}
				}
			}
		}
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}