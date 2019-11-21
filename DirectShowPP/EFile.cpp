// EFile.cpp: implementation of the CEFile class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "EFile.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEFile::CEFile()
{

}

CEFile::~CEFile()
{

}

int CEFile::ReadOneLine(CString& strline)
{
	char buf[2000];
	int i=0;
	char c;
	int state=0;
	memset(buf,0,2000);
	while(Read(&c,1)>0)
	{
		buf[i]=c;		
		i++;
		if(i>=2000) goto NEXT;
		switch(state) {
		case 0:
			if(c==0x0d) state=1;
			//if(c==0x0a) { state=2; goto NEXT;}
			break;
		case 1:
			if(c==0x0a) 
			{
			//read one line and return
				state=2;
				goto NEXT;
			}
			else if(c==0x0d)
			{
				state=1;
			}
			else
			{
				state =0;
			}
			break;
		}
		
	}
NEXT:
	if(state==2) 
	{		
		i=i-2;		
		buf[i]=0;
	}
	if(i>0) strline =buf;
    return i;
}

BOOL CEFile::Open(LPCTSTR lpszFileName, UINT nOpenFlags, CFileException *pError)
{	
	BOOL ret;
	ret =CFile::Open(lpszFileName,nOpenFlags,pError);
	if(ret)
	{
		m_open=TRUE;
		m_file =lpszFileName;
	}
	else   
	{
		m_open =FALSE;
	}
	return  ret;
}

void CEFile::Close()
{
	CFile::Close();
	m_open =FALSE;
	m_file.Empty();
}

BOOL CEFile::isOpen()
{
  return m_open;
}
