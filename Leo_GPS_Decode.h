/******************** (C) COPYRIGHT********************
 * 文件名  ：Leo_GPS_Decode
 * 平台    ：C
 * 描述    ：自动存储串口接收的GPS数据，并进行解析
 * 作者    ：王成宾
 **********************************************************************/

#ifndef LEO_GPS_DECODE_H
#define LEO_GPS_DECODE_H

#include "minmea.h"
#include "stdint.h"

#ifdef __cplusplus
extern "C" {
#endif

#define LEO_GPS_BUFFER_MAXLENGTH 256

//@brief GPS数据缓存区结构体定义
/*--------------------------------------------------------------------------*/
struct Leo_gps_buffer {
  int8_t buffer[LEO_GPS_BUFFER_MAXLENGTH]; // GPS串口数据存储缓存区  char字符
  int8_t *pSave;                           //缓存区中当前存储的位置
};

//@brief 有一个系统时间，通过1pps校准
/*--------------------------------------------------------------------------*/
// uint32_t G_Time_WeekSenconds;                         //GPS
// 周内秒，通过1pps校准

//@brief GPS数据解析后存放
/*--------------------------------------------------------------------------*/
struct Leo_gps_data {
  struct minmea_date mDate;      //日期 年月日
  struct minmea_time mTime;      //时间 小时 分钟 秒 毫秒
  struct minmea_float latitude;  //纬度
  struct minmea_float longitude; //经度
  struct minmea_float hdop;      // HDOP水平精度因子（0.5~99.9）
  struct minmea_float mHigh; //[注意：高程(是点 到地球椭球面的高度)=
                             //点到水准面高度(海拔高度)+水准面到椭球面高度]
};

//@brief GPS数据缓存区初始化
/*--------------------------------------------------------------------------*/
//<*参数说明:
//<*    struct Leo_gps_buffer* mLeo_gps_buffer      被初始化的对象
//<*返回值说明:
//<*    0 成功,	其它失败
/*--------------------------------------------------------------------------*/
uint8_t Leo_GPS_BUFFER_Initial(struct Leo_gps_buffer *mLeo_gps_buffer);

//@brief GPS数据缓存区 内存入一个字符
/*--------------------------------------------------------------------------*/
//<*参数说明:
//<*    struct Leo_gps_buffer* mLeo_gps_buffer      GPS数据缓存区
//<*    int8_t mChar                                所要存入的字符
//<*返回值说明:
//<*    0 当前字符没有构成一个整的语句 $....*HH;HH为Checksum
//<*    1 意味着目前存储的是一条完整的GPS语句，从起始位置 buffer 到
//pSave指针指示位置的前一位
/*--------------------------------------------------------------------------*/
uint8_t Leo_GPS_BUFFER_SaveByte(struct Leo_gps_buffer *mLeo_gps_buffer,
                                int8_t mChar);

//@brief 存入接收到的字符，进行GPS数据包解析
/*--------------------------------------------------------------------------*/
//<*参数说明:
//<*    struct Leo_gps_buffer* mLeo_gps_buffer      GPS数据缓存区
//<*    int8_t mChar                                所要存入的字符
//<*返回值说明:
//<*    0 没有解析
//<*    1 解析成功，获取数据
/*--------------------------------------------------------------------------*/
uint8_t Leo_GPS_Decode(struct Leo_gps_buffer *mLeo_gps_buffer, int8_t mChar);

//@brief 基于nRF52 往 SDCard卡 内写入数据
/*--------------------------------------------------------------------------*/
//<*参数说明:
//<*    const void * buff,        [IN]指向要写入的数据的指针
//<*    UINT btw,                 [IN]要写入的字节数
//<*返回值说明:
//<*    FR_OK = 0,	其它失败
/*--------------------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* ：Leo_GPS_Decode.h */
