//
// Created by bernardoct on 6/7/17.
//

#ifndef TRIANGLEMODEL_MATRICES_H
#define TRIANGLEMODEL_MATRICES_H



#include <iostream>
#include <cstring>

template<typename T>
class Matrix2D {
private:
    int di_, dj_;
    T *data_;
    bool not_initialized = true;
public:
    Matrix2D();

    Matrix2D(int di, int dj);

    T &operator()(int i, int j);        // Subscript operators often come in pairs
    T operator()(int di, int d) const;  // Subscript operators often come in pairs
    // ...
    ~Matrix2D();                              // Destructor
    Matrix2D(const Matrix2D &m);               // Copy constructor
    Matrix2D<T> &operator=(const Matrix2D<T> &m);   // Assignment operator
    Matrix2D<T> &operator+=(const Matrix2D<T> &m);

    Matrix2D<T> &operator/(const double m);

    void setData(T *data);

    void reset(T value);

    void print() const;

    int get_i();

    int get_j();

    bool empty();
};


template<typename T>
Matrix2D<T>::Matrix2D(int di, int dj) : di_(di), dj_(dj) {
    if (di == 0 || dj == 0)
        std::__throw_length_error("Matrix2D constructor has 0 size");
    data_ = new T[di * dj];
    memset(data_, 0, sizeof(T) * di_ * dj_);
}

template<typename T>
Matrix2D<T>::Matrix2D(const Matrix2D<T> &m) : di_(m.di_), dj_(m.dj_), not_initialized(m.not_initialized) {
    data_ = new T[di_ * dj_];
    std::copy(m.data_, m.data_ + di_ * dj_, data_);
}

template<class T>
Matrix2D<T>::Matrix2D() {}

template<class T>
Matrix2D<T>::~Matrix2D() {
    delete[] data_;
}

template<typename T>
Matrix2D<T> &Matrix2D<T>::operator=(const Matrix2D<T> &m) {
    di_ = m.di_;
    dj_ = m.dj_;
    not_initialized = m.not_initialized;
    data_ = new T[di_ * dj_];
    std::copy(m.data_, m.data_ + di_ * dj_, data_);
    return *this;
}

template<typename T>
Matrix2D<T> &Matrix2D<T>::operator+=(const Matrix2D<T> &m) {

    if (m.di_ != di_ || m.dj_ != dj_)
        std::__throw_length_error("Matrixes of different sizes cannot be added.");

    for (int i = 0; i < di_ * dj_; ++i) {
        data_[i] += m.data_[i];
    }
    return *this;
}

template<typename T>
Matrix2D<T> &Matrix2D<T>::operator/(const double n) {

    for (int i = 0; i < di_ * dj_; ++i) {
        data_[i] /= n;
    }
    return *this;
}

template<typename T>
T &Matrix2D<T>::operator()(int i, int j) {
    if (i >= di_ || j >= dj_)
        std::__throw_length_error("Matrix2D subscript out of bounds");
    not_initialized = false;
    return data_[dj_ * i + j];
}

template<typename T>
T Matrix2D<T>::operator()(int i, int j) const {
    if (i >= di_ || j >= dj_)
        std::__throw_length_error("Matrix2D subscript out of bounds");
    return data_[dj_ * i + j];
}

template<typename T>
void Matrix2D<T>::reset(T value) {
    not_initialized = false;
    memset(data_, value, sizeof(T) * di_ * dj_);
}

template<typename T>
void Matrix2D<T>::print() const {
    for (int i = 0; i < di_; ++i) {
        for (int j = 0; j < dj_; ++j) {
            std::cout << data_[dj_ * i + j] << " ";
        }
        std::cout << std::endl;
    }
}

template<typename T>
void Matrix2D<T>::setData(T *data) {
    not_initialized = false;
    data_ = data;
}

template<typename T>
int Matrix2D<T>::get_i() {
    return di_;
}

template<typename T>
int Matrix2D<T>::get_j() {
    return dj_;
}

template<typename T>
bool Matrix2D<T>::empty() {
    return not_initialized;
}

template<typename T>
class Matrix3D {
private:
    int di_, dj_, dk_;
    T *data_;
public:
    Matrix3D();

    Matrix3D(int di, int dj, int dk);

    T &operator()(int i, int j, int k);        // Subscript operators often come in pairs
    T operator()(int di, int dj, int dk) const;  // Subscript operators often come in pairs
    // ...
    ~Matrix3D();                              // Destructor
    Matrix3D(const Matrix3D &m);               // Copy constructor
    Matrix3D<T> &operator=(const Matrix3D<T> &m);   // Assignment operator
    Matrix3D<T> &operator+=(const Matrix3D<T> &m);

    Matrix3D<T> &operator/(const double m);

    Matrix2D<T> get2D(int ijk, char dim);

    void reset(T value);

    void print(int k) const;

    int get_i() const;

    int get_j() const;

    int get_k() const;
};


template<typename T>
Matrix3D<T>::Matrix3D(int di, int dj, int dk) : di_(di), dj_(dj), dk_(dk)
//, data_ ← initialized below after the if...throw statement
{
    if (di == 0 || dj == 0 || dk == 0)
        std::__throw_length_error("Matrix3D constructor has 0 size");
    data_ = new T[di * dj * dk];
    memset(data_, 0, sizeof(T) * di_ * dj_ * dk_);
}

template<typename T>
Matrix3D<T>::Matrix3D(const Matrix3D<T> &m) : di_(m.di_), dj_(m.dj_), dk_(m.dk_) {
    data_ = new T[di_ * dj_ * dk_];
    std::copy(m.data_, m.data_ + di_ * dj_ * dk_, data_);
}

template<typename T>
Matrix3D<T>::Matrix3D() {}

template<typename T>
Matrix3D<T>::~Matrix3D() {
    delete[] data_;
}

template<typename T>
Matrix3D<T> &Matrix3D<T>::operator=(const Matrix3D<T> &m) {
    di_ = m.di_;
    dj_ = m.dj_;
    dk_ = m.dk_;
    data_ = new T[di_ * dj_ * dk_];
    std::copy(m.data_, m.data_ + di_ * dj_ * dk_, data_);
    return *this;
}

template<typename T>
Matrix3D<T> &Matrix3D<T>::operator+=(const Matrix3D<T> &m) {

    if (m.di_ != di_ || m.dj_ != dj_ || m.dk_ != dk_)
        std::__throw_length_error("Matrixes of different sizes cannot be added.");

    for (int i = 0; i < di_ * dj_ * dk_; ++i) {
        data_[i] += m.data_[i];
    }
    return *this;
}

template<typename T>
Matrix3D<T> &Matrix3D<T>::operator/(const double n) {

    for (int i = 0; i < di_ * dj_ * dk_; ++i) {
        data_[i] /= n;
    }
    return *this;
}

template<typename T>
T &Matrix3D<T>::operator()(int i, int j, int k) {
    if (i >= di_ || j >= dj_ || k >= dk_)
        std::__throw_length_error("Matrix3D subscript out of bounds");
    return data_[dj_ * dk_ * i + dk_ * j + k];
}

template<typename T>
T Matrix3D<T>::operator()(int i, int j, int k) const {
    if (i >= di_ || j >= dj_ || k >= dk_)
        std::__throw_length_error("Matrix3D subscript out of bounds");
    return data_[dj_ * dk_ * i + dk_ * j + k];
}

template<typename T>
void Matrix3D<T>::reset(T value) {
    memset(data_, value, sizeof(T) * di_ * dj_ * dk_);
}

template<typename T>
void Matrix3D<T>::print(int k) const {
    for (int i = 0; i < di_; ++i) {
        for (int j = 0; j < dj_; ++j) {
            std::cout << data_[dj_ * dk_ * i + dk_ * j + k] << " ";
        }
        std::cout << std::endl;
    }
}

template<typename T>
Matrix2D<T> Matrix3D<T>::get2D(int ijk, char dim) {
    T *data2D;
    Matrix2D<T> m;

    if (dim == 'k') {
        data2D = new T[di_ * dj_];
        for (int i = 0; i < di_; ++i) {
            for (int j = 0; j < dj_; ++j) {
                data2D[dj_ * i + j] = data_[dj_ * dk_ * i + dk_ * j + ijk];
            }
        }

        m = Matrix2D<T>(dj_, dk_);
        m.setData(data2D);

    } else if (dim == 'i') {
        data2D = new T[dj_ * dk_];
        for (int j = 0; j < dj_; ++j) {
            for (int k = 0; k < dk_; ++k) {
                data2D[dk_ * j + k] = data_[dj_ * dk_ * ijk + dk_ * j + k];
            }
        }

        m = Matrix2D<T>(di_, dk_);
        m.setData(data2D);

    } else if (dim == 'j') {
        data2D = new T[dj_ * dk_];
        for (int i = 0; i < di_; ++i) {
            for (int k = 0; k < dk_; ++k) {
                data2D[dk_ * i + +k] = data_[dj_ * dk_ * i + dk_ * ijk + k];
            }
        }

        m = Matrix2D<T>(di_, dj_);
        m.setData(data2D);
    } else
        std::__throw_invalid_argument("the first argument must be either one of chars 'i', 'j' or 'k.'");

//    delete[] data2D;

    return m;
}

template<typename T>
int Matrix3D<T>::get_i() const {
    return di_;
}

template<typename T>
int Matrix3D<T>::get_j() const {
    return dj_;
}

template<typename T>
int Matrix3D<T>::get_k() const {
    return dk_;
}


#endif //TRIANGLEMODEL_MATRICES_H
