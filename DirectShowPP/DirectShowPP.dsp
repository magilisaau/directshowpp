# Microsoft Developer Studio Project File - Name="SSSHOW" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=SSSHOW - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "DirectShowPP.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "DirectShowPP.mak" CFG="SSSHOW - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "SSSHOW - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "SSSHOW - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "SSSHOW - Win32 Release"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 winmm.lib lib/JpegLib.lib shlwapi.lib DDRAW.LIB /nologo /subsystem:windows /machine:I386 /out:"fabu/SSSHOW.EXE"

!ELSEIF  "$(CFG)" == "SSSHOW - Win32 Debug"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 winmm.lib lib/jpeglib.lib shlwapi.lib DDRAW.LIB Msimg32.lib /nologo /subsystem:windows /debug /machine:I386 /out:"fabu\SSSHOW.EXE" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "SSSHOW - Win32 Release"
# Name "SSSHOW - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\Bmpfile.cpp
# End Source File
# Begin Source File

SOURCE=.\ComDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ConfigDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\CZoom.cpp
# End Source File
# Begin Source File

SOURCE=.\DES.cpp
# End Source File
# Begin Source File

SOURCE=.\dibapi.cpp
# End Source File
# Begin Source File

SOURCE=.\DirectShowPP.cpp
# End Source File
# Begin Source File

SOURCE=.\DirectShowPP.rc
# End Source File
# Begin Source File

SOURCE=.\DirectShowPPDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\DirectShowPPView.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgASS.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgCertificate.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgInputPwd.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgShow.cpp
# End Source File
# Begin Source File

SOURCE=.\EFile.cpp
# End Source File
# Begin Source File

SOURCE=.\IniFile.cpp
# End Source File
# Begin Source File

SOURCE=.\Jpegfile.cpp
# End Source File
# Begin Source File

SOURCE=.\Lrc.cpp
# End Source File
# Begin Source File

SOURCE=.\LrcBuf.cpp
# End Source File
# Begin Source File

SOURCE=.\LrcSearchDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\MainFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\MusicPlay.cpp
# End Source File
# Begin Source File

SOURCE=.\MyListBox.cpp
# End Source File
# Begin Source File

SOURCE=.\ProBar.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Wave.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\BMPFILE.h
# End Source File
# Begin Source File

SOURCE=.\ComDlg.h
# End Source File
# Begin Source File

SOURCE=.\ConfigDlg.h
# End Source File
# Begin Source File

SOURCE=.\dibapi.h
# End Source File
# Begin Source File

SOURCE=.\DirectShowPP.h
# End Source File
# Begin Source File

SOURCE=.\DirectShowPPDoc.h
# End Source File
# Begin Source File

SOURCE=.\DirectShowPPView.h
# End Source File
# Begin Source File

SOURCE=.\DlgASS.h
# End Source File
# Begin Source File

SOURCE=.\DlgCertificate.h
# End Source File
# Begin Source File

SOURCE=.\DlgInputPwd.h
# End Source File
# Begin Source File

SOURCE=.\DlgShow.h
# End Source File
# Begin Source File

SOURCE=.\EFile.h
# End Source File
# Begin Source File

SOURCE=.\IniFile.h
# End Source File
# Begin Source File

SOURCE=.\Jpegfile.h
# End Source File
# Begin Source File

SOURCE=.\Lrc.h
# End Source File
# Begin Source File

SOURCE=.\LrcBuf.h
# End Source File
# Begin Source File

SOURCE=.\LrcSearchDlg.h
# End Source File
# Begin Source File

SOURCE=.\MainFrm.h
# End Source File
# Begin Source File

SOURCE=.\MusicPlay.h
# End Source File
# Begin Source File

SOURCE=.\MyListBox.h
# End Source File
# Begin Source File

SOURCE=.\myresource.h
# End Source File
# Begin Source File

SOURCE=.\ProBar.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\Wave.h
# End Source File
# Begin Source File

SOURCE=.\Zoom.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\001.bmp
# End Source File
# Begin Source File

SOURCE=.\res\002.bmp
# End Source File
# Begin Source File

SOURCE=.\res\011.bmp
# End Source File
# Begin Source File

SOURCE=.\123.bmp
# End Source File
# Begin Source File

SOURCE=.\res\3232.ico
# End Source File
# Begin Source File

SOURCE=.\res\3232doc.ico
# End Source File
# Begin Source File

SOURCE=.\res\bitmap2.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bitmap3.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bitmap4.bmp
# End Source File
# Begin Source File

SOURCE=.\res\button.bmp
# End Source File
# Begin Source File

SOURCE=.\res\button_common.bmp
# End Source File
# Begin Source File

SOURCE=.\res\close.bmp
# End Source File
# Begin Source File

SOURCE=.\res\DirectShowPP.rc2
# End Source File
# Begin Source File

SOURCE=.\res\down.bmp
# End Source File
# Begin Source File

SOURCE=.\res\down1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ExeIco11.ico
# End Source File
# Begin Source File

SOURCE=.\res\hel_focu.bmp
# End Source File
# Begin Source File

SOURCE=.\res\hel_norm.bmp
# End Source File
# Begin Source File

SOURCE=.\res\icon2.ico
# End Source File
# Begin Source File

SOURCE=.\res\Left_Down.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Left_Down1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\line.bmp
# End Source File
# Begin Source File

SOURCE=.\res\m_button2.bmp
# End Source File
# Begin Source File

SOURCE=.\res\m_button3.bmp
# End Source File
# Begin Source File

SOURCE=.\res\m_button4.bmp
# End Source File
# Begin Source File

SOURCE=.\res\m_button5.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Max.bmp
# End Source File
# Begin Source File

SOURCE=.\res\middle.bmp
# End Source File
# Begin Source File

SOURCE=.\res\min.bmp
# End Source File
# Begin Source File

SOURCE=.\res\MmShow.ico
# End Source File
# Begin Source File

SOURCE=.\res\MmShowDoc.ico
# End Source File
# Begin Source File

SOURCE=.\res\net.ico
# End Source File
# Begin Source File

SOURCE=.\res\Normal.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Right_Down.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Right_Down1.bmp
# End Source File
# Begin Source File

SOURCE=.\rc\title_Left.bmp
# End Source File
# Begin Source File

SOURCE=.\res\title_Left.bmp
# End Source File
# Begin Source File

SOURCE=.\res\title_Left1.bmp
# End Source File
# Begin Source File

SOURCE=.\rc\title_Right.bmp
# End Source File
# Begin Source File

SOURCE=.\res\title_Right.bmp
# End Source File
# Begin Source File

SOURCE=.\res\title_Right1.bmp
# End Source File
# Begin Source File

SOURCE=.\rc\top.bmp
# End Source File
# Begin Source File

SOURCE=.\res\top.bmp
# End Source File
# Begin Source File

SOURCE=.\rc\top1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\top1.bmp
# End Source File
# Begin Source File

SOURCE=".\res\¹¤¾ßÌõ2_2.bmp"
# End Source File
# End Group
# Begin Source File

SOURCE=.\camera4.wav
# End Source File
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
