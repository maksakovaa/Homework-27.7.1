#pragma once
#include "net.h"

template<class T> class Base
{
protected:
    vector<T>* base;
public:
    Base();
    ~Base();
    void addInBase(T newElem);
    size_t getCount();
};

template<class T> Base<T>::Base()
{
    base = new vector<T>;
}

template<class T> Base<T>::~Base()
{
    delete base;
}

template<class T> void Base<T>::addInBase(T newElem)
{
    base->push_back(newElem);
}

template<class T> size_t Base<T>::getCount()
{
    return base->size();
}