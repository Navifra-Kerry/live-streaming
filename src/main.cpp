#include "poco/Util/ServerApplication.h"
#include "Poco/Util/Option.h"
#include "Poco/Util/OptionSet.h"
#include "Poco/Util/HelpFormatter.h"
#include "Poco/Util/AbstractConfiguration.h"
#include "Poco/Util/PropertyFileConfiguration.h"
#include "Poco/DataURIStreamFactory.h"
#include "Poco/ErrorHandler.h"
#include "Poco/Format.h"
#include "Poco/String.h"
#include "Poco/Environment.h"
#include "Poco/File.h"
#include "LiveSubSystem.h"

#include <cstring>
#include <iostream>


#ifdef WIN32
	#pragma comment(lib,"ws2_32")
	#pragma comment(lib,"iphlpapi")
#endif

using Poco::Util::ServerApplication;
using Poco::Util::Option;
using Poco::Util::OptionSet;
using Poco::Util::HelpFormatter;
using Poco::Util::AbstractConfiguration;
using Poco::Util::OptionCallback;

class LiveStreamingServer : public ServerApplication
{
public:
	LiveStreamingServer() :
		_errorHandler(*this),
		_subSystem(new LiveStream::LiveSubSystem)		
	{
		Poco::DataURIStreamFactory::registerFactory();
		Poco::ErrorHandler::set(&_errorHandler);
		addSubsystem(_subSystem);
	}

	~LiveStreamingServer()
	{
		Poco::ThreadPool::defaultPool().joinAll();
		Poco::DataURIStreamFactory::unregisterFactory();
	}

protected:
	class ErrorHandler : public Poco::ErrorHandler
	{
	public:
		ErrorHandler(LiveStreamingServer& app) :
			_app(app)
		{
		}

		void exception(const Poco::Exception& exc)
		{
			if (std::strcmp(exc.name(), "Connection reset by peer") != 0 &&
				std::strcmp(exc.name(), "Timeout") != 0)
			{
				log(exc.displayText());
			}
		}

		void exception(const std::exception& exc)
		{
			log(exc.what());
		}

		void exception()
		{
			log("unknown exception");
		}

		void log(const std::string& message)
		{
			_app.logger().notice("A thread was terminated by an unhandled exception: " + message);
		}

	private:
		LiveStreamingServer& _app;
	};

	std::string loadSettings()
	{
		std::string settingsPath = config().getString("LiveStreamingServer.settings.path", "");
		if (!settingsPath.empty())
		{
			Poco::AutoPtr<Poco::Util::PropertyFileConfiguration> pSettings;
			Poco::File settingsFile(settingsPath);
			if (settingsFile.exists())
			{
				pSettings = new Poco::Util::PropertyFileConfiguration(settingsPath);
			}
			else
			{
				pSettings = new Poco::Util::PropertyFileConfiguration;
			}
			config().add(pSettings, "LiveStreamingServer.settings", Poco::Util::Application::PRIO_DEFAULT, true);
		}
		return settingsPath;
	}

	void initialize(Application& self)
	{
		if (!_skipDefaultConfig)
		{
			loadConfiguration();
		}
		for (const auto& cf : _configs)
		{
			loadConfiguration(cf);
		}

		int defaultThreadPoolCapacity = config().getInt("poco.threadPool.default.capacity", 32);
		int defaultThreadPoolCapacityDelta = defaultThreadPoolCapacity - Poco::ThreadPool::defaultPool().capacity();
		if (defaultThreadPoolCapacityDelta > 0)
		{
			Poco::ThreadPool::defaultPool().addCapacity(defaultThreadPoolCapacityDelta);
		}

		std::string settingsPath = loadSettings();

		ServerApplication::initialize(self);

		if (!settingsPath.empty() && !_showHelp)
		{
			logger().information("Settings loaded from \"%s\".", settingsPath);
		}

		if (!_showHelp)
		{
			logger().information(
                "LiveStreamingServer Copyright (c) 2020 by Kerry Cho"
			);
			logger().information("System information: %s (%s) on %s, %u CPU core(s).",
				Poco::Environment::osDisplayName(),
				Poco::Environment::osVersion(),
				Poco::Environment::osArchitecture(),
				Poco::Environment::processorCount());
		}
	}

	void defineOptions(OptionSet& options)
	{
		ServerApplication::defineOptions(options);

		options.addOption(
			Option("help", "h", "Display help information on command line arguments.")
			.required(false)
			.repeatable(false)
			.callback(OptionCallback<LiveStreamingServer>(this, &LiveStreamingServer::handleHelp)));

		options.addOption(
			Option("config-file", "c", "Load configuration data from a file.")
			.required(false)
			.repeatable(true)
			.argument("file")
			.callback(OptionCallback<LiveStreamingServer>(this, &LiveStreamingServer::handleConfig)));

		options.addOption(
			Option("skip-default-config", "", "Don't load default configuration file.")
			.required(false)
			.repeatable(false)
			.callback(OptionCallback<LiveStreamingServer>(this, &LiveStreamingServer::handleSkipDefaultConfig)));
	}

	void handleHelp(const std::string& name, const std::string& value)
	{
		_showHelp = true;
		displayHelp();
		stopOptionsProcessing();
		_subSystem->cancelInit();
	}

	void handleConfig(const std::string& name, const std::string& value)
	{
		_configs.push_back(value);
	}

	void handleSkipDefaultConfig(const std::string& name, const std::string& value)
	{
		_skipDefaultConfig = true;
	}

	void displayHelp()
	{
		HelpFormatter helpFormatter(options());
		helpFormatter.setCommand(commandName());
		helpFormatter.setUsage("OPTIONS");
		helpFormatter.setHeader(
			"The following command line options are supported:"
		);
		helpFormatter.setFooter(
			"For more information, please see the document "
		);
		helpFormatter.setIndent(8);
		helpFormatter.format(std::cout);
	}

	int main(const std::vector<std::string>& args)
	{
		if (!_showHelp)
		{
			waitForTerminationRequest();
		}
		return Application::EXIT_OK;
	}

private:
	ErrorHandler _errorHandler;
	bool _showHelp = false;
	bool _skipDefaultConfig = false;
	LiveStream::LiveSubSystem* _subSystem;
	std::vector<std::string> _configs;
};


POCO_SERVER_MAIN(LiveStreamingServer)