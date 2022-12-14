//
// Written by Austin Polanco on 15 JUN 22.
//

#ifndef HCP_HIERARCHICAL_MODEL_H
#define HCP_HIERARCHICAL_MODEL_H

#include "parameters.h"
#include <iostream>
#include <array>
#include <gsl/gsl_rng.h>
#include "indexed_list.h"
#include "readgml.h"

class hierarchical_model {
    public:
        int num_groups;
        int max_num_groups;

        NETWORK G; // struct storing the network
        std::vector<uint64_t> g; // group assignments
        indexed_list nodes_in;
        indexed_list nodes_out;
        std::vector<long long> group_size;
        std::vector<long long> hcg_edges; // edges in each group
        std::vector<long long> hcg_pairs; // viable pairs in each group
        std::vector<double> log_fact = std::vector<double>(270000000, 0);
        std::unordered_map<uint64_t, std::size_t> bit_groups;
        gsl_rng *rng;
        double loglike;
        bool removed_group = false;
        bool removed_node = false;
        bool added_group = false;

        hierarchical_model(parameters params);

        inline std::size_t hcg(int u, int v);
        inline std::size_t hcg_node(const uint64_t& old_state, int u);
        inline uint64_t insert_zero_at(uint64_t val, std::size_t pos);
        inline uint64_t remove_bit_at(uint64_t val, std::size_t pos);

        void partition();

        void set_hcg_edges();
        void set_hcg_pairs();
        std::vector<std::vector<int>> get_group_matrix();
        void set_nodes_in_out();


        void print_hcg_pairs();
        void print_hcg_edges();
        void print_group_size();
        void print_g();

        void update_hcg_props(int u, const uint64_t& old_state);

        double ln_fact(int arg);

        double calc_loglike();
        void update_bit(uint64_t& state, uint64_t bit, std::size_t group);


        void uniform_group_size(uint64_t& old_state, int& rand_node, int& rand_group, int& rand_idx);
        void get_groups();

};


#endif //HCP_HIERARCHICAL_MODEL_H
