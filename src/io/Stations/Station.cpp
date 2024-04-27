#include "Station.h"



void tsunami_lab::io::Station::write(tsunami_lab::t_idx              i_x,
                                     tsunami_lab::t_idx              i_y,
                                     tsunami_lab::t_real             i_time_in_seconds,
                                     tsunami_lab::t_real             i_water_height,
                                     tsunami_lab::t_real             i_water_hu,
                                     tsunami_lab::t_real             i_water_hv,
                                     std::string                     i_csv_path,
                                     std::string i_dim){

    std::ofstream io_stream(i_csv_path,std::ios::app);
  
    if (!io_stream.is_open()) {
        std::cerr << "Error opening file: " << i_csv_path << std::endl;
        return ; 
    }
  
    std::uintmax_t fileSize = std::filesystem::file_size(i_csv_path);
    if(fileSize == 0){
        
        io_stream << "x,y,water_height,water_hu";
        if(i_dim == "2d"){
            io_stream <<",water_hv"; 
        }
        io_stream <<",time_in_seconds";
    }
    io_stream << "\n";
    io_stream << i_x << "," << i_y << "," << i_water_height << "," << i_water_hu; 
    if(i_dim == "2d"){
        io_stream << "," << i_water_hv;
    }
    io_stream << ","<< i_time_in_seconds;
    io_stream << std::flush;
}


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


std::string tsunami_lab::io::Station::Stringify() {
    
    std::string json_file_path = "configs/stations.json";
    // Open the JSON file
    std::ifstream json_file(json_file_path);

    if (!json_file.is_open()) {
        std::cerr << "Error opening JSON file: " << json_file_path << std::endl;
        return "";
    }

    // Read the contents of the JSON file into a string
    std::string json_string((std::istreambuf_iterator<char>(json_file)),
                            std::istreambuf_iterator<char>());

    // Close the JSON file
    json_file.close();

    return json_string;
}