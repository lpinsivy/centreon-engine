/*
** Copyright 2011-2015 Merethis
**
** This file is part of Centreon Engine.
**
** Centreon Engine is free software: you can redistribute it and/or
** modify it under the terms of the GNU General Public License version 2
** as published by the Free Software Foundation.
**
** Centreon Engine is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
** General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Centreon Engine. If not, see
** <http://www.gnu.org/licenses/>.
*/

#include <fstream>
#include <iomanip>
#include "com/centreon/engine/broker.hh"
#include "com/centreon/engine/error.hh"
#include "com/centreon/engine/globals.hh"
#include "com/centreon/engine/logging/logger.hh"
#include "com/centreon/engine/retention/dump.hh"

using namespace com::centreon::engine::logging;
using namespace com::centreon::engine::retention;

/**
 *  Dump retention of custom variables.
 *
 *  @param[out] os  The output stream.
 *  @param[in]  obj The custom variables to dump.
 *
 *  @return The output stream.
 */
std::ostream& dump::customvariables(
                std::ostream& os,
                customvariablesmember_struct const& obj) {
  for (customvariablesmember const* member(&obj);
       member;
       member = member->next)
    if (member->variable_name)
      os << "_" << member->variable_name << "="
         << member->has_been_modified << ";"
         << (member->variable_value ? member->variable_value : "")
         << "\n";
  return (os);
}

/**
 *  Dump header retention.
 *
 *  @param[out] os The output stream.
 *
 *  @return The output stream.
 */
std::ostream& dump::header(std::ostream& os) {
  os << "##############################################\n"
    "#    CENTREON ENGINE STATE RETENTION FILE    #\n"
    "#                                            #\n"
    "# THIS FILE IS AUTOMATICALLY GENERATED BY    #\n"
    "# CENTREON ENGINE. DO NOT MODIFY THIS FILE ! #\n"
    "##############################################\n";
  return (os);
}

/**
 *  Dump retention of host.
 *
 *  @param[out] os  The output stream.
 *  @param[in]  obj The host to dump.
 *
 *  @return The output stream.
 */
std::ostream& dump::host(std::ostream& os, host_struct const& obj) {
  os << "host {\n"
    "host_name=" << obj.name << "\n"
    "active_checks_enabled=" << obj.checks_enabled << "\n"
    "check_command=" << (obj.host_check_command ? obj.host_check_command : "") << "\n"
    "check_execution_time=" << std::setprecision(3) << std::fixed << obj.execution_time << "\n"
    "check_latency=" << std::setprecision(3) << std::fixed << obj.latency << "\n"
    "check_options=" << obj.check_options << "\n"
    "check_period=" << (obj.check_period ? obj.check_period : "") << "\n"
    "check_type=" << obj.check_type << "\n"
    "current_attempt=" << obj.current_attempt << "\n"
    "current_event_id=" << obj.current_event_id << "\n"
    "current_problem_id=" << obj.current_problem_id << "\n"
    "current_state=" << obj.current_state << "\n"
    "event_handler=" << (obj.event_handler ? obj.event_handler : "") << "\n"
    "event_handler_enabled=" << obj.event_handler_enabled << "\n"
    "flap_detection_enabled=" << obj.flap_detection_enabled << "\n"
    "has_been_checked=" << obj.has_been_checked << "\n"
    "is_flapping=" << obj.is_flapping << "\n"
    "last_check=" << static_cast<unsigned long>(obj.last_check) << "\n"
    "last_event_id=" << obj.last_event_id << "\n"
    "last_hard_state=" << obj.last_hard_state << "\n"
    "last_hard_state_change=" << static_cast<unsigned long>(obj.last_hard_state_change) << "\n"
    "last_problem_id=" << obj.last_problem_id << "\n"
    "last_state=" << obj.last_state << "\n"
    "last_state_change=" << static_cast<unsigned long>(obj.last_state_change) << "\n"
    "last_time_down=" << static_cast<unsigned long>(obj.last_time_down) << "\n"
    "last_time_unreachable=" << static_cast<unsigned long>(obj.last_time_unreachable) << "\n"
    "last_time_up=" << static_cast<unsigned long>(obj.last_time_up) << "\n"
    "long_plugin_output=" << (obj.long_plugin_output ? obj.long_plugin_output : "") << "\n"
    "max_attempts=" << obj.max_attempts << "\n"
    "modified_attributes=" << obj.modified_attributes << "\n"
    "next_check=" << static_cast<unsigned long>(obj.next_check) << "\n"
    "normal_check_interval=" << obj.check_interval << "\n"
    "obsess_over_host=" << obj.obsess_over_host << "\n"
    "percent_state_change=" << std::setprecision(2) << std::fixed << obj.percent_state_change << "\n"
    "performance_data=" << (obj.perf_data ? obj.perf_data : "") << "\n"
    "plugin_output=" << (obj.plugin_output ? obj.plugin_output : "") << "\n"
    "retry_check_interval=" << obj.check_interval << "\n"
    "state_type=" << obj.state_type << "\n";

  os << "state_history=";
  for (unsigned int x(0); x < MAX_STATE_HISTORY_ENTRIES; ++x)
    os << (x > 0 ? "," : "") << obj.state_history[(x + obj.state_history_index) % MAX_STATE_HISTORY_ENTRIES];
  os << "\n";

  dump::customvariables(os, *obj.custom_variables);
  os << "}\n";
  return (os);
}

/**
 *  Dump retention of hosts.
 *
 *  @param[out] os The output stream.
 *
 *  @return The output stream.
 */
std::ostream& dump::hosts(std::ostream& os) {
  for (host_struct* obj(host_list); obj; obj = obj->next)
    dump::host(os, *obj);
  return (os);
}

/**
 *  Dump retention of info.
 *
 *  @param[out] os The output stream.
 *
 *  @return The output stream.
 */
std::ostream& dump::info(std::ostream& os) {
  os << "info {\n"
    "created=" << static_cast<unsigned long>(time(NULL)) << "\n"
    "}\n";
  return (os);
}

/**
 *  Dump retention of program.
 *
 *  @param[out] os The output stream.
 *
 *  @return The output stream.
 */
std::ostream& dump::program(std::ostream& os) {
  os << "program {\n"
    "check_host_freshness=" << config->check_host_freshness() << "\n"
    "check_service_freshness=" << config->check_service_freshness() << "\n"
    "enable_event_handlers=" << config->enable_event_handlers() << "\n"
    "enable_flap_detection=" << config->enable_flap_detection() << "\n"
    "global_host_event_handler=" << config->global_host_event_handler().c_str() << "\n"
    "global_service_event_handler=" << config->global_service_event_handler().c_str() << "\n"
    "modified_host_attributes=" << modified_host_process_attributes << "\n"
    "modified_service_attributes=" << modified_service_process_attributes << "\n"
    "next_event_id=" << next_event_id << "\n"
    "next_problem_id=" << next_problem_id << "\n"
    "obsess_over_hosts=" << config->obsess_over_hosts() << "\n"
    "obsess_over_services=" << config->obsess_over_services() << "\n"
    "}\n";
  return (os);
}

/**
 *  Save all data.
 *
 *  @param[in] path The file path to use to save.
 *
 *  @return True on success, otherwise false.
 */
bool dump::save(std::string const& path) {
  // send data to event broker
  broker_retention_data(
    NEBTYPE_RETENTIONDATA_STARTSAVE,
    NEBFLAG_NONE,
    NEBATTR_NONE,
    NULL);

  bool ret(false);
  try {
    std::ofstream stream(
                    path.c_str(),
                    std::ios::binary | std::ios::trunc);
    if (!stream.is_open())
      throw (engine_error() << "Cannot open retention file '"
             << config->state_retention_file() << "'");
    dump::header(stream);
    dump::info(stream);
    dump::program(stream);
    dump::hosts(stream);
    dump::services(stream);

    ret = true;
  }
  catch (std::exception const& e) {
    logger(log_runtime_error, basic)
      << e.what();
  }

  // send data to event broker.
  broker_retention_data(
    NEBTYPE_RETENTIONDATA_ENDSAVE,
    NEBFLAG_NONE,
    NEBATTR_NONE,
    NULL);
  return (ret);
}

/**
 *  Dump retention of service.
 *
 *  @param[out] os  The output stream.
 *  @param[in]  obj The service to dump.
 *
 *  @return The output stream.
 */
std::ostream& dump::service(std::ostream& os, service_struct const& obj) {
  os << "service {\n"
    "host_name=" << obj.host_name << "\n"
    "service_description=" << obj.description << "\n"
    "active_checks_enabled=" << obj.checks_enabled << "\n"
    "check_command=" << (obj.service_check_command ? obj.service_check_command : "") << "\n"
    "check_execution_time=" << std::setprecision(3) << std::fixed << obj.execution_time << "\n"
    "check_latency=" << std::setprecision(3) << std::fixed << obj.latency << "\n"
    "check_options=" << obj.check_options << "\n"
    "check_period=" << (obj.check_period ? obj.check_period : "") << "\n"
    "check_type=" << obj.check_type << "\n"
    "current_attempt=" << obj.current_attempt << "\n"
    "current_event_id=" << obj.current_event_id << "\n"
    "current_problem_id=" << obj.current_problem_id << "\n"
    "current_state=" << obj.current_state << "\n"
    "event_handler=" << (obj.event_handler ? obj.event_handler : "") << "\n"
    "event_handler_enabled=" << obj.event_handler_enabled << "\n"
    "flap_detection_enabled=" << obj.flap_detection_enabled << "\n"
    "has_been_checked=" << obj.has_been_checked << "\n"
    "is_flapping=" << obj.is_flapping << "\n"
    "last_check=" << static_cast<unsigned long>(obj.last_check) << "\n"
    "last_event_id=" << obj.last_event_id << "\n"
    "last_hard_state=" << obj.last_hard_state << "\n"
    "last_hard_state_change=" << static_cast<unsigned long>(obj.last_hard_state_change) << "\n"
    "last_problem_id=" << obj.last_problem_id << "\n"
    "last_state=" << obj.last_state << "\n"
    "last_state_change=" << static_cast<unsigned long>(obj.last_state_change) << "\n"
    "last_time_critical=" << static_cast<unsigned long>(obj.last_time_critical) << "\n"
    "last_time_ok=" << static_cast<unsigned long>(obj.last_time_ok) << "\n"
    "last_time_unknown=" << static_cast<unsigned long>(obj.last_time_unknown) << "\n"
    "last_time_warning=" << static_cast<unsigned long>(obj.last_time_warning) << "\n"
    "long_plugin_output=" << (obj.long_plugin_output ? obj.long_plugin_output : "") << "\n"
    "max_attempts=" << obj.max_attempts << "\n"
    "modified_attributes=" << obj.modified_attributes << "\n"
    "next_check=" << static_cast<unsigned long>(obj.next_check) << "\n"
    "normal_check_interval=" << obj.check_interval << "\n"
    "obsess_over_service=" << obj.obsess_over_service << "\n"
    "percent_state_change=" << std::setprecision(2) << std::fixed << obj.percent_state_change << "\n"
    "performance_data=" << (obj.perf_data ? obj.perf_data : "") << "\n"
    "plugin_output=" << (obj.plugin_output ? obj.plugin_output : "") << "\n"
    "retry_check_interval=" << obj.retry_interval << "\n"
    "state_type=" << obj.state_type << "\n";

  os << "state_history=";
  for (unsigned int x(0); x < MAX_STATE_HISTORY_ENTRIES; ++x)
    os << (x > 0 ? "," : "") << obj.state_history[(x + obj.state_history_index) % MAX_STATE_HISTORY_ENTRIES];
  os << "\n";

  dump::customvariables(os, *obj.custom_variables);
  os << "}\n";
  return (os);
}

/**
 *  Dump retention of services.
 *
 *  @param[out] os The output stream.
 *
 *  @return The output stream.
 */
std::ostream& dump::services(std::ostream& os) {
  for (service_struct* obj(service_list); obj; obj = obj->next)
    dump::service(os, *obj);
  return (os);
}
