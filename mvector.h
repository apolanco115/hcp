//
// Written by Austin Polanco on 16 JUL 23.
//

#ifndef HCP_MVECTOR_H
#define HCP_MVECTOR_H

#include <iostream>

class mvector {

    private:

        std::size_t _ncols;
        std::size_t _nrows;
        const std::size_t MAX_NROWS = 64;
        std::vector<int> _mvec;
        void copy(const mvector& mvec);
        std::size_t get_idx(std::size_t row, std::size_t col);
        void shift_back_one(std::size_t index);
        void shift_forward_one(std::size_t index);


    public:
        mvector();
        mvector(std::size_t nrows, std::size_t ncols);

        void set_ncols(std::size_t ncols);
        int size();
        int& at(std::size_t row, std::size_t col);

        void push_back(std::vector<int> data);
        void insert_row_at(std::vector<int> data, std::size_t index);

        void remove_row_at(std::size_t index);

        void display();


};


#endif //HCP_MVECTOR_H
