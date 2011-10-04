/*
** Copyright 2011 Merethis
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

#include <QDebug>
#include <exception>
#include "error.hh"
#include "commands.hh"
#include "globals.hh"
#include "error.hh"

/**
 *  Run enable_contactgroup_host_notifications test.
 */
static void check_enable_contactgroup_host_notifications() {
  init_object_skiplists();

  contact* cntct = add_contact("name", NULL, NULL, NULL, NULL, NULL, NULL, 0,
                               0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
  if (!cntct)
    throw (engine_error() << "create contact failed.");

  contactgroup* group = add_contactgroup("group", NULL);
  if (!group)
    throw (engine_error() << "create contactgroup failed.");

  contactsmember* member = add_contact_to_contactgroup(group, "name");
  if (!member)
    throw (engine_error() << "create contactsmember failed.");

  member->contact_ptr = cntct;
  cntct->host_notifications_enabled = false;
  char const* cmd("[1317196300] ENABLE_CONTACTGROUP_HOST_NOTIFICATIONS;group");
  process_external_command(cmd);

  if (!cntct->host_notifications_enabled)
    throw (engine_error() << "enable_contactgroup_host_notifications failed.");

  delete[] member->contact_name;
  delete member;

  delete[] group->group_name;
  delete[] group->alias;
  delete group;

  delete[] cntct->name;
  delete[] cntct->alias;
  delete cntct;

  free_object_skiplists();
}

/**
 *  Check processing of enable_contactgroup_host_notifications works.
 */
int main(void) {
  try {
    check_enable_contactgroup_host_notifications();
  }
  catch (std::exception const& e) {
    qDebug() << "error: " << e.what();
    return (1);
  }
  return (0);
}