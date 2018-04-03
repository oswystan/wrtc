/*
 **************************************************************************************
 *       Filename:  opusdec.cpp
 *    Description:   source file
 *
 *        Version:  1.0
 *        Created:  2018-03-13 17:06:09
 *
 *       Revision:  initial draft;
 **************************************************************************************
 */

#include <sys/mman.h>
#include <errno.h>
#include <arpa/inet.h>

#include "webrtc/base/helpers.h"
#include "webrtc/base/opensslidentity.h"
#include "webrtc/base/sslfingerprint.h"
#include "webrtc/modules/audio_coding/codecs/opus/opus_interface.h"
#include "webrtc/modules/audio_coding/codecs/isac/main/include/audio_decoder_isac.h"
#include <iostream>
using namespace std;
using namespace rtc;
using namespace webrtc;

uint8_t* map_file(const char* fn, size_t* size) {
    FILE* fp = fopen(fn, "r");
    if(!fp) {
        cout << "fail to open file: " << fn << endl;
        return (uint8_t*)MAP_FAILED;
    }
    fseek(fp, 0L, SEEK_END);
    long pos = ftell(fp);
    cout << pos << endl;
    *size = pos;
    uint8_t* ptr = (uint8_t*)mmap(NULL, pos, PROT_READ|PROT_WRITE, MAP_PRIVATE, fileno(fp), 0);
    fclose(fp);
    return ptr;
}

int main(int argc, char *argv[])
{
    if(argc != 2) {
        cout << "usage: " << argv[0] << " <file>" << endl;
        return -1;
    }
    auto decoder = new AudioDecoderIsac(32000);
    size_t len = 0;
    int ret = 0;
    int count = 0;
    AudioDecoder::SpeechType audio_type = AudioDecoder::SpeechType::kComfortNoise;
    uint8_t* buf = map_file(argv[1], &len);
    uint8_t* ptr = buf;
    if (buf == (uint8_t*)MAP_FAILED) {
        return -1;
    }
    FILE* fp = fopen("out.pcm", "wb");
    int16_t* outbuf = (int16_t*)malloc(len*4);
    if(!outbuf) {
        cout << "fail to alloc buffer" << endl;
        ret = -ENOMEM;
        goto exit;
    }

    while(ptr < buf + len) {
        count = *(int*)(ptr);
        //count = ntohl(count);
        cout << count << "==>";
        ptr += 4;
        ret = decoder->Decode(ptr, count, 32000, len*4, outbuf, &audio_type);
        cout << ret <<" " << audio_type << endl;
        if (ret > 0) {
            fwrite(outbuf, 2, ret, fp);
        }
        ptr += count;
    }
    fclose(fp);

exit:
    munmap(buf, len);
    return ret;
}


/********************************** END **********************************************/

