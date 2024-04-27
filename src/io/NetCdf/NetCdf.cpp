#include "NetCdf.h"
#include <netcdf.h>
#include <iostream>
#include <ctime>
#include <iomanip>
#include <vector>
#include <omp.h>
#include <cmath>

void tsunami_lab::io::NetCdf::fillConstants(t_idx i_nx,
                                            t_idx i_ny,
                                            t_idx i_k,
                                            t_idx i_stride,
                                            t_real i_dxy,
                                            t_real i_domainstart_x,
                                            t_real i_domainstart_y,
                                            t_real const *i_b,
                                            const char *filename)
{

    int l_ncId, l_err;
    l_err = nc_open(filename, NC_WRITE, &l_ncId);

    std::vector<t_real> l_coordinateX(i_nx / i_k);
    std::vector<t_real> l_coordinateY(i_ny / i_k);

    for (t_idx l_iy = 0; l_iy < (i_ny / i_k); l_iy++)
    {
        l_coordinateY[l_iy] = ((l_iy + 0.5) * i_dxy * i_k) + i_domainstart_y;
    }
    // put y coordinates
    l_err = nc_put_var_float(l_ncId, m_varIdY, l_coordinateY.data());
    checkNcErr(l_err, __FILE__, __LINE__);

    for (t_idx l_ix = 0; l_ix < (i_nx / i_k); l_ix++)
    {
        l_coordinateX[l_ix] = ((l_ix + 0.5) * i_dxy * i_k) + i_domainstart_x;
    }
    // put x coordinates
    l_err = nc_put_var_float(l_ncId, m_varIdX, l_coordinateX.data());
    checkNcErr(l_err, __FILE__, __LINE__);

    // put bathymetry values
    makeLowerResGrid(i_b, i_nx, i_ny, i_k, i_stride, 1, true, m_varIdBathymetry, l_ncId);

    l_err = nc_close(l_ncId);
    checkNcErr(l_err, __FILE__, __LINE__);
}

void tsunami_lab::io::NetCdf::read(const char *i_filename,
                                   const char *i_varname,
                                   t_idx &i_nx,
                                   t_idx &i_ny,
                                   t_real **o_zdata,
                                   t_real **o_xdata,
                                   t_real **o_ydata)
{
    if (std::filesystem::exists(i_filename))
    {

        int l_ncId, l_err = 0;
        int l_varidz, l_varidy, l_varidx, l_dimX, l_dimY;

        l_err = nc_open(i_filename, NC_NOWRITE, &l_ncId);
        checkNcErr(l_err, __FILE__, __LINE__);

        l_err = nc_inq_dimid(l_ncId, "x", &l_dimX);
        checkNcErr(l_err, __FILE__, __LINE__);

        l_err = nc_inq_dimlen(l_ncId, l_dimX, &i_nx);
        checkNcErr(l_err, __FILE__, __LINE__);

        l_err = nc_inq_dimid(l_ncId, "y", &l_dimY);
        checkNcErr(l_err, __FILE__, __LINE__);

        l_err = nc_inq_dimlen(l_ncId, l_dimY, &i_ny);
        checkNcErr(l_err, __FILE__, __LINE__);

        if (o_xdata != nullptr)
        {
            nc_inq_varid(l_ncId, "x", &l_varidx);
            *o_xdata = new t_real[i_nx];
            l_err = nc_get_var_float(l_ncId, l_varidx, *o_xdata);
            checkNcErr(l_err, __FILE__, __LINE__);
        }

        if (o_ydata != nullptr)
        {
            nc_inq_varid(l_ncId, "y", &l_varidy);
            *o_ydata = new t_real[i_ny];
            l_err = nc_get_var_float(l_ncId, l_varidy, *o_ydata);
            checkNcErr(l_err, __FILE__, __LINE__);
        }

        if (o_zdata != nullptr)
        {
            nc_inq_varid(l_ncId, i_varname, &l_varidz);
            *o_zdata = new t_real[i_ny * i_nx];
            l_err = nc_get_var_float(l_ncId, l_varidz, *o_zdata);
            checkNcErr(l_err, __FILE__, __LINE__);
        }

        if (nc_close(l_ncId) != NC_NOERR)
        {
            std::cerr << "Error closing NetCDF file: " << i_filename << std::endl;
        }
    }
    else
    {
        if (std::getenv("GITHUB_ACTIONS") == nullptr)
        {
            throw std::runtime_error(std::string("File does not exist: ") + i_filename);
        }
        else
        {
            std::cerr << "Warning: File does not exist: " << i_filename << '\n';
        }
    }
}

void tsunami_lab::io::NetCdf::updateFile(t_idx i_nx,
                                         t_idx i_ny,
                                         t_idx i_stride,
                                         t_idx i_time_step,
                                         t_idx i_k,
                                         t_real i_time,
                                         t_real const *i_h,
                                         t_real const *i_hu,
                                         t_real const *i_hv,
                                         const char *filename)
{

    int l_ncId, l_err;
    l_err = nc_open(filename, NC_WRITE, &l_ncId);
    checkNcErr(l_err, __FILE__, __LINE__);

    makeLowerResGrid(i_h, i_nx, i_ny, i_k, i_stride, i_time_step, false, m_varIdHeight, l_ncId);
    makeLowerResGrid(i_hu, i_nx, i_ny, i_k, i_stride, i_time_step, false, m_varIdImpulseX, l_ncId);
    makeLowerResGrid(i_hv, i_nx, i_ny, i_k, i_stride, i_time_step, false, m_varIdImpulseY, l_ncId);

    // time step is how many timesteps there are and i_time what simtime it is
    l_err = nc_put_var1_float(l_ncId, m_varIdTime, &i_time_step, &i_time);
    checkNcErr(l_err, __FILE__, __LINE__);

    l_err = nc_close(l_ncId);
    checkNcErr(l_err, __FILE__, __LINE__);
}

void tsunami_lab::io::NetCdf::makeLowerResGrid(t_real const *oldgrid,
                                               t_idx i_nx,
                                               t_idx i_ny,
                                               t_idx i_k,
                                               t_idx i_stride,
                                               t_idx i_time_step,
                                               bool twoDimensionsOnly,
                                               int m_varId,
                                               int l_ncId)
{
    if (i_k != 1)
    {
        t_idx result_x = i_nx / i_k;
        t_idx result_y = i_ny / i_k;
        std::vector<t_real> grid(result_x * result_y, 0.0f);

#pragma omp parallel for collapse(2) default(none) shared(grid, oldgrid) firstprivate(result_x, result_y, i_stride, i_k)
        for (t_idx l_iy = 0; l_iy < result_y; l_iy++) // für y wert neues feld
        {
            for (t_idx l_ix = 0; l_ix < result_x; l_ix++) // für x wert neues feld
            {
                for (t_idx l_jy = 0; l_jy < i_k; l_jy++) // iterator von 0 bis k um von l_iy and zu zählen
                {
                    for (t_idx l_jx = 0; l_jx < i_k; l_jx++) // iterator von 0 bis k um von l_ix and zu zählen
                    {
                        grid[l_iy * result_x + l_ix] += oldgrid[(l_iy * i_k + l_jy + 1) * i_stride + (l_ix * i_k + l_jx + 1)];
                    }
                }
                grid[l_iy * result_x + l_ix] /= (i_k * i_k);
            }
        }

        std::vector<size_t> l_startp;
        std::vector<size_t> l_endp;
        std::vector<ptrdiff_t> l_stridep;
        if (twoDimensionsOnly)
        {
            l_startp = {0, 0};
            l_endp = {result_y, result_x};
            l_stridep = {1, 1};
        }
        else
        {
            l_startp = {i_time_step, 0, 0};
            l_endp = {1, result_y, result_x};
            l_stridep = {1, 1, 1};
        }
        int l_err;
        l_err = nc_put_vars_float(l_ncId, m_varId, l_startp.data(), l_endp.data(), l_stridep.data(), grid.data());
        checkNcErr(l_err, __FILE__, __LINE__);
    }
    else // if k == 1 then just put the old grid
    {
        std::vector<t_real> grid(i_nx * i_ny);
#pragma omp parallel for collapse(2) default(none) shared(grid, oldgrid) firstprivate(i_stride, i_nx, i_ny)
        for (t_idx l_iy = 0; l_iy < i_ny; l_iy++)
        {
            for (t_idx l_ix = 0; l_ix < i_nx; l_ix++)
            {
                grid[l_iy * i_nx + l_ix] = oldgrid[(l_iy + 1) * i_stride + (l_ix + 1)];
            }
        }
        std::vector<size_t> l_startp;
        std::vector<size_t> l_endp;
        std::vector<ptrdiff_t> l_stridep;
        if (twoDimensionsOnly)
        {
            l_startp = {0, 0};
            l_endp = {i_ny, i_nx};
            l_stridep = {1, 1};
        }
        else
        {
            l_startp = {i_time_step, 0, 0};
            l_endp = {1, i_ny, i_nx};
            l_stridep = {1, 1, 1};
        }
        int l_err;
        l_err = nc_put_vars_float(l_ncId, m_varId, l_startp.data(), l_endp.data(), l_stridep.data(), grid.data());
        checkNcErr(l_err, __FILE__, __LINE__);
    }
}

tsunami_lab::io::NetCdf::NetCdf(t_real l_nx, t_real l_ny, t_idx l_k, const char *filename)
{

    int l_ncId, l_err;
    // Dimensions x, y, time
    int l_dimXId, l_dimYId, l_dimTimeId;
    int l_dim2Ids[2];
    int l_dim3Ids[3];

    l_err = nc_create(filename, NC_CLOBBER, &l_ncId);
    checkNcErr(l_err, __FILE__, __LINE__); // NC_NETCDF4

    // dimensions
    l_err = nc_def_dim(l_ncId, "x", l_nx / l_k, &l_dimXId);
    checkNcErr(l_err, __FILE__, __LINE__);

    l_err = nc_def_dim(l_ncId, "y", l_ny / l_k, &l_dimYId);
    checkNcErr(l_err, __FILE__, __LINE__);

    l_err = nc_def_dim(l_ncId, "time", NC_UNLIMITED, &l_dimTimeId);
    checkNcErr(l_err, __FILE__, __LINE__);

    // variable x
    l_err = nc_def_var(l_ncId, "x", NC_FLOAT, 1, &l_dimXId, &m_varIdX);
    checkNcErr(l_err, __FILE__, __LINE__);
    const char *units_attribute_x = "meter";
    nc_put_att_text(l_ncId, m_varIdX, "units", strlen(units_attribute_x), units_attribute_x);

    // variable y
    l_err = nc_def_var(l_ncId, "y", NC_FLOAT, 1, &l_dimYId, &m_varIdY);
    checkNcErr(l_err, __FILE__, __LINE__);
    const char *units_attribute_y = "meter";
    nc_put_att_text(l_ncId, m_varIdY, "units", strlen(units_attribute_y), units_attribute_y);

    // variable time
    l_err = nc_def_var(l_ncId, "time", NC_FLOAT, 1, &l_dimTimeId, &m_varIdTime);
    checkNcErr(l_err, __FILE__, __LINE__);
    const char *units_attribute_time = "seconds";
    nc_put_att_text(l_ncId, m_varIdTime, "units", strlen(units_attribute_time), units_attribute_time);

    // variable bathymetry
    l_dim2Ids[0] = l_dimYId;
    l_dim2Ids[1] = l_dimXId;

    l_err = nc_def_var(l_ncId, "b", NC_FLOAT, 2, l_dim2Ids, &m_varIdBathymetry);
    checkNcErr(l_err, __FILE__, __LINE__);
    const char *units_attribute_Bathymetry = "meters";
    nc_put_att_text(l_ncId, m_varIdBathymetry, "units", strlen(units_attribute_Bathymetry), units_attribute_Bathymetry);

    // variable height

    l_dim3Ids[0] = l_dimTimeId;
    l_dim3Ids[1] = l_dimYId;
    l_dim3Ids[2] = l_dimXId;

    l_err = nc_def_var(l_ncId, "h", NC_FLOAT, 3, l_dim3Ids, &m_varIdHeight);
    checkNcErr(l_err, __FILE__, __LINE__);
    const char *units_attribute_height = "meters";
    nc_put_att_text(l_ncId, m_varIdHeight, "units", strlen(units_attribute_height), units_attribute_height);

    // variable momentum in x direction
    l_err = nc_def_var(l_ncId, "hu", NC_FLOAT, 3, l_dim3Ids, &m_varIdImpulseX);
    checkNcErr(l_err, __FILE__, __LINE__);
    const char *units_attribute_impulseX = "meters";
    nc_put_att_text(l_ncId, m_varIdImpulseX, "units", strlen(units_attribute_impulseX), units_attribute_impulseX);

    // variable momentum in y direction

    l_err = nc_def_var(l_ncId, "hv", NC_FLOAT, 3, l_dim3Ids, &m_varIdImpulseY);
    checkNcErr(l_err, __FILE__, __LINE__);
    const char *units_attribute_impulseY = "meters";
    nc_put_att_text(l_ncId, m_varIdImpulseY, "units", strlen(units_attribute_impulseY), units_attribute_impulseY);

    // enddef
    l_err = nc_enddef(l_ncId);
    checkNcErr(l_err, __FILE__, __LINE__);
}

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
                                               t_idx i_k,
                                               std::string i_setup,
                                               std::string i_stations_string,
                                               std::string i_name_cp,
                                               std::string i_disfile,
                                               std::string i_batfile,
                                               bool i_reflecting_boundary)
{

    // folder_path = "outputs/cp"
    std::string l_name_cp = "outputs/cp/" + i_name_cp;
    int l_err, l_ncId;
    l_err = nc_create(l_name_cp.data(), NC_NETCDF4, &l_ncId);
    checkNcErr(l_err, __FILE__, __LINE__);

    int l_dimXId, l_dimYId;
    l_err = nc_def_dim(l_ncId, "x", i_nx, &l_dimXId);
    checkNcErr(l_err, __FILE__, __LINE__);
    l_err = nc_def_dim(l_ncId, "y", i_ny, &l_dimYId);
    checkNcErr(l_err, __FILE__, __LINE__);

    int l_var_domain_start_x_id, l_var_domain_start_y_id, l_var_solver_id, l_var_dimension_x_id,
        l_var_dimension_y_id, l_var_endtime_id, l_var_simTime_id, l_var_frequency_id, l_var_b_id,
        l_var_h_id, l_var_hu_id, l_var_hv_id, l_var_dt_id, l_var_disfile_id, l_var_batfile_id, l_var_reflecting_boundary_id,
        l_var_time_step_index_id, l_var_stations_string, l_var_setup, l_simulation_time_for_last_cp_id, l_var_k_Id;
    //-----------------------------------------------------Define Variables

    l_err = nc_def_var(l_ncId, "reflecting_boundary", NC_INT, 0, nullptr, &l_var_reflecting_boundary_id);
    checkNcErr(l_err, __FILE__, __LINE__);
    l_err = nc_def_var(l_ncId, "domain_start_x", NC_FLOAT, 0, nullptr, &l_var_domain_start_x_id);
    checkNcErr(l_err, __FILE__, __LINE__);
    l_err = nc_def_var(l_ncId, "domain_start_y", NC_FLOAT, 0, nullptr, &l_var_domain_start_y_id);
    checkNcErr(l_err, __FILE__, __LINE__);
    l_err = nc_def_var(l_ncId, "solver", NC_INT, 0, nullptr, &l_var_solver_id);
    checkNcErr(l_err, __FILE__, __LINE__);
    l_err = nc_def_var(l_ncId, "dimension_x", NC_FLOAT, 0, nullptr, &l_var_dimension_x_id);
    checkNcErr(l_err, __FILE__, __LINE__);
    l_err = nc_def_var(l_ncId, "dimension_y", NC_FLOAT, 0, nullptr, &l_var_dimension_y_id);
    checkNcErr(l_err, __FILE__, __LINE__);

    l_err = nc_def_var(l_ncId, "simulationtimeforlastcp", NC_FLOAT, 0, nullptr, &l_simulation_time_for_last_cp_id);
    checkNcErr(l_err, __FILE__, __LINE__);

    l_err = nc_def_var(l_ncId, "endtime", NC_FLOAT, 0, nullptr, &l_var_endtime_id);
    checkNcErr(l_err, __FILE__, __LINE__);
    l_err = nc_def_var(l_ncId, "simTime", NC_FLOAT, 0, nullptr, &l_var_simTime_id);
    checkNcErr(l_err, __FILE__, __LINE__);
    l_err = nc_def_var(l_ncId, "setup", NC_STRING, 0, nullptr, &l_var_setup);
    checkNcErr(l_err, __FILE__, __LINE__);
    l_err = nc_def_var(l_ncId, "stations", NC_STRING, 0, nullptr, &l_var_stations_string);
    checkNcErr(l_err, __FILE__, __LINE__);

    l_err = nc_def_var(l_ncId, "k", NC_INT, 0, nullptr, &l_var_k_Id);
    checkNcErr(l_err, __FILE__, __LINE__);
    l_err = nc_def_var(l_ncId, "timeStep", NC_INT, 0, nullptr, &l_var_time_step_index_id);
    checkNcErr(l_err, __FILE__, __LINE__);
    l_err = nc_def_var(l_ncId, "frequency", NC_FLOAT, 0, nullptr, &l_var_frequency_id);
    checkNcErr(l_err, __FILE__, __LINE__);
    l_err = nc_def_var(l_ncId, "dt", NC_FLOAT, 0, nullptr, &l_var_dt_id);
    checkNcErr(l_err, __FILE__, __LINE__);

    l_err = nc_def_var(l_ncId, "disfile", NC_STRING, 0, nullptr, &l_var_disfile_id);
    checkNcErr(l_err, __FILE__, __LINE__);
    l_err = nc_def_var(l_ncId, "batfile", NC_STRING, 0, nullptr, &l_var_batfile_id);
    checkNcErr(l_err, __FILE__, __LINE__);

    int l_dims[2] = {l_dimYId, l_dimXId};

    l_err = nc_def_var(l_ncId, "b", NC_FLOAT, 2, l_dims, &l_var_b_id);
    checkNcErr(l_err, __FILE__, __LINE__);

    l_err = nc_def_var(l_ncId, "hu", NC_FLOAT, 2, l_dims, &l_var_hu_id);
    checkNcErr(l_err, __FILE__, __LINE__);

    l_err = nc_def_var(l_ncId, "h", NC_FLOAT, 2, l_dims, &l_var_h_id);
    checkNcErr(l_err, __FILE__, __LINE__);

    l_err = nc_def_var(l_ncId, "hv", NC_FLOAT, 2, l_dims, &l_var_hv_id);
    checkNcErr(l_err, __FILE__, __LINE__);

    l_err = nc_enddef(l_ncId);
    checkNcErr(l_err, __FILE__, __LINE__);

    //-----------------------------------------------------Write Variables
    const char *setupData = i_setup.data();
    l_err = nc_put_var_string(l_ncId, l_var_setup, &setupData);
    checkNcErr(l_err, __FILE__, __LINE__);

    const char *l_stations_string = i_stations_string.data();
    const char *l_disfile_string = i_disfile.data();
    const char *l_batfile_string = i_batfile.data();

    l_err = nc_put_var_string(l_ncId, l_var_disfile_id, &l_batfile_string);
    checkNcErr(l_err, __FILE__, __LINE__);

    l_err = nc_put_var_string(l_ncId, l_var_batfile_id, &l_disfile_string);
    checkNcErr(l_err, __FILE__, __LINE__);

    l_err = nc_put_var_string(l_ncId, l_var_stations_string, &l_stations_string);
    checkNcErr(l_err, __FILE__, __LINE__);

    l_err = nc_put_var_float(l_ncId, l_var_domain_start_x_id, &i_domain_start_x);
    checkNcErr(l_err, __FILE__, __LINE__);

    l_err = nc_put_var_float(l_ncId, l_simulation_time_for_last_cp_id, &i_simulation_time_for_last_cp);
    checkNcErr(l_err, __FILE__, __LINE__);

    l_err = nc_put_var_float(l_ncId, l_var_domain_start_y_id, &i_domain_start_y);
    checkNcErr(l_err, __FILE__, __LINE__);

    l_err = nc_put_var_float(l_ncId, l_var_dimension_x_id, &i_dimension_x);
    checkNcErr(l_err, __FILE__, __LINE__);

    l_err = nc_put_var_float(l_ncId, l_var_dimension_y_id, &i_dimension_y);
    checkNcErr(l_err, __FILE__, __LINE__);

    l_err = nc_put_var_float(l_ncId, l_var_endtime_id, &i_endtime);
    checkNcErr(l_err, __FILE__, __LINE__);

    l_err = nc_put_var_float(l_ncId, l_var_simTime_id, &i_simTime);
    checkNcErr(l_err, __FILE__, __LINE__);

    l_err = nc_put_var_float(l_ncId, l_var_frequency_id, &i_frequency);
    checkNcErr(l_err, __FILE__, __LINE__);

    l_err = nc_put_var_float(l_ncId, l_var_dt_id, &i_dt);
    checkNcErr(l_err, __FILE__, __LINE__);

    l_err = nc_put_var_int(l_ncId, l_var_time_step_index_id, (const int *)&i_time_step_index);
    checkNcErr(l_err, __FILE__, __LINE__);

    l_err = nc_put_var_int(l_ncId, l_var_k_Id, (const int *)&i_k);
    checkNcErr(l_err, __FILE__, __LINE__);

    int l_reflecting_boundary = 0;
    if (i_reflecting_boundary)
    {
        l_reflecting_boundary = 1;
    }

    l_err = nc_put_var_int(l_ncId, l_var_reflecting_boundary_id, &l_reflecting_boundary);
    checkNcErr(l_err, __FILE__, __LINE__);

    int l_solver = 0;
    if (i_solver == "roe")
        l_solver = 1;

    l_err = nc_put_var_int(l_ncId, l_var_solver_id, &l_solver);
    checkNcErr(l_err, __FILE__, __LINE__);

    std::vector<size_t> l_start = {0, 0};
    std::vector<size_t> l_end = {1, i_nx};
    std::vector<ptrdiff_t> l_strid = {1, 1};

    for (l_start[0] = 0; l_start[0] < i_ny; l_start[0]++)
    {
        l_err = nc_put_vars_float(l_ncId, l_var_b_id, l_start.data(), l_end.data(), l_strid.data(), (i_b + (l_start[0] + 1) * i_stride + 1));
        checkNcErr(l_err, __FILE__, __LINE__);
        l_err = nc_put_vars_float(l_ncId, l_var_h_id, l_start.data(), l_end.data(), l_strid.data(), (i_h + (l_start[0] + 1) * i_stride + 1));
        checkNcErr(l_err, __FILE__, __LINE__);
        l_err = nc_put_vars_float(l_ncId, l_var_hu_id, l_start.data(), l_end.data(), l_strid.data(), (i_hu + (l_start[0] + 1) * i_stride + 1));
        checkNcErr(l_err, __FILE__, __LINE__);
        l_err = nc_put_vars_float(l_ncId, l_var_hv_id, l_start.data(), l_end.data(), l_strid.data(), (i_hv + (l_start[0] + 1) * i_stride + 1));
        checkNcErr(l_err, __FILE__, __LINE__);
    }

    l_err = nc_close(l_ncId);
    checkNcErr(l_err, __FILE__, __LINE__);
}

void tsunami_lab::io::NetCdf::readCheckPoint(std::string i_path_cp,
                                             std::string *o_solver,
                                             t_real *o_domain_start_x,
                                             t_real *o_domain_start_y,
                                             t_real *o_dimension_x,
                                             t_real *o_dimension_y,
                                             t_real *o_endtime,
                                             t_real *o_simTime,
                                             t_real *o_frequency,
                                             t_real *o_dt,
                                             t_real *o_simulation_time_for_last_cp,
                                             t_real **o_b,
                                             t_real **o_h,
                                             t_real **o_hu,
                                             t_real **o_hv,
                                             t_idx *o_time_step_index,
                                             t_idx *o_nx,
                                             t_idx *o_ny,
                                             t_idx *o_k,
                                             std::string *o_setup,
                                             std::string *o_stations_string,
                                             std::string *o_disfile,
                                             std::string *o_batfile,
                                             bool *o_reflecting_boundary)
{

    int l_ncId = 0, l_err = 0;
    l_err = nc_open(i_path_cp.c_str(), NC_NOWRITE, &l_ncId);
    checkNcErr(l_err, __FILE__, __LINE__);

    int l_dimXId, l_dimYId;
    l_err = nc_inq_dimid(l_ncId, "x", &l_dimXId);
    checkNcErr(l_err, __FILE__, __LINE__);
    l_err = nc_inq_dimid(l_ncId, "y", &l_dimYId);
    checkNcErr(l_err, __FILE__, __LINE__);

    l_err = nc_inq_dimlen(l_ncId, l_dimXId, o_nx);
    checkNcErr(l_err, __FILE__, __LINE__);

    l_err = nc_inq_dimlen(l_ncId, l_dimYId, o_ny);
    checkNcErr(l_err, __FILE__, __LINE__);

    int l_var_domain_start_x_id, l_var_domain_start_y_id, l_var_solver_id, l_var_dimension_x_id,
        l_var_dimension_y_id, l_var_endtime_id, l_var_simTime_id, l_var_frequency_id, l_var_b_id,
        l_var_h_id, l_var_hu_id, l_var_hv_id, l_var_dt_id, l_var_disfile_id, l_var_batfile_id, l_var_reflecting_boundary_id,
        l_var_time_step_index_id, l_var_stations_string, l_var_setup, l_simulation_time_for_last_cp_id, l_var_k_id;

    l_err = nc_inq_varid(l_ncId, "domain_start_x", &l_var_domain_start_x_id);
    checkNcErr(l_err, __FILE__, __LINE__);

    l_err = nc_inq_varid(l_ncId, "domain_start_y", &l_var_domain_start_y_id);
    checkNcErr(l_err, __FILE__, __LINE__);

    l_err = nc_inq_varid(l_ncId, "solver", &l_var_solver_id);
    checkNcErr(l_err, __FILE__, __LINE__);

    l_err = nc_inq_varid(l_ncId, "simulationtimeforlastcp", &l_simulation_time_for_last_cp_id);
    checkNcErr(l_err, __FILE__, __LINE__);

    l_err = nc_inq_varid(l_ncId, "dimension_x", &l_var_dimension_x_id);
    checkNcErr(l_err, __FILE__, __LINE__);

    l_err = nc_inq_varid(l_ncId, "dimension_y", &l_var_dimension_y_id);
    checkNcErr(l_err, __FILE__, __LINE__);

    l_err = nc_inq_varid(l_ncId, "endtime", &l_var_endtime_id);
    checkNcErr(l_err, __FILE__, __LINE__);

    l_err = nc_inq_varid(l_ncId, "simTime", &l_var_simTime_id);
    checkNcErr(l_err, __FILE__, __LINE__);

    l_err = nc_inq_varid(l_ncId, "setup", &l_var_setup);
    checkNcErr(l_err, __FILE__, __LINE__);

    l_err = nc_inq_varid(l_ncId, "stations", &l_var_stations_string);
    checkNcErr(l_err, __FILE__, __LINE__);

    l_err = nc_inq_varid(l_ncId, "timeStep", &l_var_time_step_index_id);
    checkNcErr(l_err, __FILE__, __LINE__);

    l_err = nc_inq_varid(l_ncId, "k", &l_var_k_id);
    checkNcErr(l_err, __FILE__, __LINE__);

    l_err = nc_inq_varid(l_ncId, "frequency", &l_var_frequency_id);
    checkNcErr(l_err, __FILE__, __LINE__);

    l_err = nc_inq_varid(l_ncId, "dt", &l_var_dt_id);
    checkNcErr(l_err, __FILE__, __LINE__);

    l_err = nc_inq_varid(l_ncId, "disfile", &l_var_disfile_id);
    checkNcErr(l_err, __FILE__, __LINE__);

    l_err = nc_inq_varid(l_ncId, "batfile", &l_var_batfile_id);
    checkNcErr(l_err, __FILE__, __LINE__);

    l_err = nc_inq_varid(l_ncId, "reflecting_boundary", &l_var_reflecting_boundary_id);
    checkNcErr(l_err, __FILE__, __LINE__);

    l_err = nc_inq_varid(l_ncId, "b", &l_var_b_id);
    checkNcErr(l_err, __FILE__, __LINE__);

    l_err = nc_inq_varid(l_ncId, "hu", &l_var_hu_id);
    checkNcErr(l_err, __FILE__, __LINE__);

    l_err = nc_inq_varid(l_ncId, "h", &l_var_h_id);
    checkNcErr(l_err, __FILE__, __LINE__);

    l_err = nc_inq_varid(l_ncId, "hv", &l_var_hv_id);
    checkNcErr(l_err, __FILE__, __LINE__);

    // GET THE VARIABLES

    char *l_setup;
    l_err = nc_get_var_string(l_ncId, l_var_setup, &l_setup);
    *o_setup = std::string(l_setup);
    checkNcErr(l_err, __FILE__, __LINE__);

    char *l_batfile;
    l_err = nc_get_var_string(l_ncId, l_var_batfile_id, &l_batfile);
    *o_batfile = std::string(l_batfile);
    checkNcErr(l_err, __FILE__, __LINE__);

    char *l_disfile;
    l_err = nc_get_var_string(l_ncId, l_var_disfile_id, &l_disfile);
    *o_disfile = std::string(l_disfile);
    checkNcErr(l_err, __FILE__, __LINE__);

    char *l_stations_string;
    l_err = nc_get_var_string(l_ncId, l_var_stations_string, &l_stations_string);
    *o_stations_string = std::string(l_stations_string);
    checkNcErr(l_err, __FILE__, __LINE__);

    l_err = nc_get_var_float(l_ncId, l_simulation_time_for_last_cp_id, o_simulation_time_for_last_cp);
    checkNcErr(l_err, __FILE__, __LINE__);

    l_err = nc_get_var_float(l_ncId, l_var_domain_start_x_id, o_domain_start_x);
    checkNcErr(l_err, __FILE__, __LINE__);

    l_err = nc_get_var_float(l_ncId, l_var_domain_start_y_id, o_domain_start_y);
    checkNcErr(l_err, __FILE__, __LINE__);

    l_err = nc_get_var_float(l_ncId, l_var_dimension_x_id, o_dimension_x);
    checkNcErr(l_err, __FILE__, __LINE__);

    l_err = nc_get_var_float(l_ncId, l_var_dimension_y_id, o_dimension_y);
    checkNcErr(l_err, __FILE__, __LINE__);

    l_err = nc_get_var_float(l_ncId, l_var_endtime_id, o_endtime);
    checkNcErr(l_err, __FILE__, __LINE__);

    l_err = nc_get_var_float(l_ncId, l_var_simTime_id, o_simTime);
    checkNcErr(l_err, __FILE__, __LINE__);

    l_err = nc_get_var_float(l_ncId, l_var_frequency_id, o_frequency);
    checkNcErr(l_err, __FILE__, __LINE__);

    l_err = nc_get_var_float(l_ncId, l_var_dt_id, o_dt);
    checkNcErr(l_err, __FILE__, __LINE__);

    int l_time_step_index;
    l_err = nc_get_var_int(l_ncId, l_var_time_step_index_id, &l_time_step_index);
    *o_time_step_index = l_time_step_index;

    int reflecting_boundary;
    l_err = nc_get_var_int(l_ncId, l_var_reflecting_boundary_id, &reflecting_boundary);

    if (reflecting_boundary == 1)
    {
        *o_reflecting_boundary = true;
    }
    else
    {
        *o_reflecting_boundary = false;
    }

    int temp_k;
    l_err = nc_get_var_int(l_ncId, l_var_k_id, &temp_k);
    *o_k = (size_t)temp_k;

    checkNcErr(l_err, __FILE__, __LINE__);
    int l_solver = 0;

    l_err = nc_get_var_int(l_ncId, l_var_solver_id, &l_solver);
    checkNcErr(l_err, __FILE__, __LINE__);

    if (l_solver == 1)
    {
        *o_solver = "roe";
    }
    else
    {
        *o_solver = "fwave";
    }

    *o_b = new t_real[(*o_nx) * (*o_ny)];
    *o_h = new t_real[(*o_nx) * (*o_ny)];
    *o_hu = new t_real[(*o_nx) * (*o_ny)];
    *o_hv = new t_real[(*o_nx) * (*o_ny)];

    l_err = nc_get_var_float(l_ncId, l_var_b_id, *o_b);
    checkNcErr(l_err, __FILE__, __LINE__);

    l_err = nc_get_var_float(l_ncId, l_var_h_id, *o_h);
    checkNcErr(l_err, __FILE__, __LINE__);

    l_err = nc_get_var_float(l_ncId, l_var_hu_id, *o_hu);
    checkNcErr(l_err, __FILE__, __LINE__);

    l_err = nc_get_var_float(l_ncId, l_var_hv_id, *o_hv);
    checkNcErr(l_err, __FILE__, __LINE__);

    l_err = nc_close(l_ncId);
    checkNcErr(l_err, __FILE__, __LINE__);

    free(l_setup);
    free(l_stations_string);
    free(l_batfile);
    free(l_disfile);
}

void tsunami_lab::io::NetCdf::checkNcErr(int i_err, char const *i_file, int i_line)
{
    if (i_err)
    {
        std::cerr << "Error: " << nc_strerror(i_err) << " , in " << i_file << " , line " << i_line << std::endl;
        exit(2);
    }
}