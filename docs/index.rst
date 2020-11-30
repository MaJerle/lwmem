LwMEM |version| documentation
=============================

Welcome to the documentation for version |version|.

LwMEM is lightweight dynamic memory manager optimized for embedded systems.

.. rst-class:: center
.. rst-class:: index_links

	:ref:`download_library` :ref:`getting_started` `Open Github <https://github.com/MaJerle/lwmem>`_ `Donate <https://paypal.me/tilz0R>`_

Features
^^^^^^^^

* Written in ANSI C99, compatible with ``size_t`` for size data types
* Implements standard C library functions for memory allocation, malloc, calloc, realloc and free
* Uses *first-fit* algorithm to search for free block
* Supports multiple allocation instances to split between memories and/or CPU cores
* Supports different memory regions to allow use of fragmented memories
* Highly configurable for memory allocation and reallocation
* Supports embedded applications with fragmented memories
* Supports automotive applications
* Supports advanced free/realloc algorithms to optimize memory usage
* Operating system ready, thread-safe API
* User friendly MIT license

Requirements
^^^^^^^^^^^^

* C compiler
* Less than ``2kB`` of non-volatile memory

Contribute
^^^^^^^^^^

Fresh contributions are always welcome. Simple instructions to proceed:

#. Fork Github repository
#. Respect `C style & coding rules <https://github.com/MaJerle/c-code-style>`_ used by the library
#. Create a pull request to ``develop`` branch with new features or bug fixes

Alternatively you may:

#. Report a bug
#. Ask for a feature request

License
^^^^^^^

.. literalinclude:: ../LICENSE

Table of contents
^^^^^^^^^^^^^^^^^

.. toctree::
    :maxdepth: 2

    self
    get-started/index
    user-manual/index
    api-reference/index
    examples/index
