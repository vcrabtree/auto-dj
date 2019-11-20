//
// Created by vcrab on 11/19/2019.
//

#ifndef AUTO_DJ_LIST_H
#define AUTO_DJ_LIST_H

#include <stdexcept>
#include <string>
#include "Song.h"

class List {
public:
    List() {}

    virtual ~List() {}

    virtual List(const List& listToCopy);

    virtual List& operator=(const List& listToCopy);

    virtual void add(Song* song) = 0;

    virtual Song remove(std::string) = 0;

    virtual bool isEmpty() = 0;

    virtual void clear() = 0;

    virtual int getLength() = 0;

    virtual int find(std::string) = 0;
};

#endif //AUTO_DJ_LIST_H