//
// Created by Austin Polanco on 7 DEC 22.
//

#include "parameters.h"
#include <climits>
#include <iostream>
#include <fstream>
#include <sstream>

parameters::parameters(std::string file_name)
{
    read_params(file_name);
}

void parameters::read_params(std::string file_name) {


    std::ifstream file{file_name};
    if(file.fail()){
        std::cerr << "Error reading: "<<file_name;
        exit(EXIT_FAILURE);
    }

    std::string line;
    while(std::getline(file, line) )
    {
        std::istringstream is_line(line);
        std::string key;
        if(std::getline(is_line, key, ':')){
            if (key == "max_itr") {
                long long value;
                is_line >> value;
                if (value > 0 && value <= LLONG_MAX) {
                    max_itr = value;
                } else {
                    std::cout << "Warning: unsupported maximum number of iterations. Using default value instead."
                              << std::endl;
                }
                std::cout << "max_itr: " << max_itr << std::endl;
            }else if(key == "max_num_groups"){
                int value;
                is_line >> value;
                if (value > 0 && value <= 64) {
                    max_num_groups = value;
                } else {
                    std::cout << "Warning: unsupported maximum number of groups. Using default value instead."
                              << std::endl;
                }
                std::cout << "max_num_groups: " << max_num_groups << std::endl;
            }else if(key == "initial_num_groups"){
                int value;
                is_line >> value;
                if (value > 0 && value <= 64) {
                    initial_num_groups = value;
                } else {
                    std::cout << "Warning: unsupported number of groups. Using default value instead."
                              << std::endl;
                }
                std::cout << "initial_num_groups: " << initial_num_groups << std::endl;
            }else if(key == "gml_path"){
                std::string value;
                is_line >> value;
                if( value.empty()){
                    std::cerr<<"no network specified"<<std::endl;
                    error_status = 1;
                    return;
                }else{
                    gml_path = value;
                    std::cout<<"gml_path: "<<gml_path<<std::endl;
                }
            }else if (key == "initial_group_config"){
                    uint64_t value;
                    std::vector<uint64_t> group_configs{};
                    while ( is_line >> value ) {
                        group_configs.push_back(value);
                    }

                    initial_group_config = group_configs;
            }else if(key == "saved_data_name"){
                std::string value;
                is_line >> value;
                if(!value.empty()){
                    saved_data_name = value;
                }else{
                    std::cout << "Warning: unspecified saved data name. Using default value instead."
                              << std::endl;
                }

            }else if(key == "save_directory"){
                std::string value;
                is_line >> value;
                if(!value.empty()){
                    std::filesystem::path p(value);
                    if (!std::filesystem::exists(p)){
                        std::filesystem::create_directories(p);
                    }
                    save_dir = p;
                }else{
                    std::cout << "Warning: unspecified save directory. Saving in current working directory instead."
                              << std::endl;
                }

            }

        }
    }
    if (max_num_groups < initial_num_groups){
        std::cout<<"initial number of groups is greater than maximum number of groups."<<std::endl;
        std::cout<<"Using default value instead."<<std::endl;
        initial_num_groups = 2;
    }

}

int parameters::get_error_status() const {
    return error_status;
}

int parameters::get_max_num_groups() const {
    return max_num_groups;
}

int parameters::get_initial_num_groups() const {
    return initial_num_groups;
}

long  parameters::get_max_itr() const {
    return max_itr;
}

const std::string &parameters::get_gml_path() const {
    return gml_path;
}

const std::string &parameters::get_saved_data_name() const {
    return saved_data_name;
}

const std::filesystem::path &parameters::get_save_dir() const {
    return save_dir;
}

const std::vector<uint64_t> &parameters::get_initial_group_config() const {
    return initial_group_config;
}
