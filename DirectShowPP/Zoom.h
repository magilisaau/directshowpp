#ifndef  _ZOOM_2007_lxb
#define  _ZOOM_2007_lxb


typedef unsigned char TUInt8; // [0..255]
struct TARGB32      //32 bit color
{
    TUInt8  b,g,r;          //a is alpha
};

struct TPicRegion  //一块颜色数据区的描述，便于参数传递
{
    TARGB32*    pdata;         //颜色数据首地址
    unsigned long        byte_width;    //一行数据的物理宽度(字节宽度)；
	//abs(byte_width)有可能大于等于width*sizeof(TARGB32);
    unsigned long        width;         //像素宽度
    unsigned long        height;        //像素高度
};

#endif