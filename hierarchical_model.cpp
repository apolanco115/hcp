//
// Written by Austin Polanco on 15 JUN 22.
//

#include "hierarchical_model.h"
#include <iostream>
#include <random>
#include "readgml.h"
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>


hierarchical_model::hierarchical_model(parameters params){
    std::cout<<"reading in network"<<std::endl;
    std::string network_path = params.get_gml_path();
    read_network(&G, network_path);
    num_groups = params.get_initial_num_groups();
    max_num_groups = params.get_max_num_groups();

    rng = gsl_rng_alloc(gsl_rng_mt19937);
    gsl_rng_set(rng,time(NULL));

    if (params.get_initial_group_config().empty()){
        std::cout<<"assigning random groups to nodes"<<std::endl;
        g.assign(G.nvertices, 0);
        partition();
    }else{
        std::cout<<"assigning user specified groups to nodes"<<std::endl;
        g = params.get_initial_group_config();
    }

    hcg_edges.assign(num_groups, 0);
    hcg_pairs.assign(num_groups, 0);

    set_nodes_in_out();
    set_hcg_edges();
    set_hcg_pairs();

    for(int i = 0; i < log_fact.size(); ++i){
        log_fact[i] = ln_fact(i);
    }

    loglike = calc_loglike();

}


double hierarchical_model::ln_fact(int arg){
    return lgamma(arg+1);
}


void hierarchical_model::partition() {

    uint64_t max = (1UL<<(num_groups-1));

    for (int u = 0; u < G.nvertices; ++u){
        g[u] = (gsl_rng_uniform_int(rng, max)<<1UL)+1;
    }
}

inline std::size_t hierarchical_model::hcg(int u, int v){

    uint64_t group_mask = (1UL<<num_groups)-1;
    uint64_t masked_u = g[u] & group_mask;
    uint64_t masked_v = g[v] & group_mask;

    uint64_t common_bits = masked_u&masked_v;
    common_bits |= (common_bits>>1UL);
    common_bits |= (common_bits>>2UL);
    common_bits |= (common_bits>>4UL);
    common_bits |= (common_bits>>8UL);
    common_bits |= (common_bits>>16UL);
    common_bits |= (common_bits>>32UL);

    return (63UL - __builtin_clzll((common_bits - (common_bits>>1UL))));
}

inline std::size_t hierarchical_model::hcg_node(const uint64_t& old_state, int u) {

    uint64_t group_mask = (1UL<<num_groups)-1;
    uint64_t masked_u = old_state & group_mask;
    uint64_t masked_v = g[u] & group_mask;

    uint64_t common_bits = masked_u&masked_v;
    common_bits |= (common_bits>>1UL);
    common_bits |= (common_bits>>2UL);
    common_bits |= (common_bits>>4UL);
    common_bits |= (common_bits>>8UL);
    common_bits |= (common_bits>>16UL);
    common_bits |= (common_bits>>32UL);

    return (63UL - __builtin_clzll((common_bits - (common_bits>>1UL))));
}

void hierarchical_model::set_hcg_edges(){
    for (int u = 0; u < G.nvertices; ++u){
        for (int w = 0; w < G.vertex[u].degree; ++w){
            int v = G.vertex[u].edge[w].target;
            if (u < v){
                std::size_t highest = hcg(u, v);
                hcg_edges[highest]++;
            }
        }
    }
}

std::vector<std::vector<int>> hierarchical_model::get_group_matrix(){
    std::vector<std::vector<int>> group_matrix;

    for(int u = 0; u < G.nvertices; ++u){
        std::vector<int> group_col;
        for(int r =  0; r < num_groups; ++r){
            group_col.push_back((g[u] >> r)&1);
        }
        group_matrix.push_back(group_col);
    }
    return group_matrix;
}

void hierarchical_model::set_hcg_pairs() {
    for(int u = 0; u < G.nvertices; ++u){
        for(int v = u+1; v < G.nvertices; ++v){
            hcg_pairs[hcg(u, v)]++;
        }
    }
}

void hierarchical_model::set_nodes_in_out() {
    std::vector<std::vector<int>> group_matrix = get_group_matrix();

    for(std::size_t r = 0; r < num_groups; ++r){
        nodes_in.push_back(std::vector<int>(G.nvertices, -1));
        nodes_out.push_back(std::vector<int>(G.nvertices, -1));
        int in_g = 0;
        int out_g = 0;
        for (int u = 0; u < G.nvertices; ++u){
            if (group_matrix[u][r]== 1){
                nodes_in.at(in_g, r) = u;
                in_g++;
            }else{
                nodes_out.at(out_g, r) = u;
                out_g++;
            }
        }
        group_size.push_back(in_g);
    }
}

inline uint64_t hierarchical_model::remove_bit_at(uint64_t val, std::size_t pos) {
    uint64_t group_mask = (1UL<<num_groups)-1;
    uint64_t upper_mask = (group_mask<<(pos+1))&group_mask;
    uint64_t lower_mask = (group_mask>>(num_groups-pos))&group_mask;

    uint64_t upper = val&upper_mask;
    uint64_t lower = val&lower_mask;

    return ((upper>>1) | lower);
}

inline uint64_t hierarchical_model::insert_zero_at(uint64_t val, std::size_t pos) {

    uint64_t group_mask = (1UL<<num_groups)-1;
    uint64_t select_mask = (group_mask<<pos)&group_mask;

    uint64_t left = val&select_mask;
    uint64_t right = val&(~select_mask);

    return ((left<<1) | right);

}

void hierarchical_model::print_hcg_pairs() {
    std::cout<<"number of pairs: ";
    for(int q = 0; q < num_groups; ++q){
        std::cout<<hcg_pairs[q]<<" ";
    }
}

void hierarchical_model::print_hcg_edges() {
    std::cout<<"number of edges: ";
    for (int q = 0; q < num_groups; ++q){
        std::cout<<hcg_edges[q]<<" ";
    }
}

void hierarchical_model::print_group_size() {
    std::cout<<"group sizes: ";
    for (int q = 0; q < num_groups; ++q){
        std::cout<<group_size[q]<<" ";
    }
}

void hierarchical_model::print_g() {
    std::cout<<"group assignments"<<std::endl;
    std::vector<std::vector<int>> group_matrix = get_group_matrix();
    for (int u = 0; u < G.nvertices; ++u){
        for (int q = 0; q<num_groups; ++q){
            std::cout<<group_matrix[u][q]<<", ";
        }
        std::cout<<std::endl;
    }
}

void hierarchical_model::update_hcg_props(int u, const uint64_t& old_state){
    for (int v = 0; v < u; ++v){
        int nh = hcg(u, v);
        int old = hcg_node(old_state, v);
        hcg_pairs.at(old)--;
        hcg_pairs.at(nh)++;
    }

    for (int v = u+1; v < G.nvertices; ++v){
        int nh = hcg(u, v);
        int old = hcg_node(old_state, v);
        hcg_pairs.at(old)--;
        hcg_pairs.at(nh)++;
    }

    for(int w = 0; w < G.vertex[u].degree; ++w){
        int v = G.vertex[u].edge[w].target;
        int nh = hcg(u, v);
        int old = hcg_node(old_state, v);
        hcg_edges.at(old)--;
        hcg_edges.at(nh)++;
    }
}

double hierarchical_model::calc_loglike() {
    double res = 0.0;
    for (int q = 0; q < num_groups; ++q){
        res+=(log_fact[hcg_edges[q]] + log_fact[hcg_pairs[q] - hcg_edges[q]]);
        res-=(log_fact[hcg_pairs[q]+1]);
    }

    return res;
}

void hierarchical_model::update_bit(uint64_t& state, uint64_t bit, std::size_t group){

    uint64_t group_mask = (1UL<<num_groups)-1;
    uint64_t idxr = ~(1UL<<group)&group_mask;
    uint64_t mask = state & idxr;
    state = (mask | (bit<<group))&group_mask;

}

void hierarchical_model::uniform_group_size(uint64_t& old_state, int& rand_node, int& rand_group, int& rand_idx){

    std::size_t num_nodes = G.nvertices;
    double p_type2 = 1.0/(2*num_groups*(num_nodes+1));

    if(gsl_rng_uniform(rng) < p_type2){
        // adds empty group or does nothing if number of groups is equal to maximum number of groups
        old_state = 0;
        if (num_groups == max_num_groups){
            rand_node = -1;
            rand_idx = -1;
            rand_group = -1;
            return;
        }else{
            // add empty group

            rand_node = -2;
            rand_idx = -2;

            rand_group = gsl_rng_uniform_int(rng, num_groups) + 1;

            nodes_in.insert_at(std::vector<int>(G.nvertices, -1), rand_group);
            nodes_out.insert_at(std::vector<int>(G.nvertices, -1), rand_group);

            for (int i = 0; i < num_nodes; ++i) {
                nodes_out.at(i, rand_group) = i;
            }

            group_size.insert(group_size.begin() + rand_group, 0);
            hcg_edges.insert(hcg_edges.begin() + rand_group, 0);
            hcg_pairs.insert(hcg_pairs.begin() + rand_group, 0);

            for (int u = 0; u < G.nvertices; ++u) {
                g[u] = insert_zero_at(g[u], rand_group);
            }

            num_groups++;
            added_group = true;
        }
    }else{
        if (num_groups == 1){
            // do nothing!
            rand_node = -1;
            rand_group = -1;
            rand_idx = -1;
            old_state = 0;
            return;
        }

        rand_group = gsl_rng_uniform_int(rng,num_groups-1)+1;

        std::size_t n_out = G.nvertices - group_size[rand_group];

        if(gsl_rng_uniform(rng) < 0.5){
            // remove a node from the group
            if(group_size[rand_group] == 0) {
                // remove group entirely!
                rand_idx = -2;
                rand_node = -2;

                for(int u = 0; u < G.nvertices; ++u){
                    g[u] = remove_bit_at(g[u], rand_group);
                }

                old_state = 0;

                nodes_in.remove_at(rand_group);
                nodes_out.remove_at(rand_group);

                hcg_edges.erase(hcg_edges.begin()+rand_group);
                hcg_pairs.erase(hcg_pairs.begin()+rand_group);
                group_size.erase(group_size.begin()+rand_group);
                num_groups--;
                removed_group = true;
                return;
            }else{
                // chose a node from the group at random and remove it
                rand_idx = gsl_rng_uniform_int(rng,group_size[rand_group]);
                rand_node = nodes_in.at(rand_idx, rand_group);
                nodes_in.at(rand_idx, rand_group) = nodes_in.at(group_size[rand_group]-1, rand_group);
                nodes_out.at(n_out, rand_group) = rand_node;
                old_state = g[rand_node];
                g[rand_node]-=(1UL<<rand_group);
                group_size[rand_group]--;
                removed_node = true;
                return;
            }
        }else{
            // add a node to the group
            if(group_size[rand_group] == G.nvertices){
                rand_node = -1;
                rand_idx = -1;
                old_state = 0;
                return;
            }else{
                rand_idx = gsl_rng_uniform_int(rng,n_out);
                rand_node = nodes_out.at(rand_idx, rand_group);
                nodes_out.at(rand_idx, rand_group) = nodes_out.at(n_out-1, rand_group);
                nodes_in.at(group_size[rand_group], rand_group) = rand_node;
                old_state = g[rand_node];
                g[rand_node]+=(1UL<<rand_group);
                group_size[rand_group]++;
            }
        }
    }
}

void hierarchical_model::get_groups() {

    double new_loglike;
    uint64_t old_state;
    std::vector<long long> old_hcg_edges(num_groups, 0);
    std::vector<long long> old_hcg_pairs(num_groups, 0);
    for(int q = 0; q < num_groups; ++q){
        old_hcg_edges[q] = hcg_edges[q];
        old_hcg_pairs[q] = hcg_pairs[q];
    }

    int rand_node;
    int rand_group;
    int rand_idx;
    uniform_group_size(old_state, rand_node, rand_group, rand_idx);
    if (rand_node == -1){
        new_loglike = loglike;

    }else{
        if (rand_node == -2){
            new_loglike = loglike;
        }else{
            update_hcg_props(rand_node, old_state);
            new_loglike = calc_loglike();
        }
        if(gsl_rng_uniform(rng) < exp(new_loglike - loglike)){
            loglike = new_loglike;
            removed_node = false;
            removed_group = false;
            added_group = false;
        }else{
            if(removed_node){
                group_size[rand_group]++;
                int n_out = G.nvertices - group_size[rand_group];
                nodes_out.at(n_out, rand_group) = -1;
                nodes_in.at(rand_idx, rand_group) = rand_node;
                g[rand_node]+=(1UL<<rand_group);
                for(int q = 0; q < num_groups; ++q) {
                    hcg_edges[q] = old_hcg_edges[q];
                    hcg_pairs[q] = old_hcg_pairs[q];
                }
                removed_node = false;
            }else if(removed_group){
                nodes_in.insert_at(std::vector<int>(G.nvertices, -1), rand_group);
                nodes_out.insert_at(std::vector<int>(G.nvertices, -1), rand_group);

                for (int u = 0; u < G.nvertices; ++u){
                    nodes_out.at(u, rand_group) = u;
                }
                for (int u = 0; u < G.nvertices; ++u){
                    g[u] = insert_zero_at(g[u], rand_group);
                }
                num_groups++;
                group_size.insert(group_size.begin()+rand_group, 0);
                hcg_edges.insert(hcg_edges.begin()+rand_group, 0);
                hcg_pairs.insert(hcg_pairs.begin()+rand_group, 0);
                for(int q = 0; q < num_groups; ++q) {
                    hcg_edges[q] = old_hcg_edges[q];
                    hcg_pairs[q] = old_hcg_pairs[q];
                }
                removed_group=false;
            }else if (added_group){
                nodes_in.remove_at(rand_group);
                nodes_out.remove_at(rand_group);
                group_size.erase(group_size.begin()+rand_group);
                for (int u = 0; u < G.nvertices; ++u){
                    g[u] = remove_bit_at(g[u], rand_group);
                }
                hcg_edges.erase(hcg_edges.begin()+rand_group);
                hcg_pairs.erase(hcg_pairs.begin()+rand_group);
                num_groups--;
                for(int q = 0; q < num_groups; ++q) {
                    hcg_edges[q] = old_hcg_edges[q];
                    hcg_pairs[q] = old_hcg_pairs[q];
                }
                added_group = false;
            }else{
                // node added to a group
                group_size[rand_group]--;
                nodes_in.at(group_size[rand_group], rand_group) = -1;
                nodes_out.at(rand_idx, rand_group) = rand_node;
                g[rand_node]-=(1UL<<rand_group);
                for(int q = 0; q < num_groups; ++q) {
                    hcg_edges[q] = old_hcg_edges[q];
                    hcg_pairs[q] = old_hcg_pairs[q];
                }
            }
        }
    }
}




