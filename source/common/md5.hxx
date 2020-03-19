/**
 * @file md5.hxx
 * @author sqhao (sqhao@aibee.com)
 * @brief 
 * @version 0.1
 * @date 2019-08-23
 * 
 * @copyright Copyright (c) 2019
 * 
*/

#ifndef __MD5_HXX__
#define __MD5_HXX__

#include <string.h>
#include <string>
#include <fstream>
#include "default_config.hxx"

NAMESPACE_PREFIX
    typedef unsigned char byte;
    typedef unsigned int uint32;
    using std::string;
    using std::ifstream;

    class MD5 {
        enum{ MD5_SIZE = 32,};
        public:
            MD5(){
                reset();
            }
            MD5(const void *input, size_t length){
                reset();
                update(input,length);
            }
            MD5(const string &str){
                reset();
                update(str);
            }
            MD5(ifstream &in){
                reset();
                update(in);
            }
            void update(const void *input, size_t length);
            void update(const string &str);
            void update(ifstream &in);
            const byte* digest();
            string toString();
            void reset();
        private:
            void update(const byte *input, size_t length);
            void final();
            void transform(const byte block[64]);
            void encode(const uint32 *input, byte *output, size_t length);
            void decode(const byte *input, uint32 *output, size_t length);
            string bytesToHexString(const byte *input, size_t length);

            /* class uncopyable */
            MD5(const MD5&);
            MD5& operator=(const MD5&);
        private:
            uint32 _state[4];   /* state (ABCD) */
            uint32 _count[2];   /* number of bits, modulo 2^64 (low-order word first) */
            byte _buffer[64];   /* input buffer */
            byte _digest[16];   /* message digest */
            bool _finished;     /* calculate finished ? */

            static const byte PADDING[64];  /* padding for calculate */
            static const char HEX[16];
            static const size_t BUFFER_SIZE = 1024;
    };

NAMESPACE_SUBFIX
#endif //! __MD5_HXX__

