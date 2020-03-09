HTCondor Containers
===================

Currently, we provide two kinds of containers for HTCondor services:
the Minicondor container (`htcondor/mini`) and the Execute Node container
(`htcondor/execute`).


Using the Minicondor Container
------------------------------

### Overview

The minicondor container is an install with all of the HTCondor daemons
running, only listening on local interfaces.  This is useful for
experimentation and learning.

Start the container by running:
```console
dockerhost$ docker run --detach \
                --name=minicondor \
                htcondor/mini:el7
```
Then, enter the container by running:
```console
dockerhost$ docker exec -ti minicondor /bin/bash
```

You can submit jobs by first becoming the `submituser` user:
```console
container$ su - submituser
```


Using the Execute Node Container
--------------------------------

### Overview

The execute node container can connect to an existing HTCondor pool via
token authentication (recommended) or pool password.  Token authentication
requires HTCondor 8.9.2+ on both sides of the connection.  Because of CCB, this
container needs no inbound connectivity, and only needs outbound connectivity
to the central manager and the submit host, even for running jobs or using
`condor_ssh_to_job`.

You must specify the address of the pool's central manager in the
`CONDOR_HOST` environment variable.

If you are using token auth, you must have a token with the
`ADVERTISE_MASTER` and `ADVERTISE_STARTD` privileges mounted at
`/root/secrets/token` inside the container.

Otherwise, if you are using pool password auth, you must have a copy of
the pool password file mounted at `/root/secrets/pool_password` inside the
container, and specify `USE_POOL_PASSWORD=yes` in the container environment.

Here are the environment variables that the container can use:
- `CONDOR_HOST` (required): the address of the central manager
- `NUM_CPUS` (optional, default 1): the number of CPUs to advertise
- `MEMORY` (optional, default 1024): the amount of total memory (in MB)
  to advertise
- `RESERVED_DISK` (optional, default 1024): the amount of disk space
  (in MB) to reserve for non-HTCondor processes
- `USE_POOL_PASSWORD` (optional, default no): set this to `yes` to use
  pool password authentication

In addition, you can add more HTCondor configuration by putting it in
`/root/config/*.conf` files in the container.


### Example

This example is for creating an execute node and adding it to a pool using
token auth.  The execute node will have one partitionable slot with 2 CPUs
and 4 GB of RAM, and will use the identity `dockerworker@example.net`.
The central manager has the hostname `cm.example.net`.  The container is
run by the user `user` on the host `dockerhost.example.net`.

Create a directory for holding the token:
```console
dockerhost$ mkdir -p ~/condorexec/secrets
dockerhost$ chmod 0700 ~/condorexec/secrets
```

Grant access to the identity that the container will use.  On the central
manager, add the following lines to the HTCondor configuration:
```
ALLOW_ADVERTISE_MASTER = \
    $(ALLOW_ADVERTISE_MASTER) \
    $(ALLOW_WRITE_COLLECTOR) \
    dockerworker@example.net
ALLOW_ADVERTISE_STARTD = \
    $(ALLOW_ADVERTISE_STARTD) \
    $(ALLOW_WRITE_COLLECTOR) \
    dockerworker@example.net
```
Run `condor_reconfig` on the central manager to pick up the changes.

Create a token for the execute node.  On the central manager:
```console
cm$ sudo condor_create_token -authz ADVERTISE_MASTER \
         -authz ADVERTISE_STARTD -identity dockerworker@example.net \
         dockerworker_token
cm$ sudo scp user@dockerhost.example.net:volumes/condorexec/secrets/token
```

On the Docker host, create an environment file for the container:
```console
dockerhost$ echo 'CONDOR_HOST=cm.example.net' > ~/condorexec/env
dockerhost$ echo 'NUM_CPUS=2' >> ~/condorexec/env
dockerhost$ echo 'MEMORY=4096' >> ~/condorexec/env
```

Start the container:
```console
dockerhost$ docker run --detach --env-file=~/condorexec/env \
                -v ~/condorexec/secrets:/root/secrets:ro \
                --name=htcondor-execute \
                --cpus=2
                --memory-reservation=$(( 4096 * 1048576 )) \
                htcondor/execute:el7
```

To verify the container is functioning, use `docker ps` to get the name
of the container, then run:
```console
dockerhost$ docker exec -ti <container name> /bin/bash
container$ condor_status
```
You should see the container in the output of `condor_status`.


### Adding additional software to the image

To add additional software to the execute node, create your own Dockerfile
that uses the execute node image as a base, and installs the additional
software.  For example, to install numpy, put this in a Dockerfile:
```dockerfile
FROM htcondor/execute:8.9.5-el7
RUN \
    yum install -y numpy && \
    yum clean all && \
    rm -rf /var/cache/yum/*
```

and build with:
```console
$ docker build -t custom-htcondor-worker .
```

Afterwards, use `custom-htcondor-worker` instead of `htcondor/execute:el7`
in your `docker run` command.


Building
--------

The images are built using a Makefile.  You need to be `root` or be in the
`docker` Unix group in order to use the Makefile.

The Makefile needs to know what version of HTCondor the images are built for,
in order to tag them properly.  The images for the various roles (`execute`,
`cm`, `mini`, `submit`) are built on top of the `htcondor/base` image,
which gets built first.  `htcondor/base` contains the HTCondor software,
some common configuration and scripts.

To build all the images, run:
```console
$ make build VERSION=8.9.5
```

The images will be named like `htcondor/execute:8.9.5-el7`.  To create
versionless aliases for the built products (e.g. `htcondor/execute:el7`),
pass `UNVERSIONED=yes` to `make`.


To push the images to Docker Hub, run:
```console
$ make push VERSION=8.9.5
```

To push to an alternate registry, set the `REGISTRY` parameter to the
registry you want to push to:
```console
$ make push VERSION=8.9.5 REGISTRY=registry.example.net
```

To push the versionless tags for the images, pass `UNVERSIONED=yes` to
`make`.

See `make help` for additional targets and options.



Known Issues
------------

- Logs are in multiple files in `/var/log/condor`.  This means you need
  to enter the container to see the logs; also, they are destroyed when the
  container is removed.

- Changes to the volumes mounted at `/root/config` and `/root/secrets`
  do not get noticed automatically.  After updating config, run
  `/update-config` in the container.  After updating secrets, run
  `/update-secrets` in the container.

- cgroups support is not yet implemented.

- Docker universe support is not yet implemented.

- Singularity support is not yet implemented.
