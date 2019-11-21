// IniFile.cpp: implementation of the CIniFile class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "IniFile.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIniFile::CIniFile()
{

}

CIniFile::~CIniFile()
{

}

int CIniFile::readInt(char *item, char *key)
{
  return GetPrivateProfileInt(item,key,-1,m_filename);
}

int CIniFile::readText(char *item, char *key, char *result,int size)
{
 return GetPrivateProfileString(item,key,NULL,result,size,m_filename);
}



BOOL CIniFile::write(char *item, char *key, char *str)
{
	return WritePrivateProfileString(item,key,str,m_filename);	
}

BOOL CIniFile::InitFile(char *filename)
{
	if(strlen(filename) > MAX_PATH_LENGTH)
		return FALSE;
    strcpy(m_filename,filename);
	return TRUE;
}

BOOL CIniFile::findItem(char *item)
{
	return false;
}

BOOL CIniFile::writeInt(char *item, char *key, int val)
{
	CString str;
	str.Format("%d",val);
	return WritePrivateProfileString(item,key,str,m_filename);		
}
