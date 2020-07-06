#include "RecordingService.h"

/// <summary>
/// Initializes a new instance of the <see cref="Recorder"/> class.
/// </summary>
/// <param name="recordingMode">The recording mode.</param>
/// <param name="domain_id">The DDS domain identifier.</param>
/// <param name="config">The path to the configuration xml.</param>
/// <param name="serviceName">Name of the recording service.</param>
Livesim::Recording::Recorder::Recorder(RecorderModes recordingMode, std::int32_t domain_id, String^ config, String^ serviceName, String^ database_dir)
{
	logger = NLog::LogManager::GetCurrentClassLogger();

	service_property = new rti::recording::ServiceProperty();

	std::string config_std = ConvertString(config);
	std::string serviceName_std = ConvertString(serviceName);

	// convert enum
	rti::recording::ApplicationRoleKind nativ_role;
	switch (recordingMode)
	{
	case RecorderModes::RECORD:
		nativ_role = rti::recording::ApplicationRoleKind::RECORD_APPLICATION;
		break;
	case RecorderModes::REPLAY:
		nativ_role = rti::recording::ApplicationRoleKind::REPLAY_APPLICATION;
		break;
	default:
		throw gcnew System::Exception("Exception in RecordingService: Unsupported recording mode");
	}

	// Setup enviroment variables for recording parameters
	std::map<std::string, std::string> env_vars;
	env_vars.emplace("DATABASE_DIR", ConvertString(database_dir));

	logger->Info(("Recorder configured with: recordmode = " + recordingMode.ToString() + " ; domain_id = " + domain_id.ToString() + " ; config = " + config + " ; serviceName = " + serviceName + " ; databasepath = " + database_dir + ""));

	// initialize property
	(*service_property).application_role(nativ_role);
	(*service_property).cfg_file(config_std);
	(*service_property).user_environment(env_vars);
	(*service_property).domain_id_base(domain_id);
	(*service_property).administration_domain_id(domain_id);
	(*service_property).enable_administration(true);
	(*service_property).monitoring_domain_id(domain_id);
	(*service_property).enable_monitoring(true);
	(*service_property).service_name(serviceName_std);
}


/// <summary>
/// Finalizes an instance of the <see cref="Recorder"/> class.
/// </summary>
Livesim::Recording::Recorder::~Recorder()
{
	try {
		Stop();
		delete embedded_service;
		delete service_property;
	}
	catch (const std::exception& ex) {
		System::Exception^ exp = gcnew System::Exception("Exception in RecordingService: " + gcnew System::String(ex.what()));
		logger->Error(exp, "Exception in RecordingService: ");
		throw exp;
	}
}

/// <summary>
/// Starts the DDS recording service depending on its configuration
/// </summary>
/// <returns>
///   <c>true</c> if the recording service could be started; otherwise, <c>false</c>.
/// </returns>
bool Livesim::Recording::Recorder::Start()
{
	try {
		if (!_started)
		{
			embedded_service = new rti::recording::RecordingService(*service_property);
			(*embedded_service).start();
			_started = true;
			return true;
		}
		return false;
	}
	catch (const std::exception& ex) {
		System::Exception^ exp = gcnew System::Exception("Exception in start RecordingService: " + gcnew System::String(ex.what()));
		logger->Error(exp, "Exception in RecordingService: ");
		throw exp;
	}
	catch (...) {
		System::Exception^ exp = gcnew System::Exception("Exception in start RecordingService: unknown");
		logger->Error(exp, "Exception in RecordingService: ");
		throw exp;
	}
}

/// <summary>
/// Determines whether the recording service is started
/// </summary>
/// <returns>
///   <c>true</c> if this recording service is started; otherwise, <c>false</c>.
/// </returns>
bool Livesim::Recording::Recorder::IsStarted()
{
	return _started;
}

/// <summary>
/// Stops the running DDS recording service
/// </summary>
/// <returns>
///   <c>true</c> if the recording service could be stopped; otherwise, <c>false</c>.
/// </returns>
bool Livesim::Recording::Recorder::Stop()
{
	try {
		if (embedded_service != NULL && _started)
		{
			(*embedded_service).stop();
			embedded_service = NULL;
			_started = false;
			return true;
		}
		_started = false;
		return false;
	}
	catch (const std::exception& ex) {
		System::Exception^ exp = gcnew System::Exception("Exception in stop RecordingService: " + gcnew System::String(ex.what()));
		logger->Error(exp, "Exception in RecordingService: ");
		throw exp;
	}
	catch (...) {
		System::Exception^ exp = gcnew System::Exception("Exception in stop RecordingService: unknown");
		logger->Error(exp, "Exception in RecordingService: ");
		throw exp;
	}
}

/// <summary>
/// Marshal the clr string to c++ std string
/// </summary>
/// <param name="value">The clr string</param>
/// <returns>converted c++ std string</returns>
std::string Livesim::Recording::Recorder::ConvertString(String^ value)
{
	try
	{
		msclr::interop::marshal_context context;
		return context.marshal_as<std::string>(value);
	}
	catch (const std::exception& ex)
	{
		System::Exception^ exp = gcnew System::Exception("Exception in ConvertString: " + gcnew System::String(ex.what()));
		logger->Error(exp, "Error on marshalling string");
	}
	catch (...)
	{
		System::Exception^ exp = gcnew System::Exception("Exception in ConvertString: unknown");
		logger->Error(exp, "Error on marshalling string");
	}

	return std::string();
}