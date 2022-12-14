//
// Written by Austin Polanco on 14 JUL 22.
//

#ifndef HCP_INDEXED_LIST_H
#define HCP_INDEXED_LIST_H

#include "node.h"
#include <iostream>
#include <unordered_map>

class indexed_list {

    private:
        std::size_t length;
        node* head;
        node* tail;
        std::unordered_map<std::size_t, node*> lookup;

        void add_to_lookup(int index, node* new_node);
        void remove_from_lookup(std::size_t index);
        void copy(const indexed_list& list);
        void clear();



    public:
        indexed_list();
        indexed_list(const indexed_list& list);
        indexed_list& operator= (const indexed_list& rhs);
        ~indexed_list();

        int size();
        int& at(std::size_t row, std::size_t col);
        std::vector<int> at(std::size_t col);
        const node* get_head();
        const node* get_tail();
        std::vector<int> get_head_data();
        std::vector<int> get_tail_data();

        void push_front(std::vector<int> data);
        void push_back(std::vector<int> data);
        void insert_at(std::vector<int> data, std::size_t index);
        void pop_front();
        void pop_back();

        void remove_at(std::size_t col);

        void display();
        void display_lookup();

};


#endif //HCP_INDEXED_LIST_H
