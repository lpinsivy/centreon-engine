/*
** Copyright 2011-2013 Merethis
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

#include "com/centreon/engine/configuration/applier/state.hh"
#include "com/centreon/engine/flapping.hh"
#include "com/centreon/engine/globals.hh"
#include "com/centreon/engine/misc/string.hh"
#include "com/centreon/engine/retention/host.hh"
#include "com/centreon/engine/statusdata.hh"

using namespace com::centreon::engine::configuration::applier;
using namespace com::centreon::engine;

/**
 *  Constructor.
 *
 *  @param[in] obj The host to use for retention.
 */
retention::host::host(host_struct* obj)
  : object(object::host),
    _obj(obj),
    _scheduling_info_is_ok(false),
    _was_flapping(false) {

}

/**
 *  Destructor.
 */
retention::host::~host() throw () {
  _finished();
}

/**
 *  Set scheduling info is ok.
 *
 *  @param[in] value The new scheduling info.
 */
void retention::host::scheduling_info_is_ok(bool value) {
  _scheduling_info_is_ok = value;
}

/**
 *  Set new value on specific property.
 *
 *  @param[in] key   The property to set.
 *  @param[in] value The new value.
 *
 *  @return True on success, otherwise false.
 */
bool retention::host::set(
       std::string const& key,
       std::string const& value) {
  if (!_obj && value == "host_name") {
    umap<std::string, shared_ptr<host_struct> >::const_iterator
      it(state::instance().hosts().find(value));
    if (it != state::instance().hosts().end())
      _obj = &*it->second;
    return (true);
  }
  else if (!_obj)
    return (false);
  if (_modified_attributes(key, value))
    return (true);
  if (_retain_status_information(key, value))
    return (true);
  return (_retain_nonstatus_information(key, value));
}

/**
 *  Finish all host update.
 */
void retention::host::_finished() throw () {
  if (!_obj)
    return;

  bool allow_flapstart_notification(true);

  // adjust modified attributes if necessary.
  if (!_obj->retain_nonstatus_information)
    _obj->modified_attributes = MODATTR_NONE;

  // adjust modified attributes if no custom variables
  // have been changed.
  if (_obj->modified_attributes & MODATTR_CUSTOM_VARIABLE) {
    for (customvariablesmember* member(_obj->custom_variables);
         member;
         member = member->next)
      if (member->has_been_modified) {
        _obj->modified_attributes -= MODATTR_CUSTOM_VARIABLE;
        break;
      }
  }

  // calculate next possible notification time.
  if (_obj->current_state != HOST_UP && _obj->last_host_notification)
    _obj->next_host_notification
      = get_next_host_notification_time(
          _obj,
          _obj->last_host_notification);

  // ADDED 01/23/2009 adjust current check attempts if host in hard
  // problem state (max attempts may have changed in config
  // since restart).
  if (_obj->current_state != HOST_UP && _obj->state_type == HARD_STATE)
    _obj->current_attempt = _obj->max_attempts;

  // ADDED 02/20/08 assume same flapping state if large install
  // tweaks enabled.
  if (config->use_large_installation_tweaks())
    _obj->is_flapping =_was_flapping;
  // else use normal startup flap detection logic.
  else {
    // host was flapping before program started.
    // 11/10/07 don't allow flapping notifications to go out.
    allow_flapstart_notification = (_was_flapping ? false : true);

    // check for flapping.
    check_for_host_flapping(
      _obj,
      false,
      false,
      allow_flapstart_notification);

    // host was flapping before and isn't now, so clear recovery
    // check variable if host isn't flapping now.
    if (_was_flapping && !_obj->is_flapping)
      _obj->check_flapping_recovery_notification = false;
  }

  // handle new vars added in 2.x.
  if (!_obj->last_hard_state_change)
    _obj->last_hard_state_change = _obj->last_state_change;

  // update host status.
  update_host_status(_obj, false);
}

/**
 *  Set new value on specific modified attrivute property.
 *
 *  @param[in] key   The property to set.
 *  @param[in] value The new value.
 *
 *  @return True on success, otherwise false.
 */
bool retention::host::_modified_attributes(
       std::string const& key,
       std::string const& value) {
  if (key == "modified_attributes") {
    misc::to(value, _obj->modified_attributes);
    // mask out attributes we don't want to retain.
    _obj->modified_attributes
      &= ~config->retained_host_attribute_mask();
  }
  else
    return (false);
  return (true);
}

/**
 *  Set new value on specific retain nonstatus information property.
 *
 *  @param[in] key   The property to set.
 *  @param[in] value The new value.
 *
 *  @return True on success, otherwise false.
 */
bool retention::host::_retain_nonstatus_information(
       std::string const& key,
       std::string const& value) {
  if (!_obj->retain_nonstatus_information)
    return (false);

  if (key == "problem_has_been_acknowledged")
    misc::to<bool, int>(value, _obj->problem_has_been_acknowledged);
  else if (key == "acknowledgement_type")
    misc::to(value, _obj->acknowledgement_type);
  else if (key == "notifications_enabled") {
    if (_obj->modified_attributes & MODATTR_NOTIFICATIONS_ENABLED)
      misc::to<bool, int>(value, _obj->notifications_enabled);
  }
  else if (key == "active_checks_enabled") {
    if (_obj->modified_attributes & MODATTR_ACTIVE_CHECKS_ENABLED)
      misc::to<bool, int>(value, _obj->checks_enabled);
  }
  else if (key == "passive_checks_enabled") {
    if (_obj->modified_attributes & MODATTR_PASSIVE_CHECKS_ENABLED)
      misc::to<bool, int>(value, _obj->accept_passive_host_checks);
  }
  else if (key == "event_handler_enabled") {
    if (_obj->modified_attributes & MODATTR_EVENT_HANDLER_ENABLED)
      misc::to<bool, int>(value, _obj->event_handler_enabled);
  }
  else if (key == "flap_detection_enabled") {
    if (_obj->modified_attributes & MODATTR_FLAP_DETECTION_ENABLED)
      misc::to<bool, int>(value, _obj->flap_detection_enabled);
  }
  else if (key == "failure_prediction_enabled") {
    if (_obj->modified_attributes & MODATTR_FAILURE_PREDICTION_ENABLED)
      misc::to<bool, int>(value, _obj->failure_prediction_enabled);
  }
  else if (key == "process_performance_data") {
    if (_obj->modified_attributes & MODATTR_PERFORMANCE_DATA_ENABLED)
      misc::to<bool, int>(value, _obj->process_performance_data);
  }
  else if (key == "obsess_over_host") {
    if (_obj->modified_attributes & MODATTR_OBSESSIVE_HANDLER_ENABLED)
      misc::to<bool, int>(value, _obj->obsess_over_host);
  }
  else if (key == "check_command") {
    if (_obj->modified_attributes & MODATTR_CHECK_COMMAND) {
      std::size_t pos(value.find('!'));
      if (pos != std::string::npos) {
        std::string command(value.substr(pos + 1));
        if (!find_command(command.c_str()))
          _obj->modified_attributes -= MODATTR_CHECK_COMMAND;
        else {
          delete[] _obj->host_check_command;
          _obj->host_check_command = my_strdup(value);
        }
      }
    }
  }
  else if (key == "check_period") {
    if (_obj->modified_attributes & MODATTR_CHECK_TIMEPERIOD) {
      if (!find_timeperiod(value.c_str()))
        _obj->modified_attributes -= MODATTR_CHECK_TIMEPERIOD;
      else {
        delete[] _obj->check_period;
        _obj->check_period = my_strdup(value);
      }
    }
  }
  else if (key == "notification_period") {
    if (_obj->modified_attributes & MODATTR_NOTIFICATION_TIMEPERIOD) {
      if (!find_timeperiod(value.c_str()))
        _obj->modified_attributes -= MODATTR_NOTIFICATION_TIMEPERIOD;
      else {
        delete[] _obj->notification_period;
        _obj->notification_period = my_strdup(value);
      }
    }
  }
  else if (key == "event_handler") {
    if (_obj->modified_attributes & MODATTR_EVENT_HANDLER_COMMAND) {
      std::size_t pos(value.find('!'));
      if (pos != std::string::npos) {
        std::string command(value.substr(pos + 1));
        if (!find_command(command.c_str()))
          _obj->modified_attributes -= MODATTR_EVENT_HANDLER_COMMAND;
        else {
          delete[] _obj->event_handler;
          _obj->event_handler = my_strdup(value);
        }
      }
    }
  }
  else if (key == "normal_check_interval") {
    if (_obj->modified_attributes & MODATTR_NORMAL_CHECK_INTERVAL) {
      double val;
      if (misc::to(value, val) && val >= 0)
        _obj->check_interval = val;
    }
  }
  else if (key == "retry_check_interval") {
    if (_obj->modified_attributes & MODATTR_RETRY_CHECK_INTERVAL) {
      double val;
      if (misc::to(value, val) && val >= 0)
        _obj->retry_interval = val;
    }
  }
  else if (key == "max_attempts") {
    if (_obj->modified_attributes & MODATTR_MAX_CHECK_ATTEMPTS) {
      int val;
      if (misc::to(value, val) && val > 0) {
        _obj->max_attempts = val;

        // adjust current attempt number if in a hard state.
        if (_obj->state_type == HARD_STATE
            && _obj->current_state != HOST_UP
            && _obj->current_attempt > 1)
          _obj->current_attempt = _obj->max_attempts;
      }
    }
  }
  else if (!key.empty() && key[0] == '_') {
    if (_obj->modified_attributes & MODATTR_CUSTOM_VARIABLE
        && value.size() > 3) {
      char const* cvname(key.c_str() + 1);
      char const* cvvalue(value.c_str() + 2);

      for (customvariablesmember* member = _obj->custom_variables;
           member;
           member = member->next) {
        if (!strcmp(cvname, member->variable_name)) {
          if (strcmp(cvvalue, member->variable_value)) {
            delete[] member->variable_value;
            member->variable_value = my_strdup(cvvalue);
            member->has_been_modified = true;
          }
          break;
        }
      }
    }
  }
  else
    return (false);
  return (true);
}

/**
 *  Set new value on specific retain nonstatus information property.
 *
 *  @param[in] key   The property to set.
 *  @param[in] value The new value.
 *
 *  @return True on success, otherwise false.
 */
bool retention::host::_retain_status_information(
       std::string const& key,
       std::string const& value) {
  if (!_obj->retain_status_information)
    return (false);

  if (key == "has_been_checked")
    misc::to<bool, int>(value, _obj->has_been_checked);
  else if (key == "check_execution_time")
    misc::to(value, _obj->execution_time);
  else if (key == "check_latency")
    misc::to(value, _obj->latency);
  else if (key == "check_type")
    misc::to(value, _obj->check_type);
  else if (key == "current_state")
    misc::to(value, _obj->current_state);
  else if (key == "last_state")
    misc::to(value, _obj->last_state);
  else if (key == "last_hard_state")
    misc::to(value, _obj->last_hard_state);
  else if (key == "plugin_output") {
    delete[] _obj->plugin_output;
    _obj->plugin_output = my_strdup(value);
  }
  else if (key == "long_plugin_output") {
    delete[] _obj->long_plugin_output;
    _obj->long_plugin_output = my_strdup(value);
  }
  else if (key == "performance_data") {
    delete[] _obj->perf_data;
    _obj->perf_data = my_strdup(value);
  }
  else if (key == "last_check")
    misc::to(value, _obj->last_check);
  else if (key == "next_check") {
    if (config->use_retained_scheduling_info() && _scheduling_info_is_ok)
      misc::to(value, _obj->next_check);
  }
  else if (key == "check_options") {
    if (config->use_retained_scheduling_info() && _scheduling_info_is_ok)
      misc::to(value, _obj->check_options);
  }
  else if (key == "current_attempt")
    misc::to<bool, int>(value, _obj->current_attempt);
  else if (key == "current_event_id")
    misc::to(value, _obj->current_event_id);
  else if (key == "last_event_id")
    misc::to(value, _obj->last_event_id);
  else if (key == "current_problem_id")
    misc::to(value, _obj->current_problem_id);
  else if (key == "last_problem_id")
    misc::to(value, _obj->last_problem_id);
  else if (key == "state_type")
    misc::to(value, _obj->state_type);
  else if (key == "last_state_change")
    misc::to(value, _obj->last_state_change);
  else if (key == "last_hard_state_change")
    misc::to(value, _obj->last_hard_state_change);
  else if (key == "last_time_up")
    misc::to(value, _obj->last_time_up);
  else if (key == "last_time_down")
    misc::to(value, _obj->last_time_down);
  else if (key == "last_time_unreachable")
    misc::to(value, _obj->last_time_unreachable);
  else if (key == "notified_on_down")
    misc::to(value, _obj->notified_on_down);
  else if (key == "notified_on_unreachable")
    misc::to<bool, int>(value, _obj->notified_on_unreachable);
  else if (key == "last_notification")
    misc::to(value, _obj->last_host_notification);
  else if (key == "current_notification_number")
    misc::to(value, _obj->current_notification_number);
  else if (key == "current_notification_id")
    misc::to(value, _obj->current_notification_id);
  else if (key == "is_flapping")
    misc::to(value, _was_flapping);
  else if (key == "percent_state_change")
    misc::to(value, _obj->percent_state_change);
  else if (key == "check_flapping_recovery_notification")
    misc::to(value, _obj->check_flapping_recovery_notification);
  else if (key == "state_history") {
    unsigned int x(0);
    std::list<std::string> lst_history;
    misc::split(value, lst_history, ',');
    for (std::list<std::string>::const_iterator
           it(lst_history.begin()), end(lst_history.end());
         it != end && x < MAX_STATE_HISTORY_ENTRIES;
         ++it) {
      misc::to(*it, _obj->state_history[x++]);
    }
    _obj->state_history_index = 0;
  }
  else
    return (false);
  return (true);
}
