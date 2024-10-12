.. _light_version:

LwMEM light implementation
==========================

When system is super memory constrained or when system only requires memory allocation at initialization stage,
it is possible to put the library into *light* mode by controlling the :c:macro:`LWMEM_CFG_FULL` user configuration option

When *full* mode is disabled, user must be aware of some contraints:

* It is only possible to allocate memory (no free, no realloc)
* It is only possible to use one (``1``) memory region. When assigning the memory with more than one region, function will return an error.

.. tip::
    Light mode is useful for opaque types that are returned to user and must be allocated on the heap.
    These are typically allocated at initialization stage and never freed during program execution.

.. toctree::
    :maxdepth: 2
