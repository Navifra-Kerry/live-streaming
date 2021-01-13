#include "Network/MediaTypeMapper.h"
#include "Poco/String.h"
#include <cctype>

namespace LiveStream {

MediaTypeMapper::MediaTypeMapper() :
	_default("application/binary")
{
}


MediaTypeMapper::~MediaTypeMapper()
{
}

MediaTypeMapper::ConstIterator MediaTypeMapper::find(const std::string& suffix) const
{
	return _map.find(Poco::toLower(suffix));
}


void MediaTypeMapper::add(const std::string& suffix, const std::string& mediaType)
{
	_map[Poco::toLower(suffix)] = mediaType;
}


const std::string& MediaTypeMapper::map(const std::string& suffix) const
{
	ConstIterator it = find(Poco::toLower(suffix));
	if (it != end())
		return it->second;
	else
		return _default;
}


void MediaTypeMapper::setDefault(const std::string& mediaType)
{
	_default = mediaType;
}

}