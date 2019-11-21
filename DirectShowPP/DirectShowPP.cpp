// DirectShowPP.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "DirectShowPP.h"

#include "MainFrm.h"
#include "DirectShowPPDoc.h"
#include "DirectShowPPView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#include "myresource.h"
#include "BasicPlayer.h"
/////////////////////////////////////////////////////////////////////////////
//添加的标识只运行一次的属性名
CString g_szPropName = "show1980";
HANDLE	g_hValue = (HANDLE)1;
int g_type =SHOW_TYPE_PRO;

int getFileType();
/////////////////////////////////////////////////////////////////////////////
// CDirectShowPPApp

BEGIN_MESSAGE_MAP(CDirectShowPPApp, CWinApp)
	//{{AFX_MSG_MAP(CDirectShowPPApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDirectShowPPApp construction

CDirectShowPPApp::CDirectShowPPApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CDirectShowPPApp object

CDirectShowPPApp theApp;

/////////////////////////////////////////////////////////////////////////////
int ReadRegisterInfo( char *pStrType,  CString &strDescript, CString &strShellOpen)
{
	HKEY hKey;	
	char ptmp[256];
	unsigned long len=256;
	DWORD type=REG_SZ;
    if(RegOpenKey(HKEY_CLASSES_ROOT,pStrType,&hKey)!=ERROR_SUCCESS)
	{
        return  -1;
	}
	if(0!=RegQueryValueEx(hKey, "", 0, &type,(unsigned char*)ptmp,&len))
	{
		return -2;
	}
	strDescript =ptmp;
	RegCloseKey(hKey);			
	sprintf((char*)ptmp,"%s\\shell\\open\\command",strDescript.GetBuffer(0));
	if(RegOpenKey(HKEY_CLASSES_ROOT,ptmp,&hKey)!=ERROR_SUCCESS)
	{
		return -3;
		
	}
	len=256;
	if(0!=RegQueryValueEx(hKey, "", 0, &type,(unsigned char*)ptmp,&len))
	{
		return -2;
	}
	strShellOpen =ptmp;
	RegCloseKey(hKey);		
	return 0;
	
}

void RegisterReg(char *pStrPath,  char *pStrValue)
{
	HKEY hKey;	
	if(RegOpenKey(HKEY_CLASSES_ROOT,pStrPath,&hKey)!=ERROR_SUCCESS)
	{
		if(RegCreateKey(HKEY_CLASSES_ROOT, pStrPath, &hKey)!=ERROR_SUCCESS)
		{
			RegCloseKey(hKey);
			return;	
		}
	}
	RegSetValueEx(hKey, "", 0, REG_SZ,(unsigned char*)pStrValue, strlen(pStrValue)); //.mp3=ssshow_mp3_file
	RegCloseKey(hKey);	
}




void RegisterFileTheApp( char *pStrType,  char *pStrDescript)
{
	HKEY hKey;	
	char ptmp1[256],ptmp2[256];
    RegisterReg(pStrType,pStrDescript); //.mp3=ssshow_mp3_file
	
	sprintf(ptmp1,"\"%s\" \"%c1\"",__argv[0],0x25);	
	sprintf(ptmp2,"%s\\shell\\open\\command",pStrDescript); //ssshow_mp3_file.shell.open.command=xxxxxx,%1
    RegisterReg(ptmp2,ptmp1);
    if(pStrType==".sld" || pStrType==".SLD")
	{
	   sprintf(ptmp1,"\"%s\",1",__argv[0]);	
	}
	else
	{
	   sprintf(ptmp1,"\"%s\",0",__argv[0]);	
	}
	sprintf(ptmp2,"%s\\DefaultIcon",pStrDescript); //ssshow_mp3_file.shell.open.command=xxxxxx,%1
    RegisterReg(ptmp2,ptmp1);

	if(RegOpenKey(HKEY_LOCAL_MACHINE,"SOFTWARE\\ssshow",&hKey)!=ERROR_SUCCESS)
	{
		if(RegCreateKey(HKEY_LOCAL_MACHINE, "SOFTWARE\\ssshow", &hKey)!=ERROR_SUCCESS)		
		{
			RegCloseKey(hKey);
			return;	
		}
		else
		{
	        RegSetValueEx(hKey, "", 0, REG_SZ,(const unsigned char*)"0",1);
	        RegCloseKey(hKey);		
		}
	}	
}


void RemoveRegister(char *pStrType,  char *pStrDescript)
{	
	char ptmp[256];	
	RegDeleteKey(HKEY_CLASSES_ROOT,pStrType);
	sprintf(ptmp,"%s\\shell\\open\\command",pStrDescript);
	RegDeleteKey(HKEY_CLASSES_ROOT,ptmp);
	sprintf(ptmp,"%s\\shell\\open",pStrDescript);
	RegDeleteKey(HKEY_CLASSES_ROOT,ptmp);
	sprintf(ptmp,"%s\\shell",pStrDescript);
	RegDeleteKey(HKEY_CLASSES_ROOT,ptmp);
	sprintf(ptmp,"%s",pStrDescript);
	RegDeleteKey(HKEY_CLASSES_ROOT,ptmp);
	RegDeleteKey(HKEY_LOCAL_MACHINE, "SOFTWARE\\ssshow");		
		
}
//添加一个枚举窗口的函数
BOOL CALLBACK EnumWndProc(HWND hwnd,LPARAM lParam)
{
	HANDLE h = GetProp(hwnd,g_szPropName);
	if( h == g_hValue)
	{
		*(HWND*)lParam = hwnd;
		return false;
	}
	
	return true;
}
#include "Comdlg.h"
BOOL CDirectShowPPApp::InitInstance()
{
	if(__argv[1]!=NULL)
	{
		if(strcmp(__argv[1],"REG_FILE")==0)
		{
			return FALSE;
		}
		if((strcmp(__argv[1],"UNREG_FILE")==0))
		{
		   return FALSE;
		}
	}
	g_head.autosize=  3;
	g_head.delaytime= 3;
	g_head.direct= 2;
	g_head.speed= 1;
    g_type =getFileType();
	switch(g_type) {
	case SHOW_TYPE_EXE:
		break;
	case SHOW_TYPE_SCR:
		{
			if(__argv[1])
			{
				CString strcmd =__argv[1];
				strcmd.MakeUpper();
				if(strcmd.Left(2)=="/C") 
				{
					::AfxMessageBox("此屏幕保护程序没有可以设置的选项");
					return FALSE;
				}
				if(strcmd!="/S") return FALSE;
			}
			else
			{
				return FALSE;
			}
		}
		break;
	case SHOW_TYPE_PRO:
		break;
	default:
		{
			::AfxMessageBox("错误的文件类型");
			return FALSE;
		}
	}	

	//查找是否有本程序的前一个实例运行
	HWND oldHWnd = NULL;
	EnumWindows(EnumWndProc,(LPARAM)&oldHWnd);	//枚举所有运行的窗口
	if(oldHWnd != NULL)
	{
		if(g_type==SHOW_TYPE_PRO)
		::SendMessage(oldHWnd,WM_MYMESSAGE_RUN,0,0);
		//		::ShowWindow(oldHWnd,SW_SHOWNORMAL);			//激活找到的前一个程序
		//		::SetForegroundWindow(oldHWnd);			//把它设为前景窗口
		//		return false;							//退出本次运行

	}

	// Change the registry key under which our settings are stored.
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization.
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	LoadStdProfileSettings();  // Load standard INI file options (including MRU)

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CDirectShowPPDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CDirectShowPPView));
	AddDocTemplate(pDocTemplate);

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

    if(((CMainFrame*)m_pMainWnd)->InitMyMsg()== -1) 
		return FALSE; 

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About



CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CDirectShowPPApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

static int getFileType()
{
#if 1
	T_FILE_HEADER head;
	int magic=0;
	UINT type=SHOW_TYPE_PRO;
	bool bFind =false;
	CFile file;
	int ii=0;
    while(!file.Open(__argv[0],CFile::modeRead|CFile::shareDenyNone))
	{
		Sleep(100);
		ii++;
		if(ii>20) return -1;
	}
#ifdef _DEBUG
	file.Seek(2860*1024,0);
#else
   file.Seek(1250*1024,0);
#endif 
    while(file.Read((void*)&magic,sizeof(UINT))==sizeof(UINT))
	{
		if(magic==MAGIC_NUM) 
		{
			bFind =true;
			break;
		}
	}
    if(bFind)
	{  
	    if(file.Read(&head,sizeof(T_FILE_HEADER))==sizeof(T_FILE_HEADER))
		{
             type =head.type;   
		}
		else
		{
           type =-1;
		}

	} 
	file.Close();
	return type;
#endif 
}
/////////////////////////////////////////////////////////////////////////////
// CDirectShowPPApp message handlers

