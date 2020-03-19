#ifndef __CONFIGOPERATION_H_
#define __CONFIGOPERATION_H_

#include <string>  
#include <cctype>
#include <map>  
#include <unordered_map>
#include <iostream>  
#include <fstream>  
#include <sstream>  
#include <vector>
#include "default_config.hxx"
#include "String.hxx"

NAMESPACE_PREFIX

	class CConfig
	{ 
	public:
		struct File_not_found
		{
			std::string filename;
			File_not_found(const std::string& filename_ = std::string()) : filename(filename_)
			{
			}
		};
		struct Key_not_found
		{
			std::string key;
			Key_not_found(const std::string& key_ = std::string()) : key(key_)
			{
			}
		};
	protected:
		std::string m_Delimiter;  // key  value  
		std::string m_Comment;    //  value comments  
		std::map<std::string, std::string> m_Contents;  
		std::vector<std::string> m_DefKeys;
		typedef std::map<std::string, std::string>::iterator mapi;
		typedef std::map<std::string, std::string>::const_iterator mapci;

	public:
		CConfig();
		~CConfig()=default;
		template<class T> T Read(const std::string& in_key);
		template<class T> T Read(const std::string& in_key, const T& in_value);
		template<class T> bool ReadInto(T& out_var, const std::string& in_key) const;
		template<class T> bool ReadInto(T& out_var, const std::string& in_key, const T& in_value) const;
		bool FileExist(std::string filename);
		const std::vector<std::string> &GetDefKeys(){return m_DefKeys;}

		void ReadFile(std::string filename, std::string delimiter = "=", std::string comment = "#");


		bool KeyExists(const std::string& in_key) const
		{
			mapci p = m_Contents.find(in_key); 
			return (p != m_Contents.end());
			
		}
		void  FuzzyLook(const std::string fuzzy_key,std::unordered_map<std::string,std::string> & result){
			for(auto it = m_Contents.cbegin();it != m_Contents.cend();++it){
				if(it->first.find(fuzzy_key) != std::string::npos){
					result[it->first] = it->second;
				}
			}
		}
		void  FuzzyLook(const std::string fuzzy_key,std::map<std::string,std::string> & result){
			for(auto it = m_Contents.cbegin();it != m_Contents.cend();++it){
				if(it->first.find(fuzzy_key) != std::string::npos){
					result[it->first] = it->second;
				}
			}
		}
		template<class T> 
		void Add(const std::string& in_key, const T& in_value);

		void Remove(const std::string& in_key)
		{
			m_Contents.erase(m_Contents.find(in_key));
		}

		std::string GetDelimiter() const
		{
			return m_Delimiter;
		}
		std::string GetComment() const
		{
			return m_Comment;
		}
		std::string SetDelimiter(const std::string& in_s)
		{
			std::string old = m_Delimiter;  m_Delimiter = in_s;  return old;
		}
		std::string SetComment(const std::string& in_s)
		{
			std::string old = m_Comment;  m_Comment = in_s;  return old;
		}

		friend std::ostream& operator<<(std::ostream& os, const CConfig& cf);
		friend std::istream& operator>>(std::istream& is, CConfig& cf);

	protected:
		template<class T> static std::string T_as_string(const T& t);
		template<class T> static T string_as_T(const std::string& s);
	};


	template<class T>
	std::string CConfig::T_as_string(const T& t)
	{
		std::ostringstream ost;
		ost << t;
		return ost.str();
	}

	template<class T>
	T CConfig::string_as_T(const std::string& s)
	{ 
		T t;
		std::istringstream ist(s);
		ist >> t;
		return t;
	}

	//specialization
	template<>
	inline std::string CConfig::string_as_T<std::string>(const std::string& s)
	{ 
		return s;
	}
	//bool
	template<>
	inline bool CConfig::string_as_T<bool>(const std::string& s)
	{
		// "false", "F", "no", "n", "0" as false  
		// "true", "T", "yes", "y", "1", "-1", or anything else as true  
		bool b = true;
		std::string sup = s;
		for (auto p = sup.begin(); p != sup.end(); ++p)
			*p = toupper(*p); 
		if (sup == std::string("FALSE") || sup == std::string("F") ||
			sup == std::string("NO") || sup == std::string("N") ||
			sup == std::string("0") || sup == std::string("NONE"))
			b = false;
		return b;
	}


	template<class T>
	T CConfig::Read(const std::string& key)
	{ 
		mapci p = m_Contents.find(key);
		if (p == m_Contents.end()) throw Key_not_found(key);
		if(p->second.empty())
			m_DefKeys.push_back(key);
		return string_as_T<T>(p->second);
	}


	template<class T>
	T CConfig::Read(const std::string& key, const T& value)
	{
	
		mapci p = m_Contents.find(key);
		if (p == m_Contents.end()||p->second.empty()){
			m_DefKeys.push_back(key);
			return value;
		} 
		return string_as_T<T>(p->second);
	}


	template<class T>
	bool CConfig::ReadInto(T& var, const std::string& key) const
	{
		mapci p = m_Contents.find(key);
		bool found = (p != m_Contents.end());
		if (found) var = string_as_T<T>(p->second);
		return found;
	}


	template<class T>
	bool CConfig::ReadInto(T& var, const std::string& key, const T& value) const
	{
		mapci p = m_Contents.find(key);
		bool found = (p != m_Contents.end());
		if (found)
			var = string_as_T<T>(p->second);
		else
			var = value;
		return found;
	}


	template<class T>
	void CConfig::Add(const std::string& in_key, const T& value)
	{
		std::string v = T_as_string(value);
		std::string key = in_key;
		trimInPlace(key);
		trimInPlace(v);
		m_Contents[key] = v;
		return;
	}
NAMESPACE_SUBFIX
#endif
