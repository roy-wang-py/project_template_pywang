/**
 * @file sha1.hxx
 * @author sqhao (sqhao@aibee.com)
 * @brief 
 * @version 0.1
 * @date 2019-10-09
 * 
 * @copyright Copyright (c) 2019
 * 
*/

#ifndef __SHA1_HXX__
#define __SHA1_HXX__


#include <cstdint>
#include <iostream>
#include <string>
#include "default_config.hxx"

NAMESPACE_PREFIX

class SHA1
{
public:
    SHA1();
    void update(const std::string &s);
    void update(std::istream &is);
    std::string final();
    static std::string from_file(const std::string &filename);

private:
    uint32_t digest[5];
    std::string buffer;
    uint64_t transforms;
};

NAMESPACE_SUBFIX

#endif 
