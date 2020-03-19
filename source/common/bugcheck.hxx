#ifndef __SQHAO_BUGCHECK_HXX__
#define __SQHAO_BUGCHECK_HXX__

#include <string>
#include <cstdlib>
#include <iostream>
#include "default_config.hxx"

NAMESPACE_PREFIX


class  Bugcheck{
public:
	static void assertion(const char* cond, const char* file, int line, const char* text = 0);
		/// An assertion failed. Break into the debugger, if
		/// possible, then throw an AssertionViolationException.
	static void assertion(const char* cond, const char* file, int line, const std::string& text);
	static void nullPointer(const char* ptr, const char* file, int line);
		/// An null pointer was encountered. Break into the debugger, if
		/// possible, then throw an NullPointerException.

	static void bugcheck(const char* file, int line);
	static void bugcheck(const std::string &msg,const char * file,int line);
	static void bugcheck(const char* msg, const char* file, int line);
	static void unexpected(const char* file, int line);
		/// An exception was caught in a destructor. Break into debugger,
		/// if possible and report exception. Must only be called from
		/// within a catch () block as it rethrows the exception to
		/// determine its class.

	static void debugger(const char* file, int line);
		/// An internal error was encountered. Break into the debugger, if
		/// possible.

	static void debugger(const char* msg, const char* file, int line);
	static void debugger(const std::string& msg, const char* file, int line);

protected:
	static std::string what(const char* msg, const char* file, int line, const char* text = 0);
	static std::string what(const std::string& msg, const char* file, int line, const char* text = 0);
	static std::string what(const char* msg, const char* file, int line, const std::string& text);
	static std::string what(const std::string& msg, const char* file, int line, const std::string&text);
};


NAMESPACE_SUBFIX


#if !defined(NODEBUG)
	#define common_assert_dbg(cond) \
		if (!(cond)) DEF_NAMESPACE::Bugcheck::assertion(#cond, __FILE__, __LINE__); else (void) 0

	#define common_assert_msg_dbg(cond, text) \
		if (!(cond)) DEF_NAMESPACE::Bugcheck::assertion(#cond, __FILE__, __LINE__, text); else (void) 0
#else
	#define common_ssert_msg_dbg(cond, text)
	#define common_assert_dbg(cond)
#endif


#define common_assert(cond) \
	if (!(cond)) DEF_NAMESPACE::Bugcheck::assertion(#cond, __FILE__, __LINE__); else (void) 0


#define common_assert_msg(cond, text) \
	if (!(cond)) DEF_NAMESPACE::Bugcheck::assertion(#cond, __FILE__, __LINE__, text); else (void) 0


#define common_check_ptr(ptr) \
	if (!(ptr)) DEF_NAMESPACE::Bugcheck::nullPointer(#ptr, __FILE__, __LINE__); else (void) 0


#define common_bugcheck() \
	DEF_NAMESPACE::Bugcheck::bugcheck(__FILE__, __LINE__)


#define common_bugcheck_msg(msg) \
	DEF_NAMESPACE::Bugcheck::bugcheck(msg, __FILE__, __LINE__)

#define common_unexpected() \
	DEF_NAMESPACE::Bugcheck::unexpected(__FILE__, __LINE__);


#define common_debugger() \
	DEF_NAMESPACE::Bugcheck::debugger(__FILE__, __LINE__)


#define common_debugger_msg(msg) \
	DEF_NAMESPACE::Bugcheck::debugger(msg, __FILE__, __LINE__)

#endif // __SQHAO_BUGCHECK_HXX__
