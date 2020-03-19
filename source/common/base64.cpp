/**
 * @file base64.cpp
 * @author sqhao (sqhao@aibee.com)
 * @brief 
 * @version 0.1
 * @date 2019-09-23
 * 
 * @copyright Copyright (c) 2019
 * 
*/

#include "base64.hxx"
#include "common_header.hxx"
#include <iterator>
NAMESPACE_PREFIX

const std::string  Base64::base64_chars_{"ABCDEFGHIJKLMNOPQRSTUVWXYZ""abcdefghijklmnopqrstuvwxyz0123456789+/"};

std::string Base64::encode(const std::string &to_encode_string){
    if(to_encode_string.empty()){
        LOG_E()<<"base64 encode input string is empty";
        return {};
    }
    std::string ret{""};
    int i = 0,j = 0;
    const unsigned char * bytes_to_encode = reinterpret_cast<const unsigned char*>(to_encode_string.c_str());
    unsigned int in_len = to_encode_string.size();
    unsigned char char_array_3[3];
    unsigned char char_array_4[4];

    while (in_len--) {
        char_array_3[i++] = *(bytes_to_encode++);
        if (i == 3) {
        char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
        char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
        char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
        char_array_4[3] = char_array_3[2] & 0x3f;

        for(i = 0; (i <4) ; i++)
            ret += base64_chars_[char_array_4[i]];
        i = 0;
        }
    }

    if (i)
    {
        for(j = i; j < 3; j++)
        char_array_3[j] = '\0';

        char_array_4[0] = ( char_array_3[0] & 0xfc) >> 2;
        char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
        char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);

        for (j = 0; (j < i + 1); j++)
        ret += base64_chars_[char_array_4[j]];

        while((i++ < 3))
        ret += '=';

    }

    return ret;

}

std::string Base64::decode(const std::string &encoded_string){
    if(encoded_string.empty()){
        LOG_E()<<"base64 decode input string is empty";
        return {};
    }
    int in_len = encoded_string.size();
    int i = 0;
    int j = 0;
    int in_ = 0;
    unsigned char char_array_4[4], char_array_3[3];
    std::string ret{""};

    while (in_len-- && ( encoded_string[in_] != '=') && is_base64(encoded_string[in_])) {
        char_array_4[i++] = encoded_string[in_]; in_++;
        if (i ==4) {
        for (i = 0; i <4; i++)
            char_array_4[i] = base64_chars_.find(char_array_4[i]);

        char_array_3[0] = ( char_array_4[0] << 2       ) + ((char_array_4[1] & 0x30) >> 4);
        char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
        char_array_3[2] = ((char_array_4[2] & 0x3) << 6) +   char_array_4[3];

        for (i = 0; (i < 3); i++)
            ret += char_array_3[i];
        i = 0;
        }
    }

    if (i) {
        for (j = 0; j < i; j++)
        char_array_4[j] = base64_chars_.find(char_array_4[j]);

        char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
        char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);

        for (j = 0; (j < i - 1); j++) ret += char_array_3[j];
    }

    return ret;

}


NAMESPACE_SUBFIX