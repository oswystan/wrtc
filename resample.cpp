/*
 **************************************************************************************
 *       Filename:  resample.cpp
 *    Description:   source file
 *
 *        Version:  1.0
 *        Created:  2018-03-27 11:15:26
 *
 *       Revision:  initial draft;
 **************************************************************************************
 */

#include <iostream>
#include "webrtc/common_audio/resampler/include/resampler.h"
#include "log.h"
using namespace std;
using namespace webrtc;

const int SAMPLE_RATE_48K = 48000;
const int SAMPLE_RATE_16K = 16000;
const int CHANNEL_MONO    = 1;
const int CHANNEL_STEREO  = 1;
const int BUF_SIZE = 512*1024;

int main()
{
    int16_t* audio_in  = new int16_t[BUF_SIZE];
    int16_t* audio_out = new int16_t[BUF_SIZE];
    if (!audio_in || !audio_out) {
        loge("fail to allocate buffer");
        return -ENOMEM;
    }
    size_t fsize = 0;
    size_t outlen = 0;
    Resampler rs;

    FILE* fpout = fopen("rs.pcm", "w");
    FILE* fpin = fopen("out.pcm", "rb");
    if (!fpin || !fpout) {
        loge("fail to open file");
        goto out;
    }

    fsize = fread(audio_in, sizeof(uint16_t), BUF_SIZE, fpin);
    if (fsize == 0) {
        loge("fail to read file");
        goto out;
    }

    rs.Reset(SAMPLE_RATE_48K, SAMPLE_RATE_16K, CHANNEL_MONO);
    rs.Push(audio_in, fsize, audio_out, BUF_SIZE, outlen);
    fwrite(audio_out, sizeof(int16_t), outlen, fpout);


    goto out;
out:
    if (fpout) fclose(fpout);
    if (fpin) fclose(fpin);
    delete []audio_in;
    delete []audio_out;
    return 0;
}


/********************************** END **********************************************/

