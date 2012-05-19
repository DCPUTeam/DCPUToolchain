Downloading Modules
=============================================

You can automatically install and manage modules using the `dtmm`
tool.  The `dtmm` tool has the following syntax:

.. code-block:: bash

    dtmm [-h] [search|se|install|in|uninstall|rm] [command-options]

Searching (se)
---------------

You can search and list the available modules using the `search`
command.  As an example:

.. code-block:: bash

    dtmm search -t hardware rng

Options
~~~~~~~~~~~~~~~

.. cmdoption:: -t hardware
                --type=hardware
    
    Limit the search to only contain modules of the specified type.
    The following types are accepted: `hardware`, `preprocessor`, `debugger`.

Installing (in)
---------------

Modules are automatically downloaded from the module repository.  To
install a module, you can use:

.. code-block:: bash

    dtmm install rng

Modules will be installed into the `modules/` folder, or the path
specified by :envvar:`TOOLCHAIN_MODULES` if it is set.

Uninstalling (rm)
------------------

You can also uninstall modules using the `dtmm` tool with the
following command:

.. code-block:: bash

    dtmm uninstall rng

Modules will be removed from the `modules/` folder, or the path
specified by :envvar:`TOOLCHAIN_MODULES` if it is set.
