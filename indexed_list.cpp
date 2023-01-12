//
// Written by Austin Polanco on 14 JUL 22.
//

#include "indexed_list.h"



indexed_list::indexed_list()
        : length(0), head(nullptr), tail(nullptr) {};
// copy constructor
indexed_list::indexed_list(const indexed_list& list)
        : length(0), head(nullptr), tail(nullptr){

    copy(list);

}

indexed_list::~indexed_list(){
    clear();
}

indexed_list& indexed_list::operator= (const indexed_list& rhs){
    clear();
    indexed_list temp(rhs);
    std::swap(temp.head, head);
    std::swap(temp.tail, tail);
    std::swap(temp.lookup, lookup);
    std::swap(temp.length, length);
    return *this;
}

void indexed_list::copy(const indexed_list& list) {
    node* curr = list.head;
    while (curr){
        push_back(curr->data);
        curr = curr->next;
    }
}



void indexed_list::add_to_lookup(int index, node* new_node){
    for(std::size_t i = length; i>index; --i){
        lookup[i] = lookup[i-1];
    }
    lookup[index] = new_node;
    length++;
}
void indexed_list::remove_from_lookup(std::size_t index){
    for(std::size_t i = index; i < length-1; ++i){
        lookup[i] = lookup[i+1];
    }
    lookup.erase(length-1);
    length--;
}

int& indexed_list::at(std::size_t row, std::size_t col){
    return lookup[col]->data.at(row);
}

std::vector<int> indexed_list::at(std::size_t col){
    return lookup[col]->data;
}

int indexed_list::size(){
    return length;
}
const node* indexed_list::get_head() {
    if (head) {
        return head;
    }
    std::cout<<"list is empty"<<std::endl;
    return {};
}

const node* indexed_list::get_tail() {
    if (head) {
        return tail;
    }
    std::cout<<"list is empty"<<std::endl;
    return {};
}


std::vector<int> indexed_list::get_head_data(){
    if (head) {
        return head->data;
    }
    std::cout<<"list is empty"<<std::endl;
    return {};
}
std::vector<int> indexed_list::get_tail_data(){

    if (head) {
        return tail->data;
    }
    std::cout<<"list is empty"<<std::endl;
    return {};
}
void indexed_list::display_lookup(){
    if(head == nullptr){
        std::cout<<"list is empty"<<std::endl;
        return;
    }
    std::cout << "index" << '\t' << "address" <<std::endl;
    for ( const auto &entry : lookup ){
        std::cout << entry.first << '\t' << entry.second << std::endl;
    }
}
void indexed_list::push_front(std::vector<int> data){

    node* new_node = new node(data);

    if(head == nullptr){
        head = new_node;
        tail = new_node;
        add_to_lookup(0, head);
        return;
    }
    new_node->next = head;
    head = new_node;
    add_to_lookup(0, head);
}

void indexed_list::push_back(std::vector<int> data){
    node* new_node = new node(data);
    if (head == nullptr){
        head = new_node;
        tail = new_node;
        add_to_lookup(length, tail);
        return;
    }

    tail->next = new_node;
    tail = tail->next;
    add_to_lookup(length, tail);
}

void indexed_list::insert_at(std::vector<int> data, std::size_t index){
    if (index == 0){
        push_front(data);
        return;
    }else if (index == length){
        push_back(data);
        return;
    }else if(index < 0 || index > length){
        std::cout<<"invalid index"<<std::endl;
        return;
    }
    node* new_node = new node(data);
    new_node->next = lookup[index-1]->next;
    lookup[index-1]->next = new_node;
    add_to_lookup(index, new_node);
}

void indexed_list::pop_front(){
    if (head == nullptr){
        std::cout<<"list is empty"<<std::endl;
        return;
    }
    node* temp = head;
    head = head->next;
    delete temp;
    remove_from_lookup(0);
}

void indexed_list::pop_back(){
    if (head == nullptr){
        std::cout<<"list is empty"<<std::endl;
        return;
    }else if(length == 1){
        pop_front();
        return;
    }
    node* temp = tail;
    tail  = lookup[length-2];
    tail->next = nullptr;
    delete temp;
    remove_from_lookup(length-1);
}

void indexed_list::remove_at(std::size_t col){
    if (head == nullptr){
        std::cout<<"list is empty"<<std::endl;
        return;
    }
    if (col < 0 || col > length-1) {
        std::cout<<"invalid index"<<std::endl;
        return;
    }else if(col == 0){
        pop_front();
        return;

    }else if(col == length-1){
        pop_back();
        return;
    }

    node* temp = lookup[col];
    lookup[col - 1]->next = lookup[col+1];
    delete temp;
    remove_from_lookup(col);
}
void indexed_list::clear(){
    if (head == nullptr){
        return;
    }
    node* temp = head;
    while(temp){
        temp = head->next;
        delete head;
        head = temp;
    }
    lookup.clear();
    length = 0;
}

void indexed_list::display() {
    if (head == nullptr){
        std::cout<<"list is empty"<<std::endl;
        return;
    }
    node* temp = head;
    while(temp){
        for(const int& i : temp->data){
            std::cout<<i<<" ";
        }
        std::cout<<std::endl;
        temp = temp->next;
    }
}
