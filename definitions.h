#ifndef BMSD_H_INCLUDED
#define BMSD_H_INCLUDED

#if defined(UNICODE) && !defined(_UNICODE)
#define _UNICODE
#elif defined(_UNICODE) && !defined(UNICODE)
#define UNICODE
#endif

#pragma comment(lib, "gdiplus.lib")

#define open_file             1
#define OnSerialRefresh		  2
#define OnConnectRequest	  3
#define OnExitSoftware		  4
#define emergency_stop_button 5
#define solve_but             6
#define params                7
#define send_commands         8
#define draw_plot             9
#define open_plot             11
#define open_file_gr          12
#define flip_plot             13
#define help                  14

#define TextBufferSize		  256
#define ComSelectIndex		  120
#define ComPortAmount		  50

void LoadData_gr(LPCSTR path);
void SetOpenFileParams_gr(HWND gr);
CONST DWORD ReadTimeout = 1;
double nPixPerT;
int selectedPort = 1;
int targetBaudRate = 9600;

char Buffer[TextBufferSize];
int U_0;

const char* xMark_1[]= {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "20", "21", "22", "23", "24", "25", "26", "27", "28", "29", "30", "31", "32", "33", "34", "35", "36", "37", "38", "39", "40", "41", "42", "43", "44", "45", "46", "47", "48", "49", "50", "51", "52", "53", "54", "55", "56", "57", "58", "59", "60"};
const char* xMark_2[]= {"0", "2", "4", "6", "8", "10", "12", "14", "16", "18", "20", "22", "24", "26", "28", "30", "32", "34", "36", "38", "40", "42", "44", "46", "48", "50", "52", "54", "56", "58", "60", "62", "64", "66", "68", "70", "72", "74", "76", "78", "80", "82", "84", "86", "88", "90", "92", "94", "96", "98", "100", "102", "104", "106", "108", "110", "112", "114", "116", "118", "120"};
const char* xMark_5[]= {"0", "5", "10", "15", "20", "25", "30", "35", "40", "45", "50", "55", "60", "65", "70", "75", "80", "85", "90", "95", "100", "105", "110", "115", "120", "125", "130", "135", "140", "145", "150", "155", "160", "165", "170", "175", "180", "185", "190", "195", "200", "205", "210", "215", "220", "225", "230", "235", "240", "245", "250", "255", "260", "265", "270", "275", "280", "285", "290", "295", "300"};
const char* xMark_10[]= {"0", "10", "20", "30", "40", "50", "60", "70", "80", "90", "100", "110", "120", "130", "140", "150", "160", "170", "180", "190", "200", "210", "220", "230", "240", "250", "260", "270", "280", "290", "300", "310", "320", "330", "340", "350", "360", "370", "380", "390", "400", "410", "420", "430", "440", "450", "460", "470", "480", "490", "500", "510", "520", "530", "540", "550", "560", "570", "580", "590", "600"};

const char* yMark[]= {"30", "28", "26", "24", "22", "20", "18", "16", "14", "12", "10", "8", "6", "4", "2", "0", "-2", "-4", "-6", "-8", "-10", "-12", "-14", "-16", "-18", "-20", "-22", "-24", "-26", "-28", "-30"};

OPENFILENAMEA ofn;
char filename[260];

OPENFILENAMEA ofn_gr;
char filename_gr[260];

volatile bool isConnected = false;
volatile bool isThreading = true;

HANDLE connectedPort;
HANDLE readThread;

HWND hStaticControl, text_enter;
HWND hEditControl;
HWND open_file_but, send_but, emergency_stop_but, test_but;

HWND open_file_gr_but, hEditPoints, flip_plot_but;

HWND connect_lines_but;

HWND hwnd_0, gr_draw, gr_open, help_win;
HINSTANCE hInst;

HMENU ComPortSubMenu;
HMENU ComPortListMenu;
HMENU GraphSubMenu;

double Omega_ = 28.;
double U_max = 120.;
double U_min = 10.;

static BOOL bFirstSave = TRUE;

LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
void czCreateMenu(HWND hWwnd);
void SaveScreen(HWND hWND);
bool save(HWND hWnd, OPENFILENAME openFile);

LRESULT CALLBACK SoftwareMainProcedure(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);
LRESULT CALLBACK GraphMainProcedure(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);
LRESULT CALLBACK HELPProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);

// LRESULT CALLBACK DrawProcedure(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);
WNDCLASS SoftwareMainClass, GraphClass, DrawClass, HelpClass;


void MainWndAddMenus(HWND hWnd);

void MainWndAddWidgets(HWND hWnd);
void SetOpenFileParams(HWND hWnd);
void SetWindowStatus(std::string status);
unsigned char getSeed(unsigned char* line);
#endif // BMSD_H_INCLUDED
