
Large Data Input and Output
===========================

netCDF Output
-------------

Install netCDF
..............

1. Please install netCDF from `here`_:

.. _here: https://www.unidata.ucar.edu/software/netcdf/

or

If you're working with Debian-based distros, execute the provided commands to install netCDF.

.. code-block::

  sudo add-apt-repository universe

  sudo apt-get update

  sudo apt-get install zlib1g-dev libhdf5-dev libnetcdf-dev


now lets change our SConstruct:

.. code-block:: python

    netcdf_lib = env.Append(LIBS=['netcdf'])

    zlib_lib = env.Append(LIBS=['z'])

    hdf5_lib = env.Append(LIBS=['hdf5_serial'])

    if 'LD_LIBRARY_PATH' not in env['ENV']:
        env['ENV']['LD_LIBRARY_PATH'] = ''

    env['ENV']['LD_LIBRARY_PATH'] = '/usr/lib/x86_64-linux-gnu' + env['ENV']['LD_LIBRARY_PATH']


Implementation of a class NetCdf
.................................



Now, let's implement a NetCdf class by creating the following files:  : ``NetCdf.cpp`` , ``NetCdf.h`` and ``NetCdf.test.cpp``

1. Let's proceed with the implementation of the header file named``NetCdf.h`` 

.. code-block:: cpp

    #ifndef TSUNAMI_LAB_IO_NETCDF
    #define TSUNAMI_LAB_IO_NETCDF

    #include "../../constants.h"
    #include <cstring>
    #include <iostream>
    #include <fstream>
    #include <sstream>
    #include <vector>
    #include <netcdf.h>

    namespace tsunami_lab {
    namespace io {
        class NetCdf;
    }
    }

    class tsunami_lab::io::NetCdf {
    private:
        int m_varIdX, m_varIdY,  m_varIdHeight, m_varIdTime, m_varIdBathymetry, m_varIdImpulseX, m_varIdImpulseY;
        t_idx m_time_step = 0;

        static void checkNcErr( int i_err);


    public:
   
        void updateFile(t_idx                i_nx,
                        t_idx                i_ny,
                        t_idx                i_stride,
                        t_real               i_time,
                        t_real       const * i_h,
                        t_real       const * i_hu,
                        t_real       const * i_hv,
                        const char*          filename);
        
     
        NetCdf(t_real l_nx,t_real l_ny, const char* filename);

   
        void fillConstants(t_idx                  i_nx,  
                        t_idx                   i_ny,
                        t_real                  i_dxy,
                        t_real                  i_domainstart_x,
                        t_real                  i_domainstart_y,
                        t_real                  i_stride,
                        t_real          const * i_b,
                        const char*           filename);

   
        static int read( const char* filename,
                        const char* varname,
                        std::vector<t_real> &data);
                                    
    };

    #endif


In the document, you'll find two functions: ``fillConstants``, ``updateFile``, and a constructor named ``netCDF``.

    1. ``netCDF``:

        This constructer is responsible for creating the NetCDF output file.

    2. ``fillConstants``:

        The purpose of this function is to write the constants into the NetCDF output file.

    3. ``updateFile``:

        This function is designed to update the output file after each time step.




2.  After incorporating the header file, we can proceed to develop the write function in the ``NetCdf.cpp`` file.

.. code-block:: cpp 


    void tsunami_lab::io::NetCdf::fillConstants(t_idx                   i_nx,
                                                t_idx                   i_ny,
                                                t_real                  i_dxy,
                                                t_real                  i_domainstart_x,
                                                t_real                  i_domainstart_y,
                                                t_real                  i_stride,
                                                t_real          const * i_b,
                                                const char*           filename){
        int l_ncId,l_err;
        l_err = nc_open(filename,NC_WRITE, &l_ncId);
        t_real *l_data0 = new t_real[i_ny];

        for( t_idx l_iy = 0; l_iy < i_ny; l_iy++ )
        {
            l_data0[l_iy] = ((l_iy + 0.5) * i_dxy )+ i_domainstart_y;
        }
        // put y coordinates
        l_err = nc_put_var_float(l_ncId, m_varIdY, l_data0);
        checkNcErr(l_err);
        delete[] l_data0;

        t_real *l_data1 = new t_real[i_nx];
        for(t_idx l_ix = 0; l_ix < i_nx; l_ix++) 
        {
            l_data1[l_ix] = ((l_ix + 0.5) * i_dxy )+ i_domainstart_x;
        }
        // put x coordinates
        l_err = nc_put_var_float(l_ncId, m_varIdX, l_data1);
        checkNcErr(l_err);
        delete[] l_data1;

        t_real *l_data2 = new t_real[i_nx*i_ny];
        for( t_idx l_iy = 0; l_iy < i_ny; l_iy++)
        {
            for( t_idx l_ix = 0; l_ix < i_nx; l_ix++)
            {
                t_idx l_id = (l_iy+1) * i_stride + (l_ix+1);

                l_data2[(l_iy) * i_nx + (l_ix) ] = i_b[l_id];
            }
        }
        
        // put bathymetry values
        l_err = nc_put_var_float(l_ncId, m_varIdBathymetry, l_data2);
        checkNcErr(l_err);
        delete[] l_data2;


        //close file for now
        l_err = nc_close(l_ncId);
        checkNcErr(l_err);
    

    }


    void tsunami_lab::io::NetCdf::updateFile(t_idx                i_nx,
                                            t_idx                i_ny,
                                            t_idx                i_stride,
                                            t_real               i_time,
                                            t_real       const * i_h,
                                            t_real       const * i_hu,
                                            t_real       const * i_hv,
                                            const char*           filename){
        
        int l_ncId, l_err;  
        //open file
        l_err = nc_open(filename,NC_WRITE,&l_ncId); 
        checkNcErr(l_err);

        //create memory
        std::vector<t_real> l_temp_data_height(i_ny * i_nx);
        std::vector<t_real> l_temp_data_momentum_x(i_ny * i_nx);
        std::vector<t_real> l_temp_data_momentum_y(i_ny * i_nx);

        //
        for( t_idx l_iy = 1; l_iy < i_ny+1; l_iy++ ) {
        for( t_idx l_ix = 1; l_ix < i_nx+1; l_ix++ ){

            t_idx l_id = l_iy * i_stride + l_ix;

            l_temp_data_height[(l_iy-1) * i_nx + (l_ix-1)] = i_h[l_id];
            l_temp_data_momentum_x[(l_iy-1) * i_nx + (l_ix-1)] = i_hu[l_id];
            l_temp_data_momentum_y[(l_iy-1) * i_nx + (l_ix-1)] = i_hv[l_id];
        }
        }
        
        std::vector<size_t> l_startp     = {m_time_step,0,0};
        std::vector<size_t> l_endp       = {1,i_ny,i_nx};
        std::vector<ptrdiff_t> l_stridep = {1,1,1}; //no elements get skipped
        
        l_err = nc_put_vars_float(l_ncId, m_varIdHeight, l_startp.data(), l_endp.data(), l_stridep.data(), l_temp_data_height.data());
        checkNcErr(l_err);
            
        l_err = nc_put_vars_float(l_ncId, m_varIdImpulseX, l_startp.data(), l_endp.data(), l_stridep.data(), l_temp_data_momentum_x.data());
        checkNcErr(l_err);
            
        l_err = nc_put_vars_float(l_ncId, m_varIdImpulseY, l_startp.data(), l_endp.data(), l_stridep.data(), l_temp_data_momentum_y.data());
        checkNcErr(l_err);
        //time step is how many timesteps there are and i_time what simtime it is
        l_err = nc_put_var1_float(l_ncId, m_varIdTime, &m_time_step, &i_time);
        checkNcErr(l_err);

        l_err = nc_close(l_ncId);
        checkNcErr(l_err);
        m_time_step ++;
    }

        
    tsunami_lab::io::NetCdf::NetCdf(t_real l_nx,t_real l_ny,const char*  filename) {
        
        int l_ncId,l_err;
        // Dimensions x, y, time 
        int l_dimXId, l_dimYId, l_dimTimeId;
        int l_dim2Ids[2];
        int l_dim3Ids[3];

        

        std::cout << "generating netcdf-file ' " <<filename<< " '"<< std::endl;

        l_err = nc_create(filename,NC_CLOBBER, &l_ncId);      
        checkNcErr(l_err);

        //dimensions
        l_err = nc_def_dim(l_ncId, "x", l_nx, &l_dimXId);
        checkNcErr(l_err);
        
        l_err = nc_def_dim(l_ncId, "y", l_ny, &l_dimYId);
        checkNcErr(l_err);

        l_err = nc_def_dim(l_ncId, "time", NC_UNLIMITED, &l_dimTimeId);
        checkNcErr(l_err);

        //variable x
        l_err = nc_def_var(l_ncId, "x", NC_FLOAT, 1, &l_dimXId, &m_varIdX);
        checkNcErr(l_err);
        const char* units_attribute_x = "meter";
        nc_put_att_text(l_ncId, m_varIdX, "units", strlen(units_attribute_x), units_attribute_x);

        //variable y
        l_err = nc_def_var(l_ncId, "y", NC_FLOAT, 1, &l_dimYId, &m_varIdY);
        checkNcErr(l_err);
        const char* units_attribute_y = "meter";
        nc_put_att_text(l_ncId, m_varIdY, "units", strlen(units_attribute_y), units_attribute_y);

        //variable time
        l_err = nc_def_var(l_ncId, "time", NC_FLOAT, 1, &l_dimTimeId, &m_varIdTime);
        checkNcErr(l_err);
        const char* units_attribute_time = "seconds";
        nc_put_att_text(l_ncId, m_varIdTime, "units", strlen(units_attribute_time), units_attribute_time);
        
        
        //variable bathymetry
        l_dim2Ids[0] =l_dimYId;
        l_dim2Ids[1] =l_dimXId;

        
        l_err = nc_def_var(l_ncId, "b", NC_FLOAT, 2, l_dim2Ids, &m_varIdBathymetry);
        checkNcErr(l_err);
        const char* units_attribute_Bathymetry = "meters";
        nc_put_att_text(l_ncId, m_varIdBathymetry, "units", strlen(units_attribute_Bathymetry), units_attribute_Bathymetry);
        
        
        
        //variable height

        l_dim3Ids[0] =l_dimTimeId;
        l_dim3Ids[1] =l_dimYId;
        l_dim3Ids[2] =l_dimXId;

        l_err = nc_def_var(l_ncId, "h", NC_FLOAT, 3, l_dim3Ids, &m_varIdHeight);
        checkNcErr(l_err);
        const char* units_attribute_height = "meters";
        nc_put_att_text(l_ncId, m_varIdHeight, "units", strlen(units_attribute_height), units_attribute_height);

        //variable momentum in x direction
        l_err = nc_def_var(l_ncId, "hu", NC_FLOAT, 3, l_dim3Ids, &m_varIdImpulseX);
        checkNcErr(l_err);
        const char* units_attribute_impulseX = "meters";
        nc_put_att_text(l_ncId, m_varIdImpulseX, "units", strlen(units_attribute_impulseX), units_attribute_impulseX);
        
        //variable momentum in y direction 
        
        l_err = nc_def_var(l_ncId, "hv", NC_FLOAT, 3, l_dim3Ids, &m_varIdImpulseY);
        checkNcErr(l_err);
        const char* units_attribute_impulseY = "meters";
        nc_put_att_text(l_ncId, m_varIdImpulseY, "units", strlen(units_attribute_impulseY), units_attribute_impulseY);
        
        //enddef
        l_err = nc_enddef( l_ncId ); 
        checkNcErr( l_err );
    }

3. lastly lets implement a test unit for the  ``NetCdf.test.cpp``.



.. code-block:: cpp 

    #include <catch2/catch.hpp>
    #include "../../constants.h"
    #include <sstream>
    #include <vector>
    #define private public
    #include "NetCdf.h"
    #include <filesystem>
    #undef public

    TEST_CASE( "Test the NetCdf-writer", "[NetCdfWrite]" ) {

        tsunami_lab::t_real l_h[10]  = { 0, 1, 2, 3, 4, 5, 6 ,7, 8 ,5};
        tsunami_lab::t_real l_hu[10] = { 6, 5, 4, 3, 2, 1, 0 ,7, 8 ,5};
        tsunami_lab::t_real l_hv[10] = { 6, 5, 4, 3, 2, 1, 0 ,7, 8 ,5};
        tsunami_lab::t_real l_b[10]  = { 0, 0, 0, 0, 0, 0, 0 ,7, 8 ,5};

        tsunami_lab::io::NetCdf* l_netCdf = new tsunami_lab::io::NetCdf(5,2,"testsFiles/test.nc");
        l_netCdf->fillConstants(5,2,1,-50,-50,0,l_b,"testsFiles/test.nc");
        l_netCdf->updateFile(5,2,1,1,l_h,l_hu,l_hv,"testsFiles/test.nc");
        int l_err;
        int l_ncidp;
        int  l_dimXId,l_dimYId,l_TimeId;
        size_t l_nx,l_ny , l_nt;
        int l_vHId, l_vXId,l_vHuId, l_vYId, l_vTimeId, l_vHvId ,l_vBId;

        REQUIRE(std::filesystem::exists("testsFiles/test.nc"));

        l_err = nc_open("testsFiles/test.nc", NC_NOWRITE, &l_ncidp);
        l_netCdf->checkNcErr(l_err);

        l_err = nc_inq_dimid(l_ncidp,"x",&l_dimXId);
        l_netCdf->checkNcErr(l_err);

        l_err = nc_inq_dimid(l_ncidp,"y",&l_dimYId);
        l_netCdf->checkNcErr(l_err);

        l_err= nc_inq_dimid(l_ncidp, "time", &l_TimeId);
        l_netCdf->checkNcErr(l_err);

        l_err = nc_inq_dimlen(l_ncidp, l_dimXId, &l_nx);
        l_netCdf->checkNcErr(l_err);

        l_err = nc_inq_dimlen(l_ncidp, l_dimYId, &l_ny);
        l_netCdf->checkNcErr(l_err);

        l_err = nc_inq_dimlen(l_ncidp, l_TimeId, &l_nt);
        l_netCdf->checkNcErr(l_err);

        REQUIRE(l_nx == 5);
        REQUIRE(l_ny == 2);
        REQUIRE(l_nt == 1);


        l_err = nc_inq_varid(l_ncidp, "x", &l_vXId);
        l_netCdf->checkNcErr(l_err);
        l_err = nc_inq_varid(l_ncidp, "y", &l_vYId);
        l_netCdf->checkNcErr(l_err);
        l_err = nc_inq_varid(l_ncidp, "time", &l_vTimeId);
        l_netCdf->checkNcErr(l_err);
        l_err = nc_inq_varid(l_ncidp, "h", &l_vHId);
        l_netCdf->checkNcErr(l_err);
        l_err = nc_inq_varid(l_ncidp, "hu", &l_vHuId);
        l_netCdf->checkNcErr(l_err);
        l_err = nc_inq_varid(l_ncidp, "hv", &l_vHvId);
        l_netCdf->checkNcErr(l_err);
        l_err = nc_inq_varid(l_ncidp, "b", &l_vBId);
        l_netCdf->checkNcErr(l_err);


        std::vector<tsunami_lab::t_real> x(10);
        std::vector<tsunami_lab::t_real> y(2);
        std::vector<tsunami_lab::t_real> time(1);
        std::vector<tsunami_lab::t_real> height(10);
        std::vector<tsunami_lab::t_real> momentum_x(10);
        std::vector<tsunami_lab::t_real> momentum_y(10);
        std::vector<tsunami_lab::t_real> bathymetry(10);
        
        
        l_err = nc_get_var_float(l_ncidp, l_vXId, &x[0]);
        l_netCdf->checkNcErr(l_err);
        l_err = nc_get_var_float(l_ncidp, l_vYId, &y[0]);
        l_netCdf->checkNcErr(l_err);
        l_err = nc_get_var_float(l_ncidp, l_vTimeId, &time[0]);
        l_netCdf->checkNcErr(l_err);
        l_err = nc_get_var_float(l_ncidp, l_vHId, &height[0]);
        l_netCdf->checkNcErr(l_err);
        l_err = nc_get_var_float(l_ncidp, l_vHuId, &momentum_x[0]);
        l_netCdf->checkNcErr(l_err);
        l_err = nc_get_var_float(l_ncidp, l_vHvId, &momentum_y[0]);
        l_netCdf->checkNcErr(l_err);
        l_err = nc_get_var_float(l_ncidp, l_vBId, &bathymetry[0]);
        l_netCdf->checkNcErr(l_err);

        REQUIRE(x[0] == Approx(-49.5));
        REQUIRE(x[1] == Approx(-48.5));
        REQUIRE(x[2] == Approx(-47.5));
        REQUIRE(x[3] == Approx(-46.5));
        REQUIRE(x[4] == Approx(-45.5));

        REQUIRE(y[0] == Approx(-49.5));
        REQUIRE(y[1] == Approx(-48.5));


        REQUIRE(time[0] == Approx(1));

        REQUIRE(momentum_x[0] == Approx(4));
        REQUIRE(momentum_x[1] == Approx(3));
        REQUIRE(momentum_x[2] == Approx(2));
        REQUIRE(momentum_x[3] == Approx(1));
        REQUIRE(momentum_x[4] == Approx(0));
        REQUIRE(momentum_x[5] == Approx(3));
        REQUIRE(momentum_x[6] == Approx(2));
        REQUIRE(momentum_x[7] == Approx(1));
        REQUIRE(momentum_x[8] == Approx(0));
        REQUIRE(momentum_x[9] == Approx(7));

        REQUIRE(momentum_y[0] == Approx(4));
        REQUIRE(momentum_y[1] == Approx(3));
        REQUIRE(momentum_y[2] == Approx(2));
        REQUIRE(momentum_y[3] == Approx(1));
        REQUIRE(momentum_y[4] == Approx(0));
        REQUIRE(momentum_y[5] == Approx(3));
        REQUIRE(momentum_y[6] == Approx(2));
        REQUIRE(momentum_y[7] == Approx(1));
        REQUIRE(momentum_y[8] == Approx(0));
        REQUIRE(momentum_y[9] == Approx(7));

    for (size_t i = 0; i < 10; i++)
        {
            REQUIRE(bathymetry[i] == Approx(0));
        }

        delete l_netCdf;
        
        
    }

NetCDF Input
-------------

Implementation and test for the artificial tsunami setup
.........................................................

Now, let's proceed to implement the following files ``ArtificialTsunami2d.cpp`` , ``ArtificialTsunami2d.h`` and ``ArtificialTsunami2d.test.cpp`` utilizing the provided data.

.. math::

   \begin{split}
       h  &= \begin{cases}
               \max( -b_\text{in}, \delta), &\text{if } b_\text{in} < 0 \\
               0, &\text{else}
             \end{cases}\\
       hu &= 0\\
       hv &= 0\\
       b  &= \begin{cases}
               \min(b_\text{in}, -\delta) + d, & \text{ if } b_\text{in} < 0\\
               \max(b_\text{in}, \delta) + d, & \text{ else}.
             \end{cases}
   \end{split}


and 


.. math:: \text{d}: [-500, +500]\times [-500, +500] \rightarrow \mathbb{R}

.. math::
   :label: eq:artificial

   \begin{aligned}
       \text{d}(x, y) & = & 5 \cdot f(x)g(y) \\
       \text{f}(x) & = & \sin\left(\left(\frac{x}{500}+1\right) \cdot \pi\right) \\
       \text{g}(y) & = & -\left(\frac{y}{500}\right)^2 + 1
     \end{aligned}


1. Let's proceed with the implementation of the initial document ``ArtificialTsunami2d.h``:

.. code-block:: cpp



    #ifndef TSUNAMI_LAB_SETUPS_ARTIFICIALTSUNAMI2D_H
    #define TSUNAMI_LAB_SETUPS_ARTIFICIALTSUNAMI2D_H

    #include "../Setup.h"
    #include <fstream>
    #include <sstream>
    #include <vector>


    namespace tsunami_lab {
    namespace setups {
        class ArtificialTsunami2d;
    }
    }

    /**
    * ArtificialTsunami2d setup.
    **/
    class tsunami_lab::setups::ArtificialTsunami2d: public Setup {

    private:
        /**
        * @param m_delta avoids running into numerical issues due to missing support for wetting and drying in our solver.
        */
        t_real m_delta = 0;
        /**
        * @param m_Bin The swimming pool has a constant bathymetry of -100m before the “earthquake” 
        */
        t_real m_Bin = -100;
        
        /**
        * @brief the method adds the vertical displacement, typically caused by a subduction-zone earthquake.
        * @param i_x is the distance from the Fukushima Daini Nuclear Power Plant.
        */
        t_real displacement( t_real i_x ,t_real i_y) const;

        t_real gFunction(t_real i_y) const;

        t_real fFunction(t_real i_x) const;

    public:
    
        /**
        * @brief The constructor for ArtificialTsunami2d.
        * @param i_delta to avoid running into numerical issues (small value)
        **/

        ArtificialTsunami2d( t_real i_delta);

        /**
        * @brief is an arificial simulation so no inputs needed
        * @return height is hardcoded to 100;
        **/
        t_real getHeight( t_real ,
                        t_real ) const;

        /**
        * @brief Gets the momentum in x-direction.
        * @return Momentum in x-direction.
        **/
        t_real getMomentumX( t_real,
                            t_real) const;

        /**
        * @brief Gets the momentum in y-direction.
        * @return Momentum in y-direction.
        **/
        t_real getMomentumY( t_real,
                            t_real ) const;
        
        /**
        * @brief Computes the bathymetry explained in <a href="https://scalable.uni-jena.de/opt/tsunami/chapters/assignment_5.html#equation-eq-artificial">5.2.1</a>.
        * @param i_x position in the swimming pool.
        * @param i_x position in the swimming pool.
        * @return artificial computed Bathymetry.
        */
        t_real getBathymetry( t_real i_x,
                            t_real i_y) const ;

    
    };

    #endif



2. Now, we will proceed with the implementation of the  ``ArtificialTsunami2d.cpp`` file :

.. code-block:: cpp

    #include "ArtificialTsunami2d.h"
    #include <cmath>
    #include <cstddef> 


    tsunami_lab::setups::ArtificialTsunami2d::ArtificialTsunami2d(t_real i_delta){

        m_delta = i_delta;

    }

    tsunami_lab::t_real tsunami_lab::setups::ArtificialTsunami2d::getBathymetry( t_real i_x,
                                                                                t_real i_y) const {


        //m_Bin is hardcoded to -100                                                                     
        
        if(m_Bin < 0 ){
            if( m_Bin < -m_delta){
                return m_Bin + displacement(i_x,i_y);  
            }else{
                return -m_delta + displacement(i_x,i_y);
            }
        }else{
        if( m_Bin > m_delta){
                return m_Bin    + displacement(i_x,i_y);
            }else{
                return m_delta + displacement(i_x,i_y);
            }
        }
    }

    tsunami_lab::t_real tsunami_lab::setups::ArtificialTsunami2d::getHeight( t_real, 
                                                                        t_real )const{
        //-100 m bathymetry and water level is at 0 -> 100m height
        return 100;                                                                                                                                                                                   
    }

    tsunami_lab::t_real tsunami_lab::setups::ArtificialTsunami2d::displacement( t_real i_x,t_real i_y) const{

        if((i_x > -500) && (i_y > -500) && (i_x < 500) && (i_y < 500) ){
            return 5 * fFunction(i_x) * gFunction(i_y);
        }else{
            return 0;
        }
    }

    tsunami_lab::t_real tsunami_lab::setups::ArtificialTsunami2d::fFunction(t_real i_x) const{
        return sin(((i_x/500)+1)* M_PI );
    }

    tsunami_lab::t_real tsunami_lab::setups::ArtificialTsunami2d::gFunction(t_real i_y) const{
        return -((i_y/500)*(i_y/500))+1;
    }

    tsunami_lab::t_real tsunami_lab::setups::ArtificialTsunami2d::getMomentumY(  t_real ,
                                                                            t_real )const{
        return 0;
    }



    tsunami_lab::t_real tsunami_lab::setups::ArtificialTsunami2d::getMomentumX(  t_real,
                                                                            t_real )const{
        return 0;
    }



3. Finally, let's implement the test unit for the ArtificialTsunami2d in the ``ArtificialTsunami2d.test.cpp`` file:

.. code-block:: cpp

    #include <catch2/catch.hpp>
    #include "ArtificialTsunami2d.h"
    #include "string"

    TEST_CASE("Test the two-dimensional ArtificialTsunami2d setup.", "[ArtificialTsunami2d]")
    {
        tsunami_lab::setups::ArtificialTsunami2d* l_ArtificialTsunami2d = new tsunami_lab::setups::ArtificialTsunami2d(20);

        tsunami_lab::t_real l_momentumX = l_ArtificialTsunami2d->getMomentumX(0,0);
        REQUIRE(l_momentumX == Approx(0));

        tsunami_lab::t_real l_momentumY = l_ArtificialTsunami2d->getMomentumY(0,0);
        REQUIRE(l_momentumY == Approx(0));

        tsunami_lab::t_real l_bathymetryValue = l_ArtificialTsunami2d->getBathymetry(0,0);
        REQUIRE(l_bathymetryValue == Approx(-99.84296f));
        l_bathymetryValue = l_ArtificialTsunami2d->getBathymetry(9,0);
        REQUIRE(l_bathymetryValue == Approx(-99.84296f));
        l_bathymetryValue = l_ArtificialTsunami2d->getBathymetry(0,4);
        REQUIRE(l_bathymetryValue == Approx(-100.15704f));
        l_bathymetryValue = l_ArtificialTsunami2d->getBathymetry(9,4);
        REQUIRE(l_bathymetryValue == Approx( -100.15704f));

        tsunami_lab::t_real l_heightValue = l_ArtificialTsunami2d->getHeight(2,1);
        l_heightValue = l_ArtificialTsunami2d->getHeight(0,0);
        REQUIRE(l_heightValue == 100.0f);
        l_heightValue = l_ArtificialTsunami2d->getHeight(0,4);
        REQUIRE(l_heightValue == 100.0f);
        l_heightValue = l_ArtificialTsunami2d->getHeight(9,0);
        REQUIRE(l_heightValue == 100.0f);
        l_heightValue = l_ArtificialTsunami2d->getHeight(9,4);
        REQUIRE(l_heightValue == 100.0f);
    }


    //within the boundary [-500,500]x[-500,500]->R
    //x = 300 , y = 300

    /*(300 > -500) && (300 > -500) && (300 < 500) && (300 < 500) -> true
        =  m_Bin + displacement(i_x,i_y)
        
        = -100 + 5 * fFunction(i_x)             *         gFunction(i_y);
        
        = -100 + 5 * sin(((300/500)+1)* M_PI )   * (-((300/500)*(300/500))+1) 

        ca ~ 100 + -3.04338

        = -103.04338f
    */
    REQUIRE( l_ArtificialTsunami2d.getHeight( 300, 300 ) == 100.0f);
    
    REQUIRE( l_ArtificialTsunami2d.getBathymetry( 300, 300 ) == -103.04338f );
    
    REQUIRE( l_ArtificialTsunami2d.getMomentumX( 300, 300 ) == 0);

    REQUIRE( l_ArtificialTsunami2d.getMomentumY( 300, 300 ) == 0 );
    /*
        x = 10 , y = 100;
        -> both values in the domain ->

        /*(10 > -500) && (10 > -500) && (100 < 500) && (100 < 500) -> true
        =  m_Bin + displacement(i_x,i_y)
        
        = -100 + 5 * fFunction(i_x)             *         gFunction(i_y);
        
        = -100 + 5 * sin(((10/500)+1)* M_PI )   * (-((100/500)*(100/500))+1) 

        ca ~ -100 + -0.30139449

        = -100.30139449f
    */
    REQUIRE( l_ArtificialTsunami2d.getHeight( 10, 100 ) == 100.0f );

    REQUIRE( l_ArtificialTsunami2d.getBathymetry( 10, 100 ) == -100.30139449f );
    
    REQUIRE( l_ArtificialTsunami2d.getMomentumX( 10, 100 ) == 0);

    REQUIRE( l_ArtificialTsunami2d.getMomentumY( 10, 100 ) == 0 );  
    } 


adding support for reading netCDF files
.........................................

To enable reading from our NetCDF files, we need to implement the read function:

To implement the function, we'll utilize the filename, variablename (which may represent a dimension or a variable), 
and a vector to store the data.
The function is expected to return an integer indicating the state.
If the function returns -1, it should result in the program's exit.

now lests implement the following fucntion in the ``NetCdf.cpp`` the fowllowing function:


.. code-block:: cpp

    int tsunami_lab::io::NetCdf::read(  const char* filename,
                                        const char* varname,
                                        std::vector<t_real>  & data){
        int l_ncId; 
        //if an error occurs it return -1
        if (nc_open(filename, NC_NOWRITE, &l_ncId) != NC_NOERR) {
            std::cerr << "Error opening NetCDF file: " << filename << std::endl;
            return -1;
        }

        int varid; 
        //get the variable identifier
        if (nc_inq_varid(l_ncId, varname, &varid) != NC_NOERR) {
            std::cerr << "Error getting variable ID for variable: " << varname << std::endl;
            nc_close(l_ncId);
            return -1; 
        }

        int ndims; 
        int dimids[NC_MAX_VAR_DIMS];
        // get amount of dimensions (ndims) and the identifiers of the dimensions (dimids)
        if (nc_inq_var(l_ncId, varid, nullptr, nullptr, &ndims, dimids, nullptr) != NC_NOERR) {
            std::cerr << "Error getting variable information for variable: " << varname << std::endl;
            nc_close(l_ncId);
            return -1;
        }

        //get the length of the dimension
        t_idx dataSize = 1;
        for (int i = 0; i < ndims; ++i) {
            char dimname[NC_MAX_NAME + 1];
            size_t dimlen;
            if (nc_inq_dim(l_ncId, dimids[i], dimname, &dimlen) != NC_NOERR) {
                std::cerr << "Error getting dimension information." << std::endl;
                nc_close(l_ncId); 
            }
            dataSize *= dimlen;
        }
        //write the data in the vector
        data.resize(dataSize);
        int l_err = nc_get_var_float(l_ncId, varid , &data[0]);
        checkNcErr(l_err);

        if (nc_close(l_ncId) != NC_NOERR) {
            std::cerr << "Error closing NetCDF file: " << filename << std::endl;
        }
        return 1;
    }


now we have to add the function to the header file ``NetCdf.h``:

.. code-block:: cpp

    
    int read( const char* filename,
              const char* varname,
              std::vector<t_real> &data);


Let's now create a test unit for the reader function within the``NetCdf.test.cpp`` file:


Now, locate the file we utilized either in our project folder or download it from the provided source `testfile`_

.. _testfile: https://people.sc.fsu.edu/~jburkardt/data/netcdf/simple_xy.nc


.. code-block:: cpp 

    TEST_CASE( "Test the NetCdf-reader ", "[NetCdfreader]" ) {
        std::vector<tsunami_lab::t_real> data;

        tsunami_lab::io::NetCdf* l_netCdf = new tsunami_lab::io::NetCdf(10,10,"testsFiles/reader.nc");
    

        l_netCdf->read("testsFiles/testFileForTheReader.nc" , "data", data ) ;
        


    for (tsunami_lab::t_real i = 0; i < 71; i++)
    {
        REQUIRE(i == data[i]);
    }

    delete l_netCdf;

    }




Integration the new class TsunamiEvent2d
.........................................


lets implement TsunamiEvent2d setup. for the fowllowing  setup we will need to implement the fowllowing files : 
``TsunamiEvent2d.cpp`` , ``TsunamiEvent2d.h`` and ``TsunamiEvent2d.test.cpp``. 

Prior to incorporating TsunamiEvent2d, it is essential to develop the following function, which will determine the nearest available value in the grid file in case the exact coordinates are unavailable:

.. code-block:: cpp

    tsunami_lab::t_idx tsunami_lab::setups::TsunamiEvent2d::findClosestIndex(const std::vector<t_real>& vec, t_real value) const {

        t_idx closestIndex = 0;
        for (t_idx index = 0; index < vec.size(); ++index) {
            if (vec[index] > value) {
                if (value - vec[index - 1] > vec[index] - value){
                    closestIndex = index ;
                }
                else{
                    closestIndex = index- 1;
                }
                break;
            }
        }

        return closestIndex;
    }



Now, let's commence the implementation of the following header file,``TsunamiEvent2d.h`` :




.. code-block:: cpp 

        /**
    * @author Ward Tammaa 
    *
    * @section DESCRIPTION

    **/
    #ifndef TSUNAMI_LAB_SETUPS_TSUNAMIEVENT2D_H
    #define TSUNAMI_LAB_SETUPS_TSUNAMIEVENT2D_H

    #include "../Setup.h"
    #include <fstream>
    #include <sstream>
    #include <vector>


    namespace tsunami_lab {
    namespace setups {
        class TsunamiEvent2d;
    }
    }

    /**
    * TsunamiEvent2d setup.
    **/
    class tsunami_lab::setups::TsunamiEvent2d: public Setup {

    private:
        /**
        * @param m_delta avoids running into numerical issues due to missing support for wetting and drying in our solver.
        */
        t_real m_delta               = 0;
        t_real m_width_bathymetry    = 0; 
        t_real m_length_bathymetry   = 0;
        t_real m_width_displacement  = 0;
        t_real m_length_displacement = 0;


        /**
        * @param m_bathymetry_values contains all bathymetry values 
        */
        std::vector<t_real> m_bathymetry_values;
        std::vector<t_real> m_bathymetry_x_values;
        std::vector<t_real> m_bathymetry_y_values;

        std::vector<t_real> m_displacement_values;
        std::vector<t_real> m_displacement_x_values;
        std::vector<t_real> m_displacement_y_values;

        t_real getBathymetryNetCdf(t_real i_x, t_real i_y) const;
        
        t_idx findClosestIndex(const std::vector<t_real>& vec, t_real value) const;


    public:

    
        /**
        * @brief the method adds the vertical displacement, typically caused by a subduction-zone earthquake.
        * @param i_x is the distance from the Fukushima Daini Nuclear Power Plant.
        */
    
        t_real displacement( t_real i_x,t_real i_y) const;
        /**
        * @brief The constructor for TsunamiEvent2d.
        * @param i_delta to avoid running into numerical issues (small value)
        **/

        TsunamiEvent2d( t_real i_delta);

        /**
        * @brief Gets the water height at a given point.
        *
        * @param i_x is the distance from the Fukushima Daini Nuclear Power Plant
        * @return Height at the given point.
        **/
        t_real getHeight( t_real i_x,
                        t_real i_y) const;

        /**
        * @brief Gets the momentum in x-direction.
        *
        * @return Momentum in x-direction.
        **/
        t_real getMomentumX( t_real ,
                            t_real ) const;

        /**
        * @brief Gets the momentum in y-direction.
        * @return Momentum in y-direction.
        **/
        t_real getMomentumY( t_real,
                            t_real ) const;
        
        /**
        * @brief Computes the bathymetry as explained in <a href="https://scalable.uni-jena.de/opt/tsunami/chapters/assignment_3.html#equation-eq-tsunami-event-1d">3.4.1</a>.
        * @param i_x is the distance from the Fukushima Daini Nuclear Power Plant.
        * @return Bathymetry.
        */
        t_real getBathymetry( t_real i_x,
                            t_real i_y) const ;

        /** 
        * @brief divide the distance i_x by 250 to determine the index for the bathymetry in the csv file.
        * @param i_x is the distance from the Fukushima Daini Nuclear Power Plant.
        * @return Bathymetry in the csv file (not the value that we use).
        */
        

    };

    #endif




Let's now proceed with the implementation of the ``TsunamiEvent2d.cpp`` file. Before we implement the class, we need to read the bathymetry and displacement data from the files given in the task.

.. code-block:: cpp
    
        #include "TsunamiEvent2d.h"
        #include "../../io/NetCdf/NetCdf.h"
        #include <cmath>
        #include <cstddef> 


        tsunami_lab::setups::TsunamiEvent2d::TsunamiEvent2d(t_real i_delta)
        {

            tsunami_lab::io::NetCdf::read("data/artificialtsunami_bathymetry_1000.nc","z",m_bathymetry_values);
            tsunami_lab::io::NetCdf::read("data/artificialtsunami_bathymetry_1000.nc","x",m_bathymetry_x_values);
            tsunami_lab::io::NetCdf::read("data/artificialtsunami_bathymetry_1000.nc","y",m_bathymetry_y_values); 
        
            tsunami_lab::io::NetCdf::read("data/artificialtsunami_displ_1000.nc","z",m_displacement_values);
            tsunami_lab::io::NetCdf::read("data/artificialtsunami_displ_1000.nc","x",m_displacement_x_values);
            tsunami_lab::io::NetCdf::read("data/artificialtsunami_displ_1000.nc","y",m_displacement_y_values);

            m_delta = i_delta;
            
        }

        tsunami_lab::t_real tsunami_lab::setups::TsunamiEvent2d::getBathymetry( t_real i_x,
                                                                                t_real i_y) const {

            
            t_real l_batNetCdf = getBathymetryNetCdf(i_x, i_y);

            if(l_batNetCdf < 0 )
            {
                if(l_batNetCdf < -m_delta)
                {
                    return l_batNetCdf + displacement(i_x, i_y);
                }
                else
                {
                    return  displacement(i_x, i_y) - m_delta;
                }
            }
            else
            {
                if(l_batNetCdf < m_delta)
                {
                    return  displacement(i_x, i_y) + m_delta;
                }
                else
                {
                    return l_batNetCdf + displacement(i_x, i_y);
                }
            }

        }

        tsunami_lab::t_real tsunami_lab::setups::TsunamiEvent2d::displacement( t_real i_x,t_real i_y) const {

            if (i_x < m_displacement_x_values[0] || i_x > m_displacement_x_values[m_displacement_x_values.size() - 1] ||
                i_y < m_displacement_y_values[0] || i_y > m_displacement_y_values[m_displacement_y_values.size() - 1])
            {
                return 0;
            }
            //nachdem man schon eigentlich den index raushat wollen wir ihn nochmal berechnen????? 
            t_idx l_x = findClosestIndex(m_displacement_x_values, i_x);
            t_idx l_y = findClosestIndex(m_displacement_y_values, i_y);

            return m_displacement_values[l_x * m_displacement_y_values.size() + l_y];
        }

        tsunami_lab::t_real tsunami_lab::setups::TsunamiEvent2d::getHeight( t_real i_x,
                                                                            t_real i_y)const{

            t_real l_bin = getBathymetryNetCdf(i_x, i_y);

            if (l_bin < 0) {
                return (-l_bin > m_delta) ? -l_bin : m_delta;
            }
            return 0;
        }

        tsunami_lab::t_real tsunami_lab::setups::TsunamiEvent2d::getBathymetryNetCdf(t_real i_x, t_real i_y) const {
            //check whether the position is within our domain
            if (i_x < m_bathymetry_x_values[0] || i_x > m_bathymetry_x_values[m_bathymetry_x_values.size() - 1] ||
                i_y < m_bathymetry_y_values[0] || i_y > m_bathymetry_y_values[m_bathymetry_y_values.size() - 1])
            {
                return 0;
            }

            t_idx l_x = findClosestIndex(m_bathymetry_x_values, i_x);
            t_idx l_y = findClosestIndex(m_bathymetry_y_values, i_y);
            
            return m_bathymetry_values[l_y * m_bathymetry_x_values.size() + l_x];
        }

        tsunami_lab::t_idx tsunami_lab::setups::TsunamiEvent2d::findClosestIndex(const std::vector<t_real>& vec, t_real value) const {

            t_idx closestIndex = 0;
            for (t_idx index = 0; index < vec.size(); ++index) {
                if (vec[index] > value) {
                    if (value - vec[index - 1] > vec[index] - value){
                        closestIndex = index ;
                    }
                    else{
                        closestIndex = index- 1;
                    }
                    break;
                }
            }

            return closestIndex;
        }


        tsunami_lab::t_real tsunami_lab::setups::TsunamiEvent2d::getMomentumY(  t_real,
                                                                                t_real)const{
            return 0;                                                                                                                                                                      
        }



        tsunami_lab::t_real tsunami_lab::setups::TsunamiEvent2d::getMomentumX(  t_real ,
                                                                                t_real)const{
            return 0;                                                                                                                       
        }                    




Finally, let's proceed with implementing the unit tests in the ``TsunamiEvent2d.test.cpp`` file:

.. code-block:: cpp

        #include <catch2/catch.hpp>
    #include "TsunamiEvent2d.h"
    #include "string"

    TEST_CASE("Test the two-dimensional tsunamiEvent setup.", "[TsunamiEvent2d]")
    {
    tsunami_lab::setups::TsunamiEvent2d* l_tsunamiEvent2d = new tsunami_lab::setups::TsunamiEvent2d(20);

    tsunami_lab::t_real l_momentumX = l_tsunamiEvent2d->getMomentumX(0,0);
    REQUIRE(l_momentumX == Approx(0));

    tsunami_lab::t_real l_momentumY = l_tsunamiEvent2d->getMomentumY(0,0);
    REQUIRE(l_momentumY == Approx(0));

    tsunami_lab::t_real l_bathymetryValue = l_tsunamiEvent2d->getBathymetry(0,0);
    REQUIRE(l_bathymetryValue == Approx(-99.84296f));
    l_bathymetryValue = l_tsunamiEvent2d->getBathymetry(9,0);
    REQUIRE(l_bathymetryValue == Approx(-99.84296f));
    l_bathymetryValue = l_tsunamiEvent2d->getBathymetry(0,4);
    REQUIRE(l_bathymetryValue == Approx(-100.15704f));
    l_bathymetryValue = l_tsunamiEvent2d->getBathymetry(9,4);
    REQUIRE(l_bathymetryValue == Approx( -100.15704f));

    tsunami_lab::t_real l_heightValue = l_tsunamiEvent2d->getHeight(2,1);
    l_heightValue = l_tsunamiEvent2d->getHeight(0,0);
    REQUIRE(l_heightValue == 100.0f);
    l_heightValue = l_tsunamiEvent2d->getHeight(0,4);
    REQUIRE(l_heightValue == 100.0f);
    l_heightValue = l_tsunamiEvent2d->getHeight(9,0);
    REQUIRE(l_heightValue == 100.0f);
    l_heightValue = l_tsunamiEvent2d->getHeight(9,4);
    REQUIRE(l_heightValue == 100.0f);
    }

Check the correctness 
......................

The last question was to compare the ``ArtificialTsunami2d`` with the ``TsunamiEvent2d`` the first animation is from the
``ArtificialTsunami2d``:

.. video:: _static/Artificial_sim.mp4
      :width: 700
      :autoplay:

And now the ``TsunamiEvent2d`` animation

.. video:: _static/Tsunami_decent_2D.mp4
      :width: 700
      :autoplay:

The second video had these settings : "domain_start_x" : -5050,"domain_start_y" : -5050 ,"dimension_x" : 10100,"dimension_y" : 10100,

As you can see both simulation are very similar but the best way to test the correctness is by having the same tests for both setups.
In case they haven't already noticed :D








Personal Contribution
---------------------

- Ward Tammaa, Daniel Schicker Doxygen Documentation
- Mohamad Khaled Minawe, Ward Tammaa, Daniel Schicker Sphnix Documentation
- Daniel Schicker, Mohamad Khaled Minawe , Ward Tammaa functions implementation
- Mohamad Khaled Minawe, Daniel Schicker, Ward Tammaa Unit Testing
- Mohamad Khaled Minawe, Daniel Schicker Geogebra Datei(Calculations for the Unit Tests)
- Ward Tammaa Hosting the code , Action runner


    