#ifndef  _ZOOM_2007_lxb
#define  _ZOOM_2007_lxb


typedef unsigned char TUInt8; // [0..255]
struct TARGB32      //32 bit color
{
    TUInt8  b,g,r;          //a is alpha
};

struct TPicRegion  //һ����ɫ�����������������ڲ�������
{
    TARGB32*    pdata;         //��ɫ�����׵�ַ
    unsigned long        byte_width;    //һ�����ݵ�������(�ֽڿ��)��
	//abs(byte_width)�п��ܴ��ڵ���width*sizeof(TARGB32);
    unsigned long        width;         //���ؿ��
    unsigned long        height;        //���ظ߶�
};

#endif