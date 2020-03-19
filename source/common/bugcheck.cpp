#include "bugcheck.hxx"
#include "Exception.hxx"
#include "log_wrapper.hxx"

#include <sstream>


NAMESPACE_PREFIX


void Bugcheck::assertion(const char* cond, const char* file, int line, const char* text)
{
	std::string message = what(cond, file, line, text);
	LOG_A()<<"Assertion violation: "<<message;
	throw AssertionViolationException(message);
}
void Bugcheck::assertion(const char* cond, const char* file, int line, const std::string& text){
	std::string message = what(cond, file, line, text);
	LOG_A()<<"Assertion violation: "<<message;
	throw AssertionViolationException(message);
}


void Bugcheck::nullPointer(const char* ptr, const char* file, int line)
{
	std::string message =what(ptr, file, line);
	LOG_A()<<std::string("NULL pointer: ")<<message;
	throw NullPointerException(message);
}


void Bugcheck::bugcheck(const char* file, int line)
{
	std::string message = what(0, file, line);
	LOG_A()<<"Bugcheck"<<message;
	throw BugcheckException(message);
}


void Bugcheck::bugcheck(const char* msg, const char* file, int line)
{
	std::string message = what(msg, file, line);
	LOG_A()<<"Bugcheck"<<message;
	throw BugcheckException(message);
}
void Bugcheck::bugcheck(const std::string& msg, const char* file, int line)
{
	std::string message = what(msg, file, line);
	LOG_A()<<"Bugcheck"<<message;
	throw BugcheckException(message);
}

void Bugcheck::unexpected(const char* file, int line)
{
	try
	{
		std::string msg("Unexpected exception in noexcept function or destructor: ");
		try
		{
			throw;
		}
		catch (DEF_NAMESPACE::Exception& exc)
		{
			msg += exc.displayText();
		}
		catch (std::exception& exc)
		{
			msg += exc.what();
		}
		catch (...)
		{
			msg += "unknown exception";
		}
		std::string message = what(msg,file,line);
		LOG_A()<<message;
	}
	catch (...)
	{
	}
}


void Bugcheck::debugger(const char* file, int line)
{
	LOG_A()<<"debugger in file\""<<file<<"\",line: "<<line;
}


void Bugcheck::debugger(const char* msg, const char* file, int line)
{
	LOG_A()<<"debugger "<<msg<< "in file\""<<file<<"\",line: "<<line;
}
void Bugcheck::debugger(const std::string& msg, const char* file, int line)
{
	LOG_A()<<"debugger "<<msg<< "in file\""<<file<<"\",line: "<<line;
}

std::string Bugcheck::what(const char* msg, const char* file, int line, const char* text)
{
	std::ostringstream str;
	if (msg) str << msg << " ";
   if (text != NULL) str << "(" << text << ") ";
	str << "in file \"" << file << "\", line " << line;
	return str.str();
}
std::string Bugcheck::what(const char* msg, const char* file, int line, const std::string& text)
{
	std::ostringstream str;
	if (msg) str << msg << " ";
	str << "(" << text << ") ";
	str << "in file \"" << file << "\", line " << line;
	return str.str();
}
std::string Bugcheck::what(const std::string& msg, const char* file, int line, const char* text)
{
	std::ostringstream str;
	str << msg << " ";
   	if (text != NULL) str << "(" << text << ") ";
	str << "in file \"" << file << "\", line " << line;
	return str.str();
}
std::string Bugcheck::what(const std::string& msg, const char* file, int line, const std::string&text)
{
	std::ostringstream str;
	str << msg << " ";
   	str << "(" << text << ") ";
	str << "in file \"" << file << "\", line " << line;
	return str.str();
}
NAMESPACE_SUBFIX
