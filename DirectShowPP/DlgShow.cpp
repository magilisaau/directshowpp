// ShowDlg2.cpp : implementation file
// 

#include "stdafx.h" 
#include "JpegFile.h"
#include "BMPFile.h"
#include "wave.h"
#include "Configdlg.h"
#include "myresource.h"
#include "afxtempl.h"
#include "proBar.h" 
#include "iniFile.h"
#include "DirectShowPP.h"
#include "DlgShow.h"
#include "MainFrm.h"
#include "DlgAss.h"
#include "Zoom.h"
#include "afxmt.h"
#include "afxtempl.h"
#include "des.h"
#include "dlgInputPwd.h"
#include "DlgCertificate.h"
extern int g_type;
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
extern int g_type;
extern CWinThread* g_pThread;
#define  PIC_MIDDLE_WIDTH  485
#define  PIC_MIDDLE_HEIGHT 40
#define FRAME_WIDTH 0
extern CString gstrSong;
int   FONT_HIGH =24;
int   FONT_WIDTH =12;
int g_bStick =1;
char path[500];
int  g_magic_offset  =1;
T_FILE_HEADER g_head;
CString strMusic;
CString strLrc;
bool g_setChanged =FALSE;
BOOL g_includeSubDir =false;
CWinThread* g_pThread =NULL,*g_pThread2=NULL;
//int g_foward =0;
volatile int g_threadExitFlag =0;
//CMusicPlay m_player;
CBasicPlayer m_player;
int g_client_top =REV_CLIENT_TOP;
int g_client_bottom =0;
CString g_nextPic;
HANDLE g_semload;  // Semaphore
HANDLE g_semshow; // Semaphore
extern void RemoveRegister(char *pStrType,  char *pStrDescript);
extern int ReadRegisterInfo( char *pStrType,  CString &strDescript, CString &strShellOpen);
extern void RegisterReg(char *pStrPath,  char *pStrValue);
extern void RegisterFileTheApp( char *pStrType,  char *pStrDescript);

BEGIN_MESSAGE_MAP(CDlgShow, CDialog)
//{{AFX_MSG_MAP(CDlgShow)
ON_WM_LBUTTONDBLCLK()
ON_WM_CTLCOLOR()
ON_WM_SIZE()
ON_WM_NCLBUTTONDOWN()
ON_WM_RBUTTONDOWN()
ON_WM_DROPFILES()
ON_WM_MOUSEMOVE()
ON_WM_TIMER()
ON_WM_NCLBUTTONDBLCLK()
ON_WM_CREATE()
ON_WM_NCCALCSIZE()
ON_WM_SYSKEYDOWN()
ON_WM_SYSCOMMAND()
ON_WM_NCPAINT()
ON_WM_NCMOUSEMOVE()
ON_WM_NCLBUTTONUP()
	ON_WM_GETMINMAXINFO()
ON_WM_CLOSE()
ON_COMMAND(WM_MYMESSAGE_BUTTON_CLICK,OnPushButton)
ON_COMMAND(WM_MYMESSAGE_SETTING,OnSetting)	
ON_COMMAND(WM_MYMESSAGE_EXIT,OnExit)
ON_COMMAND(WM_MYMESSAGE_EXE,OnMakeExe)
ON_COMMAND(WM_MYMESSAGE_SCR,OnMakeScr)
ON_COMMAND(WM_MYMESSAGE_ASS,OnAssociateLrc)
ON_COMMAND(WM_MYMESSAGE_LINK_FILE,OnLinkFile)
ON_COMMAND(WM_MYMESSAGE_UNLINK_FILE,OnUnLinkFile)
ON_COMMAND(WM_MYMESSAGE_OPEN,OnOpenSlide)
ON_COMMAND(WM_MYMESSAGE_SAVE,OnSaveSlide)
ON_COMMAND(WM_MYMESSAGE_SETPATH,OnSetPath)
ON_COMMAND(WM_MYMESSAGE_SETLRCPATH,OnSetLrcPath)
ON_COMMAND(WM_MYMESSAGE_SELECTMUSIC,OnSetMusic)
ON_COMMAND(WM_MYMESSAGE_OPEN_CLICK,OnSetMusic)
ON_COMMAND(WM_MYMESSAGE_STOP_CLICK,OnButtonStop)
ON_COMMAND(WM_MYMESSAGE_SHOW_CLICK,OnButtonShow)
ON_COMMAND(WM_MYMESSAGE_PICDIR_CLICK,OnSetPath)
ON_COMMAND(WM_MYMESSAGE_SETTOP,OnSetTop)
ON_WM_MOUSEWHEEL()
ON_COMMAND(WM_MYMESSAGE_GUANYU,OnAbout)
ON_COMMAND(WM_MYMESSAGE_RESTORE,OnRestore)
ON_COMMAND(WM_MYMESSAGE_EXITTRAY,OnExitTray)
ON_COMMAND(WM_MYMESSAGE_FORWARD,OnForward)	
ON_COMMAND(WM_MYMESSAGE_BACK,OnBack)	

	//}}AFX_MSG_MAP
ON_MESSAGE(MM_MCINOTIFY,OnEndPlay)	
ON_MESSAGE(WM_MYMESSAGE_NOTIFY_PLAY,OnNotifyPlay)
ON_MESSAGE(WM_MYMESSAGE_NOTIFY,OnClickTray)	
ON_WM_LBUTTONDOWN()
ON_WM_SETFOCUS()
ON_WM_KILLFOCUS()
ON_WM_ACTIVATE()
END_MESSAGE_MAP()

/* Handle to a DIB */
#include "dibapi.h"
extern void PicZoom_BilInear0(const TPicRegion& Dst,const TPicRegion& Src);
extern void PicZoom_BilInear3(const TPicRegion& Dst,const TPicRegion& Src);
extern void PicZoom_BilInear2(const TPicRegion& Dst,const TPicRegion& Src);
extern void PicZoom_ThreeOrder0(const TPicRegion& Dst,const TPicRegion& Src);
HGLOBAL WINAPI ZoomDIB(LPSTR lpDIB, LPSTR lpDataBuf,float fXZoomRatio, float fYZoomRatio)
{
	TPicRegion Tnew;
	TPicRegion Told;
	Told.height = ::DIBHeight(lpDIB);
	Told.width =  ::DIBWidth(lpDIB);
	Told.pdata = (TARGB32*)lpDataBuf;
	Told.byte_width = Told.width*3;

	Tnew.height = (long)(Told.height*fYZoomRatio+0.5);
	Tnew.width = (long)(Told.width*fXZoomRatio+0.5);
	Tnew.pdata = (TARGB32*)new char[Tnew.width*Tnew.height*3];
	Tnew.byte_width = Tnew.width*3;
	if(!Tnew.pdata) return NULL;
    PicZoom_BilInear2(Tnew,Told); 
	/*PicZoom_ThreeOrder0(Tnew,Told);*/
	return Tnew.pdata;
}

//释放资源文件
static LONG GetDataFromRes(UINT nIDResource,LPSTR name) 
{
	LPSTR szResourceName = MAKEINTRESOURCE(LOWORD(nIDResource));
	LPSTR szResourceType = "EXE";
	HRSRC hRes = FindResource(NULL, szResourceName,szResourceType);
	HGLOBAL hResData;
	if (!hRes || !(hResData = LoadResource(NULL,hRes))) 
	{ return -1; }; 
	DWORD dwSize = SizeofResource(NULL,hRes);
	PBYTE pSrc = (PBYTE)LockResource(hResData); 
	if (!pSrc)  
	{ 
		FreeResource(hResData); 
		return -1;
	}; 
	FILE* fp =NULL;
	if((fp=fopen(name,"wb"))==NULL)
	{
		return -1;
	}
	fwrite(pSrc,dwSize,1,fp);
	FreeResource(hResData);
	fclose(fp);
	return dwSize;
}

CDlgShow::CDlgShow(CWnd* pParent /*=NULL*/) 
	: CDialog(CDlgShow::IDD, pParent)
{
	m_abc=-1;
    m_bTop =0;
	m_buf =NULL;
	m_pause=0;
	m_ShowTitle =TRUE;
	m_time=0;
	m_iPos=0;
	m_playFlag =0;
	m_pListWindow =0;
	m_bListShow=1;
	m_ButtonPosx=0;
    m_ButtonPosy=0;
	m_flagDestroy =0;
	m_slideRatio =0.0;
	m_curLycris ="";
	m_nextLycris="";
	lpDDPic=0;
	lpDDSBack=0;
	lpBackSurface=0;
	m_forwardflag =-1;
    if(g_type==SHOW_TYPE_PRO)
	{
		CString strFileName =__argv[0];
        strFileName.MakeUpper();
		strFileName.Replace(".EXE",".BUF");
		m_lrcBuf.Load(strFileName);
		//load sld file
	}
	else
	{//2015,liuxb
		m_bListShow=0;
	}
	//{{AFX_DATA_INIT(CDlgShow)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDlgShow::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgShow)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

CString ParseSong(CString strFile)
{
	CString strr =strFile;
	int iPosStart =strr.FindOneOf("-._");
	if(iPosStart!=-1) 
	{
		strr =strr.Right(strr.GetLength()-iPosStart-1);
	}
	else
	{
        iPosStart =strr.FindOneOf("－");
		if(iPosStart!=-1) strr =strr.Right(strr.GetLength()-iPosStart-2);
	}

	strr.TrimLeft();
	return strr;
}

/////////////////////////////////////////////////////////////////////////////
// CDlgShow message handlers
extern int extractflag;
VOID ShowThread(void* param)
{
	if(param==NULL) return;
	BOOL bFind=FALSE;
	if(g_type!=SHOW_TYPE_PRO)
	{
		while(extractflag==0)
		{
			Sleep(10);
		}						
	}
	g_threadExitFlag =0;	
	CDlgShow* pDlg =(CDlgShow*)param;
	while (1)
	{
		char tmppath[MAX_PATH_LENGTH];
		GetCurrentDirectory(MAX_PATH_LENGTH,tmppath);
		bFind =FALSE;
		CFileFind ff;
		Sleep(10);
		CArray<CString,CString> dirArray;
		if(strlen(path)==0)
		{
		 strcpy(path,tmppath);
		}
		dirArray.Add(path);
		while(dirArray.GetSize())
		{
			CArray<CString,CString>  fileArray;
			CString strCurrentDir;
			strCurrentDir =dirArray.GetAt(0);
			dirArray.RemoveAt(0);
			if(strCurrentDir.Right(1)!="\\") strCurrentDir+="\\*.*";
			BOOL res = ff.FindFile(strCurrentDir);
			while( res )
			{
				res = ff.FindNextFile();
				if(ff.IsDots()) continue;
				if(ff.IsDirectory())
				{	
					if(g_includeSubDir) dirArray.Add(ff.GetFilePath());
				}
				else
				{   
					CString filename =ff.GetFileName();
					filename.MakeUpper();
					if(filename.Right(4) ==".JPG"  || 
					   filename.Right(5) ==".JPEG" ||
					   filename.Right(4) ==".BMP")
					{
						bFind =TRUE;
						if(g_client_bottom!=0)
						{
							g_client_bottom=0;
                            pDlg->ReShow();
						}
						fileArray.Add(ff.GetFilePath());
					}
				}
				if(g_threadExitFlag==1) break;
				
			}
			ff.Close();
			// show 
//			g_threadExitFlag=0;
			int filenum =fileArray.GetSize();
			srand(GetCurrentTime());
           
			while (filenum)
			{		
				int findex=rand()%filenum;
				WaitForSingleObject(g_semload,INFINITE);
				if(g_threadExitFlag==1) 
				{
					break;		
				}
				if(pDlg->LoadPicFile(fileArray[findex].GetBuffer(0)))
				{
				   g_nextPic =fileArray[findex].GetBuffer(0);
				}
				else
				{
					g_nextPic.Empty();
				}
			    ReleaseSemaphore(g_semshow,1,NULL);
				fileArray.RemoveAt(findex);
				filenum--;
			}
			if(g_threadExitFlag==1) break;	
		 }//while
		if(g_threadExitFlag==1) 
		{
			break;	
		}		
		if(!bFind) 
		{
            g_client_bottom=REV_CLIENT_BOTTOM;
			pDlg->ReShow();
    		pDlg->updateSpectrum();
			break;
		}
		Sleep(1);
  }//while(1)

}

VOID ShowThread2(void* param)
{
	if(param==NULL) 
	{
		return;
	}
	CDlgShow* pDlg =(CDlgShow*)param;	
	while(1)
	{
		if(g_threadExitFlag==1)
		{
			break;
		}				
		pDlg->ShowPic();
	}
}


static int BitShift(DWORD bits)
{
	for (int i=0; i<32; i++) {
		if (bits & 1)
			return i;
		bits >>= 1;
	}
	return -1;
}
inline void tracetime(CString strr)
{
	LARGE_INTEGER lg;
    QueryPerformanceCounter(&lg);
	TRACE(strr);
	TRACE(" time is %x %x\n",lg.u.HighPart,lg.u.LowPart);
}
BOOL CDlgShow::LoadPicFile(CString filename)
{	

	CString strext=filename.Right(4);
	strext.MakeUpper();
	if(strext==".BMP")
	{
		return LoadBMP(filename.GetBuffer(0));
	}
	else
	{
		return LoadJPG(filename.GetBuffer(0));
	}
}

HRESULT CDlgShow::ShowPic()
{
	BYTE *tmp =NULL;
	CDC *theDC =NULL;
	LARGE_INTEGER lg1,lg2;	
	BITMAPINFOHEADER bmiHeader;	
	int posx=0,posy =0;
	float factor=1;		
	//这里申请信号量sem1
	WaitForSingleObject(g_semshow,INFINITE);
	if(g_threadExitFlag==1) 
	{
		ReleaseSemaphore(g_semload,1,NULL);
		return 0;	
	}
    if(m_buf==NULL) 
	{
		ReleaseSemaphore(g_semload,1,NULL);
		return E_FAIL;
	}
    m_curPic =g_nextPic;
	if(m_curPic.IsEmpty())
	{
		ReleaseSemaphore(g_semload,1,NULL);
		return E_FAIL;
	}
	theDC = GetDC();
	if(theDC==NULL)
	{
		ReleaseSemaphore(g_semload,1,NULL);
		return E_FAIL;
	}
	
	SetStretchBltMode(theDC->GetSafeHdc(),COLORONCOLOR);
	// set up a DIB 
	bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmiHeader.biWidth = m_width;
	bmiHeader.biHeight = m_height;
	bmiHeader.biPlanes = 1;
	bmiHeader.biBitCount = 24;
	bmiHeader.biCompression = BI_RGB;
	bmiHeader.biSizeImage = 0;
	bmiHeader.biXPelsPerMeter = 0;
	bmiHeader.biYPelsPerMeter = 0;
	bmiHeader.biClrUsed = 0;
	bmiHeader.biClrImportant = 0;
	
	int current_direct;
    switch(g_head.direct) {
    case 0:
    case 1:
		current_direct =g_head.direct;
		break;
	case 2:
    default: 
		if(m_height*1.0/m_width > window_height*1.0/window_width)
			current_direct =0 ;
		else
			current_direct =1 ;
		break;
    } 

	unsigned char* pNewBuf =m_buf;
	if(current_direct==0)//纵向
	{
		if(g_head.autosize==0)//保持原尺寸
		{
			factor =1.0;
			posx =(int)(window_width-m_width)/2;			
		}
		else if(g_head.autosize==1)//滑动效果
		{
			
			if(m_width>window_width)
			{
				factor =(float)(1.0*window_width/m_width);
			}

			else
			{
				factor=1.0;
				posx =(int)(window_width-m_width)/2;
			}		

		}
		else if(g_head.autosize==2)//适合屏幕
		{
              factor =(float)(1.0*window_height/m_height);
			  posx =(int)(window_width-m_width*factor)/2;
		}
		else //充满屏幕
		{
				factor =(float)(1.0*window_width/m_width);
		}
        if(factor<=0.90 || factor>=1.1)
		{//需要进行变换
          pNewBuf =(unsigned char*)ZoomDIB((LPSTR)&bmiHeader,(LPSTR)m_buf,(float)factor,(float)factor);
		  if(pNewBuf==NULL)
		  {
             ReleaseSemaphore(g_semload,1,NULL);
             goto _EXIT1;
		  }
	      tracetime("ZoomDIB");
		  bmiHeader.biWidth =(long)(bmiHeader.biWidth*factor+0.5);
		  bmiHeader.biHeight =(long)(bmiHeader.biHeight*factor+0.5);
		  //此处销毁m_buf
		  delete[] m_buf;
		  m_buf =NULL;
		}
		tmp = JpegFile::MakeDwordAlignedBuf(pNewBuf,
			bmiHeader.biWidth,
			bmiHeader.biHeight,
			&m_widthDW);
		tracetime("MakeDwordAlignedBuf");
        delete[] pNewBuf;
		m_buf=NULL;
		//这里释放信号量sem2
		ReleaseSemaphore(g_semload,1,NULL);

		int ii =0;

        //usr directdraw 
		//创建离屏页面
		CRect clrct;
		DDSURFACEDESC ddsd;
		ZeroMemory(&ddsd, sizeof(ddsd));
		ddsd.dwSize = sizeof(ddsd);
		ddsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;    
		ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
		GetClientRect(&clrct);
		ddsd.dwWidth = clrct.Width();
		ddsd.dwHeight = clrct.Height();
		window_width=clrct.Width();
		window_height=clrct.Height();
		HRESULT rst;
		rst =lpDD->CreateSurface( &ddsd, &lpDDSBack, NULL );
		if(!lpDDSBack)
		{
			goto _EXIT1;
		}

		//liuxb 2013
 
	   tracetime("createSurface");

        //创建图像屏幕
		ddsd.dwWidth=bmiHeader.biWidth;
		ddsd.dwHeight=bmiHeader.biHeight;
		rst =lpDD->CreateSurface(&ddsd,&lpDDPic,NULL);
		if(!lpDDPic) 
		{
			goto _EXIT1;
		}
        //将图像拷贝到图像屏幕
		HDC hdc;
		if(lpDDPic->GetDC(&hdc)!=DD_OK)
		{
		   goto _EXIT1;
		}
		StretchDIBits(hdc,
			0 , 0,
			bmiHeader.biWidth,
			bmiHeader.biHeight,
			0,0,
			bmiHeader.biWidth,
			bmiHeader.biHeight,
			tmp, 
			(LPBITMAPINFO)&bmiHeader,
			DIB_RGB_COLORS,
			SRCCOPY);
		lpDDPic->ReleaseDC(hdc);
		tracetime("StretchDIBits");		
		//设置源窗口和目的窗口范围
        CRect srcrct,dstcrt;
        srcrct.left=0;
		srcrct.right=(bmiHeader.biWidth<clrct.Width())?bmiHeader.biWidth:clrct.Width();
		srcrct.top=0;
		srcrct.bottom=(bmiHeader.biHeight<clrct.Height())?bmiHeader.biHeight:clrct.Height();
		
		dstcrt=srcrct;
        if(posx<0)OffsetRect(&srcrct,-posx,0);
		else  OffsetRect(&dstcrt,posx,0);

		if(srcrct.Height()<clrct.Height())
		{
            OffsetRect(&dstcrt,0,(clrct.Height()-srcrct.Height())/2);
		}
		clearScreen();
		
		if(g_head.autosize==3) //充满屏幕的情况
		{
			ii = (bmiHeader.biHeight - (LONG)window_height)/3;
			if(ii>=0) 
			{
				OffsetRect(&srcrct,0,ii); 
                bmiHeader.biHeight = (LONG)window_height; //
			}
			else 
			{
				ii=0;
			}
		}
		int idrag=0;
        //拖拽显示图像
		while(1)
		{   
			if(g_setChanged)
			{
				g_setChanged=false;
				goto _EXIT1;
			}
		     QueryPerformanceCounter(&lg1);
			// lpDDSBack->Lock(NULL,&ddsd,DDLOCK_WAIT|DDLOCK_SURFACEMEMORYPTR, NULL);
			 if(!lpDDSBack) goto _EXIT1;
		     clearScreen();
			 if(lpDDSBack->Blt( &dstcrt, lpDDPic, &srcrct, DDBLT_WAIT , NULL))
			 {
                  //CString strrtm;
				  //strrtm.Format("%d %d %d %d %d %d %d %d",srcrct.left,srcrct.right,srcrct.top,srcrct.bottom,dstcrt.left,dstcrt.right,dstcrt.top,dstcrt.bottom);
				  //::AfxMessageBox(strrtm);
				
			 }
			 ShowLRC();
			 //lpDDSBack->Unlock(NULL);
			 Flip();
			if(g_threadExitFlag) goto _EXIT1;						
			if(idrag==0) 
			{
				for(int mm=0;mm<g_head.delaytime;mm++)
				{
		           // lpDDSBack->Lock(NULL,&ddsd,DDLOCK_WAIT|DDLOCK_SURFACEMEMORYPTR, NULL);
					if(!lpDDSBack) goto _EXIT1;
					clearScreen();
					lpDDSBack->Blt( &dstcrt, lpDDPic, &srcrct, DDBLT_WAIT , NULL);	
					ShowLRC();
			        //lpDDSBack->Unlock(NULL);
					Flip();					
                    MySleep(20);
				}				
				idrag++;
			}
			if((UINT)(bmiHeader.biHeight-ii) <=window_height) 
			{//画面停止几秒,此时歌词应该继续显示才对
			//	MySleep(g_head.delaytime*50);	
				for(int mm=0;mm<g_head.delaytime;mm++)
				{
		            //lpDDSBack->Lock(NULL,&ddsd,DDLOCK_WAIT|DDLOCK_SURFACEMEMORYPTR, NULL);
					if(!lpDDSBack) goto _EXIT1;
		            clearScreen();
					lpDDSBack->Blt( &dstcrt, lpDDPic, &srcrct, DDBLT_WAIT , NULL);	
					ShowLRC();
			        //lpDDSBack->Unlock(NULL);
					Flip();					
                    MySleep(50);
				}
				if(!m_pause) break;
			}		
			while(1)
			{
				QueryPerformanceCounter(&lg2);
				if(lg2.QuadPart -lg1.QuadPart >12000)break;							
			}
 			MySleep(g_head.speed);
			while(m_pause)
			{   //此时歌词应该能够显示
		        //lpDDSBack->Lock(NULL,&ddsd,DDLOCK_WAIT|DDLOCK_SURFACEMEMORYPTR, NULL);
				if(!lpDDSBack) goto _EXIT1;
		        clearScreen();
				lpDDSBack->Blt( &dstcrt, lpDDPic, &srcrct, DDBLT_WAIT , NULL);	
				ShowLRC();
                //lpDDSBack->Unlock(NULL);
				Flip();
				MySleep(50);
			}
			ii++;
			OffsetRect(&srcrct,0,1); 				 				
		}

	}
	else if(current_direct==1)//横向
    {
		if(g_head.autosize==0)//保持原尺寸
		{
			factor =1.0;
			posy =(int)(window_height-m_height)/2;			
		}
		else if(g_head.autosize==1)//滑动效果
		{
		
			if(m_height>window_height)
			{
				factor =(float)(1.0*window_height/m_height);
			}
			
			else
			{
				factor=1.0;
				posy =(int)(window_height-m_height)/2;
			}
			
			
		}
		else if(g_head.autosize==2)//适合屏幕
		{
			factor =(float)(1.0*window_width/m_width);
			posy =(int)(window_height-m_height*factor)/2;
		}		
		else
		{
				factor =(float)(1.0*window_height/m_height);
		}
		if(factor<=0.9 || factor >=1.1)
		{//需要进行变换		
			pNewBuf =(unsigned char*)ZoomDIB((LPSTR)&bmiHeader,(LPSTR)m_buf,(float)factor,(float)factor);
		    if(pNewBuf==NULL)
		    {
               ReleaseSemaphore(g_semload,1,NULL);
               goto _EXIT1;
		    }
			bmiHeader.biWidth =(long)(bmiHeader.biWidth*factor+0.5);
			bmiHeader.biHeight =(long)(bmiHeader.biHeight*factor+0.5);
			//此处销毁m_buf
			delete[] m_buf;
			m_buf =NULL;

		}
		tmp = JpegFile::MakeDwordAlignedBuf(pNewBuf,
			bmiHeader.biWidth,
			bmiHeader.biHeight,
			&m_widthDW);
        if(pNewBuf!=m_buf) delete[]  pNewBuf;
		m_buf=NULL;
		ReleaseSemaphore(g_semload,1,NULL);
		
		int ii =0;
        //usr directdraw 	
		//创建离屏页面
		CRect clrct;
		DDSURFACEDESC ddsd;
		ZeroMemory(&ddsd, sizeof(ddsd));
		ddsd.dwSize = sizeof(ddsd);
		ddsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;    
		ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
		GetClientRect(&clrct);
		ddsd.dwWidth = clrct.Width();
		ddsd.dwHeight = clrct.Height();
		window_height=clrct.Height();
		window_width=clrct.Width();
		HRESULT rst;
		rst =lpDD->CreateSurface( &ddsd, &lpDDSBack, NULL );
		if(!lpDDSBack)
		{
			goto _EXIT1;
		}
        //创建图像屏幕
		ddsd.dwWidth=bmiHeader.biWidth;
		ddsd.dwHeight=bmiHeader.biHeight;
		rst =lpDD->CreateSurface(&ddsd,&lpDDPic,NULL);
		if(!lpDDPic) 
		{

			goto _EXIT1; 
		}
        //将图像拷贝到图像屏幕
		HDC hdc;
		if(lpDDPic->GetDC(&hdc)!=DD_OK)
		{
		   goto _EXIT1;
		}
		StretchDIBits(hdc,
			0 , 0,
			bmiHeader.biWidth,
			bmiHeader.biHeight,
			0,0,
			bmiHeader.biWidth,
			bmiHeader.biHeight,
			tmp,
			(LPBITMAPINFO)&bmiHeader,
			DIB_RGB_COLORS,
			SRCCOPY);
		lpDDPic->ReleaseDC(hdc);
		//设置源窗口和目的窗口范围
        CRect srcrct,dstcrt;
        srcrct.left=0;
		srcrct.right=(bmiHeader.biWidth<clrct.Width())?bmiHeader.biWidth:clrct.Width();
		srcrct.top=0;
		srcrct.bottom=(bmiHeader.biHeight<clrct.Height())?bmiHeader.biHeight:clrct.Height();
		
		dstcrt=srcrct;
        if(posy<0)OffsetRect(&srcrct,0,-posy);
		else  OffsetRect(&dstcrt,0,posy);

		if(srcrct.Width()<clrct.Width())
		{
            OffsetRect(&dstcrt,(clrct.Width()-srcrct.Width())/2,0);
		}

		//清屏
		clearScreen();		

		if(g_head.autosize==3) //充满屏幕的情况，2011.8增加
		{
			ii = (bmiHeader.biWidth - (LONG)window_width)/2;
			if(ii>0) 
			{
				OffsetRect(&srcrct,ii,0); 
				bmiHeader.biWidth= (LONG)window_width; //
			}
			else 
			{
				ii=0;
			}
		}
        //拖拽显示图像
		int idrag=0;
		while(1)
		{    
			if(g_setChanged)
			{
				g_setChanged=false;
				goto _EXIT1;
			}			
		     QueryPerformanceCounter(&lg1);
		    // lpDDSBack->Lock(NULL,&ddsd,DDLOCK_WAIT|DDLOCK_SURFACEMEMORYPTR, NULL);
			 if(!lpDDSBack) goto _EXIT1;
		     clearScreen();	
			 lpDDSBack->Blt( &dstcrt, lpDDPic, &srcrct, DDBLT_WAIT , NULL);
			 ShowLRC();		
			// lpDDSBack->Unlock(NULL);
			 Flip();

			if(g_threadExitFlag) goto _EXIT1;						
			if(idrag==0)
			{
				for(int mm=0;mm<g_head.delaytime;mm++)
				{
					//lpDDSBack->Lock(NULL,&ddsd,DDLOCK_WAIT|DDLOCK_SURFACEMEMORYPTR, NULL);
					if(!lpDDSBack) goto _EXIT1;
                    clearScreen();
					lpDDSBack->Blt( &dstcrt, lpDDPic, &srcrct, DDBLT_WAIT , NULL);					
					ShowLRC();
					Flip();		
					//lpDDSBack->Unlock(NULL);
                    MySleep(20);
				}		
				idrag++;
			}	 
			if((UINT)(bmiHeader.biWidth-ii)<=window_width) 
			{//画面停止几秒
				//MySleep(g_head.delaytime*50);
				for(int mm=0;mm<g_head.delaytime;mm++)
				{
					//lpDDSBack->Lock(NULL,&ddsd,DDLOCK_WAIT|DDLOCK_SURFACEMEMORYPTR, NULL);
					if(!lpDDSBack) goto _EXIT1;
                    clearScreen();
					lpDDSBack->Blt( &dstcrt, lpDDPic, &srcrct, DDBLT_WAIT , NULL);	
					ShowLRC();
					//lpDDSBack->Unlock(NULL);
					Flip();		
                    MySleep(50);
				}
				
				if(!m_pause) break;
			}
			while(1)
			{
				QueryPerformanceCounter(&lg2);
				if(lg2.QuadPart -lg1.QuadPart >12000) break;							
			}
			MySleep(g_head.speed);
			while(m_pause)
			{
				//lpDDSBack->Lock(NULL,&ddsd,DDLOCK_WAIT|DDLOCK_SURFACEMEMORYPTR, NULL);
				if(!lpDDSBack) goto _EXIT1;
				clearScreen();
				lpDDSBack->Blt( &dstcrt, lpDDPic, &srcrct, DDBLT_WAIT , NULL);	
				ShowLRC();	
				//lpDDSBack->Unlock(NULL);
				Flip();	
				MySleep(50);
			}
			ii++;
			OffsetRect(&srcrct,1,0); 				 				
		}//end while(1)
	}
		
_EXIT1:	
	if(lpDDSBack)
	{;
		lpDDSBack->Release();
		lpDDSBack=NULL;
	}
	if(lpDDPic)
	{
		lpDDPic->Release();
        lpDDPic=NULL;
	}
    if(tmp) delete [] tmp;
	m_curPic ="";
	return S_OK; 
}

extern int g_type;
void CDlgShow::OnLButtonDblClk(UINT nFlags, CPoint point) 
{

	if(g_type==SHOW_TYPE_SCR) return;
    if(!this->IsZoomed())  
	{
		this->ShowWindow(SW_HIDE);
        this->ModifyStyle(WS_OVERLAPPED|WS_CAPTION,WS_BORDER,0);	
		ShowWindow(SW_MAXIMIZE);					

	}
	else
	{      
		this->ShowWindow(SW_HIDE);
		this->ModifyStyle(WS_BORDER,WS_OVERLAPPED|WS_CAPTION,0);				
		ShowWindow(SW_NORMAL);
	}

}
void CDlgShow::OnCancel() 
{
	OnClose();
}

HBRUSH CDlgShow::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	
	switch (nCtlColor) { 		
	case CTLCOLOR_EDIT:	
	case CTLCOLOR_MSGBOX:
	case CTLCOLOR_DLG : 
		
	{
			const COLORREF BGColor = RGB(0,0,40);			
			HBRUSH B = CreateSolidBrush(BGColor); //COLOR是你想设置的颜色 
			return (HBRUSH) B;
	}				
	default:  ;//其他控件设置自己默认的颜色和背景刷.		
	}

	return hbr;	

}

void CDlgShow::OnClose() 
{

	// 等待线程退出
	int iTryCnt=0;
	DWORD exitCode =STILL_ACTIVE;
	m_pause =0;
	g_threadExitFlag =1;
	KillTimer(100);
	//m_player.close();
	m_player.Stop();
	if(g_head.forward)
	{
    	ModifyLrc();
	}
	if(g_semload)
	{
		ReleaseSemaphore(g_semload,1,NULL);
	}
	if(g_semshow)
	{
		ReleaseSemaphore(g_semshow,1,NULL);
	}
	while(1)
	{
		Sleep(10);
		if(g_pThread==NULL) break;
		if(!GetExitCodeThread(g_pThread->m_hThread,&exitCode)) 
		{
			g_pThread =NULL;
			break;
		}
		iTryCnt++;
		if(exitCode!=STILL_ACTIVE)
		{
            break;
		}
		else
		{
          if(iTryCnt>=100)
		  {
             if(g_pThread)
			 {
				 g_pThread->Delete();
				 break;
			 }
		  }

		}   
		g_threadExitFlag =1;		
	}
    iTryCnt=0;
	while(1)
	{
		Sleep(10);
		if(g_pThread2==NULL) break;
		if(!GetExitCodeThread(g_pThread2->m_hThread,&exitCode)) 
		{
			g_pThread2 =NULL;
			break;
		}
		iTryCnt++;
		if(exitCode!=STILL_ACTIVE)
		{
            break;
		}
		else
		{
			if(iTryCnt>=100)
			{
				if(g_pThread2)
				{
					g_pThread2->Delete();
					break;
				}
			}
		}        
		g_threadExitFlag =1;		
	}
    if(g_type==SHOW_TYPE_EXE||g_type==SHOW_TYPE_SCR)
	{
		DeleteTmpFile();
	}
	for(int i=0; i<m_buttons.GetSize(); i++)
	{
		delete m_buttons[i];
	}
	m_buttons.RemoveAll();
    if(m_lrcBuf.Changed())
	{
		CString strFileName =__argv[0];
		strFileName.MakeUpper();
		strFileName.Replace(".EXE",".BUF");	
		m_lrcBuf.Store(strFileName);
	}
    // save sld file
	if(g_type == SHOW_TYPE_PRO/* && __argv[1]==NULL*/)
	{
		CString strFile=__argv[0];
		strFile.MakeUpper();
		strFile.Replace(".EXE",".INI");
		SaveSldFile(strFile.GetBuffer(0));
	}
	if(m_pListWindow)
	{
		delete m_pListWindow;
		m_pListWindow=NULL;
	}	
	m_lrc.Free();
	CDialog::OnCancel();	
    m_flagDestroy =1;
	::SendMessage(AfxGetMainWnd()->GetSafeHwnd(),WM_MYMESSAGE_EXIT,0,0);	
}
void CDlgShow::OnSize(UINT nType, int cx, int cy) 
{
	updateSpectrum();
	CDialog::OnSize(nType, cx, cy);
    //g_setChanged =TRUE;
	// TODO: Add your message handler code here
	
}
BOOL CDlgShow::LoadJPG(CString fileName)
{
	// m_buf is the global buffer
	if (m_buf!=NULL) {
		delete [] m_buf;
		m_buf=NULL;
	}
	
	// read to buffer tmp
	m_buf=JpegFile::JpegFileToRGB(fileName, &m_width, &m_height);
	if(!m_buf) return FALSE;
	//////////////////////
	// set up for display
	
	// do this before DWORD-alignment!!!
	// this works on packed (not DWORD-aligned) buffers
	// swap red and blue for display
	if(JpegFile::BGRFromRGB(m_buf, m_width, m_height)==FALSE)
	{
		delete [] m_buf;
		m_buf=NULL;
		return FALSE;
	}
	
	// vertical flip for display
	if(JpegFile::VertFlipBuf(m_buf, m_width * 3, m_height)==FALSE)
	{	
		delete [] m_buf;
		m_buf=NULL;
		return FALSE;
	}
	return TRUE;
}

BOOL CDlgShow::LoadBMP(CString fileName)
{
	if (m_buf!=NULL) {
		delete [] m_buf;
	}
	BMPFile theBmpFile;
	m_buf=theBmpFile.LoadBMP(fileName, &m_width, &m_height);
	if ((m_buf==NULL) || (theBmpFile.m_errorText!="OK")) 
	{
//		AfxMessageBox(theBmpFile.m_errorText);
		m_buf=NULL;
		return FALSE;
	}
	
	//////////////////////
	// set up for display
	
	// do this before DWORD-alignment!!!
	// this works on packed (not DWORD-aligned) buffers
	// swap red and blue for display
	if(JpegFile::BGRFromRGB(m_buf, m_width, m_height)==FALSE)
	{
		delete [] m_buf;
		m_buf=NULL;
		return FALSE;
	}
	
	// vertical flip for display
	if(JpegFile::VertFlipBuf(m_buf, m_width * 3, m_height)==FALSE)
	{
		delete [] m_buf;
		m_buf=NULL;
		return FALSE;
	}
	return TRUE;
}
int g_leftMetrics=0;
BOOL CDlgShow::OnInitDialog() 
{
	CDialog::OnInitDialog();	
	g_leftMetrics=GetSystemMetrics(SM_CXFRAME)+1;
	g_client_top =(42-GetSystemMetrics(SM_CYSIZE)-GetSystemMetrics(SM_CYFRAME));//(PIC_MIDDLE_HEIGHT-GetSystemMetrics(SM_CYSIZE)-GetSystemMetrics(SM_CYFRAME)+1)+3;
	if(g_type==SHOW_TYPE_EXE || g_type ==SHOW_TYPE_SCR)
	{
		g_client_bottom =0;
	}
	if(g_type==SHOW_TYPE_SCR)
	{
	    //g_client_top =0;
		ModifyStyle(WS_OVERLAPPED|WS_CAPTION,WS_BORDER,0);	
		ShowWindow(SW_MAXIMIZE);
	}
	else
	{
		tagRECT rt,rtthis;
		GetDesktopWindow()->GetWindowRect(&rt);
		GetWindowRect(&rtthis);
		int wi,hi,wid,hid;
		wi =rtthis.right-rtthis.left;
		hi =rtthis.bottom-rtthis.top;
		wid =rt.right-rt.left;
		hid =rt.bottom -rt.top;	
		rtthis.left =(wid-wi)/2;
		rtthis.right =rtthis.left+wi;
		rtthis.top =(hid-hi)/2;
		rtthis.bottom =rtthis.top+hi;
		this->MoveWindow(&rtthis);
	}

	// 建立一个Imagelist 控件, 	
	HIMAGELIST himl; 	
	//MYICON_CX,MYICON_CY是每个按钮的大小 	
	himl= ImageList_Create(40,38,ILC_COLOR32,0,4); 	
	//加入事先作好的工具条位图IDB_BITMAP2 	
	ImageList_Add( himl,
		LoadBitmap(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDB_PLAY_BUTTON)),NULL);  
	AddButton(WM_MYMESSAGE_BUTTON_CLICK,0,1,40, 38, himl,"Play/Pause");	
//move to here for all type

	tagRECT rt;
	GetWindowRect(&rt);
	m_pListWindow =new CComDlg(this);
	m_pListWindow->Create(IDD_COMDLG_DIALOG);			
	m_pListWindow->MoveWindow(rt.right,rt.top,250,rt.bottom-rt.top);
	if(g_type ==SHOW_TYPE_PRO)
	{
        
 		if(m_bListShow) m_pListWindow->ShowWindow(SW_SHOW);
		/*添加button*/		
		HIMAGELIST himl; 					
		//MYICON_CX,MYICON_CY是每个按钮的大小 	
		himl= ImageList_Create(40,26,ILC_COLOR32,0,4); 	
		//加入事先作好的工具条位图IDB_BITMAP2 	
		ImageList_Add( himl,
			LoadBitmap(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDB_STOP_BUTTON)),NULL);  
		AddButton(WM_MYMESSAGE_STOP_CLICK,62,7,40, 26, himl,"Stop");	
		
		
		//MYICON_CX,MYICON_CY是每个按钮的大小 	
		himl= ImageList_Create(40,26,ILC_COLOR32,0,4); 	
		//加入事先作好的工具条位图IDB_BITMAP2 	
		ImageList_Add( himl,
			LoadBitmap(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDB_OPEN_BUTTON)),NULL);  
		AddButton(WM_MYMESSAGE_OPEN_CLICK,-60,6,40, 26, himl,"Open Music File");	
		ChangeButtonState(2,CAPTION_BTN_NORMAL);
		
		
		//MYICON_CX,MYICON_CY是每个按钮的大小 	
		himl= ImageList_Create(40,25,ILC_COLOR32,0,4); 	
		//加入事先作好的工具条位图IDB_BITMAP2 	
		ImageList_Add( himl,
			LoadBitmap(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDB_SHOW_BUTTON)),NULL);  
		AddButton(WM_MYMESSAGE_SHOW_CLICK,120,7,40, 25, himl,"Display/Hide Playlist");	
		if(m_bListShow) ChangeButtonState(3,CAPTION_BTN_CHECKED);
		else ChangeButtonState(3,CAPTION_BTN_NORMAL);
		
		//MYICON_CX,MYICON_CY是每个按钮的大小 	
		himl= ImageList_Create(40,26,ILC_COLOR32,0,4); 	
		//加入事先作好的工具条位图IDB_BITMAP2 	
		ImageList_Add( himl,
			LoadBitmap(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDB_PIC_CLICK)),NULL);  
		AddButton(WM_MYMESSAGE_PICDIR_CLICK,-120,7,40, 26, himl,"Select Picture Directory");	
		ChangeButtonState(4,CAPTION_BTN_NORMAL);		
	}
    m_slideTop =39;	
	this->SetTimer(100,1,NULL);	
	//创建directdraw
	InitDDraw();
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void CDlgShow::OnExit()
{
	OnClose();	
}

void CDlgShow::Warning()
{
    tagRECT tr;
	GetWindowRect(&tr);
	CDC *theDC =NULL;
	theDC = GetDC();
   	
  
	CFont titleFont;	
	titleFont.CreateFont( 30, // nHeight 
		12, // nWidth 
		0, // nEscapement文本行逆时针旋转角度 
		0, // nOrientation字体角度
		FW_THIN, // nWeight字体粗细程度 
		FALSE, // bItalic 
		FALSE, // bUnderline 
		0, // cStrikeOut 删除线
		ANSI_CHARSET, // nCharSet 
		OUT_DEFAULT_PRECIS, // nOutPrecision 
		CLIP_DEFAULT_PRECIS, // nClipPrecision 
		DEFAULT_QUALITY, // nQuality 
		DEFAULT_PITCH | FF_SWISS, // nPitchAndFamily 
		_T("SYSTEM_FIXED_FONT")); // lpszFac		pDC->SelectStockObject(SYSTEM_FIXED_FONT);		
	CFont *OldFont;
	OldFont = theDC->SelectObject(&titleFont);
	theDC->SetBkMode(TRANSPARENT);
	theDC->SetTextColor(RGB(50,109,105));
	theDC->TextOut((tr.right-tr.left)/2-7*12,(tr.bottom-tr.top)/3, "Select Picture Directory");
	
	titleFont.DeleteObject();
	titleFont.CreateFont( 15, // nHeight 
		6, // nWidth 
		0, // nEscapement文本行逆时针旋转角度 
		0, // nOrientation字体角度
		FW_THIN, // nWeight字体粗细程度 
		FALSE, // bItalic 
		FALSE, // bUnderline 
		0, // cStrikeOut 删除线
		ANSI_CHARSET, // nCharSet 
		OUT_DEFAULT_PRECIS, // nOutPrecision 
		CLIP_DEFAULT_PRECIS, // nClipPrecision 
		DEFAULT_QUALITY, // nQuality 
		DEFAULT_PITCH | FF_SWISS, // nPitchAndFamily 
		_T("SYSTEM_FIXED_FONT")); // lpszFac		pDC->SelectStockObject(SYSTEM_FIXED_FONT);		
    theDC->SelectObject(&titleFont);
	theDC->TextOut((tr.right-tr.left)/2-14*6,(tr.bottom-tr.top)/3+38, "↑ Lrc Forward 0.5s  ↓ Lrc Backward 0.5s");
	theDC->TextOut((tr.right-tr.left)/2-14*6,(tr.bottom-tr.top)/3+58, "→ Increase moving speed  ← Decrease moving speed");


	theDC->SelectObject(OldFont);
	titleFont.DeleteObject();	
	ReleaseDC(theDC);
	
}

LRESULT CDlgShow::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	

	return CDialog::WindowProc(message, wParam, lParam);
}
/*假设窗口宽度为rd， 则左边填充部分为 (rd-451)/2  , 右边为(rd-451)/2
  [0, (rd-451)/2]    , 右边为 [rd - (rd-451)/2 , rd]
  中间部分位置为 [(rd-451)/2 +1, 451+... ... ]
*/
void CDlgShow::DrawTitleBar(CDC *pDC)
{
	if(g_type==SHOW_TYPE_SCR) return;
 	FillRevClient(1);	
	
	if(!(GetStyle()&WS_DLGFRAME))
	{
		return;
	}
	if (m_hWnd)
	{
		CBrush brush(RGB(0,0,0));
		int xFramesWidth,yFrameWidth;
		CBrush Brush(RGB(0,100,255));
		CBrush* pOldBrush = pDC->SelectObject(&Brush);
		CRect rtWnd, rtTitle, rtButtons;
		GetWindowRect(&rtWnd); 
		//取得标题栏的位置
		xFramesWidth=rtTitle.left = GetSystemMetrics(SM_CXFRAME);
		yFrameWidth=rtTitle.top = GetSystemMetrics(SM_CYFRAME);
		rtTitle.right = rtWnd.right - rtWnd.left - GetSystemMetrics(SM_CXFRAME);
		rtTitle.bottom = rtTitle.top + GetSystemMetrics(SM_CYSIZE);
		//debug
		//计算最小化按钮的位置，位图大小为23*23
		rtButtons.left = rtTitle.right-70;
		rtButtons.top= rtTitle.top+2;
		rtButtons.right = rtTitle.right-47;
		rtButtons.bottom = rtTitle.top+25;
		m_rtButtMin = rtButtons;
		//计算最大化按钮的位置，位图大小为23*23
		m_rtButtMax.left = m_rtButtMin.left + 23;
		m_rtButtMax.top = m_rtButtMin.top;
		m_rtButtMax.right = m_rtButtMin.right + 23;
		m_rtButtMax.bottom = m_rtButtMin.bottom;

		m_rtButtExit.left = m_rtButtMax.left + 23;
		m_rtButtExit.top =  m_rtButtMax.top;
		m_rtButtExit.right = m_rtButtMax.right + 23;
		m_rtButtExit.bottom = m_rtButtMax.bottom;


		//准备画图
		CBitmap* pBitmap = new CBitmap;
		BITMAP BmpInfo;
		CBitmap* pOldBitmap;
		CDC* pDisplayMemDC=new CDC;
		pDisplayMemDC->CreateCompatibleDC(pDC);

        

		//重画最Caption
        int iwd =rtWnd.right -rtWnd.left;
		int imarign = (iwd - PIC_MIDDLE_WIDTH)/2;
        
         

		POINT  DrawPonit;
		DrawPonit.x =	0;
		DrawPonit.y =	0;
		pBitmap->LoadBitmap(IDB_LINE);
		pOldBitmap=(CBitmap*)pDisplayMemDC->SelectObject(pBitmap);
		pBitmap->GetBitmap(&BmpInfo);
		int i=0;
		for(i=0; i< imarign; i++)
		{
		  pDC->BitBlt(DrawPonit.x+i, DrawPonit.y, BmpInfo.bmWidth, BmpInfo.bmHeight, pDisplayMemDC, 0, 0, SRCCOPY);
		}
        DrawPonit.x =DrawPonit.x +i;
		pDisplayMemDC->SelectObject(pOldBitmap);
		pBitmap->DeleteObject();
        

		
		pBitmap->LoadBitmap(IDB_MIDDLE);
		pOldBitmap=(CBitmap*)pDisplayMemDC->SelectObject(pBitmap);
		pBitmap->GetBitmap(&BmpInfo);
		this->m_ButtonPosx = DrawPonit.x + BmpInfo.bmWidth/2 -20;
		
		pDC->BitBlt(DrawPonit.x, DrawPonit.y, BmpInfo.bmWidth, BmpInfo.bmHeight, pDisplayMemDC, 0, 0, SRCCOPY);
        DrawPonit.x =DrawPonit.x +BmpInfo.bmWidth;
		pDisplayMemDC->SelectObject(pOldBitmap);
		pBitmap->DeleteObject();
        
       
		pBitmap->LoadBitmap(IDB_LINE);
		pOldBitmap=(CBitmap*)pDisplayMemDC->SelectObject(pBitmap);
		pBitmap->GetBitmap(&BmpInfo);
		for(i=0; i<imarign+1; i++)
		{
		 pDC->BitBlt(DrawPonit.x+i, DrawPonit.y, BmpInfo.bmWidth, BmpInfo.bmHeight, pDisplayMemDC, 0, 0, SRCCOPY);
		}
		pDisplayMemDC->SelectObject(pOldBitmap);
		pBitmap->DeleteObject();
		
		//重画最小化button
		rtButtons = m_rtButtMin;
		pBitmap->LoadBitmap(IDB_BUTTON);
		pBitmap->GetBitmap(&BmpInfo);
		pOldBitmap=(CBitmap*)pDisplayMemDC->SelectObject(pBitmap);
		pDC->BitBlt(rtButtons.left, rtButtons.top, BmpInfo.bmWidth, BmpInfo.bmHeight, pDisplayMemDC, 0, 0, SRCCOPY);
		pDisplayMemDC->SelectObject(pOldBitmap);
		pBitmap->DeleteObject();


		//重画左边框
		int max_xFramesWidth=min(xFramesWidth,4);
		pBitmap->LoadBitmap(IDB_RIGHTDOWN);
		pBitmap->GetBitmap(&BmpInfo);
		pOldBitmap=(CBitmap*)pDisplayMemDC->SelectObject(pBitmap);
		for (i= m_slideTop+1; i<=rtWnd.bottom-rtWnd.top-BmpInfo.bmHeight;i=i+BmpInfo.bmHeight-3)
		{
			pDC->StretchBlt(0,i,max_xFramesWidth,BmpInfo.bmHeight, pDisplayMemDC, 0, 0,BmpInfo.bmWidth,BmpInfo.bmHeight, SRCCOPY);
		}
		//
		if(xFramesWidth>4)
		{
			pDC->FillRect(CRect(max_xFramesWidth,m_slideTop+1,xFramesWidth,rtWnd.bottom-rtWnd.top),&brush);	
		}
		pDisplayMemDC->SelectObject(pOldBitmap);
		pBitmap->DeleteObject();

		


		//重画右边框
		pBitmap->LoadBitmap(IDB_LEFTDOWN);
		pBitmap->GetBitmap(&BmpInfo);
		pOldBitmap=(CBitmap*)pDisplayMemDC->SelectObject(pBitmap);
		for (i= m_slideTop+1;i<=rtWnd.bottom-rtWnd.top;i=i+BmpInfo.bmHeight-3)
		{
			pDC->BitBlt(rtWnd.right-rtWnd.left-max_xFramesWidth,  /*rtButtons.top+*/i, BmpInfo.bmWidth, BmpInfo.bmHeight, pDisplayMemDC, 0, 0, SRCCOPY);
		}
		if(xFramesWidth>4)
		{
			pDC->FillRect(CRect(rtWnd.right-rtWnd.left-xFramesWidth,m_slideTop+1,rtWnd.right-rtWnd.left-max_xFramesWidth,rtWnd.bottom-rtWnd.top),&brush);	
		}
		pDisplayMemDC->SelectObject(pOldBitmap);
		pBitmap->DeleteObject();


		//重画底边框
		int max_yFrameWidth=min(4,yFrameWidth);
		pBitmap->LoadBitmap(IDB_DOWN1);
		pBitmap->GetBitmap(&BmpInfo);
		pOldBitmap=(CBitmap*)pDisplayMemDC->SelectObject(pBitmap);
		GetClientRect(&rtWnd);
		GetWindowRect(&rtWnd);
		for (i= 0; i<rtWnd.right-rtWnd.left; i=i+BmpInfo.bmWidth)
		{
			pDC->StretchBlt(i,rtWnd.bottom-rtWnd.top-max_yFrameWidth, BmpInfo.bmWidth, max_yFrameWidth,pDisplayMemDC,0,0,BmpInfo.bmWidth,BmpInfo.bmHeight,SRCCOPY);			
		}
	    if(yFrameWidth>4)
		{
			pDC->FillRect(CRect(max_xFramesWidth,rtWnd.bottom-rtWnd.top-yFrameWidth,rtWnd.right-rtWnd.left-max_xFramesWidth,rtWnd.bottom-rtWnd.top-max_yFrameWidth),&brush);				
		}
		pDisplayMemDC->SelectObject(pOldBitmap);
		pBitmap->DeleteObject();

		if(!IsIconic())
		{
			for ( i=0; i<(int)m_buttons.GetSize(); i++)
			{
				_button* pbtn = m_buttons[i];
				int i=ImageList_Draw(pbtn->himl, pbtn->uStatus,pDC->GetSafeHdc(), pbtn->x+m_ButtonPosx, pbtn->y+m_ButtonPosy, ILD_TRANSPARENT);		
			}
		}		
		brush.DeleteObject();
		ReleaseDC(pDisplayMemDC);
		delete pDisplayMemDC;
		delete pBitmap;    
        DrawSlider();
	}
}
LRESULT CDlgShow::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{

	LRESULT lrst=CDialog::DefWindowProc(message, wParam, lParam);

	if (!::IsWindow(m_hWnd))
		return lrst;
	if (/*message==WM_MOVE||message==WM_PAINT||message==WM_NCPAINT||*/message==WM_NCACTIVATE)
	{
		CDC* pWinDC = GetWindowDC();
		if (pWinDC)
		{
			DrawTitleBar(pWinDC);
			ReleaseDC(pWinDC);
		}

	}
	if(WM_MOVE==message || WM_SIZE==message) AdjustListPos();    
	return lrst;
}
//int ii=0;
void CDlgShow::OnNcLButtonDown(UINT nHitTest, CPoint point) 
{
		CRect rtWnd;
		GetWindowRect(&rtWnd);
		//mousse坐标转化为本窗口坐标 重要
		point.x = point.x - rtWnd.left;
		point.y = point.y - rtWnd.top;
		//检测各按钮是否按到
		if(1)
		{ 
			if (m_rtButtHelp.PtInRect(point))
			{
				SendMessage(WM_HELP);
			}
			else 
			{
				if (m_rtButtExit.PtInRect(point))
				{
//					g_pThread->Delete();
 					SendMessage(WM_CLOSE);
					
				}
				else 
				{
					if (m_rtButtMin.PtInRect(point))
					{
						m_ShowTitle = FALSE;
						if(g_pThread)
						{
// 							g_pThread->SuspendThread();
						}
//						SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, MAKELPARAM(point.x, point.y));
						HideInTray();
						ShowWindow(SW_HIDE);
						::AfxGetMainWnd()->ShowWindow(SW_HIDE);
					}
					else
					{
						if (m_rtButtMax.PtInRect(point))
						{
/*							int flag=0;
							if(IsIconic()) flag=1;
*/
							m_ShowTitle = TRUE;
							if (IsZoomed())
							{
								this->ShowWindow(SW_HIDE);
								this->ModifyStyle(WS_BORDER,WS_OVERLAPPED|WS_CAPTION,0);												
								SendMessage(WM_SYSCOMMAND, SC_RESTORE, MAKELPARAM(point.x, point.y));
							}
							else
							{
								this->ShowWindow(SW_HIDE);
								this->ModifyStyle(WS_OVERLAPPED|WS_CAPTION,WS_BORDER,0);	
//								ShowWindow(SW_MAXIMIZE);													
								SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE, MAKELPARAM(point.x, point.y));
								Invalidate();
							}
/*
							if(g_pThread&&flag)
							{
                              g_pThread->ResumeThread();
							}
*/
						}
						else
						{
                               if(m_player.IsPlaying()) 								   
							   {
							      LARGE_INTEGER li;
                                  if( point.y > m_slideTop-4 && point.y <= m_slideTop+4)
								  {
								  m_slideRatio = point.x*1.0/ (rtWnd.right -rtWnd.left);
								  li.QuadPart =m_slideRatio*m_player.getSize().QuadPart;
								  m_player.SetPosition(li);
								  DrawSlider();								  
								  }

							   }
						}
					}
				}
			}
		}		
//button show
		for(int i=0; i<m_buttons.GetSize(); i++)
		{
			RECT rt;
			rt.left =m_buttons[i]->x +m_ButtonPosx;
			rt.top = m_buttons[i]->y +m_ButtonPosy;
			rt.right =rt.left + m_buttons[i]->cx;
			rt.bottom =rt.top + m_buttons[i]->cy;
			if(PtInRect(&rt,point))
			{
				m_Pressed =i;
				UpdateButtonStatus();
				break;
			}
		} 
        if(!m_flagDestroy) CDialog::OnNcLButtonDown(nHitTest,point);	
}

extern int g_type;
void CDlgShow::OnRButtonDown(UINT nFlags, CPoint point) 
{
	if(g_type!=SHOW_TYPE_PRO)
	{
	    CDialog::OnRButtonDown(nFlags, point);
		return;
	}
	POINT pt;
	GetCursorPos(&pt);		
	CMenu menu;
	menu.CreatePopupMenu();
	menu.AppendMenu(MF_STRING,WM_MYMESSAGE_SETPATH,"Play Pictures"); 	
	menu.AppendMenu(MF_STRING,WM_MYMESSAGE_SELECTMUSIC,"Play Music"); 		 		
	menu.AppendMenu(MF_STRING,WM_MYMESSAGE_ASS,"Lrc Association"); 		 	

//     menu.AppendMenu(MF_SEPARATOR);			 		 
	if(strMusic.IsEmpty())	
	{
		menu.EnableMenuItem(WM_MYMESSAGE_ASS,1);
	}
	menu.AppendMenu(MF_SEPARATOR);			 		 
	menu.AppendMenu(MF_STRING,WM_MYMESSAGE_OPEN,"Load Playing List");/*打开播放列表*/ 
	menu.AppendMenu(MF_STRING,WM_MYMESSAGE_SAVE,"Save Playing List"); 		 
	
	menu.AppendMenu(MF_SEPARATOR);			 		 
	menu.AppendMenu(MF_STRING,WM_MYMESSAGE_EXE,"Output Runable Album"); 
	menu.AppendMenu(MF_STRING,WM_MYMESSAGE_SCR,"Output Screen Saver "); 		 
	
    menu.AppendMenu(MF_SEPARATOR);				
    menu.AppendMenu(MF_STRING,WM_MYMESSAGE_LINK_FILE,"File Association"); 
    menu.AppendMenu(MF_STRING,WM_MYMESSAGE_UNLINK_FILE,"Remove File Association"); 
	
    menu.AppendMenu(MF_SEPARATOR);				
    menu.AppendMenu(MF_STRING,WM_MYMESSAGE_SETTING,"Picture Playing Config"); 
	menu.AppendMenu(MF_STRING,WM_MYMESSAGE_SETLRCPATH,"Lrc Searching Folder"); 		 			

	menu.AppendMenu(MF_SEPARATOR);				
    menu.AppendMenu(MF_STRING,WM_MYMESSAGE_FORWARD,"Lrc advance 0.25s"); 
	menu.AppendMenu(MF_STRING,WM_MYMESSAGE_BACK,"Lrc delay 0.25s"); 	
	if(strLrc.IsEmpty())
	{
		menu.EnableMenuItem(WM_MYMESSAGE_FORWARD,1);	
		menu.EnableMenuItem(WM_MYMESSAGE_BACK,1);	
	}
	menu.AppendMenu(MF_SEPARATOR);			 		 
	menu.AppendMenu(MF_STRING,WM_MYMESSAGE_SETTOP,"Always On Top"); 	
	if(m_bTop)
	{
	  menu.CheckMenuItem(WM_MYMESSAGE_SETTOP,MF_CHECKED);	
	}
	else
	{
	  menu.CheckMenuItem(WM_MYMESSAGE_SETTOP,MF_UNCHECKED);	
	}


	menu.AppendMenu(MF_STRING,WM_MYMESSAGE_EXIT,"Quit"); 
	
	menu.TrackPopupMenu(TPM_LEFTALIGN,pt.x,pt.y,this);
	HMENU hmenu=menu.Detach();
	menu.DestroyMenu();

	
	CDialog::OnRButtonDown(nFlags, point);
}

void CDlgShow::OnSetting()
{
	CConfigDlg dlg;
	dlg.m_radioSpeed =g_head.speed;
	dlg.m_radioAutosize =g_head.autosize;
	dlg.m_delayTime =g_head.delaytime;
	dlg.m_direct =g_head.direct;
//	dlg.m_sound =g_head.sound;
	if(dlg.DoModal()!=IDOK) return ;
	g_head.delaytime =dlg.m_delayTime;
	if(dlg.m_radioSpeed!=-1)
	  g_head.speed =dlg.m_radioSpeed;
	if(dlg.m_radioAutosize!=-1)
	  g_head.autosize =dlg.m_radioAutosize;
	g_head.direct =dlg.m_direct;
//	g_head.sound =dlg.m_sound;
}

bool AppendFile(char* strSrc,char* strDest,UINT type,unsigned char key[],int iEncryFlag)
{
	CFile fileSrc,fileDest;
	UINT len;
	unsigned char* pData=0;
	if(!fileSrc.Open(strSrc,CFile::modeRead|CFile::shareDenyNone)) 
	{
		::AfxMessageBox("read file error");
		return false;
	}
    if(!fileDest.Open(strDest,CFile::modeReadWrite))
	{
		fileSrc.Close();
		return false;
	}
    fileDest.SeekToEnd();
	len =fileSrc.GetLength();
	pData =new unsigned char[len];
	if(pData)
	{
		fileSrc.Read(pData,len);
		fileDest.Write((void*)&type,sizeof(UINT));
		//在这里进行DES加密,只针对图像文件,前64字节
		if(iEncryFlag && (BMPTYPE==type || JPGTYPE==type))
		{
		  DES destool;
		  int DstLen=64;
		  unsigned char *pDataEncry=new unsigned char[DstLen];
		  destool.CDesEnter(pData,pDataEncry,DstLen,key,0);
          memcpy(pData,pDataEncry,64);
		  delete[] pDataEncry;
		}
		fileDest.Write((void*)&len,sizeof(UINT));
		fileDest.Write(pData,len);
		delete[] pData;
	}
	fileSrc.Close();
	fileDest.Close();
	return true;	
}
void ExtractFile(char* strfile,char* strpath)
{
//建立临时文件，并将lrc写到临时文件中，最后再从临时文件中提取
	int i=0;
    unsigned char strpass[8];
	int magic=0;
	UINT type=0,len=0;
	bool bFind =false;
	CFile file;
	if(!file.Open(strfile,CFile::modeRead)) return;
	//从500k处开始读，假设编译出来的可执行文件大于500K
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
		char  fn[100];					
		CFile musicfile;
		CString strlastmusic;
		int lrc_flag=0;
		unsigned char* pData=0;
        if(file.Read(&g_head,sizeof(T_FILE_HEADER))!=sizeof(T_FILE_HEADER))
		{
			file.Close();
			return;
		}
		
		//这里如果是exe,且需要保护,则需要输入密码
		if(g_head.iEncyFlag)
		{
			CDlgCertificate dlg;
			CString strPwd;
			CString strprn;
			dlg.DoModal();
			dlg.getPwd(strPwd);
			memset(strpass,0,8);
			memcpy(strpass,strPwd.GetBuffer(0),min(strPwd.GetLength(),8));
		    unsigned char strkey[8]={'s','s','s','h','o','w',0,0};
		    unsigned char strresult[8];
		    DES destool;
	  	    destool.CDesEnter(g_head.pwd,strresult,8,strpass,1);
	  	    for(int i=0; i<8; i++)
		    {
               if(strresult[i]!=strkey[i])
			   {
				::AfxMessageBox("密码不正确");
				return;
			   }
		    }
		 }
		sprintf(fn,"%s\\music.txt",strpath);		
		musicfile.Open(fn,CFile::modeCreate|CFile::modeReadWrite);
		while(1)
		{
			if(file.Read((void*)&type,sizeof(UINT))!=sizeof(UINT)) break;
			if(file.Read((void*)&len,sizeof(UINT))!=sizeof(UINT))  break;
			if(type!=BMPTYPE && type!=JPGTYPE && type!=MP3TYPE && type!=WAVTYPE&& type!=WMATYPE  && type!=LRCTYPE) break;
			pData =new unsigned char[len];
		 	if(file.Read(pData,len)!=len)
			{
				delete pData;
				break;
			}
			//add your code here
			CFile ff;
			switch(type) {
			case BMPTYPE:
				sprintf(fn,"%s\\tmp%d.bmp",strpath,i);
				break;
			case JPGTYPE:
				sprintf(fn,"%s\\tmp%d.jpg",strpath,i);
				break;
			case MP3TYPE:
				sprintf(fn,"%s\\tmp%d.mp3",strpath,i);
				musicfile.Write(fn,strlen(fn));
				musicfile.Write(";",1);				
				break;
			case WAVTYPE:
				sprintf(fn,"%s\\tmp%d.wav",strpath,i);
				musicfile.Write(fn,strlen(fn));	
				musicfile.Write(";",1);								
				break;
			case WMATYPE:
				//sprintf(fn,"%s\\tmp%d.wma",strpath,i);
				//musicfile.Write(fn,strlen(fn));		
				//musicfile.Write(";",1);								
				break;
			case LRCTYPE:
				i--;//歌词文件保持与music文件一致
				sprintf(fn,"%s\\tmp%d.lrc",strpath,i);
				break;
			default: ;
			}
			if(ff.Open(fn,CFile::modeCreate|CFile::modeWrite))
			{
				//这里的pData是加密过的数据,应该先解密
				if(g_head.iEncyFlag &&(type==BMPTYPE || type==JPGTYPE))
				{
			    	int dstlen=64;
				    DES destool;
				    unsigned char *pDataEncry=new unsigned char[dstlen];
                    destool.CDesEnter(pData,pDataEncry,dstlen,strpass,1);
					memcpy(pData,pDataEncry,64);
					delete[] pDataEncry;
				}
                ff.Write(pData,len);
				ff.Close();
			}
			delete pData;	
			i++;

		}
		musicfile.Close();
	}
	file.Close();
	SendMessage(AfxGetMainWnd()->GetSafeHwnd(),WM_MYMESSAGE_STARTPLAY,0,0);
}


void CDlgShow::OnMakeExe()
{
   OnMakeFile(SHOW_TYPE_EXE);
}
void CDlgShow::OnMakeScr()
{
   OnMakeFile(SHOW_TYPE_SCR);		
}
void CDlgShow::CountFile()
{
	//遍历当前目录下的所有文件，附着到exe文件后
	CFileFind ff;
	CArray<CString,CString> dirArray;
	m_count =0;
	dirArray.Add(path);
	while(dirArray.GetSize())
	{
		CString strCurrentDir;
		strCurrentDir =dirArray.GetAt(0);
		dirArray.RemoveAt(0);
		if(strCurrentDir.Right(1)!="\\") strCurrentDir+="\\*.*";
		BOOL res = ff.FindFile(strCurrentDir);
		while( res )
		{
			res = ff.FindNextFile();
			if(ff.IsDots()) continue;
			if(ff.IsDirectory())
			{	
				if(g_includeSubDir) dirArray.Add(ff.GetFilePath());
			}
			else
			{   
				CString filename =ff.GetFileName();
				filename.MakeUpper();
				if(filename.Right(4) ==".JPG"  || 
					filename.Right(5) ==".JPEG" ||
					filename.Right(4) ==".BMP")
				{
					m_count++;
				}
			}
			
		}
		ff.Close();
	}//while
}



void CDlgShow::OnOpenSlide()
{
	CFileDialog dlg(TRUE,
		NULL, 
		NULL, 
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT |OFN_ENABLESIZING ,
		_T("Task Files (*.sld)|*.sld|All Files (*.*)|*.*||"),
		NULL);
	if(dlg.DoModal()==IDOK)
	{
        LoadSldFile(dlg.m_ofn.lpstrFile);
		SlideShow();
	}		
}

bool CDlgShow::LoadSldFile(char *lpstrFile)
{
	CFile file;
	if(!file.Open(lpstrFile,CFile::modeRead))
	{
	   return false;
	}
	file.Close();
	CIniFile sldFile;
	sldFile.InitFile(lpstrFile);
	g_head.autosize =sldFile.readInt("SETTING","autosize");
	g_head.delaytime =sldFile.readInt("SETTING","delay time");
	g_head.direct =sldFile.readInt("SETTING","direct");
	g_head.speed =sldFile.readInt("SETTING","speed");
    g_head.forward =0;//sldFile.readInt("SETTING","forward");

	if(g_head.autosize>3 ||g_head.autosize<0) g_head.autosize =3;
	if(g_head.delaytime>20 ||g_head.delaytime<0) g_head.delaytime =3;
	if(g_head.speed>2 ||g_head.speed<0) g_head.speed =2;
	if(g_head.direct>2 ||g_head.direct<0) g_head.direct =2;
	

	char tmppath[MAX_PATH]={0};
	sldFile.readText("SETTING","path",tmppath,MAX_PATH);
	strcpy(path,tmppath);
	tmppath[0]=0;
//	strMusic =sldFile.readText("SETTING","music",tmppath,MAX_PATH);
//	strMusic =tmppath;


	tmppath[0]=0;
//	sldFile.readText("SETTING","LRC",tmppath,MAX_PATH);
//	strLrc =tmppath;

    if(m_pListWindow)
	{
		m_pListWindow->ResetList();
		int listcnt = sldFile.readInt("SETTING","LIST_CNT");
		if(listcnt<100 && listcnt>0)
		{
			CString strr;
		    for(int i=0; i< listcnt; i++)
			{
				strr.Format("list%d",i);
                tmppath[0] =0;
				sldFile.readText("SETTING",strr.GetBuffer(0),tmppath,MAX_PATH);
				if(strlen(tmppath)>0)
				{
					m_pListWindow->InsertString(tmppath);
				}
			}
		}
	}
//	m_curLycris.Empty();
	
	return true;
}

void CDlgShow::OnSaveSlide()
{
	OPENFILENAME   ofn;               //   common   dialog   box   structure   
	char   szFile[260];               //   buffer   for   file   name   
	ZeroMemory(&ofn,   sizeof(ofn));   
	ofn.lStructSize   =   sizeof(ofn);   
	ofn.hwndOwner   =   this->GetSafeHwnd();   
	ofn.lpstrFile   =   szFile;   
	ofn.lpstrFile[0]   =   '\0';   
	ofn.nMaxFile   =   sizeof(szFile);   
	ofn.lpstrFilter   =   "SLIDE FILE(*.sld)\0*.sld\0";   
	ofn.nFilterIndex   =   1;   
	ofn.lpstrFileTitle   =   NULL;   
	ofn.nMaxFileTitle   =   0;   
	ofn.lpstrInitialDir   =   NULL;   
	ofn.Flags   =   OFN_PATHMUSTEXIST   |   OFN_FILEMUSTEXIST;   
	//   Display   the   Save   dialog   box.         
	GetSaveFileName(&ofn);     
	//   ofn.lpstrFile   Store   the   Filename.   
	if(strlen(ofn.lpstrFile))
	{
		CString strSave;
		strSave =ofn.lpstrFile;
		if(strSave.Right(4)!=".sld")					  
			strSave +=".sld";	
		SaveSldFile(strSave.GetBuffer(0));
	}
	
}
#include "efile.h"

void CDlgShow::SlideShow()
{
	int trytime=0;
	static int sm_first=0;
	m_pause =0;			
	DWORD exitCode =STILL_ACTIVE;	
//	g_threadExitFlag =1;
	//if(g_semload)
	//{
	//	ReleaseSemaphore(g_semload,1,NULL);	
	//}
	//if(g_semshow)
	//{
	//	ReleaseSemaphore(g_semshow,1,NULL);
	//}
	while(exitCode==STILL_ACTIVE)
	{
	    g_threadExitFlag =1;
		if(g_pThread==NULL) break;
	    if(g_semload)
	    {
		  ReleaseSemaphore(g_semload,1,NULL);	
	    }
    	if(g_semshow)
	    {
	    	ReleaseSemaphore(g_semshow,1,NULL);
	    }
		if(!GetExitCodeThread(g_pThread->m_hThread,&exitCode)) 
		{
			g_pThread =NULL;
			break;
		}
		trytime++;
		if(trytime>10000) 
		{
//			::AfxMessageBox("lala");
			break;
		}
	}	

	trytime=0;
	while(exitCode==STILL_ACTIVE)
	{
		g_threadExitFlag =1;
		if(g_pThread2==NULL) break;
    	if(g_semshow)
	    {
	    	ReleaseSemaphore(g_semshow,1,NULL);
	    }
	    if(g_semload)
	    {
		  ReleaseSemaphore(g_semload,1,NULL);	
	    }
		if(!GetExitCodeThread(g_pThread2->m_hThread,&exitCode)) 
		{
			g_pThread2 =NULL;
			break;
		}
		trytime++;
		if(trytime>10000) 
		{
//			::AfxMessageBox("haha");
			break;
		}
	}	
	Invalidate(TRUE);
 	g_threadExitFlag=0;
	//这里应该首先删除信号量,防止内存泄露
	g_semload = CreateSemaphore(NULL, 1, 1, NULL);  //初始计数为M
	g_semshow = CreateSemaphore(NULL, 0, 1, NULL);  //初始计数为0
	g_pThread =AfxBeginThread((AFX_THREADPROC)ShowThread,(void*)this,THREAD_PRIORITY_NORMAL);
	g_pThread2 =AfxBeginThread((AFX_THREADPROC)ShowThread2,(void*)this,THREAD_PRIORITY_ABOVE_NORMAL);

}

int   CALLBACK   BrowseCallbackProc(HWND   hwnd,   UINT   msg,   LPARAM   lp,   LPARAM   pData)   
{   
	if   (msg   ==   BFFM_INITIALIZED   )   
	{   
		::SendMessage(hwnd,BFFM_SETSELECTION,TRUE,(LPARAM)pData);   
	}   
	return   0;   
}   
void CDlgShow::OnSetPath()
{

	char tmppath[256];
	BROWSEINFO bi;
	ITEMIDLIST * pidl;
	bi.hwndOwner =GetSafeHwnd();
	bi.iImage =0;
	bi.lpfn =BrowseCallbackProc;	
	bi.lParam =(LPARAM)path;
	bi.lpszTitle ="Select Picture Folder";
	bi.pidlRoot =0;
	bi.pszDisplayName =0;
	bi.ulFlags =BIF_RETURNONLYFSDIRS;   
	UpdateData(TRUE);
	pidl=SHBrowseForFolder(&bi);
	if(SHGetPathFromIDList(pidl,tmppath)==TRUE)	
	{		
        strcpy(path,tmppath);
        SlideShow();		
	}	
}

void CDlgShow::OnDropFiles(HDROP hDropInfo) 
{
	// TODO: Add your message handler code here and/or call default
	if(g_type!=SHOW_TYPE_PRO) return;
	char filename[MAX_PATH];
	DragQueryFile(hDropInfo,0,filename,MAX_PATH);
    if(GetFileAttributes(filename)&FILE_ATTRIBUTE_DIRECTORY)
	{
		strcpy(path,filename);
		SlideShow();		
	}
	else
	{
		int forward=0;
		CString strTmp;
		strTmp =filename;
		strTmp.MakeUpper();
		strTmp =strTmp.Right(4);
		if( strTmp==".MP3" ||
			//strTmp==".WMA" ||
			strTmp==".WAV" 			
			)
		{
			CString retLrc;
			strLrc.Empty();
			strMusic =filename;
			if(m_lrcBuf.Find(strMusic,retLrc,forward))
			{
				strLrc =retLrc;
				g_head.forward =forward;
			}	
			g_head.forward =0;
			if(m_pListWindow)
			{
				m_pListWindow->ResetList();
				m_pListWindow->InsertString(strMusic);	
			}
			PlayMusic();
		}
		else if(strTmp==".SLD")
		{
			LoadSldFile(filename);
			PlayMusic();		 
			SlideShow();
		}
		else
		{
			::AfxMessageBox("不支持的文件类型");
			return;
		}
		
	}
	
	CDialog::OnDropFiles(hDropInfo);
}

void CDlgShow::OnOK()
{
  m_pause =1-m_pause;
}

void CDlgShow::SaveSldFile(char *lpstrFile)
{
	CIniFile  sldfile;
	sldfile.InitFile(lpstrFile);
	sldfile.write("SETTING","path",path);	  
	sldfile.writeInt("SETTING","autosize",g_head.autosize);
	sldfile.writeInt("SETTING","delay time",g_head.delaytime);
	sldfile.writeInt("SETTING","direct",g_head.direct);
	sldfile.writeInt("SETTING","speed",g_head.speed);   
	if(m_pListWindow)
	{
		int listcnt =m_pListWindow->GetCount();
		sldfile.writeInt("SETTING","LIST_CNT",listcnt);
		for(int i=0; i< listcnt; i++)
		{
			CString strindex;
			strindex.Format("list%d",i);
			sldfile.write("SETTING",strindex.GetBuffer(0),m_pListWindow->GetString(i).GetBuffer(0));
		}
	}
}

void CDlgShow::OnMakeFile(int type)
{
    int i=0;
	CString strpwd;
	unsigned char strpass[8];
	CString strTitle;
	OPENFILENAME   ofn;               //   common   dialog   box   structure   
	char   szFile[MAX_PATH];               //   buffer   for   file   name   
	CountFile();
	ZeroMemory(&ofn,   sizeof(ofn));   
	ofn.lStructSize   =   sizeof(ofn);   
	ofn.hwndOwner   =   this->GetSafeHwnd();   
	ofn.lpstrFile   =   szFile;   
	ofn.lpstrFile[0]   =   '\0';   
	ofn.nMaxFile   =   sizeof(szFile);   
	if(type==SHOW_TYPE_EXE)
	  ofn.lpstrFilter   =   "Executable File(*.exe)\0*.exe\0";   
	else
  	  ofn.lpstrFilter   =   "Screen Saver(*.scr)\0*.scr\0";   	
	ofn.nFilterIndex   =   1;   
	ofn.lpstrFileTitle   =   NULL;   
	ofn.nMaxFileTitle   =   0;   
	ofn.lpstrInitialDir   =   NULL;   
	ofn.Flags   =   OFN_PATHMUSTEXIST   |   OFN_FILEMUSTEXIST;   
	//   Display   the   Save   dialog   box.         
	GetSaveFileName(&ofn);     
	//   ofn.lpstrFile   Store   the   Filename.   
	if(strlen(ofn.lpstrFile))
	{
		CString strSave;
		strSave =ofn.lpstrFile;	
		//释放资源文件
		if(type==SHOW_TYPE_EXE)
		{
			if(strSave.Right(4)!=".exe")
				strSave +=".exe";
		}
		else
		{
			if(strSave.Right(4)!=".scr")
				strSave +=".scr";
		}
		//生成新文件,第一部分为ssshow.exe
		CFile file;
		char* pSrcBuf=0;
		int len=0;
		if(!file.Open(__argv[0],CFile::modeRead)) return;
		len =file.GetLength();
		pSrcBuf =new char[len];
		file.Read(pSrcBuf,len);
		file.Close();
        
		if(!file.Open(strSave.GetBuffer(0),CFile::modeCreate|CFile::modeReadWrite))
		{
			delete[] pSrcBuf;
			return;
		}
		file.Write(pSrcBuf,len);
        delete[] pSrcBuf;
		//补齐四字节
		int  num=0;
		UINT  tmp=0;
		num =4-len%4;
		for( i=0; i<num; i++)
		{
			file.Write((void*)&tmp,1);
		}
		//写入标识符,magic num,后面仅跟着一个文件头标识符
		tmp =MAGIC_NUM;
		file.Write(&tmp,sizeof(UINT));
		if(type==SHOW_TYPE_EXE)		
		{
			g_head.type =SHOW_TYPE_EXE;
		}
		else 		
		{
			g_head.type =SHOW_TYPE_SCR;
		}
		g_head.iEncyFlag=0; //默认不加密
		//只对exe文件进行加密
		if(type==SHOW_TYPE_EXE)
		{//加一个对话框,提示输入pass，或者不使用pass
			CDlgInputPwd dlg;
			dlg.DoModal();
			dlg.GetPwd(strpwd);
			if(!strpwd.IsEmpty())
			{

			   DES destool;
			   unsigned char strms[8]={'s','s','s','h','o','w',0,0};
			   memset(strpass,0,8);
			   memcpy(strpass,strpwd.GetBuffer(0),min(strpwd.GetLength(),8));
			   destool.CDesEnter(strms,g_head.pwd,8,strpass,0);
			   g_head.iEncyFlag=1;
			}
		}
		
		file.Write(&g_head,sizeof(T_FILE_HEADER));
		file.Close();
		
		//显示进度条
		CProBar* proDlg =new CProBar;
		proDlg->Create(IDD_DIALOG2);
        proDlg->ShowWindow(SW_SHOW);
		proDlg->m_probar.SetStep(1);
        proDlg->m_probar.SetPos(0);
	//追加mp3.wma歌曲
		CString strsong;
		int cnt =m_pListWindow->GetCount();
		for(i=0; i<cnt; i++)
		{
			CString lrcpath;
			int forward;
			strsong =m_pListWindow->GetString(i);
			strsong.MakeUpper();
            if(strsong.Right(4)==".MP3")
				AppendFile(strsong.GetBuffer(0),strSave.GetBuffer(0),MP3TYPE,NULL,0);
			
           // if(strsong.Right(4)==".WMA")
		//		AppendFile(strsong.GetBuffer(0),strSave.GetBuffer(0),WMATYPE,NULL,0);
			
            if(strsong.Right(4)==".WAV")
				AppendFile(strsong.GetBuffer(0),strSave.GetBuffer(0),WAVTYPE,NULL,0);
			
			if(m_lrcBuf.Find(strsong,lrcpath,forward))
			{
				AppendFile(lrcpath.GetBuffer(0),strSave.GetBuffer(0),LRCTYPE,NULL,0);			
			}	
		}
		//遍历当前目录下的所有文件，附着到exe文件后		
        CFileFind ff;
		CArray<CString,CString> dirArray;
		dirArray.Add(path);
		while(dirArray.GetSize())
		{
			CString strCurrentDir;
			strCurrentDir =dirArray.GetAt(0);
			dirArray.RemoveAt(0);
			if(strCurrentDir.Right(1)!="\\") strCurrentDir+="\\*.*";
			BOOL res = ff.FindFile(strCurrentDir);
			while( res )
			{
				res = ff.FindNextFile();
				if(ff.IsDots()) continue;
				if(ff.IsDirectory())
				{	
					if(g_includeSubDir) dirArray.Add(ff.GetFilePath());
				}
				else
				{   
					CString filename =ff.GetFileName();
					filename.MakeUpper();
					if(filename.Right(4) ==".JPG"  || 
						filename.Right(5) ==".JPEG" ||
						filename.Right(4) ==".BMP")
					{
						i++;
						if(m_count)
						{
							proDlg->m_probar.SetPos(i*100/m_count);
						}						
						strTitle=ff.GetFilePath();
						proDlg->SetWindowText(strTitle.GetBuffer(0));
						if(filename.Right(4)==".BMP") 
							AppendFile(ff.GetFilePath().GetBuffer(0),strSave.GetBuffer(0),BMPTYPE,strpass, g_head.iEncyFlag);
						if(filename.Right(4)==".JPG") 
							AppendFile(ff.GetFilePath().GetBuffer(0),strSave.GetBuffer(0),JPGTYPE,strpass, g_head.iEncyFlag);
						if(filename.Right(5)==".JPEG") 
							AppendFile(ff.GetFilePath().GetBuffer(0),strSave.GetBuffer(0),JPGTYPE,strpass, g_head.iEncyFlag);
						
					}
				}				
			}
			ff.Close();
		}//while

		delete proDlg;		
	}
}
static int gcnt=0;
void CDlgShow::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	CDialog::OnMouseMove(nFlags, point);
	gcnt++;
	if(g_type==SHOW_TYPE_SCR && gcnt>16) 
	{ 		
		OnClose();
	}

}

void CDlgShow::DeleteTmpFile()
{
	//删除临时目录下的所有文件
	CFileFind ff;
	CString szDir =path;	
	if(szDir.Right(1) != "\\")
		szDir += "\\";	
	szDir += "*.*";
	BOOL res = ff.FindFile(szDir);
	while( res )
	{
		res = ff.FindNextFile();
		if(ff.IsDots()) continue;
		if(ff.IsDirectory())
		{	
			/*if(m_bIncSubDir) Browse( ff.GetFilePath());*/
		}
		else
		{   
			DeleteFile(ff.GetFilePath().GetBuffer(0));			
		}		
	}
	ff.Close();
	//删除临时目录
	RemoveDirectory(path);
	
}

void CDlgShow::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
    m_time++;	
	if(m_abc!=-1) 
	{
       m_abc++;
	   if(m_abc>100)
	   {
		   m_abc =-1;
           m_curLycris="";
	   }
	}
	if(m_forwardflag>=0)
	{
		m_forwardflag++;
	    if(m_forwardflag>40)
		{
			m_forwardflag=-1;
		}
	}
	CDialog::OnTimer(nIDEvent);
	if(IsIconic()) return;
	/*******************************/
	//判读当前是否在播放music，如果是，则提取当前时间，本根据时间取出歌词

	if(m_playFlag==1 && m_time%50==0)
	{
		if(strLrc.IsEmpty())
		{
			//m_curLycris ="";
		}
		else if(m_player.IsPlaying())
		{
			
			UINT itime = m_player.getTime()+g_head.forward;			
            CString strr=m_lrc.GetTxt(itime);			
 			if(!strr.IsEmpty()) m_curLycris =strr;
			m_nextLycris=m_lrc.GetNext(itime);

		}
	}

    
	if(m_playFlag==1 && m_time%20==0)
	{
		m_slideRatio =m_player.getTime()*1.0/m_player.GetTotalTime();
		DrawSlider();
		if(g_client_bottom) 
		{
			ShowLrc2();
		}
	}  
}

void CDlgShow::MySleep(UINT interval)
{
  UINT time1 =m_time;
  UINT time2 =0;
  if(interval==0) return;
  while (1)
  {
	  time2 =m_time;
	  if(time2-time1>=interval || time2-time1<0) return; 
      if(g_threadExitFlag==1 || g_setChanged==true)	return;
	  Sleep(1);	  
  }
}

LRESULT CDlgShow::OnEndPlay(WPARAM wParam, LPARAM lParam)
{
//	if(m_player.getTime()>=m_player.GetTotalTime())
	{ //一首歌放完
		if(m_pListWindow)
		{
			m_pListWindow->GetNextPlay();
		}			
		OnNotifyPlay(0,0);  	  
	}
	return 0;
}

void CDlgShow::OnNcLButtonDblClk(UINT nHitTest, CPoint point) 
{

	CAboutDlg dlg;
	dlg.DoModal();
	CDialog::OnNcLButtonDblClk(nHitTest, point);
}

int CDlgShow::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;	
	tagRECT rct;
	this->GetWindowRect(&rct);
	return 0;
}

void CDlgShow::DrawText(CDC *pDC, CString strTxt, UINT posx, UINT posy,COLORREF color)
{	
	int nOldMode = pDC->SetBkMode(TRANSPARENT);
	COLORREF clOldText=pDC->GetTextColor();		
	CFont titleFont;	
	titleFont.CreateFont( FONT_HIGH, // nHeight 
		FONT_WIDTH, // nWidth 
		0, // nEscapement文本行逆时针旋转角度 
		0, // nOrientation字体角度
		FW_BOLD, // nWeight字体粗细程度 
		FALSE, // bItalic 
		FALSE, // bUnderline 
		0, // cStrikeOut 删除线
		GB2312_CHARSET, // nCharSet 
		OUT_DEFAULT_PRECIS, // nOutPrecision 
		CLIP_DEFAULT_PRECIS, // nClipPrecision 
		DEFAULT_QUALITY, // nQuality 
		DEFAULT_PITCH | FF_SWISS, // nPitchAndFamily 
		_T("仿宋")); // lpszFac		pDC->SelectStockObject(SYSTEM_FIXED_FONT);		
	CFont *OldFont;
	OldFont = pDC->SelectObject(&titleFont);
	pDC->SetTextColor(color);
	//		if(m_ShowTitle)
	{
		pDC->TextOut(posx,posy,strTxt);	
	}
	pDC->SetBkMode(nOldMode);
	pDC->SetTextColor(clOldText);
	pDC->SelectObject(OldFont);		
}

void CDlgShow::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp) 
{
	int top=0;
	if(!(GetStyle()&WS_DLGFRAME))
	{
       top=0;
	}
	else
	{
       top=g_client_top;
	}
 	lpncsp->rgrc[0].top +=top;
	lpncsp->rgrc[0].bottom -=g_client_bottom;	
	CDialog::OnNcCalcSize(bCalcValidRects, lpncsp);
}



void CDlgShow::FillRevClient(int flag)
{
	
		CDC* pWinDC = GetWindowDC();
		if (pWinDC)
		{
            tagRECT rtTitle,rtWnd,rcWnd;
			GetWindowRect(&rtWnd);
			GetClientRect(&rcWnd);
			ClientToScreen(&rcWnd);
			rtTitle.left = GetSystemMetrics(SM_CXFRAME);
			rtTitle.top = GetSystemMetrics(SM_CYFRAME);
			rtTitle.right = rtWnd.right - rtWnd.left - GetSystemMetrics(SM_CXFRAME);
			rtTitle.bottom = rtTitle.top + GetSystemMetrics(SM_CYSIZE);
            CBrush brush(RGB(0,0,40));
			if(flag==1)
			{
				if(GetStyle()&WS_DLGFRAME) //with board
				{
					if(rcWnd.top-rtWnd.top >=30)
						//pWinDC->FillRect(CRect(rtTitle.left,30,rtTitle.right,rtTitle.bottom+g_client_top+1),&brush);                
						pWinDC->FillRect(CRect(rtTitle.left,30,rtTitle.right,rcWnd.top-rtWnd.top),&brush);                
				}
				else //with no board
				{
					pWinDC->FillRect(CRect(rtTitle.left,0,rtTitle.right,g_client_top+GetSystemMetrics(SM_CYFRAME)),&brush);
				}					
			}

			pWinDC->FillRect(CRect(rtTitle.left,rtWnd.bottom-rtWnd.top-g_client_bottom-GetSystemMetrics(SM_CYFRAME),rtTitle.right,rtWnd.bottom-rtWnd.top-GetSystemMetrics(SM_CYFRAME)),&brush);			
			brush.DeleteObject();
			ReleaseDC(pWinDC);
		}	
}

void CDlgShow::LoadLrcArray()
{
	m_iPos =0;
	m_playFlag =0;
    char filename[256];
	_splitpath(strMusic,NULL,NULL,filename,0);
    m_curLycris ="";
	if(g_type==SHOW_TYPE_PRO) 
	{
		m_curLycris =filename;	
	}
	if(!strLrc.IsEmpty() )
	{
		m_lrc.Free();
        m_lrc.Open(strLrc);
	}
	else
	{
		m_abc=0;
	}
	m_playFlag =1;	
}

void CDlgShow::OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	CDialog::OnSysKeyDown(nChar, nRepCnt, nFlags);
}
int g_tmp_autosize =1;
BOOL CDlgShow::PreTranslateMessage(MSG* pMsg) 
{
    if(pMsg->message==WM_KEYDOWN)
	{ 
		switch(pMsg->wParam)
		{
		case VK_DOWN:
			if(!strLrc.IsEmpty())
			{
				g_head.forward -=25;
				m_forwardflag=0;
			}
			break;
		case VK_UP:
			if(!strLrc.IsEmpty())
			{
				g_head.forward +=25  ;
				m_forwardflag=0;				
			}
			break;
		case VK_LEFT:
			if(g_head.speed<10) g_head.speed++;			
			break;
		case VK_RIGHT:
			if(g_head.speed>0) g_head.speed--;		
			break;			
		case 0x2e: //DEL键
			if(!m_curPic.IsEmpty()) 
			{
				CString strr,strFile;
				strFile =m_curPic;
				strr.Format("要删除文件%s吗 ?",strFile);
				if(AfxMessageBox(strr,MB_YESNO)==IDYES)
				{
					DeleteFile(m_curPic.GetBuffer(0));
					g_setChanged =true;				
				}
			}
			break;
		case 0x20:
			OnOK();
			break;
		case 's':
		case 'S':
			{
//				if(::GetKeyState(VK_CONTROL)<0)
				{
					CConfigDlg dlg;
					dlg.m_radioSpeed =g_head.speed;
					dlg.m_radioAutosize =g_head.autosize;
					dlg.m_delayTime =g_head.delaytime;
					dlg.m_direct =g_head.direct;
					if(dlg.DoModal()==IDOK)
					{
						g_head.delaytime =dlg.m_delayTime;
						if(dlg.m_radioSpeed!=-1)
							g_head.speed =dlg.m_radioSpeed;
						if(dlg.m_radioAutosize!=-1)
							g_head.autosize =dlg.m_radioAutosize;
						g_head.direct =dlg.m_direct;
					}
					return true;					
				}				
			}
			break;
		case 'm':
		case 'M':
			{
				ModifyLrc();
			}
			break;
		case 0x22: //page down
			g_setChanged =true;		
			break;
        case 'f':
        case 'F':
//			if(::GetKeyState(VK_CONTROL)<0)
			{
				if(g_head.autosize!=3) 
				{
					g_tmp_autosize =g_head.autosize;
					g_head.autosize =3;
					::AfxMessageBox("进入拉伸模式");
				}
				else
				{
					g_head.autosize =g_tmp_autosize;
					::AfxMessageBox("退出拉伸模式");					
				}
			}
			break;
			
		}
	}
// 	m_tooltips.RelayEvent(pMsg);
	return CDialog::PreTranslateMessage(pMsg);
}


void CDlgShow::ReShow()
{  

	if(strLrc.IsEmpty()) 
	{
		//g_client_bottom =0;      
	}

	else
	{
		if(!(GetStyle()&WS_DLGFRAME))
        {
           //g_client_top =0;
		}
	}    
	tagRECT rct;
	this->GetWindowRect(&rct);
	this->SetWindowPos(0,0,0,rct.right-rct.left ,rct.bottom-rct.top,SWP_NOMOVE |SWP_FRAMECHANGED );
}

void CDlgShow::OnAssociateLrc()
{
   CDlgASS dlg;
   dlg.m_lrc =strLrc;
   if(dlg.DoModal()==IDOK)
   {
       if(strLrc!=dlg.m_lrc)
	   {
	      strLrc =dlg.m_lrc;
		  m_lrcBuf.Add(strMusic,strLrc,0);
		  LoadLrcArray();
	   }
   }
	
}
void CDlgShow::OnLinkFile()
{
  CString strFile=__argv[0];
  strFile.MakeUpper();
  strFile.Replace(".EXE",".INI");
  CIniFile strIniFile;
  strIniFile.InitFile(strFile.GetBuffer(0));
  CString strDescript,strShellOpen;
  if(0==ReadRegisterInfo(".sld",strDescript,strShellOpen))
  {
	  ::AfxMessageBox("ok");
	  return;
  }
  RegisterFileTheApp(".sld","sld_auto_file");
  ReadRegisterInfo(".mp3",strDescript,strShellOpen);
  //记录到配置文件中
  strIniFile.write("file link desc",".mp3",strDescript.GetBuffer(0));
  strIniFile.write("file link open",".mp3",strShellOpen.GetBuffer(0));
  RegisterFileTheApp(".mp3","ssshow_mp3_file");

 // ReadRegisterInfo(".wma",strDescript,strShellOpen);  
  //记录到配置文件中
  //strIniFile.write("file link desc",".wma",strDescript.GetBuffer(0));
 // strIniFile.write("file link open",".wma",strShellOpen.GetBuffer(0));
  //RegisterFileTheApp(".wma","ssshow_wma_file");
  ::AfxMessageBox("File .sld/.mp3/.wma Associated");
}
void CDlgShow::OnUnLinkFile()
{
  char ptmp[256];
  CString strFile=__argv[0];
  strFile.MakeUpper();
  strFile.Replace(".EXE",".INI");
  CIniFile strIniFile;
  strIniFile.InitFile(strFile.GetBuffer(0));
  CString strDescript,strShellOpen;
  
  if(0!=ReadRegisterInfo(".sld",strDescript,strShellOpen))	
  {	
   ::AfxMessageBox("File Association(.sld/.mp3/.wma) Removed"); 
	return;
  }

  RemoveRegister(".sld","sld_auto_file");
  //restore old set
  memset(ptmp,0,256);
  strIniFile.readText("file link desc",".mp3",ptmp,256);
  
  if(strlen(ptmp)!=0)
  {
     RegisterReg(".mp3",ptmp);	  
  }
  
  //memset(ptmp,0,256);
  //strIniFile.readText("file link desc",".wma",ptmp,256);
 
  //if(strlen(ptmp)!=0)
  //{
//	 RegisterReg(".wma",ptmp);  
 // }
  ::AfxMessageBox("解除sld/mp3文件关联"); 
}
#include "LrcSearchDlg.h"
void CDlgShow::OnSetLrcPath()
{
	CLrcSearchDlg dlg;
	dlg.m_path =m_lrcBuf.GetSearchPath();
	if(dlg.DoModal()==IDOK)
	{
		m_lrcBuf.SetSearchPath(dlg.m_path);
	}	

}
LRESULT CDlgShow::OnNotifyPlay(WPARAM wparam, LPARAM lparam)
{
	
	CString retLrc;
	int forward;
	strLrc.Empty();
	if(!strMusic.IsEmpty()&&g_head.forward)
	{
	   ModifyLrc();
	}
	strMusic =gstrSong;
	if(m_lrcBuf.Find(strMusic,retLrc,forward))
	{
		strLrc =retLrc;
	}
	g_head.forward=0;
 	PlayMusic();
	return 0;
}

void CDlgShow::AdjustListPos()
{
	if(m_pListWindow)
	{
		tagRECT rtlist;
		tagRECT rtWnd;
		GetWindowRect(&rtWnd);	
		if(m_pListWindow)
		{
		  m_pListWindow->GetWindowRect(&rtlist);
		  if(g_bStick)
		  {
			  m_pListWindow->MoveWindow(rtWnd.right,rtWnd.top,rtlist.right-rtlist.left,rtWnd.bottom-rtWnd.top);			  
		  }

		}
	}		
}

void CDlgShow::OnSysCommand(UINT nID, LPARAM lParam) 
{
	// TODO: Add your message handler code here and/or call default

	CDialog::OnSysCommand(nID, lParam);
	if(nID==SC_MINIMIZE)
	{
        if(m_bListShow && m_pListWindow)
		{
			m_pListWindow->ShowWindow(SW_HIDE);
		}
	}
	else if(nID ==SC_MAXIMIZE ||nID==0xf122 || nID ==0xf120)
	{

		if(m_bListShow && m_pListWindow)
		{
		  m_pListWindow->ShowWindow(SW_SHOW);
//		  m_pListWindow->SendMessage(WM_NCPAINT);
		}
	}

   if( (nID & 0xFFF0) == SC_MOVE ) PostMessage(WM_NCLBUTTONUP, nID, lParam); 

}
void CDlgShow::OnSetMusic()
{

	CString strTitle;
	OPENFILENAME   ofn;               //   common   dialog   box   structure   
	char   szFile[MAX_PATH];               //   buffer   for   file   name  
	ZeroMemory(&ofn,   sizeof(ofn));   
	ofn.lStructSize   =   sizeof(ofn);   
	ofn.hwndOwner   =   this->GetSafeHwnd();   
    strcpy(szFile,strMusic.GetBuffer(0));
	ofn.lpstrFile   =   szFile;   
	ofn.nMaxFile   =   sizeof(szFile);   
	ofn.lpstrFilter   = _TEXT("All supported files\0*.mp3;\0") \
					    _TEXT("MP3 files(*.mp3)\0*.mp3\0");
					    
	ofn.nFilterIndex   =   1;   
	ofn.lpstrFileTitle   =   NULL;   
	ofn.nMaxFileTitle   =   0;   
	ofn.lpstrInitialDir   =   NULL;  
	ofn.Flags   =   OFN_PATHMUSTEXIST   |   OFN_FILEMUSTEXIST;   
	if(GetOpenFileName(&ofn))
	{
		if(strlen(ofn.lpstrFile))
		{
			CString retLrc;		
			int forward;
			strMusic =ofn.lpstrFile;
			strMusic.MakeUpper();
			strLrc.Empty();
			g_head.forward =0;		
			if(m_lrcBuf.Find(strMusic,retLrc,forward))
			{
				strLrc =retLrc;
				g_head.forward=forward; 
			}
			if(m_pListWindow)
			{
				m_pListWindow->ResetList();
				m_pListWindow->InsertString(strMusic);	
			}
			PlayMusic();		
		}	
	}
}
static int srcflag=1;
void CDlgShow::PlayMusic()
{
    if(strMusic.IsEmpty()) return;
	CFileFind ff;
	if(!ff.FindFile(strMusic))
	{
		::AfxMessageBox("文件不存在");
		m_pListWindow->RemoveString(strMusic);
		return;
	}
	m_player.Stop();
	m_player.Open(this->GetSafeHwnd(),strMusic.GetBuffer(0));
	m_player.Start();
	LoadLrcArray();
	ChangeButtonState(0,CAPTION_BTN_CHECKED);
	ChangeButtonState(1,CAPTION_BTN_NORMAL);

    if(strLrc.IsEmpty())
	{
		  //if(srcflag==1) ReShow();
          srcflag=0;
	}
	else
	{
		  //if(srcflag==0) ReShow();
          srcflag =1;
	}


}

void CDlgShow::OnNcPaint() 
{
	// TODO: Add your message handler code here
	CDC* pWinDC = GetWindowDC();
	if (pWinDC)
	{
		DrawTitleBar(pWinDC);
		ReleaseDC(pWinDC);	
	}

	// Do not call CDialog::OnNcPaint() for painting messages
}



void CDlgShow::OnNcMouseMove(UINT nHitTest, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
    UpdateButtonStatus();	
	CDialog::OnMouseMove(nHitTest, point);
}

int CDlgShow::AddButton(UINT uID, int x,int y,int cx, int cy, HIMAGELIST himl,char* pHint)
{
   _button* pButton =new _button;
   pButton->x =x;
   pButton->y =y;
   pButton->cx =cx;
   pButton->cy =cy;
   pButton->himl =himl;
   strcpy(pButton->szHint,pHint);
   pButton->uID =uID;
   pButton->uStatus =CAPTION_BTN_DISABLED;
   m_buttons.Add(pButton);
   return m_buttons.GetSize();
}

void CDlgShow::UpdateButtonStatus()
{
	if(IsIconic()) return;
    CRect	rcWnd;
	CPoint	pt;
	::GetCursorPos(&pt);	
	GetWindowRect(&rcWnd);
	pt.Offset(-rcWnd.TopLeft());	
	bool	bRepaint = false;
	for (int i=0; i<(int)m_buttons.GetSize(); i++)
	{
		_button* pBtn = m_buttons[i];		
		if (pBtn->uStatus != CAPTION_BTN_DISABLED)
		{
			UINT	uNewStatus;
			CRect	rc ;
			rc.left =pBtn->x + m_ButtonPosx;
			rc.top =pBtn->y + m_ButtonPosy;
			rc.right =rc.left + pBtn->cx;
			rc.bottom =rc.top + pBtn->cy;
			uNewStatus =pBtn->uStatus;
			if (rc.PtInRect(pt))
			{
			    
				if(m_Pressed ==i && pBtn->uStatus!=CAPTION_BTN_CHECKED)
				{
					uNewStatus =CAPTION_BTN_PUSHED;					
				}
				else
				{
					switch(pBtn->uStatus) {
					case CAPTION_BTN_CHECKED:
						break;
					case CAPTION_BTN_PUSHED:
 						uNewStatus =CAPTION_BTN_HOVER;
						break;
					case CAPTION_BTN_NORMAL:
 						uNewStatus =CAPTION_BTN_HOVER;
						break;
					case CAPTION_BTN_HOVER:
						break;
					case CAPTION_BTN_DISABLED:
						break;
					default: ;
					}
				}
			}
			else
			{
				switch(pBtn->uStatus) {
				case CAPTION_BTN_CHECKED:
					break;
				case CAPTION_BTN_PUSHED:
					break;
				case CAPTION_BTN_NORMAL:
					break;
				case CAPTION_BTN_HOVER:
					uNewStatus =CAPTION_BTN_NORMAL;					
					break;
				case CAPTION_BTN_DISABLED:
					break;
				default: ;
				} //switch		
			}//else
			
			if (pBtn->uStatus != uNewStatus)//repaint only if status changed
			{
	            CDC* pWinDC = GetWindowDC();
				bRepaint = true;
				pBtn->uStatus = uNewStatus;
                if(pWinDC)
				{
				  ImageList_Draw(pBtn->himl, pBtn->uStatus,pWinDC->GetSafeHdc(), pBtn->x+m_ButtonPosx, pBtn->y+m_ButtonPosy, ILD_TRANSPARENT);		
				  ReleaseDC(pWinDC);
				}
			}
		}
	}
}

void CDlgShow::ChangeButtonState(int index,int state)
{
   if(index < m_buttons.GetSize())
   {
	   _button* pBtn =m_buttons[index];
	   pBtn->uStatus =state;
       if(!(GetStyle()&WS_DLGFRAME)) 
	   {
		   return;
	   } 
	   CDC* pWinDC = GetWindowDC();
	   if(pWinDC)
	   {
		   ImageList_Draw(pBtn->himl, pBtn->uStatus,pWinDC->GetSafeHdc(), pBtn->x+m_ButtonPosx, pBtn->y+m_ButtonPosy, ILD_TRANSPARENT);		
		   ReleaseDC(pWinDC);
	   }
   }
}

void CDlgShow::OnNcLButtonUp(UINT nHitTest, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	CRect rtWnd;
	GetWindowRect(&rtWnd);
	::GetCursorPos(&point);	
	//mouse坐标转化为本窗口坐标 重要
	point.x = point.x - rtWnd.left;
	point.y = point.y - rtWnd.top;
	for(int i=0; i<m_buttons.GetSize(); i++)
	{
		RECT rt;
		rt.left =m_buttons[i]->x +m_ButtonPosx;
		rt.top = m_buttons[i]->y +m_ButtonPosy;
		rt.right =rt.left + m_buttons[i]->cx;
		rt.bottom =rt.top + m_buttons[i]->cy;
		if(PtInRect(&rt,point))
		{
			m_Pressed =-1;
            SendMessage(WM_COMMAND, MAKELONG(m_buttons[i]->uID, 0), 0);
 // 		UpdateButtonStatus();       
			break;
		}
	}		
	CDialog::OnNcLButtonUp(nHitTest, point);

}

bool CDlgShow::IsButtonChecked(int index)
{
	_button* pbtn = m_buttons[index];	
	return (pbtn->uStatus == CAPTION_BTN_CHECKED);
	
}
int CDlgShow::GetButtonState(int index)
{
    return m_buttons[index]->uStatus;
}
void CDlgShow::OnPushButton()
{

	if(GetButtonState(0)==CAPTION_BTN_DISABLED) 
	{
		CString retLrc;
		int forward;
        m_pListWindow->GetNextPlay();
		strMusic=gstrSong;
		if(m_lrcBuf.Find(strMusic,retLrc,forward))
		{
			strLrc =retLrc;
		}
		PlayMusic();
 		return;
	}

    if(IsButtonChecked(0)) 
	{
		ChangeButtonState(0,CAPTION_BTN_NORMAL);
		m_player.pause();
	}
	else
	{
		ChangeButtonState(0,CAPTION_BTN_CHECKED);
		if(m_player.isPause())
			m_player.resume();
		else
		{
			if(strMusic.IsEmpty())
			{
              m_pListWindow->GetNextPlay();
			  strMusic=gstrSong;
			}
			PlayMusic();
		}
		//	m_player.resume();	
        ChangeButtonState(1,CAPTION_BTN_NORMAL);
	}
}
void CDlgShow::OnButtonStop()
{
    if(GetButtonState(1)==CAPTION_BTN_DISABLED) return;
    if(!IsButtonChecked(1)) 
	{
		ChangeButtonState(1,CAPTION_BTN_CHECKED);
	    m_player.Stop();
		if(GetButtonState(0)==CAPTION_BTN_CHECKED)
           ChangeButtonState(0,CAPTION_BTN_NORMAL);
		m_curLycris="";
	}

}

void CDlgShow::OnButtonShow()
{
    if(IsButtonChecked(3)) 
	{
		ChangeButtonState(3,CAPTION_BTN_NORMAL);
     	m_bListShow =0;

	}
	else
	{
		ChangeButtonState(3,CAPTION_BTN_CHECKED);			
		m_bListShow=1;
	}
	if(m_bListShow) m_pListWindow->ShowWindow(SW_SHOW);
	else m_pListWindow->ShowWindow(SW_HIDE);
}

void CDlgShow::OnSetTop()
{
  m_bTop =1-m_bTop;
  if(m_bTop)
  {
	  tagRECT rct;
	  this->GetWindowRect(&rct);
	  ::SetWindowPos(this->GetSafeHwnd(),HWND_TOPMOST,rct.left,rct.top,rct.right-rct.left,rct.bottom-rct.top,0); 
	  m_pListWindow->GetWindowRect(&rct);
	  ::SetWindowPos(m_pListWindow->GetSafeHwnd(),HWND_TOPMOST,rct.left,rct.top,rct.right-rct.left,rct.bottom-rct.top,0); 
  }
  else
  {
	  tagRECT rct;
	  this->GetWindowRect(&rct);
	  ::SetWindowPos(this->GetSafeHwnd(),HWND_NOTOPMOST,rct.left,rct.top,rct.right-rct.left,rct.bottom-rct.top,0); 
	  m_pListWindow->GetWindowRect(&rct);
	  ::SetWindowPos(m_pListWindow->GetSafeHwnd(),HWND_NOTOPMOST,rct.left,rct.top,rct.right-rct.left,rct.bottom-rct.top,0); 	  
  }
}

void CDlgShow::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI) 
{
	// TODO: Add your message handler code here and/or call default
	lpMMI->ptMinTrackSize.x   =   490;   	
	lpMMI->ptMinTrackSize.y   =   160;   

    
	
	CDialog::OnGetMinMaxInfo(lpMMI);
}

void CDlgShow::DrawSlider()
{	
	if(!(GetStyle()&WS_DLGFRAME))
	{
		return;
	}
	
	CDC* pDC =GetWindowDC();
	if(pDC==NULL)
	{
		return;
	}
	CBrush Brush(RGB(0,100,255));
	CBrush* pOldBrush = pDC->SelectObject(&Brush);
	if(pOldBrush==NULL)
	{
		return;
	}
	tagRECT rtTitle,rtWnd,rcWnd;
	GetWindowRect(&rtWnd);
	GetClientRect(&rcWnd);
	ClientToScreen(&rcWnd);
	rtTitle.left = GetSystemMetrics(SM_CXFRAME);
	rtTitle.top = GetSystemMetrics(SM_CYFRAME);
	rtTitle.right = rtWnd.right - rtWnd.left - GetSystemMetrics(SM_CXFRAME);
	rtTitle.bottom = rtTitle.top + GetSystemMetrics(SM_CYSIZE);
	

	//准备画图
	CBitmap* pBitmap = new CBitmap;
	BITMAP BmpInfo;
	CBitmap* pOldBitmap;
	CDC* pDisplayMemDC=new CDC;
	if(!pDisplayMemDC->CreateCompatibleDC(pDC))
	{
		//此处还有释放brush的操作2011
		return;
	}
	//重画最Caption
	pBitmap->LoadBitmap(IDB_001);
	pOldBitmap=(CBitmap*)pDisplayMemDC->SelectObject(pBitmap);
	pBitmap->GetBitmap(&BmpInfo);

    int itop =m_slideTop+1;
	int i;
	int m_slidePos =(int)(m_slideRatio*(rcWnd.right-rcWnd.left));
	for( i=rtTitle.left+1; i< m_slidePos; i+=2)
	{
		pDC->BitBlt(i, itop, BmpInfo.bmWidth, BmpInfo.bmHeight, pDisplayMemDC, 0, 0, SRCCOPY);
	}
	pDisplayMemDC->SelectObject(pOldBitmap);
	pBitmap->DeleteObject();	
	

	
	pBitmap->LoadBitmap(IDB_002);
	pOldBitmap=(CBitmap*)pDisplayMemDC->SelectObject(pBitmap);
	pBitmap->GetBitmap(&BmpInfo);	
	for( ; i< (rtWnd.right-rtWnd.left-rtTitle.left-1); i+=2)
	{
		pDC->BitBlt(i, itop, BmpInfo.bmWidth, BmpInfo.bmHeight, pDisplayMemDC, 0, 0, SRCCOPY);
	}


	pDisplayMemDC->SelectObject(pOldBitmap);
	pBitmap->DeleteObject();	
	ReleaseDC(pDisplayMemDC);
	delete pDisplayMemDC;
	delete pBitmap;
}

BOOL CDlgShow::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	// TODO: Add your message handler code here and/or call default
	if(m_pause==1) m_pause=0;
	g_setChanged =true;	
	return CDialog::OnMouseWheel(nFlags, zDelta, pt);
}

BOOL CDlgShow::InitDDraw()
{
	DWORD dwFlags;
	DDSURFACEDESC ddsd;
	HWND hwnd;
	hwnd =this->GetSafeHwnd();
	//创建DirectDraw对象
	DirectDrawCreate(NULL, &lpDD, NULL);
	
	//设置协作级别为窗口模式
	dwFlags = DDSCL_NORMAL;
	lpDD->SetCooperativeLevel(hwnd, dwFlags);
	
	//创建主页面
	ZeroMemory(&ddsd, sizeof(ddsd));
	ddsd.dwSize = sizeof(ddsd);
	ddsd.dwFlags = DDSD_CAPS;
	ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
	lpDD->CreateSurface(&ddsd, &lpDDSFront, NULL);

	//创建Clipper裁剪器
	lpDD->CreateClipper(0, &lpClipper, NULL);
	lpClipper->SetHWnd(0, hwnd);
	lpDDSFront->SetClipper(lpClipper);
	return TRUE;
}
int ii=0;
void CDlgShow::Flip()
{
	RECT rt;
	POINT pt;
	GetClientRect(&rt);
	pt.x=pt.y=0;
	ClientToScreen(&pt);
	OffsetRect(&rt, pt.x, pt.y);
	if(lpDDSBack) 
    {
		HRESULT rval=lpDDSFront->Blt(&rt, lpDDSBack, NULL, DDBLT_WAIT, NULL);
		if(rval == DDERR_SURFACELOST) lpDDSFront->Restore();
    }
	return ;
}

/*
1. 创建后援页面，大小等于窗口大小
2. 创建一个图片页面，大小等于图片size(调整后的)
3. 将图片页面拷贝到后援页面
4. 将TEXT写到后援页面
5. 将后援页面拷贝到主页面
6. 删除所有页面
*/

void CDlgShow::ShowLRC()
{

    int posx,posy;
	FONT_HIGH=window_height/18;
	if(FONT_HIGH<25) FONT_HIGH=25;
	FONT_WIDTH=FONT_HIGH/2;
	if(m_curLycris.IsEmpty()) return;
    //填充
	HDC bhdc;
	CDC bkdc;
	CBrush brush(RGB(0,0,0));
	lpDDSBack->GetDC(&bhdc);
	if(bhdc==0)
	{
		brush.Detach();
		return;
	}
	bkdc.Attach(bhdc);
	
	if(!m_curLycris.IsEmpty())
  {
  	//draw lrc 
	  posx =(window_width-FONT_WIDTH*m_curLycris.GetLength())/2;
		posy =window_height-FONT_HIGH-FONT_HIGH/3/*-FONT_HIGH-FONT_HIGH/3*/;	
		DrawText(&bkdc,m_curLycris,posx+FONT_HIGH/25,posy+FONT_HIGH/25,RGB(200,200,200));		
		DrawText(&bkdc,m_curLycris,posx,posy,RGB(0,0,0));	
	  //draw forward info
		if(/*g_head.forward!=0 && */m_forwardflag!=-1)
		{
			CString strForward;
			if(g_head.forward>=0) strForward.Format("+%0.2f秒",g_head.forward*0.1/10);
			if(g_head.forward<0) strForward.Format("%0.2f秒",g_head.forward*0.1/10);
			posx =(window_width-FONT_WIDTH*strForward.GetLength())/2;
			posy =window_height-FONT_HIGH-FONT_HIGH/3-FONT_HIGH-FONT_HIGH/3;
			DrawText(&bkdc,strForward,posx+FONT_HIGH/25,posy+FONT_HIGH/25,RGB(200,200,200));		
			DrawText(&bkdc,strForward,posx,posy,RGB(0,0,0));	
		}
  }
  
  //draw config info
  //释放
  if(m_pause)
  {
/*
      CString strCfgInfo="已暂停,按<空格键>继续";
	  posx =(window_width-FONT_WIDTH*strCfgInfo.GetLength())/2;
		posy =window_height-FONT_HIGH-FONT_HIGH/3-FONT_HIGH-FONT_HIGH/3-FONT_HIGH-FONT_HIGH/3;	
		DrawText(&bkdc,strCfgInfo,posx+FONT_HIGH/25,posy+FONT_HIGH/25,RGB(200,200,200));		
		DrawText(&bkdc,strCfgInfo,posx,posy,RGB(0,0,0));	  	
*/
  }
	lpDDSBack->ReleaseDC(bhdc);
	bkdc.Detach();
	brush.DeleteObject();
	
}

void CDlgShow::UpdateForward()
{
	if(!strLrc.IsEmpty())
		m_lrcBuf.Add(gstrSong,strLrc,g_head.forward);
	
}

void CDlgShow::HideInTray()
{
	NOTIFYICONDATA tnd;
	tnd.cbSize=sizeof(NOTIFYICONDATA);
	tnd.hWnd=this->m_hWnd;
	tnd.uID=IDR_MAINFRAME;
	tnd.uFlags=NIF_MESSAGE|NIF_ICON|NIF_TIP;
	tnd.uCallbackMessage=WM_MYMESSAGE_NOTIFY;
	tnd.hIcon=LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME));
	strcpy(tnd.szTip,"ssshow");
	Shell_NotifyIcon(NIM_ADD,&tnd);
    /*隐藏song列表*/
	if(m_bListShow) m_pListWindow->ShowWindow(SW_HIDE);
}

void CDlgShow::DelTray()
{
	NOTIFYICONDATA tnid;
	memset(&tnid,0,sizeof(NOTIFYICONDATA));
	tnid.cbSize=sizeof(NOTIFYICONDATA);
	tnid.hWnd=this->m_hWnd;
	tnid.uID=IDR_MAINFRAME;
	Shell_NotifyIcon(NIM_DELETE,&tnid);
}
int popflag=0;
LRESULT CDlgShow::OnClickTray(WPARAM wParam,LPARAM lParam)
{
	UINT uID;//发出该消息的图标的ID
	UINT uMouseMsg;//鼠标动作
	POINT pt;
	uID=(UINT) wParam;
	uMouseMsg=(UINT) lParam;
	if(popflag==1) return 0;
	if(uID!=IDR_MAINFRAME) 
	{
		return 0;
	}
	if(WM_LBUTTONDBLCLK==uMouseMsg)
	{
	  //m_bListShow=0;
	  OnRestore();
	  return 0;
	}
	popflag=1;
	if(uMouseMsg==WM_RBUTTONDOWN)
	{
		GetCursorPos(&pt);		
		CMenu menu;
		menu.CreatePopupMenu();		 
		menu.AppendMenu(MF_STRING,WM_MYMESSAGE_RESTORE,"Restore");
		menu.AppendMenu(MF_STRING,WM_MYMESSAGE_EXITTRAY,"Exit"); 
		menu.AppendMenu(MF_STRING,WM_MYMESSAGE_GUANYU,"About"); 
		menu.TrackPopupMenu(TPM_LEFTALIGN,pt.x,pt.y,this);
		HMENU hmenu=menu.Detach();
		menu.DestroyMenu();
	}
	popflag=0;
	return 0;
}

void CDlgShow::OnAbout()
{
  popflag=1;
  CAboutDlg dlg;
  dlg.DoModal();
  popflag=0;
}

void CDlgShow::OnRestore()
{
		//删除图标，并显示窗口
        DelTray();
		::AfxGetMainWnd()->ShowWindow(SW_SHOW);
		SendMessage(WM_SYSCOMMAND,SC_RESTORE,0);
//		if(g_pThread) g_pThread->ResumeThread();
}
void CDlgShow::OnForward()
{
	if(!strLrc.IsEmpty())
	{
		g_head.forward +=25  ;
		m_forwardflag=0;				
	}
			
}
void CDlgShow::OnBack()
{
	if(!strLrc.IsEmpty())
	{
		g_head.forward -=25;
		m_forwardflag=0;
	}
}
void CDlgShow::OnExitTray()
{
    DelTray();
    SendMessage(WM_CLOSE);
}

void CDlgShow::ShowCfgInfo()
{
   int posx,posy;
	FONT_HIGH=window_height/18;
	if(FONT_HIGH<25) FONT_HIGH=25;
	FONT_WIDTH=FONT_HIGH/2;
	HDC bhdc;
	CDC bkdc;
	CBrush brush(RGB(0,0,0));
	lpDDSBack->GetDC(&bhdc);
	if(bhdc==0)
	{
		brush.Detach();
		return;
	}
	bkdc.Attach(bhdc);
	if(g_head.forward!=0 && m_forwardflag!=-1)
	{
		CString strForward;
		if(g_head.forward>0) strForward.Format("+%0.2f秒",g_head.forward*0.1/10);
		if(g_head.forward<0) strForward.Format("%0.2f秒",g_head.forward*0.1/10);
		posx =(window_width-FONT_WIDTH*strForward.GetLength())/2;
		posy =window_height-FONT_HIGH-FONT_HIGH/3-FONT_HIGH-FONT_HIGH/3;
		DrawText(&bkdc,strForward,posx+FONT_HIGH/25,posy+FONT_HIGH/25,RGB(200,200,200));		
		DrawText(&bkdc,strForward,posx,posy,RGB(0,0,0));	
	}
	lpDDSBack->ReleaseDC(bhdc);
	bkdc.Detach();
	brush.DeleteObject();
}

void CDlgShow::clearScreen()
{
	//清屏
	tagRECT clrct;
	HDC hdc;
	CDC bkdc;
	GetClientRect(&clrct);
	CBrush brush(RGB(0,0,0));
	if(lpDDSBack==NULL) return;
	lpDDSBack->GetDC(&hdc);
	if(hdc==0) 
	{
		return;
	}
	bkdc.Attach(hdc);
	bkdc.FillRect(&clrct,&brush);	
	bkdc.Detach();
	brush.DeleteObject();
	lpDDSBack->ReleaseDC(hdc);
	
}

void CDlgShow::InsertMusic(CString str)
{
   m_pListWindow->InsertString(str);
   UpdateData(FALSE);
}

void CDlgShow::ModifyLrc()
{
	if(g_head.forward!=0)
	{
		CString strr;
		strr.Format("Lrc Changed,Need Save?");
		if(AfxMessageBox(strr,MB_YESNO)==IDYES)
		{
			m_lrc.Modify(g_head.forward);
			g_head.forward=0;								
		}
	}
				
}


void CDlgShow::OnOpenMusic(char *song)
{
	// TODO: Add your message handler code here and/or call default
	if(g_type!=SHOW_TYPE_PRO) return;
	int forward=0;
	{
		CString retLrc;
		strLrc.Empty();
		strMusic =song;
		if(m_lrcBuf.Find(strMusic,retLrc,forward))
		{
			strLrc =retLrc;
			g_head.forward =forward;
		}	
		g_head.forward =0;
		if(m_pListWindow)
		{
		//	m_pListWindow->ResetList(); //lxb 2011
			int cursel =m_pListWindow->InsertString(strMusic);	
			m_pListWindow->SetSel(cursel);	
		}
		PlayMusic();
	}
		
	
}

int flagg=0;
int CDlgShow::ShowLrc2(void)
{	
	FONT_HIGH=28;
	FONT_WIDTH=14;

	CDC* pWinDC = GetWindowDC();
	if (pWinDC)
	{
		tagRECT tg;
		GetWindowRect(&tg);
        FillRevClient(2);			
        //DrawText(pWinDC,m_curLycris,100,100);
		this->DrawText(pWinDC,m_curLycris,(tg.right-tg.left-FONT_WIDTH*m_curLycris.GetLength())/2,tg.bottom-tg.top-g_client_bottom+2,RGB(128,128,128));
		ReleaseDC(pWinDC);
	} 
	return 0;
}

int CDlgShow::updateSpectrum(void)
{
	CSpectrumAnalyser* pSpectrumAnalyser=NULL;
	tagRECT rt;
	this->GetClientRect(&rt);
	window_height =rt.bottom -rt.top/*+FRAME_WIDTH*2*/;
	window_width  =rt.right  -rt.left/*+FRAME_WIDTH*2*/;	
	if(m_player.IsPlaying())
	{
	   pSpectrumAnalyser =m_player.GetSpectrumAnalyser();
	   if(pSpectrumAnalyser) pSpectrumAnalyser->update();
	}
	return 0;
}

void CDlgShow::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
   if(m_player.IsPlaying()) 								   
   {
      LARGE_INTEGER li;
	  CRect rtWnd;
	  GetWindowRect(&rtWnd);
      if( point.y <= 4)
	  {
	  m_slideRatio = point.x*1.0/ (rtWnd.right -rtWnd.left);
	  li.QuadPart =m_slideRatio*m_player.getSize().QuadPart;
	  m_player.SetPosition(li);
	  DrawSlider();								  
	  }

   }
   CDialog::OnLButtonDown(nFlags, point);
}

void CDlgShow::OnSetFocus(CWnd* pOldWnd)
{
	CDialog::OnSetFocus(pOldWnd);
	/*
	CDC* pWinDC = GetWindowDC();
	if (pWinDC)
	{
		DrawTitleBar(pWinDC);
		ReleaseDC(pWinDC);
	} */
	

	// TODO: 在此处添加消息处理程序代码
}

void CDlgShow::OnKillFocus(CWnd* pNewWnd)
{
	CDialog::OnKillFocus(pNewWnd);
	// TODO: 在此处添加消息处理程序代码
}

void CDlgShow::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	CDialog::OnActivate(nState, pWndOther, bMinimized);

	// TODO: 在此处添加消息处理程序代码
}
