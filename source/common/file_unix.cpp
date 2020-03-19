
#include "file_unix.hxx"
#include "path_opt.hxx"
#include "Exception.hxx"
#include "buffer.hxx"
#include <algorithm>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>

#include <sys/statfs.h>

#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <utime.h>
#include <cstring>


#define STATFSFN statfs
#define STATFSSTRUCT statfs


NAMESPACE_PREFIX

bool FileImpl::canReadImpl() const{
	struct stat st;
	if (stat(_path.c_str(), &st) == 0)
	{
		if (st.st_uid == geteuid())
			return (st.st_mode & S_IRUSR) != 0;
		else if (st.st_gid == getegid())
			return (st.st_mode & S_IRGRP) != 0;
		else
			return (st.st_mode & S_IROTH) != 0 || geteuid() == 0;
	}
	else handleLastErrorImpl(_path);
	return false;
}


bool FileImpl::canWriteImpl() const{
	struct stat st;
	if (stat(_path.c_str(), &st) == 0)
	{
		if (st.st_uid == geteuid())
			return (st.st_mode & S_IWUSR) != 0;
		else if (st.st_gid == getegid())
			return (st.st_mode & S_IWGRP) != 0;
		else
			return (st.st_mode & S_IWOTH) != 0 || geteuid() == 0;
	}
	else handleLastErrorImpl(_path);
	return false;
}


bool FileImpl::canExecuteImpl() const{
	struct stat st;
	if (stat(_path.c_str(), &st) == 0)
	{
		if (st.st_uid == geteuid() || geteuid() == 0)
			return (st.st_mode & S_IXUSR) != 0;
		else if (st.st_gid == getegid())
			return (st.st_mode & S_IXGRP) != 0;
		else
			return (st.st_mode & S_IXOTH) != 0;
	}
	else handleLastErrorImpl(_path);
	return false;
}


bool FileImpl::isFileImpl() const{
	struct stat st;
	if (stat(_path.c_str(), &st) == 0)
		return S_ISREG(st.st_mode);
	else
		handleLastErrorImpl(_path);
	return false;
}


bool FileImpl::isDirectoryImpl() const{
	struct stat st;
	if (stat(_path.c_str(), &st) == 0)
		return S_ISDIR(st.st_mode);
	else
		handleLastErrorImpl(_path);
	return false;
}


bool FileImpl::isLinkImpl() const{
	struct stat st;
	if (lstat(_path.c_str(), &st) == 0)
		return S_ISLNK(st.st_mode);
	else
		handleLastErrorImpl(_path);
	return false;
}


bool FileImpl::isDeviceImpl() const{
	struct stat st;
	if (stat(_path.c_str(), &st) == 0)
		return S_ISCHR(st.st_mode) || S_ISBLK(st.st_mode);
	else
		handleLastErrorImpl(_path);
	return false;
}


bool FileImpl::isHiddenImpl() const{
	Path p(_path);
	p.makeFile();

	return p.getFileName()[0] == '.';
}


Timestamp FileImpl::createdImpl() const{
	struct stat st;
	if (stat(_path.c_str(), &st) == 0)
		return Timestamp::fromEpochTime(st.st_ctime);
	else
		handleLastErrorImpl(_path);
	return 0;
}


Timestamp FileImpl::getLastModifiedImpl() const{
	struct stat st;
	if (stat(_path.c_str(), &st) == 0)
		return Timestamp::fromEpochTime(st.st_mtime);
	else
		handleLastErrorImpl(_path);
	return 0;
}


void FileImpl::setLastModifiedImpl(const Timestamp& ts){
	struct utimbuf tb;
	tb.actime  = ts.epochTime();
	tb.modtime = ts.epochTime();
	if (utime(_path.c_str(), &tb) != 0)
		handleLastErrorImpl(_path);
}


FileImpl::FileSizeImpl FileImpl::getSizeImpl() const{
	struct stat st;
	if (stat(_path.c_str(), &st) == 0)
		return st.st_size;
	else
		handleLastErrorImpl(_path);
	return 0;
}


void FileImpl::setSizeImpl(FileSizeImpl size){
	if (truncate(_path.c_str(), size) != 0)
		handleLastErrorImpl(_path);
}


void FileImpl::setWriteableImpl(bool flag){
	struct stat st;
	if (stat(_path.c_str(), &st) != 0)
		handleLastErrorImpl(_path);
	mode_t mode;
	if (flag)
	{
		mode = st.st_mode | S_IWUSR;
	}
	else
	{
		mode_t wmask = S_IWUSR | S_IWGRP | S_IWOTH;
		mode = st.st_mode & ~wmask;
	}
	if (chmod(_path.c_str(), mode) != 0)
		handleLastErrorImpl(_path);
}


void FileImpl::setExecutableImpl(bool flag){

	struct stat st;
	if (stat(_path.c_str(), &st) != 0)
		handleLastErrorImpl(_path);
	mode_t mode;
	if (flag)
	{
		mode = st.st_mode | S_IXUSR;
		if (st.st_mode & S_IRGRP)
			mode |= S_IXGRP;
		if (st.st_mode & S_IROTH)
			mode |= S_IXOTH;
	}
	else
	{
		mode_t wmask = S_IXUSR | S_IXGRP | S_IXOTH;
		mode = st.st_mode & ~wmask;
	}
	if (chmod(_path.c_str(), mode) != 0)
		handleLastErrorImpl(_path);
}


void FileImpl::copyToImpl(const std::string& path) const{
	int sd = open(_path.c_str(), O_RDONLY);
	if (sd == -1) handleLastErrorImpl(_path);

	struct stat st;
	if (fstat(sd, &st) != 0)
	{
		close(sd);
		handleLastErrorImpl(_path);
	}
	const long blockSize = st.st_blksize;

	int dd = open(path.c_str(), O_CREAT | O_TRUNC | O_WRONLY, st.st_mode);
	if (dd == -1)
	{
		close(sd);
		handleLastErrorImpl(path);
	}
	Buffer<char> buffer(blockSize);
	try
	{
		int n;
		while ((n = read(sd, buffer.begin(), blockSize)) > 0)
		{
			if (write(dd, buffer.begin(), n) != n)
				handleLastErrorImpl(path);
		}
		if (n < 0)
			handleLastErrorImpl(_path);
	}
	catch (...)
	{
		close(sd);
		close(dd);
		throw;
	}
	close(sd);
	if (fsync(dd) != 0)
	{
		close(dd);
		handleLastErrorImpl(path);
	}
	if (close(dd) != 0)
		handleLastErrorImpl(path);
}

void FileImpl::fillImpl(){
	int sd = open(_path.c_str(),   O_WRONLY);
	if (sd == -1) handleLastErrorImpl(_path);

	struct stat st;
	if (fstat(sd, &st) != 0)
	{
		close(sd);
		handleLastErrorImpl(_path);
	}
	
	long count_size =st.st_size;
	const long blockSize = st.st_blksize < count_size ? st.st_blksize: count_size;
	int dd = open("/dev/zero",O_RDONLY);
	if (dd == -1)
	{
		close(sd);
		handleLastErrorImpl("open /dev/zero fail");
	}
	Buffer<char> buffer(blockSize);
	try
	{
		int n = 0;
		while ((count_size > 0 &&(n = read(dd, buffer.begin(), blockSize)) > 0))
		{
			count_size -= n;
			if (write(sd, buffer.begin(), n) != n)
				handleLastErrorImpl("write"+_path+"faile");
		}
		if (n < 0)
			handleLastErrorImpl(_path);
	}
	catch (...)
	{
		close(sd);
		close(dd);
		throw;
	}
	close(dd);
	if (fsync(sd) != 0)
	{
		close(sd);
		handleLastErrorImpl(_path);
	}
	if (close(sd) != 0)
		handleLastErrorImpl(_path);
}

void FileImpl::renameToImpl(const std::string& path){
	if (rename(_path.c_str(), path.c_str()) != 0)
		handleLastErrorImpl(_path);
}


void FileImpl::linkToImpl(const std::string& path, int type) const{

	if (type == 0)
	{
		if (link(_path.c_str(), path.c_str()) != 0)
			handleLastErrorImpl(_path);
	}
	else
	{
		if (symlink(_path.c_str(), path.c_str()) != 0)
			handleLastErrorImpl(_path);
	}
}


void FileImpl::removeImpl(){
	int rc;
	if (!isLinkImpl() && isDirectoryImpl())
		rc = rmdir(_path.c_str());
	else
		rc = unlink(_path.c_str());
	if (rc) handleLastErrorImpl(_path);
}


bool FileImpl::createFileImpl(){
	int n = open(_path.c_str(), O_WRONLY | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
	if (n != -1)
	{
		close(n);
		return true;
	}
	if (n == -1 && errno == EEXIST)
		return false;
	else
		handleLastErrorImpl(_path);
	return false;
}


bool FileImpl::createDirectoryImpl(){
	if (existsImpl() && isDirectoryImpl())
		return false;
	if (mkdir(_path.c_str(), S_IRWXU | S_IRWXG | S_IRWXO) != 0)
		handleLastErrorImpl(_path);
	return true;
}


FileImpl::FileSizeImpl FileImpl::totalSpaceImpl() const{

	struct STATFSSTRUCT stats;
	if (STATFSFN(const_cast<char*>(_path.c_str()), &stats) != 0)
		handleLastErrorImpl(_path);

	return (FileSizeImpl)stats.f_blocks * (FileSizeImpl)stats.f_bsize;
}


FileImpl::FileSizeImpl FileImpl::usableSpaceImpl() const{
	struct STATFSSTRUCT stats;
	if (STATFSFN(const_cast<char*>(_path.c_str()), &stats) != 0)
		handleLastErrorImpl(_path);

	return (FileSizeImpl)stats.f_bavail * (FileSizeImpl)stats.f_bsize;
}


FileImpl::FileSizeImpl FileImpl::freeSpaceImpl() const{
	struct STATFSSTRUCT stats;
	if (STATFSFN(const_cast<char*>(_path.c_str()), &stats) != 0)
		handleLastErrorImpl(_path);

	return (FileSizeImpl)stats.f_bfree * (FileSizeImpl)stats.f_bsize;
}


void FileImpl::handleLastErrorImpl(const std::string& path){
	switch (errno)
	{
	case EIO:
		throw IOException(path, errno);
	case EPERM:
		throw FileAccessDeniedException("insufficient permissions", path, errno);
	case EACCES:
		throw FileAccessDeniedException(path, errno);
	case ENOENT:
		throw FileNotFoundException(path, errno);
	case ENOTDIR:
		throw OpenFileException("not a directory", path, errno);
	case EISDIR:
		throw OpenFileException("not a file", path, errno);
	case EROFS:
		throw FileReadOnlyException(path, errno);
	case EEXIST:
		throw FileExistsException(path, errno);
	case ENOSPC:
		throw FileException("no space left on device", path, errno);
	case EDQUOT:
		throw FileException("disk quota exceeded", path, errno);
	case ENOTEMPTY:
		throw DirectoryNotEmptyException(path, errno);
	case ENAMETOOLONG:
		throw PathSyntaxException(path, errno);
	case ENFILE:
	case EMFILE:
		throw FileException("too many open files", path, errno);
	default:
		throw FileException(strerror(errno), path, errno);
	}
}


NAMESPACE_SUBFIX
