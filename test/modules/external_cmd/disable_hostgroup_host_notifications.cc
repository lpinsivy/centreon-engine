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
 *  Run disable_hostgroup_host_notifications test.
 */
static void check_disable_hostgroup_host_notifications() {
  init_object_skiplists();

  host* hst = add_host("name", NULL, NULL, "localhost", NULL, 0, 0.0, 0.0, 42,
                       0, 0, 0, 0, 0, 0.0, 0.0, NULL, 0, NULL, 0, 0, NULL, 0,
                       0, 0.0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, 0, 0, NULL,
                       NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0.0, 0.0,
                       0.0, 0, 0, 0, 0, 0);
  if (!hst)
    throw (engine_error() << "create host failed.");

  hostgroup* group = add_hostgroup("group", NULL, NULL, NULL, NULL);
  if (!group)
    throw (engine_error() << "create hostgroup failed.");

  hostsmember* member = add_host_to_hostgroup(group, "name");
  if (!member)
    throw (engine_error() << "host link to hostgroup.");

  member->host_ptr = hst;
  hst->notifications_enabled = true;
  char const* cmd("[1317196300] DISABLE_HOSTGROUP_HOST_NOTIFICATIONS;group");
  process_external_command(cmd);

  if (hst->notifications_enabled)
    throw (engine_error() << "disable_hostgroup_host_notifications failed.");


  delete[] member->host_name;
  delete member;

  delete[] group->group_name;
  delete[] group->alias;
  delete group;

  delete[] hst->name;
  delete[] hst->display_name;
  delete[] hst->alias;
  delete[] hst->address;
  delete hst;

  free_object_skiplists();
}

/**
 *  Check processing of disable_hostgroup_host_notifications works.
 */
int main(void) {
  try {
    check_disable_hostgroup_host_notifications();
  }
  catch (std::exception const& e) {
    qDebug() << "error: " << e.what();
    return (1);
  }
  return (0);
}