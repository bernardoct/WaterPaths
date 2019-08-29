// $Id: Array.hh 249 2008-11-20 09:58:23Z schaerf $
// This file is part of EasyLocalpp: a C++ Object-Oriented framework
// aimed at easing the development of Local Search algorithms.
// Copyright (C) 2001--2008 Andrea Schaerf, Luca Di Gaspero. 
//
// This software may be modified and distributed under the terms
// of the MIT license.  See the LICENSE file for details.

#if !defined(_ARRAY_HH)
#define _ARRAY_HH

#include <set>
#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <cmath>
#include <cstdlib>

enum MType { DIAG };

template <typename T>
class Vector
{
public:
    Vector();
    Vector(const unsigned int n);
    Vector(const T& a, const unsigned int n); //initialize to constant value
    Vector(const T* a, const unsigned int n); // Initialize to array
    Vector(const Vector &rhs); // copy constructor
    ~Vector(); // destructor

    inline T& operator[](const unsigned int& i); //i-th element
    inline const T& operator[](const unsigned int& i) const;

    inline unsigned int size() const;
    inline void resize(const unsigned int n);
    inline void resize(const T& a, const unsigned int n);

    Vector<T>& operator=(const Vector<T>& rhs); //assignment
    Vector<T>& operator=(const T& a); //assign a to every element
    inline Vector<T>& operator+=(const Vector<T>& rhs);
    inline Vector<T>& operator-=(const Vector<T>& rhs);
    inline Vector<T>& operator*=(const Vector<T>& rhs);
    inline Vector<T>& operator/=(const Vector<T>& rhs);
    inline Vector<T>& operator^=(const Vector<T>& rhs);
    inline Vector<T>& operator+=(const T& a);
    inline Vector<T>& operator-=(const T& a);
    inline Vector<T>& operator*=(const T& a);
    inline Vector<T>& operator/=(const T& a);
    inline Vector<T>& operator^=(const T& a);
private:
    unsigned int n; // size of array. upper index is n-1
    T* v; // storage for data
};

template <typename T>
Vector<T>::Vector()
        : n(0), v(0)
{}

template <typename T>
Vector<T>::Vector(const unsigned int n)
        : v(new T[n])
{
    this->n = n;
}

template <typename T>
Vector<T>::Vector(const T& a, const unsigned int n)
        : v(new T[n])
{
    this->n = n;
    for (unsigned int i = 0; i < n; i++)
        v[i] = a;
}

template <typename T>
Vector<T>::Vector(const T* a, const unsigned int n)
        : v(new T[n])
{
    this->n = n;
    for (unsigned int i = 0; i < n; i++)
        v[i] = *a++;
}

template <typename T>
Vector<T>::Vector(const Vector<T>& rhs)
        : v(new T[rhs.n])
{
    this->n = rhs.n;
    for (unsigned int	i = 0; i < n; i++)
        v[i] = rhs[i];
}

template <typename T>
Vector<T>::~Vector()
{
    delete[] v;
}

template <typename T>
void Vector<T>::resize(const unsigned int n)
{
    if (n == this->n)
        return;

    delete[] (v);
    v = new T[n];

    this->n = n;
}

template <typename T>
void Vector<T>::resize(const T& a, const unsigned int n)
{
    resize(n);
    for (unsigned int i = 0; i < n; i++)
        v[i] = a;
}


template <typename T>
inline Vector<T>& Vector<T>::operator=(const Vector<T>& rhs)
// postcondition: normal assignment via copying has been performed;
// if vector and rhs were different sizes, vector
// has been resized to match the size of rhs
{
    if (this != &rhs)
    {
        resize(rhs.n);
        for (unsigned int i = 0; i < n; i++)
            v[i] = rhs[i];
    }
    return *this;
}

template <typename T>
inline Vector<T> & Vector<T>::operator=(const T& a) //assign a to every element
{
    for (unsigned int i = 0; i < n; i++)
        v[i] = a;
    return *this;
}

template <typename T>
inline T & Vector<T>::operator[](const unsigned int& i) //subscripting
{
    return v[i];
}

template <typename T>
inline const T& Vector<T>::operator[](const unsigned int& i) const //subscripting
{
    return v[i];
}

template <typename T>
inline unsigned int Vector<T>::size() const
{
    return n;
}

template <typename T>
inline Vector<T>& Vector<T>::operator+=(const Vector<T>& rhs)
{
    if (this->size() != rhs.size())
        throw std::logic_error("Operator+=: vectors have different sizes");
    for (unsigned int i = 0; i < n; i++)
        v[i] += rhs[i];

    return *this;
}


template <typename T>
inline Vector<T>& Vector<T>::operator+=(const T& a)
{
    for (unsigned int i = 0; i < n; i++)
        v[i] += a;

    return *this;
}

template <typename T>
inline Vector<T> operator+(const Vector<T>& rhs)
{
    return rhs;
}

template <typename T>
inline Vector<T> operator+(const Vector<T>& lhs, const Vector<T>& rhs)
{
    if (lhs.size() != rhs.size())
        throw std::logic_error("Operator+: vectors have different sizes");
    Vector<T> tmp(lhs.size());
    for (unsigned int i = 0; i < lhs.size(); i++)
        tmp[i] = lhs[i] + rhs[i];

    return tmp;
}

template <typename T>
inline Vector<T> operator+(const Vector<T>& lhs, const T& a)
{
    Vector<T> tmp(lhs.size());
    for (unsigned int i = 0; i < lhs.size(); i++)
        tmp[i] = lhs[i] + a;

    return tmp;
}

template <typename T>
inline Vector<T> operator+(const T& a, const Vector<T>& rhs)
{
    Vector<T> tmp(rhs.size());
    for (unsigned int i = 0; i < rhs.size(); i++)
        tmp[i] = a + rhs[i];

    return tmp;
}

template <typename T>
inline Vector<T>& Vector<T>::operator-=(const Vector<T>& rhs)
{
    if (this->size() != rhs.size())
        throw std::logic_error("Operator-=: vectors have different sizes");
    for (unsigned int i = 0; i < n; i++)
        v[i] -= rhs[i];

    return *this;
}


template <typename T>
inline Vector<T>& Vector<T>::operator-=(const T& a)
{
    for (unsigned int i = 0; i < n; i++)
        v[i] -= a;

    return *this;
}

template <typename T>
inline Vector<T> operator-(const Vector<T>& rhs)
{
    return (T)(-1) * rhs;
}

template <typename T>
inline Vector<T> operator-(const Vector<T>& lhs, const Vector<T>& rhs)
{
    if (lhs.size() != rhs.size())
        throw std::logic_error("Operator-: vectors have different sizes");
    Vector<T> tmp(lhs.size());
    for (unsigned int i = 0; i < lhs.size(); i++)
        tmp[i] = lhs[i] - rhs[i];

    return tmp;
}

template <typename T>
inline Vector<T> operator-(const Vector<T>& lhs, const T& a)
{
    Vector<T> tmp(lhs.size());
    for (unsigned int i = 0; i < lhs.size(); i++)
        tmp[i] = lhs[i] - a;

    return tmp;
}

template <typename T>
inline Vector<T> operator-(const T& a, const Vector<T>& rhs)
{
    Vector<T> tmp(rhs.size());
    for (unsigned int i = 0; i < rhs.size(); i++)
        tmp[i] = a - rhs[i];

    return tmp;
}

template <typename T>
inline Vector<T>& Vector<T>::operator*=(const Vector<T>& rhs)
{
    if (this->size() != rhs.size())
        throw std::logic_error("Operator*=: vectors have different sizes");
    for (unsigned int i = 0; i < n; i++)
        v[i] *= rhs[i];

    return *this;
}


template <typename T>
inline Vector<T>& Vector<T>::operator*=(const T& a)
{
    for (unsigned int i = 0; i < n; i++)
        v[i] *= a;

    return *this;
}

template <typename T>
inline Vector<T> operator*(const Vector<T>& lhs, const Vector<T>& rhs)
{
    if (lhs.size() != rhs.size())
        throw std::logic_error("Operator*: vectors have different sizes");
    Vector<T> tmp(lhs.size());
    for (unsigned int i = 0; i < lhs.size(); i++)
        tmp[i] = lhs[i] * rhs[i];

    return tmp;
}

template <typename T>
inline Vector<T> operator*(const Vector<T>& lhs, const T& a)
{
    Vector<T> tmp(lhs.size());
    for (unsigned int i = 0; i < lhs.size(); i++)
        tmp[i] = lhs[i] * a;

    return tmp;
}

template <typename T>
inline Vector<T> operator*(const T& a, const Vector<T>& rhs)
{
    Vector<T> tmp(rhs.size());
    for (unsigned int i = 0; i < rhs.size(); i++)
        tmp[i] = a * rhs[i];

    return tmp;
}

template <typename T>
inline Vector<T>& Vector<T>::operator/=(const Vector<T>& rhs)
{
    if (this->size() != rhs.size())
        throw std::logic_error("Operator/=: vectors have different sizes");
    for (unsigned int i = 0; i < n; i++)
        v[i] /= rhs[i];

    return *this;
}


template <typename T>
inline Vector<T>& Vector<T>::operator/=(const T& a)
{
    for (unsigned int i = 0; i < n; i++)
        v[i] /= a;

    return *this;
}

template <typename T>
inline Vector<T> operator/(const Vector<T>& lhs, const Vector<T>& rhs)
{
    if (lhs.size() != rhs.size())
        throw std::logic_error("Operator/: vectors have different sizes");
    Vector<T> tmp(lhs.size());
    for (unsigned int i = 0; i < lhs.size(); i++)
        tmp[i] = lhs[i] / rhs[i];

    return tmp;
}

template <typename T>
inline Vector<T> operator/(const Vector<T>& lhs, const T& a)
{
    Vector<T> tmp(lhs.size());
    for (unsigned int i = 0; i < lhs.size(); i++)
        tmp[i] = lhs[i] / a;

    return tmp;
}

template <typename T>
inline Vector<T> operator/(const T& a, const Vector<T>& rhs)
{
    Vector<T> tmp(rhs.size());
    for (unsigned int i = 0; i < rhs.size(); i++)
        tmp[i] = a / rhs[i];

    return tmp;
}

template <typename T>
inline Vector<T> operator^(const Vector<T>& lhs, const Vector<T>& rhs)
{
    if (lhs.size() != rhs.size())
        throw std::logic_error("Operator^: vectors have different sizes");
    Vector<T> tmp(lhs.size());
    for (unsigned int i = 0; i < lhs.size(); i++)
        tmp[i] = pow(lhs[i], rhs[i]);

    return tmp;
}

template <typename T>
inline Vector<T> operator^(const Vector<T>& lhs, const T& a)
{
    Vector<T> tmp(lhs.size());
    for (unsigned int i = 0; i < lhs.size(); i++)
        tmp[i] = pow(lhs[i], a);

    return tmp;
}

template <typename T>
inline Vector<T> operator^(const T& a, const Vector<T>& rhs)
{
    Vector<T> tmp(rhs.size());
    for (unsigned int i = 0; i < rhs.size(); i++)
        tmp[i] = pow(a, rhs[i]);

    return tmp;
}

template <typename T>
inline Vector<T>& Vector<T>::operator^=(const Vector<T>& rhs)
{
    if (this->size() != rhs.size())
        throw std::logic_error("Operator^=: vectors have different sizes");
    for (unsigned int i = 0; i < n; i++)
        v[i] = pow(v[i], rhs[i]);

    return *this;
}

template <typename T>
inline Vector<T>& Vector<T>::operator^=(const T& a)
{
    for (unsigned int i = 0; i < n; i++)
        v[i] = pow(v[i], a);

    return *this;
}

template <typename T>
inline bool operator==(const Vector<T>& v, const Vector<T>& w)
{
    if (v.size() != w.size())
        throw std::logic_error("Vectors of different size are not confrontable");
    for (unsigned i = 0; i < v.size(); i++)
        if (v[i] != w[i])
            return false;
    return true;
}

template <typename T>
inline bool operator!=(const Vector<T>& v, const Vector<T>& w)
{
    if (v.size() != w.size())
        throw std::logic_error("Vectors of different size are not confrontable");
    for (unsigned i = 0; i < v.size(); i++)
        if (v[i] != w[i])
            return true;
    return false;
}

template <typename T>
inline bool operator<(const Vector<T>& v, const Vector<T>& w)
{
    if (v.size() != w.size())
        throw std::logic_error("Vectors of different size are not confrontable");
    for (unsigned i = 0; i < v.size(); i++)
        if (v[i] >= w[i])
            return false;
    return true;
}

template <typename T>
inline bool operator<=(const Vector<T>& v, const Vector<T>& w)
{
    if (v.size() != w.size())
        throw std::logic_error("Vectors of different size are not confrontable");
    for (unsigned i = 0; i < v.size(); i++)
        if (v[i] > w[i])
            return false;
    return true;
}

template <typename T>
inline bool operator>(const Vector<T>& v, const Vector<T>& w)
{
    if (v.size() != w.size())
        throw std::logic_error("Vectors of different size are not confrontable");
    for (unsigned i = 0; i < v.size(); i++)
        if (v[i] <= w[i])
            return false;
    return true;
}

template <typename T>
inline bool operator>=(const Vector<T>& v, const Vector<T>& w)
{
    if (v.size() != w.size())
        throw std::logic_error("Vectors of different size are not confrontable");
    for (unsigned i = 0; i < v.size(); i++)
        if (v[i] < w[i])
            return false;
    return true;
}

/**
   Input/Output
*/
template <typename T>
inline std::ostream& operator<<(std::ostream& os, const Vector<T>& v)
{
    os << std::endl << v.size() << std::endl;
    for (unsigned int i = 0; i < v.size() - 1; i++)
        os << std::setw(20) << std::setprecision(16) << v[i] << ", ";
    os << std::setw(20) << std::setprecision(16) << v[v.size() - 1] << std::endl;

    return os;
}

template <typename T>
std::istream& operator>>(std::istream& is, Vector<T>& v)
{
    int elements;
    char comma;
    is >> elements;
    v.resize((const unsigned int) elements);
    for (unsigned int i = 0; i < elements; i++)
        is >> v[i] >> comma;

    return is;
}

/**
   Index utilities
*/


template <typename T>
inline unsigned int partition(Vector<T>& v, unsigned int begin, unsigned int end)
{
    unsigned int i = begin + 1, j = begin + 1;
    T pivot = v[begin];
    while (j <= end)
    {
        if (v[j] < pivot) {
            std::swap(v[i], v[j]);
            i++;
        }
        j++;
    }
    v[begin] = v[i - 1];
    v[i - 1] = pivot;
    return i - 2;
}


template <typename T>
inline void quicksort(Vector<T>& v, unsigned int begin, unsigned int end)
{
    if (end > begin)
    {
        unsigned int index = partition(v, begin, end);
        quicksort(v, begin, index);
        quicksort(v, index + 2, end);
    }
}

template <typename T>
class Matrix
{
public:
    Matrix(); // Default constructor
    Matrix(const unsigned int n, const unsigned int m); // Construct a n allocations_aux m matrix
    Matrix(const T& a, const unsigned int n, const unsigned int m); // Initialize the content to constant a
    Matrix(MType t, const T& a, const T& o, const unsigned int n, const unsigned int m);
    Matrix(MType t, const Vector<T>& v, const T& o, const unsigned int n, const unsigned int m);
    Matrix(const T* a, const unsigned int n, const unsigned int m); // Initialize to array
    Matrix(const Matrix<T>& rhs); // Copy constructor
    ~Matrix(); // destructor

    inline T* operator[](const unsigned int& i) { return v[i]; } // Subscripting: row i
    inline const T* operator[](const unsigned int& i) const { return v[i]; }; // const subsctipting

    inline void resize(const unsigned int n, const unsigned int m);
    inline void resize(const T& a, const unsigned int n, const unsigned int m);

    inline Vector<T> extractRow(const unsigned int i) const;
    inline Vector<T> extractColumn(const unsigned int j) const;


    inline unsigned int nrows() const { return n; } // number of rows
    inline unsigned int ncols() const { return m; } // number of columns

    inline Matrix<T>& operator=(const Matrix<T>& rhs); // Assignment operator
    inline Matrix<T>& operator=(const T& a); // Assign to every element value a
    inline Matrix<T>& operator+=(const Matrix<T>& rhs);
    inline Matrix<T>& operator-=(const Matrix<T>& rhs);
    inline Matrix<T>& operator*=(const Matrix<T>& rhs);
    inline Matrix<T>& operator/=(const Matrix<T>& rhs);
    inline Matrix<T>& operator^=(const Matrix<T>& rhs);
    inline Matrix<T>& operator+=(const T& a);
    inline Matrix<T>& operator-=(const T& a);
    inline Matrix<T>& operator*=(const T& a);
    inline Matrix<T>& operator/=(const T& a);
    inline Matrix<T>& operator^=(const T& a);
    inline operator Vector<T>();
private:
    unsigned int n; // number of rows
    unsigned int m; // number of columns
    T **v; // storage for data
};

template <typename T>
Matrix<T>::Matrix()
        : n(0), m(0), v(0)
{}

template <typename T>
Matrix<T>::Matrix(unsigned int n, unsigned int m)
        : v(new T*[n])
{
    register unsigned int i;
    this->n = n; this->m = m;
    v[0] = new T[m * n];
    for (i = 1; i < n; i++)
        v[i] = v[i - 1] + m;
}

template <typename T>
Matrix<T>::Matrix(const T& a, unsigned int n, unsigned int m)
        : v(new T*[n])
{
    register unsigned int i, j;
    this->n = n; this->m = m;
    v[0] = new T[m * n];
    for (i = 1; i < n; i++)
        v[i] = v[i - 1] + m;
    for (i = 0; i < n; i++)
        for (j = 0; j < m; j++)
            v[i][j] = a;
}

template <class T>
Matrix<T>::Matrix(const T* a, unsigned int n, unsigned int m)
        : v(new T*[n])
{
    register unsigned int i, j;
    this->n = n; this->m = m;
    v[0] = new T[m * n];
    for (i = 1; i < n; i++)
        v[i] = v[i - 1] + m;
    for (i = 0; i < n; i++)
        for (j = 0; j < m; j++)
            v[i][j] = *a++;
}

template <class T>
Matrix<T>::Matrix(MType t, const T& a, const T& o, unsigned int n, unsigned int m)
        : v(new T*[n])
{
    register unsigned int i, j;
    this->n = n; this->m = m;
    v[0] = new T[m * n];
    for (i = 1; i < n; i++)
        v[i] = v[i - 1] + m;
    switch (t)
    {
        case DIAG:
            for (i = 0; i < n; i++)
                for (j = 0; j < m; j++)
                    if (i != j)
                        v[i][j] = o;
                    else
                        v[i][j] = a;
            break;
        default:
            throw std::logic_error("Matrix type not supported");
    }
}

template <class T>
Matrix<T>::Matrix(MType t, const Vector<T>& a, const T& o, unsigned int n, unsigned int m)
        : v(new T*[n])
{
    register unsigned int i, j;
    this->n = n; this->m = m;
    v[0] = new T[m * n];
    for (i = 1; i < n; i++)
        v[i] = v[i - 1] + m;
    switch (t)
    {
        case DIAG:
            for (i = 0; i < n; i++)
                for (j = 0; j < m; j++)
                    if (i != j)
                        v[i][j] = o;
                    else
                        v[i][j] = a[i];
            break;
        default:
            throw std::logic_error("Matrix type not supported");
    }
}

template <typename T>
Matrix<T>::Matrix(const Matrix<T>& rhs)
        : v(new T*[rhs.n])
{
    register unsigned int i, j;
    n = rhs.n; m = rhs.m;
    v[0] = new T[m * n];
    for (i = 1; i < n; i++)
        v[i] = v[i - 1] + m;
    for (i = 0; i < n; i++)
        for (j = 0; j < m; j++)
            v[i][j] = rhs[i][j];
}

template <typename T>
Matrix<T>::~Matrix()
{
    if (v != 0) {
        delete[] (v[0]);
        delete[] (v);
    }
}

template <typename T>
inline Matrix<T>& Matrix<T>::operator=(const Matrix<T> &rhs)
// postcondition: normal assignment via copying has been performed;
// if matrix and rhs were different sizes, matrix
// has been resized to match the size of rhs
{
    register unsigned int i, j;
    if (this != &rhs)
    {
        resize(rhs.n, rhs.m);
        for (i = 0; i < n; i++)
            for (j = 0; j < m; j++)
                v[i][j] = rhs[i][j];
    }
    return *this;
}

template <typename T>
inline Matrix<T>& Matrix<T>::operator=(const T& a) // assign a to every element
{
    register unsigned int i, j;
    for (i = 0; i < n; i++)
        for (j = 0; j < m; j++)
            v[i][j] = a;
    return *this;
}


template <typename T>
inline void Matrix<T>::resize(const unsigned int n, const unsigned int m)
{
    register unsigned int i;
    if (n == this->n && m == this->m)
        return;
    if (v != 0)
    {
        delete[] (v[0]);
        delete[] (v);
    }
    this->n = n; this->m = m;
    v = new T*[n];
    v[0] = new T[m * n];
    for (i = 1; i < n; i++)
        v[i] = v[i - 1] + m;
}

template <typename T>
inline void Matrix<T>::resize(const T& a, const unsigned int n, const unsigned int m)
{
    register unsigned int i, j;
    resize(n, m);
    for (i = 0; i < n; i++)
        for (j = 0; j < m; j++)
            v[i][j] = a;
}

template <typename T>
inline Vector<T> Matrix<T>::extractRow(const unsigned int i) const
{
    if (i >= n)
        throw std::logic_error("Error in extractRow: trying to extract a row out of matrix bounds");
    Vector<T> tmp(v[i], m);

    return tmp;
}

template <typename T>
inline Vector<T> Matrix<T>::extractColumn(const unsigned int j) const
{
    register unsigned int i;
    if (j >= m)
        throw std::logic_error("Error in extractRow: trying to extract a row out of matrix bounds");
    Vector<T> tmp(n);

    for (i = 0; i < n; i++)
        tmp[i] = v[i][j];

    return tmp;
}


template <typename T>
Matrix<T> operator+(const Matrix<T>& rhs)
{
    return rhs;
}

template <typename T>
Matrix<T> operator+(const Matrix<T>& lhs, const Matrix<T>& rhs)
{
    if (lhs.ncols() != rhs.ncols() || lhs.nrows() != rhs.nrows())
        throw std::logic_error("Operator+: matrices have different sizes");
    Matrix<T> tmp(lhs.nrows(), lhs.ncols());
    for (unsigned int i = 0; i < lhs.nrows(); i++)
        for (unsigned int j = 0; j < lhs.ncols(); j++)
            tmp[i][j] = lhs[i][j] + rhs[i][j];

    return tmp;
}

template <typename T>
Matrix<T> operator+(const Matrix<T>& lhs, const T& a)
{
    Matrix<T> tmp(lhs.nrows(), lhs.ncols());
    for (unsigned int i = 0; i < lhs.nrows(); i++)
        for (unsigned int j = 0; j < lhs.ncols(); j++)
            tmp[i][j] = lhs[i][j] + a;

    return tmp;
}

template <typename T>
Matrix<T> operator+(const T& a, const Matrix<T>& rhs)
{
    Matrix<T> tmp(rhs.nrows(), rhs.ncols());
    for (unsigned int i = 0; i < rhs.nrows(); i++)
        for (unsigned int j = 0; j < rhs.ncols(); j++)
            tmp[i][j] = a + rhs[i][j];

    return tmp;
}

template <typename T>
inline Matrix<T>& Matrix<T>::operator+=(const Matrix<T>& rhs)
{
    if (m != rhs.ncols() || n != rhs.nrows())
        throw std::logic_error("Operator+=: matrices have different sizes");
    for (unsigned int i = 0; i < n; i++)
        for (unsigned int j = 0; j < m; j++)
            v[i][j] += rhs[i][j];

    return *this;
}

template <typename T>
inline Matrix<T>& Matrix<T>::operator+=(const T& a)
{
    for (unsigned int i = 0; i < n; i++)
        for (unsigned int j = 0; j < m; j++)
            v[i][j] += a;

    return *this;
}

template <typename T>
Matrix<T> operator-(const Matrix<T>& rhs)
{
    return (T)(-1) * rhs;
}

template <typename T>
Matrix<T> operator-(const Matrix<T>& lhs, const Matrix<T>& rhs)
{
    if (lhs.ncols() != rhs.ncols() || lhs.nrows() != rhs.nrows())
        throw std::logic_error("Operator-: matrices have different sizes");
    Matrix<T> tmp(lhs.nrows(), lhs.ncols());
    for (unsigned int i = 0; i < lhs.nrows(); i++)
        for (unsigned int j = 0; j < lhs.ncols(); j++)
            tmp[i][j] = lhs[i][j] - rhs[i][j];

    return tmp;
}

template <typename T>
Matrix<T> operator-(const Matrix<T>& lhs, const T& a)
{
    Matrix<T> tmp(lhs.nrows(), lhs.ncols());
    for (unsigned int i = 0; i < lhs.nrows(); i++)
        for (unsigned int j = 0; j < lhs.ncols(); j++)
            tmp[i][j] = lhs[i][j] - a;

    return tmp;
}

template <typename T>
Matrix<T> operator-(const T& a, const Matrix<T>& rhs)
{
    Matrix<T> tmp(rhs.nrows(), rhs.ncols());
    for (unsigned int i = 0; i < rhs.nrows(); i++)
        for (unsigned int j = 0; j < rhs.ncols(); j++)
            tmp[i][j] = a - rhs[i][j];

    return tmp;
}

template <typename T>
inline Matrix<T>& Matrix<T>::operator-=(const Matrix<T>& rhs)
{
    if (m != rhs.ncols() || n != rhs.nrows())
        throw std::logic_error("Operator-=: matrices have different sizes");
    for (unsigned int i = 0; i < n; i++)
        for (unsigned int j = 0; j < m; j++)
            v[i][j] -= rhs[i][j];

    return *this;
}

template <typename T>
inline Matrix<T>& Matrix<T>::operator-=(const T& a)
{
    for (unsigned int i = 0; i < n; i++)
        for (unsigned int j = 0; j < m; j++)
            v[i][j] -= a;

    return *this;
}

template <typename T>
Matrix<T> operator*(const Matrix<T>& lhs, const Matrix<T>& rhs)
{
    if (lhs.ncols() != rhs.ncols() || lhs.nrows() != rhs.nrows())
        throw std::logic_error("Operator*: matrices have different sizes");
    Matrix<T> tmp(lhs.nrows(), lhs.ncols());
    for (unsigned int i = 0; i < lhs.nrows(); i++)
        for (unsigned int j = 0; j < lhs.ncols(); j++)
            tmp[i][j] = lhs[i][j] * rhs[i][j];

    return tmp;
}

template <typename T>
Matrix<T> operator*(const Matrix<T>& lhs, const T& a)
{
    Matrix<T> tmp(lhs.nrows(), lhs.ncols());
    for (unsigned int i = 0; i < lhs.nrows(); i++)
        for (unsigned int j = 0; j < lhs.ncols(); j++)
            tmp[i][j] = lhs[i][j] * a;

    return tmp;
}

template <typename T>
Matrix<T> operator*(const T& a, const Matrix<T>& rhs)
{
    Matrix<T> tmp(rhs.nrows(), rhs.ncols());
    for (unsigned int i = 0; i < rhs.nrows(); i++)
        for (unsigned int j = 0; j < rhs.ncols(); j++)
            tmp[i][j] = a * rhs[i][j];

    return tmp;
}

template <typename T>
inline Matrix<T>& Matrix<T>::operator*=(const Matrix<T>& rhs)
{
    if (m != rhs.ncols() || n != rhs.nrows())
        throw std::logic_error("Operator*=: matrices have different sizes");
    for (unsigned int i = 0; i < n; i++)
        for (unsigned int j = 0; j < m; j++)
            v[i][j] *= rhs[i][j];

    return *this;
}

template <typename T>
inline Matrix<T>& Matrix<T>::operator*=(const T& a)
{
    for (unsigned int i = 0; i < n; i++)
        for (unsigned int j = 0; j < m; j++)
            v[i][j] *= a;

    return *this;
}

template <typename T>
Matrix<T> operator/(const Matrix<T>& lhs, const Matrix<T>& rhs)
{
    if (lhs.ncols() != rhs.ncols() || lhs.nrows() != rhs.nrows())
        throw std::logic_error("Operator+: matrices have different sizes");
    Matrix<T> tmp(lhs.nrows(), lhs.ncols());
    for (unsigned int i = 0; i < lhs.nrows(); i++)
        for (unsigned int j = 0; j < lhs.ncols(); j++)
            tmp[i][j] = lhs[i][j] / rhs[i][j];

    return tmp;
}

template <typename T>
Matrix<T> operator/(const Matrix<T>& lhs, const T& a)
{
    Matrix<T> tmp(lhs.nrows(), lhs.ncols());
    for (unsigned int i = 0; i < lhs.nrows(); i++)
        for (unsigned int j = 0; j < lhs.ncols(); j++)
            tmp[i][j] = lhs[i][j] / a;

    return tmp;
}

template <typename T>
Matrix<T> operator/(const T& a, const Matrix<T>& rhs)
{
    Matrix<T> tmp(rhs.nrows(), rhs.ncols());
    for (unsigned int i = 0; i < rhs.nrows(); i++)
        for (unsigned int j = 0; j < rhs.ncols(); j++)
            tmp[i][j] = a / rhs[i][j];

    return tmp;
}

template <typename T>
inline Matrix<T>& Matrix<T>::operator/=(const Matrix<T>& rhs)
{
    if (m != rhs.ncols() || n != rhs.nrows())
        throw std::logic_error("Operator+=: matrices have different sizes");
    for (unsigned int i = 0; i < n; i++)
        for (unsigned int j = 0; j < m; j++)
            v[i][j] /= rhs[i][j];

    return *this;
}

template <typename T>
inline Matrix<T>& Matrix<T>::operator/=(const T& a)
{
    for (unsigned int i = 0; i < n; i++)
        for (unsigned int j = 0; j < m; j++)
            v[i][j] /= a;

    return *this;
}

template <typename T>
Matrix<T> operator^(const Matrix<T>& lhs, const T& a)
{
    Matrix<T> tmp(lhs.nrows(), lhs.ncols());
    for (unsigned int i = 0; i < lhs.nrows(); i++)
        for (unsigned int j = 0; j < lhs.ncols(); j++)
            tmp[i][j] = pow(lhs[i][j], a);

    return tmp;
}

template <typename T>
inline Matrix<T>& Matrix<T>::operator^=(const Matrix<T>& rhs)
{
    if (m != rhs.ncols() || n != rhs.nrows())
        throw std::logic_error("Operator^=: matrices have different sizes");
    for (unsigned int i = 0; i < n; i++)
        for (unsigned int j = 0; j < m; j++)
            v[i][j] = pow(v[i][j], rhs[i][j]);

    return *this;
}


template <typename T>
inline Matrix<T>& Matrix<T>::operator^=(const T& a)
{
    for (unsigned int i = 0; i < n; i++)
        for (unsigned int j = 0; j < m; j++)
            v[i][j] = pow(v[i][j], a);

    return *this;
}

template <typename T>
inline Matrix<T>::operator Vector<T>()
{
    if (n > 1 && m > 1)
        throw std::logic_error("Error matrix cast to vector: trying to cast a multi-dimensional matrix");
    if (n == 1)
        return extractRow(0);
    else
        return extractColumn(0);
}

template <typename T>
inline bool operator==(const Matrix<T>& a, const Matrix<T>& b)
{
    if (a.nrows() != b.nrows() || a.ncols() != b.ncols())
        throw std::logic_error("Matrices of different size are not confrontable");
    for (unsigned i = 0; i < a.nrows(); i++)
        for (unsigned j = 0; j < a.ncols(); j++)
            if (a[i][j] != b[i][j])
                return false;
    return true;
}

template <typename T>
inline bool operator!=(const Matrix<T>& a, const Matrix<T>& b)
{
    if (a.nrows() != b.nrows() || a.ncols() != b.ncols())
        throw std::logic_error("Matrices of different size are not confrontable");
    for (unsigned i = 0; i < a.nrows(); i++)
        for (unsigned j = 0; j < a.ncols(); j++)
            if (a[i][j] != b[i][j])
                return true;
    return false;
}



/**
   Input/Output
*/
template <typename T>
std::ostream& operator<<(std::ostream& os, const Matrix<T>& m)
{
    os << std::endl << m.nrows() << " " << m.ncols() << std::endl;
    for (unsigned int i = 0; i < m.nrows(); i++)
    {
        for (unsigned int j = 0; j < m.ncols() - 1; j++)
            os << std::setw(20) << std::setprecision(16) << m[i][j] << ", ";
        os << std::setw(20) << std::setprecision(16) << m[i][m.ncols() - 1] << std::endl;
    }

    return os;
}

template <typename T>
std::istream& operator>>(std::istream& is, Matrix<T>& m)
{
    unsigned int rows, cols;
    char comma;
    is >> rows >> cols;
    m.resize(rows, cols);
    for (unsigned int i = 0; i < rows; i++)
        for (unsigned int j = 0; j < cols; j++)
            is >> m[i][j] >> comma;

    return is;
}

template <typename T>
Matrix<T> t(const Matrix<T>& a)
{
    Matrix<T> tmp(a.ncols(), a.nrows());
    for (unsigned int i = 0; i < a.nrows(); i++)
        for (unsigned int j = 0; j < a.ncols(); j++)
            tmp[j][i] = a[i][j];

    return tmp;
}

template <typename T>
void cholesky_solve(const Matrix<T>& LL, Vector<T>& x, const Vector<T>& b)
{
    x = cholesky_solve(LL, b);
}

template <typename T>
void forward_elimination(const Matrix<T>& L, Vector<T>& y, const Vector<T> b)
{
    if (L.ncols() != L.nrows())
        throw std::logic_error("Error in Forward elimination: matrix must be squared (lower triangular)");
    if (b.size() != L.nrows())
        throw std::logic_error("Error in Forward elimination: b vector must be of the same dimensions of L matrix");
    register int i, j, n = b.size();
    y.resize((const unsigned int) n);

    y[0] = b[0] / L[0][0];
    for (i = 1; i < n; i++)
    {
        y[i] = b[i];
        for (j = 0; j < i; j++)
            y[i] -= L[i][j] * y[j];
        y[i] = y[i] / L[i][i];
    }
}

template <typename T>
void backward_elimination(const Matrix<T>& U, Vector<T>& x, const Vector<T>& y)
{
    if (U.ncols() != U.nrows())
        throw std::logic_error("Error in Backward elimination: matrix must be squared (upper triangular)");
    if (y.size() != U.nrows())
        throw std::logic_error("Error in Backward elimination: b vector must be of the same dimensions of U matrix");
    register int i, j, n = y.size();
    x.resize((const unsigned int) n);

    x[n - 1] = y[n - 1] / U[n - 1][n - 1];
    for (i = n - 2; i >= 0; i--)
    {
        x[i] = y[i];
        for (j = i + 1; j < n; j++)
            x[i] -= U[i][j] * x[j];
        x[i] = x[i] / U[i][i];
    }
}

template <typename T>
Vector<T> cholesky_solve(const Matrix<T>& LL, const Vector<T>& b)
{
    if (LL.ncols() != LL.nrows())
        throw std::logic_error("Error in Cholesky solve: matrix must be squared");
    unsigned int n = LL.ncols();
    if (b.size() != n)
        throw std::logic_error("Error in Cholesky decomposition: b vector must be of the same dimensions of LU matrix");
    Vector<T> x, y;

    /* Solve L * y = b */
    forward_elimination(LL, y, b);
    /* Solve L^T * allocations_aux = y */
    backward_elimination(LL, x, y);

    return x;
}

#endif // define _ARRAY_HH_
