// IniFile.h: interface for the CIniFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INIFILE_H__7B9F30B5_F375_4EE3_94E5_D208BE05C41B__INCLUDED_)
#define AFX_INIFILE_H__7B9F30B5_F375_4EE3_94E5_D208BE05C41B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#define  MAX_PATH_LENGTH 400
class CIniFile  
{
public:
	BOOL writeInt(char* item,char* key,int val);
	BOOL findItem(char* item);
	BOOL InitFile(char* filename);
	BOOL write(char* index, char* name, char *str);
	int readText(char* index, char* name,char* result,int size);
	int readInt(char* index, char* name);
	CIniFile();
	virtual ~CIniFile();
private:
	char m_filename[MAX_PATH_LENGTH];
};

#endif // !defined(AFX_INIFILE_H__7B9F30B5_F375_4EE3_94E5_D208BE05C41B__INCLUDED_)
