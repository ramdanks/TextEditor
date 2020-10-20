#pragma once
#include <string>
#include "Logging.h"

namespace Util
{
	class Err
	{
		std::string message;
	public:
		Err( const std::string& message ) : message(message) {}

		void Log( LogLevel lv, const std::string& filepath );
		std::string Seek() const;
	};

	#define THROW_ERR(MSG)					throw Util::Err(MSG)
	#define THROW_ERR_IF(X, MSG)			if (X) throw Util::Err(MSG)
	#define THROW_ERR_IFNOT(X, MSG)			if (!X) throw Util::Err(MSG)
	#define THROW_ERR_IFNULL(X, MSG)		if (X == 0) throw Util::Err(MSG)
	#define THROW_ERR_IFNULLPTR(X, MSG)		if (X == nullptr) throw Util::Err(MSG)
}