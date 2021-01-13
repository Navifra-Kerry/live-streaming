#ifndef WEBSERVER_DISPATCHER
#define WEBSERVER_DISPATCHER


#include "Network/MediaTypeMapper.h"
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPRequestHandlerFactory.h"
#include "Poco/Net/HTTPResponse.h"
#include "Poco/RegularExpression.h"
#include "Poco/Logger.h"
#include "Poco/SharedPtr.h"
#include "Poco/ThreadPool.h"
#include "Poco/Mutex.h"
#include <vector>
#include <map>
#include <set>


namespace LiveStream {

class WebServerDispatcher : public virtual Poco::RefCountedObject
{
public:
	using Prt = Poco::AutoPtr<WebServerDispatcher>;
	using RequestHandlerFactoryPtr = Poco::SharedPtr<Poco::Net::HTTPRequestHandlerFactory>;
	using RegularExpressionPtr = Poco::SharedPtr<Poco::RegularExpression>;	

	struct PathCORS
	{
		PathCORS() :
			enable(false),
			allowCredentials(true)
		{
		}

		bool               enable;           /// enable or disable CORS
		std::string        allowOrigin;      /// CORS allowed origin
		std::string        allowMethods;     /// CORS allowed methods
		bool               allowCredentials; /// allow credentials with CORS requests
		std::string        allowHeaders;     /// allowed headers in CORS requests
	};

	struct VirtualPath
		/// A VirtualPath struct is used to specify a path mapping for a bundle.
	{
		VirtualPath() :
			hidden(false)
		{
		}
	
		VirtualPath(const std::string& aPath, const std::string& aResource) :
			path(aPath),
			resource(aResource),
			hidden(false),
			cache(true)
		{
		}
	
		VirtualPath(const std::string& aPath, RequestHandlerFactoryPtr aFactory) :
			path(aPath),
			pFactory(aFactory),
			hidden(false),
			cache(true)
		{
		}
	
		RegularExpressionPtr     pPattern;     /// pattern for matching request handlers
		std::string              path;         /// virtual server path (e.g., /images)
		std::set<std::string>    methods;      /// allowed methods ("GET", "POST", etc.)
		std::string              description;  /// user-readable description of resource or service
		std::string              resource;     /// resource path (if mapped to resource)
		std::string              indexPage;    /// index page (only used if resource path is set; defaults to "index.html")
		RequestHandlerFactoryPtr pFactory;     /// request handler factory (null if resource path is specified)
		PathCORS                 cors;         /// CORS settings
		bool                     hidden;       /// path is not included in list returned by listVirtualPaths()
		bool                     cache;        /// resource can be cached
	};

	struct PathInfo
	{
		std::string description;
		std::string permission;
		std::string session;
	};

	enum Options
	{
		CONF_OPT_COMPRESS_RESPONSES = 0x01,
		/// Compress responses using gzip content encoding.

		CONF_OPT_CACHE_RESOURCES = 0x02,
		/// Enable in-memory caching of bundle resources.

		CONF_OPT_ADD_AUTH_HEADER = 0x04,
		/// Add X-OSP-Authorized-User header to authenticated requests.

		CONF_OPT_ADD_SIGNATURE = 0x08
		/// Add server signature to generated HTML error responses.
	};

	struct Config
	{
		Config()  //options(), authMethods(AUTH_ALL) { }
		{

		}

		MediaTypeMapper::Ptr pMediaTypeMapper;
		//std::string authServiceName;
		//std::string tokenValidatorName;
		std::set<std::string> compressedMediaTypes;
		//Poco::Net::NameValueCollection customResponseHeaders;
		int options;
		//int authMethods;
		std::string corsAllowedOrigin;
	};

	using PathMap = std::map<std::string, VirtualPath>;
	using PathInfoMap = std::map<std::string, PathInfo>;
	using PatternVec = std::vector<VirtualPath>;

	explicit WebServerDispatcher(const Config& config);
	/// Creates the WebServerDispatcher.

	virtual ~WebServerDispatcher();
	/// Destroys the WebServerDispatcher.

	void addVirtualPath(const VirtualPath& virtualPath);

	void removeVirtualPath(const std::string& virtualPath);

	void handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response, bool secure);

	Poco::ThreadPool& threadPool();

protected:
	static std::string normalizePath(const std::string& path);
	/// Creates normalized path for internal storage.
	/// The normalized path always starts and ends with a slash.

	const VirtualPath& mapPath(const std::string& path, const std::string& method) const;
	/// Maps a URI to a VirtualPath.

	void sendResource(Poco::Net::HTTPServerRequest& request, const std::string& path, const std::string& vpath, const std::string& resPath, const std::string& resBase, const std::string& index, bool canCache);
	///// Sends a bundle resource as response.
	//
	std::istream* findResource(const std::string& base, const std::string& res, const std::string& index, std::string& mediaType, std::string& resolvedPath, bool canCache) const;
	///// Returns a resource stream for the given path, or a null pointer
	///// if no matching resource exists.
	//
	std::istream* getCachedResource(const std::string& path, bool canCache) const;
	///// Returns a resource stream for the given path, or a null pointer
	///// if no matching resource exists. If caching is enabled both globally
	///// and for the specific resource, attempts to cache the resource.
	//
	static bool cleanPath(std::string& path);
	///// Removes unnecessary characters (such as trailing dots)
	///// from the path and checks for illegal or dangerous
	///// characters.
	/////
	///// Returns true if the path is okay, false otherwise.

	bool handleCORS(const Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response, const VirtualPath& vPath) const;
	/// Handles CORS preflight requests and headers.

	bool enableCORS(const Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response, const VirtualPath& vPath) const;
	/// Adds CORS headers on the response if required.

	void sendFound(Poco::Net::HTTPServerRequest& request, const std::string& path);
	/// Sends a 302 Found response.

	void sendNotFound(Poco::Net::HTTPServerRequest& request, const std::string& path);
	/// Sends a 404 Not Found error response.

	void sendNotAuthorized(Poco::Net::HTTPServerRequest& request, const std::string& path);
	/// Sends a 401 Unauthorized error response.

	void sendForbidden(Poco::Net::HTTPServerRequest& request, const std::string& path);
	/// Sends a 403 Forbidden error response.

	void sendBadRequest(Poco::Net::HTTPServerRequest& request, const std::string& message);
	/// Sends a 404 Not Found error response.

	void sendMethodNotAllowed(Poco::Net::HTTPServerRequest& request, const std::string& message);
	/// Sends a 405 Method Not Allowed error response.

	void sendInternalError(Poco::Net::HTTPServerRequest& request, const std::string& message);
	/// Sends a 500 Internal Server Error response.

	void sendResponse(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPResponse::HTTPStatus status, const std::string& message);
	/// Sends a standard status/error response.

	void sendHTMLResponse(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPResponse::HTTPStatus status, const std::string& message);
	/// Sends a standard status/error response in HTML format.

	void sendJSONResponse(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPResponse::HTTPStatus status, const std::string& message);
	/// Sends a standard status/error response in JSON format.

	static std::string htmlize(const std::string& str);
	/// Returns a HTML-ized version of the given string.

	static std::string jsonize(const std::string& str);
	/// Returns a JSON-ized (escaped) version of the given string.

	std::string formatMessage(const std::string& messageId, const std::string& arg1 = std::string(), const std::string& arg2 = std::string());
	/// Reads a message from the bundle.properties resource and replaces
	/// placeholders $1 and $2 with arg1 and arg2, respectively.

	bool shouldCompressMediaType(const std::string& mediaType) const;
	/// Returns true iff content with the given media type should be compressed.

	void addCustomResponseHeaders(Poco::Net::HTTPServerResponse& response);
	/// Adds any configured custom response headers.

	void logRequest(const Poco::Net::HTTPServerRequest& request, const Poco::Net::HTTPServerResponse& response);
	/// Logs the HTTP request.

	static const std::string BEARER;
	static const std::string X_OSP_AUTHORIZED_USER;

private:
	using ResourceCache = std::map<std::string, std::string>;

	PathMap _pathMap;
	PatternVec _patternVec;
	MediaTypeMapper::Ptr _pMediaTypeMapper;
	std::string _corsAllowedOrigin;
	bool _compressResponses;
	bool _cacheResources;
	std::set<std::string> _compressedMediaTypes;
	Poco::Net::NameValueCollection _customResponseHeaders;
	mutable ResourceCache _resourceCache;
	mutable Poco::FastMutex _resourceCacheMutex;
	Poco::ThreadPool _threadPool;
	mutable Poco::FastMutex _mutex;
	Poco::Logger& _logger;
	Poco::Logger& _accessLogger;
};

//
// inlines
//
inline Poco::ThreadPool& WebServerDispatcher::threadPool()
{
	return _threadPool;
}
	
}

#endif // WEBSERVER_DISPATCHER
