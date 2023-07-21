//
// Created by Austin Polanco on 7/16/23.
//

#include "mvector.h"
#include <cassert>



mvector::mvector()
    : _nrows(0) {}

mvector::mvector(std::size_t nrows, std::size_t ncols)
        : _nrows(nrows), _ncols(ncols) {
    std::fill_n(std::back_inserter(_mvec), _ncols*MAX_NROWS, -3);
}

inline std::size_t mvector::get_idx(std::size_t row, std::size_t col){
    return (_ncols*row + col);
}

void mvector::set_ncols(std::size_t ncols){
    _ncols = ncols;
    std::fill_n(std::back_inserter(_mvec), _ncols*MAX_NROWS, -3);
}

int& mvector::at(std::size_t row, std::size_t col){
    return _mvec.at(get_idx(row, col));
}

void mvector::shift_back_one(std::size_t index){
    std::vector<int> tmp(_ncols, -1);
    for (std::size_t i = index; i < _nrows; ++i){
        for(std::size_t j = 0; j < _ncols; ++j){
            _mvec.at(get_idx(i, j)) = _mvec.at(get_idx(i+1, j));

        }
    }
}

void mvector::shift_forward_one(std::size_t index){
    std::vector<int> tmp(_ncols, -1);
    for (std::size_t i = _nrows; i > index; --i){
        for(std::size_t j = 0; j < _ncols; ++j){
            _mvec.at(get_idx(i, j)) = _mvec.at(get_idx(i-1, j));

        }
    }
}

void mvector::insert_row_at(std::vector<int> data, std::size_t index){

    assert(index <= _nrows);
    if (index == _nrows){
        push_back(data);
    }else{
        assert(data.size() == _ncols);
        shift_forward_one(index);
        for(std::size_t j = 0; j < _ncols; ++j){
            _mvec.at(get_idx(index, j)) = data.at(j);
        }
        _nrows++;
    }
}

void mvector::remove_row_at(std::size_t index){

    assert(index < _nrows);
    shift_back_one(index);
    _nrows--;
}

void mvector::push_back(std::vector<int> data){

    assert(data.size() == _ncols);

    // std::size_t loc = (nrows == 0) ? nrows : nrows + 1;

    for(std::size_t j = 0; j < _ncols; ++j){
        _mvec.at(get_idx(_nrows, j)) = data.at(j);
    }
    _nrows++;
}

void mvector::display(){
    if (_nrows == 0){
        std::cout<<"mvector is empty"<<std::endl;
    }else{
        for(size_t i = 0; i < _nrows; ++i){
            for(size_t j = 0; j < _ncols; ++j){
                std::cout<<_mvec.at(get_idx(i,j))<<" ";
            }
            std::cout<<std::endl;
        }
    }
}

int mvector::size(){
    return _nrows;
}