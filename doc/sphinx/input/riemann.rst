.. Tsunami Project documentation master file, created by
   sphinx-quickstart on Sat Oct 28 03:50:41 2023.
   You can adapt this file completely to your liking, but it should at least
   contain the root `toctree` directive.
.. _ch:riemann_solver:
   

Riemann
===========================================

Welcome to the documentation for our the integration of a Riemann solver in a Finite Volume scheme in C++. This solver is designed to help 
you only to slove for do 1D shallow water equations.
This documentation will guide you through the functions we implemented to compile the Riemann
solver. numerical solver which uses the Riemann solver produces CSV files as output, which can be visualized through `ParaView <https://www.paraview.org/download>`_ .



Important Links
------------------

:ref:`Installation`

`GitHub Page <https://github.com/Sinio-Manoka/tsunami_lab>`_

`Doxygen Documentation <https://doxygen-tsunami.web.app>`_


Riemann Solver
---------------------

Eigenvalues
..............

To solve the problem, we must calculate the Eigenvalues. Before computing the eigenvalues, 
we need to determine :math:`u^{\text{Roe}}(q_l, q_r)` and  :math:`h^{\text{Roe}}(q_l, q_r)`
using the following formula:

.. math::

   \begin{aligned}
         h^{\text{Roe}}(q_l, q_r) &= \frac{1}{2} (h_l + h_r), \\
         u^{\text{Roe}}(q_l, q_r) &=  \frac{u_l \sqrt{h_l} + u_r \sqrt{h_r}}{\sqrt{h_l}+\sqrt{h_r}}.
   \end{aligned}


To calculate the eigenvalue, we need to utilize the formula: 

.. math::
   
   \begin{aligned}
         \lambda^{\text{Roe}}_{1}(q_l, q_r) &= u^{\text{Roe}}(q_l, q_r) - \sqrt{gh^{\text{Roe}}(q_l, q_r)}, \\
         \lambda^{\text{Roe}}_{2}(q_l, q_r) &= u^{\text{Roe}}(q_l, q_r) + \sqrt{gh^{\text{Roe}}(q_l, q_r)},
   \end{aligned}

The Equivalent C++ Code for that is :

.. code-block:: cpp
   
   void tsunami_lab::solvers::fwave::eigenwerte( t_real   i_hL,
                                t_real   i_hR,
                                t_real   i_uL,
                                t_real   i_uR,
                                t_real & o_waveSpeedL,
                                t_real & o_waveSpeedR ){

   t_real l_hSqrtL = std::sqrt( i_hL );
   t_real l_hSqrtR = std::sqrt( i_hR );

   //determine the height : h_Roe 
   t_real l_hRoe = 0.5f * ( i_hL + i_hR );

   //determine the particle velocity : u_Roe
   t_real l_uRoe = l_hSqrtL * i_uL + l_hSqrtR * i_uR;
   l_uRoe /= l_hSqrtL + l_hSqrtR;

   //compute the wave speeds
   t_real l_ghSqrtRoe = m_gSqrt * std::sqrt( l_hRoe );

   o_waveSpeedL = l_uRoe - l_ghSqrtRoe;
   o_waveSpeedR = l_uRoe + l_ghSqrtRoe;
   }

.. Important::
   The function takes inputs :math:`h_l` , :math:`h_r` , :math:`u_l` and :math:`u_r` and produces the eigenvalue :math:`\lambda^{\text{Roe}}_{1/2}` using the formula provided above. 
   :math:`u_i` can be easily calculated using  :math:`u_i = hu_i / h_i`. 


Flux function
..............

Now we need to calculate the difference in the flux function :math:`\Delta f` which can be easily calculated
using this Formula: :math:`\Delta f := f(q_r) - f(q_l)`. Before anything else, we must calculate :math:`f(q_r) and f(q_l)`
using the following formula:  :math:`f := [hu, hu^2 + \frac{1}{2}gh^2]^T` with respect to :math:`(q_r)` and :math:`(q_l)`

The Equivalent C++ Code for that is:

.. code-block:: cpp

   void tsunami_lab::solvers::fwave::flux( t_real i_hL,
                                        t_real i_hR,
                                        t_real i_huL,
                                        t_real i_huR,
                                        t_real o_fdelta[2]){
    /*
     ___(1.3.3)___
     compute f(q_left):
     compute hu² = (hu)² / h
    */
    t_real pow2HuL= pow(i_huL, 2)/i_hL;
    t_real gPowHL = g * pow(i_hL, 2);
    t_real totalL = pow2HuL + 0.5 * gPowHL;
    t_real fql[2] = {i_huL, totalL};

    //compute f(q_right)
    t_real pow2HuR= pow(i_huR, 2)/i_hR;
    t_real gPowHR = g * pow(i_hR, 2);
    t_real totalR = pow2HuR + 0.5 * gPowHR;
    t_real fqr[2] = {i_huR, totalR};

    //compute Δf
    if (o_fdelta == nullptr) {
        o_fdelta = new t_real[2];
    }
    o_fdelta[0] = fqr[0] - fql[0];
    o_fdelta[1] = fqr[1] - fql[1];
   } 


.. Important::
   We are provided with  :math:`q_l = [h_l, (hu)_l]^T` and :math:`q_r = [h_r, (hu)_r]^T` We simply insert these values into the equation 
   :math:`f := [hu, hu^2 + \frac{1}{2}gh^2]^T` which results in the output :math:`\Delta f`  as a 2D vector.


Eigencoefficients 
..................

The eigencoefficients :math:`\alpha_p` are obtained by multiplying the `inverse
<https://mathworld.wolfram.com/MatrixInverse.html>`_ of the matrix of right
eigenvectors :math:`R=[r_1^\text{Roe}, r_2^\text{Roe}]` with the output vector from the 
flux function

.. math::

   \begin{bmatrix}
         \alpha_1 \\
         \alpha_2
       \end{bmatrix} =
       \begin{bmatrix}
         1 & 1 \\
         \lambda^{\text{Roe}}_1 & \lambda^{\text{Roe}}_2
       \end{bmatrix}^{-1} \Delta f.


The Equivalent C++ Code for that is :

.. code-block:: cpp

   void tsunami_lab::solvers::fwave::eigencoefficientAlpha(t_real i_inverse[4],
                                                        t_real i_delta_f[2],
                                                        t_real o_eigencoefficients[2]){
    if (o_eigencoefficients == nullptr) {
        o_eigencoefficients = new t_real[2];
    }
    //m x n ° n x p = 
    o_eigencoefficients[0] = (i_inverse[0] * i_delta_f[0]) + (i_inverse[1] * i_delta_f[1]);
    o_eigencoefficients[1] = (i_inverse[2] * i_delta_f[0]) + (i_inverse[3] * i_delta_f[1]);
   }


For the Calculation of the Inverse:

.. code-block:: cpp

   void tsunami_lab::solvers::fwave::inverseMatrix(t_real i_eigen1,
                                                t_real i_eigen2,
                                                t_real o_inverse[4]){
    //Compute the determinant of a 2x2 matrix 
    t_real det = i_eigen2 - i_eigen1;

    //Compute the inverses of specific 2x2 matrices. (first row contains only ones)
    o_inverse[0] = i_eigen2 / det;
    o_inverse[1] = -1 / det;
    o_inverse[2] = -i_eigen1 / det;
    o_inverse[3] = 1 / det;
   }

decomposition
.................
After computing the eigencoefficient, :math:`\alpha_p` ,
we can proceed to break down the difference in the flux function, :math:`f`. 
w between the values at qᵣ and qₗ, into the eigenvectors, which gives us the waves :math:`Z_{1/2}`

.. math:: \Delta f = \sum_{p=1}^2 \alpha_p r_p \equiv  \sum_{p=1}^2 Z_p, \qquad \alpha_p \in \mathbb{R}.


After computing the :math:`Z_{1/2}`, we need to divide them into  :math:`A^- \Delta Q` (the cell influenced by the left-going waves) and :math:`A^+ \Delta Q` (the cell influenced by the right-going waves)
by using this formula: 

.. math::

   \begin{split}
         A^- \Delta Q := \sum_{p:\{ \lambda_p^\text{Roe} < 0 \}} Z_p \\
         A^+ \Delta Q := \sum_{p:\{ \lambda_p^\text{Roe} > 0 \}} Z_p
   \end{split}

The Equivalent C++ Code for that is :

.. code-block:: cpp

   void tsunami_lab::solvers::fwave::decompose(t_real i_alphas[2],
                                            t_real i_eigens[2],
                                            t_real o_minus_A_deltaQ[2], 
                                            t_real o_plus_A_deltaQ[2]){


    //Negative speed of wave propagation                                    
    if( i_eigens[0] < 0){
        o_minus_A_deltaQ[0] =  i_alphas[0];
        o_minus_A_deltaQ[1] = (i_alphas[0] * i_eigens[0]);
        o_plus_A_deltaQ[0] = 0;
        o_plus_A_deltaQ[1] = 0;
    }else{
        o_plus_A_deltaQ[0] =   i_alphas[0];
        o_plus_A_deltaQ[1] =  (i_alphas[0] * i_eigens[0]);
        o_minus_A_deltaQ[0] =  0;
        o_minus_A_deltaQ[1] = 0;
    }
    //Positive speed of wave propagation
    if(i_eigens[1] < 0){
        o_minus_A_deltaQ[0] = o_minus_A_deltaQ[0] + (i_alphas[1]);
        o_minus_A_deltaQ[1] = o_minus_A_deltaQ[1] + (i_alphas[1] * i_eigens[1]);
        
    }else{
        o_plus_A_deltaQ[0] = o_plus_A_deltaQ[0] + (i_alphas[1]);
        o_plus_A_deltaQ[1] = o_plus_A_deltaQ[1] + (i_alphas[1] * i_eigens[1]);
    }

   }


.. Important::
   the input of the function are the eigencoefficient :math:`\alpha_p` and the Roe eigenvectors :math:`r_{1/2}^{\text{Roe}}`

   .. math::

      \begin{aligned}
         r_1^{\text{Roe}} &=
           \begin{bmatrix}
             1 \\ \lambda^{\text{Roe}}_1
           \end{bmatrix}, \\
         r_2^{\text{Roe}} &=
           \begin{bmatrix}
             1 \\ \lambda^{\text{Roe}}_2
           \end{bmatrix}.
       \end{aligned}

   and the output of the function should be :math:`A^- \Delta Q` (the cell influenced by the left-going waves) and :math:`A^+ \Delta Q` (the cell influenced by the right-going waves) as a 2d vector.

netUpdates
..........

This function should invoke every other function in the file to compute :math:`A^- \Delta Q` and :math:`A^+ \Delta Q`.



.. code-block:: cpp

   void tsunami_lab::solvers::fwave::netUpdates(t_real   i_hL,
                                             t_real   i_hR,
                                             t_real   i_huL,
                                             t_real   i_huR,
                                             t_real   o_minus_A_deltaQ[2],
                                             t_real   o_plus_A_deltaQ[2]){
      //calculate ul and ur
    t_real l_uL = i_huL / i_hL;
    t_real l_uR = i_huR / i_hR;
      
    t_real l_sL = 0;
    t_real l_sR = 0;

      //calculate the eigenvalue
    eigenwerte(i_hL,i_hR,l_uL,l_uR,l_sL,l_sR);

    t_real l_inverse[4];

      //calculate the inverse matrix
    inverseMatrix(l_sL, l_sR, l_inverse);

      //calculate difference in the flux function 
    t_real l_fdelta[2];
    flux(i_hL,i_hR,i_huL,i_huR,l_fdelta);
    
      //calculate Alpha
    t_real l_eigencoefficients[2];
    eigencoefficientAlpha(l_inverse,l_fdelta,l_eigencoefficients);

      //calculate Output
    t_real l_eigens[2] = {l_sL,l_sR};
    decompose(l_eigencoefficients,l_eigens,o_minus_A_deltaQ,o_plus_A_deltaQ);

   }

.. Important::
   We are given :math:`q_l = [h_l, (hu)_l]^T` and :math:`q_r = [h_r, (hu)_r]^T` so we simply plug them in the function to compute :math:`A^- \Delta Q` and :math:`A^+ \Delta Q` as 2D vector.
   for more information check the `Documentation <https://doxygen-tsunami.web.app/>`_.




ParaView
---------------------
Now that we have successfully implemented the Riemann Solver,
we can utilize it to create our own CSV files and employ them for wave visualization.
As demonstrated in the :ref:`Installation` section, we have learned how to generate these CSV files.
Simply import them into `ParaView <https://www.paraview.org/download>`_, and from there,
you can proceed to export them as either images or animations.

Here is an example where we used 200 as the input.

.. video:: _static/animation.mp4
   :width: 700
   :autoplay:





.. Important::
   Make sure to hide the X and Y axes since they remain constant and won't change, leaving us with only the "Height" and "Momentum X" values.



Conclusion
...........
Congratulations! You've successfully learned how to use our numerical solver which uses the Riemann solver.in C++.
We hope this documentation helps you integrate the solver into your projects effectively. 
If you have any questions or feedback, please don't hesitate to contact us.



Personal Contribution
---------------------

- Ward Tammaa, Danial Schicker Doxygen Documentation
- Mohamad Khaled Minawe, Ward Tammaa Sphnix Documentation
- Danial Schicker Mohamad Khaled Minawe , Ward Tammaa functions implementation
- Mohamad Khaled Minawe, Danial Schicker, Ward Tammaa Unit Testing
- Mohamad Khaled Minawe, Danial Schicker Geogebra Datei(Calculation for the Unit Tests)
- Ward Tammaa Hosting the code


    



