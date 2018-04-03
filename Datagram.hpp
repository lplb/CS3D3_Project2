#ifndef DATAGRAM_H
#define DATAGRAM_H

#include <map>
#include <vector>
#include <string>

class Datagram {

private:
    char src;
    std::map<char,int> dv;
    char dest;
    //data
    
public:
    // creates a new datagram from an encoded char vector
    void consume(std::vector<char> wire);
    
    char getSrc();
    char getDest();
    std::map<char,int> getDV();
    
    void setSrc(char src);
    void setDest(char dest);
    void setDV(std::map<char,int> dv);
    
    // encodes the datagram in a char vector
    std::vector<char> encode();
    
};

#endif
