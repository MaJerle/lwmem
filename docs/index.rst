LwMEM documentation!
====================

LwMEM is lightweight dynamic memory manager optimized for embedded systems.

.. class::center

:ref:`download_library` · `Github <https://github.com/MaJerle/lwmem>`_

Features
^^^^^^^^

* Written in ANSI C99, compatible with ``size_t`` for size data types
* Implements standard C library functions for memory allocation, malloc, calloc, realloc and free
* Uses *first-fit* algorithm to search free block
* Supports different memory regions to allow use of fragmented memories
* Suitable for embedded applications with fragmented memories
* Suitable for automotive applications
* Supports advanced free/realloc algorithms to optimize memory usage
* Operating system ready, thread-safe API
* User friendly MIT license

Requirements
^^^^^^^^^^^^

* C compiler
* Less than ``2kB`` of memory

Contribute
^^^^^^^^^^

We always welcome new contributors. To be as efficient as possible, we recommend:

#. Fork Github repository
#. Respect `C style & coding rules <https://github.com/MaJerle/c-code-style>`_ used by the library
#. Make a pull request to ``develop`` branch with new features or bug fixes

Alternatively you may:

#. Report a bug
#. Ask for a feature request

License
^^^^^^^

.. literalinclude:: license.txt

Table of contents
^^^^^^^^^^^^^^^^^

.. toctree::
    :maxdepth: 2

    get-started/index
    user-manual/index
    api-reference/index
    examples/index
