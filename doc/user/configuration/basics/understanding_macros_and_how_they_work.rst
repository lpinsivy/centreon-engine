.. _understanding_macros:

Understanding Macros and How They Work
**************************************

Macros
======

One of the main features that make Centreon Engine so flexible is the
ability to use macros in command defintions. Macros allow you to
reference information from hosts, services, and other sources in your
commands.

Macro Substitution - How Macros Work
====================================

Before Centreon Engine executes a command, it will replace any macros it
finds in the command definition with their corresponding values. This
macro substitution occurs for all types of commands that Centreon Engine
executes - host and service checks, event handlers, etc.

Example 1: Host Address Macro
=============================

When you use host and service macros in command definitions, they refer
to values for the host or service for which the command is being
run. Let's try an example. Assuming we are using a host definition and a
check_ping command defined like this::

  define host{
    host_name     linuxbox
    address       192.168.1.2
    check_command check_ping
    ...
  }

  define command{
    command_name check_ping
    command_line /usr/lib/nagios/plugins/check_ping -H $HOSTADDRESS$ -w 100.0,90% -c 200.0,60%
  }

The expanded/final command line to be executed for the host's check
command would look like this::

  $ /usr/lib/nagios/plugins/check_ping -H 192.168.1.2 -w 100.0,90% -c 200.0,60%

Pretty simple, right? The beauty in this is that you can use a single
command definition to check an unlimited number of hosts. Each host can
be checked with the same command definition because each host's address
is automatically substituted in the command line before execution.

Example 2: Command Argument Macros
==================================

You can pass arguments to commands as well, which is quite handy if
you'd like to keep your command definitions rather generic. Arguments
are specified in the object (i.e. host or service) definition, by
separating them from the command name with exclamation points (!) like
so::

  define service{
    host_name           linuxbox
    service_description PING
    check_command       check_ping!200.0,80%!400.0,40%
    ...
  }

In the example above, the service check command has two arguments (which
can be referenced with `ARGn` macros). The $ARG1$ macro will be
"200.0,80%" and $ARG2$ will be "400.0,40%" (both without
quotes). Assuming we are using the host definition given earlier and a
check_ping command defined like this::

  define command{
    command_name check_ping
    command_line /usr/lib/nagios/plugins/check_ping -H $HOSTADDRESS$ -w $ARG1$ -c $ARG2$
  }

The expanded/final command line to be executed for the service's check
command would look like this::

  $ /usr/lib/nagios/plugins/check_ping -H 192.168.1.2 -w 200.0,80% -c 400.0,40%

.. note::
   If you need to pass bang (!) characters in your command arguments,
   you can do so by escaping them with a backslash (\). If you need to
   include backslashes in your command arguments, they should also be
   escaped with a backslash.

On-Demand Macros
================

Normally when you use host and service macros in command definitions,
they refer to values for the host or service for which the command is
being run. For instance, if a host check command is being executed for a
host named "linuxbox", all the
:ref:`standard host macros <standard_macros>` will refer to values for
that host ("linuxbox").

If you would like to reference values for another host or service in a
command (for which the command is not being run), you can use what are
called "on-demand" macros. On-demand macros look like normal macros,
except for the fact that they contain an identifier for the host or
service from which they should get their value. Here's the basic format
for on-demand macros::

  * **HOSTMACRONAME:** host_name
  * **SERVICEMACRONAME:** host_name:service_description

Replace HOSTMACRONAME and SERVICEMACRONAME with the name of one of the
standard host of service macros found :ref:`here <standard_macros>`.

Note that the macro name is separated from the host or service
identifier by a colon (:). For on-demand service macros, the service
identifier consists of both a host name and a service description -
these are separated by a colon (:) as well.

.. note::
   On-demand service macros can contain an empty host name field. In
   this case the name of the host associated with the service will
   automatically be used.

Examples of on-demand host and service macros follow::

  $SERVICESTATEID:novellserver:DS Database$ <--- On-demand service macro
  $SERVICESTATEID::CPU Load$                <--- On-demand service macro with blank host name field

Custom Variable Macros
======================

Any :ref:`custom object variables <custom_object_variables>`
that you define in host or service definitions are also available as
macros. Custom variable macros are named as follows:

  * $_HOSTvarname$
  * $_SERVICEvarname$

Take the following host definition with a custom variable called
"_MACADDRESS"::

  define host{
    host_name linuxbox
    address   192.168.1.1
    _MACADDRESS 00:01:02:03:04:05
    ...
  }

The _MACADDRESS custom variable would be available in a macro called
$_HOSTMACADDRESS$. More information on custom object variables and how
they can be used in macros can be found
:ref:`here <custom_object_variables>`.

Macro Cleansing
===============

Some macros are stripped of potentially dangerous shell metacharacters
before being substituted into commands to be executed. Which characters
are stripped from the macros depends on the setting of the
:ref:`illegal_macro_output_chars <main_cfg_opt_illegal_macro_output_characters>`
directive. The following macros are stripped of potentially dangerous
characters:

  * :ref:`HOSTOUTPUT <user_configuration_macros_host>`
  * :ref:`LONGHOSTOUTPUT <user_configuration_macros_host>`
  * :ref:`HOSTPERFDATA <user_configuration_macros_host>`
  * :ref:`SERVICEOUTPUT <user_configuration_macros_service>`
  * :ref:`LONGSERVICEOUTPUT <user_configuration_macros_service>`
  * :ref:`SERVICEPERFDATA <user_configuration_macros_service>`
  * :ref:`SERVICEACKAUTHOR <user_configuration_macros_service>`
  * :ref:`SERVICEACKCOMMENT <user_configuration_macros_service>`

Additionally, any macros that contain
:ref:`custom variables <custom_object_variables>` are stripped for
safety and security.

Available Macros
================

A list of all the macros that are available in Centreon Engine, as well
as a chart of when they can be used, can be found
:ref:`here <standard_macros>`.
