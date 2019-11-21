// LrcBuf.cpp: implementation of the CLrcBuf class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DirectShowPP.h"
#include "LrcBuf.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CLrcBuf::CLrcBuf()
{
	
}

CLrcBuf::~CLrcBuf()
{

}

bool CLrcBuf::Load(CString strFile)
{
	CFile file;
	if(!file.Open(strFile,CFile::modeRead))
	{
		if(!file.Open(strFile,CFile::modeCreate))
		{
			return false;
		}
	}
    file.Close();
    m_iniFile.InitFile(strFile.GetBuffer(0));
 
	char path2[MAX_PATH_LENGTH];
	path2[0]=0;
	if(m_iniFile.readText("CONFIG","search_path",path2,255)<=0)
	{
		GetCurrentDirectory(MAX_PATH_LENGTH,path2);
		if(strlen(path2)<=3) 
		{
			//��Ŀ¼����Ϊ����·��
			path2[0]=0;
		}
	}
	this->m_searchPath =path2;	
	return true;
}

bool CLrcBuf::Store(CString strFile)
{    
	return false;
}
/*�����������,��Ҫ�ж��ļ��Ƿ����,���������������,������������false��������ӵ������ļ���*/
//���ӶԵ�ǰĿ¼�����������Ӷ����������ļ���ȷ��
//�޸Ļ����ļ���·��
#include "afxtempl.h"
extern CString ParseSong(CString);
bool CLrcBuf::Find(CString song, CString &lrcFilePath,int &forward)
{
    if(song.IsEmpty()) return false;	
	song.MakeUpper();
	////////////////////////////////////////////////////////////
	char  filedir[4];
	char  filepath[256];
	char  fileName[256];
	char  path[256];
	CString lrcFile;
	CString lrcFind;
	fileName[0]=0;
	path[0] =0;    	
	forward=0;
	//��full path�г�ȡ�ļ���
	_splitpath(song,filedir,filepath,fileName,0);
	//���lrc buf���Ƿ��д˼�¼
	int rdcount =m_iniFile.readText("LRC_PATH",fileName,path,255);
    if(rdcount>0)
	{
        //����ļ��Ƿ���ڣ���������򷵻�
		CFile file;
		//�Ӽ�¼�з��������ļ�·����forward
		char* pToken=strtok(path,",");
		if(pToken) 
		{
			lrcFile =pToken;
		}
		pToken=strtok(NULL,",");
		if(pToken)
		{
			forward=0; /*atoi(pToken);*/
		}		
		if(file.Open(path,CFile::modeRead))
		{
			file.Close();
			lrcFilePath=lrcFile;
			return true;
		}
	}	

	CString strKey =ParseSong(fileName);
	if(strKey.IsEmpty()) return false;
    int bFind =false;



	CFileFind ff;
	CArray<CString,CString> dirArray;
    BOOL res=FALSE;

	//��������ǰ��������·��
    lrcFind =filedir;
	lrcFind+=filepath;
	lrcFind+=fileName;
	lrcFind+=".LRC";
	CFile file;
	if(file.Open(lrcFind,CFile::modeRead))
	{
		file.Close();
        lrcFilePath=lrcFind;
		bFind=true;
		goto _EExit;
	}

	//������ָ����lrcĿ¼
	if(m_searchPath.IsEmpty()) return false;		
	dirArray.Add(m_searchPath);
	while(dirArray.GetSize())
	{
		CString strCurrentDir=dirArray.GetAt(0);
		dirArray.RemoveAt(0);
		if(strCurrentDir.Right(1)!="\\") strCurrentDir+="\\*.*";
		res = ff.FindFile(strCurrentDir);
		while( res )
		{
			res = ff.FindNextFile();
			if(ff.IsDots()) continue;
			if(ff.IsDirectory())
			{	
				dirArray.Add(ff.GetFilePath());
			}
			else
			{   				
				lrcFind =ff.GetFileName();
				lrcFind.MakeUpper();
				if(lrcFind.Right(4)==".LRC")
				{									
					if(lrcFind.Find(strKey)!=-1)
					{
						bFind =true;
						lrcFilePath =ff.GetFilePath();
						lrcFind =lrcFilePath;
						goto _EExit;
					}
				}
			}			
		}
		ff.Close();

	}//while

	//������ǧǧ����Ŀ¼
	char strwinpath[256];
	if(GetWindowsDirectory(strwinpath,256)==0) 
	{
		return false;
	}
    strcpy(strwinpath+3,"\\Program Files\\TTPlayer\\Lyrics\\*.*");
	res = ff.FindFile(strwinpath);
	while( res )
	{
		res = ff.FindNextFile();
		if(ff.IsDots()) continue;
		if(ff.IsDirectory())
		{	
			continue;
		}
		else
		{   				
			lrcFind =ff.GetFileName();
			lrcFind.MakeUpper();
			if(lrcFind.Right(4)==".LRC")
			{									
				if(lrcFind.Find(strKey)!=-1)
				{
					bFind =true;
					lrcFilePath =ff.GetFilePath();
					lrcFind =lrcFilePath;
					goto _EExit;
				}
			}
		}			
	}
	ff.Close();


_EExit:
   if(bFind)
   {
	     lrcFind +=",0";
         m_iniFile.write("LRC_PATH",fileName,lrcFind.GetBuffer(0));
		 return true;
   }
   return false;
}
//���ԭ��������ɾ��
void CLrcBuf::Add(CString song, CString lrcPath,int forward)
{
	CString strTmp=lrcPath;
	char   fileName[256];
	fileName[0]=0;
	song.MakeUpper(); 
	_splitpath(song.GetBuffer(0),0,0,fileName,0);
//	strTmp.Format(",%d",forward);
	strTmp=lrcPath;/*+strTmp;*/
	m_iniFile.write("LRC_PATH",fileName,strTmp.GetBuffer(0));	
}

void CLrcBuf::Del(CString song)
{

}

void CLrcBuf::SetSearchPath(CString path)
{
   m_searchPath =path;
   m_iniFile.write("CONFIG","search_path",path.GetBuffer(0));
}

bool CLrcBuf::Changed()
{

	return true;
}

CString CLrcBuf::GetSearchPath()
{
  return m_searchPath;
}
int CLrcBuf::GetMusicInfo(CString strFile, CString &strTitle, CString &strArtist)
{

	return 0;
}