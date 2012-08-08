/*
** Copyright 2011-2012 Merethis
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

#ifndef TEST_UNITTEST_HH
#  define TEST_UNITTEST_HH

#  include <cstdlib>
#  include <iostream>
#  include "com/centreon/clib.hh"
#  include "com/centreon/engine/broker/compatibility.hh"
#  include "com/centreon/engine/broker/loader.hh"
#  include "com/centreon/engine/checks/checker.hh"
#  include "com/centreon/engine/commands/set.hh"
#  include "com/centreon/engine/events/loop.hh"
#  include "com/centreon/engine/logging/engine.hh"
#  include "com/centreon/engine/namespace.hh"

CCE_BEGIN()

/**
 *  @class unittest unittest.hh
 *  @brief Class unittest init and destroy all
 *  engine needs to make unit test and run
 *  unit test.
 */
class    unittest {
public:
  /**
   *  Constructor.
   *
   *  @param[in] argc Argument count.
   *  @param[in] argv Argument values.
   *  @param[in] func Unit test routine.
   *
   *  @return Return value of func.
   */
         unittest(int argc, char** argv, int (* func)(int, char**))
    : _argc(argc), _argv(argv), _func(func) {}

  /**
   *  Destructor.
   */
         ~unittest() throw () {}

  /**
   *  Entry point.
   *
   *  @return Return value of unit test routine.
   */
  int    run() {
    int ret(EXIT_FAILURE);
    try {
      _init();
      ret = (*_func)(_argc, _argv);
      _deinit();
    }
    catch (std::exception const& e) {
      std::cerr << "error: " << e.what() << std::endl;
    }
    catch (...) {
      std::cerr << "error: unknown exception" << std::endl;
    }
    return (ret);
  }

private:
  void   _init() {
    com::centreon::clib::load();
    logging::engine::load();
    commands::set::load();
    checks::checker::load();
    events::loop::load();
    broker::loader::load();
    broker::compatibility::load();
    return ;
  }

  void   _deinit() {
    broker::compatibility::unload();
    broker::loader::unload();
    events::loop::unload();
    checks::checker::unload();
    commands::set::unload();
    logging::engine::unload();
    com::centreon::clib::unload();
    return ;
  }

  int    _argc;
  char** _argv;
  int    (*_func)(int, char**);
};

CCE_END()

#endif // !TEST_UNITTEST_HH
