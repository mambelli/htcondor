*bosco_install*
================

:index:`bosco_install<single: bosco_install; Bosco commands>`
:index:`bosco_install command`

Synopsis
--------

**bosco_install** [--**help**] | [--**usage**]

**bosco_install** [--**install[=<path/to/release_dir>]**]
[--**prefix=<path>**] [--**install-dir=<path>**]
[--**local-dir=<path>**] [--**make-personal-condor**] [--**bosco**]
[--**type=<[submit][,execute][,manager]>**]
[--**central-manager=<host>**] [--**credd**] [--**owner=<username>**]
[--**maybe-daemon-owner**] [--**install-log=<file>**] [--**overwrite**]
[--**env-scripts-dir=<dir>**] [--**no-env-scripts**]
[--**ignore-missing-libs**] [--**force**] [--**backup**] [--**verbose**]

Description
-----------

*bosco_install* is part of the Bosco system for accessing high
throughput computing resources from a local desktop. For detailed
information, please see the Bosco web site:
`https://osg-bosco.github.io/docs/ <https://osg-bosco.github.io/docs/>`_

*bosco_install* is linked to *condor_install*. The command

::

      bosco_install

becomes

::

      condor_install --bosco

Please see the *condor_install* man page for details of the command
line options.

A Personal HTCondor specialized for Bosco is installed, permitting
central manager tasks and job submission.

Author
------

Center for High Throughput Computing, University of Wisconsin-Madison

Copyright
---------

Copyright © 1990-2019 Center for High Throughput Computing, Computer
Sciences Department, University of Wisconsin-Madison, Madison, WI. All
Rights Reserved. Licensed under the Apache License, Version 2.0.


