// Lrc.cpp: implementation of the CLrc class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Lrc.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CString Convert(CString str, int sourceCodepage, int targetCodepage) 
{ 
	int len=str.GetLength(); 
	
	int unicodeLen=MultiByteToWideChar(sourceCodepage,0,str,-1,NULL,0); 
	
	wchar_t * pUnicode; 
	pUnicode=new wchar_t[unicodeLen+1]; 
	
	memset(pUnicode,0,(unicodeLen+1)*sizeof(wchar_t)); 
	
	
	MultiByteToWideChar(sourceCodepage,0,str,-1,(LPWSTR)pUnicode,unicodeLen); 
	
	BYTE * pTargetData=NULL; 
	int targetLen=WideCharToMultiByte(targetCodepage,0,(LPWSTR)pUnicode,-1,(char *)pTargetData,0,NULL,NULL); 
	
	pTargetData=new BYTE[targetLen+1]; 
	memset(pTargetData,0,targetLen+1); 
	
	WideCharToMultiByte(targetCodepage,0,(LPWSTR)pUnicode,-1,(char *)pTargetData,targetLen,NULL,NULL); 
	
	CString rt; 
	rt.Format("%s",pTargetData); 
	
	delete pUnicode; 
	delete pTargetData; 
	return rt; 
	
} 

CLrc::CLrc()
{
  offset=0;
}

CLrc::~CLrc()
{
   Free();
}

BOOL CLrc::Open(CString strFile)
{
	unsigned char* pFileTxt =0;
	CFile lrcFile;
	if(!lrcFile.Open(strFile.GetBuffer(0),CFile::modeRead)) return FALSE;
	int len =lrcFile.GetLength();
	if(len<=0) 
	{
		lrcFile.Close();
		return FALSE;
	}
    pFileTxt =new unsigned char[len+1];
	memset(pFileTxt,0,len+1);
    if(lrcFile.Read(pFileTxt,len)>0)
	{	  	  
     CString strtmp; 
	 if( pFileTxt[0]==0xEF && 
		 pFileTxt[1]==0xBB && 
		 pFileTxt[2]==0xBF)
	 {
		 strtmp =Convert(pFileTxt,CP_UTF8,936); 
	 }
	 else
	 {
		 strtmp=pFileTxt;
	 }
	 int ilen =strtmp.GetLength();
	 for(int k=ilen-1; k>0; k--)
	 {
		 if(strtmp.GetAt(k)=='[' && strtmp.GetAt(k-1)!=']' && strtmp.GetAt(k-1)!=0x0a)
		 {
			 strtmp.Insert(k,"\r\n");
		 }
	 }

	  
      char* pToken =strtok(strtmp.GetBuffer(0),"\r\n");
	  while (pToken) 
	  {
          //每个Token就是一行
		  TLrcRecord* pTmpRecord[20];
		  int iTmpRecord=0;
		  int istart=0,iend=-1,iTxtStart=-1;
		  CString strLine =pToken;
		  //strLine.Replace(" ","");  //trim space
		  strLine.Replace("\t","");  //trim \t
		  for(int i=0; i<20; i++)
		  {
			  pTmpRecord[i] =0;
		  }
		  while((istart=strLine.Find("[",iend+1))!=-1)
		  {
			  char strMi[3],strSec[3],strmSec[3];
			  iend =strLine.Find("]",istart+1);
			  if(iend== -1) break;
			  //取出ti ar al by等信息
			  CString strTmp;
			  strTmp=strLine.Mid(istart+1,iend-1);
			  if(!strTmp.IsEmpty())
			  {
				 int ipos;
				 ipos =strTmp.Find("ti:");
				 if(ipos!=-1)
				 {
					 ti =strTmp.Right(strTmp.GetLength()-ipos-3);
				 }
				 ipos =strTmp.Find("ar:");
				 if(ipos!=-1)
				 {
					 ar =strTmp.Right(strTmp.GetLength()-ipos-3);
				 }
				 ipos =strTmp.Find("by:");
				 if(ipos!=-1)
				 {
					 by =strTmp.Right(strTmp.GetLength()-ipos-3);
				 }
				 ipos =strTmp.Find("al:");
				 if(ipos!=-1)
				 {
					 al =strTmp.Right(strTmp.GetLength()-ipos-3);
				 }
				 ipos =strTmp.Find("offset:");
				 if(ipos!=-1)
				 {
   				 
				   offset =atoi(strTmp.Right(strTmp.GetLength()-ipos-7).GetBuffer(0))/10;
         
				 }				 
			  }

			  if( iend-istart!=9 && iend-istart!=6) continue;
			  strMi[0] = strLine.GetAt(istart+1);
			  strMi[1] = strLine.GetAt(istart+2);
			  strSec[0] = strLine.GetAt(istart+4);
			  strSec[1] = strLine.GetAt(istart+5);
			  if(iend-istart==9)
			  {
				  strmSec[0] =strLine.GetAt(istart+7);
				  strmSec[1] =strLine.GetAt(istart+8);
			  }
			  else
			  {
				  strmSec[0] ='0';
				  strmSec[1] ='0';
			  }
              
			  if(iTmpRecord>=20) break;
			  pTmpRecord[iTmpRecord] =new TLrcRecord;
              pTmpRecord[iTmpRecord]->m_time = 
				  ((strMi[0]-'0')*10+(strMi[1]-'0'))*60*100 +
				  ((strSec[0]-'0')*10+(strSec[1]-'0'))*100 +
				  ((strmSec[0]-'0')*10+(strmSec[1]-'0'));
			  iTmpRecord++;
			  iTxtStart =iend+1;
		  }
		  if(iTxtStart!=-1)
		  {//插入排序
            for(int i=0; i< iTmpRecord;  i++)   
			{
               int j=0;
			   while (j<m_recordArray.GetSize() && pTmpRecord[i]->m_time > m_recordArray[j]->m_time)  j++;
               pTmpRecord[i]->m_str =strLine.Right(strLine.GetLength()-iTxtStart);
			   m_recordArray.InsertAt(j,pTmpRecord[i],1);
			}

		  }
		  pToken =strtok(NULL,"\r\n");
	  }
	}
	lrcFile.Close();
	delete[] pFileTxt;
	m_file =strFile;
	return TRUE;
}

void CLrc::Free()
{
	for(int i=0; i<m_recordArray.GetSize();i++)
	{
		delete m_recordArray[i];
	}
	m_recordArray.RemoveAll();
}

CString CLrc::GetTxt(UINT time)
{
	int i =0; 
	while((i<m_recordArray.GetSize()) && ((time+offset) >= m_recordArray[i]->m_time))
	{
		i ++;
	}

	if(i-1 >=0) return m_recordArray[i-1]->m_str;
    if(m_recordArray.GetSize()>0)	return m_recordArray[0]->m_str;
	else return "";

}

CString CLrc::GetNext(UINT time)
{
	int i =0; 
	while((i<m_recordArray.GetSize()) && (time+offset) >= (m_recordArray[i]->m_time))
	{
		i ++;
	}
	
	if(i-1 >=0 && i<m_recordArray.GetSize()) return m_recordArray[i]->m_str;
	return "";
}
//修改歌词进度
void CLrc::Modify(int forward)
{

	CFile fl;
	CString strr;
	int flag=0;
	int len,pos1,pos2;
	char* pstr;
	offset+=forward;

	if(!fl.Open(m_file,CFile::modeRead))
	{
		return;
	}
	len =fl.GetLength();
    pstr =(char*)malloc(len+1);
	fl.Read(pstr,len);
	pstr[len]=0;
	strr=pstr;
    free(pstr);
	fl.Close();
	pos1=strr.Find("[offset:",0);
	if(pos1!=-1)
	{
       pos2=strr.Find("]",pos1+1);
	   if(pos2!=-1)
	   {
		   CString stroffsetnew;
		   char aucoffset[64];
		   memset(aucoffset,0,64);
		   for(int i=0; i<pos2-pos1+1; i++)
		   {
			   aucoffset[i]=strr.GetAt(pos1+i);
		   }
		   stroffsetnew.Format("[offset:%d]",offset*10);
		   strr.Replace(aucoffset,stroffsetnew);
		   flag=1;
	   }
	}
	if(flag==0)
	{
		CString strtmp;
		strtmp.Format("[offset:%d]",offset*10);
		strr=strtmp+strr;
	}
	if(!fl.Open(m_file,CFile::modeCreate|CFile::modeWrite))
	{
		return;
	}
	
	fl.Write(strr.GetBuffer(0),strr.GetLength());
	fl.Close();
	
}

void CLrc::Patch()
{
//	[ti:苏州河里的慕容雪]
//	[ar:薛凯琪]
//	[al:]
//	[by:匿名]
//	[offset:0]  	

}
