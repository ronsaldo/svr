#ifndef _SVR_INTERFACE_HPP_
#define _SVR_INTERFACE_HPP_

#include <memory>
#include "SVR/Common.hpp"

namespace SVR
{
#define DECLARE_SMART_POINTERS(ClassName) \
	typedef std::shared_ptr<ClassName> ClassName ## Ptr; \
	typedef std::weak_ptr<ClassName> ClassName ## WeakPtr;

#define DECLARE_INTERFACE(InterfaceName) \
	struct InterfaceName; \
	DECLARE_SMART_POINTERS(InterfaceName)

#define DECLARE_CLASS(ClassName) \
	class ClassName; \
	DECLARE_SMART_POINTERS(ClassName)


/**
 * The base interface for everything. This interface only declares an empty virtual destructor.
 */
struct SVR_EXPORT Interface
{
	virtual ~Interface() {}
};

}

#endif //_SVR_INTERFACE_HPP_
