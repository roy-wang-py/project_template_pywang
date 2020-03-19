/**
 * @file file_format_input.cpp
 * @author sqhao (sqhao@aibee.com)
 * @brief 
 * @version 0.1
 * @date 2019-06-20
 * 
 * @copyright Copyright (c) 2019
 * 
*/

#include <iterator>
#include <string>
#include "file_opt.hxx"
#include "path_opt.hxx"
#include "log_wrapper.hxx"
#include "Exception.hxx"
#include "bugcheck.hxx"
#include "stringtokenizer.hxx"
#include "file_format_input.hxx"

NAMESPACE_PREFIX
    using std::string;
    
    File_Input::File_Input(const string&filename,const std::string& separators){
        DEF_NAMESPACE::Path p{filename};
        p.makeAbsolute();
        DEF_NAMESPACE::File fi{p};
        if(!fi.exists() || !fi.isFile())
            throw FileException(fi.path()
            +" no exist or not file");
        ReadFile(fi.path(),separators);
    }
    void File_Input::ReadFile(const string&fn,const std::string& separators){
        std::ifstream is(fn.c_str());
        if(!is) 
            throw FileException("open "+ fn+" fail...");

		while (is) {
			string line;
            do{
                std::getline(is, line);
                //去掉注释  
                line = line.substr(0, line.find("#"));
            }
            while(is && line.empty());
            if(line.empty())
                break;
			StringTokenizer token{line,separators,1};
            v_t v;
            for_each(token.begin(),token.end(),[&v](const v_v_t&it)
                {v.emplace_back(it);});
            content_.emplace_back(std::move(v));
		}
    }
NAMESPACE_SUBFIX