#ifndef TSUNAMI_LAB_IO_STATIONS
#define TSUNAMI_LAB_IO_STATIONS

#include "../../constants.h"
#include "string"
#include <fstream>
#include <vector>
#include <iostream>
#include <filesystem>
#include <algorithm>
#include <cmath>


namespace tsunami_lab {
  namespace io {
    class Station;
  }
}

class tsunami_lab::io::Station{
    public:
    /**
    * @brief Writes station data to a CSV file.
    *
    * This function appends station data to a CSV file specified by the provided path. The CSV file
    *
    * @param i_x                The x-coordinate of the station.
    * @param i_y                The y-coordinate of the station.
    * @param i_time_in_seconds  This is the time for the values.
    * @param i_water_height     The water height at the station.
    * @param i_csv_path         The path to the CSV file to which the station data will be appended.
    */
    static void write(tsunami_lab::t_idx              i_x,
                      tsunami_lab::t_idx              i_y,
                      tsunami_lab::t_real             i_time_in_seconds,
                      tsunami_lab::t_real             i_water_height,
                      tsunami_lab::t_real             i_water_hu,
                      tsunami_lab::t_real             i_water_hv,
                      std::string                     i_csv_path,
                      std::string i_dim);
    /**
    * @brief resets a station file to the last Checkpoint.
    *
    * This function resets the station to the last checkpoint and writes the station data to a CSV file specified
    * by the provided path.
    *
    * @param simulation_time    The time where the station will be reseted too.
    * @param csv_file_path      The path to the CSV file to which the station data will be reseted.
    */
    static void updateStation(t_real simulation_time, std::string csv_file_path);

    /**
    * @brief Converts the station data (json) to a string.
    *
    * This function converts the station data (json) to a string and then returns it.
    * @return Json as a string.
    */
    static std::string Stringify();

};

#endif