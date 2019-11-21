// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "DirectShowPP.h"
#include "MainFrm.h"
#include <afxmt.h> 
#include "myresource.h"
#include "iniFile.h"
#include "shlwapi.h"
#include "probar.h"
#include "afxtempl.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
//int  gAutoHideSwitch =0;
//int  g_delayTime=3;
//int  g_speed=1;
//int  g_direct =2;
//int  g_autosize=0;
//BOOL g_sound =FALSE;

extern char path[];
/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_MESSAGE(WM_MYMESSAGE_NOTIFY,OnMsgNotify)	
	ON_WM_TIMER()
	ON_MESSAGE(WM_MYMESSAGE_RUN,OnRevMsgExit)	
	ON_MESSAGE(WM_MYMESSAGE_EXIT,OnTuiChu)	
	ON_MESSAGE(WM_MYMESSAGE_STARTPLAY,StartPlay)	
	
	
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here	
}

CMainFrame::~CMainFrame()
{
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
/*cf8000*/	
//	cs.style  &=(~WS_SYSMENU); //;WS_POPUP;       //使主窗口不可见
	cs.style =WS_POPUP;


//	cs.dwExStyle |=WS_EX_TOOLWINDOW;//不显示任务按钮		
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;	
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers
#include "myresource.h"
extern int g_type;
extern CString strMusic;
extern CString gstrSong;
extern void ExtractFile(char* strfile,char* strpath);
int extractflag=0;
VOID extract_file_thread(void* para)
{

	ExtractFile(__argv[0],path);	
	extractflag=1;	
}
extern int g_client_bottom;
int CMainFrame::InitMyMsg()
{
	this->SetWindowText("ssshow v1.0 release");	
	m_pShowDlg =new CDlgShow;
	m_pShowDlg->Create(IDD_DIALOG1);
	switch(g_type) {
	case SHOW_TYPE_EXE:
	case SHOW_TYPE_SCR:
		{//释放文件并演示
			CString strcmd;
			//获取图片信息,并释放到临时文件,设置path为临时目录,并放映
			char tmppath[MAX_PATH];
			tmppath[0]='\0';
			CString filepath;
			GetTempPath(MAX_PATH,tmppath);
			filepath =tmppath;
			filepath +="\\sld";
			SYSTEMTIME now;
			GetLocalTime(&now);
			UINT  appendnum =now.wHour*3600+now.wMinute*60+now.wSecond;
			CString strr=filepath;
			filepath.Format("%s%d",strr.GetBuffer(0),appendnum);
			if (!PathFileExists(filepath))
			{
				if (!CreateDirectory(filepath,NULL)) 
				{
					return 0;			
				}
			}	 
			strcpy(path,filepath.GetBuffer(0));
//            ::AfxMessageBox(path);
			m_pShowDlg->SlideShow();
            AfxBeginThread((AFX_THREADPROC)extract_file_thread,(void*)this,THREAD_PRIORITY_NORMAL);
			//检查music.txt是否存在,如果存在则取出
			return 0;
		}
		break;
	case SHOW_TYPE_PRO:
		{
			//打开配置文件
			CString strFile =__argv[0];
			strFile.MakeUpper();
			strFile.Replace(".EXE",".INI");
			OnRun();						
			if(__argv[1]!=NULL)
			{
				CString strtmp;
				strtmp=__argv[1];
				strtmp.MakeUpper();
				if(strtmp.Right(4)==".SLD")
				{
					m_pShowDlg->LoadSldFile(__argv[1]);					
				}
				else if(strtmp.Right(4)==".MP3")
				{
                    m_pShowDlg->LoadSldFile(strFile.GetBuffer(0));
					m_pShowDlg->OnOpenMusic(__argv[1]);					
				}
				else
				{
					::AfxMessageBox("不可识别的文件");
				}
				m_pShowDlg->SlideShow();
			}
			else
			{
				if(m_pShowDlg->LoadSldFile(strFile.GetBuffer(0)))
				{
				  gstrSong =strMusic;
				  gstrSong.MakeUpper();
				  if(!strMusic.IsEmpty())  m_pShowDlg->OnNotifyPlay(0,0);	
				}
				else
				{
					CFile file;
                    if(file.Open(strFile,CFile::modeCreate))
					{
					file.Close();
					}
				}
				extractflag=1;
				m_pShowDlg->SlideShow();					  				
			}
			//m_pShowDlg->ShowWindow(SW_NORMAL);			
		}
		break;
	default: ;
	}
	return 0;
}

LRESULT CMainFrame::OnTuiChu(WPARAM wParam, LPARAM lPara)
{	
//	if(m_pShowDlg) 
//	{
//        m_pShowDlg->OnClose();
//		delete m_pShowDlg;
//	}
//	NOTIFYICONDATA tnid;
//	tnid.cbSize=sizeof(NOTIFYICONDATA);
//	tnid.hWnd=this->m_hWnd;
//	tnid.uID=IDR_MAINFRAME;
//	Shell_NotifyIcon(NIM_DELETE,&tnid);	

    if(m_pShowDlg && m_pShowDlg->m_hWnd) delete m_pShowDlg;	
	CFrameWnd::OnClose();	
	return 0;
}

LRESULT CMainFrame::OnMsgNotify(WPARAM wParam, LPARAM lParam)
{
  return 0;
}


extern CString g_szPropName;
extern HANDLE	g_hValue;
void CMainFrame::OnRun()
{
	SetProp(m_hWnd,g_szPropName,g_hValue);	
}


LRESULT CMainFrame::OnRevMsgExit(WPARAM,LPARAM)
{
   if(g_type==SHOW_TYPE_EXE||g_type==SHOW_TYPE_SCR) return 0;
   else 
   {
	   m_pShowDlg->OnClose();
   }
   return 0;
}

CDlgShow* CMainFrame::GetShowWindow()
{
  return m_pShowDlg;
}

LRESULT CMainFrame::StartPlay(WPARAM wParam, LPARAM lParam)
{
	CFile file;
	char fnmusic[256];
	sprintf(fnmusic,"%s\\music.txt",path);
	if(file.Open(fnmusic,CFile::modeRead))
	{
		char* pbuf=NULL;
		int len=file.GetLength();
		if(len)
		{
			pbuf=new char[len+1];
			file.Read(pbuf,len);
			pbuf[len]=0;
			char* pToken =strtok(pbuf,";");
			while (pToken) 
			{
				m_pShowDlg->InsertMusic(pToken);
				pToken=strtok(NULL,";");
			}
			m_pShowDlg->m_pListWindow->GetNextPlay();
			m_pShowDlg->OnNotifyPlay(0,0);						
		}					
		file.Close();
	}
	return 0;
}
