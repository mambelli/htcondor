Submitting a Job
================

:index:`submitting<single: submitting; job>`

The *condor_submit* command takes a job description file as input
and submits the job to HTCondor.
:index:`submit description file`\ :index:`submit description<single: submit description; file>`
In the submit description file, HTCondor finds everything it needs to
know about the job. Items such as the name of the executable to run, the
initial working directory, and command-line arguments to the program all
go into the submit description file. *condor_submit* creates a job
ClassAd based upon the information, and HTCondor works toward running
the job. :index:`contents of<single: contents of; submit description file>`

It is easy to submit multiple runs of a program
to HTCondor with a single submit description file. To run the same
program many times with different input data sets, arrange the data files
accordingly so that each run reads its own input, and each run writes
its own output. Each individual run may have its own initial working
directory, files mapped for ``stdin``, ``stdout``, ``stderr``,
command-line arguments, and shell environment.

The :doc:`/man-pages/condor_submit` manual page contains a complete and full
description of how to use *condor_submit*. It also includes descriptions of
all of the many commands that may be placed into a submit description
file. In addition, the index lists entries for each command under the
heading of Submit Commands.

Sample submit description files
-------------------------------

In addition to the examples of submit description files given here,
there are more in the :doc:`/man-pages/condor_submit` manual page.
:index:`examples<single: examples; submit description file>`


**Example 1**

Example 1 is one of the simplest submit description files possible. It
queues the program *myexe* for execution somewhere in the pool.
As this submit description file does not request a specific operating
system to run on, HTCondor will use the default, which is to run the job
on a machine which has the same architecture and operating system 
it was submitted from.

Before submitting a job to HTCondor, it is a good idea to test it
first locally, by running it from a command shell.  This example job
might look like this when run from the shell prompt.

::

      $ ./myexe SomeArgument

      
The corresponding submit description file might look like the following

::

      ####################
      #
      # Example 1
      # Simple HTCondor submit description file
      # Everything with a leading # is a comment
      ####################

      executable   = myexe

      arguments    = SomeArgument

      output       = outputfile
      error        = errorfile
      log          = myexe.log

      request_cpus   = 1
      request_memory = 1024
      request_disk   = 10240
      
      should_transfer_files = yes
      queue

The standard output for this job will go to the file
``outputfile``, as specified by the
**output** :index:`output<single: output; submit commands>` command. Likewise,
the standard error output will go to ``errorfile``. 

HTCondor will append events about the job to a log file wih the 
requested name``myexe.log``. When the job
finishes, its exit conditions and resource usage will also be noted in the log file. 
This file's contents are an excellent way to figure out what happened to jobs.

HTCondor needs to know how many machine resources to allocate to this job.
The ``request_`` lines describe that this job should be allocated 1 cpu core, 1024 
megabytes of memory and 10240 kilobytes of scratch disk space.

Finally, the queue statement tells HTCondor that you are done describing the
job, and to send it to the queue for processing.

**Example 2**

The submit description file for Example 2 queues 150
:index:`running multiple programs`\ runs of program *foo*. 
This job requires machines which have at least
4 GiB of physical memory, one cpu core and 16 Gb of scratch disk.
Each of the 150 runs of the program is given its own HTCondor process number, 
starting with 0. $(Process) is expanded by HTCondor to the actual number
used by each instance of the job. So, ``stdout``, and ``stderr`` will refer to
``out.0``, and ``err.0`` for the first run of the program,
``out.1``, and ``err.1`` for the second run of the program,
and so forth. A log file containing entries about when and where
HTCondor runs, checkpoints, and migrates processes for all the 150
queued programs will be written into the single file ``foo.log``.
If there are 150 or more available slots in your pool, all 150 instances
might be run at the same time, otherwise, HTCondor will run as many as
it can concurrently.

Each instance of this program works on one input file.  The name of this
input file is passed to the program as the only argument.  We prepare
150 copies of this input file in the current directory, and name them
input_file.0, input_file.1 ... up to input_file.149.  Using transfer_input_files,
we tell HTCondor which input file to send to each instance of the program.
::

      ####################
      #
      # Example 2: Show off some fancy features including
      # the use of pre-defined macros.
      #
      ####################

      Executable     = foo
      arguments      = input_file.$(Process)

      
      request_memory = 4096
      request_cpus   = 1
      request_disk   = 16383

      error   = err.$(Process)
      output  = out.$(Process)
      log     = foo.log

      should_transfer_files = yes
      transfer_input_files = input_file.$(Process)

      # submit 150 instances of this job 
      queue 150

:index:`examples<single: examples; submit description file>`

Using the Power and Flexibility of the Queue Command
----------------------------------------------------

A wide variety of job submissions can be specified with extra
information to the **queue** :index:`queue<single: queue; submit commands>`
submit command. This flexibility eliminates the need for a job wrapper
or Perl script for many submissions.

The form of the **queue** command defines variables and expands values,
identifying a set of jobs. Square brackets identify an optional item.

**queue** [**<int expr>** ]

**queue** [**<int expr>** ] [**<varname>** ] **in** [**slice** ]
**<list of items>**

**queue** [**<int expr>** ] [**<varname>** ] **matching** [**files |
dirs** ] [**slice** ] **<list of items with file globbing>**

**queue** [**<int expr>** ] [**<list of varnames>** ] **from**
[**slice** ] **<file name> | <list of items>**

All optional items have defaults:

-  If ``<int expr>`` is not specified, it defaults to the value 1.
-  If ``<varname>`` or ``<list of varnames>`` is not specified, it
   defaults to the single variable called ``ITEM``.
-  If ``slice`` is not specified, it defaults to all elements within the
   list. This is the Python slice ``[::]``, with a step value of 1.
-  If neither ``files`` nor ``dirs`` is specified in a specification
   using the **from** key word, then both files and directories are
   considered when globbing.

The list of items uses syntax in one of two forms. One form is a comma
and/or space separated list; the items are placed on the same line as
the **queue** command. The second form separates items by placing each
list item on its own line, and delimits the list with parentheses. The
opening parenthesis goes on the same line as the **queue** command. The
closing parenthesis goes on its own line. The **queue** command
specified with the key word **from** will always use the second form of
this syntax. Example 3 below uses this second form of syntax. Finally,
the key word **from** accepts a shell command in place of file name, 
followed by a pipe ``|`` (example 4).

The optional ``slice`` specifies a subset of the list of items using the
Python syntax for a slice. Negative step values are not permitted.

Here are a set of examples.


**Example 1**

::

      transfer_input_files = $(filename)
      arguments            = -infile $(filename)
      queue filename matching files *.dat

The use of file globbing expands the list of items to be all files in
the current directory that end in ``.dat``. Only files, and not
directories are considered due to the specification of ``files``. One
job is queued for each file in the list of items. For this example,
assume that the three files ``initial.dat``, ``middle.dat``, and
``ending.dat`` form the list of items after expansion; macro
``filename`` is assigned the value of one of these file names for each
job queued. That macro value is then substituted into the **arguments**
and **transfer_input_files** commands. The **queue** command expands
to

::

      transfer_input_files = initial.dat
      arguments            = -infile initial.dat
      queue
      transfer_input_files = middle.dat
      arguments            = -infile middle.dat
      queue
      transfer_input_files = ending.dat
      arguments            = -infile ending.dat
      queue



**Example 2**

::

      queue 1 input in A, B, C

Variable ``input`` is set to each of the 3 items in the list, and one
job is queued for each. For this example the **queue** command expands
to

::

      input = A
      queue
      input = B
      queue
      input = C
      queue


**Example 3**

::

      queue input,arguments from (
        file1, -a -b 26
        file2, -c -d 92
      )

Using the ``from`` form of the options, each of the two variables
specified is given a value from the list of items. For this example the
**queue** command expands to

::

      input = file1
      arguments = -a -b 26
      queue
      input = file2
      arguments = -c -d 92
      queue

**Example 4**

::

      queue from seq 7 9 |
      
feeds the list of items to queue with the output of ``seq 7 9``:

::

      item = 7
      queue
      item = 8
      queue
      item = 9
      queue

Variables in the Submit Description File
----------------------------------------

:index:`automatic variables<single: automatic variables; submit description file>`
:index:`in submit description file<single: in submit description file; automatic variables>`

There are automatic variables for use within the submit description
file.

``$(Cluster)`` or ``$(ClusterId)``
    Each set of queued jobs from a specific user, submitted from a
    single submit host, sharing an executable have the same value of
    ``$(Cluster)`` or ``$(ClusterId)``. The first cluster of jobs are
    assigned to cluster 0, and the value is incremented by one for each
    new cluster of jobs. ``$(Cluster)`` or ``$(ClusterId)`` will have
    the same value as the job ClassAd attribute ``ClusterId``.

``$(Process)`` or ``$(ProcId)``
    Within a cluster of jobs, each takes on its own unique
    ``$(Process)`` or ``$(ProcId)`` value. The first job has value 0.
    ``$(Process)`` or ``$(ProcId)`` will have the same value as the job
    ClassAd attribute ``ProcId``.

``$(Item)``
    The default name of the variable when no ``<varname>`` is provided
    in a **queue** command.

``$(ItemIndex)``
    Represents an index within a list of items. When no slice is
    specified, the first ``$(ItemIndex)`` is 0. When a slice is
    specified, ``$(ItemIndex)`` is the index of the item within the
    original list.

``$(Step)``
    For the ``<int expr>`` specified, ``$(Step)`` counts, starting at 0.

``$(Row)``
    When a list of items is specified by placing each item on its own
    line in the submit description file, ``$(Row)`` identifies which
    line the item is on. The first item (first line of the list) is
    ``$(Row)`` 0. The second item (second line of the list) is
    ``$(Row)`` 1. When a list of items are specified with all items on
    the same line, ``$(Row)`` is the same as ``$(ItemIndex)``.

Here is an example of a **queue** command for which the values of these
automatic variables are identified.


**Example 1**

This example queues six jobs.

::

    queue 3 in (A, B)

-  ``$(Process)`` takes on the six values 0, 1, 2, 3, 4, and 5.
-  Because there is no specification for the ``<varname>`` within this
   **queue** command, variable ``$(Item)`` is defined. It has the value
   ``A`` for the first three jobs queued, and it has the value ``B`` for
   the second three jobs queued.
-  ``$(Step)`` takes on the three values 0, 1, and 2 for the three jobs
   with ``$(Item)=A``, and it takes on the same three values 0, 1, and 2
   for the three jobs with ``$(Item)=B``.
-  ``$(ItemIndex)`` is 0 for all three jobs with ``$(Item)=A``, and it
   is 1 for all three jobs with ``$(Item)=B``.
-  ``$(Row)`` has the same value as ``$(ItemIndex)`` for this example.


Including Submit Commands Defined Elsewhere
-------------------------------------------

:index:`including commands from elsewhere<single: including commands from elsewhere; submit description file>`

Externally defined submit commands can be incorporated into the submit
description file using the syntax

::

      include : <what-to-include>

The <what-to-include> specification may specify a single file, where the
contents of the file will be incorporated into the submit description
file at the point within the file where the **include** is. Or,
<what-to-include> may cause a program to be executed, where the output
of the program is incorporated into the submit description file. The
specification of <what-to-include> has the bar character (``|``)
following the name of the program to be executed.

The **include** key word is case insensitive. There are no requirements
for white space characters surrounding the colon character.

Included submit commands may contain further nested **include**
specifications, which are also parsed, evaluated, and incorporated.
Levels of nesting on included files are limited, such that infinite
nesting is discovered and thwarted, while still permitting nesting.

Consider the example

::

      include : list-infiles.sh |

In this example, the bar character at the end of the line causes the
script ``list-infiles.sh`` to be invoked, and the output of the script
is parsed and incorporated into the submit description file. If this
bash script is in the PATH when submit is run, and contains

::

      #!/bin/sh

      echo "transfer_input_files = `ls -m infiles/*.dat`"
      exit 0

then the output of this script has specified the set of input files to
transfer to the execute host. For example, if directory ``infiles``
contains the three files ``A.dat``, ``B.dat``, and ``C.dat``, then the
submit command

::

      transfer_input_files = infiles/A.dat, infiles/B.dat, infiles/C.dat

is incorporated into the submit description file.


Using Conditionals in the Submit Description File
-------------------------------------------------

:index:`IF/ELSE syntax<single: IF/ELSE syntax; submit commands>`
:index:`IF/ELSE submit commands syntax`

Conditional if/else semantics are available in a limited form. The
syntax:

::

      if <simple condition>
         <statement>
         . . .
         <statement>
      else
         <statement>
         . . .
         <statement>
      endif

An else key word and statements are not required, such that simple if
semantics are implemented. The <simple condition> does not permit
compound conditions. It optionally contains the exclamation point
character (!) to represent the not operation, followed by

-  the defined keyword followed by the name of a variable. If the
   variable is defined, the statement(s) are incorporated into the
   expanded input. If the variable is not defined, the statement(s) are
   not incorporated into the expanded input. As an example,

   ::

         if defined MY_UNDEFINED_VARIABLE
            X = 12
         else
            X = -1
         endif

   results in ``X = -1``, when ``MY_UNDEFINED_VARIABLE`` is not yet
   defined.

-  the version keyword, representing the version number of of the daemon
   or tool currently reading this conditional. This keyword is followed
   by an HTCondor version number. That version number can be of the form
   x.y.z or x.y. The version of the daemon or tool is compared to the
   specified version number. The comparison operators are

   -  == for equality. Current version 8.2.3 is equal to 8.2.
   -  >= to see if the current version number is greater than or equal
      to. Current version 8.2.3 is greater than 8.2.2, and current
      version 8.2.3 is greater than or equal to 8.2.
   -  <= to see if the current version number is less than or equal to.
      Current version 8.2.0 is less than 8.2.2, and current version
      8.2.3 is less than or equal to 8.2.

   As an example,

   ::

         if version >= 8.1.6
            DO_X = True
         else
            DO_Y = True
         endif

   results in defining ``DO_X`` as ``True`` if the current version of
   the daemon or tool reading this if statement is 8.1.6 or a more
   recent version.

-  True or yes or the value 1. The statement(s) are incorporated.
-  False or no or the value 0 The statement(s) are not incorporated.
-  $(<variable>) may be used where the immediately evaluated value is a
   simple boolean value. A value that evaluates to the empty string is
   considered False, otherwise a value that does not evaluate to a
   simple boolean value is a syntax error.

The syntax

::

      if <simple condition>
         <statement>
         . . .
         <statement>
      elif <simple condition>
         <statement>
         . . .
         <statement>
      endif

is the same as syntax

::

      if <simple condition>
         <statement>
         . . .
         <statement>
      else
         if <simple condition>
            <statement>
            . . .
            <statement>
         endif
      endif

Here is an example use of a conditional in the submit description file.
A portion of the ``sample.sub`` submit description file uses the if/else
syntax to define command line arguments in one of two ways:

::

      if defined X
        arguments = -n $(X)
      else
        arguments = -n 1 -debug
      endif

Submit variable ``X`` is defined on the *condor_submit* command line
with

::

      condor_submit  X=3  sample.sub

This command line incorporates the submit command ``X = 3`` into the
submission before parsing the submit description file. For this
submission, the command line arguments of the submitted job become

::

        -n 3

If the job were instead submitted with the command line

::

      condor_submit  sample.sub

then the command line arguments of the submitted job become

::

        -n 1 -debug


Function Macros in the Submit Description File
----------------------------------------------

:index:`function macros<single: function macros; submit description file>`

A set of predefined functions increase flexibility. Both submit
description files and configuration files are read using the same
parser, so these functions may be used in both submit description files
and configuration files.

Case is significant in the function's name, so use the same letter case
as given in these definitions.

``$CHOICE(index, listname)`` or ``$CHOICE(index, item1, item2, ...)``
    An item within the list is returned. The list is represented by a
    parameter name, or the list items are the parameters. The ``index``
    parameter determines which item. The first item in the list is at
    index 0. If the index is out of bounds for the list contents, an
    error occurs.

``$ENV(environment-variable-name[:default-value])``
    Evaluates to the value of environment variable
    ``environment-variable-name``. If there is no environment variable
    with that name, Evaluates to UNDEFINED unless the optional
    :default-value is used; in which case it evaluates to default-value.
    For example,

    ::

          A = $ENV(HOME)

    binds ``A`` to the value of the ``HOME`` environment variable.

``$F[fpduwnxbqa](filename)``
    One or more of the lower case letters may be combined to form the
    function name and thus, its functionality. Each letter operates on
    the ``filename`` in its own way.

    -  ``f`` convert relative path to full path by prefixing the current
       working directory to it. This option works only in
       *condor_submit* files.
    -  ``p`` refers to the entire directory portion of ``filename``,
       with a trailing slash or backslash character. Whether a slash or
       backslash is used depends on the platform of the machine. The
       slash will be recognized on Linux platforms; either a slash or
       backslash will be recognized on Windows platforms, and the parser
       will use the same character specified.
    -  ``d`` refers to the last portion of the directory within the
       path, if specified. It will have a trailing slash or backslash,
       as appropriate to the platform of the machine. The slash will be
       recognized on Linux platforms; either a slash or backslash will
       be recognized on Windows platforms, and the parser will use the
       same character specified unless u or w is used. if b is used the
       trailing slash or backslash will be omitted.
    -  ``u`` convert path separators to Unix style slash characters
    -  ``w`` convert path separators to Windows style backslash
       characters
    -  ``n`` refers to the file name at the end of any path, but without
       any file name extension. As an example, the return value from
       ``$Fn(/tmp/simulate.exe)`` will be ``simulate`` (without the
       ``.exe`` extension).
    -  ``x`` refers to a file name extension, with the associated period
       (``.``). As an example, the return value from
       ``$Fn(/tmp/simulate.exe)`` will be ``.exe``.
    -  ``b`` when combined with the d option, causes the trailing slash
       or backslash to be omitted. When combined with the x option,
       causes the leading period (``.``) to be omitted.
    -  ``q`` causes the return value to be enclosed within quotes.
       Double quote marks are used unless a is also specified.
    -  ``a`` When combined with the q option, causes the return value to
       be enclosed within single quotes.

``$DIRNAME(filename)`` is the same as ``$Fp(filename)``

``$BASENAME(filename)`` is the same as ``$Fnx(filename)``

``$INT(item-to-convert)`` or ``$INT(item-to-convert, format-specifier)``
    Expands, evaluates, and returns a string version of
    ``item-to-convert``. The ``format-specifier`` has the same syntax as
    a C language or Perl format specifier. If no ``format-specifier`` is
    specified, "%d" is used as the format specifier.

``$RANDOM_CHOICE(choice1, choice2, choice3, ...)``
    :index:`$RANDOM_CHOICE() function macro` A random choice
    of one of the parameters in the list of parameters is made. For
    example, if one of the integers 0-8 (inclusive) should be randomly
    chosen:

    ::

          $RANDOM_CHOICE(0,1,2,3,4,5,6,7,8)

``$RANDOM_INTEGER(min, max [, step])``
    :index:`in configuration<single: in configuration; $RANDOM_INTEGER()>` A random integer
    within the range min and max, inclusive, is selected. The optional
    step parameter controls the stride within the range, and it defaults
    to the value 1. For example, to randomly chose an even integer in
    the range 0-8 (inclusive):

    ::

          $RANDOM_INTEGER(0, 8, 2)

``$REAL(item-to-convert)`` or ``$REAL(item-to-convert, format-specifier)``
    Expands, evaluates, and returns a string version of
    ``item-to-convert`` for a floating point type. The
    ``format-specifier`` is a C language or Perl format specifier. If no
    ``format-specifier`` is specified, "%16G" is used as a format
    specifier.

``$SUBSTR(name, start-index)`` or ``$SUBSTR(name, start-index, length)``
    Expands name and returns a substring of it. The first character of
    the string is at index 0. The first character of the substring is at
    index start-index. If the optional length is not specified, then the
    substring includes characters up to the end of the string. A
    negative value of start-index works back from the end of the string.
    A negative value of length eliminates use of characters from the end
    of the string. Here are some examples that all assume

    ::

          Name = abcdef

    -  ``$SUBSTR(Name, 2)`` is ``cdef``.
    -  ``$SUBSTR(Name, 0, -2)`` is ``abcd``.
    -  ``$SUBSTR(Name, 1, 3)`` is ``bcd``.
    -  ``$SUBSTR(Name, -1)`` is ``f``.
    -  ``$SUBSTR(Name, 4, -3)`` is the empty string, as there are no
       characters in the substring for this request.

Here are example uses of the function macros in a submit description
file. Note that these are not complete submit description files, but
only the portions that promote understanding of use cases of the
function macros.


**Example 1**

Generate a range of numerical values for a set of jobs, where values
other than those given by $(Process) are desired.

::

      MyIndex     = $(Process) + 1
      initial_dir = run-$INT(MyIndex,%04d)

Assuming that there are three jobs queued, such that $(Process) becomes
0, 1, and 2, ``initial_dir`` will evaluate to the directories
``run-0001``, ``run-0002``, and ``run-0003``.


**Example 2**

This variation on Example 1 generates a file name extension which is a
3-digit integer value.

::

      Values     = $(Process) * 10
      Extension  = $INT(Values,%03d)
      input      = X.$(Extension)

Assuming that there are four jobs queued, such that $(Process) becomes
0, 1, 2, and 3, ``Extension`` will evaluate to 000, 010, 020, and 030,
leading to files defined for **input** of ``X.000``, ``X.010``,
``X.020``, and ``X.030``.


**Example 3**

This example uses both the file globbing of the
**queue** :index:`queue<single: queue; submit commands>` command and a macro
function to specify a job input file that is within a subdirectory on
the submit host, but will be placed into a single, flat directory on the
execute host.

::

      arguments            = $Fnx(FILE)
      transfer_input_files = $(FILE)
      queue  FILE  MATCHING (
           samplerun/*.dat
           )

Assume that two files that end in ``.dat``, ``A.dat`` and ``B.dat``, are
within the directory ``samplerun``. Macro ``FILE`` expands to
``samplerun/A.dat`` and ``samplerun/B.dat`` for the two jobs queued. The
input files transferred are ``samplerun/A.dat`` and ``samplerun/B.dat``
on the submit host. The ``$Fnx()`` function macro expands to the
complete file name with any leading directory specification stripped,
such that the command line argument for one of the jobs will be
``A.dat`` and the command line argument for the other job will be
``B.dat``.


About Requirements and Rank
---------------------------

The ``requirements`` and ``rank`` commands in the submit description
file are powerful and flexible.
:index:`requirements<single: requirements; submit commands>`\ :index:`requirements attribute`
:index:`rank attribute`\ :index:`requirements<single: requirements; ClassAd attribute>`
:index:`rank<single: rank; ClassAd attribute>`\ Using them effectively requires
care, and this section presents those details.

Both ``requirements`` and ``rank`` need to be specified as valid
HTCondor ClassAd expressions, however, default values are set by the
*condor_submit* program if these are not defined in the submit
description file. From the *condor_submit* manual page and the above
examples, you see that writing ClassAd expressions is intuitive,
especially if you are familiar with the programming language C. There
are some pretty nifty expressions you can write with ClassAds. A
complete description of ClassAds and their expressions can be found in
the :doc:`/misc-concepts/classad-mechanism` section.

All of the commands in the submit description file are case insensitive,
except for the ClassAd attribute string values. ClassAd attribute names
are case insensitive, but ClassAd string values are case preserving.

Note that the comparison operators (<, >, <=, >=, and ==) compare
strings case insensitively. The special comparison operators =?= and =!=
compare strings case sensitively.

A **requirements** :index:`requirements<single: requirements; submit commands>` or
**rank** :index:`rank<single: rank; submit commands>` command in the submit
description file may utilize attributes that appear in a machine or a
job ClassAd. Within the submit description file (for a job) the prefix
MY. (on a ClassAd attribute name) causes a reference to the job ClassAd
attribute, and the prefix TARGET. causes a reference to a potential
machine or matched machine ClassAd attribute.

The *condor_status* command displays
:index:`condor_status<single: condor_status; HTCondor commands>`\ statistics about
machines within the pool. The **-l** option displays the machine ClassAd
attributes for all machines in the HTCondor pool. The job ClassAds, if
there are jobs in the queue, can be seen with the *condor_q -l*
command. This shows all the defined attributes for current jobs in the
queue.

A list of defined ClassAd attributes for job ClassAds is given in the
Appendix on the :doc:`/classad-attributes/job-classad-attributes` page. A
list of defined ClassAd attributes for machine ClassAds is given in the
Appendix on the :doc:`/classad-attributes/machine-classad-attributes` page.

Rank Expression Examples
''''''''''''''''''''''''

:index:`examples<single: examples; rank attribute>`
:index:`rank examples<single: rank examples; ClassAd attribute>`
:index:`rank<single: rank; submit commands>`

When considering the match between a job and a machine, rank is used to
choose a match from among all machines that satisfy the job's
requirements and are available to the user, after accounting for the
user's priority and the machine's rank of the job. The rank expressions,
simple or complex, define a numerical value that expresses preferences.

The job's ``Rank`` expression evaluates to one of three values. It can
be UNDEFINED, ERROR, or a floating point value. If ``Rank`` evaluates to
a floating point value, the best match will be the one with the largest,
positive value. If no ``Rank`` is given in the submit description file,
then HTCondor substitutes a default value of 0.0 when considering
machines to match. If the job's ``Rank`` of a given machine evaluates to
UNDEFINED or ERROR, this same value of 0.0 is used. Therefore, the
machine is still considered for a match, but has no ranking above any
other.

A boolean expression evaluates to the numerical value of 1.0 if true,
and 0.0 if false.

The following ``Rank`` expressions provide examples to follow.

For a job that desires the machine with the most available memory:

::

       Rank = memory

For a job that prefers to run on a friend's machine on Saturdays and
Sundays:

::

       Rank = ( (clockday == 0) || (clockday == 6) )
              && (machine == "friend.cs.wisc.edu")

For a job that prefers to run on one of three specific machines:

::

       Rank = (machine == "friend1.cs.wisc.edu") ||
              (machine == "friend2.cs.wisc.edu") ||
              (machine == "friend3.cs.wisc.edu")

For a job that wants the machine with the best floating point
performance (on Linpack benchmarks):

::

       Rank = kflops

This particular example highlights a difficulty with ``Rank`` expression
evaluation as currently defined. While all machines have floating point
processing ability, not all machines will have the ``kflops`` attribute
defined. For machines where this attribute is not defined, ``Rank`` will
evaluate to the value UNDEFINED, and HTCondor will use a default rank of
the machine of 0.0. The ``Rank`` attribute will only rank machines where
the attribute is defined. Therefore, the machine with the highest
floating point performance may not be the one given the highest rank.

So, it is wise when writing a ``Rank`` expression to check if the
expression's evaluation will lead to the expected resulting ranking of
machines. This can be accomplished using the *condor_status* command
with the *-constraint* argument. This allows the user to see a list of
machines that fit a constraint. To see which machines in the pool have
``kflops`` defined, use

::

    condor_status -constraint kflops

Alternatively, to see a list of machines where ``kflops`` is not
defined, use

::

    condor_status -constraint "kflops=?=undefined"

For a job that prefers specific machines in a specific order:

::

       Rank = ((machine == "friend1.cs.wisc.edu")*3) +
              ((machine == "friend2.cs.wisc.edu")*2) +
               (machine == "friend3.cs.wisc.edu")

If the machine being ranked is ``friend1.cs.wisc.edu``, then the
expression

::

       (machine == "friend1.cs.wisc.edu")

is true, and gives the value 1.0. The expressions

::

       (machine == "friend2.cs.wisc.edu")

and

::

       (machine == "friend3.cs.wisc.edu")

are false, and give the value 0.0. Therefore, ``Rank`` evaluates to the
value 3.0. In this way, machine ``friend1.cs.wisc.edu`` is ranked higher
than machine ``friend2.cs.wisc.edu``, machine ``friend2.cs.wisc.edu`` is
ranked higher than machine ``friend3.cs.wisc.edu``, and all three of
these machines are ranked higher than others.

Submitting Jobs Using a Shared File System
------------------------------------------

:index:`submission using a shared file system<single: submission using a shared file system; job>`
:index:`submission of jobs<single: submission of jobs; shared file system>`

If vanilla, java, or parallel universe jobs are submitted without using
the File Transfer mechanism, HTCondor must use a shared file system to
access input and output files. In this case, the job must be able to
access the data files from any machine on which it could potentially
run.

As an example, suppose a job is submitted from blackbird.cs.wisc.edu,
and the job requires a particular data file called
``/u/p/s/psilord/data.txt``. If the job were to run on
cardinal.cs.wisc.edu, the file ``/u/p/s/psilord/data.txt`` must be
available through either NFS or AFS for the job to run correctly.

HTCondor allows users to ensure their jobs have access to the right
shared files by using the ``FileSystemDomain`` and ``UidDomain`` machine
ClassAd attributes. These attributes specify which machines have access
to the same shared file systems. All machines that mount the same shared
directories in the same locations are considered to belong to the same
file system domain. Similarly, all machines that share the same user
information (in particular, the same UID, which is important for file
systems like NFS) are considered part of the same UID domain.

The default configuration for HTCondor places each machine in its own
UID domain and file system domain, using the full host name of the
machine as the name of the domains. So, if a pool does have access to a
shared file system, the pool administrator must correctly configure
HTCondor such that all the machines mounting the same files have the
same ``FileSystemDomain`` configuration. Similarly, all machines that
share common user information must be configured to have the same
``UidDomain`` configuration.

When a job relies on a shared file system, HTCondor uses the
``requirements`` expression to ensure that the job runs on a machine in
the correct ``UidDomain`` and ``FileSystemDomain``. In this case, the
default ``requirements`` expression specifies that the job must run on a
machine with the same ``UidDomain`` and ``FileSystemDomain`` as the
machine from which the job is submitted. This default is almost always
correct. However, in a pool spanning multiple ``UidDomain``\ s and/or
``FileSystemDomain``\ s, the user may need to specify a different
``requirements`` expression to have the job run on the correct machines.

For example, imagine a pool made up of both desktop workstations and a
dedicated compute cluster. Most of the pool, including the compute
cluster, has access to a shared file system, but some of the desktop
machines do not. In this case, the administrators would probably define
the ``FileSystemDomain`` to be ``cs.wisc.edu`` for all the machines that
mounted the shared files, and to the full host name for each machine
that did not. An example is ``jimi.cs.wisc.edu``.

In this example, a user wants to submit vanilla universe jobs from her
own desktop machine (jimi.cs.wisc.edu) which does not mount the shared
file system (and is therefore in its own file system domain, in its own
world). But, she wants the jobs to be able to run on more than just her
own machine (in particular, the compute cluster), so she puts the
program and input files onto the shared file system. When she submits
the jobs, she needs to tell HTCondor to send them to machines that have
access to that shared data, so she specifies a different
``requirements`` expression than the default:

::

       Requirements = TARGET.UidDomain == "cs.wisc.edu" && \
                      TARGET.FileSystemDomain == "cs.wisc.edu"

WARNING: If there is no shared file system, or the HTCondor pool
administrator does not configure the ``FileSystemDomain`` setting
correctly (the default is that each machine in a pool is in its own file
system and UID domain), a user submits a job that cannot use remote
system calls (for example, a vanilla universe job), and the user does
not enable HTCondor's File Transfer mechanism, the job will only run on
the machine from which it was submitted.

Jobs That Require GPUs
----------------------

:index:`requesting GPUs for a job<single: requesting GPUs for a job; GPUs>`

A job that needs GPUs to run identifies the number of GPUs needed in the
submit description file by adding the submit command

::

      request_GPUs = <n>

where ``<n>`` is replaced by the integer quantity of GPUs required for
the job. For example, a job that needs 1 GPU uses

::

      request_GPUs = 1

Because there are different capabilities among GPUs, the job might need
to further qualify which GPU of available ones is required. Do this by
specifying or adding a clause to an existing
**Requirements** :index:`Requirements<single: Requirements; submit commands>` submit
command. As an example, assume that the job needs a speed and capacity
of a CUDA GPU that meets or exceeds the value 1.2. In the submit
description file, place

::

      request_GPUs = 1
      requirements = (CUDACapability >= 1.2) && $(requirements:True)

Access to GPU resources by an HTCondor job needs special configuration
of the machines that offer GPUs. Details of how to set up the
configuration are in the :doc:`/admin-manual/policy-configuration` section.

Interactive Jobs
----------------

:index:`interactive<single: interactive; job>` :index:`interactive jobs`

An interactive job is a Condor job that is provisioned and scheduled
like any other vanilla universe Condor job onto an execute machine
within the pool. The result of a running interactive job is a shell
prompt issued on the execute machine where the job runs. The user that
submitted the interactive job may then use the shell as desired, perhaps
to interactively run an instance of what is to become a Condor job. This
might aid in checking that the set up and execution environment are
correct, or it might provide information on the RAM or disk space
needed. This job (shell) continues until the user logs out or any other
policy implementation causes the job to stop running. A useful feature
of the interactive job is that the users and jobs are accounted for
within Condor's scheduling and priority system.

Neither the submit nor the execute host for interactive jobs may be on
Windows platforms.

The current working directory of the shell will be the initial working
directory of the running job. The shell type will be the default for the
user that submits the job. At the shell prompt, X11 forwarding is
enabled.

Each interactive job will have a job ClassAd attribute of

::

      InteractiveJob = True

Submission of an interactive job specifies the option **-interactive**
on the *condor_submit* command line.

A submit description file may be specified for this interactive job.
Within this submit description file, a specification of these 5 commands
will be either ignored or altered:

#. **executable** :index:`executable<single: executable; submit commands>`
#. **transfer_executable** :index:`transfer_executable<single: transfer_executable; submit commands>`
#. **arguments** :index:`arguments<single: arguments; submit commands>`
#. **universe** :index:`universe<single: universe; submit commands>`. The
   interactive job is a vanilla universe job.
#. **queue** :index:`queue<single: queue; submit commands>` **<n>**. In this
   case the value of **<n>** is ignored; exactly one interactive job is
   queued.

The submit description file may specify anything else needed for the
interactive job, such as files to transfer.

If no submit description file is specified for the job, a default one is
utilized as identified by the value of the configuration variable
``INTERACTIVE_SUBMIT_FILE`` :index:`INTERACTIVE_SUBMIT_FILE`.

Here are examples of situations where interactive jobs may be of
benefit.

-  An application that cannot be batch processed might be run as an
   interactive job. Where input or output cannot be captured in a file
   and the executable may not be modified, the interactive nature of the
   job may still be run on a pool machine, and within the purview of
   Condor.
-  A pool machine with specialized hardware that requires interactive
   handling can be scheduled with an interactive job that utilizes the
   hardware.
-  The debugging and set up of complex jobs or environments may benefit
   from an interactive session. This interactive session provides the
   opportunity to run scripts or applications, and as errors are
   identified, they can be corrected on the spot.
-  Development may have an interactive nature, and proceed more quickly
   when done on a pool machine. It may also be that the development
   platforms required reside within Condor's purview as execute hosts.


