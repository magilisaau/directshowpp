; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CConfigDlg
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "directshowpp.h"
LastPage=0

ClassCount=14
Class1=CComDlg
Class2=CConfigDlg
Class3=CDirectShowPPApp
Class4=CAboutDlg
Class5=CDirectShowPPDoc
Class6=CDirectShowPPView
Class7=CDlgASS
Class8=CDlgShow
Class9=CLrcSearchDlg
Class10=CMainFrame
Class11=CMyListBox
Class12=CProBar

ResourceCount=11
Resource1=IDD_DIALOG1
Resource2=IDD_DIALOG4
Resource3=IDD_PROGRESS
Resource4=IDD_PARAM_CONFIG
Resource5=IDD_LRC_PATH
Resource6=IDD_LRC_ASS
Resource7=IDD_DLG_INPUT_PASS
Resource8=IDD_COMDLG_DIALOG
Resource9=IDD_ABOUTBOX
Class13=CDlgInputPwd
Resource10=IDR_MAINFRAME
Class14=CDlgCertificate
Resource11=IDD_DLG_CERT

[CLS:CComDlg]
Type=0
BaseClass=CDialog
HeaderFile=ComDlg.h
ImplementationFile=ComDlg.cpp
LastObject=CComDlg
Filter=W
VirtualFilter=dWC

[CLS:CConfigDlg]
Type=0
BaseClass=CDialog
HeaderFile=ConfigDlg.h
ImplementationFile=ConfigDlg.cpp
LastObject=CConfigDlg

[CLS:CDirectShowPPApp]
Type=0
BaseClass=CWinApp
HeaderFile=DirectShowPP.h
ImplementationFile=DirectShowPP.cpp

[CLS:CAboutDlg]
Type=0
BaseClass=CDialog
HeaderFile=DirectShowPP.h
ImplementationFile=DirectShowPP.cpp
LastObject=CAboutDlg

[CLS:CDirectShowPPDoc]
Type=0
BaseClass=CDocument
HeaderFile=DirectShowPPDoc.h
ImplementationFile=DirectShowPPDoc.cpp

[CLS:CDirectShowPPView]
Type=0
BaseClass=CView
HeaderFile=DirectShowPPView.h
ImplementationFile=DirectShowPPView.cpp

[CLS:CDlgASS]
Type=0
BaseClass=CDialog
HeaderFile=DlgASS.h
ImplementationFile=DlgASS.cpp

[CLS:CDlgShow]
Type=0
BaseClass=CDialog
HeaderFile=DlgShow.h
ImplementationFile=DlgShow.cpp
LastObject=CDlgShow

[CLS:CLrcSearchDlg]
Type=0
BaseClass=CDialog
HeaderFile=LrcSearchDlg.h
ImplementationFile=LrcSearchDlg.cpp

[CLS:CMainFrame]
Type=0
BaseClass=CFrameWnd
HeaderFile=MainFrm.h
ImplementationFile=MainFrm.cpp
LastObject=CMainFrame

[CLS:CMyListBox]
Type=0
BaseClass=CListBox
HeaderFile=MyListBox.h
ImplementationFile=MyListBox.cpp

[CLS:CProBar]
Type=0
BaseClass=CDialog
HeaderFile=ProBar.h
ImplementationFile=ProBar.cpp

[DLG:IDD_COMDLG_DIALOG]
Type=1
Class=CComDlg
ControlCount=1
Control1=IDC_LIST1,listbox,1342243075

[DLG:IDD_PARAM_CONFIG]
Type=1
Class=CConfigDlg
ControlCount=21
Control1=IDC_RADIO1,button,1342308361
Control2=IDC_RADIO2,button,1342177289
Control3=IDC_RADIO3,button,1342242825
Control4=IDC_RADIO6,button,1342308361
Control5=IDC_RADIO7,button,1342177289
Control6=IDC_RADIO8,button,1342177289
Control7=IDC_RADIO4,button,1342308361
Control8=IDC_RADIO5,button,1342177289
Control9=IDC_RADIO9,button,1342177289
Control10=IDC_RADIO10,button,1342177289
Control11=IDC_EDIT1,edit,1350631552
Control12=IDOK,button,1342242817
Control13=IDCANCEL,button,1342242816
Control14=IDC_STATIC,static,1342308352
Control15=IDC_STATIC,static,1342308352
Control16=IDC_STATIC,static,1342308352
Control17=IDC_STATIC,static,1342308352
Control18=IDC_STATIC,static,1476526080
Control19=IDC_STATIC,static,1476526080
Control20=IDC_STATIC,static,1476526080
Control21=IDC_STATIC,static,1342308352

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[DLG:IDD_LRC_ASS]
Type=1
Class=CDlgASS
ControlCount=5
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_EDIT1,edit,1350631552
Control4=IDC_BUTTON1,button,1342242816
Control5=IDC_STATIC,static,1342308352

[DLG:IDD_DIALOG1]
Type=1
Class=CDlgShow
ControlCount=0

[DLG:IDD_LRC_PATH]
Type=1
Class=CLrcSearchDlg
ControlCount=5
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_EDIT1,edit,1350631552
Control4=IDC_BUTTON1,button,1342242816
Control5=IDC_STATIC,static,1342308352

[DLG:IDD_DIALOG2]
Type=1
Class=CProBar

[DLG:IDD_DIALOG4]
Type=1
Class=?
ControlCount=2
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816

[DLG:IDD_DLG_INPUT_PASS]
Type=1
Class=CDlgInputPwd
ControlCount=6
Control1=IDC_EDIT1,edit,1350631456
Control2=IDC_EDIT2,edit,1350631584
Control3=IDOK,button,1342242817
Control4=IDCANCEL,button,1342242816
Control5=IDC_STATIC,static,1342308352
Control6=IDC_STATIC,static,1342308352

[MNU:IDR_MAINFRAME]
Type=1
Class=?
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_FILE_SAVE_AS
Command5=ID_FILE_MRU_FILE1
Command6=ID_APP_EXIT
Command7=ID_EDIT_UNDO
Command8=ID_EDIT_CUT
Command9=ID_EDIT_COPY
Command10=ID_EDIT_PASTE
Command11=ID_APP_ABOUT
CommandCount=11

[ACL:IDR_MAINFRAME]
Type=1
Class=?
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_EDIT_UNDO
Command5=ID_EDIT_CUT
Command6=ID_EDIT_COPY
Command7=ID_EDIT_PASTE
Command8=ID_EDIT_UNDO
Command9=ID_EDIT_CUT
Command10=ID_EDIT_COPY
Command11=ID_EDIT_PASTE
Command12=ID_NEXT_PANE
Command13=ID_PREV_PANE
CommandCount=13

[DLG:IDD_PROGRESS]
Type=1
Class=?
ControlCount=1
Control1=IDC_PROGRESS1,msctls_progress32,1350565889

[CLS:CDlgInputPwd]
Type=0
HeaderFile=DlgInputPwd.h
ImplementationFile=DlgInputPwd.cpp
BaseClass=CDialog
Filter=D
LastObject=IDC_EDIT1
VirtualFilter=dWC

[DLG:IDD_DLG_CERT]
Type=1
Class=CDlgCertificate
ControlCount=2
Control1=IDC_EDIT1,edit,1350631584
Control2=IDC_STATIC,static,1342308352

[CLS:CDlgCertificate]
Type=0
HeaderFile=DlgCertificate.h
ImplementationFile=DlgCertificate.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=CDlgCertificate

