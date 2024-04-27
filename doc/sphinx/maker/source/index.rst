.. Tsunami Project documentation master file, created by
   sphinx-quickstart on Sat Oct 28 03:50:41 2023.
   You can adapt this file completely to your liking, but it should at least
   contain the root `toctree` directive.

User Documentation for the Riemann solver!
===========================================

.. toctree::
   :maxdepth: 2
   :caption: Contents: second assigmen 

   

   



1. Introduction
-------------------
Welcome to the user documentation for our Riemann solver in C++. This solver is designed
to help you solve the Riemann problem for various hyperbolic partial differential equations.
This documentation will guide you through installation and usage.
The solver produces CSV files as output, which can be visualized through `ParaView`_.

.. _ParaView: https://www.paraview.org/download/


2. Getting Started
-------------------
Before you begin, make sure you have the following prerequisites:

- **A C++ compiler (e.g., g++).**

- **Scons (for building).**

- **Git (for cloning the repository).**






3. Installation
----------------
**1. Clone the repository from our** `GitHub Page`_:

.. _GitHub Page: https://github.com/Sinio-Manoka/tsunami_lab


.. code-block:: 

   # git clone https://github.com/Sinio-Manoka/tsunami_lab.git 


**2. Install** `Scons`_ **on Linux :**

.. _Scons: https://scons.org/doc/2.2.0/HTML/scons-user/x121.html#:~:text=If%20your%20Linux%20distribution%20does,%2Fusr%2Flib%2Fscons.&text=Or%2C%20you%20can%20use%20a%20graphical%20RPM%20package%20manager.


.. code-block:: 

   # apt-get install scons 



**3. Include Catch2:**

   3.1. Navigate to the project directory:


      .. code-block:: 

         # cd tsunami_lab/



   3.2. initialize the submodules:


      .. code-block:: 

       # git submodule init 



   3.3. update the submodules within a Git repository:

      .. code-block:: 

         # git submodule update 






**4. How build the project:**


   4.1. Navigate to the project directory:


      .. code-block:: 

         # cd tsunami_lab/

   
  
   4.2. Build the project with Scons:


      .. code-block:: 

         # scons

   4.3. You will find the compiled binary in the build directory.




5. Usage
--------
**To use the Riemann solver, follow these steps:**

**1. Navigate to the build directory:**

.. code-block:: 

   #  cd build


**2. To run the Riemann solver, use the following command:**

.. code-block:: 

   #  ./tsunami_lab <specific_number>

Replace <specific_number> with the specific input parameter or numerical value required for your Riemann solver.

.. code-block:: 

   Example: # ./tsunami_lab 300.



**For every 100 input, there will be 10 corresponding CSV files generated in the build directory.!!**

6. Documentation
----------------
**The Project Documentation can be found** `Here`_ .

.. _Here: https://tsunami-99843.web.app/

7. Conclusion
-------------
Congratulations! You've successfully learned how to use our Riemann solver in C++.
We hope this documentation helps you integrate the solver into your projects effectively. 
If you have any questions or feedback, please don't hesitate to contact us.


second assigmen 
================

















