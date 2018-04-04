#ifndef DATAGRAM_H
#define DATAGRAM_H

#include <map>
#include <vector>
#include <string>

class Datagram {

private:
    char src;
    std::map<char,double> dv;
    char dest;
    char type;
    std::string data;
    
public:
    // creates a new datagram from an encoded char vector
    void consume(std::vector<char> wire);
    
    bool isDV();
    char getSrc();
    char getDest();
    std::map<char,double> getDV();
    std::string getData();
    
    void setSrc(char src);
    void setDest(char dest);
    void setDV(std::map<char,double> dv);
    void setData(std::string data);
    
    // encodes the datagram in a char vector
    std::vector<char> encode();
    
};

#endif
