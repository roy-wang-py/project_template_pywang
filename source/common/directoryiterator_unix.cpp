
#include "directoryiterator_unix.hxx"
#include "file_opt.hxx"
#include "path_opt.hxx"
#include "bugcheck.hxx"

NAMESPACE_PREFIX


DirectoryIteratorImpl::DirectoryIteratorImpl(const std::string& path): _pDir(0), _rc(1)
{
	common_assert(!path.empty());
	Path p(path);
	p.makeFile();
	_pDir = opendir(p.toString().c_str());
	if (!_pDir) File::handleLastError(path);
	next();
}
const std::string& DirectoryIteratorImpl::next()
{
	do
	{
		struct dirent* pEntry = readdir(_pDir);
		if (pEntry)
			_current = pEntry->d_name;
		else
			_current.clear();
	}
	while (_current == "." || _current == "..");
	return _current;
}


NAMESPACE_SUBFIX
