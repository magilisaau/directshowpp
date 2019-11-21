#include "stdafx.h"
#include "BasicPlayer.h"
#define DEBUG_OUTPUT_BUFFER_SIZE 256
TCHAR BUFFER[DEBUG_OUTPUT_BUFFER_SIZE];
INT DEFAULT_WIDTH=600;
INT DEFAULT_HEIGHT=180;
INT DEFAULT_LEFT=0;
INT DEFAULT_TOP=0;
/************************************************************************/
/* CSystem                                                              */
/************************************************************************/
inline void set_low(jlong* value, jint low)
{
	*value &= (jlong)0xffffffff << 32;
	*value |= (jlong)(julong)(juint)low;
}

inline void set_high(jlong* value, jint high)
{
	*value &= (jlong)(julong)(juint)0xffffffff;
	*value |= (jlong)high << 32;
}

jlong as_long(LARGE_INTEGER x) {
	jlong result = 0; // initialization to avoid warning
	set_high(&result, x.HighPart);
	set_low(&result,  x.LowPart);
	return result;
}

LARGE_INTEGER liFrequency = {0};
BOOL gSupportPerformanceFrequency = QueryPerformanceFrequency(&liFrequency);
jlong CSystem::frequency = as_long(liFrequency);
int CSystem::ready = 1;

/************************************************************************/
/* CPlayThread                                                          */
/************************************************************************/
CPlayThread::CPlayThread(CBasicPlayer* pPlayer) : CThread(TRUE)
{
	m_Player = pPlayer;
	m_cs = new CMyCriticalSection;
}

CPlayThread::~CPlayThread(void)
{
	if(m_cs != NULL)
		delete m_cs;

	m_cs = NULL;
}
//此处区分自动结束还是被动结束,如果是被动结束,则不发送消息,2010.10.2,lxb
void CPlayThread::Execute()
{
	if(m_Player == NULL)
		return;

	if(m_Stop == TRUE)
		return;

	const DWORD buffersize = 16000;

	// wait time = 1/4 of buffer time
	DWORD waitTime = (DWORD)((m_Player->m_BufferSize*1000.0F)/(m_Player->m_SampleRate*m_Player->m_FrameSize));
	waitTime = (DWORD)(waitTime / 4);
	if(waitTime<10) waitTime = 1;
	if(waitTime>1000) waitTime = 1000;


	CSpectrumAnalyser* pSpectrum = m_Player->m_SpectrumAnalyser;
	CFileIn* pInput = m_Player->m_Input;
	pInput->Init();


	BOOL EndOfInput = FALSE;	
	if(DAUDIO_Start((void*)m_Player->m_info, TRUE))
	{
		printf("start play ...\n");
		char buffer[buffersize];
		DWORD len = 0;
		DWORD offset = 0;
		DWORD written = 0;
		while(!m_IsStoping/*!m_Stop*/)
		{
			memset(buffer, 0, buffersize);
			pInput->FillBuffer((void*)buffer, buffersize, &EndOfInput);

			len = buffersize;
			offset = 0;
			written = 0;

			/*
			* in this loop, the data may can not be written to device one time,
			* maybe more than one times. so, we need this loop to process it.
			*/
			while(!m_IsStoping)
			{
				m_cs->Enter();
				int thisWritten = DAUDIO_Write((void*)m_Player->m_info, buffer+offset, len);
				if(thisWritten < 0) break;
				m_Player->m_bytePosition += thisWritten;
				m_cs->Leave();

				len -= thisWritten;
				written += thisWritten;
				if(len > 0)
				{
					offset += thisWritten;
					m_cs->Enter();
					Sleep(waitTime);
					m_cs->Leave();
				}
				else break;
			}

			//copy audio data to audio buffer
			//for audio data synchronize
			DWORD pLength = buffersize;
			jbyte* pAudioDataBuffer = pSpectrum->GetAudioDataBuffer();
			if(pAudioDataBuffer != NULL)
			{
				int wOverrun = 0;
				int iPosition = pSpectrum->GetPosition();
				DWORD dwAudioDataBufferLength = pSpectrum->GetAudioDataBufferLength();
				if (iPosition + pLength > (int)(dwAudioDataBufferLength - 1)) {
					wOverrun = (iPosition + pLength) - dwAudioDataBufferLength;
					pLength = dwAudioDataBufferLength - iPosition;
				}

				memcpy(pAudioDataBuffer + iPosition, buffer, pLength);
				if (wOverrun > 0) {
					memcpy(pAudioDataBuffer, buffer + pLength, wOverrun);
					pSpectrum->SetPosition(wOverrun);
				} else {
					pSpectrum->SetPosition(iPosition + pLength);
				}
			}
			if(EndOfInput)
			{
				break;
			}
		}
		m_Player->m_SpectrumAnalyser->Stop();
		DAUDIO_Stop((void*)m_Player->m_info, TRUE);
		DAUDIO_Close((void*)m_Player->m_info, TRUE);
		m_Player->m_bytePosition = 0;
		printf("stop play.\n");
	}
	m_Player->m_info = NULL;
	m_Stop=TRUE;
	m_IsStoping=FALSE;
	if(EndOfInput)
	{
		::SendMessage(m_Player->m_hWnd,MM_MCINOTIFY,0,0);
	}
}

/************************************************************************/
/* CSpectrumAnalyserThread                                              */
/************************************************************************/
CSpectrumAnalyserThread::CSpectrumAnalyserThread(CSpectrumAnalyser* pSpectrumAnalyser) : CThread(TRUE)
{
	m_Player = pSpectrumAnalyser->m_Player;
	m_SpectrumAnalyser = pSpectrumAnalyser;
	m_cs = new CMyCriticalSection;
	m_process = TRUE;
	m_lfp = 0;
	m_frameSize = DEFAULT_FRAME_SIZE;
}

CSpectrumAnalyserThread::~CSpectrumAnalyserThread(void)
{
	if(m_cs != NULL)
		delete m_cs;

	m_cs = NULL;
}

int CSpectrumAnalyserThread::calculateSamplePosition()
{
	jlong wFp = m_Player->GetLongFramePosition();
	jlong wNfp = m_lfp;
	m_lfp = wFp;
	int wSdp = (int)((jlong)(wNfp * m_frameSize) - (jlong)(m_SpectrumAnalyser->m_AudioDataBufferLength * m_SpectrumAnalyser->m_offset));
	return wSdp;
}

void CSpectrumAnalyserThread::processSamples(int nPosition)
{
	int c = nPosition;
	if (m_SpectrumAnalyser->m_channelMode == 1 && m_SpectrumAnalyser->m_sampleType == 1) {
		for (int a = 0; a < m_SpectrumAnalyser->m_SampleSize;) {
			if ((DWORD)c >= m_SpectrumAnalyser->m_AudioDataBufferLength) {
				m_SpectrumAnalyser->m_offset++;
				c -= m_SpectrumAnalyser->m_AudioDataBufferLength;
			}

			m_SpectrumAnalyser->m_Left[a] = (float) m_SpectrumAnalyser->m_AudioDataBuffer[c] / 128.0F;
			m_SpectrumAnalyser->m_Right[a] = m_SpectrumAnalyser->m_Left[a];
			a++;
			c++;
		}

	} else if (m_SpectrumAnalyser->m_channelMode == 2 && m_SpectrumAnalyser->m_sampleType == 1) {
		for (int a = 0; a < m_SpectrumAnalyser->m_SampleSize;) {
			if ((DWORD)c >= m_SpectrumAnalyser->m_AudioDataBufferLength) {
				m_SpectrumAnalyser->m_offset++;
				c -= m_SpectrumAnalyser->m_AudioDataBufferLength;
			}

			m_SpectrumAnalyser->m_Left[a] = (float) m_SpectrumAnalyser->m_AudioDataBuffer[c] / 128.0F;
			m_SpectrumAnalyser->m_Right[a] = (float) m_SpectrumAnalyser->m_AudioDataBuffer[c + 1] / 128.0F;
			a++;
			c += 2;
		}

	} else if (m_SpectrumAnalyser->m_channelMode == 1 && m_SpectrumAnalyser->m_sampleType == 2) {
		for (int a = 0; a < m_SpectrumAnalyser->m_SampleSize;) {
			if ((DWORD)c >= m_SpectrumAnalyser->m_AudioDataBufferLength) {
				m_SpectrumAnalyser->m_offset++;
				c -= m_SpectrumAnalyser->m_AudioDataBufferLength;
			}

			m_SpectrumAnalyser->m_Left[a] = (float) ((m_SpectrumAnalyser->m_AudioDataBuffer[c + 1] << 8) +
				m_SpectrumAnalyser->m_AudioDataBuffer[c]) / 32767.0F;
			m_SpectrumAnalyser->m_Right[a] = m_SpectrumAnalyser->m_Left[a];
			a++;
			c += 2;
		}

	} else if (m_SpectrumAnalyser->m_channelMode == 2 && m_SpectrumAnalyser->m_sampleType == 2) {
		for (int a = 0; a < m_SpectrumAnalyser->m_SampleSize;) {
			if ((DWORD)c >= m_SpectrumAnalyser->m_AudioDataBufferLength) {
				m_SpectrumAnalyser->m_offset++;
				c -= m_SpectrumAnalyser->m_AudioDataBufferLength;
			}

			m_SpectrumAnalyser->m_Left[a] = (float) ((m_SpectrumAnalyser->m_AudioDataBuffer[c + 1] << 8) +
				m_SpectrumAnalyser->m_AudioDataBuffer[c]) / 32767.0F;
			m_SpectrumAnalyser->m_Right[a] = (float) ((m_SpectrumAnalyser->m_AudioDataBuffer[c + 3] << 8) +
				m_SpectrumAnalyser->m_AudioDataBuffer[c + 2]) / 32767.0F;
			a++;
			c += 4;
		}

	}
}

void CSpectrumAnalyserThread::Execute()
{
	while(!m_IsStoping/*!m_Stop*/)
	{
		jlong wStn = CSystem::nanoTime();
		int wSdp = calculateSamplePosition();

		if (wSdp > 0)
			processSamples(wSdp);

		for (int a = 0; a < 1; a++)
		{
			float wFrr = (float) m_SpectrumAnalyser->m_FpsAsNS / (float) m_SpectrumAnalyser->m_DesiredFpsAsNS;
			m_SpectrumAnalyser->Process(wFrr);
		}

		jlong wEdn = CSystem::nanoTime();
		long wDelay = m_SpectrumAnalyser->m_FpsAsNS - (long)(wEdn - wStn);

		if (wDelay > 0L)
		{
			DWORD ms = (DWORD)wDelay / 0xf4240L;
			DWORD ns = (DWORD)wDelay % 0xf4240L;
			if(ns >= 500000) ms++;			
			Sleep(ms);

			if (m_SpectrumAnalyser->m_FpsAsNS > m_SpectrumAnalyser->m_DesiredFpsAsNS)
				m_SpectrumAnalyser->m_FpsAsNS -= wDelay;
			else
				m_SpectrumAnalyser->m_FpsAsNS = m_SpectrumAnalyser->m_DesiredFpsAsNS;
		}
		else
		{
			m_SpectrumAnalyser->m_FpsAsNS += -wDelay;
			Sleep(10);
		}
	}

	Sleep(50);
	memset(m_SpectrumAnalyser->m_peaks, 0, m_SpectrumAnalyser->m_saBands);
	memset(m_SpectrumAnalyser->m_peaksDelay, 0, m_SpectrumAnalyser->m_saBands);
	memset(m_SpectrumAnalyser->m_oldFFT, 0, m_SpectrumAnalyser->m_saFFTSampleSize);
	m_Stop=TRUE;
	m_IsStoping=FALSE;
}

/************************************************************************/
/* CSpectrumAnalyser                                                    */
/************************************************************************/
CSpectrumAnalyser::CSpectrumAnalyser(CBasicPlayer* pPlayer)
{
	m_Player = pPlayer;
	m_SpectrumAnalyserThread = new CSpectrumAnalyserThread(this);
    m_show=false;
	/* digital signal process */
	m_hdcMem = 0;
	m_hdcMem1 = 0;
	m_hbmMem = 0;
	m_hbmMem1 =0;
	m_hbrush = 0;
	m_hbrush1 = 0;
	m_hOld = 0;
	m_hOld1 = 0;
	m_hbrushOld =0;
	m_hbrushOld1 = 0;

	m_AudioDataBufferLength = DEFAULT_BUFFER_SIZE << 1;
	m_AudioDataBuffer = new jbyte[m_AudioDataBufferLength];
	m_SampleSize = DEFAULT_SAMPLE_SIZE;
	m_DesiredFpsAsNS = 0x3B9ACA00L / DEFAULT_FPS;
	m_FpsAsNS = m_DesiredFpsAsNS;
	m_Left = new FLOAT[DEFAULT_SAMPLE_SIZE];
	m_Right = new FLOAT[DEFAULT_SAMPLE_SIZE];
	memset(m_Left,0,sizeof(FLOAT)*DEFAULT_SAMPLE_SIZE);
	memset(m_Right,0,sizeof(FLOAT)*DEFAULT_SAMPLE_SIZE);
	m_position = 0;
	m_offset = 0;
	m_sampleType = SAMPLE_TYPE_SIXTEEN_BIT;
	m_channelMode = CHANNEL_MODE_STEREO;

	memset(m_AudioDataBuffer, 0, m_AudioDataBufferLength);
	calcPos();
	m_saFFTSampleSize = DEFAULT_SPECTRUM_ANALYSER_FFT_SAMPLE_SIZE;
	m_saBands = DEFAULT_SPECTRUM_ANALYSER_BAND_COUNT;
	m_saDecay = DEFAULT_SPECTRUM_ANALYSER_DECAY;
	m_FFT = new CFastFourierTransform(m_saFFTSampleSize);
	m_peaks = new INT[m_saBands];
	m_peaksDelay = new INT[m_saBands];
	m_oldFFT = new FLOAT[m_saFFTSampleSize];
	m_saMultiplier = (FLOAT)((m_saFFTSampleSize / 2) / m_saBands);
	m_barOffset = 1;
	m_peakDelay = DEFAULT_SPECTRUM_ANALYSER_PEAK_DELAY;
	m_peaksEnabled = TRUE;
	m_saColorScale = (FLOAT)(256 / m_height) * m_barOffset * 1.0F;
	memset(m_peaks, 0, m_saBands*sizeof(INT));
	memset(m_peaksDelay, 0, m_saBands*sizeof(INT));
	memset(m_oldFFT, 0, m_saFFTSampleSize*sizeof(FLOAT));

	m_hdcScreen = GetWindowDC(m_Player->m_hWnd);
	m_hdcMem = CreateCompatibleDC(m_hdcScreen);
	m_hdcMem1 = CreateCompatibleDC(m_hdcScreen);
	m_hbmMem = CreateCompatibleBitmap(m_hdcScreen, m_winwidth, m_winheight);
	m_hbmMem1 = CreateCompatibleBitmap(m_hdcScreen, m_winwidth, m_winheight);
	m_hbrush = CreateSolidBrush(RGB(0, 0, 40));
	m_hbrush1 = CreateSolidBrush(RGB(125, 125, 125));
	m_hOld = (HBITMAP)SelectObject(m_hdcMem, m_hbmMem);
	m_hOld1 = (HBITMAP)SelectObject(m_hdcMem1, m_hbmMem1);
	m_hbrushOld = (HBRUSH)SelectObject(m_hdcMem, m_hbrush);
	m_hbrushOld1 = (HBRUSH)SelectObject(m_hdcMem, m_hbrush1);
	prepare();
}

CSpectrumAnalyser::~CSpectrumAnalyser(void)
{
	if(m_SpectrumAnalyserThread != NULL)
		delete m_SpectrumAnalyserThread;

	if(m_AudioDataBuffer != NULL)
		delete [] m_AudioDataBuffer;

	if(m_Left != NULL)
		delete [] m_Left;

	if(m_Right != NULL)
		delete [] m_Right;

	if(m_peaks != NULL)
		delete [] m_peaks;

	if(m_peaksDelay != NULL)
		delete [] m_peaksDelay;

	if(m_oldFFT != NULL)
		delete [] m_oldFFT;

	if(m_FFT != NULL)
		delete m_FFT;

	m_SpectrumAnalyserThread = NULL;
	m_AudioDataBuffer = NULL;
	m_Left = NULL;
	m_Right = NULL;
	m_peaks = NULL;
	m_peaksDelay = NULL;
	m_oldFFT = NULL;
	m_FFT = NULL;

	/* draw spectrum analyser */
	SelectObject(m_hdcMem, m_hOld);
	SelectObject(m_hdcMem1, m_hOld1);
	SelectObject(m_hdcMem, m_hbrushOld);
	SelectObject(m_hdcMem, m_hbrushOld1);
	DeleteObject(m_hbmMem);
	DeleteObject(m_hbmMem1);
	DeleteObject(m_hbrush);
	DeleteObject(m_hbrush1);
	DeleteDC(m_hdcMem);
	DeleteDC(m_hdcMem1);
}

void CSpectrumAnalyser::Start()
{
	if(m_SpectrumAnalyserThread != NULL)
		m_SpectrumAnalyserThread->Resume();
}

void CSpectrumAnalyser::Stop()
{
	if(m_SpectrumAnalyserThread != NULL)
		m_SpectrumAnalyserThread->Stop();
    m_SpectrumAnalyserThread=NULL;
}
extern INT g_client_bottom;
extern INT g_client_top;
extern int g_leftMetrics;
void CSpectrumAnalyser::Process(float pFrameRateRatioHint)
{
	if(IsIconic(m_Player->m_hWnd) == TRUE)
		return;
   
	
	for (int a = 0; a < m_SampleSize; a++) {
		m_Left[a] = (m_Left[a] + m_Right[a]) / 2.0f;
	}

	float c = 0;
	float pFrrh = pFrameRateRatioHint;
	float* wFFT = m_FFT->Calculate(m_Left, m_SampleSize);
	/*
	CString strr;
	strr.Format("%d,%d",m_Left,m_SampleSize);
	::AfxMessageBox(strr);
	return;
	*/
	float wSadfrr = m_saDecay * pFrrh;
	float wBw = ((float) m_width / (float) m_saBands);

	RECT rect;
	rect.left = 0;
	rect.top = 0;
	rect.right = rect.left + m_winwidth;
	rect.bottom = rect.top + m_winheight;
	FillRect(m_hdcMem, &rect, m_hbrush);

	for (int a = 0,  bd = 0; bd < m_saBands; a += (INT)m_saMultiplier, bd++) {
		float wFs = 0;
		for (int b = 0; b < (INT)m_saMultiplier; b++) {
			wFs += wFFT[a + b];
		}
		wFs = (wFs * (float) log(bd + 2.0F));

		if(wFs > 0.005F && wFs < 0.009F)
			wFs *= 9.0F * PI;
		else if(wFs > 0.01F && wFs < 0.1F)
			wFs *= 3.0F * PI;
		else if(wFs > 0.1F && wFs < 0.5F)
			wFs *= PI; //enlarge PI times, if do not, the bar display abnormally, why??

		if (wFs > 1.0F) {
			wFs = 0.9F;
		}

		if (wFs >= (m_oldFFT[a] - wSadfrr)) {
			m_oldFFT[a] = wFs;
		} else {
			m_oldFFT[a] -= wSadfrr;
			if (m_oldFFT[a] < 0) {
				m_oldFFT[a] = 0;
			}
			wFs = m_oldFFT[a];
		}
        if(/*m_show*/g_client_bottom)
		{
		   drawSpectrumAnalyserBar(&rect, (int) c+m_left, m_height+m_top, (int) wBw - 1, (int) (wFs * m_height), bd);	

		}
		c += wBw;
	}
	if(g_client_bottom/*m_show*/)
	{
        int postop;
		if(!(GetWindowStyle(m_Player->m_hWnd)&WS_DLGFRAME))
		{
           postop=0;
		}
		else
		{
           postop=42;
		}
		BitBlt(m_hdcScreen,g_leftMetrics, postop, m_winwidth-2, m_winheight, m_hdcMem, 0, 0, SRCCOPY); //liuxb,need modify
	}
}

void CSpectrumAnalyser::drawSpectrumAnalyserBar(RECT* pRect, int pX, int pY, int pWidth, int pHeight, int band)
{
	/* draw gradient bar */
	BitBlt(m_hdcMem, pX, pY-pHeight, pWidth, pHeight, m_hdcMem1, pX, pY-pHeight, SRCCOPY);

	if (m_peaksEnabled == TRUE) {
		if (pHeight > m_peaks[band]) {
			m_peaks[band] = pHeight;
			m_peaksDelay[band] = m_peakDelay;
		} else {
			m_peaksDelay[band]--;
			if (m_peaksDelay[band] < 0) {
				m_peaks[band]--;
			}
			if (m_peaks[band] < 0) {
				m_peaks[band] = 0;
			}
		}

		RECT rect = {0};
		rect.left = pX;
		rect.right = rect.left + pWidth;
		rect.top = pY - m_peaks[band];
		rect.bottom = rect.top + 1;
		FillRect(m_hdcMem, &rect, m_hbrush1); //这里画白线
	}
}

void CSpectrumAnalyser::prepare()
{
	/* draw gradient */

	RECT rect = {m_left,m_top,m_left+m_width,m_top+m_height};
	DWORD colors[] = {RGB(200,0,0), RGB(200,200,0), RGB(0,200,0), RGB(0,0,0)};
	DrawGradient(m_hdcMem1, &rect, colors,4, GRADIENT_FILL_RECT_V);
	/* draw grid */
	//模拟分辨率
	for(int i=m_height+m_top;i>=m_top;i--)
	{
		if(i % 2 == 0)
			continue;

		MoveToEx(m_hdcMem1,m_left, i, NULL);
		LineTo(m_hdcMem1, m_left+m_width, i);
	}
}
void CSpectrumAnalyser::calcPos()
{
	/* spectrum analyser */
	RECT rt;
	GetClientRect(m_Player->m_hWnd,&rt);
	m_winwidth = rt.right - rt.left;
	m_winheight = rt.bottom - rt.top;
	m_width  = m_winwidth/2;
	m_height = m_winheight*2/3;
	m_left   = m_width/2;
	m_top    = m_height/4;
}

void CSpectrumAnalyser::update()
{
	if(m_SpectrumAnalyserThread)
	{
	   m_SpectrumAnalyserThread->Suspend();
       calcPos();
	   if(m_hbmMem)  {DeleteObject(m_hbmMem); m_hbmMem =0; }
	   if(m_hbmMem1) {DeleteObject(m_hbmMem1);m_hbmMem1=0; }
	   if(m_hdcMem)  {DeleteObject(m_hdcMem); DeleteDC(m_hdcMem); m_hdcMem =0; }
	   if(m_hdcMem1) {DeleteObject(m_hdcMem1);DeleteDC(m_hdcMem1);m_hdcMem1=0; }
	   m_hdcMem  = CreateCompatibleDC(m_hdcScreen);
	   m_hdcMem1 = CreateCompatibleDC(m_hdcScreen);
	   m_hbmMem  = CreateCompatibleBitmap(m_hdcScreen, m_winwidth, m_winheight);
	   m_hbmMem1 = CreateCompatibleBitmap(m_hdcScreen, m_winwidth, m_winheight);	   
	   SelectObject(m_hdcMem, m_hbmMem);
	   SelectObject(m_hdcMem1, m_hbmMem1);
	   SelectObject(m_hdcMem, m_hbrush);
	   SelectObject(m_hdcMem, m_hbrush1);
	   prepare();	 
       m_SpectrumAnalyserThread->Resume();
	}

}
int CSpectrumAnalyser::Show(BOOL flag)
{

	m_show=flag;
	return 0;
}

/************************************************************************/
/* CBasicPlayer                                                         */
/************************************************************************/
CBasicPlayer::CBasicPlayer()
{
  m_isPause=FALSE;
}

CSpectrumAnalyser* CBasicPlayer::GetSpectrumAnalyser()
{
	return m_SpectrumAnalyser;
}
void CBasicPlayer::Open(HWND hWnd, TCHAR *pszFileName) 
{
	if(pszFileName == NULL)
	{
		MessageBox(hWnd, _TEXT("You should specify a file to play."), _TEXT("Error"), MB_OK);
		return;
	}
    m_PlayThread=NULL;
	m_hWnd=hWnd;
	m_Playing=FALSE;
	m_Input = NULL;
//	m_WMInput = NULL;
	m_WAVInput = NULL;
	m_MP3Input = NULL;
	m_VorbisInput = NULL;
	m_PlayThread = new CPlayThread(this);
	m_SpectrumAnalyser = new CSpectrumAnalyser(this);
	m_cs = new CMyCriticalSection;

	BOOL IsMp3(FALSE), IsWav(FALSE), IsWma(FALSE), IsVorbis(FALSE);
#ifdef _UNICODE
	if(wcsstr((const TCHAR*)_wcsupr(pszFileName), _TEXT(".MP3")) != NULL)
		IsMp3 = TRUE;

	if(wcsstr((const TCHAR*)_wcsupr(pszFileName), _TEXT(".WMA")) != NULL)
		IsWma = TRUE;
  
	if(wcsstr((const TCHAR*)_wcsupr(pszFileName), _TEXT(".WAV")) != NULL)
		IsWav = TRUE;

	if(wcsstr((const TCHAR*)_wcsupr(pszFileName), _TEXT(".OGG")) != NULL)
		IsVorbis = TRUE;
#else
	if(strstr((const TCHAR*)_strupr(pszFileName), _TEXT(".MP3")) != NULL)
		IsMp3 = TRUE;

	if(strstr((const TCHAR*)_strupr(pszFileName), _TEXT(".WMA")) != NULL)
		IsWma = TRUE;

	if(strstr((const TCHAR*)_strupr(pszFileName), _TEXT(".WAV")) != NULL)
		IsWav = TRUE;

	if(strstr((const TCHAR*)_strupr(pszFileName), _TEXT(".OGG")) != NULL)
		IsVorbis = TRUE;
#endif

	if(IsWma)
	{
//		if(m_WMInput == NULL)
//			m_WMInput = new CWMIn;

//		m_Input = (CFileIn*)m_WMInput;
	}
	else if(IsWav)
	{
		if(m_WAVInput == NULL)
			m_WAVInput = new CWaveIn;

		m_Input = (CFileIn*)m_WAVInput;
	}
	else if(IsVorbis)
	{
		if(m_VorbisInput == NULL)
			m_VorbisInput = new CVorbisIn;

		m_Input = (CFileIn*)m_VorbisInput;
	}
	else if(IsMp3)
	{
		if(m_MP3Input == NULL)
			m_MP3Input = new CMP3In;

		m_Input = (CFileIn*)m_MP3Input;
	}

	m_Input->SetFileName(pszFileName);
	m_bytePosition = 0;
}

CBasicPlayer::~CBasicPlayer(void)
{
	if(m_PlayThread != NULL)
		delete m_PlayThread;

	if(m_SpectrumAnalyser != NULL)
		delete m_SpectrumAnalyser;

	if(m_cs != NULL)
		delete m_cs;

	if(m_Input != NULL)
		m_Input->CloseFile();

//	if(m_WMInput != NULL)
//		delete m_WMInput;

	if(m_WAVInput != NULL)
		delete m_WAVInput;

	if(m_MP3Input != NULL)
		delete m_MP3Input;

	if(m_VorbisInput != NULL)
		delete m_VorbisInput;

	m_PlayThread = NULL;
	m_cs = NULL;
	m_info = NULL;
	m_Input = NULL;
//	m_WMInput = NULL;
	m_WAVInput = NULL;
	m_MP3Input = NULL;
	m_VorbisInput = NULL;
}

void CBasicPlayer::Start()
{
	m_BufferSize = DEFAULT_BUFFER_SIZE;
	m_SampleRate = m_Input->GetSampleRate();
	//m_FrameSize = DEFAULT_FRAME_SIZE; liuxb 2001 modify
	m_BitPerSample = m_Input->GetBitsPerSample();
	m_Channels = m_Input->GetChannels();
	m_FrameSize =m_BitPerSample/8*m_Channels;

	/* initialize direct sound */
	INT32 count = DAUDIO_GetDirectAudioDeviceCount();
	m_info = (DS_Info*)DAUDIO_Open(0, 0 , TRUE, DAUDIO_PCM, m_SampleRate,m_BitPerSample,
		m_FrameSize, m_Channels, TRUE, FALSE, m_BufferSize);
	m_SpectrumAnalyser->Start();
	if(m_PlayThread != NULL && m_PlayThread->Suspended())
	{
		m_PlayThread->Resume();
		m_Playing = TRUE;
	}
}

void CBasicPlayer::Stop()
{
	if(m_PlayThread != NULL)
	{
		m_PlayThread->Stop();
		m_SpectrumAnalyser->Stop();
	}
    m_Playing =FALSE;
    m_isPause =FALSE;

}

jlong CBasicPlayer::GetLongFramePosition()
{
	//m_cs->Enter();
	jlong pos = DAUDIO_GetBytePosition((void*)m_info, TRUE, m_bytePosition);
	if(pos < 0) pos = 0;
	return (jlong)(pos / DEFAULT_FRAME_SIZE);
}

int CBasicPlayer::pause(void)
{
	m_isPause=TRUE;
	DAUDIO_Stop((void*)m_info, TRUE);
	return 0;
}

int CBasicPlayer::resume(void)
{
	m_isPause=FALSE;
    DAUDIO_Start((void*)m_info, TRUE);
	return 0;
}

//歌曲总时间
DWORD CBasicPlayer::GetTotalTime(void)
{
	return m_Input->GetTotalTime();
}
DWORD CBasicPlayer::getTime(void)
{
//当前播放时间
	return m_Input->GetCurrentTime();
}

#if 0
LARGE_INTEGER CBasicPlayer::getPosition(void)
{
//	return m_Input->GetPosition();	
	LARGE_INTEGER li;
	li.QuadPart=m_bytePosition;
	return li;	
}
#endif 
int CBasicPlayer::SetPosition(LARGE_INTEGER pos)
{
	return m_Input->Seek(&pos);
}

LARGE_INTEGER CBasicPlayer::getSize(void)
{
	return m_Input->GetSize();
}
BOOL CBasicPlayer::isPause(void)
{
	return m_isPause;
}
