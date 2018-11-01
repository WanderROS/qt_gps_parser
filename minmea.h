/*
 * Copyright © 2014 Kosma Moczek <kosma@cloudyourcar.com>
 * This program is free software. It comes without any warranty, to the extent
 * permitted by applicable law. You can redistribute it and/or modify it under
 * the terms of the Do What The Fuck You Want To Public License, Version 2, as
 * published by Sam Hocevar. See the COPYING file for more details.
 */

#ifndef MINMEA_H
#define MINMEA_H

#ifdef __cplusplus
extern "C" {
#endif

#include <errno.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>
#ifdef MINMEA_INCLUDE_COMPAT
#include <minmea_compat.h>
#endif

#define MINMEA_MAX_LENGTH 80

enum minmea_sentence_id {
  MINMEA_INVALID = -1,
  MINMEA_UNKNOWN = 0,
  MINMEA_SENTENCE_RMC,
  MINMEA_SENTENCE_GGA,
  MINMEA_SENTENCE_GSA,
  MINMEA_SENTENCE_GLL,
  MINMEA_SENTENCE_GST,
  MINMEA_SENTENCE_GSV,
  MINMEA_SENTENCE_VTG,
  MINMEA_SENTENCE_ZDA,
};

struct minmea_float {
  int_least32_t value;
  int_least32_t scale;
};

struct minmea_date {
  int day;
  int month;
  int year;
};

struct minmea_time {
  int hours;
  int minutes;
  int seconds;
  int microseconds;
};

struct minmea_longlat {
  float longitude;
  float latitude;
};
/*
 *  使用 RMC中的 时间 经纬度 和 GGA中的 高程
 */

// d 代表 方向 ；D 代表日期
//$GPRMC,<1>,<2>,<3>,<4>,<5>,<6>,<7>,<8>,<9>,<10>,<11>,<12>*hh<CR><LF>
// " t    T   c   f   d   f   d   f   f    D   f   d"
// type,<1>time,<2>valid(1/0),<3>latitude(N 正 S 负),<5>longitude(E 正 W
// 负),<7>speed,<8>course,<9>date, <10>variation(E 正 W 负), <1>
//UTC时间，hhmmss（时分秒）格式 <2> 定位状态，A=有效定位，V=无效定位 <3>
//纬度ddmm.mmmm（度分）格式（前面的0也将被传输） <4>
//纬度半球N（北半球）或S（南半球） <5>
//经度dddmm.mmmm（度分）格式（前面的0也将被传输） <6>
//经度半球E（东经）或W（西经） <7> 地面速率（000.0~999.9节，前面的0也将被传输）
//<8> 地面航向（000.0~359.9度，以真北为参考基准，前面的0也将被传输）
//<9> UTC日期，ddmmyy（日月年）格式
//<10> 磁偏角（000.0~180.0度，前面的0也将被传输）
//<11> 磁偏角方向，E（东）或W（西）
//<12>
//模式指示（仅NMEA0183 3.00版本输出，A=自主定位，D=差分，E=估算，N=数据无效）
struct minmea_sentence_rmc {
  struct minmea_time time;
  uint8_t valid;
  struct minmea_float latitude;
  struct minmea_float longitude;
  struct minmea_float speed;
  struct minmea_float course;
  struct minmea_date date;
  struct minmea_float variation;
};

//$GPGGA,<1>,<2>,<3>,<4>,<5>,<6>,<7>,<8>,<9>,M,<10>,M,<11>,<12>*hh<CR><LF>
//     t  T   f   d   f   d   i   i   f   f  c   f  c   f    _
//<1> UTC时间，hhmmss（时分秒）格式
//<2> 纬度ddmm.mmmm（度分）格式（前面的0也将被传输）
//<3> 纬度半球N（北半球）或S（南半球）
//<4> 经度dddmm.mmmm（度分）格式（前面的0也将被传输）
//<5> 经度半球E（东经）或W（西经）
//<6> GPS状态：0=未定位，1=非差分定位，2=差分定位，6=正在估算
//<7> 正在使用解算位置的卫星数量（00~12）（前面的0也将被传输）
//<8> HDOP水平精度因子（0.5~99.9）
//<9> 海拔高度（-9999.9~99999.9）
//<10> 地球椭球面相对大地水准面的高度
//<11> 差分时间（从最近一次接收到差分信号开始的秒数，如果不是差分定位将为空）
//<12> 差分站ID号0000~1023（前面的0也将被传输，如果不是差分定位将为空）
struct minmea_sentence_gga {
  struct minmea_time time;       // UTC时间
  struct minmea_float latitude;  //纬度 北为正 南为负
  struct minmea_float longitude; //经度 东为正 西为负
  int fix_quality; // GPS状态：0=未定位，1=非差分定位，2=差分定位，6=正在估算
  int satellites_tracked; //正在使用解算位置的卫星数量（00~12）（前面的0也将被传输）
  struct minmea_float hdop; // HDOP水平精度因子（0.5~99.9）
  struct minmea_float
      altitude; //海拔高度（-9999.9~99999.9）
                //[注意：高程(是点 到地球椭球面的高度)= 点到水准面高度(海拔高度)
                //+ 水准面到椭球面高度]
  char altitude_units;        //单位 M
  struct minmea_float height; //地球椭球面相对大地水准面的高度
  char height_units;          //单位 M
  struct minmea_float
      dgps_age; //差分时间（从最近一次接收到差分信号开始的秒数，如果不是差分定位将为空）
};

enum minmea_gll_status {
  MINMEA_GLL_STATUS_DATA_VALID = 'A',
  MINMEA_GLL_STATUS_DATA_NOT_VALID = 'V',
};

// FAA mode added to some fields in NMEA 2.3.
enum minmea_faa_mode {
  MINMEA_FAA_MODE_AUTONOMOUS = 'A',
  MINMEA_FAA_MODE_DIFFERENTIAL = 'D',
  MINMEA_FAA_MODE_ESTIMATED = 'E',
  MINMEA_FAA_MODE_MANUAL = 'M',
  MINMEA_FAA_MODE_SIMULATED = 'S',
  MINMEA_FAA_MODE_NOT_VALID = 'N',
  MINMEA_FAA_MODE_PRECISE = 'P',
};

struct minmea_sentence_gll {
  struct minmea_float latitude;
  struct minmea_float longitude;
  struct minmea_time time;
  char status;
  char mode;
};

struct minmea_sentence_gst {
  struct minmea_time time;
  struct minmea_float rms_deviation;
  struct minmea_float semi_major_deviation;
  struct minmea_float semi_minor_deviation;
  struct minmea_float semi_major_orientation;
  struct minmea_float latitude_error_deviation;
  struct minmea_float longitude_error_deviation;
  struct minmea_float altitude_error_deviation;
};

enum minmea_gsa_mode {
  MINMEA_GPGSA_MODE_AUTO = 'A',
  MINMEA_GPGSA_MODE_FORCED = 'M',
};

enum minmea_gsa_fix_type {
  MINMEA_GPGSA_FIX_NONE = 1,
  MINMEA_GPGSA_FIX_2D = 2,
  MINMEA_GPGSA_FIX_3D = 3,
};

struct minmea_sentence_gsa {
  char mode;
  int fix_type;
  int sats[12];
  struct minmea_float pdop;
  struct minmea_float hdop;
  struct minmea_float vdop;
};

struct minmea_sat_info {
  int nr;
  int elevation;
  int azimuth;
  int snr;
};

struct minmea_sentence_gsv {
  int total_msgs;
  int msg_nr;
  int total_sats;
  struct minmea_sat_info sats[4];
};

struct minmea_sentence_vtg {
  struct minmea_float true_track_degrees;
  struct minmea_float magnetic_track_degrees;
  struct minmea_float speed_knots;
  struct minmea_float speed_kph;
  enum minmea_faa_mode faa_mode;
};

struct minmea_sentence_zda {
  struct minmea_time time;
  struct minmea_date date;
  int hour_offset;
  int minute_offset;
};

/**
 * Calculate raw sentence checksum. Does not check sentence integrity.
 */
uint8_t minmea_checksum(const char *sentence);

/**
 * Check sentence validity and checksum. Returns true for valid sentences.
 */
uint8_t minmea_check(const char *sentence /*, uint8_t strict*/);

/**
 * Determine talker identifier.
 */
uint8_t minmea_talker_id(char talker[3], const char *sentence);

/**
 * Determine sentence identifier.
 */
enum minmea_sentence_id
minmea_sentence_id(const char *sentence /*, uint8_t strict*/);

/**
 * Scanf-like processor for NMEA sentences. Supports the following formats:
 * c - single character (char *)
 * d - direction, returned as 1/-1, default 0 (int *)
 * f - fractional, returned as value + scale (int *, int *)
 * i - decimal, default zero (int *)
 * s - string (char *)
 * t - talker identifier and type (char *)
 * T - date/time stamp (int *, int *, int *)
 * Returns true on success. See library source code for details.
 */
uint8_t minmea_scan(const char *sentence, const char *format, ...);

/*
 * Parse a specific type of sentence. Return true on success.
 */
uint8_t minmea_parse_rmc(struct minmea_sentence_rmc *frame,
                         const char *sentence);
uint8_t minmea_parse_gga(struct minmea_sentence_gga *frame,
                         const char *sentence);
uint8_t minmea_parse_gsa(struct minmea_sentence_gsa *frame,
                         const char *sentence);
uint8_t minmea_parse_gll(struct minmea_sentence_gll *frame,
                         const char *sentence);
uint8_t minmea_parse_gst(struct minmea_sentence_gst *frame,
                         const char *sentence);
uint8_t minmea_parse_gsv(struct minmea_sentence_gsv *frame,
                         const char *sentence);
uint8_t minmea_parse_vtg(struct minmea_sentence_vtg *frame,
                         const char *sentence);
uint8_t minmea_parse_zda(struct minmea_sentence_zda *frame,
                         const char *sentence);

/**
 * Convert GPS UTC date/time representation to a UNIX timestamp.
 */
int minmea_gettime(struct timespec *ts, const struct minmea_date *date,
                   const struct minmea_time *time_);

/**
 * Rescale a fixed-point value to a different scale. Rounds towards zero.
 */
static inline int_least32_t minmea_rescale(struct minmea_float *f,
                                           int_least32_t new_scale) {
  if (f->scale == 0)
    return 0;
  if (f->scale == new_scale)
    return f->value;
  if (f->scale > new_scale)
    return (f->value +
            ((f->value > 0) - (f->value < 0)) * f->scale / new_scale / 2) /
           (f->scale / new_scale);
  else
    return f->value * (new_scale / f->scale);
}

/**
 * Convert a fixed-point value to a floating-point value.
 * Returns NaN for "unknown" values.
 */
static inline float minmea_tofloat(struct minmea_float *f) {
  if (f->scale == 0)
    return NAN;
  return (float)f->value / (float)f->scale;
}

/**
 * Convert a raw coordinate to a floating point DD.DDD... value.
 * Returns NaN for "unknown" values.
 */
static inline float minmea_tocoord(struct minmea_float *f) {
  if (f->scale == 0)
    return NAN;
  int_least32_t degrees = f->value / (f->scale * 100);
  int_least32_t minutes = f->value % (f->scale * 100);
  return (float)degrees + (float)minutes / (60 * f->scale);
}
struct minmea_longlat NemaTransform(struct minmea_sentence_rmc raw);
#ifdef __cplusplus
}
#endif

#endif /* MINMEA_H */

/* vim: set ts=4 sw=4 et: */
