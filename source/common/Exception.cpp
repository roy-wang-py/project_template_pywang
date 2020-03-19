
#include "Exception.hxx"
#include <typeinfo>


NAMESPACE_PREFIX

Exception::Exception(const std::string& msg, const std::string& arg, int code): _msg(msg), _pNested(0), _code(code)
{
	if (!arg.empty())
	{
		_msg.append(": ");
		_msg.append(arg);
	}
}


Exception::Exception(const std::string& msg, const Exception& nested, int code): _msg(msg), _pNested(nested.clone()), _code(code)
{
}


Exception::Exception(const Exception& exc):
	std::exception(exc),
	_msg(exc._msg),
	_code(exc._code)
{
	_pNested = exc._pNested ? exc._pNested->clone() : 0;
}

	
Exception::~Exception() throw()
{
	delete _pNested;
}


Exception& Exception::operator = (const Exception& exc)
{
	if (&exc != this)
	{
		Exception* newPNested = exc._pNested ? exc._pNested->clone() : 0;
		delete _pNested;
		_msg     = exc._msg;
		_pNested = newPNested;
		_code    = exc._code;
	}
	return *this;
}


const char* Exception::className() const throw()
{
	return typeid(*this).name();
}

	
const char* Exception::what() const throw()
{
	return name();
}

	
std::string Exception::displayText() const
{
	std::string txt = name();
	if (!_msg.empty())
	{
		txt.append(": ");
		txt.append(_msg);
	}
	return txt;
}


void Exception::extendedMessage(const std::string& arg)
{
	if (!arg.empty())
	{
		if (!_msg.empty()) _msg.append(": ");
		_msg.append(arg);
	}
}


Exception* Exception::clone() const
{
	return new Exception(*this);
}


void Exception::rethrow() const
{
	throw *this;
}


IMPLEMENT_EXCEPTION(LogicException, Exception, "Logic exception")
IMPLEMENT_EXCEPTION(AssertionViolationException, LogicException, "Assertion violation")
IMPLEMENT_EXCEPTION(NullPointerException, LogicException, "Null pointer")
IMPLEMENT_EXCEPTION(NullValueException, LogicException, "Null value")
IMPLEMENT_EXCEPTION(BugcheckException, LogicException, "Bugcheck")
IMPLEMENT_EXCEPTION(InvalidArgumentException, LogicException, "Invalid argument")
IMPLEMENT_EXCEPTION(NotImplementedException, LogicException, "Not implemented")
IMPLEMENT_EXCEPTION(RangeException, LogicException, "Out of range")
IMPLEMENT_EXCEPTION(IllegalStateException, LogicException, "Illegal state")
IMPLEMENT_EXCEPTION(InvalidAccessException, LogicException, "Invalid access")
IMPLEMENT_EXCEPTION(SignalException, LogicException, "Signal received")
IMPLEMENT_EXCEPTION(UnhandledException, LogicException, "Unhandled exception")

IMPLEMENT_EXCEPTION(RuntimeException, Exception, "Runtime exception")
IMPLEMENT_EXCEPTION(NotFoundException, RuntimeException, "Not found")
IMPLEMENT_EXCEPTION(ExistsException, RuntimeException, "Exists")
IMPLEMENT_EXCEPTION(TimeoutException, RuntimeException, "Timeout")
IMPLEMENT_EXCEPTION(SystemException, RuntimeException, "System exception")
IMPLEMENT_EXCEPTION(RegularExpressionException, RuntimeException, "Error in regular expression")
IMPLEMENT_EXCEPTION(LibraryLoadException, RuntimeException, "Cannot load library")
IMPLEMENT_EXCEPTION(LibraryAlreadyLoadedException, RuntimeException, "Library already loaded")
IMPLEMENT_EXCEPTION(NoThreadAvailableException, RuntimeException, "No thread available")
IMPLEMENT_EXCEPTION(PropertyNotSupportedException, RuntimeException, "Property not supported")
IMPLEMENT_EXCEPTION(PoolOverflowException, RuntimeException, "Pool overflow")
IMPLEMENT_EXCEPTION(NoPermissionException, RuntimeException, "No permission")
IMPLEMENT_EXCEPTION(OutOfMemoryException, RuntimeException, "Out of memory")
IMPLEMENT_EXCEPTION(DataException, RuntimeException, "Data error")

IMPLEMENT_EXCEPTION(DataFormatException, DataException, "Bad data format")
IMPLEMENT_EXCEPTION(SyntaxException, DataException, "Syntax error")
IMPLEMENT_EXCEPTION(CircularReferenceException, DataException, "Circular reference")
IMPLEMENT_EXCEPTION(PathSyntaxException, SyntaxException, "Bad path syntax")
IMPLEMENT_EXCEPTION(IOException, RuntimeException, "I/O error")
IMPLEMENT_EXCEPTION(ProtocolException, IOException, "Protocol error")
IMPLEMENT_EXCEPTION(FileException, IOException, "File access error")
IMPLEMENT_EXCEPTION(FileExistsException, FileException, "File exists")
IMPLEMENT_EXCEPTION(FileNotFoundException, FileException, "File not found")
IMPLEMENT_EXCEPTION(PathNotFoundException, FileException, "Path not found")
IMPLEMENT_EXCEPTION(FileReadOnlyException, FileException, "File is read-only")
IMPLEMENT_EXCEPTION(FileAccessDeniedException, FileException, "Access to file denied")
IMPLEMENT_EXCEPTION(CreateFileException, FileException, "Cannot create file")
IMPLEMENT_EXCEPTION(OpenFileException, FileException, "Cannot open file")
IMPLEMENT_EXCEPTION(WriteFileException, FileException, "Cannot write file")
IMPLEMENT_EXCEPTION(ReadFileException, FileException, "Cannot read file")
IMPLEMENT_EXCEPTION(DirectoryNotEmptyException, FileException, "Directory not empty")
IMPLEMENT_EXCEPTION(UnknownURISchemeException, RuntimeException, "Unknown URI scheme")
IMPLEMENT_EXCEPTION(TooManyURIRedirectsException, RuntimeException, "Too many URI redirects")
IMPLEMENT_EXCEPTION(URISyntaxException, SyntaxException, "Bad URI syntax")

IMPLEMENT_EXCEPTION(ApplicationException, Exception, "Application exception")
IMPLEMENT_EXCEPTION(BadCastException, RuntimeException, "Bad cast exception")


NAMESPACE_SUBFIX
