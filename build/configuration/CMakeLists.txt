##
## Copyright 2011-2015 Merethis
##
## This file is part of Centreon Engine.
##
## Centreon Engine is free software: you can redistribute it and/or
## modify it under the terms of the GNU General Public License version 2
## as published by the Free Software Foundation.
##
## Centreon Engine is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
## General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with Centreon Engine. If not, see
## <http://www.gnu.org/licenses/>.
##

# Set directories.
set(SRC_DIR "${SRC_DIR}/configuration")
set(INC_DIR "${INC_DIR}/com/centreon/engine/configuration")

# Subdirectory.
add_subdirectory("applier")

# Set files.
set(
  FILES
  ${FILES}

  # Sources.
  "${SRC_DIR}/command.cc"
  "${SRC_DIR}/connector.cc"
  "${SRC_DIR}/daterange.cc"
  "${SRC_DIR}/deprecated.cc"
  "${SRC_DIR}/duration.cc"
  "${SRC_DIR}/group.cc"
  "${SRC_DIR}/host.cc"
  "${SRC_DIR}/hostdependency.cc"
  "${SRC_DIR}/object.cc"
  "${SRC_DIR}/parser.cc"
  "${SRC_DIR}/reload.cc"
  "${SRC_DIR}/state.cc"
  "${SRC_DIR}/service.cc"
  "${SRC_DIR}/servicedependency.cc"
  "${SRC_DIR}/timeperiod.cc"
  "${SRC_DIR}/timerange.cc"

  # Headers.
  "${INC_DIR}/command.hh"
  "${INC_DIR}/connector.hh"
  "${INC_DIR}/daterange.hh"
  "${INC_DIR}/deprecated.hh"
  "${INC_DIR}/duration.hh"
  "${INC_DIR}/file_info.hh"
  "${INC_DIR}/group.hh"
  "${INC_DIR}/host.hh"
  "${INC_DIR}/hostdependency.hh"
  "${INC_DIR}/object.hh"
  "${INC_DIR}/parser.hh"
  "${INC_DIR}/reload.hh"
  "${INC_DIR}/servicedependency.hh"
  "${INC_DIR}/service.hh"
  "${INC_DIR}/state.hh"
  "${INC_DIR}/timeperiod.hh"
  "${INC_DIR}/timerange.hh"

  PARENT_SCOPE
)
