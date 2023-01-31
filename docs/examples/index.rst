.. _examples:

Examples and demos
==================

Various examples are provided for fast library evaluation on embedded systems. These are prepared and maintained for ``2`` platforms, but could be easily extended to more platforms:

* WIN32 examples, prepared as `CMake` projects, ready for `MSYS2 GCC compiler`
* ARM Cortex-M examples for STM32, prepared as `STM32CubeIDE <https://www.st.com/en/development-tools/stm32cubeide.html>`_ GCC projects. These are also supported in *Visual Studio Code* through *CMake* and *ninja* build system. `Dedicated tutorial <https://github.com/MaJerle/stm32-cube-cmake-vscode>`_ is available to get started in *VSCode*.

.. warning::
	Library is platform independent and can be used on any platform.

Example architectures
^^^^^^^^^^^^^^^^^^^^^

There are many platforms available today on a market, however supporting them all would be tough task for single person.
Therefore it has been decided to support (for purpose of examples) ``2`` platforms only, `WIN32` and `STM32`.

WIN32
*****

Examples for *WIN32* are CMake-ready and *VSCode*-ready.
It utilizes CMake-presets feature to let you select the example and compile it directly.

* Make sure you have installed GCC compiler and is in env path (you can get it through MSYS2 packet manager)
* Install ninja and cmake and make them available in the path (you can get all through MSYS2 packet manager)
* Go to *examples win32* folder, open vscode there or run cmd: ``cmake --preset <project name>`` to configure cmake and later ``cmake --build --preset <project name>`` to compile the project

STM32
*****

Embedded market is supported by many vendors and STMicroelectronics is, with their `STM32 <https://www.st.com/en/microcontrollers-microprocessors/stm32-32-bit-arm-cortex-mcus.html>`_ series of microcontrollers, one of the most important players.
There are numerous amount of examples and topics related to this architecture.

Examples for *STM32* are natively supported with `STM32CubeIDE <https://www.st.com/en/development-tools/stm32cubeide.html>`_, an official development IDE from STMicroelectronics.

You can run examples on one of official development boards, available in repository examples.

Examples list
^^^^^^^^^^^^^

Here is a list of all examples coming with this library.

.. tip::
	Examples are located in ``/examples/`` folder in downloaded package.
	Check :ref:`download_library` section to get your package.

LwMEM bare-metal
****************

Simple example, not using operating system, showing basic configuration of the library.
It can be also called `bare-metal` implementation for simple applications

LwMEM OS
********

LwMEM library integrated as application memory manager with operating system.
It configurex mutual exclusion object ``mutex`` to allow multiple application threads accessing to LwMEM core functions

LwMEM multi regions
*******************

Multi regions example shows how to configure multiple linear regions to be applied to single LwMEM instance.
It uses simple varible array to demonstrate memory sections in embedded systems.

LwMEM multi instances & regions
*******************************

This example shows how can application add custom (or more of them) instances for LwMEM memory management.
Each LwMEM instance has its own set of regions to work with.

LwMEM instances are between each-other completely isolated.

.. toctree::
	:maxdepth: 2
