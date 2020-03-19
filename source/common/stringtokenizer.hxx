#ifndef __SQHAO_STRINGTOKENIZER_HXX__
#define __SQHAO_STRINGTOKENIZER_HXX__

#include "Exception.hxx"
#include <vector>
#include <cstddef>
#include <algorithm>


NAMESPACE_PREFIX

class  StringTokenizer
{
public:
	enum  Options
	{
		TOK_IGNORE_EMPTY = 1, /// ignore empty tokens
		TOK_TRIM	 = 2  /// remove leading and trailing whitespace from tokens
	};
	
	typedef std::vector<std::string> TokenVec;
	typedef TokenVec::const_iterator Iterator;
	
	StringTokenizer(const std::string& str, const std::string& separators, int options = 0);

	~StringTokenizer()= default;
		/// Destroys the tokenizer.
	
	Iterator begin() const{return _tokens.begin();}
	Iterator end() const{return _tokens.end();}
	
	const std::string& operator [] (std::size_t index) const{
		if (index >= _tokens.size()) throw RangeException();
		return _tokens[index];
	}
	std::string& operator [] (std::size_t index){
		if (index >= _tokens.size()) throw RangeException();
		return _tokens[index];
	}
	bool has(const std::string& token) const{
		Iterator it = std::find(_tokens.begin(), _tokens.end(), token);
		return it != _tokens.end();
	}


	std::string::size_type find(const std::string& token, std::string::size_type pos = 0) const;
		/// Returns the index of the first occurence of the token
		/// starting at position pos.
		/// Throws a NotFoundException if the token is not found.

	std::size_t replace(const std::string& oldToken, const std::string& newToken, std::string::size_type pos = 0);
		/// Starting at position pos, replaces all subsequent tokens having value 
		/// equal to oldToken with newToken.
		/// Returns the number of modified tokens.
		
	std::size_t count() const{
		return _tokens.size();
	}
	std::size_t count(const std::string& token) const;

private:
	StringTokenizer(const StringTokenizer&)=delete;
	StringTokenizer& operator = (const StringTokenizer&)=delete;
	
	void trim(std::string& token);

	TokenVec _tokens;
};

NAMESPACE_SUBFIX


#endif // __SQHAO_STRINGTOKENIZER_HXX__
