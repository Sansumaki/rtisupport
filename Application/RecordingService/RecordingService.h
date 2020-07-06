#pragma once

#include <string>
#include <msclr\marshal_cppstd.h>

#include <rti/recording/RecordingService.hpp>
#include <rti/recording/ServiceProperty.hpp>

using namespace System;
using std::string;

namespace Livesim {
	namespace Recording {

		public enum class RecorderModes {
			RECORD,
			REPLAY
		};

		public ref class Recorder
		{
		private:
			rti::recording::ServiceProperty* service_property = NULL;
			rti::recording::RecordingService* embedded_service = NULL;
			NLog::Logger^ logger;
			bool _started;
			std::string ConvertString(String^ value);

		public:
			Recorder(RecorderModes recordingMode, std::int32_t domain_id, String^ config, String^ serviceName, String^ database_dir);
			~Recorder();
			bool Start();
			bool IsStarted();
			bool Stop();
		};
	}
}