#pragma once
#include <vector>
#include <string>
#include "Logging.h"

namespace Util
{
	static std::vector<uint8_t> f = { FORMAT_LEVEL, FORMAT_TIME, FORMAT_MSG };
	static Logging log( f );

	struct Err
	{
		Err( const std::string& message ) : message(message) {}
		std::string Seek() const { return this->message; }

	private: std::string message;
	};

	static void Log( const Err& e, const std::string& filepath )
	{ log.Log_File( LEVEL_ERROR, e.Seek(), filepath ); }

	#define THROW_ERR(MSG)                        throw Util::Err(MSG)
	#define THROW_ERR_IF(X, MSG)                  if (X) throw Util::Err(MSG)
	#define THROW_ERR_IFNOT(X, MSG)               if (!X) throw Util::Err(MSG)
	#define THROW_ERR_IFNULL(X, MSG)              if (X == 0) throw Util::Err(MSG)
	#define THROW_ERR_IFNULLPTR(X, MSG)           if (X == nullptr) throw Util::Err(MSG)
	#define THROW_ERR_IFEMPTY(X, MSG)             if (X.empty()) throw Util::Err(MSG)
}