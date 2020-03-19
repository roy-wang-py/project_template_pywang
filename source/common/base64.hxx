/**
 * @file base64.hxx
 * @author sqhao (sqhao@aibee.com)
 * @brief 
 * @version 0.1
 * @date 2019-09-23
 * 
 * @copyright Copyright (c) 2019
 * 
*/

#ifndef __BASE64_HXX__
#define __BASE64_HXX__

#include <string>
#include "default_config.hxx"

NAMESPACE_PREFIX
    class Base64{
        private:
            static const std::string  base64_chars_;
            static bool is_base64(unsigned char c) {
                return (isalnum(c) || (c == '+') || (c == '/'));
            }

        public:
            static std::string encode(const std::string& to_encode_string);
            static std::string decode(const std::string& encoded_string);
    };
NAMESPACE_SUBFIX


#endif // ! __BASE64_HXX__