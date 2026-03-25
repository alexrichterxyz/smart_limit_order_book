#include <iostream>
#include "include/slob.hpp"



int main() {
    
    
    slob::book book1;
    book1.insert(std::make_shared<slob::order>(slob::side::ask, 1000, 1));
    book1.insert(std::make_shared<slob::order>(slob::side::bid, 1000, 1));
    std::cout << book1 << std::endl;

    slob::book book2;
    book2.insert(std::make_shared<slob::order>(slob::side::bid, 1000, 1));
    book2.insert(std::make_shared<slob::order>(slob::side::ask, 1000, 1));
    std::cout << book2 << std::endl;
   
    

 
    return 0;
}