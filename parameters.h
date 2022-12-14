//
// Created by Austin Polanco on 7 DEC 22.
//

#ifndef HCP_PARAMETERS_H
#define HCP_PARAMETERS_H

#include <unordered_map>
#include <string>
#include <any>
#include <vector>
#include <filesystem>

class parameters {

    private:
        void read_params(std::string file_name);
        int error_status = 0;
        long max_itr = 1000000000;
        int max_num_groups = 64;
        int initial_num_groups = 2;
        std::vector<uint64_t> initial_group_config;

        std::string gml_path = "";
        std::string saved_data_name = "data";
        std::filesystem::path save_dir = std::filesystem::current_path();




    public:
        parameters(std::string file_name);

        int get_error_status() const;
        int get_max_num_groups() const;
        int get_initial_num_groups() const;
        long get_max_itr() const;
        const std::vector<uint64_t> &get_initial_group_config() const;
        const std::string &get_gml_path() const;
        const std::string &get_saved_data_name() const;
        const std::filesystem::path &get_save_dir() const;


};


#endif //HCP_PARAMETERS_H
