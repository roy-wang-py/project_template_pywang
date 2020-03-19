/**
 * @file file_format_input.hxx
 * @author sqhao (sqhao@aibee.com)
 * @brief 
 * @version 0.1
 * @date 2019-06-20
 * 
 * @copyright Copyright (c) 2019
 * 
*/
#ifndef __FILE_FORMAT_INPUT_HXX
#define __FILE_FORMAT_INPUT_HXX
#include <iostream>
#include <vector>
#include <string>
#include <fstream>

#include "Exception.hxx"
#include "utility.hxx"
NAMESPACE_PREFIX

class File_Input{
    private:
        std::vector<std::vector<std::string>> content_;
        using Iterator = typename decltype(content_)::iterator;
        using v_t = typename decltype(content_)::value_type;
        using v_v_t = typename v_t::value_type;
    public:
        File_Input()=default;
        File_Input(const std::string&filename,const std::string& separators=" ");
        ~File_Input()=default;

        void ReadFile(const std::string&fn,const std::string& separators = " ");

        v_t& operator [](size_t i){
            if(i >= content_.size())
                throw OutOfMemoryException("File_Input access out memory,size:" \
                    +TOSTRING(content_.size() \
                    +",access input:" \
                    +TOSTRING(i)));
            return content_.at(i);
        }
        const v_t& operator [](size_t i)const{
            if(i >= content_.size())
                throw OutOfMemoryException("File_Input access out memory,size:" \
                    +TOSTRING(content_.size() \
                    +",access input:" \
                    +TOSTRING(i)));
            return content_.at(i);
        }

        Iterator begin() {return content_.begin();}
	    Iterator end() {return content_.end();}
};

NAMESPACE_SUBFIX
#endif //! __FILE_FORMAT_INPUT_HXX