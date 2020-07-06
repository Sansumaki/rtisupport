/*******************************************************************************
 (c) 2020 Copyright, Real-Time Innovations, Inc.  All rights reserved.
 RTI grants Licensee a license to use, modify, compile, and create derivative
 works of the Software.  Licensee has the right to distribute object form only
 for use with RTI products.  The Software is provided "as is", with no warranty
 of any type, including any warranty for fitness for any purpose. RTI is under
 no obligation to maintain or support the Software.  RTI shall not be liable for
 any incidental or consequential damages arising out of the use or inability to
 use the software.
 ******************************************************************************/

#include <csignal>
#include <cstdlib>
#include <future>
#include <string>
#include <vector>

#include <rti/domain/rtidomain.hpp>
#include <rti/recording/RecordingService.hpp>
#include <rti/recording/ServiceProperty.hpp>

/* Program State */
namespace {

volatile bool doExit{ false };

/* Signal handler to request graceful/ungraceful termination */
extern "C" void
signal_handler(int sig)
{
  switch (sig) {
    case SIGINT:
      if (doExit) {
        /* Force kill if impatient (may corrupt database, etc) */
        std::exit(EXIT_FAILURE);
      } else {
        doExit = true;
      }
  }
}
} // end-namespace

/* Handles the main logic of the application */
int
example_main();

int
main(int argc, const char** argv)
{
  /* Increase the maximum objects per thread */
  auto domain_participant_factory_qos =
    dds::domain::qos::DomainParticipantFactoryQos{};
  auto system_resource_limits =
    rti::core::policy::SystemResourceLimits{ 1 << 20 };
  domain_participant_factory_qos.policy(system_resource_limits);
  dds::domain::DomainParticipant::participant_factory_qos(
    domain_participant_factory_qos);

  /* Set up signal handler to gracefully shutdown the program */
  std::signal(SIGINT, signal_handler);

  return example_main();
}

namespace ruag_example {
using rti::recording::RecordingService;
using rti::recording::ServiceProperty;

/* Describes the configurable values of the replays */
struct ReplayConfig
{
  std::string participant_name;
  std::uint32_t domain_id;
  std::string session_name;
  std::string topic_group_name;
  std::string topic_group_allow_filter;
} const
  /* Square Config */
  cfg_Squares{ "SquareParticipant",
               50,
               "SquareSession",
               "SquareTopicGroup",
               "Square" },
  /* Circle Config */
  cfg_Circles{ "CircleParticipant",
               60,
               "CircleSession",
               "CircleTopicGroup",
               "Circle" },
  /* Triangle Config */
  cfg_Triangles{ "TriangleParticipant",
                 70,
                 "TriangleSession",
                 "TriangleTopicGroup",
                 "Triangle" };

/* Contains the service instances. Can be extended with additional logic */
struct Container
{
  RecordingService recording_service;
  std::vector<RecordingService> replay_services;

  Container()
    : recording_service{ nullptr }
    , replay_services{ /* empty */ }
  {}
};

/* Generates a (fixed) Configuration for Recording */
inline rti::recording::ServiceProperty
record_service_property_new()
{
  ServiceProperty property;

  property.application_role(
    rti::recording::ApplicationRoleKind::RECORD_APPLICATION);
  property.enforce_xsd_validation(true);
  property.service_name("RuagExampleRecorder");
  property.cfg_file("recorder_config.xml");

  return property;
}

/* Generates a (modificable) Configuration for Replay */
inline rti::recording::ServiceProperty
replay_service_property_new(ReplayConfig const& cfg,
                             std::int32_t domain_id = 0)
{
  ServiceProperty property;

  std::map<std::string, std::string> env{
    { "DomainId", std::to_string(cfg.domain_id) },
    { "SessionName", cfg.session_name },
    { "ParticipantName", cfg.participant_name },
    { "TopicGroupName", cfg.topic_group_name },
    { "TopicGroupAllowFilter", cfg.topic_group_allow_filter },
  };

  property.user_environment(env);
  /* Replay in domain 0 by default */
  property.domain_id_base(-cfg.domain_id + domain_id);
  property.application_role(
    rti::recording::ApplicationRoleKind::REPLAY_APPLICATION);
  property.enforce_xsd_validation(true);
  property.service_name("RuagExampleReplay");
  property.cfg_file("replay_config.xml");

  return property;
}

} // end-namespace ruag_example

int
example_main()
{
  std::printf("Starting application...\n");

  /* Building the Recording Service instance */
  auto recording = rti::recording::RecordingService(
    ruag_example::record_service_property_new());

  /* Building a few Replay Services instances */
  auto replay_Squares = rti::recording::RecordingService(
    ruag_example::replay_service_property_new(ruag_example::cfg_Squares));
  auto replay_Circles = rti::recording::RecordingService(
    ruag_example::replay_service_property_new(ruag_example::cfg_Circles));
  auto replay_Triangles = rti::recording::RecordingService(
    ruag_example::replay_service_property_new(ruag_example::cfg_Triangles));

  /* Building the container that stores the Rec/Rep instances */
  ruag_example::Container container;
  container.recording_service = std::move(recording);
  container.replay_services.push_back(std::move(replay_Squares));
  container.replay_services.push_back(std::move(replay_Circles));
  container.replay_services.push_back(std::move(replay_Triangles));

  /* Starting the Recording instance */
  container.recording_service.start();
  std::printf("Recording...\n");

  /* Starting the Replay instances at different times, using std::async  */
  auto future_Squares = std::async([&container]() {
    std::this_thread::sleep_for(std::chrono::seconds{ 2 });
    container.replay_services.at(0).start(); /* Squares */
    std::printf("Replaying Squares...\n");
  });

  auto future_Circles = std::async([&container]() {
    std::this_thread::sleep_for(std::chrono::seconds{ 6 });
    container.replay_services.at(1).start(); /* Circles */
    std::printf("Replaying Circles...\n");
  });

  auto future_Triangles = std::async([&container]() {
    std::this_thread::sleep_for(std::chrono::seconds{ 3 });
    container.replay_services.at(2).start(); /* Triangles */
    std::printf("Replaying Triangles...\n");
  });

  while (!doExit) {
    /* Keep main thread alive, each instance lives on their own */
    std::this_thread::sleep_for(std::chrono::seconds{ 1 });
  }

  /* Gracefully end every Recording/Replay instance */
  std::printf("Stopping application...\n");
  container.recording_service.stop();
  for (auto& rep : container.replay_services) {
    rep.stop();
  }

  return EXIT_SUCCESS;
}