#include "Datagram.hpp"

void Datagram::consume(std::vector<char> wire) {
    std::string encodedString(wire.begin(), wire.end());
    this->src = encodedString[0];
    this->dest = encodedString[1];
    this->type = encodedString[2];
    if (isDV()) {
        size_t curPos = 3;
        size_t nextCommaPos = encodedString.find(",", curPos);
        size_t nextLinePos = encodedString.find("\r\n", curPos);
        while (nextCommaPos != std::string::npos && nextLinePos != std::string::npos) {
            this->dv[encodedString[nextCommaPos-1]] = std::stod(encodedString.substr(nextCommaPos + 1, nextLinePos - nextCommaPos - 1));
            curPos = nextLinePos + 1;
            nextCommaPos = encodedString.find(",", curPos);
            nextLinePos = encodedString.find("\r\n", curPos);
        }
    } else {
        this->data = encodedString.substr(3);
    }
}

bool Datagram::isDV() {
    return this->type == 'v';
}

char Datagram::getSrc() {
    return this->src;
}

char Datagram::getDest() {
    return this->dest;
}

std::map<char,double> Datagram::getDV() {
    return this->dv;
}

std::string Datagram::getData() {
    return this->data;
}

void Datagram::setSrc(char src) {
    this->src = src;
}

void Datagram::setDest(char dest) {
    this->dest = dest;
}

void Datagram::setDV(std::map<char,double> dv) {
    this->dv = dv;
    this->type = 'v';
}

void Datagram::setData(std::string data) {
    this->data = data;
    this->type = 'd';
}

std::vector<char> Datagram::encode() {
    std::string encodedString = std::string("") + this->src + this->dest + this->type;
    for (std::map<char, double>::iterator it = this->dv.begin(); it != this->dv.end(); ++it) {
		encodedString += std::string("") + (it->first) + ","  + std::to_string(it->second) + "\r\n";
	}
	std::vector<char> encodedVector(encodedString.begin(), encodedString.end());
	return encodedVector;
}
