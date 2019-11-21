#include "stdafx.h"
#include "Zoom.h"


//那么访问一个点的函数可以写为：
inline TARGB32& Pixels(const TPicRegion& pic,const long x,const long y)
{
    return ( (TARGB32*)((TUInt8*)pic.pdata+pic.byte_width*y) )[x];
}
//访问一个点的函数，(x,y)坐标可能超出图片边界,进行边界饱和
inline TARGB32& Pixels_Bound(const TPicRegion& pic,long x,long y,bool& IsInPic)
{
    //assert((pic.width>0)&&(pic.height>0));
    IsInPic=true;
    if (x<0) {x=0; IsInPic=false; } else if (x>=pic.width ) {x=pic.width -1; IsInPic=false; }
    if (y<0) {y=0; IsInPic=false; } else if (y>=pic.height) {y=pic.height-1; IsInPic=false; }
    return Pixels(pic,x,y);
}
inline TARGB32 BilInear0(const TPicRegion& pic,float fx,float fy)
{
	long x=(long)fx; if (x>fx) --x; //x=floor(fx);    
	long y=(long)fy; if (y>fy) --y; //y=floor(fy);
	bool IsInPic;
	TARGB32 Color0=Pixels_Bound(pic,x,y,IsInPic);
//	if (!IsInPic) Color0.a=0;
	TARGB32 Color2=Pixels_Bound(pic,x+1,y,IsInPic);
//	if (!IsInPic) Color2.a=0;
	TARGB32 Color1=Pixels_Bound(pic,x,y+1,IsInPic);
//	if (!IsInPic) Color1.a=0;
	TARGB32 Color3=Pixels_Bound(pic,x+1,y+1,IsInPic);
//	if (!IsInPic) Color3.a=0;
	
	float u=fx-x;
	float v=fy-y;
	float pm3=u*v;
	float pm2=u*(1-v);
	float pm1=v*(1-u);
	float pm0=(1-u)*(1-v);
	
	TARGB32 result;
//	result.a=(pm0*Color0.a+pm1*Color1.a+pm2*Color2.a+pm3*Color3.a);
	result.r=(unsigned char)(pm0*Color0.r+pm1*Color1.r+pm2*Color2.r+pm3*Color3.r);
	result.g=(unsigned char)(pm0*Color0.g+pm1*Color1.g+pm2*Color2.g+pm3*Color3.g);
	result.b=(unsigned char)(pm0*Color0.b+pm1*Color1.b+pm2*Color2.b+pm3*Color3.b);
	
	return result;
}

void PicZoom_BilInear0(const TPicRegion& Dst,const TPicRegion& Src)
{
    if (  (0==Dst.width)||(0==Dst.height)
        ||(0==Src.width)||(0==Src.height)) return;
	
    unsigned long dst_width=Dst.width;
    TARGB32* pDstLine=Dst.pdata;
    for (unsigned long y=0;y<Dst.height;++y)
    {
        float srcy=(float)y*Src.height/Dst.height-0.5;
        for (unsigned long x=0;x<dst_width;++x)
        {
            float srcx=(float)x*Src.width/Dst.width-0.5;
            pDstLine[x]=BilInear0(Src,srcx,srcy);
        }
        ((TUInt8*&)pDstLine)+=Dst.byte_width;
    }
}

inline TARGB32 BilInear3(TARGB32* PColor0,TARGB32* PColor1,unsigned long u_8,unsigned long v_8)
{
	unsigned long pm3_8=(u_8*v_8)>>8;
	unsigned long pm2_8=u_8-pm3_8;
	unsigned long pm1_8=v_8-pm3_8;
	unsigned long pm0_8=256-pm1_8-pm2_8-pm3_8;
	
	unsigned long Color=*(unsigned long*)(PColor0);
	unsigned long BR=(Color & 0x00FF00FF)*pm0_8;
	unsigned long GA=((Color & 0xFF00FF00)>>8)*pm0_8;
	Color=((unsigned long*)(PColor0))[1];
	GA+=((Color & 0xFF00FF00)>>8)*pm2_8;
	BR+=(Color & 0x00FF00FF)*pm2_8;
	Color=*(unsigned long*)(PColor1);
	GA+=((Color & 0xFF00FF00)>>8)*pm1_8;
	BR+=(Color & 0x00FF00FF)*pm1_8;
	Color=((unsigned long*)(PColor1))[1];
	GA+=((Color & 0xFF00FF00)>>8)*pm3_8;
	BR+=(Color & 0x00FF00FF)*pm3_8;
	
	TARGB32 result;
	*(unsigned long*)(&result)=(GA & 0xFF00FF00)|((BR & 0xFF00FF00)>>8);
	return result; 
}

void PicZoom_BilInear3(const TPicRegion& Dst,const TPicRegion& Src)
{
    if (  (0==Dst.width)||(0==Dst.height)
        ||(0==Src.width)||(0==Src.height)) return;
	
    unsigned long xrIntFloat_16=((Src.width-1)<<16)/Dst.width+1; 
    unsigned long yrIntFloat_16=((Src.height-1)<<16)/Dst.height+1;
	
    unsigned long dst_width=Dst.width;
    long Src_byte_width=Src.byte_width;
    TARGB32* pDstLine=Dst.pdata;
    unsigned long srcy_16=0;
    for (unsigned long y=0;y<Dst.height;++y)
    {
        unsigned long v_8=(srcy_16 & 0xFFFF)>>8;
        TARGB32* PSrcLineColor= (TARGB32*)((TUInt8*)(Src.pdata)+Src_byte_width*(srcy_16>>16)) ;
        unsigned long srcx_16=0;
        for (unsigned long x=0;x<dst_width;++x)
        {
            TARGB32* PColor0=&PSrcLineColor[srcx_16>>16];
            pDstLine[x]=BilInear3(PColor0,(TARGB32*)((TUInt8*)(PColor0)+Src_byte_width),(srcx_16 & 0xFFFF)>>8,v_8);
            srcx_16+=xrIntFloat_16;
        }
        srcy_16+=yrIntFloat_16;
        ((TUInt8*&)pDstLine)+=Dst.byte_width;
    }
}


inline TARGB32 BilInear2(TARGB32* PColor0,TARGB32* PColor1,unsigned long u_8,unsigned long v_8)
{
	unsigned long pm3_16=u_8*v_8;
	unsigned long pm2_16=(u_8<<8)-pm3_16;
	unsigned long pm1_16=(v_8<<8)-pm3_16;
	unsigned long pm0_16=(1<<16)-pm1_16-pm2_16-pm3_16;
    
	TARGB32 result;
	
	result.r=(unsigned char)((pm0_16*PColor0[0].r+pm2_16*PColor0[1].r+pm1_16*PColor1[0].r+pm3_16*PColor1[1].r)>>16);
//	result.a=((pm0_16*PColor0[0].a+pm2_16*PColor0[1].a+pm1_16*PColor1[0].a+pm3_16*PColor1[1].a)>>16);
	result.g=(unsigned char)((pm0_16*PColor0[0].g+pm2_16*PColor0[1].g+pm1_16*PColor1[0].g+pm3_16*PColor1[1].g)>>16);
	result.b=(unsigned char)((pm0_16*PColor0[0].b+pm2_16*PColor0[1].b+pm1_16*PColor1[0].b+pm3_16*PColor1[1].b)>>16);
	
	return result; 
}

void PicZoom_BilInear2(const TPicRegion& Dst,const TPicRegion& Src)
{
    if (  (0==Dst.width)||(0==Dst.height)
        ||(0==Src.width)||(0==Src.height)) return;
	
    unsigned long xrIntFloat_16=((Src.width-1)<<16)/Dst.width+1; 
    unsigned long yrIntFloat_16=((Src.height-1)<<16)/Dst.height+1;
	
    unsigned long dst_width=Dst.width;
    long Src_byte_width=Src.byte_width;
    TARGB32* pDstLine=Dst.pdata;
    unsigned long srcy_16=0;
    for (unsigned long y=0;y<Dst.height;++y)
    {
        unsigned long v_8=(srcy_16 & 0xFFFF)>>8;
        TARGB32* PSrcLineColor= (TARGB32*)((TUInt8*)(Src.pdata)+Src_byte_width*(srcy_16>>16)) ;
        unsigned long srcx_16=0;
        for (unsigned long x=0;x<dst_width;++x)
        {
            TARGB32* PColor0=&PSrcLineColor[srcx_16>>16];
            pDstLine[x]=BilInear2(PColor0,(TARGB32*)((TUInt8*)(PColor0)+Src_byte_width),(srcx_16 & 0xFFFF)>>8,v_8);
            srcx_16+=xrIntFloat_16;
        }
        srcy_16+=yrIntFloat_16;
        ((TUInt8*&)pDstLine)+=Dst.byte_width;
    }
}



float SinXDivX(float x) 
{
    const float a = -1; //a???? a=-2,-1,-0.75,-0.5??,??????????????

    if (x<0) x=-x; //x=abs(x);
    double x2=x*x;
    double x3=x2*x;
    if (x<=1)
      return (a+2)*x3 - (a+3)*x2 + 1;
    else if (x<=2) 
      return a*x3 - (5*a)*x2 + (8*a)*x - (4*a);
    else
      return 0;
} 
inline TUInt8 ColorBound(long Color)
{
    if (Color<=0)
        return 0;
    else if (Color>=255)
        return 255;
    else
        return Color;
}
TARGB32 ThreeOrder0(const TPicRegion& pic,const float fx,const float fy)
{
    long x0=(long)fx; if (x0>fx) --x0; //x0=floor(fx);    
    long y0=(long)fy; if (y0>fy) --y0; //y0=floor(fy);
    float fu=fx-x0;
    float fv=fy-y0;

    TARGB32 pixel[16];
    long i,j;

    for (j=0;j<4;++j)
    {
        for (i=0;i<4;++i)
        {
            long x=x0+i-1;
            long y=y0+j-1;
            bool IsInPic;
            pixel[i*4+j]=Pixels_Bound(pic,x,y,IsInPic);
//                if (!IsInPic) pixel[i*4+j].a=0;
        }
    }

    float afu[4];
    float afv[4];
    //
    afu[0]=SinXDivX(1+fu);
    afu[1]=SinXDivX(fu);
    afu[2]=SinXDivX(1-fu);
    afu[3]=SinXDivX(2-fu);
    afv[0]=SinXDivX(1+fv);
    afv[1]=SinXDivX(fv);
    afv[2]=SinXDivX(1-fv);
    afv[3]=SinXDivX(2-fv);

    float afR[4];
    float afG[4];
    float afB[4];
    float afA[4];
    for (i=0;i<4;++i)
    {
        afA[i]=0;
        afR[i]=0;
        afG[i]=0;
        afB[i]=0;
    }
    for (i=0;i<4;++i)
    {
        for (j=0;j<4;++j)
        {
//                afA[i]=afA[i]+afv[j]*pixel[i*4+j].a;
            afR[i]=afR[i]+afv[j]*pixel[i*4+j].r;
            afG[i]=afG[i]+afv[j]*pixel[i*4+j].g;
            afB[i]=afB[i]+afv[j]*pixel[i*4+j].b;
        }
    }

    TARGB32 result;
//        result.a=ColorBound((long)(afu[0]*afA[0]+afu[1]*afA[1]+afu[2]*afA[2]+afu[3]*afA[3]+0.5));
    result.r=ColorBound((long)(afu[0]*afR[0]+afu[1]*afR[1]+afu[2]*afR[2]+afu[3]*afR[3]+0.5));
    result.g=ColorBound((long)(afu[0]*afG[0]+afu[1]*afG[1]+afu[2]*afG[2]+afu[3]*afG[3]+0.5));
    result.b=ColorBound((long)(afu[0]*afB[0]+afu[1]*afB[1]+afu[2]*afB[2]+afu[3]*afB[3]+0.5));

    return result;
}

void PicZoom_ThreeOrder0(const TPicRegion& Dst,const TPicRegion& Src)
{
    if (  (0==Dst.width)||(0==Dst.height)
        ||(0==Src.width)||(0==Src.height)) return;


    unsigned long dst_width=Dst.width;
    TARGB32* pDstLine=Dst.pdata;
    for (unsigned long y=0;y<Dst.height;++y)
    {
        float srcy=(float)y*Src.height/Dst.height-0.5;
        for (unsigned long x=0;x<dst_width;++x)
        {
            float srcx=(float)x*Src.width/Dst.width-0.5;
            pDstLine[x]=ThreeOrder0(Src,srcx,srcy);
        }
        ((TUInt8*&)pDstLine)+=Dst.byte_width;
    }
}