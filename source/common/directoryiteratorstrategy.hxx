#ifndef __SQHAO_DIRECTORYITERATORSTRATEGY_HXX__
#define __SQHAO_DIRECTORYITERATORSTRATEGY_HXX__


#include "directoryiterator.hxx"
#include <stack>
#include <queue>
#include <functional>


NAMESPACE_PREFIX


class TraverseBase
{
public:
	typedef std::stack<DirectoryIterator> Stack;
	typedef std::pointer_to_unary_function<const Stack&, UInt16> DepthFunPtr;

	enum
	{
		D_INFINITE = 0 /// Special value for infinite traverse depth.
	};

	TraverseBase(DepthFunPtr depthDeterminer, UInt16 maxDepth = D_INFINITE);

protected:
	bool isFiniteDepth();
	bool isDirectory(File& file);

	DepthFunPtr _depthDeterminer;
	UInt16 _maxDepth;
	DirectoryIterator _itEnd;

private:
	TraverseBase();
	TraverseBase(const TraverseBase&);
	TraverseBase& operator=(const TraverseBase&);
};


class  ChildrenFirstTraverse: public TraverseBase
{
public:
	ChildrenFirstTraverse(DepthFunPtr depthDeterminer, UInt16 maxDepth = D_INFINITE);

	const std::string next(Stack* itStack, bool* isFinished);

private:
	ChildrenFirstTraverse();
	ChildrenFirstTraverse(const ChildrenFirstTraverse&);
	ChildrenFirstTraverse& operator=(const ChildrenFirstTraverse&);
};


class  SiblingsFirstTraverse: public TraverseBase
{
public:
	SiblingsFirstTraverse(DepthFunPtr depthDeterminer, UInt16 maxDepth = D_INFINITE);

	const std::string next(Stack* itStack, bool* isFinished);

private:
	SiblingsFirstTraverse();
	SiblingsFirstTraverse(const SiblingsFirstTraverse&);
	SiblingsFirstTraverse& operator=(const SiblingsFirstTraverse&);

	std::stack<std::queue<std::string> > _dirsStack;
};


NAMESPACE_SUBFIX


#endif // __SQHAO_DIRECTORYITERATORSTRATEGY_HXX__
