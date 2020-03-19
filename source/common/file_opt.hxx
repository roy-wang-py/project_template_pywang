
#ifndef __SQHAO_FILE_HXX__
#define __SQHAO_FILE_HXX__


#include "timestamp.hxx"
#include <vector>
#include "bugcheck.hxx"
#include "file_unix.hxx"
NAMESPACE_PREFIX
class Path;

class File: private FileImpl
{
public:
	typedef FileSizeImpl FileSize;

	enum class LinkType
	{
		LINK_HARD     = 0, /// hard link
		LINK_SYMBOLIC = 1  /// symbolic link
	};

	File()=default;
	File(const std::string& path): FileImpl(path){}
	File(const char* path): FileImpl(std::string(path)){}
	File(const Path& path);
	File(const File& file): FileImpl(file.getPathImpl()){}
	virtual ~File()=default;
	File& operator = (const File& file){
		setPathImpl(file.getPathImpl());
		return *this;
	}
	File& operator = (const std::string& path){
		setPathImpl(path);
		return *this;
	}
	File& operator = (const char* path){
		common_check_ptr(path);
		setPathImpl(path);
		return *this;
	}
	File& operator = (const Path&path);
	void swap(File& file){
		swapImpl(file);
	}
	const std::string& path() const{
		return getPathImpl();
	}
	bool exists() const{
		return existsImpl();
	}
	bool canRead() const{
		return canReadImpl();
	}
	bool canWrite() const{
		return canWriteImpl();
	}
	bool canExecute() const{
		return canExecuteImpl();
	}
	bool isFile() const{
		return isFileImpl();
	}
	bool isLink() const{
		return isLinkImpl();
	}
	bool isDirectory() const{
		return isDirectoryImpl();
	}
	bool isDevice() const{
		return isDeviceImpl();
	}
	bool isHidden() const{
		return isHiddenImpl();
	}
	Timestamp created() const{
		return createdImpl();
	}

	Timestamp getLastModified() const{
		return getLastModifiedImpl();
	}

	File& setLastModified(const Timestamp& ts){
		setLastModifiedImpl(ts);
		return *this;
	}

	FileSize getSize() const{
		return getSizeImpl();
	}
	File& setSize(FileSize size){
		setSizeImpl(size);
		return *this;
	}
	File& setWriteable(bool flag = true){
		setWriteableImpl(flag);
		return *this;
	}
	File& setReadOnly(bool flag = true){
		setWriteableImpl(!flag);
		return *this;
	}
	File& setExecutable(bool flag = true){
		setExecutableImpl(flag);
		return *this;
	}

	void copyTo(const std::string& path) const;

	void moveTo(const std::string& path){
		copyTo(path);
		remove(true);
		setPathImpl(path);
	}
	void fill(){
		fillImpl();
	}
	void renameTo(const std::string& path){
		renameToImpl(path);
		setPathImpl(path);
	}
	void linkTo(const std::string& path, LinkType type = LinkType::LINK_SYMBOLIC) const{
		linkToImpl(path, int(type));
	}

	void remove(bool recursive = false);

	bool createFile();
		/// Creates a new, empty file in an atomic operation.
		/// Returns true if the file has been created and false
		/// if the file already exists. Throws an exception if
		/// an error occurs.

	bool createDirectory();
		/// Creates a directory. Returns true if the directory
		/// has been created and false if it already exists.
		/// Throws an exception if an error occurs.

	void createDirectories();
		/// Creates a directory (and all parent directories
		/// if necessary).

	void list(std::vector<std::string>& files) const;

	void list(std::vector<File>& files) const;
		/// Fills the vector with the names of all
		/// files in the directory.

	FileSize totalSpace() const;
		/// Returns the total size in bytes of the partition containing this path.

	FileSize usableSpace() const;
		/// Returns the number of usable free bytes on the partition containing this path.

	FileSize freeSpace() const;
		/// Returns the number of free bytes on the partition containing this path.

	bool operator == (const File& file) const;
	bool operator != (const File& file) const;
	bool operator <  (const File& file) const;
	bool operator <= (const File& file) const;
	bool operator >  (const File& file) const;
	bool operator >= (const File& file) const;

	static void handleLastError(const std::string& path);
		/// For internal use only. Throws an appropriate
		/// exception for the last file-related error.

protected:
	void copyDirectory(const std::string& path) const;
};


inline bool File::operator == (const File& file) const
{
	return getPathImpl() == file.getPathImpl();
}


inline bool File::operator != (const File& file) const
{
	return getPathImpl() != file.getPathImpl();
}


inline bool File::operator < (const File& file) const
{
	return getPathImpl() < file.getPathImpl();
}


inline bool File::operator <= (const File& file) const
{
	return getPathImpl() <= file.getPathImpl();
}


inline bool File::operator > (const File& file) const
{
	return getPathImpl() > file.getPathImpl();
}


inline bool File::operator >= (const File& file) const
{
	return getPathImpl() >= file.getPathImpl();
}


inline void swap(File& f1, File& f2)
{
	f1.swap(f2);
}


NAMESPACE_SUBFIX


#endif // __SQHAO_FILE_HXX__
