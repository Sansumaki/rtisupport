#pragma once


#include <rti/recording/RecordingService.hpp>
#include <rti/recording/ServiceProperty.hpp>

int mainx(rti::recording::ApplicationRoleKind role, std::string config, std::int32_t domain_id);
