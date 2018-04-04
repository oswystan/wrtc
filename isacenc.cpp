/*
 **************************************************************************************
 *       Filename:  isacenc.cpp
 *    Description:   source file
 *
 *        Version:  1.0
 *        Created:  2018-04-04 15:03:11
 *
 *       Revision:  initial draft;
 **************************************************************************************
 */

#define LOG_TAG "isac-enc"
#include <sys/mman.h>
#include <errno.h>
#include <arpa/inet.h>
#include <iostream>
#include "webrtc/modules/audio_coding/codecs/isac/main/include/audio_encoder_isac.h"
#include "log.h"

using namespace std;
using namespace rtc;
using namespace webrtc;

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
    cout << pos << endl;
    *size = pos;
    uchar* ptr = (uchar*)mmap(NULL, pos, PROT_READ|PROT_WRITE, MAP_PRIVATE, fileno(fp), 0);
    fclose(fp);
    return ptr;
}

int main(int argc, char *argv[])
{
    if(argc != 2) {
        log("usage: %s <pcm_file>", argv[0]);
        return -1;
    }

    // 1. map input file
    size_t fsize = 0;
    uchar* ptr = map_file(argv[1], &fsize);
    if (ptr == MAP_FAILED) {
        loge("fail to map file");
        return -ENOMEM;
    }

    // 2. prepare output file
    FILE* fp = fopen("enc.isac", "wb");
    if (!fp) {
        loge("fail to open: enc.isac");
        return -EPERM;
    }

    AudioEncoderIsac::Config cfg;
    cfg.bit_rate = 20000;
    cfg.sample_rate_hz = 16000;
    auto encoder = new AudioEncoderIsac(cfg);
    if (!encoder) {
        loge("no memory for encoder");
        return -ENOMEM;
    }
    logd("channels:%lu, sample_rate:%d", encoder->NumChannels(), encoder->SampleRateHz());

    int16_t* input = (int16_t*)ptr;
    //int samples = cfg.sample_rate_hz / 1000 * cfg.frame_size_ms;
    //fixed samples because of checks in audio_encoder.cc
    int samples = 160;
    logd("samples=%d", samples);
    uint32_t timestamp = 1000;

    auto input_view = rtc::MakeArrayView((const int16_t*)input, fsize/sizeof(int16_t));
    uint32_t offset = 0;
    int encoded_size = 0;
    rtc::Buffer out_buf(0, 1024);

    while((uchar*)(input + samples) <= ptr + fsize) {
        out_buf.SetSize(0);
        auto info = encoder->Encode(timestamp, input_view.subview(offset, samples), &out_buf);
        if (info.encoded_bytes) {
            logd("encoded_bytes=%lu", info.encoded_bytes);
            encoded_size = info.encoded_bytes;
            fwrite(&encoded_size, 1, sizeof(encoded_size), fp);
            fwrite(out_buf.data(), 1, info.encoded_bytes, fp);
        }
        input += samples;
        offset += samples;
    }

    fclose(fp);

    return 0;
}

/********************************** END **********************************************/
