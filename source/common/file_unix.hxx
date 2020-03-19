
#ifndef __FILE_UNIX_HXX__
#define __FILE_UNIX_HXX__

#include <string>
#include <sys/stat.h>
#include "timestamp.hxx"
NAMESPACE_PREFIX
class FileImpl
{
	typedef unsigned long long UInt64;
protected:
	typedef UInt64  FileSizeImpl;

	FileImpl()=default;
	FileImpl(const std::string& path):_path(path){
		std::string::size_type n = _path.size();
		if (n > 1 && _path[n - 1] == '/')
			_path.resize(n - 1);
	}
	virtual ~FileImpl()=default;
	void swapImpl(FileImpl& file){
		std::swap(_path, file._path);
	}
	void setPathImpl(const std::string& path){
		_path = path;
		std::string::size_type n = _path.size();
		if (n > 1 && _path[n - 1] == '/')
			_path.resize(n - 1);
	}
	const std::string& getPathImpl() const{return _path;}
	bool existsImpl() const{
		struct stat st;
		return stat(_path.c_str(), &st) == 0;
	}
	bool canReadImpl() const;
	bool canWriteImpl() const;
	bool canExecuteImpl() const;
	bool isFileImpl() const;
	bool isDirectoryImpl() const;
	bool isLinkImpl() const;
	bool isDeviceImpl() const;
	bool isHiddenImpl() const;
	Timestamp createdImpl() const;
	Timestamp getLastModifiedImpl() const;
	void setLastModifiedImpl(const Timestamp& ts);
	FileSizeImpl getSizeImpl() const;
	void setSizeImpl(FileSizeImpl size);
	void setWriteableImpl(bool flag = true);
	void setExecutableImpl(bool flag = true);
	void copyToImpl(const std::string& path) const;
	void fillImpl();
	void renameToImpl(const std::string& path);
	void linkToImpl(const std::string& path, int type) const;
	void removeImpl();
	bool createFileImpl();
	bool createDirectoryImpl();
	FileSizeImpl totalSpaceImpl() const;
	FileSizeImpl usableSpaceImpl() const;
	FileSizeImpl freeSpaceImpl() const;
	static void handleLastErrorImpl(const std::string& path);

private:
	std::string _path;

	friend class DirectoryIteratorImpl;
	friend class LinuxDirectoryWatcherStrategy;
	friend class BSDDirectoryWatcherStrategy;
};


NAMESPACE_SUBFIX


#endif // __FILE_UNIX_HXX__
