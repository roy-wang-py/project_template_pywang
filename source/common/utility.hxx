/**
 * @file Utility.hxx
 * @author sqhao (sqhao@aibee.com)
 * @brief 
 * @version 0.1
 * @date 2019-06-13
 * 
 * @copyright Copyright (c) 2019
 * 
*/
#ifndef __SQHAO_UTILITY_HXX__
#define __SQHAO_UTILITY_HXX__
#include <string>
 #include <typeinfo>
#include <type_traits>
#include "default_config.hxx"
#include "String.hxx"

#define TOSTRING(x) DEF_NAMESPACE::Utility::ToString(x)


#define CONVERTTO(type,x) DEF_NAMESPACE::Utility::ConvertTo<type>(x)
#define UPPER(x) DEF_NAMESPACE::toUpper(x)

NAMESPACE_PREFIX
namespace Utility{
    template<typename T>
    inline std::string ToString(T t){
        return std::to_string(t);
    }

    inline std::string ToString(bool b){
        return b ? "true" : "false";
    }
    inline std::string ToString(const std::string& s){
        return s;
    }
    inline std::string ToString(char* c){
        return c ? std::string(c) : ""; 
    }
    inline std::string ToString(const char* c){
        return c ? std::string(c) : ""; 
    }
    inline std::string ToString(const unsigned char* c){
        return c ? std::string(reinterpret_cast<const char*>(c)) : ""; 
    }


    template<typename R>
    R ConvertTo(const std::string&src){
        std::stringstream os{src};
        R r;
        os >>r;
        return r;
    }
    enum  ESTATUS_TAG {
        SUCCESS_FAILURE,
        ENABLE_DISABLE,
        TRUE_FALSE,
        YES_NO,
        ON_OFFLINE,
    };


    template<int N>
    class sstatus_tostring_helper;

    template<>
    class sstatus_tostring_helper<SUCCESS_FAILURE>{
        protected:
        virtual ~sstatus_tostring_helper()=default;
        static std::string to_string(bool b){
            return b?"success":"failure";
        }
    };
    template<>
    class sstatus_tostring_helper<ENABLE_DISABLE>{
        protected:
        virtual ~sstatus_tostring_helper()=default;
        static std::string to_string(bool b){
            return b?"enable":"disable";
        }
    };
    template<>
    class sstatus_tostring_helper<TRUE_FALSE>{
        protected:
        virtual ~sstatus_tostring_helper()=default;
        static std::string to_string(bool b){
            return b?"true":"false";
        }
    };
    template<>
    class sstatus_tostring_helper<YES_NO>{
        protected:
        virtual ~sstatus_tostring_helper()=default;
        static std::string to_string(bool b){
            return b?"yes":"no";
        }
    };
    template<>
    class sstatus_tostring_helper<ON_OFFLINE>{
        protected:
        virtual ~sstatus_tostring_helper()=default;
        static std::string to_string(bool b){
            return b?"online":"offline";
        }
    };

    template<int N>
    class SStatus : public sstatus_tostring_helper<N>{
        private:
            typedef sstatus_tostring_helper<N> helper;
            bool status_{false};
        public:
            SStatus(bool s = false):status_(s){}
            template<int T>
            SStatus(const SStatus<T> &s):status_(s.status_){}
            template<int T>
            SStatus & operator = (const SStatus<T> &s){
                if(this == &s)
                    return *this;
                status_ = s.status_;
                return *this;
            }
            inline SStatus& operator=(bool b){
                status_ = b;
                return *this;
            }

            inline void status(bool b){status_ = b;}
            inline bool status(void)const {return status_;}
            inline friend std::ostream&operator<<(std::ostream&out,const SStatus&ss){
                    return out<<ss.to_string();    
            }
            std::string to_string()const{
                return helper::to_string(status_);
            }
            friend std::istream&operator>>(std::istream&in,SStatus&ss){
                ss.status(true);
                std::string in_str;
                in >> in_str;
                if(in_str.empty() || in_str == ""){
                    ss.status(false);
                    return in;
                }

                toUpperInPlace(in_str);
                if (in_str == std::string("FALSE") || in_str == std::string("F") ||
			        in_str == std::string("NO") || in_str == std::string("N") ||
			        in_str == std::string("0") || in_str == std::string("NONE") || 
                    in_str == "DISABLE" || in_str == "OFFLINE" || in_str == "FAILURE")
                        ss.status(false);
                return in;
            }
            inline operator bool()const {
                return status_;
            }
            inline bool operator !()const {
                return !status_;
            }
    };
}
typedef Utility::SStatus<Utility::ENABLE_DISABLE>   status_enable_disable;
typedef Utility::SStatus<Utility::SUCCESS_FAILURE>   status_success_failure;
typedef Utility::SStatus<Utility::TRUE_FALSE>   status_true_false;
typedef Utility::SStatus<Utility::YES_NO>   status_yes_no;
typedef Utility::SStatus<Utility::ON_OFFLINE>   status_online_offline;

NAMESPACE_SUBFIX
#endif //! __SQHAO_UTILITY_HXX__