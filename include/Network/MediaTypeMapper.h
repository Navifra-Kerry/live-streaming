#ifndef MEDIA_TYPE
#define MEDIA_TYPE


#include "Poco/RefCountedObject.h"
#include "Poco/AutoPtr.h"
#include <map>
#include <istream>

namespace LiveStream {

class MediaTypeMapper :public Poco::RefCountedObject
{
public:
	using Ptr = Poco::AutoPtr<MediaTypeMapper>;
	using SuffixToMediaTypeMap = std::map<std::string, std::string>;
	using ConstIterator = SuffixToMediaTypeMap::const_iterator;
		
	MediaTypeMapper();
	~MediaTypeMapper();


	void add(const std::string& suffix, const std::string& mediaType);

	ConstIterator find(const std::string& suffix) const;

	ConstIterator begin() const;

	ConstIterator end() const;

	const std::string& map(const std::string& suffix) const;

	void setDefault(const std::string& mediaType);

	const std::string& getDefault() const;
private:
	SuffixToMediaTypeMap _map;
	std::string          _default;
};
//
// inlines
//
inline MediaTypeMapper::ConstIterator MediaTypeMapper::begin() const
{
	return _map.begin();
}


inline MediaTypeMapper::ConstIterator MediaTypeMapper::end() const
{
	return _map.end();
}


inline const std::string& MediaTypeMapper::getDefault() const
{
	return _default;
}

}
#endif // MEDIA_TYPE
