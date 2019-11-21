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
			//根目录不作为搜索路径
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
/*如果缓存中有,则还要判断文件是否存在,如果不存在则搜索,搜索不到返回false，否则添加到缓存文件中*/
//增加对当前目录的搜索，增加对新搜索到文件的确认
//修改缓存文件的路径
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
	//从full path中抽取文件名
	_splitpath(song,filedir,filepath,fileName,0);
	//检查lrc buf中是否有此记录
	int rdcount =m_iniFile.readText("LRC_PATH",fileName,path,255);
    if(rdcount>0)
	{
        //检查文件是否存在，如果存在则返回
		CFile file;
		//从记录中分离出歌词文件路径和forward
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

	//先搜索当前歌曲所在路径
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

	//再搜索指定的lrc目录
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

	//再搜索千千静听目录
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
//如果原来已有则删除
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