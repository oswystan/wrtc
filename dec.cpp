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
#include "webrtc/modules/audio_coding/codecs/opus/audio_decoder_opus.h"
#include <iostream>
#include <memory>
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

    AudioDecoderOpus* decoder = new AudioDecoderOpus(1);
    vector<AudioDecoder::ParseResult> rst;
    uint32_t ts = 0;
    int16_t dec_buf[5760];

    size_t len = 0;
    int ret = 0;
    int count = 0;
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

        rtc::Buffer buf(count);
        buf.SetData(ptr, count);
        rst = decoder->ParsePayload(std::move(buf), ts);
        ts += 10;
        cout << rst.size() << endl;
        if(rst.size() > 0) {
            memset(dec_buf, 0x00, sizeof(dec_buf));
            auto frame = std::move(rst[0].frame);
            auto dec_rst = frame->Decode(rtc::ArrayView<int16_t>(dec_buf, 5760));
            if(dec_rst) {
                cout << "samples:" << dec_rst->num_decoded_samples << "; sptype: " << dec_rst->speech_type << endl;
                if(dec_rst->num_decoded_samples > 0) {
                    fwrite(dec_buf, 2, dec_rst->num_decoded_samples, fp);
                }
            }
        }

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

