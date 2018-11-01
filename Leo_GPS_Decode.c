
#include "Leo_GPS_Decode.h"
#include "string.h"


struct minmea_sentence_rmc G_GPS_RMC;
uint8_t                    G_GPS_RMC_IsValide = 0;
struct minmea_sentence_gga G_GPS_GGA;
uint8_t                    G_GPS_GGA_IsValide = 0;


//@brief GPS数据缓存区初始化
/*--------------------------------------------------------------------------*/
//<*参数说明:
//<*    struct Leo_gps_buffer* mLeo_gps_buffer      被初始化的对象
//<*返回值说明:
//<*    0 成功,	其它失败
/*--------------------------------------------------------------------------*/
uint8_t Leo_GPS_BUFFER_Initial(struct Leo_gps_buffer* mLeo_gps_buffer)
{
    //缓存区清零
    memset(mLeo_gps_buffer->buffer,0,sizeof(mLeo_gps_buffer->buffer));
    //pSave指针指向 buffer 起始位置
    mLeo_gps_buffer->pSave = mLeo_gps_buffer->buffer;
    return 0;
}


//@brief GPS数据缓存区 内存入一个字符
/*--------------------------------------------------------------------------*/
//<*参数说明:
//<*    struct Leo_gps_buffer* mLeo_gps_buffer      GPS数据缓存区
//<*    int8_t mChar                                所要存入的字符
//<*返回值说明:
//<*    0 意味着目前存储的是一条完整的GPS语句，从起始位置 buffer 到 pSave指针指示位置的前一位
//<*    1 当前字符没有构成一个整的语句 $....*HH;HH为Checksum
/*--------------------------------------------------------------------------*/
uint8_t Leo_GPS_BUFFER_SaveByte(struct Leo_gps_buffer* mLeo_gps_buffer, int8_t mChar)
{
    //开始写入第一个字符 寻找 '$',其它舍弃
    if(mLeo_gps_buffer->pSave == mLeo_gps_buffer->buffer){
        if(mChar == (int8_t)('$')){
            *mLeo_gps_buffer->pSave = (int8_t)('$');
            mLeo_gps_buffer->pSave++;
            return 1;
        }else{
            return 1;
        }

    }

    //在第一个字符是 '$'的情况下，存储接下来的字符
    if(mLeo_gps_buffer->pSave > mLeo_gps_buffer->buffer){
        *mLeo_gps_buffer->pSave = mChar;
        mLeo_gps_buffer->pSave++;
    }
    //判断目前存储的额数据是否可以构成一条完整的语句；两个字符之前的字符是否为 '*'
    if(((mLeo_gps_buffer->pSave-3) > mLeo_gps_buffer->buffer) &&
            *(mLeo_gps_buffer->pSave-3) == (int8_t)('*')){
        return 0;
    }else{
        //继续存储字符
        return 1;
    }
}


//@brief 存入接收到的字符，进行GPS数据包解析
/*--------------------------------------------------------------------------*/
//<*参数说明:
//<*    struct Leo_gps_buffer* mLeo_gps_buffer      GPS数据缓存区
//<*    int8_t mChar                                所要存入的字符
//<*返回值说明:
//<*    0 解析成功，获取数据
//<*    1 数据不够解析
//<*    2 数据解析错误，有可能奇偶检校错误 或 数据判断错误！
/*--------------------------------------------------------------------------*/
uint8_t Leo_GPS_Decode(struct Leo_gps_buffer* mLeo_gps_buffer, int8_t mChar)
{
    if(Leo_GPS_BUFFER_SaveByte(mLeo_gps_buffer, mChar))
        return 1;
    else{
        //取出这条完整的语句后，把存储的指针移到缓存区的开始
        uint8_t mNum = mLeo_gps_buffer->pSave - mLeo_gps_buffer->buffer;
        int8_t mGPS_Sentence[mNum];
        memcpy(mGPS_Sentence,mLeo_gps_buffer->buffer,mNum);
        mLeo_gps_buffer->pSave = mLeo_gps_buffer->buffer;

        enum minmea_sentence_id mGPS_Sentence_ID = minmea_sentence_id(mGPS_Sentence);
        if(mGPS_Sentence_ID == MINMEA_INVALID || mGPS_Sentence_ID == MINMEA_UNKNOWN)
            return 2;
        if(mGPS_Sentence_ID == MINMEA_SENTENCE_RMC)
        {
            if(minmea_parse_rmc(&G_GPS_RMC,mGPS_Sentence))
            {
                G_GPS_RMC_IsValide = 1;
                return 0;
            }else{
                return 2;
            }
        }
        if(mGPS_Sentence_ID == MINMEA_SENTENCE_GGA)
        {
            if(minmea_parse_gga(&G_GPS_GGA,mGPS_Sentence))
            {
                G_GPS_GGA_IsValide = 1;
                return 0;
            }else{
                return 2;
            }
        }
        return 1;
    }
}

