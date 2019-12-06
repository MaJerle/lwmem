Get started
===========

.. _download_library:

Download library
^^^^^^^^^^^^^^^^

Library is primarly hosted on `Github <https://github.com/MaJerle/lwmem>`_.

* Download latest release from `releases area <https://github.com/MaJerle/lwmem/releases>`_ on Github
* Clone `develop` branch for latest development

Download from releases
**********************

All releases are available on Github releases `releases area <https://github.com/MaJerle/lwmem/releases>`_.

Clone from Github
*****************

First-time clone
""""""""""""""""

* Download and install ``git`` if not already
* Open console and navigate to path in the system to clone repository to. Use command ``cd your_path``
* Run ``git clone --recurse-submodules https://github.com/MaJerle/lwmem`` command to clone repository including submodules or
* Run ``git clone --recurse-submodules --branch develop https://github.com/MaJerle/lwmem`` to clone `development` branch
* Navigate to ``examples`` directory and run favourite example

Update cloned to latest version
"""""""""""""""""""""""""""""""

* Open console and navigate to path in the system where your resources repository is. Use command ``cd your_path``
* Run ``git pull origin master --recurse-submodules`` command to pull latest changes and to fetch latest changes from submodules
* Run ``git submodule foreach git pull origin master`` to update & merge all submodules

Add library to project
^^^^^^^^^^^^^^^^^^^^^^

At this point it is assumed that you have successfully download library, either cloned it or from releases page.

* Copy ``lwmem`` folder to your project
* Add ``lwmem/src/include`` folder to `include path` of your toolchain
* Add source files from ``lwmem/src/`` folder to toolchain build
* Build the project

Minimal example code
^^^^^^^^^^^^^^^^^^^^

Run below example to test and verify library

.. code-block:: c

    #include "lwmem/lwmem.h"

    void* ptr;

    /* Create regions, address and length of regions */
    static
    lwmem_region_t regions[] = {
        /* Set start address and size of each region */
        { (void *)0x10000000, 0x00001000 },
        { (void *)0xA0000000, 0x00008000 },
        { (void *)0xC0000000, 0x00008000 },
    };

    /* Later in the initialization process */
    /* Assign regions for manager */
    lwmem_assignmem(regions, sizeof(regions) / sizeof(regions[0]));

    ptr = lwmem_malloc(8);                          /* Allocate 8 bytes of memory */
    if (ptr != NULL) {
        /* Allocation successful */
    }

    /* Later... */                                  /* Free allocated memory */
    lwmem_free(ptr);
    ptr = NULL;
    /* .. or */
    lwmem_free_s(&ptr);