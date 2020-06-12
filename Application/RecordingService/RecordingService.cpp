#include <string>
#include <msclr\marshal_cppstd.h>

#include <rti/recording/RecordingService.hpp>
#include <rti/recording/ServiceProperty.hpp>

using namespace System;
using std::string;

namespace RtiDDSRecording {

	public enum class RecorderModes {
		RECORD,
		REPLAY
	};

	public ref class Recorder
	{
	private:
		rti::recording::ServiceProperty* service_property = NULL;
		rti::recording::RecordingService* embedded_service = NULL;
		bool _started = false;

	public:
		Recorder(RecorderModes recordingMode, std::int32_t domain_id, String^ config, String^ serviceName, String^ databasePath)
		{
			service_property = new rti::recording::ServiceProperty();

			// marshal string
			msclr::interop::marshal_context context;
			std::string config_std = context.marshal_as<std::string>(config);
			msclr::interop::marshal_context context2;
			std::string serviceName_std = context2.marshal_as<std::string>(serviceName);
			msclr::interop::marshal_context context3;
			std::string databasePath_std = context3.marshal_as<std::string>(databasePath);

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
				throw gcnew System::Exception("Unsupported role arg: one of {record,replay} expected");
			}

			std::map<std::string, std::string> env_vars;
			env_vars.emplace("WORKSPACE_DIR_NAME", databasePath_std);
			//env_vars.emplace("EXECUTION_DIR_NAME", "custom_dir_name"); 

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
			(*service_property).application_name(serviceName_std);
		}

		~Recorder()
		{
			try {
				Stop();
				delete embedded_service;
				delete service_property;
			}
			catch (const std::exception& ex) {
				System::Console::WriteLine("Exception in RecordingService: " + gcnew System::String(ex.what()));
				throw gcnew System::Exception("Exception in RecordingService: " + gcnew System::String(ex.what()));
			}
		}

		bool Start()
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
				throw gcnew System::Exception("Exception in start RecordingService: " + gcnew System::String(ex.what()));
			}
			catch (...) {
				throw gcnew System::Exception("Exception in start RecordingService: unknown");
			}
		}

		bool IsStarted()
		{
			return _started;
		}

		bool Stop()
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
				throw gcnew System::Exception("Exception in stop RecordingService: " + gcnew System::String(ex.what()));
			}
			catch (...) {
				throw gcnew System::Exception("Exception in stop RecordingService: unknown");
			}
		}
	};
}