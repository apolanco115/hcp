//
// Written by Austin Polanco on 15 JUN 22.
//

#include <iostream>
#include "parameters.h"
#include "hierarchical_model.h"
#include <chrono>
#include <fstream>
#include <ctime>

int main(int argc, char* argv[]) {

    std::string config_file = argv[1];
    parameters params(config_file);

    if (params.get_error_status() == 1){
        return EXIT_FAILURE;
    }


    hierarchical_model hcp(params);

    hcp.print_hcg_pairs();
    std::cout<<std::endl;
    hcp.print_hcg_edges();
    std::cout<<std::endl;

    std::vector<std::vector<uint64_t>> intermediate_states;
    std::vector<std::vector<long long>> hcg_edges;
    std::vector<std::vector<long long>> hcg_pairs;
    std::vector<std::vector<long long>> group_size;
    std::vector<double> energies;
    std::vector<std::size_t> num_groups;




    long num_itrs = params.get_max_itr();
    for(long i = 0; i < num_itrs; ++i){
        hcp.get_groups();
        if((i>10000000) && (i%1500==0)){
            intermediate_states.push_back(hcp.g);
            hcg_edges.push_back(hcp.hcg_edges);
            hcg_pairs.push_back(hcp.hcg_pairs);
            group_size.push_back(hcp.group_size);
            energies.push_back(hcp.loglike);
            num_groups.push_back(hcp.num_groups);
        }
        if(i%10000000==0){
            std::cout<<"-----------------------------------------------------"<<std::endl;
            auto curr = std::chrono::system_clock::now();
            auto tm = std::chrono::system_clock::to_time_t(curr);
            std::cout<<"time: "<< std::put_time(std::localtime(&tm), "%c %Z")<<std::endl;
            std::cout<<"iteration: "<<i<<" energy: "<<hcp.loglike<<std::endl;
            hcp.print_hcg_pairs();
            std::cout<<std::endl;
            hcp.print_hcg_edges();
            std::cout<<std::endl;
            hcp.print_group_size();
            std::cout<<std::endl;
        }
    }

    std::cout<<"Writing data to file."<<std::endl;
    std::string filename = params.get_saved_data_name();
    std::string filepath = params.get_save_dir();
    std::ofstream output_groups(filepath+filename+"_configs.txt");
    std::ofstream output_ngroups(filepath+filename+"_num_groups.txt");
    std::ofstream output_group_size(filepath+filename+"_group_size.txt");
    std::ofstream output_edges(filepath+filename+"_edges.txt");
    std::ofstream output_pairs(filepath+filename+"_pairs.txt");
    std::ofstream output_ll(filepath+filename+"_ll.txt");

    for (int el = 0; el < intermediate_states.size(); ++el) {
        // output decimal representation of groups
        for(int l = 0; l < intermediate_states[el].size(); ++l){
            output_groups << intermediate_states[el][l]<<" ";
        }
        output_groups << std::endl;

        for(int mu = 0; mu < hcg_edges[el].size(); ++mu){
            output_edges << hcg_edges[el][mu]<<" ";
            output_pairs << hcg_pairs[el][mu]<<" ";
            output_group_size << group_size[el][mu]<<" ";
        }
        output_edges << std::endl;
        output_pairs << std::endl;
        output_group_size << std::endl;
        output_ll << energies[el]<< std::endl;
        output_ngroups << num_groups[el] << std::endl;

    }
    output_groups.close();
    output_edges.close();
    output_pairs.close();
    output_group_size.close();
    output_ll.close();
    std::cout<<"Simulation data saved successfully."<<std::endl;
    return 0;
}
