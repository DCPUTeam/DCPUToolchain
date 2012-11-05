.. _toolchain-deployable:

Toolchain Deployment
=============================================

The toolchain is now shipping in two alternative versions: deployable and non-deployable builds.
There are some important differences between these two build types.

Deployable Builds
---------------------

Deployable builds are builds that are created to be installed on a machine with a set installation
directory that doesn't change.  These builds are packaged up into the Windows installers, Mac packages
and Debian and RPM formats for Linux.

This builds have better detection for where modules, kernels and standard libraries exist since hint
paths can be added at build time (since it is built for a pre-determined path).  Windows has an additional
hint source involving the registry, which the installer sets when it is run.

However, these builds can't be moved around once installed.  The building of hint paths into them
mean that once they are built, they can't really be moved around without reinstallation.

Non-deployable Builds
-------------------------

Non-deployable builds are the builds of the toolchain that have existing since the build server was
put in place.  These builds rely on environment variables (such as `TOOLCHAIN_MODULES`) and their
local execution path to determine where supporting files reside.  Often however, they aren't accurate
and thus a lot of the path detection that occurs in a deployable build does not work for non-deployable
builds.

Non-deployable builds have the advantage that they can be moved around and the most that will have to
be done is change environment variables to suit.

However, when pathing is incorrect or not provided, features such as the `-k` option on the linker will
not correctly be able to find supporting files for kernels, in which case manual policy paths may have
to be provided.

These builds are best suited to developers who work on a constantly changed codebase.