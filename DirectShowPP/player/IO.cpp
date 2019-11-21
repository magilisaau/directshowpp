#include "StdAfx.h"
#include "IO.h"

/************************************************************************/
/* CInput                                                               */
/************************************************************************/
CInput::CInput(void)
{
	memset(&m_Position, 0, sizeof(LARGE_INTEGER));
	memset(&m_Size, 0, sizeof(LARGE_INTEGER));
	m_Size.QuadPart = 0;
	m_SampleSize = 0;
	m_Busy = FALSE;
	m_BufferStart = 0;
	m_BufferEnd = 0;
	m_EndOfStream = FALSE;
	m_DataCS = new CMyCriticalSection;
}

CInput::~CInput(void)
{
	if(m_DataCS != NULL)
		delete m_DataCS;
}

DWORD CInput::GetTotalTime()
{
	return 0;
}

LARGE_INTEGER CInput::GetTotalSamples()
{
	LARGE_INTEGER ret;
	memset(&ret, 0, sizeof(LARGE_INTEGER));

	ret.QuadPart = 0;
	return ret;
}

LARGE_INTEGER CInput::GetSize()
{
	return m_Size;
}


LARGE_INTEGER CInput::GetPosition()
{
	LARGE_INTEGER lrt;
	lrt.QuadPart=m_Position.LowPart;
	return lrt;
}

void CInput::Init()
{
	m_DataCS->Enter();
	try
	{
		m_EndOfStream = FALSE;
		InitInternal();
	}
	catch(...)
	{
		m_EndOfStream = TRUE;
	}

	m_DataCS->Leave();
}

void CInput::GetData(void** buffer, DWORD* size)
{
	m_DataCS->Enter();
	try
	{
		GetDataInternal(buffer, size);
	}
	catch (...) {}

	m_DataCS->Leave();
}

DWORD CInput::CopyData(void* buffer, DWORD size)
{
	void* p = NULL;
	GetData(&p, &size);

	if(p != NULL)
		memcpy(buffer, p, size);
	return size;
}

DWORD CInput::FillBuffer(void* buffer, DWORD size, BOOL *eof)
{
	BYTE* pBuffer = (BYTE*)buffer;
	DWORD r = size, ret = 0;
	while( (size - ret) > 0 && r != 0)
	{
		r = CopyData(pBuffer + ret, size - ret);
		ret += r;
	}

	*eof = (r == 0);	
	return ret;
}

void CInput::Flush()
{
	m_DataCS->Enter();
	try
	{
		FlushInternal();
	}
	catch(...) {}

	m_DataCS->Leave();
}

void CInput::Reset()
{
	try
	{
		Flush();
	}
	catch(...) {}

	m_Busy = FALSE;
}

void CInput::Lock()
{
	m_DataCS->Enter();
}

void CInput::Unlock()
{
	m_DataCS->Leave();
}

void CInput::Pause()
{
}

void CInput::Resume()
{
}

/************************************************************************/
/* CStreamedInput                                                       */
/************************************************************************/
CStreamedInput::CStreamedInput(void) : CInput()
{
	m_pStream = NULL;
	m_StreamAssigned = FALSE;
	m_Seekable = TRUE;
	memset(&m_StartSample, 0, sizeof(LARGE_INTEGER));
	m_StartSample.LowPart = 0;

	memset(&m_EndSample, 0, sizeof(LARGE_INTEGER));
	m_EndSample.LowPart = -1;

	memset(&m_TotalSamples, 0, sizeof(LARGE_INTEGER));
	m_TotalSamples.LowPart = 0;
}

CStreamedInput::~CStreamedInput(void)
{
}

void CStreamedInput::SetStream(CStream* pStream)
{
	m_pStream = pStream;
	if(m_pStream != NULL) m_StreamAssigned = TRUE;
	else m_StreamAssigned = FALSE;
}

void CStreamedInput::GetData(void** buf, DWORD* Bytes)
{
	DWORD tmpBytes = *Bytes;

	m_DataCS->Enter();
	try
	{
		if(m_EndOfStream)
		{
			*buf = NULL;
			*Bytes = 0;
		}
		else
		{
			GetDataInternal(buf, Bytes);
			if(Bytes == 0)
				m_EndOfStream = TRUE;
			else
			{
				m_Position.LowPart += *Bytes;
				if(m_Size.LowPart > 0 && m_Position.LowPart >= m_Size.LowPart)
				{
					m_EndOfStream = TRUE;
					if(m_Position.LowPart > m_Size.LowPart)
					{
						*Bytes -= (DWORD)(m_Position.LowPart - m_Size.LowPart);
						m_Position.LowPart = m_Size.LowPart;
					}
				}
			}

			if(m_EndOfStream && m_Loop)
			{
				m_EndOfStream = FALSE;
				if(m_Seekable)
					SeekInternal(&m_StartSample);
				else
				{
					Flush();
					Init();
				}

				m_Position.LowPart = 0;
				//modify by liuxb(magi) ,2012.2.27 22:39
				//*Bytes = tmpBytes;
				//GetDataInternal(buf, Bytes);
				//if(*Bytes == 0)
					m_EndOfStream = TRUE;
				//else
				//{
				//	m_Position.LowPart += *Bytes;
				//	if(m_Size.LowPart > 0 && m_Position.LowPart >= m_Size.LowPart)
				//		m_EndOfStream = TRUE;
				//}
			}
		}

	}
	catch(...) {}

	m_DataCS->Leave();
}

BOOL CStreamedInput::Seek(LARGE_INTEGER* SampleNum)
{
	BOOL ret = FALSE;
	if(!m_Seekable)
		return FALSE;

//	if(m_TotalSamples.LowPart != 0 && SampleNum->LowPart > m_TotalSamples.LowPart)
//		return FALSE;

	m_DataCS->Enter();
	if(!m_Busy)
	{
		m_StartSample.LowPart = SampleNum->LowPart;
		m_Position.LowPart = SampleNum->LowPart * m_SampleSize;
		m_EndSample.LowPart = -1;
		return TRUE;
	}
	else
	{
		try
		{
			SampleNum->LowPart += m_StartSample.LowPart;
			ret = SeekInternal(SampleNum);
			m_Position.LowPart = (SampleNum->LowPart - m_StartSample.LowPart) * m_SampleSize;
		}
		catch(...) {}
	}

	m_DataCS->Leave();
	return ret;
}

/************************************************************************/
/* CFileIn                                                              */
/************************************************************************/
CFileIn::CFileIn(void) : CStreamedInput()
{
	m_OpenCS = new CMyCriticalSection;
	StringCchCopy(m_FileName, 1, _TEXT(""));
	m_FileNameLen = 0;
	m_Opened = 0;
	m_Valid = FALSE;
	m_BitsPerSample = 0;
	m_SampleRate = 0;
	m_Channels = 0;
	m_Time = 0;
}

CFileIn::~CFileIn(void)
{
	delete m_OpenCS;
}

void CFileIn::SetFileName(TCHAR *pszFileName)
{
	CloseFile();
	StringCchLength(pszFileName, STRSAFE_MAX_CCH, &m_FileNameLen);
	StringCchCopy(m_FileName, m_FileNameLen+1, pszFileName);
}

size_t CFileIn::GetFileNameLen()
{
	return m_FileNameLen;
}

WORD CFileIn::GetBitsPerSample()
{
	OpenFile();
	return m_BitsPerSample;
}

DWORD CFileIn::GetSampleRate()
{
	OpenFile();
	return m_SampleRate;
}

WORD CFileIn::GetChannels()
{
	OpenFile();
	return m_Channels;
}

INT CFileIn::GetOpened()
{
	return m_Opened;
}

BOOL CFileIn::GetValid()
{
	if(!m_StreamAssigned && m_FileNameLen == 0)
	{
		; // do nothing
	}
	else
	{
		try
		{
			OpenFile();
		}
		catch(...) { m_Valid = FALSE; }

		if(!m_Valid)
		{
			if(m_Opened == 0)
			{
				if(!m_StreamAssigned && m_pStream != NULL)
					delete m_pStream;
			}
			else
			{
				try
				{
					CloseFile();
				}
				catch(...) {}
			}
		}
	}

	return m_Valid;
}

LARGE_INTEGER CFileIn::GetTotalSamples()
{
	OpenFile();
	if(m_Size.QuadPart > 0)
		m_TotalSamples.LowPart = m_Size.LowPart / (m_Channels * m_BitsPerSample / 8);
	else m_TotalSamples.LowPart = -1;

	return m_TotalSamples;
}

BOOL CFileIn::SetStartTime(DWORD Minutes, DWORD Seconds)
{
	if(!m_Seekable)
		return FALSE;

	OpenFile();
	DWORD Sample = (Minutes * 60 + Seconds) * m_SampleRate;
	if(Sample > m_TotalSamples.LowPart) return FALSE;

	m_StartSample.LowPart = Sample;
	return TRUE;
}

BOOL CFileIn::SetEndTime(DWORD Minutes, DWORD Seconds)
{
	if(!m_Seekable)
		return FALSE;

	OpenFile();
	DWORD Sample = (Minutes * 60 + Seconds) * m_SampleRate;
	if(Sample > m_TotalSamples.LowPart) return FALSE;

	m_StartSample.LowPart = Sample;
	return TRUE;
}

void CFileIn::Reset()
{
	CStreamedInput::Reset();
	m_Opened = 0;
}

void CFileIn::Jump(INT offset)
{
	DOUBLE Curpos = 0;
	LARGE_INTEGER Cursample;
	memset(&Cursample, 0, sizeof(LARGE_INTEGER));

	if(!m_Seekable || m_Size.QuadPart == 0)
		return;

	Curpos = m_Position.LowPart / m_Size.LowPart + offset / 100;
	if (Curpos < 0) Curpos = 0;
	if (Curpos > 1) Curpos = 1;

	Cursample.LowPart = (DWORD)floor(Curpos * m_TotalSamples.LowPart);
	Seek(&Cursample);
}

void CFileIn::SetStream(CStream *pStream)
{
	CloseFile();
	CStreamedInput::SetStream(pStream);
}

void CFileIn::FlushInternal()
{
	CloseFile();
	m_StartSample.LowPart = 0;
	m_EndSample.LowPart = -1;
	m_Busy = FALSE;
}
#if 0
DWORD CFileIn::GetTotalTime()
{
	OpenFile();
	if(m_SampleRate == 0 || m_Channels == 0 || m_BitsPerSample == 0)
		return 0;

	DOUBLE x = m_Size.QuadPart/ (m_SampleRate * m_Channels * (m_BitsPerSample >> 3));
	return (DWORD)floor(x);
}
#endif 

void CFileIn::InitInternal()
{
	if(m_Busy)
	{
		MessageBox(GetActiveWindow(), _TEXT("The component is Busy"), _TEXT("Error"), MB_OK);
		return;
	}

	if(!m_StreamAssigned && m_FileNameLen == 0)
	{
		MessageBox(GetActiveWindow(), _TEXT("The file name is not assigned"), _TEXT("Error"), MB_OK);
		return;
	}

	m_Busy = TRUE;
	m_Position.LowPart = 0;
	OpenFile();

	m_SampleSize = m_Channels * m_BitsPerSample >> 3;
	m_TotalSamples.LowPart = m_Size.LowPart / m_SampleSize;
	m_Time = m_TotalSamples.LowPart / m_SampleRate;

	if(m_StartSample.LowPart > 0)
	{
		LARGE_INTEGER seek;
		memset(&seek, 0, sizeof(LARGE_INTEGER));

		Seek(&seek);
		m_Position.LowPart = 0;
	}

	if(m_StartSample.LowPart > 0 && m_EndSample.LowPart != -1)
	{
		if(m_EndSample.LowPart > m_TotalSamples.LowPart)
			m_EndSample.LowPart = -1;

		if(m_EndSample.LowPart == -1)
			m_TotalSamples.LowPart -= m_StartSample.LowPart + 1;
		else
			m_TotalSamples.LowPart = m_EndSample.LowPart - m_StartSample.LowPart + 1;

		m_Size.LowPart = m_TotalSamples.LowPart * m_SampleSize;
	}

	m_BufferStart = 1;
	m_BufferEnd = 0;
}


/************************************************************************/
/* CWaveIn                                                              */
/************************************************************************/
#if 0
const GUID KSDATAFORMAT_SUBTYPE_PCM = {0x00000001, 0x0000, 0x0010, {0x80, 0x00, 0x00,0xaa, 0x00, 0x38, 0x9b, 0x71} };
const GUID KSDATAFORMAT_SUBTYPE_IEEE_FLOAT = {0x00000003, 0x0000, 0x0010, {0x80, 0x00, 0x00,0xaa, 0x00, 0x38, 0x9b, 0x71} };
#endif
// DVI IMA ADPCM stuff
INT StepTab[89] = {7,     8,     9,    10,    11,    12,    13,    14,
16,    17,    19,    21,    23,    25,    28,    31,
34,    37,    41,    45,    50,    55,    60,    66,
73,    80,    88,    97,   107,   118,   130,   143,
157,   173,   190,   209,   230,   253,   279,   307,
337,   371,   408,   449,   494,   544,   598,   658,
724,   796,   876,   963,  1060,  1166,  1282,  1411,
1552,  1707,  1878,  2066,  2272,  2499,  2749,  3024,
3327,  3660,  4026,  4428,  4871,  5358,  5894,  6484,
7132,  7845,  8630,  9493, 10442, 11487, 12635, 13899,
15289, 16818, 18500, 20350, 22385, 24623, 27086, 29794,
32767 };

INT IndexTab[16] = {-1, -1, -1, -1, 2, 4, 6, 8,
-1, -1, -1, -1, 2, 4, 6, 8 };

// MS ADPCM Stuff
SHORT adaptive[16]= {230, 230, 230, 230, 307, 409, 512, 614,
768, 614, 512, 409, 307, 230, 230, 230 };

CWaveIn::CWaveIn(void) : CFileIn()
{
	memset(m_Buf, 0, BUF_SIZE);
	m_WavType = wtUnsupported;
	memset(&m_DVI_ADPCM_INFO, 0, sizeof(DVI_ADPCM_INFO));
	memset(&m_DVI_ADPCM_STATE, 0, sizeof(DVI_ADPCM_STATE_STEREO));
	memset(&m_MS_ADPCM_INFO, 0, sizeof(MS_ADPCM_INFO));
	memset(&m_MS_ADPCM_STATE, 0, sizeof(MSADPCMBlockHeaderStereo));
	m_HeaderSize = 0;
	m_MS = NULL;
	m_OldStream = NULL;
	m_OldStreamAssigned = FALSE;
	m_ShortIEEEFloat = FALSE;
}

CWaveIn::~CWaveIn(void)
{
}

void CWaveIn::DecodeDVIADPCMMono(BYTE* pInData, SHORT* pOutData, DWORD* len)
{
	int i(0), j(0), SP(0), Diff(0), PSample(0), Index(0);
	BYTE Code(0);

	pOutData[0] = m_DVI_ADPCM_STATE.valprev_l;
	PSample = m_DVI_ADPCM_STATE.valprev_l;
	Index = m_DVI_ADPCM_STATE.index_l;

	for(i = 0; i < (*len << 1); ++i)
	{
		j = i >> 1;
		Code = pInData[j];

		if((i & 1) == 0)
			Code = Code & 15;
		else
			Code = Code >> 4;

		Diff = (StepTab[Index] >> 3 );

		if((Code & 4) != 0)
			Diff += StepTab[Index];

		if((Code & 2) != 0)
			Diff += StepTab[Index] >> 1;

		if((Code & 1) != 0)
			Diff += StepTab[Index] >> 2;

		if((Code & 8) != 0)
			Diff = -Diff;

		PSample += Diff;
		if(PSample > 32767) PSample = 32767;
		if(PSample < -32767) PSample = -32767;

		SP++;
		pOutData[SP] = PSample;
		Index += IndexTab[Code];
		if(Index > 88) Index = 88;
		if(Index < 0) Index = 0;
	}

	*len = SP + 1;
}

void CWaveIn::DecodeDVIADPCMStereo(BYTE* pInData, SHORT* pOutData, DWORD* len)
{
	int i(0), j(0), SP(0), Diff(0), PSample(0), Index(0);
	BYTE Code(0);

	pOutData[0] = m_DVI_ADPCM_STATE.valprev_l;
	PSample = m_DVI_ADPCM_STATE.valprev_l;
	Index = m_DVI_ADPCM_STATE.index_l;

	for(i = 0; i<*len; ++i)
	{
		j = i >> 1;
		Code = pInData[(j / 4) * 8 + (j % 4)];

		if((i & 1) == 0)
			Code = Code & 15;
		else
			Code = Code >> 4;

		Diff = (StepTab[Index] >> 3 );
		if((Code & 4) != 0)
			Diff += StepTab[Index];

		if((Code & 2) != 0)
			Diff += (StepTab[Index] >> 1);

		if((Code & 1) != 0)
			Diff += (StepTab[Index] >> 2);

		if((Code & 8) != 0)
			Diff = -Diff;

		PSample += Diff;
		if(PSample > 32767) PSample = 32767;
		if(PSample < -32767) PSample = -32767;

		SP +=2;
		pOutData[SP] = PSample;
		Index += IndexTab[Code];
		if(Index > 88) Index = 88;
		if(Index < 0) Index = 0;
	}

	i = 1;
	pOutData[i] = m_DVI_ADPCM_STATE.valprev_r;
	SP = 1;
	PSample = m_DVI_ADPCM_STATE.valprev_r;
	Index = m_DVI_ADPCM_STATE.index_r;

	for(i = 0; i<*len; ++i)
	{
		j = i >> 1;
		Code = pInData[(j / 4) * 8 + (j % 4)];

		if((i & 1) == 0)
			Code = Code & 15;
		else
			Code = Code >> 4;

		Diff = (StepTab[Index] >> 3 );
		if((Code & 4) != 0)
			Diff += StepTab[Index];

		if((Code & 2) != 0)
			Diff += (StepTab[Index] >> 1);

		if((Code & 1) != 0)
			Diff += (StepTab[Index] >> 2);

		if((Code & 8) != 0)
			Diff = -Diff;

		PSample += Diff;
		if(PSample > 32767) PSample = 32767;
		if(PSample < -32767) PSample = -32767;

		SP +=2;
		pOutData[SP] = PSample;
		Index += IndexTab[Code];
		if(Index > 88) Index = 88;
		if(Index < 0) Index = 0;
	}

	*len = (SP / 2) + 1;
}

void CWaveIn::DecodeMSADPCMMono(BYTE* pInData, SHORT* pOutData, DWORD* len)
{
	INT pos(0), i(0), PredSamp(0), ErrorDelta(0);
	pOutData[pos] = m_MS_ADPCM_STATE.Samp2[0];
	pos++;
	pOutData[pos] = m_MS_ADPCM_STATE.Samp1[0];
	pos++;

	for(i = 0; i< (*len  >> 1); ++i)
	{
		PredSamp = (m_MS_ADPCM_STATE.Samp1[0]*m_MS_ADPCM_INFO.CoefSets[m_MS_ADPCM_STATE.predictor[0]].Coef1 +
			m_MS_ADPCM_STATE.Samp2[0]*m_MS_ADPCM_INFO.CoefSets[m_MS_ADPCM_STATE.predictor[0]].Coef2) / 256;

		ErrorDelta = pInData[i] >> 4;
		if((ErrorDelta & 8) != 0)
			PredSamp += m_MS_ADPCM_STATE.Delta[0]*(ErrorDelta - 16);
		else
			PredSamp += m_MS_ADPCM_STATE.Delta[0]*ErrorDelta;

		if(PredSamp > 32767) PredSamp = 32767;
		if(PredSamp < -32768) PredSamp = -32768;
		pOutData[pos] = PredSamp;
		pos++;

		m_MS_ADPCM_STATE.Delta[0] = (m_MS_ADPCM_STATE.Delta[0]*adaptive[ErrorDelta]) / 256;
		if (m_MS_ADPCM_STATE.Delta[0] < 16) m_MS_ADPCM_STATE.Delta[0] = 16;

		m_MS_ADPCM_STATE.Samp2[0] = m_MS_ADPCM_STATE.Samp1[0];
		m_MS_ADPCM_STATE.Samp1[0] = PredSamp;

		PredSamp = (m_MS_ADPCM_STATE.Samp1[0]*m_MS_ADPCM_INFO.CoefSets[m_MS_ADPCM_STATE.predictor[0]].Coef1 +
			m_MS_ADPCM_STATE.Samp2[0]*m_MS_ADPCM_INFO.CoefSets[m_MS_ADPCM_STATE.predictor[0]].Coef2) / 256;
		ErrorDelta = pInData[i] & 15;

		if((ErrorDelta & 8) != 0)
			PredSamp += m_MS_ADPCM_STATE.Delta[0]*(ErrorDelta - 16);
		else
			PredSamp += m_MS_ADPCM_STATE.Delta[0]*ErrorDelta;

		if(PredSamp > 32767) PredSamp = 32767;
		if(PredSamp < -32768) PredSamp = -32768;
		pOutData[pos] = PredSamp;
		pos++;

		m_MS_ADPCM_STATE.Delta[0] = (m_MS_ADPCM_STATE.Delta[0]*adaptive[ErrorDelta]) / 256;
		if (m_MS_ADPCM_STATE.Delta[0] < 16) m_MS_ADPCM_STATE.Delta[0] = 16;
		m_MS_ADPCM_STATE.Samp2[0] = m_MS_ADPCM_STATE.Samp1[0];
		m_MS_ADPCM_STATE.Samp1[0] = PredSamp;
	}

	*len = pos*2;
}

void CWaveIn::DecodeMSADPCMStereo(BYTE* pInData, SHORT* pOutData, DWORD* len)
{
	INT pos(0), i(0), PredSamp(0), ErrorDelta(0);

	pOutData[pos] = m_MS_ADPCM_STATE.Samp2[0];
	pos++;
	pOutData[pos] = m_MS_ADPCM_STATE.Samp2[1];
	pos++;
	pOutData[pos] = m_MS_ADPCM_STATE.Samp1[0];
	pos++;
	pOutData[pos] = m_MS_ADPCM_STATE.Samp1[1];
	pos++;

	for(i = 0; i< *len; ++i)
	{
		PredSamp = (m_MS_ADPCM_STATE.Samp1[0]*m_MS_ADPCM_INFO.CoefSets[m_MS_ADPCM_STATE.predictor[0]].Coef1 +
			m_MS_ADPCM_STATE.Samp2[0]*m_MS_ADPCM_INFO.CoefSets[m_MS_ADPCM_STATE.predictor[0]].Coef2) / 256;
		ErrorDelta = pInData[i] >> 4;

		if((ErrorDelta & 8) != 0)
			PredSamp += m_MS_ADPCM_STATE.Delta[0]*(ErrorDelta - 16);
		else
			PredSamp += m_MS_ADPCM_STATE.Delta[0]*(ErrorDelta);

		if(PredSamp > 32767) PredSamp = 32767;
		if(PredSamp < -32768) PredSamp = -32768;
		pOutData[pos] = PredSamp;
		pos++;

		m_MS_ADPCM_STATE.Delta[0] = (m_MS_ADPCM_STATE.Delta[0]*adaptive[ErrorDelta]) / 256;
		if (m_MS_ADPCM_STATE.Delta[0] < 16) m_MS_ADPCM_STATE.Delta[0] = 16;
		m_MS_ADPCM_STATE.Samp2[0] = m_MS_ADPCM_STATE.Samp1[0];
		m_MS_ADPCM_STATE.Samp1[0] = PredSamp;

		PredSamp = (m_MS_ADPCM_STATE.Samp1[1]*m_MS_ADPCM_INFO.CoefSets[m_MS_ADPCM_STATE.predictor[1]].Coef1 +
			m_MS_ADPCM_STATE.Samp2[1]*m_MS_ADPCM_INFO.CoefSets[m_MS_ADPCM_STATE.predictor[1]].Coef2) / 256;
		ErrorDelta = pInData[i] & 15;

		if((ErrorDelta & 8) != 0)
			PredSamp += m_MS_ADPCM_STATE.Delta[1]*(ErrorDelta - 16);
		else
			PredSamp += m_MS_ADPCM_STATE.Delta[1]*(ErrorDelta);

		if(PredSamp > 32767) PredSamp = 32767;
		if(PredSamp < -32768) PredSamp = -32768;
		pOutData[pos] = PredSamp;
		pos++;

		m_MS_ADPCM_STATE.Delta[1] = (m_MS_ADPCM_STATE.Delta[1]*adaptive[ErrorDelta]) / 256;
		if (m_MS_ADPCM_STATE.Delta[1] < 16) m_MS_ADPCM_STATE.Delta[1] = 16;
		m_MS_ADPCM_STATE.Samp2[1] = m_MS_ADPCM_STATE.Samp1[1];
		m_MS_ADPCM_STATE.Samp1[1] = PredSamp;
	}

	*len = pos*2;
}

void CWaveIn::OpenFile()
{
	CWaveConverter* pWaveConverter = NULL;
	DWORD ValidItems = 0;
	UINT Res = 0;

	m_OpenCS->Enter();
	m_Valid = TRUE;
	if(m_Opened == 0)
	{
		m_ShortIEEEFloat = FALSE;
		m_WavType = wtUnsupported;

		if(!m_StreamAssigned)
			m_pStream = new CFileStream(m_FileName, FILE_OPEN);

		ReadRIFFHeader();
		switch(m_WavType)
		{
		case wtUnsupported:
			m_Valid = FALSE;
			break;
		case wtDVIADPCM:
			{
				if(m_BitsPerSample != 4)
					m_Valid = FALSE;

				m_BitsPerSample = 16;
				m_Size.QuadPart = (DWORD)m_DVI_ADPCM_INFO.DataSize * 2 * m_Channels;
			}
			break;
		case wtMSADPCM:
			{
				m_BitsPerSample = 16;
				m_Size.QuadPart = (DWORD)m_MS_ADPCM_INFO.DataSize * 2 * m_Channels;
			}
			break;
		case wtIEEEFloat:
		case wtExtIEEEFloat:
			{
				if(m_BitsPerSample == 32)
					m_ShortIEEEFloat = TRUE;
				else
					m_Size.QuadPart = m_Size.QuadPart / 2;

				m_BitsPerSample = 32;
			}
			break;
		case wtACM:
			{
				pWaveConverter = new CWaveConverter;
				m_pStream->SetPosition(0);
				pWaveConverter->LoadStream(m_pStream);

				pWaveConverter->NewFormat.format.wFormatTag = WAVE_FORMAT_PCM;
				ValidItems = ACM_FORMATSUGGESTF_WFORMATTAG;
				Res = acmFormatSuggest(NULL, &pWaveConverter->CurrentFormat.format,
					&pWaveConverter->NewFormat.format, sizeof(ACMWaveFormat), ValidItems);

				if(Res != 0)
				{
					m_Valid = FALSE;
					delete pWaveConverter;
					pWaveConverter = NULL;
					return;
				}

				if(pWaveConverter->Convert() != 0)
				{
					m_Valid = FALSE;
					delete pWaveConverter;
					pWaveConverter = NULL;
					return;
				}

				m_MS = new CMemoryStream;
				m_OldStream = m_pStream;
				m_OldStreamAssigned = m_StreamAssigned;
				m_pStream = m_MS;
				m_MS->SetPosition(0);
				pWaveConverter->SaveWavToStream(m_MS);
				m_Size.QuadPart = m_MS->GetSize();
				m_MS->Seek(0, FILE_BEGIN);
				ReadRIFFHeader();
				m_WavType = wtACM;

				delete pWaveConverter;
				pWaveConverter = NULL;
			}
			break;
		default:
			break;
		}

		m_Opened++;
	}

	m_OpenCS->Leave();
}

void CWaveIn::CloseFile()
{
	m_OpenCS->Enter();

	if(m_Opened > 0)
	{
		if(!m_StreamAssigned)
		{
			delete m_pStream;
			m_pStream = NULL;
		}
		else if (m_Seekable)
			m_pStream->Seek(0, FILE_BEGIN);

		if(m_WavType == wtACM)
		{
			if(m_MS != NULL)
			{
				if(m_OldStreamAssigned)
					m_pStream = m_OldStream;
				else
				{
					m_pStream = NULL;
					delete m_OldStream;
					m_OldStream = NULL;
				}

				m_MS = NULL;
			}
		}

		m_Opened = 0;
	}

	m_OpenCS->Leave();
}

void CWaveIn::GetDataInternal(void **buffer, DWORD *bytes)
{
	INT l(0), Aligned(0);
	DWORD csize(0);
	void* Data = NULL;

	if(!m_Busy)
	{
		MessageBox(GetActiveWindow(), _TEXT("The Stream is not opened!"), _TEXT("Error"), MB_OK);
		return;
	}

	if(m_BufferStart > m_BufferEnd)
	{
		switch(m_WavType)
		{
		case wtDVIADPCM:
			{
				m_BufferStart = 1;
				csize = m_DVI_ADPCM_INFO.BlockLength - m_Channels * 4;
				Data = malloc(csize);

				if(ReadDVIADPCMBlock(Data))
				{
					if(m_Channels == 2)
					{
						DecodeDVIADPCMStereo((BYTE*)Data, (SHORT*)m_Buf, &csize);
						m_BufferEnd = csize * 4;
					}
					else
					{
						DecodeDVIADPCMMono((BYTE*)Data, (SHORT*)m_Buf, &csize);
						m_BufferEnd = csize * 4;
						free(Data);
						Data = NULL;
					}
				}
				else
				{
					free(Data);
					Data = NULL;

					if(!m_Seekable)
					{
						*bytes = 0;
						*buffer = NULL;
					}
					else m_BufferEnd = 0;
				}
			}

			break;

		case wtMSADPCM:
			{
				m_BufferStart = 1;
				if(m_Channels == 2)
				{
					csize = m_MS_ADPCM_INFO.BlockLength - sizeof(MSADPCMBlockHeaderStereo);
					Data = malloc(csize);

					if(ReadMSADPCMBlock(Data))
					{
						csize = m_MS_ADPCM_INFO.SamplesPerBlock - 2;
						DecodeMSADPCMStereo((BYTE*)Data, (SHORT*)m_Buf, &csize);
						m_BufferEnd = csize;
						free(Data);
						Data = NULL;
					}
					else m_BufferEnd = 0;
				}
				else
				{
					csize = m_MS_ADPCM_INFO.BlockLength - sizeof(MSADPCMBlockHeaderMono);
					Data = malloc(csize);

					if(ReadMSADPCMBlock(Data))
					{
						csize = m_MS_ADPCM_INFO.SamplesPerBlock - 2;
						DecodeMSADPCMMono((BYTE*)Data, (SHORT*)m_Buf, &csize);
						m_BufferEnd = csize;
						free(Data);
						Data = NULL;
					}
					else m_BufferEnd = 0;
				}
			}

			break;
		case wtPCM:
		case wtExtPCM:
			{
				m_BufferStart = 1;
				Aligned = BUF_SIZE - (BUF_SIZE % (m_Channels * m_BitsPerSample >> 3));
				l = m_pStream->Read(m_Buf, Aligned);
				m_BufferEnd = l;
			}
			break;
		case wtExtIEEEFloat:
		case wtIEEEFloat:
			{
				m_BufferStart = 1;
				Aligned = BUF_SIZE - (BUF_SIZE % (m_Channels * 8));
				l = m_pStream->Read(m_Buf, Aligned);
				if(m_ShortIEEEFloat)
				{
					ConvertShortIEEEFloatTo32((INT*)m_Buf, l);
					m_BufferEnd = l;
				}
				else
				{
					ConvertIEEEFloatTo32((INT*)m_Buf, l);
					m_BufferEnd = l / 2;
				}
			}

			break;

		case wtACM:
			{
				m_BufferStart = 1;
				m_pStream->Read(m_Buf, BUF_SIZE);
			}
			break;
		}
	}

	*bytes -= *bytes % (m_Channels * m_BitsPerSample >> 3);
	if(*bytes > (m_BufferEnd - m_BufferStart + 1))
		*bytes = (m_BufferEnd - m_BufferStart + 1);

	*buffer = (void*)(m_Buf + m_BufferStart - 1);
	m_BufferStart += *bytes;
}

BOOL CWaveIn::SeekInternal(LARGE_INTEGER* SampleNum)
{
	INT64 OffsSize(0);

	m_BufferStart = 1;
	m_BufferEnd = 0;
	BOOL ret = TRUE;

	switch(m_WavType)
	{
	case wtPCM:
		OffsSize = SampleNum->QuadPart * (m_BitsPerSample >> 3) * m_Channels;
		m_pStream->Seek((DWORD)(OffsSize + m_HeaderSize), FILE_BEGIN);
		break;
	case wtDVIADPCM:
		OffsSize = (SampleNum->QuadPart / m_DVI_ADPCM_INFO.SamplesPerBlock) * m_DVI_ADPCM_INFO.BlockLength;
		m_pStream->Seek((DWORD)(OffsSize + m_HeaderSize), FILE_BEGIN);
		break;
	case wtMSADPCM:
		OffsSize = (SampleNum->QuadPart / m_MS_ADPCM_INFO.SamplesPerBlock) * m_MS_ADPCM_INFO.BlockLength;
		m_pStream->Seek((DWORD)(OffsSize + m_HeaderSize), FILE_BEGIN);
		break;
	default:
		ret = FALSE;
		break;
	}

	return ret;
}

BOOL CWaveIn::ReadDVIADPCMBlock(void* pData)
{
	BYTE* block = NULL;
	DVIADPCMBlockHeader BH = {0};
	BOOL ret = FALSE;

	if(m_Seekable)
		if(m_pStream->GetPosition() >= m_pStream->GetSize())
			return ret;

	block = (BYTE*)malloc(m_DVI_ADPCM_INFO.BlockLength);
	if(m_pStream->Read(block, m_DVI_ADPCM_INFO.BlockLength) == 0)
	{
		free(block);
		block = NULL;
		return ret;
	}

	ret = TRUE;
	memcpy(&BH, block, sizeof(DVIADPCMBlockHeader));
	m_DVI_ADPCM_STATE.valprev_l = BH.Samp0;
	m_DVI_ADPCM_STATE.index_l = BH.StepTableIndex;

	if(m_Channels == 2)
	{
		memcpy(&BH, block + 4, sizeof(DVIADPCMBlockHeader));
		m_DVI_ADPCM_STATE.valprev_l = BH.Samp0;
		m_DVI_ADPCM_STATE.index_l = BH.StepTableIndex;
		memcpy(pData, block + 8, m_DVI_ADPCM_INFO.BlockLength - 8);
	}
	else memcpy(pData, block + 4, m_DVI_ADPCM_INFO.BlockLength - 4);

	free(block);
	block = NULL;

	return ret;
}

BOOL CWaveIn::ReadMSADPCMBlock(void* pData)
{
	BYTE* block = NULL;
	MSADPCMBlockHeaderMono BHM = {0};
	MSADPCMBlockHeaderStereo BHS = {0};
	BOOL ret = FALSE;

	if(m_pStream->GetPosition() >= m_pStream->GetSize())
		return ret;

	ret = TRUE;
	block = (BYTE*)malloc(m_MS_ADPCM_INFO.BlockLength);
	m_pStream->Read(block, m_MS_ADPCM_INFO.BlockLength);

	if(m_Channels == 1)
	{
		memcpy(&BHM, block, sizeof(MSADPCMBlockHeaderMono));
		m_MS_ADPCM_STATE.predictor[0] = BHM.predictor;
		m_MS_ADPCM_STATE.Delta[0] = BHM.Delta;
		m_MS_ADPCM_STATE.Samp1[0] = BHM.Samp1;
		m_MS_ADPCM_STATE.Samp2[0] = BHM.Samp2;
		memcpy(pData, block + sizeof(MSADPCMBlockHeaderMono),
			m_MS_ADPCM_INFO.BlockLength-sizeof(MSADPCMBlockHeaderMono));
	}
	else
	{
		memcpy(&BHS, block, sizeof(MSADPCMBlockHeaderStereo));
		m_MS_ADPCM_STATE = BHS;
		memcpy(pData, block + sizeof(MSADPCMBlockHeaderStereo),
			m_MS_ADPCM_INFO.BlockLength - sizeof(MSADPCMBlockHeaderStereo));
	}

	return ret;
}

void CWaveIn::ReadRIFFHeader()
{
	const int buffer_size = 4096;
	INT i(0);
	WORD WordVal(0);
	INT IntVal(0);
	char Buff[buffer_size];
	INT State(0);
	INT ChunkSize(0);
	GUID SubType;

	memset(Buff, 0, sizeof(char) * buffer_size);
	m_Size.QuadPart = 0;
	m_BitsPerSample = 0;
	m_Channels = 0;
	m_SampleRate = 0;
	m_WavType = wtUnsupported;
	State = LookingForRIFF;
	i = 4;

	m_pStream->Read(Buff, 4);
	while(i < 8192)
	{
		switch(State)
		{
		case LookingForRIFF:
			{
				if(!Compare4(Buff + i - 4, "RIFF"))
				{
					m_pStream->Read(Buff + i, 1);
					i++;
				}
				else
				{
					m_pStream->Read(Buff + i, 4);
					i += 4;
					State = LookingForWave;
				}
			}
			break;
		case LookingForWave:
			{
				if(!Compare4(Buff + i - 4, "WAVE"))
				{
					m_pStream->Read(Buff + i, 1);
					i++;
				}
				else
				{
					m_pStream->Read(Buff + i, 4);
					i += 4;
					State = LookingForFMT;
				}
			}
			break;

		case LookingForFMT:
			{
				if(!Compare4(Buff + i - 4, "fmt "))
				{
					m_pStream->Read(Buff + i, 4);
					i += 4;
					memcpy(&ChunkSize, Buff + i - 4, 4);

					m_pStream->Read(Buff + i, ChunkSize);
					i += ChunkSize;

					m_pStream->Read(Buff + i, 4);
					i += 4;
				}
				else
				{
					m_pStream->Read(Buff + i, 4);
					i += 4;
					memcpy(&ChunkSize, Buff + i - 4, 4);

					m_pStream->Read(Buff + i, ChunkSize);
					i += ChunkSize;
					memcpy(&WordVal, Buff + i - ChunkSize, 2);

					switch(WordVal)
					{
					case WAVE_FORMAT_PCM:
						m_WavType = wtPCM;
						break;
					case WAVE_FORMAT_IMA_ADPCM:
						m_WavType = wtDVIADPCM;
						break;
					case WAVE_FORMAT_ADPCM:
						m_WavType = wtMSADPCM;
						break;
					case WAVE_FORMAT_MP3:
						m_WavType = wtACM;
						break;
					case WAVE_FORMAT_IEEE_FLOAT:
						m_WavType = wtIEEEFloat;
						break;
					case WAVE_FORMAT_EXTENSIBLE:
						m_WavType = wtExtPCM;
						break;
					default:
						return;
					}

					memcpy(&WordVal, Buff + i + 2 - ChunkSize, 2);
					m_Channels = WordVal;

					memcpy(&IntVal, Buff + i + 4 - ChunkSize, 4);
					m_SampleRate = IntVal;

					memcpy(&WordVal, Buff + i + 12 - ChunkSize, 2);
					if(m_WavType == wtDVIADPCM)
						m_DVI_ADPCM_INFO.BlockLength = WordVal;
					else
						m_MS_ADPCM_INFO.BlockLength = WordVal;

					memcpy(&WordVal, Buff + i + 14 - ChunkSize, 2);
					m_BitsPerSample = WordVal;

					if(m_WavType == wtExtPCM)
					{
						memcpy(&SubType, Buff + i - 16, 16);
						if(IsEqualGUID(SubType, KSDATAFORMAT_SUBTYPE_IEEE_FLOAT))
							m_WavType = wtExtIEEEFloat;
						else if(!IsEqualGUID(SubType, KSDATAFORMAT_SUBTYPE_PCM))
							m_WavType = wtUnsupported;
					}

					if(m_WavType == wtDVIADPCM || m_WavType == wtMSADPCM || m_WavType == wtACM)
					{
						memcpy(&WordVal, Buff + i + 18 - ChunkSize, 2);
						if(m_WavType == wtDVIADPCM)
							m_DVI_ADPCM_INFO.SamplesPerBlock = WordVal;
						else
							m_MS_ADPCM_INFO.SamplesPerBlock = WordVal;

						if(m_WavType == wtMSADPCM)
						{
							memcpy(&WordVal, Buff + i + 20 - ChunkSize, 2);
							m_MS_ADPCM_INFO.NumCoeff = WordVal;
							memcpy(&m_MS_ADPCM_INFO.CoefSets[0], Buff + i + 22 - ChunkSize,
								m_MS_ADPCM_INFO.NumCoeff * sizeof(MS_ADPCM_COEF_SET));
						}

						State = LookingForFACT;
					}
					else State = LookingForDATA;

					m_pStream->Read(Buff + i, 4);
					i += 4;
				}
			}

			break;

		case LookingForFACT:
			{
				if(!Compare4(Buff + i - 4, "fact"))
				{
					m_pStream->Read(Buff + i, 4);
					i += 4;
					memcpy(&ChunkSize, Buff + i - 4, 4);

					m_pStream->Read(Buff + i, ChunkSize);
					i += ChunkSize;

					m_pStream->Read(Buff + i, 4);
					i += 4;
				}
				else
				{
					m_pStream->Read(Buff + i, 4);
					i += 4;
					memcpy(&ChunkSize, Buff + i - 4, 4);

					m_pStream->Read(Buff + i, ChunkSize);
					i += ChunkSize;

					memcpy(&IntVal, Buff + i - ChunkSize, 4);
					if(m_WavType == wtDVIADPCM)
						m_DVI_ADPCM_INFO.DataSize = IntVal;
					else
						m_MS_ADPCM_INFO.DataSize = IntVal;

					m_pStream->Read(Buff + i, 4);
					i += 4;
					State = LookingForDATA;
				}
			}

			break;

		case LookingForDATA:
			{
				if(!Compare4(Buff + i - 4, "data"))
				{
					m_pStream->Read(Buff + i, 4);
					i += 4;
					memcpy(&ChunkSize, Buff + i - 4, 4);

					m_pStream->Read(Buff + i, ChunkSize);
					i += ChunkSize;

					m_pStream->Read(Buff + i, 4);
					i += 4;
				}
				else
				{
					m_pStream->Read(Buff + i, 4);

					if(m_WavType == wtPCM || m_WavType == wtExtPCM || m_WavType == wtIEEEFloat
						|| m_WavType == wtExtIEEEFloat)
						memcpy(&m_Size.QuadPart, Buff + i, 4);

					i += 4;
					m_HeaderSize = i;
					return;
				}
			}

			break;
		default:
			break;
		}

		if(m_Seekable)
			if(m_pStream->GetPosition() >= m_pStream->GetSize())
				break;
	}

	m_WavType = wtUnsupported;
}

BOOL Compare4(CHAR* S1, CHAR* S2)
{
	BYTE i(0), Diff(0);

	for(i = 0; i <= 3; ++i)
	{
		Diff = (BYTE)S1[i] - (BYTE)S2[i];
		if( ! (Diff == 0 || Diff == 32 || Diff == 224))
			return FALSE;
	}

	return TRUE;
}

void ConvertIEEEFloatTo32(INT* InOutBuf, INT InSize)
{
	DOUBLE* p = (DOUBLE*)InOutBuf;
	for(int i = 0; i < InSize / 8; ++i)
	{
		if(p[i*2] >= 1) InOutBuf[i] = 2147483647;
		else if(p[i*2] <= -1) InOutBuf[i] = -2147483647;
		else if(p[i*2] == 0) InOutBuf[i] = 0;
		else InOutBuf[i] = (INT)floor(p[i*2] * 2147483647);
	}
}

void ConvertShortIEEEFloatTo32(INT* InOutBuf, INT InSize)
{
	FLOAT* p = (FLOAT*)InOutBuf;
	for(int i = 0; i < InSize / 4; ++i)
	{
		if(p[i] >= 1) InOutBuf[i] = 2147483647;
		else if(p[i] <= -1) InOutBuf[i] = -2147483647;
		else if(p[i] == 0) InOutBuf[i] = 0;
		else InOutBuf[i] = (INT)floor(p[i] * 2147483647);
	}
}

/************************************************************************/
/* CMP3In                                                               */
/************************************************************************/
CMP3In::CMP3In(void) : CFileIn()
{
	memset(m_buffer, 0, sizeof(m_buffer));
	memset(m_pcmbuffer, 0, sizeof(m_pcmbuffer));

	m_xing.flags = 0;
	m_samplecount = 0;
	m_timer = mad_timer_zero;
	m_length = mad_timer_zero;
	m_rate = 0;
	m_frames = 0;
	m_buflen = 0;
}

CMP3In::~CMP3In(void)
{
}

void CMP3In::GetDataInternal(void** buffer, DWORD* bytes)
{
	unsigned char* samples = m_pcmbuffer;
	unsigned int nsamples;

	nsamples = (BUF_SIZE / (SAMPLE_DEPTH / 8)) >> (m_Channels == 2 ? 1 : 0);
	*bytes = 0;

	while (nsamples)
	{
		unsigned int count, bitrate;
		count = m_synth.pcm.length - m_samplecount;

		if (count > nsamples)
			count = nsamples;

		if (count)
		{
			mad_fixed_t const *ch1, *ch2;
			ch1 = m_synth.pcm.samples[0] + m_samplecount;
			ch2 = m_synth.pcm.samples[1] + m_samplecount;

			if (m_Channels != 2)
				ch2 = 0;
			else if (m_synth.pcm.channels == 1)
				ch2 = ch1;

			pack_pcm(&samples, count, ch1, ch2);

			m_samplecount += count;
			nsamples -= count;

			if (nsamples == 0)
				break;
		}

		while (mad_frame_decode(&m_frame, &m_stream) == -1)
		{
			// DWORD bytes;
			unsigned int bytes;
			if (MAD_RECOVERABLE(m_stream.error))
				continue;

			if (m_stream.next_frame)
			{
				memmove(m_buffer, m_stream.next_frame, m_buflen = m_buffer + m_buflen - m_stream.next_frame);
			}

			if ((bytes = m_pStream->Read(m_buffer + m_buflen, sizeof(m_buffer) - m_buflen)) == 0)
				return;

			mad_stream_buffer(&m_stream, m_buffer, m_buflen += bytes);
		}

		bitrate = m_frame.header.bitrate / 1000;
		m_rate += bitrate;
		m_frames++;

		mad_synth_frame(&m_synth, &m_frame);
		m_samplecount = 0;
		mad_timer_add(&m_timer, m_frame.header.duration);
	}

	*bytes = samples - m_pcmbuffer;
	*buffer = m_pcmbuffer;
}

BOOL CMP3In::SeekInternal(LARGE_INTEGER* SampleNum)
{
	if(SampleNum->QuadPart > m_size)
		return FALSE;

	double fraction = SampleNum->QuadPart*1.0 / m_size;
	unsigned long position = (unsigned long)
		(mad_timer_count(m_length, MAD_UNITS_MILLISECONDS) * fraction);
	mad_timer_set(&m_timer, position / 1000, position % 1000, 1000);

	if (m_xing.flags & XING_TOC)
	{
		int percent(0), p1(0), p2(0);
		percent = (int)(fraction * 100);
		p1 = (percent < 100) ? m_xing.toc[percent] : 0x100;
		p2 = (percent < 99) ? m_xing.toc[percent+1] : 0x100;

		fraction = (p1 + (p2 - p1) * (fraction * 100 - percent)) / 0x100;
	}

	m_pStream->Seek((LONG)(m_size * fraction), FILE_BEGIN);

	if ((m_buflen = m_pStream->Read(m_buffer, sizeof(m_buffer))) == 0)
		m_buflen = 0;

	mad_stream_buffer(&m_stream, m_buffer, m_buflen);
	mad_frame_mute(&m_frame);
	mad_synth_mute(&m_synth);

	if (SampleNum->QuadPart)
	{
		int skip = 2;
		do
		{
			if (mad_frame_decode(&m_frame, &m_stream) == 0)
			{
				mad_timer_add(&m_timer, m_frame.header.duration);

				if (--skip == 0)
					mad_synth_frame(&m_synth, &m_frame);
			}
			else if (!MAD_RECOVERABLE(m_stream.error))
				break;

		} while (skip);
	}

	m_synth.pcm.length = 0;
	m_samplecount = 0;
	return TRUE;
}

void CMP3In::OpenFile()
{
	int iStreamStart = 0;

	m_OpenCS->Enter();
	m_Valid = TRUE;

	if(m_Opened == 0)
	{
		if(!m_StreamAssigned)
			m_pStream = new CFileStream(m_FileName, FILE_OPEN);

		m_size = m_pStream->GetSize();
		m_streamsize = m_size;

		// Skip over ID3v2 tag (if there is one)
		// only works on seekable streams
		ID3v2Tag tag;
		memset(&tag, 0, sizeof(ID3v2Tag));

		m_pStream->Read(&tag, sizeof(ID3v2Tag));
		if (memcmp(tag.m_cTAG, "ID3", 3) == 0)
		{
			iStreamStart = sizeof(ID3v2Tag);
			iStreamStart += (tag.m_cSize_Encoded[0] << 21) | (tag.m_cSize_Encoded[1] << 14)
				| (tag.m_cSize_Encoded[2] << 7) | tag.m_cSize_Encoded[3];

			m_streamsize -= iStreamStart;
			m_pStream->Seek(iStreamStart, FILE_BEGIN);
		}

		// If ID3v1 tag exists here
		DWORD dwPos = m_pStream->Seek(0, FILE_CURRENT);
		m_pStream->Seek(0, FILE_END);
		m_pStream->Seek(-128, FILE_CURRENT);

		ID3Tag tag1;
		memset(&tag1, 0, sizeof(ID3Tag));

		m_pStream->Read(&tag1, sizeof(ID3Tag));
		if(memcmp(tag1.m_cTAG, "TAG", 3) == 0)
			m_streamsize -= 128;

		mad_stream_init(&m_stream);
		mad_frame_init(&m_frame);
		mad_synth_init(&m_synth);

		m_pStream->Seek(iStreamStart, FILE_BEGIN);
		scan_header(m_pStream, &m_frame.header, &m_xing);
		m_vbr = m_xing.flags ? TRUE : FALSE;
		m_pStream->Seek(iStreamStart, FILE_BEGIN);

		m_synth.pcm.length = 0;
		m_samplecount = 0;
		m_timer = mad_timer_zero;

		if (m_xing.flags & XING_FRAMES)
		{
			m_length = m_frame.header.duration;
			mad_timer_multiply(&m_length, m_xing.frames);
		}
		else
		{
			/* estimate playing time from file size */
			mad_timer_set(&m_length, 0, 1, m_frame.header.bitrate / 8);
			mad_timer_multiply(&m_length, m_size);
		}

		m_rate = 0;
		m_frames = 0;
		m_buflen = 0;

		m_SampleRate = m_frame.header.samplerate;
		m_Channels = (m_frame.header.mode == MAD_MODE_SINGLE_CHANNEL) ? 1: 2;
		m_BitsPerSample = SAMPLE_DEPTH;

		m_Opened++;
	}
	m_OpenCS->Leave();
}

void CMP3In::CloseFile()
{
	m_OpenCS->Enter();

	if(m_Opened > 0)
	{
		if(!m_StreamAssigned)
		{
			delete m_pStream;
			m_pStream = NULL;
		}
		else if (m_Seekable)
			m_pStream->Seek(0, FILE_BEGIN);

		mad_synth_finish(&m_synth);
		mad_frame_finish(&m_frame);
		mad_stream_finish(&m_stream);		

		m_Opened = 0;
	}

	m_OpenCS->Leave();
}

int parse_xing(struct xing *xing, struct mad_bitptr ptr, unsigned int bitlen)
{
	if (bitlen < 64 || mad_bit_read(&ptr, 32) != XING_MAGIC)
		goto fail;

	xing->flags = mad_bit_read(&ptr, 32);
	bitlen -= 64;

	if (xing->flags & XING_FRAMES)
	{
		if (bitlen < 32)
			goto fail;

		xing->frames = mad_bit_read(&ptr, 32);
		bitlen -= 32;
	}

	if (xing->flags & XING_BYTES)
	{
		if (bitlen < 32)
			goto fail;

		xing->bytes = mad_bit_read(&ptr, 32);
		bitlen -= 32;
	}

	if (xing->flags & XING_TOC)
	{
		int i;

		if (bitlen < 800)
			goto fail;

		for (i = 0; i < 100; ++i)
			xing->toc[i] = (unsigned char) mad_bit_read(&ptr, 8);

		bitlen -= 800;
	}

	if (xing->flags & XING_SCALE)
	{
		if (bitlen < 32)
			goto fail;

		xing->scale = mad_bit_read(&ptr, 32);
		bitlen -= 32;
	}

	return 0;

fail:
	xing->flags = 0;
	return -1;
}

/*
* NAME:  scan_header()
* DESCRIPTION: read the initial frame(s) to get stream statistics
*/
int scan_header(CStream* pInStream, struct mad_header *header, struct xing *xing)
{
	struct mad_stream stream;
	struct mad_frame frame;
	unsigned char buffer[8192];
	unsigned int buflen = 0;
	int count = 0, result = 0;

	mad_stream_init(&stream);
	mad_frame_init(&frame);

	if (xing)
		xing->flags = 0;

	while (1)
	{
		if (buflen < sizeof(buffer))
		{
			unsigned int bytes;
			if ((bytes = pInStream->Read(buffer + buflen, sizeof(buffer) - buflen)) == 0)
			{
				result = -1;
				break;
			}

			buflen += bytes;
		}

		mad_stream_buffer(&stream, buffer, buflen);

		while (1)
		{
			if (mad_frame_decode(&frame, &stream) == -1)
			{
				if (!MAD_RECOVERABLE(stream.error))
					break;

				continue;
			}

			if (count++ || (xing && parse_xing(xing, stream.anc_ptr, stream.anc_bitlen) == 0))
				break;
		}

		if (count || stream.error != MAD_ERROR_BUFLEN)
			break;

		memmove(buffer, stream.next_frame,
			buflen = &buffer[buflen] - stream.next_frame);
	}

	if (count)
	{
		if (header)
			*header = frame.header;
	}

	else
		result = -1;

	mad_frame_finish(&frame);
	mad_stream_finish(&stream);

	return result;
}


/*
* NAME:  prng()
* DESCRIPTION: 32-bit pseudo-random number generator
*/
__inline unsigned long prng(unsigned long state)
{
	return (state * 0x0019660dL + 0x3c6ef35fL) & 0xffffffffL;
}

/*
* NAME:  dither()
* DESCRIPTION: dither and scale sample
*/
__inline signed int dither(mad_fixed_t sample, struct dither *dither)
{
	unsigned int scalebits;
	mad_fixed_t output, mask, random;

	enum
	{
		MIN = -MAD_F_ONE,
		MAX =  MAD_F_ONE - 1
	};

	/* noise shape */
	sample += dither->error[0] - dither->error[1] + dither->error[2];
	dither->error[2] = dither->error[1];
	dither->error[1] = dither->error[0] / 2;

	/* bias */
	output = sample + (1L << (MAD_F_FRACBITS + 1 - SAMPLE_DEPTH - 1));
	scalebits = MAD_F_FRACBITS + 1 - SAMPLE_DEPTH;
	mask = (1L << scalebits) - 1;

	/* dither */
	random  = prng(dither->random);
	output += (random & mask) - (dither->random & mask);
	dither->random = random;

	/* clip */

	if (output > MAX)
	{
		output = MAX;
		if (sample > MAX)
			sample = MAX;
	}

	else if (output < MIN)
	{
		output = MIN;
		if (sample < MIN)
			sample = MIN;
	}

	/* quantize */
	output &= ~mask;

	/* error feedback */
	dither->error[0] = sample - output;

	/* scale */
	return output >> scalebits;
}

/*
* NAME:  pack_pcm()
* DESCRIPTION: scale and dither MAD output
*/
void pack_pcm(unsigned char **pcm, unsigned int nsamples,
			  mad_fixed_t const *ch1, mad_fixed_t const *ch2)
{
	register signed int s0, s1;
	static struct dither d0, d1;

	if (ch2)    /* stereo */
	{
		while (nsamples--)
		{
			s0 = scale(*ch1++, &d0);
			s1 = scale(*ch2++, &d1);
# if SAMPLE_DEPTH == 16
			(*pcm)[0 + 0] = s0 >> 0;
			(*pcm)[0 + 1] = s0 >> 8;
			(*pcm)[2 + 0] = s1 >> 0;
			(*pcm)[2 + 1] = s1 >> 8;
			*pcm += 2 * 2;
# elif SAMPLE_DEPTH == 8
			(*pcm)[0] = s0 ^ 0x80;
			(*pcm)[1] = s1 ^ 0x80;

			*pcm += 2;
# else
#  error "bad SAMPLE_DEPTH"
# endif
		}
	}

	else    /* mono */
	{
		while (nsamples--)
		{
			s0 = scale(*ch1++, &d0);

# if SAMPLE_DEPTH == 16
			(*pcm)[0] = s0 >> 0;
			(*pcm)[1] = s0 >> 8;
			*pcm += 2;
# elif SAMPLE_DEPTH == 8
			*(*pcm)++ = s0 ^ 0x80;
# endif
		}
	}
}

/************************************************************************/
/* CVorbisIn                                                            */
/************************************************************************/
CVorbisIn* gVorbisIn;
CVorbisIn::CVorbisIn(void) : CFileIn()
{
	m_CurrentSection = 0;
	memset(m_pcmbuffer, 0, BUF_SIZE);
}

CVorbisIn::~CVorbisIn(void)
{

}

void CVorbisIn::GetDataInternal(void** buffer, DWORD* bytes)
{
	int ret;
	BOOL CONTINUE = TRUE;
	DWORD bytes_read = 0;

	while (bytes_read < BUF_SIZE)
	{
		ret = ov_read(&m_OggVorbisFile, (char *)m_pcmbuffer + bytes_read,
			BUF_SIZE - bytes_read, 0, 2, 1, &m_CurrentSection);

		if (ret == 0)
		{
			*bytes = bytes_read;
			CONTINUE = FALSE;
			break;
		}
		else if (ret < 0)
		{
			OutputDebugString(_TEXT("Hole in OGG/VORBIS datastream, ignoring..."));
		}

		else
		{
			bytes_read += ret;
		}
	}

	*buffer = m_pcmbuffer;
	*bytes = bytes_read;
}

BOOL CVorbisIn::SeekInternal(LARGE_INTEGER* SampleNum)
{
	ov_pcm_seek(&m_OggVorbisFile, (ogg_int64_t)(((float)SampleNum->QuadPart /
		(float)m_Size.QuadPart) * (float)ov_pcm_total(&m_OggVorbisFile, -1)));
	return TRUE;
}

void CVorbisIn::OpenFile()
{
	m_OpenCS->Enter();
	m_Valid = TRUE;

	if(m_Opened == 0)
	{
		if(!m_StreamAssigned)
			m_pStream = new CFileStream(m_FileName, FILE_OPEN);

		gVorbisIn = this;
		m_Size.QuadPart = m_pStream->GetSize();
		memset(&m_OggVorbisFile, 0, sizeof(OggVorbis_File));

		ov_callbacks callbacks = {
			(size_t (*)(void *, size_t, size_t, void *)) vorbisRead,
			(int(*)(void *, ogg_int64_t, int)) vorbisSeek,
			(int(*)(void *)) vorbisClose,
			(long(*)(void *)) vorbisTell
		};

		if (ov_open_callbacks((void *)(this), &m_OggVorbisFile, NULL, 0, callbacks) < 0)
		{
			MessageBox(GetActiveWindow(), _TEXT("Input does not appear to be an Ogg bitstream."),
				_TEXT("Error"), MB_OK);

			CloseFile();
			return;
		}

		if (ov_streams(&m_OggVorbisFile) != 1)
		{
			TCHAR BUFFER[512];
			StringCbPrintf(BUFFER, 512, _TEXT("Can\'t deal with multiple streams yet. Streams:%d"),
				ov_streams(&m_OggVorbisFile));
			MessageBox(GetActiveWindow(), BUFFER, _TEXT("Error"), MB_OK);
	
			ov_clear(&m_OggVorbisFile);
			CloseFile();
			return;
		}

		vorbis_info *pInfo = NULL;
		pInfo = ov_info(&m_OggVorbisFile, -1);

		if (pInfo == NULL)
		{
			MessageBox(GetActiveWindow(), _TEXT("Unable to get ogg info."), _TEXT("Error"), MB_OK);

			ov_clear(&m_OggVorbisFile);
			CloseFile();
			return;
		}

		if (pInfo->channels > 2)
		{
			TCHAR BUFFER[512];
			StringCbPrintf(BUFFER, 512, _TEXT("Can\'t deal with more than 2 channels yet. Channels:%d"),
				pInfo->channels);
			MessageBox(GetActiveWindow(), BUFFER, _TEXT("Error"), MB_OK);

			ov_clear(&m_OggVorbisFile);
			CloseFile();
			return;
		}

		m_SampleRate = pInfo->rate;
		m_Channels = pInfo->channels;
		m_BitsPerSample = 16;

		m_Opened++;
	}

	m_OpenCS->Leave();
}

void CVorbisIn::CloseFile()
{
	m_OpenCS->Enter();

	if(m_Opened > 0)
	{
		if(!m_StreamAssigned)
		{
			delete m_pStream;
			m_pStream = NULL;
		}
		else if (m_Seekable)
			m_pStream->Seek(0, FILE_BEGIN);

		ov_clear(&m_OggVorbisFile);
		m_Opened = 0;
	}

	m_OpenCS->Leave();
}

size_t vorbisRead(void *buffer, size_t size, size_t amount, void *file)
{
	size_t result = 0;
	result = gVorbisIn->GetStream()->Read(buffer, size*amount);
	return result;
}

int vorbisSeek(void *file, ogg_int64_t newpos, int set)
{
	if (set == SEEK_END)
		newpos = gVorbisIn->GetStream()->GetSize();

	gVorbisIn->GetStream()->Seek((UINT)newpos, FILE_BEGIN);
	return (int)newpos;
}

int vorbisClose(void *file)
{
	return 1;
}

long vorbisTell(void *file)
{
	return gVorbisIn->GetStream()->Seek(0, FILE_CURRENT);
}

DWORD CMP3In::GetTotalTime(void)
{
	return m_length.seconds*100+m_length.fraction*100/MAD_TIMER_RESOLUTION;
}
DWORD CMP3In::GetCurrentTime(void)
{
	return DWORD(m_timer.seconds*100+m_timer.fraction*100/MAD_TIMER_RESOLUTION);
}




DWORD CVorbisIn::GetTotalTime(void)
{
	return 0;
}

DWORD CWaveIn::GetTotalTime(void)
{
	return 0;
}



DWORD CWaveIn::GetCurrentTime(void)
{
	return 0;
}

DWORD CVorbisIn::GetCurrentTime(void)
{
	return 0;
}

LARGE_INTEGER CMP3In::GetSize(void)
{
	LARGE_INTEGER lrt;
	lrt.QuadPart =m_streamsize;
	return lrt;
}



