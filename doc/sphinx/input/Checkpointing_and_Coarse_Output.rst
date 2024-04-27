

Checkpointing and Coarse Output
======================================

Checkpointing
--------------

To implement the checkpoint feature, we'll enhance our NetCDF writer and modify the `main.cpp` file. Initially, let's create a method responsible for generating the checkpoint. Within this method, we'll include all necessary components for compiling
our program and additionally store station information.

.. code-block:: cpp

    void tsunami_lab::io::NetCdf::createCheckPoint(std::string i_solver,
                                            t_real i_domain_start_x,
                                            t_real i_domain_start_y,
                                            t_real i_dimension_x,
                                            t_real i_dimension_y,
                                            t_real i_endtime,
                                            t_real i_simTime,
                                            t_real i_frequency,
                                            t_real i_dt,
                                            t_real i_simulation_time_for_last_cp,
                                            const t_real *i_b,
                                            const t_real *i_h,
                                            const t_real *i_hu,
                                            const t_real *i_hv,
                                            t_idx i_time_step_index,
                                            t_idx i_stride,
                                            t_idx i_nx,
                                            t_idx i_ny,
                                            std::string i_setup,
                                            std::string i_stations_string,
                                            std::string i_name_cp,
                                            std::string i_disfile,
                                            std::string i_batfile){
    
    std::string folder_path = "outputs/cp";
    std::string l_name_cp = "outputs/cp/"+i_name_cp;
    int l_err,l_ncId;
    l_err = nc_create(l_name_cp.data(), NC_NETCDF4,&l_ncId);
    checkNcErr(l_err,__FILE__, __LINE__);

    int l_dimXId, l_dimYId;
    l_err = nc_def_dim(l_ncId, "x", i_nx, &l_dimXId);
    checkNcErr(l_err,__FILE__, __LINE__);
    l_err = nc_def_dim(l_ncId, "y", i_ny, &l_dimYId);
    checkNcErr(l_err,__FILE__, __LINE__);
    
    int l_var_domain_start_x_id, l_var_domain_start_y_id , l_var_solver_id, l_var_dimension_x_id,
        l_var_dimension_y_id,l_var_endtime_id, l_var_simTime_id, l_var_frequency_id, l_var_b_id,
        l_var_h_id,l_var_hu_id,l_var_hv_id, l_var_dt_id , l_var_disfile_id,l_var_batfile_id;    
    int  l_var_time_step_index_id,l_var_stations_string,l_var_setup, l_simulation_time_for_last_cp_id;
    //-----------------------------------------------------Define Variables
    l_err = nc_def_var(l_ncId,"domain_start_x",NC_FLOAT,0 , nullptr, &l_var_domain_start_x_id);
    checkNcErr(l_err,__FILE__, __LINE__);
    l_err = nc_def_var(l_ncId,"domain_start_y",NC_FLOAT,0 , nullptr, &l_var_domain_start_y_id);
    checkNcErr(l_err,__FILE__, __LINE__);
    l_err = nc_def_var(l_ncId,"solver",NC_INT,0 , nullptr, &l_var_solver_id);
    checkNcErr(l_err,__FILE__, __LINE__);
    l_err = nc_def_var(l_ncId,"dimension_x",NC_FLOAT,0 , nullptr, &l_var_dimension_x_id);
    checkNcErr(l_err,__FILE__, __LINE__);

    l_err = nc_def_var(l_ncId,"simulationtimeforlastcp",NC_FLOAT,0 , nullptr, &l_simulation_time_for_last_cp_id);
    checkNcErr(l_err,__FILE__, __LINE__);


    l_err = nc_def_var(l_ncId,"dimension_y",NC_FLOAT,0 , nullptr, &l_var_dimension_y_id);
    checkNcErr(l_err,__FILE__, __LINE__);
    l_err = nc_def_var(l_ncId,"endtime",NC_FLOAT,0 , nullptr, &l_var_endtime_id);
    checkNcErr(l_err,__FILE__, __LINE__);
    l_err = nc_def_var(l_ncId,"simTime",NC_FLOAT,0 , nullptr, &l_var_simTime_id);
    checkNcErr(l_err,__FILE__, __LINE__);
    l_err = nc_def_var(l_ncId,"setup",NC_STRING,0 , nullptr, &l_var_setup);
    checkNcErr(l_err,__FILE__, __LINE__);
    l_err = nc_def_var(l_ncId,"stations",NC_STRING,0 , nullptr, &l_var_stations_string);
    checkNcErr(l_err,__FILE__, __LINE__);
    l_err = nc_def_var(l_ncId,"timeStep",NC_INT,0 , nullptr, &l_var_time_step_index_id);
    checkNcErr(l_err,__FILE__, __LINE__);
    l_err = nc_def_var(l_ncId,"frequency",NC_FLOAT,0 , nullptr, &l_var_frequency_id);
    checkNcErr(l_err,__FILE__, __LINE__);
    l_err = nc_def_var(l_ncId,"dt",NC_FLOAT,0 , nullptr, &l_var_dt_id);
    checkNcErr(l_err,__FILE__, __LINE__);

    l_err = nc_def_var(l_ncId,"disfile",NC_STRING,0 , nullptr, &l_var_disfile_id);
    checkNcErr(l_err,__FILE__, __LINE__);
    l_err = nc_def_var(l_ncId,"batfile",NC_STRING,0 , nullptr, &l_var_batfile_id);
    checkNcErr(l_err,__FILE__, __LINE__);
    
    int l_dims[2] = {l_dimYId,l_dimXId};

    l_err = nc_def_var(l_ncId,"b",NC_FLOAT,2 , l_dims, &l_var_b_id);
    checkNcErr(l_err,__FILE__, __LINE__);

    l_err = nc_def_var(l_ncId,"hu",NC_FLOAT,2 , l_dims, &l_var_hu_id);
    checkNcErr(l_err,__FILE__, __LINE__);

    l_err = nc_def_var(l_ncId,"h",NC_FLOAT,2, l_dims, &l_var_h_id);
    checkNcErr(l_err,__FILE__, __LINE__);

    l_err = nc_def_var(l_ncId,"hv",NC_FLOAT,2 , l_dims, &l_var_hv_id);
    checkNcErr(l_err,__FILE__, __LINE__);

    l_err = nc_enddef(l_ncId);
    checkNcErr(l_err,__FILE__, __LINE__);

    //-----------------------------------------------------Write Variables
    const char* setupData = i_setup.data();
    l_err = nc_put_var_string(l_ncId, l_var_setup, &setupData);
    checkNcErr(l_err,__FILE__, __LINE__);

    const char* l_stations_string = i_stations_string.data();
    const char* l_disfile_string = i_disfile.data();
    const char* l_batfile_string = i_batfile.data();
    
    l_err = nc_put_var_string(l_ncId, l_var_disfile_id, &l_batfile_string);
    checkNcErr(l_err,__FILE__, __LINE__);

    l_err = nc_put_var_string(l_ncId, l_var_batfile_id, &l_disfile_string);
    checkNcErr(l_err,__FILE__, __LINE__);

    l_err = nc_put_var_string(l_ncId, l_var_stations_string, &l_stations_string);
    checkNcErr(l_err,__FILE__, __LINE__);

    l_err = nc_put_var_float(l_ncId,l_var_domain_start_x_id,&i_domain_start_x);
    checkNcErr(l_err,__FILE__, __LINE__);

    l_err = nc_put_var_float(l_ncId,l_simulation_time_for_last_cp_id,&i_simulation_time_for_last_cp);
    checkNcErr(l_err,__FILE__, __LINE__);


    l_err = nc_put_var_float(l_ncId, l_var_domain_start_y_id, &i_domain_start_y);
    checkNcErr(l_err,__FILE__, __LINE__);

    l_err = nc_put_var_float(l_ncId, l_var_dimension_x_id, &i_dimension_x);
    checkNcErr(l_err,__FILE__, __LINE__);

    l_err = nc_put_var_float(l_ncId, l_var_dimension_y_id, &i_dimension_y);
    checkNcErr(l_err,__FILE__, __LINE__);

    l_err = nc_put_var_float(l_ncId, l_var_endtime_id, &i_endtime);
    checkNcErr(l_err,__FILE__, __LINE__);

    l_err = nc_put_var_float(l_ncId, l_var_simTime_id, &i_simTime);
    checkNcErr(l_err,__FILE__, __LINE__);
    
    l_err = nc_put_var_float(l_ncId, l_var_frequency_id, &i_frequency);
    checkNcErr(l_err,__FILE__, __LINE__);

    l_err = nc_put_var_float(l_ncId, l_var_dt_id, &i_dt);
    checkNcErr(l_err,__FILE__, __LINE__);
    l_err = nc_put_var_int(l_ncId, l_var_time_step_index_id, (const int*)&i_time_step_index);
    checkNcErr(l_err,__FILE__, __LINE__);

    int l_solver = 0;
    if(i_solver == "roe") l_solver = 1;
    
    l_err = nc_put_var_int(l_ncId, l_var_solver_id, &l_solver);
    checkNcErr(l_err,__FILE__, __LINE__);

    std::vector<size_t> l_start     = {0, 0};
    std::vector<size_t> l_end       = {1,i_nx};
    std::vector<ptrdiff_t> l_strid = {1,1};

    for(l_start[0] = 0; l_start[0] < i_ny; l_start[0]++){
        l_err = nc_put_vars_float(l_ncId, l_var_b_id, l_start.data(), l_end.data(),l_strid.data() , (i_b + (l_start[0]+1) * i_stride+1));
        checkNcErr(l_err,__FILE__, __LINE__);
        l_err = nc_put_vars_float(l_ncId, l_var_h_id, l_start.data(), l_end.data(),l_strid.data() , (i_h + (l_start[0]+1) * i_stride+1));
        checkNcErr(l_err,__FILE__, __LINE__);
        l_err = nc_put_vars_float(l_ncId, l_var_hu_id, l_start.data(), l_end.data(),l_strid.data() , (i_hu + (l_start[0]+1) * i_stride+1));
        checkNcErr(l_err,__FILE__, __LINE__);
        l_err = nc_put_vars_float(l_ncId, l_var_hv_id, l_start.data(), l_end.data(),l_strid.data() , (i_hv + (l_start[0]+1) * i_stride+1));
        checkNcErr(l_err,__FILE__, __LINE__);
    }

    l_err = nc_close(l_ncId);
    checkNcErr(l_err,__FILE__, __LINE__);

    }



Once the checkpoint data is saved, we'll need to implement a method for reading the information from the checkpoint file. In this read method, we'll iterate through the contents of the checkpoint file and
retrieve each input that has been previously written.

.. code-block:: cpp 

    void tsunami_lab::io::NetCdf::readCheckPoint(std::string i_path_cp,
                                            std::string * o_solver,
                                            t_real * o_domain_start_x,
                                            t_real * o_domain_start_y,
                                            t_real * o_dimension_x,
                                            t_real * o_dimension_y,
                                            t_real * o_endtime,
                                            t_real * o_simTime,
                                            t_real * o_frequency,
                                            t_real * o_dt,
                                            t_real *o_simulation_time_for_last_cp,
                                            t_real ** o_b,
                                            t_real ** o_h,
                                            t_real ** o_hu,
                                            t_real ** o_hv,
                                            t_idx * o_time_step_index,
                                            t_idx * o_nx,
                                            t_idx * o_ny,
                                            t_idx * o_k,
                                            std::string * o_setup,
                                            std::string * o_stations_string,
                                            std::string * o_disfile,
                                            std::string * o_batfile){
    
    int l_ncId = 0,l_err = 0;
    l_err = nc_open(i_path_cp.c_str(), NC_NOWRITE, &l_ncId); 
    checkNcErr(l_err,__FILE__, __LINE__);

    int l_dimXId, l_dimYId;
    l_err = nc_inq_dimid(l_ncId, "x", &l_dimXId);
    checkNcErr(l_err,__FILE__, __LINE__);
    l_err = nc_inq_dimid(l_ncId, "y", &l_dimYId);
    checkNcErr(l_err,__FILE__, __LINE__);

    l_err = nc_inq_dimlen(l_ncId,l_dimXId,o_nx);
    checkNcErr(l_err,__FILE__, __LINE__);

    l_err = nc_inq_dimlen(l_ncId,l_dimYId,o_ny);
    checkNcErr(l_err,__FILE__, __LINE__);

    int l_var_domain_start_x_id, l_var_domain_start_y_id , l_var_solver_id, l_var_dimension_x_id,
        l_var_dimension_y_id,l_var_endtime_id, l_var_simTime_id, l_var_frequency_id, l_var_b_id,
        l_var_h_id,l_var_hu_id,l_var_hv_id, l_var_dt_id , l_var_disfile_id,l_var_batfile_id;    
    int  l_var_time_step_index_id,l_var_stations_string,l_var_setup,l_simulation_time_for_last_cp_id;

    l_err = nc_inq_varid(l_ncId,"domain_start_x",&l_var_domain_start_x_id);
    checkNcErr(l_err,__FILE__, __LINE__);

    l_err = nc_inq_varid(l_ncId,"domain_start_y",&l_var_domain_start_y_id);
    checkNcErr(l_err,__FILE__, __LINE__);

    l_err = nc_inq_varid(l_ncId,"solver",&l_var_solver_id);
    checkNcErr(l_err,__FILE__, __LINE__);

    l_err = nc_inq_varid(l_ncId,"simulationtimeforlastcp", &l_simulation_time_for_last_cp_id);
    checkNcErr(l_err,__FILE__, __LINE__);

    l_err = nc_inq_varid(l_ncId,"dimension_x",&l_var_dimension_x_id);
    checkNcErr(l_err,__FILE__, __LINE__);

    l_err = nc_inq_varid(l_ncId,"dimension_y",&l_var_dimension_y_id);
    checkNcErr(l_err,__FILE__, __LINE__);

    l_err = nc_inq_varid(l_ncId,"endtime",&l_var_endtime_id);
    checkNcErr(l_err,__FILE__, __LINE__);

    l_err = nc_inq_varid(l_ncId,"simTime",&l_var_simTime_id);
    checkNcErr(l_err,__FILE__, __LINE__);

    l_err = nc_inq_varid(l_ncId,"setup",&l_var_setup);
    checkNcErr(l_err,__FILE__, __LINE__);

    l_err = nc_inq_varid(l_ncId,"stations",&l_var_stations_string);
    checkNcErr(l_err,__FILE__, __LINE__);

    l_err = nc_inq_varid(l_ncId,"timeStep",&l_var_time_step_index_id);
    checkNcErr(l_err,__FILE__, __LINE__);

    l_err = nc_inq_varid(l_ncId,"frequency",&l_var_frequency_id);
    checkNcErr(l_err,__FILE__, __LINE__);

    l_err = nc_inq_varid(l_ncId,"dt",&l_var_dt_id);
    checkNcErr(l_err,__FILE__, __LINE__);

    l_err = nc_inq_varid(l_ncId,"disfile",&l_var_disfile_id);
    checkNcErr(l_err,__FILE__, __LINE__);
  
    l_err = nc_inq_varid(l_ncId,"batfile",&l_var_batfile_id);
    checkNcErr(l_err,__FILE__, __LINE__);

    l_err = nc_inq_varid(l_ncId, "b", &l_var_b_id);
    checkNcErr(l_err,__FILE__, __LINE__);

    l_err = nc_inq_varid(l_ncId, "hu", &l_var_hu_id);
    checkNcErr(l_err,__FILE__, __LINE__);

    l_err = nc_inq_varid(l_ncId, "h", &l_var_h_id);
    checkNcErr(l_err,__FILE__, __LINE__);

    l_err = nc_inq_varid(l_ncId, "hv", &l_var_hv_id);
    checkNcErr(l_err,__FILE__, __LINE__);


    // GET THE VARIABLES

    char *l_setup;
    l_err = nc_get_var_string(l_ncId,l_var_setup,&l_setup);
    *o_setup = std::string(l_setup);
    checkNcErr(l_err,__FILE__, __LINE__);

    char *l_batfile;
    l_err = nc_get_var_string(l_ncId,l_var_batfile_id,&l_batfile);
    *o_batfile = std::string(l_batfile);
    checkNcErr(l_err,__FILE__, __LINE__);
 
    char *l_disfile;
    l_err = nc_get_var_string(l_ncId,l_var_disfile_id,&l_disfile);
    *o_disfile = std::string(l_disfile);
    checkNcErr(l_err,__FILE__, __LINE__);

    char *l_stations_string;
    l_err = nc_get_var_string(l_ncId,l_var_stations_string,&l_stations_string);
    *o_stations_string = std::string(l_stations_string);
    checkNcErr(l_err,__FILE__, __LINE__);

    l_err = nc_get_var_float(l_ncId,l_simulation_time_for_last_cp_id,o_simulation_time_for_last_cp);
    checkNcErr(l_err,__FILE__, __LINE__);

    l_err = nc_get_var_float(l_ncId,l_var_domain_start_x_id,o_domain_start_x);
    checkNcErr(l_err,__FILE__, __LINE__);

    l_err = nc_get_var_float(l_ncId,l_var_domain_start_y_id,o_domain_start_y);
    checkNcErr(l_err,__FILE__, __LINE__);

    l_err = nc_get_var_float(l_ncId,l_var_dimension_x_id,o_dimension_x);
    checkNcErr(l_err,__FILE__, __LINE__);

    l_err = nc_get_var_float(l_ncId,l_var_dimension_y_id,o_dimension_y);
    checkNcErr(l_err,__FILE__, __LINE__);

    l_err = nc_get_var_float(l_ncId,l_var_endtime_id,o_endtime);
    checkNcErr(l_err,__FILE__, __LINE__);

    l_err = nc_get_var_float(l_ncId,l_var_simTime_id,o_simTime);
    checkNcErr(l_err,__FILE__, __LINE__);

    l_err = nc_get_var_float(l_ncId,l_var_frequency_id,o_frequency);
    checkNcErr(l_err,__FILE__, __LINE__);

    l_err = nc_get_var_float(l_ncId, l_var_dt_id, o_dt);
    checkNcErr(l_err,__FILE__, __LINE__);

    int l_time_step_index;
    l_err = nc_get_var_int(l_ncId, l_var_time_step_index_id, &l_time_step_index);
    *o_time_step_index = l_time_step_index;
    checkNcErr(l_err,__FILE__, __LINE__);
    int l_solver = 0;

    l_err = nc_get_var_int(l_ncId, l_var_solver_id, &l_solver);
    checkNcErr(l_err,__FILE__, __LINE__);
    
    if(l_solver == 1){
        *o_solver = "roe";
    }else{
        *o_solver = "fwave";  
    }

    *o_b = new t_real[(*o_nx) * (*o_ny)];
    *o_h = new t_real[(*o_nx) * (*o_ny)];
    *o_hu = new t_real[(*o_nx) * (*o_ny)];
    *o_hv = new t_real[(*o_nx) * (*o_ny)];

    l_err = nc_get_var_float(l_ncId, l_var_b_id, *o_b);
    checkNcErr(l_err,__FILE__, __LINE__); 

    l_err = nc_get_var_float(l_ncId, l_var_h_id, *o_h);
    checkNcErr(l_err,__FILE__, __LINE__); 

    l_err = nc_get_var_float(l_ncId, l_var_hu_id, *o_hu);
    checkNcErr(l_err,__FILE__, __LINE__);

    l_err = nc_get_var_float(l_ncId, l_var_hv_id, *o_hv);
    checkNcErr(l_err,__FILE__, __LINE__);  

    l_err = nc_close(l_ncId);
    checkNcErr(l_err,__FILE__, __LINE__);
    
    free(l_setup);
    free(l_stations_string);
    free(l_batfile);
    free(l_disfile);
    }


Following the implementation of the write and read methods, it's essential to create a test unit to validate the functionality. This test unit will be added to the `Netcdf.test.cpp` file to ensure that the
checkpointing mechanisms works as intended.

.. code-block:: cpp

    
        #include <catch2/catch.hpp>
        #include "../../constants.h"
        #include <sstream>
        #include <vector>
        #define private public
        #include "NetCdf.h"
        #include <filesystem>
        #include <cstdio>
        #include <string>
        #undef public

        TEST_CASE("Test the NetCdf-writer", "[NetCdfWrite]")
        {

            tsunami_lab::t_real l_h[28] = {0, 1, 2, 3, 4, 5, 0,
                                        1, 1, 2, 3, 4, 5, 4,
                                        6, 6, 7, 8, 9, 10, 10,
                                        7, 7, 8, 9, 10, 11, 11};

            tsunami_lab::t_real l_hu[28] = {12, 12, 13, 14, 15, 16, 16,
                                            17, 17, 18, 19, 20, 21, 21,
                                            22, 22, 23, 24, 25, 26, 26,
                                            27, 27, 28, 29, 30, 31, 31};

            tsunami_lab::t_real l_hv[28] = {32, 32, 33, 34, 35, 36, 36,
                                            37, 37, 38, 39, 40, 41, 41,
                                            42, 42, 43, 44, 45, 46, 46,
                                            47, 47, 48, 49, 50, 51, 51};

            tsunami_lab::t_real l_b[28] = {52, 52, 53, 54, 55, 56, 56,
                                        57, 57, 58, 59, 60, 61, 61,
                                        62, 62, 63, 64, 65, 66, 66,
                                        67, 67, 68, 69, 70, 71, 71};

            tsunami_lab::io::NetCdf *l_netCdf = new tsunami_lab::io::NetCdf(5, 2, 1, "testsFiles/test.nc");
            l_netCdf->fillConstants(5, 2, 1, 7, 0.5, -50, -50, l_b, "testsFiles/test.nc");
            l_netCdf->updateFile(5, 2, 7, 0, 1, 1, l_h, l_hu, l_hv, "testsFiles/test.nc");
            int l_err;
            int l_ncid;
            int l_dimXId, l_dimYId, l_TimeId;
            size_t l_nx, l_ny, l_nt;
            int l_vHId, l_vXId, l_vHuId, l_vYId, l_vTimeId, l_vHvId, l_vBId;

            REQUIRE(std::filesystem::exists("testsFiles/test.nc"));

            l_err = nc_open("testsFiles/test.nc", NC_NOWRITE, &l_ncid);
            l_netCdf->checkNcErr(l_err, __FILE__, __LINE__);

            l_err = nc_inq_dimid(l_ncid, "x", &l_dimXId);
            l_netCdf->checkNcErr(l_err, __FILE__, __LINE__);

            l_err = nc_inq_dimid(l_ncid, "y", &l_dimYId);
            l_netCdf->checkNcErr(l_err, __FILE__, __LINE__);

            l_err = nc_inq_dimid(l_ncid, "time", &l_TimeId);
            l_netCdf->checkNcErr(l_err, __FILE__, __LINE__);

            l_err = nc_inq_dimlen(l_ncid, l_dimXId, &l_nx);
            l_netCdf->checkNcErr(l_err, __FILE__, __LINE__);

            l_err = nc_inq_dimlen(l_ncid, l_dimYId, &l_ny);
            l_netCdf->checkNcErr(l_err, __FILE__, __LINE__);

            l_err = nc_inq_dimlen(l_ncid, l_TimeId, &l_nt);
            l_netCdf->checkNcErr(l_err, __FILE__, __LINE__);

            REQUIRE(l_nx == 5);
            REQUIRE(l_ny == 2);
            REQUIRE(l_nt == 1);

            l_err = nc_inq_varid(l_ncid, "x", &l_vXId);
            l_netCdf->checkNcErr(l_err, __FILE__, __LINE__);
            l_err = nc_inq_varid(l_ncid, "y", &l_vYId);
            l_netCdf->checkNcErr(l_err, __FILE__, __LINE__);
            l_err = nc_inq_varid(l_ncid, "time", &l_vTimeId);
            l_netCdf->checkNcErr(l_err, __FILE__, __LINE__);
            l_err = nc_inq_varid(l_ncid, "h", &l_vHId);
            l_netCdf->checkNcErr(l_err, __FILE__, __LINE__);
            l_err = nc_inq_varid(l_ncid, "hu", &l_vHuId);
            l_netCdf->checkNcErr(l_err, __FILE__, __LINE__);
            l_err = nc_inq_varid(l_ncid, "hv", &l_vHvId);
            l_netCdf->checkNcErr(l_err, __FILE__, __LINE__);
            l_err = nc_inq_varid(l_ncid, "b", &l_vBId);
            l_netCdf->checkNcErr(l_err, __FILE__, __LINE__);

            std::vector<tsunami_lab::t_real> x(5);
            std::vector<tsunami_lab::t_real> y(2);
            std::vector<tsunami_lab::t_real> time(1);
            std::vector<tsunami_lab::t_real> height(10);
            std::vector<tsunami_lab::t_real> momentum_x(10);
            std::vector<tsunami_lab::t_real> momentum_y(10);
            std::vector<tsunami_lab::t_real> bathymetry(10);

            l_err = nc_get_var_float(l_ncid, l_vXId, &x[0]);
            l_netCdf->checkNcErr(l_err, __FILE__, __LINE__);
            l_err = nc_get_var_float(l_ncid, l_vYId, &y[0]);
            l_netCdf->checkNcErr(l_err, __FILE__, __LINE__);
            l_err = nc_get_var_float(l_ncid, l_vTimeId, &time[0]);
            l_netCdf->checkNcErr(l_err, __FILE__, __LINE__);
            l_err = nc_get_var_float(l_ncid, l_vHId, &height[0]);
            l_netCdf->checkNcErr(l_err, __FILE__, __LINE__);
            l_err = nc_get_var_float(l_ncid, l_vHuId, &momentum_x[0]);
            l_netCdf->checkNcErr(l_err, __FILE__, __LINE__);
            l_err = nc_get_var_float(l_ncid, l_vHvId, &momentum_y[0]);
            l_netCdf->checkNcErr(l_err, __FILE__, __LINE__);
            l_err = nc_get_var_float(l_ncid, l_vBId, &bathymetry[0]);
            l_netCdf->checkNcErr(l_err, __FILE__, __LINE__);

            REQUIRE(x[0] == Approx(-49.75f));
            REQUIRE(x[1] == Approx(-49.25f));
            REQUIRE(x[2] == Approx(-48.75f));
            REQUIRE(x[3] == Approx(-48.25f));
            REQUIRE(x[4] == Approx(-47.75f));

            REQUIRE(y[0] == Approx(-49.75f));
            REQUIRE(y[1] == Approx(-49.25f));

            REQUIRE(time[0] == Approx(1));

            REQUIRE(momentum_x[0] == Approx(17));
            REQUIRE(momentum_x[1] == Approx(18));
            REQUIRE(momentum_x[2] == Approx(19));
            REQUIRE(momentum_x[3] == Approx(20));
            REQUIRE(momentum_x[4] == Approx(21));
            REQUIRE(momentum_x[5] == Approx(22));
            REQUIRE(momentum_x[6] == Approx(23));
            REQUIRE(momentum_x[7] == Approx(24));
            REQUIRE(momentum_x[8] == Approx(25));
            REQUIRE(momentum_x[9] == Approx(26));

            REQUIRE(momentum_y[0] == Approx(37));
            REQUIRE(momentum_y[1] == Approx(38));
            REQUIRE(momentum_y[2] == Approx(39));
            REQUIRE(momentum_y[3] == Approx(40));
            REQUIRE(momentum_y[4] == Approx(41));
            REQUIRE(momentum_y[5] == Approx(42));
            REQUIRE(momentum_y[6] == Approx(43));
            REQUIRE(momentum_y[7] == Approx(44));
            REQUIRE(momentum_y[8] == Approx(45));
            REQUIRE(momentum_y[9] == Approx(46));

            REQUIRE(height[0] == Approx(1));
            REQUIRE(height[1] == Approx(2));
            REQUIRE(height[2] == Approx(3));
            REQUIRE(height[3] == Approx(4));
            REQUIRE(height[4] == Approx(5));
            REQUIRE(height[5] == Approx(6));
            REQUIRE(height[6] == Approx(7));
            REQUIRE(height[7] == Approx(8));
            REQUIRE(height[8] == Approx(9));
            REQUIRE(height[9] == Approx(10));

            REQUIRE(bathymetry[0] == Approx(57));
            REQUIRE(bathymetry[1] == Approx(58));
            REQUIRE(bathymetry[2] == Approx(59));
            REQUIRE(bathymetry[3] == Approx(60));
            REQUIRE(bathymetry[4] == Approx(61));
            REQUIRE(bathymetry[5] == Approx(62));
            REQUIRE(bathymetry[6] == Approx(63));
            REQUIRE(bathymetry[7] == Approx(64));
            REQUIRE(bathymetry[8] == Approx(65));
            REQUIRE(bathymetry[9] == Approx(66));

            delete l_netCdf;
            std::string path = "testsFiles/test.nc";
            remove(path.c_str());
        }

        TEST_CASE("Test the NetCdf-reader ", "[NetCdfreader]")
        {

            tsunami_lab::t_real *l_data;
            tsunami_lab::t_idx l_nx;
            tsunami_lab::t_idx l_ny;
            tsunami_lab::io::NetCdf::read("testsFiles/testFileForTheReader.nc", "data", l_nx, l_ny, &l_data, nullptr, nullptr);
            for (tsunami_lab::t_idx i = 0; i < 71; i++)
            {
                REQUIRE(i == l_data[i]);
            }
            REQUIRE(6 == l_nx);
            REQUIRE(12 == l_ny);
            delete[] l_data;
        }

        TEST_CASE("Test the NetCdf-CheckPoint ", "[NetCdfCheckpoint]")
        {

            tsunami_lab::t_real l_h[16] = {0, 0, 0, 0,
                                        0, 1, 2, 0,
                                        0, 3, 4, 0,
                                        0, 0, 0, 0};

            tsunami_lab::t_real l_hu[16] = {0, 0, 0, 0,
                                            0, 5, 8, 0,
                                            0, 6, 7, 0,
                                            0, 0, 0, 0};

            tsunami_lab::t_real l_hv[16] = {0, 0, 0, 0,
                                            0, 2, 2, 0,
                                            0, 2, 2, 0,
                                            0, 0, 0, 0};

            tsunami_lab::t_real l_b[16] = {0, 0, 0, 0,
                                        0, 0, 0, 0,
                                        0, 0, 0, 0,
                                        0, 0, 0, 0};

            tsunami_lab::t_real l_h_read_result[4] = {1, 2,
                                                    3, 4};
            tsunami_lab::t_real l_hu_read_result[4] = {5, 8,
                                                    6, 7};
            tsunami_lab::t_real l_hv_read_result[4] = {2, 2,
                                                    2, 2};

            tsunami_lab::t_real l_b_read_result[4] = {0, 0,
                                                    0, 0};

            tsunami_lab::io::NetCdf *l_netCdf = new tsunami_lab::io::NetCdf(2, 2, 1, "testsFiles/testCheckPoint(the_test_is_in_output_cp_folder).nc");
            REQUIRE(std::filesystem::exists("testsFiles/testCheckPoint(the_test_is_in_output_cp_folder).nc"));

            std::string folder_path = "outputs";
            std::string l_check_point_path = folder_path + "/cp";

            if (!std::filesystem::exists(folder_path))
                std::filesystem::create_directory(folder_path);
            if (!std::filesystem::exists(l_check_point_path))
                std::filesystem::create_directory(l_check_point_path);

            l_netCdf->createCheckPoint("fwave", 1, 2, 3, 4, 5, 6, 7, 10, 3, l_b, l_h, l_hu, l_hv, 1, 4, 2, 2, 1, "test2", "test3", "CheckpointsTest.nc", "test5", "test6");
            REQUIRE(std::filesystem::exists("outputs/cp/CheckpointsTest.nc"));
            std::string solver;
            tsunami_lab::t_real o_domain_start_x;
            tsunami_lab::t_real o_domain_start_y;
            tsunami_lab::t_real o_dimension_x;
            tsunami_lab::t_real o_dimension_y;
            tsunami_lab::t_real o_endtime;
            tsunami_lab::t_real o_simTime;
            tsunami_lab::t_real o_frequency;
            tsunami_lab::t_real o_dt;
            tsunami_lab::t_real o_simulation_time_for_last_cp;
            tsunami_lab::t_real *l_ba; // with ghostcells
            tsunami_lab::t_real *l_ha;
            tsunami_lab::t_real *l_hua;
            tsunami_lab::t_real *l_hva;
            tsunami_lab::t_idx o_time_step_index;
            tsunami_lab::t_idx o_nx;
            tsunami_lab::t_idx o_ny;
            tsunami_lab::t_idx o_k;
            std::string o_setup;
            std::string o_stations_string;
            std::string o_disfile;
            std::string o_batfile;

            tsunami_lab::io::NetCdf::readCheckPoint("outputs/cp/CheckpointsTest.nc",
                                                    &solver,
                                                    &o_domain_start_x,
                                                    &o_domain_start_y,
                                                    &o_dimension_x,
                                                    &o_dimension_y,
                                                    &o_endtime,
                                                    &o_simTime,
                                                    &o_frequency,
                                                    &o_dt,
                                                    &o_simulation_time_for_last_cp,
                                                    &l_ba,
                                                    &l_ha,
                                                    &l_hua,
                                                    &l_hva,
                                                    &o_time_step_index,
                                                    &o_nx,
                                                    &o_ny,
                                                    &o_k,
                                                    &o_setup,
                                                    &o_stations_string,
                                                    &o_disfile,
                                                    &o_batfile);

            REQUIRE(solver == "fwave");
            REQUIRE(o_domain_start_x == 1.0);
            REQUIRE(o_domain_start_y == 2.0);
            REQUIRE(o_dimension_y == 4);
            REQUIRE(o_dimension_x == 3);
            REQUIRE(o_endtime == 5);
            REQUIRE(o_simTime == 6);
            REQUIRE(o_frequency == 7);
            REQUIRE(o_dt == 10);
            REQUIRE(o_simulation_time_for_last_cp == 3);

            REQUIRE(o_time_step_index == 1);
            REQUIRE(o_nx == 2);
            REQUIRE(o_ny == 2);
            REQUIRE(o_k == 1);
            REQUIRE(o_setup == "test2");
            REQUIRE(o_stations_string == "test3");
            REQUIRE(o_disfile == "test6");
            REQUIRE(o_batfile == "test5");

            for (tsunami_lab::t_idx l_i = 0; l_i < 4; l_i++)
            {
                REQUIRE(l_ba[l_i] == l_b_read_result[l_i]);
                REQUIRE(l_ha[l_i] == l_h_read_result[l_i]);
                REQUIRE(l_hua[l_i] == l_hu_read_result[l_i]);
                REQUIRE(l_hva[l_i] == l_hv_read_result[l_i]);
            }

            delete[] l_ba;
            delete[] l_ha;
            delete[] l_hva;
            delete[] l_hua;
            delete l_netCdf;

            std::string path = "outputs/cp/CheckpointsTest.nc";
            remove(path.c_str());
            path = "testsFiles/testCheckPoint(the_test_is_in_output_cp_folder).nc";
            remove(path.c_str());
        }



Before invoking the write and read methods, modifications need to be made to the ``station.cpp`` file.
In this file, a method should be implemented to update the station's CSV, ensuring that when a checkpoint is executed, the data in the checkpoint matches the information in the station CSV file.
To implement the method, we will transform the data from the station CSV file into a structured object in the  ``constant.h`` file.



    .. code-block:: cpp 

            struct DataPoint {
            t_idx x, y;
            t_real water_height, water_hu, water_hv, time_in_seconds;
        };


Now lets implement the method.

.. code-block:: cpp 

    void tsunami_lab::io::Station::updateStation(tsunami_lab::t_real simulation_time, std::string csv_file_path) {
    std::ifstream inFile(csv_file_path);
    if (!inFile) {
        std::cerr << "Error opening file: " << csv_file_path << std::endl;
        return;
    }

    std::vector<DataPoint> dataPoints;
    std::string line;

    while (std::getline(inFile, line)) {
        std::istringstream iss(line);
        DataPoint point;
        char comma;
        if (iss >> point.x >> comma >> point.y >> comma >> point.water_height >> comma
            >> point.water_hu >> comma >> point.water_hv >> comma >> point.time_in_seconds) {
            dataPoints.push_back(point);
        }
        
    }

    inFile.close();


    //tsunami_lab::t_real l_simulation_time = floor(simulation_time); //= std::round(simulation_time * 1)/1;

    auto it = std::find_if(dataPoints.begin(), dataPoints.end(),
        [simulation_time](const DataPoint& point) {
            return floor(point.time_in_seconds) == floor(simulation_time);
        });

    if (it != dataPoints.end()) {
        // Erase elements after the found point
        dataPoints.erase(it + 1, dataPoints.end());

        // Write the modified data back to the file
        std::ofstream outFile(csv_file_path);
        if (!outFile) {
            std::cerr << "Error opening file for writing: " << csv_file_path << std::endl;
            return;
        }

        for (const auto& point : dataPoints) {
            outFile << point.x << ',' << point.y << ',' << point.water_height << ','
                << point.water_hu << ',' << point.water_hv << ',' << point.time_in_seconds << '\n';
        }

        std::cout << "Data after " << simulation_time << " seconds replaced with empty strings." << std::endl;
    }
    else {
        std::cout << "No match found for " << simulation_time << " seconds in the 'time_in_seconds' column." << std::endl;
    }
    }



The method compares the latest simulation time for the station stored in the checkpoint with the time written in the CSV file. Any data saved after this point will be deleted.
Let's now integrate the write and read methods in the main.cpp file. For the read method, we'll introduce a local variable, allowing the user to decide whether toutilize a checkpoint.
Additionally, we need to check if an output file exists for the checkpoint


.. code-block:: cpp 

     bool l_use_cp = tsunami_lab::io::Configuration::readFromConfigBoolean("usecheckpoint");

.. important::

    The checkpoint will be saved in the "outputs/cp" directory. In the JSON file,
    users can customize the names of output files. If two output files
    share the same name, they will overwrite each other, including the checkpoint data.
    For each output, a corresponding checkpoint will be created and overwritten with the new data every 7 timesteps.

.. code-block:: cpp 

    std::string l_temp_outputfile =  "outputs/" + l_temp_outputfilename;
    if(l_use_cp){
    if (!std::filesystem::exists(l_temp_outputfile)) {

      std::cout <<"\033[1;31m\u2717 Cannot use Checkpoint " << "\033[0m"<< std::endl;
      std::cout << "\033[1;31mReason : there is no output file matchs the config file name" <<"\033[0m"<< std::endl;
      return EXIT_FAILURE; 
    }
    //Reading Data from the Checkpoint File
    l_temp_waveprop = "2d";
    l_temp_writer   = "netcdf";

    std::string l_cp_path= "outputs/cp/CheckPoint-" + l_temp_outputfilename;

    std::string l_stations_json_file = "";
    tsunami_lab::io::NetCdf::readCheckPoint(l_cp_path,
                                            &l_temp_solver,
                                            &l_domain_start_x,
                                            &l_domain_start_y,
                                            &l_temp_dimension_x,
                                            &l_temp_dimension_y,
                                            &l_temp_endtime,
                                            &l_simTime,
                                            &l_frequency,
                                            &l_dt,
                                            &l_last_simTime_time,
                                            &l_cp_b,
                                            &l_cp_h,
                                            &l_cp_hu,
                                            &l_cp_hv,
                                            &l_time_step_index,
                                            &l_nx,
                                            &l_ny,
                                            &l_k,
                                            &l_temp_setup,
                                            &l_stations_json_file,
                                            &l_temp_disFile,
                                            &l_temp_bathFile);
    tsunami_lab::io::Configuration::readStationsFromString(l_stations_json_file,l_stations);
    for (const auto& station : l_stations) {
      std::string l_station_path = "stations/" + station.i_name + "/" + station.i_name + ".csv";
      tsunami_lab::io::Station::updateStation(l_last_simTime_time,l_station_path);
    }

And now, let's configure the data we read for our setup.


.. code-block:: cpp 

    if(!l_use_cp){
    for( tsunami_lab::t_idx l_cy = 0; l_cy < l_ny; l_cy++ ){
    tsunami_lab::t_real l_y = l_cy * l_dxy + l_domain_start_y;
    for( tsunami_lab::t_idx l_cx = 0; l_cx < l_nx; l_cx++ ){
        tsunami_lab::t_real l_x = l_cx * l_dxy + l_domain_start_x;

        // get initial values of the setup
        tsunami_lab::t_real l_h = l_setup->getHeight( l_x,
                                                    l_y );
        l_hMax = std::max( l_h, l_hMax );
        tsunami_lab::t_real l_hu = l_setup->getMomentumX( l_x,
                                                        l_y );
        tsunami_lab::t_real l_hv = l_setup->getMomentumY( l_x,
                                                        l_y );
        tsunami_lab::t_real l_bv = l_setup->getBathymetry(l_x,
                                                        l_y );                                       
        // set initial values in wave propagation solver
        l_waveProp->setHeight( l_cx,
                            l_cy,
                            l_h );
        l_waveProp->setMomentumX( l_cx,
                                l_cy,
                                l_hu );
        l_waveProp->setMomentumY( l_cx,
                                l_cy,
                                l_hv );
        l_waveProp->setBathymetry( l_cx,
                                l_cy,
                                l_bv);
    }
    }
    // derive maximum wave speed in setup; the momentum is ignored
    tsunami_lab::t_real l_speedMax = std::sqrt( 9.81 * l_hMax );
    l_dt = 0.50 * l_dxy / l_speedMax;
    }else{
    for( tsunami_lab::t_idx l_cy = 0; l_cy < l_ny; l_cy++ ){

    for( tsunami_lab::t_idx l_cx = 0; l_cx < l_nx; l_cx++ ){ 

        l_waveProp->setHeight(l_cx,
                            l_cy,
                            l_cp_h[l_cx+l_cy*l_nx]);
        l_waveProp->setMomentumX( l_cx,
                                l_cy,
                                l_cp_hu[l_cx+l_cy*l_nx]);
        l_waveProp->setMomentumY( l_cx,
                                l_cy,
                                l_cp_hv[l_cx+l_cy*l_nx]);
        l_waveProp->setBathymetry( l_cx,
                                l_cy,
                                l_cp_b[l_cx+l_cy*l_nx]);
            }
        }    
    }

  
Now, invoke the write method within the `main.cpp` file.
The write function should be called every 7 timesteps within 
the while loop when writing the data.


.. code-block:: cpp

    while(l_simTime < l_temp_endtime){
        .......

      if(l_time_step_index%7 == 0 ){
        std::cout << "\n\033[1;34m" << "Started writing a new Checkpoint ."<< "\033[0m" << std::endl;
        l_netCdf->createCheckPoint(l_temp_solver,
                                    l_domain_start_x,
                                    l_domain_start_y,
                                    l_temp_dimension_x,
                                    l_temp_dimension_y,
                                    l_temp_endtime,
                                    l_simTime,
                                    l_frequency,
                                    l_dt,
                                    l_last_simTime_time,
                                    l_waveProp->getBathymetry(),
                                    l_waveProp->getHeight(),
                                    l_waveProp->getMomentumX(),
                                    l_waveProp->getMomentumY(),
                                    l_time_step_index,
                                    l_waveProp->getStride(),
                                    l_nx,
                                    l_ny,
                                    l_k,
                                    l_temp_setup,
                                    tsunami_lab::io::Station::Stringify(),
                                    l_checkPointName,
                                    l_temp_disFile,
                                    l_temp_bathFile);
        std::cout << "\033[1;32m\u2713 " << "Done writing the Checkpoint ."<< "\033[0m"<< std::endl;
            }
        }
    }


Coarse Output
--------------

As mentioned in the previous section, we added a variable ``k`` to create/readCheckpoint and to our ``fillConstants`` function.

Firstly, we have to adjust the length of our ``x`` and ``y`` variables in the NetCdf class. We have to divide the length of the variables by ``k``.

        .. code-block:: cpp

            tsunami_lab::io::NetCdf::NetCdf(t_real l_nx,t_real l_ny,t_idx l_k,const char*  filename) {
    
            int l_ncId,l_err;
            // Dimensions x, y, time 
            int l_dimXId, l_dimYId, l_dimTimeId;
            int l_dim2Ids[2];
            int l_dim3Ids[3];
            
            
            l_err = nc_create(filename,NC_CLOBBER, &l_ncId);      
            checkNcErr(l_err,__FILE__, __LINE__); //NC_NETCDF4

            //dimensions
            l_err = nc_def_dim(l_ncId, "x", l_nx/ l_k, &l_dimXId);
            checkNcErr(l_err,__FILE__, __LINE__);
            
            l_err = nc_def_dim(l_ncId, "y", l_ny/ l_k, &l_dimYId);
            checkNcErr(l_err,__FILE__, __LINE__);

We added the variable ``k`` to our json file: 
    
            .. code-block:: c++
    
                {
                    "solver" : "fwave",
                    "dimension_x" : 2700000,
                    "dimension_y" : 1500000,
                    "setup" :  "tsunamievent2d",
                    "nx" : 5400,
                    "ny" : 3000,
                    "k"  : 2,
                    "hu" : 0,
                    "location" : 0,
                    "hv":0.0,
                    "hr": 55,
                    "hl": 25,
                    "domain_start_x" : -200000,
                    "domain_start_y" : -750000,
                    "wavepropagation" : "2d",
                    "endtime" : 300,
                    "writer" : "netcdf",
                    "bathfile" : "data/tohoku_gebco20_ucsb3_250m_bath.nc",
                    "disfile" : "data/tohoku_gebco20_ucsb3_250m_displ.nc",
                    "outputfilename": "simulation.nc",
                    "usecheckpoint" : false
                }

 We implemented a function ``makeLowerResGrid`` to create a coarser grid from the fine grid as you can see in the following code snippet.


    .. code-block:: cpp

            void tsunami_lab::io::NetCdf::makeLowerResGrid( t_real const* oldgrid,
                                                    t_idx i_nx,
                                                    t_idx i_ny,
                                                    t_idx i_k,
                                                    t_idx i_stride,
                                                    t_idx i_time_step,
                                                    bool twoDimensionsOnly,
                                                    int m_varId,
                                                    int l_ncId) {

            t_idx result_x = i_nx / i_k;
            t_idx result_y = i_ny / i_k;
            std::vector<t_real> grid(result_x * result_y);

            for (t_idx l_iy = 0; l_iy < result_y; l_iy++) // für y wert neues feld
            {
                for (t_idx l_ix = 0; l_ix < result_x; l_ix++) // für x wert neues feld
                {
                    for (t_idx l_jy = 0; l_jy < i_k; l_jy++) // iterator von 0 bis k um von l_iy and zu zählen
                    {
                        for (t_idx l_jx = 0; l_jx < i_k; l_jx++) // iterator von 0 bis k um von l_ix and zu zählen
                        {  
                            
                            //std::cout <<"index : ["<<(l_iy * i_k + l_jy+1)<<" | "<< (l_ix * i_k + l_jx+1)<<"]     Wert vom oldgrid:  "<<oldgrid[(l_iy * i_k + l_jy+1) * i_stride + (l_ix * i_k + l_jx+1)]<<std::endl;
                            grid[l_iy * result_x + l_ix] += oldgrid[(l_iy * i_k + l_jy+1) * i_stride + (l_ix * i_k + l_jx+1)];
                        }
                    }
                    grid[l_iy * result_x + l_ix] /= (i_k * i_k);
                }
            }

            //std::cout << i_time_step <<" "<< twoDimensionsOnly <<" "<< m_varId<<" "<<l_ncId<< std::endl;
            std::vector<size_t> l_startp;
            std::vector<size_t> l_endp;
            std::vector<ptrdiff_t> l_stridep;
            if(twoDimensionsOnly)
            {
                l_startp     = {0,0};
                l_endp      = {result_y,result_x};
                l_stridep = {1,1};
            } else {
                l_startp     = {i_time_step,0,0};
                l_endp      = {1,result_y,result_x};
                l_stridep = {1,1,1}; 
            }
            int l_err;
            l_err = nc_put_vars_float(l_ncId, m_varId, l_startp.data(), l_endp.data(), l_stridep.data(), grid.data());
            checkNcErr(l_err,__FILE__, __LINE__);
        }


**Better understanding of for loops**
    
    .. important::
        the first for loop is for the y value of the new grid
        the second for loop is for the x value of the new grid
        the third and fourth for loop are for the y and x value of the old grid that adds the values of the old grid to the new grid and divide it by k*k to get the average value of the old grid value

**Test the for loops to see if they work**

In the following code snippet ist a code that you can run and it generates a grid with ghostcells and a coarse grid from the fine grid.
the coarse grid does not have any ghostcells and you can specify any but meaningful values for ``i_nx`` , ``ny`` and ``i_k``.
This code is not part of the tsunami_lab code and is only for testing the for loops.

    .. code-block:: cpp

        #include <iostream>
        #include <iomanip>
        #include <cstdlib>
        #include <ctime>
        #include <cmath>

        int main()
        {
            int i_nx = 9;
            int i_ny = 11;
            int i_k = 3;
            int i_stride =i_nx + 2;

            float *i_b = new float[(i_nx+2)*(i_ny+2)]{};

            // Seed für die Zufallszahlengenerierung initialisieren
            std::srand(static_cast<unsigned>(std::time(nullptr)));

            // Zufällige Werte generieren
            for (int i = 0; i <((i_nx+2)*(i_ny+2)); i++)
            {
                i_b[i] = std::rand() % 10 + 1;
            }

            // Ausgabe des ursprünglichen Arrays
            for (int i = 0; i < (i_ny+2); i++)
            {
                for (int j = 0; j < (i_nx+2); j++)
                {
                    std::cout << std::setw(3) << i_b[i * i_stride + j] << " ";
                }
                std::cout << std::endl;
            }
            std::cout << std::endl;
            std::cout << std::endl;
            std::cout << std::endl;
            std::cout << std::endl;
            std::cout << std::endl;
            std::cout << std::endl;

            // Speicher freigeben

            //int result_x = static_cast<int>(std::ceil(static_cast<double>(i_nx) / i_k));
            //int result_y = static_cast<int>(std::ceil(static_cast<double>(i_ny) / i_k));

            int result_x = i_nx / i_k; 
            int result_y = i_ny / i_k;
            float *l_temp_data_bathymetry = new float[result_x * result_y]{};

            for (int l_iy = 0; l_iy < result_y; l_iy++) // für y wert neues feld
            {
                for (int l_ix = 0; l_ix < result_x; l_ix++) // für x wert neues feld
                {
                    for (int l_jy = 0; l_jy < i_k; l_jy++) // iterator von 0 bis k um von l_iy and zu zählen
                    {
                        for (int l_jx = 0; l_jx < i_k; l_jx++) // iterator von 0 bis k um von l_ix and zu zählen
                        {
                            l_temp_data_bathymetry[l_iy * result_x + l_ix] += i_b[(l_iy * i_k + l_jy+1) * i_stride + (l_ix * i_k + l_jx+1)];
                        }
                    }
                    l_temp_data_bathymetry[l_iy * result_x + l_ix] /= (i_k * i_k);
                }
            }

            // Ausgabe des neuen Arrays mit 4 Zeilen und 5 Spalten
            for (int i = 0; i < result_y; i++)
            {
                for (int j = 0; j < result_x; j++)
                {
                    std::cout << std::setw(6) << l_temp_data_bathymetry[i * result_x + j] << " ";
                }
                std::cout << std::endl;
            }

            delete[] l_temp_data_bathymetry;
            delete[] i_b;

            return 0;
        }
    
Now we change the other functions so that we can use the code we have just implemented
The first one ist our ``fillConstants`` function

    .. code-block:: cpp

            void tsunami_lab::io::NetCdf::fillConstants(t_idx           i_nx,
                                                t_idx                   i_ny,
                                                t_idx                   i_k,
                                                t_idx                   i_stride,
                                                t_real                  i_dxy,
                                                t_real                  i_domainstart_x,
                                                t_real                  i_domainstart_y,
                                                t_real          const * i_b,
                                                const char*           filename){
                                        
            int l_ncId,l_err;
            l_err = nc_open(filename,NC_WRITE, &l_ncId);

            t_idx result_x = i_nx / i_k;
            t_idx result_y = i_ny / i_k;

            t_real *l_coordinateX = new t_real[result_x];
            t_real *l_coordinateY = new t_real[result_y];
            
            for( t_idx l_iy = 0; l_iy < result_y; l_iy++ )
            {
                l_coordinateY[l_iy] = ((l_iy + 0.5) * i_dxy * i_k)+ i_domainstart_y;
            }
            // put y coordinates
            l_err = nc_put_var_float(l_ncId, m_varIdY, l_coordinateY);
            checkNcErr(l_err,__FILE__, __LINE__);

            delete[] l_coordinateY;
            for(t_idx l_ix = 0; l_ix < result_x; l_ix++) 
            {
                l_coordinateX[l_ix] = ((l_ix + 0.5) * i_dxy * i_k)+ i_domainstart_x;
            }
            // put x coordinates
            l_err = nc_put_var_float(l_ncId, m_varIdX, l_coordinateX);
            checkNcErr(l_err,__FILE__, __LINE__);
            delete[] l_coordinateX;

            // put bathymetry values
            makeLowerResGrid(i_b, i_nx, i_ny, i_k, i_stride,1,true, m_varIdBathymetry, l_ncId);

            l_err = nc_close(l_ncId);
            checkNcErr(l_err,__FILE__, __LINE__);       
        }


The second one is our ``write`` function

    .. code-block:: cpp

            void tsunami_lab::io::NetCdf::updateFile(t_idx       i_nx,
                                            t_idx                i_ny,
                                            t_idx                i_stride,
                                            t_idx                i_time_step,
                                            t_idx                i_k,
                                            t_real               i_time,
                                            t_real       const * i_h,
                                            t_real       const * i_hu,
                                            t_real       const * i_hv,
                                            const char*          filename){
        
            int l_ncId, l_err;
            l_err = nc_open(filename,NC_WRITE,&l_ncId); 
            checkNcErr(l_err,__FILE__, __LINE__);

            makeLowerResGrid( i_h, i_nx, i_ny, i_k, i_stride, i_time_step,false, m_varIdHeight, l_ncId);
            makeLowerResGrid(i_hu, i_nx, i_ny, i_k, i_stride, i_time_step,false, m_varIdImpulseX, l_ncId);
            makeLowerResGrid(i_hv, i_nx, i_ny, i_k, i_stride, i_time_step,false, m_varIdImpulseY, l_ncId);

            //time step is how many timesteps there are and i_time what simtime it is
            l_err = nc_put_var1_float(l_ncId, m_varIdTime, &i_time_step, &i_time);
            checkNcErr(l_err,__FILE__, __LINE__);

            l_err = nc_close(l_ncId);
            checkNcErr(l_err,__FILE__, __LINE__);
        }



**Visualize the data**

The first video shows chile with a ``cell width = 500m`` and ``k = 5``.

    .. video:: _static/500_k_5_chile.mp4
        :width: 700
        :height: 500
        :autoplay:

The second video shows our Dambreak2D setup with ``k = 2``.
 You can see that the cells at the edge of the dam have a lower height than the cells in the middle.

    .. video:: _static/dambreak2d_k_2.mp4
        :width: 700
        :height: 500
        :autoplay:



Personal Contribution
---------------------

- Ward Tammaa, Daniel Schicker Doxygen Documentation
- Mohamad Khaled Minawe, Ward Tammaa, Daniel Schicker Sphnix Documentation
- Daniel Schicker, Mohamad Khaled Minawe , Ward Tammaa functions implementation
- Mohamad Khaled Minawe, Daniel Schicker, Ward Tammaa Unit Testing
- Mohamad Khaled Minawe, Daniel Schicker Geogebra Datei(Calculations for the Unit Tests)
- Ward Tammaa Hosting the code , Action runner


    