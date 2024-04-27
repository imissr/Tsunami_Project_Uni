
#include <catch2/catch.hpp>
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

    

    l_netCdf->createCheckPoint("fwave", 1, 2, 3, 4, 5, 6, 7, 10, 3, l_b, l_h, l_hu, l_hv, 1, 4, 2, 2, 1, "test2", "test3", "CheckpointsTest.nc", "test5", "test6",true);
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
    bool o_reflecting_boundary;

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
                                            &o_batfile,
                                            &o_reflecting_boundary);

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
    REQUIRE(o_reflecting_boundary == true);

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