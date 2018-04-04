/*
 **************************************************************************************
 *       Filename:  opusenc.cpp
 *    Description:   source file
 *
 *        Version:  1.0
 *        Created:  2018-04-04 14:19:01
 *
 *       Revision:  initial draft;
 **************************************************************************************
 */

#define LOG_TAG "opusenc"

#include <sys/mman.h>
#include <errno.h>
#include <arpa/inet.h>
#include <opus/opus.h>
#include <iostream>
#include "log.h"

using namespace std;
typedef unsigned char  uchar;
typedef unsigned short ushort;
typedef unsigned int   uint;
typedef unsigned long  ulong;

uchar* map_file(const char* fn, size_t* size) {
    FILE* fp = fopen(fn, "r");
    if(!fp) {
        loge("fail to open file: %s", fn);
        return (uchar*)MAP_FAILED;
    }
    fseek(fp, 0L, SEEK_END);
    long pos = ftell(fp);
    *size = pos;
    uchar* ptr = (uchar*)mmap(NULL, pos, PROT_READ|PROT_WRITE, MAP_PRIVATE, fileno(fp), 0);
    fclose(fp);
    return ptr;
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        log("usage: %s <pcm_file>\n", argv[0]);
        return -EINVAL;
    }
    OpusEncoder* encoder = NULL;
    int error = 0;
    size_t filesize = 0;
    int ret = 0;
    int16_t* input = NULL;
    int sample_rate = 48000;
    int frame_duration_ms = 20;

    encoder = opus_encoder_create(sample_rate, 1, OPUS_APPLICATION_VOIP, &error);
    if (!encoder) {
        loge("fail to create encoder: %d", error);
        return error;
    }
    ret = opus_encoder_ctl(encoder, OPUS_SET_INBAND_FEC(1));
    if (ret != 0) {
        loge("fail to enable fec");
        return ret;
    }

    uchar* ptr = map_file(argv[1], &filesize);
    if (ptr == MAP_FAILED) {
        loge("fail to map file");
        return -1;
    }
    input = (int16_t*)ptr;
    uchar* out = (uchar*)malloc(filesize);
    FILE* fp = fopen("enc.opus", "w");
    if (!fp) {
        loge("fail to open file out.opus");
        return -1;
    }

    int frame_size = sample_rate/1000*frame_duration_ms;

    while((uchar*)input < ptr + filesize) {
        ret = opus_encode(encoder, input, frame_size, out, filesize);
        if (ret <= 0) {
            loge("fail to encode");
            break;
        }
        logd("enc: %d", ret);
        fwrite(&ret, 1, sizeof(int), fp);
        fwrite(out, 1, ret, fp);
        input += frame_size;
    }
    fclose(fp);

    munmap(ptr, filesize);
    opus_encoder_destroy(encoder);
    return 0;
}

/********************************** END **********************************************/

