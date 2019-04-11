//
// Created by bernardoct on 6/7/17.
//

#ifndef TRIANGLEMODEL_MATRICES_H
#define TRIANGLEMODEL_MATRICES_H



#include <iostream>
#include <cstring>
#include <iostream>
#include <fstream>
#include <memory>

template<typename T>
class Matrix2D {
private:
    int di_, dj_;
    unique_ptr<T[]> data_;
    bool not_initialized = true;
public:
    Matrix2D();

    Matrix2D(int di, int dj);

    T &operator()(int i, int j);        // Subscript operators often come in pairs
    T operator()(int i, int j) const;  // Subscript operators often come in pairs
    // ...
    ~Matrix2D();                              // Destructor
    Matrix2D(const Matrix2D &m);               // Copy constructor
    Matrix2D<T> &operator=(const Matrix2D<T> &m);   // Assignment operator
    Matrix2D<T> &operator+=(const Matrix2D<T> &m);

    Matrix2D<T> &operator/(const double m);

    void reset(T value);

    void print(int i) const;

    int get_i();

    int get_j();

    bool empty();

    void setPartialData(int i, T *data, int length);

    void setData(T *data, int length);

    T *getPointerToElement(int i, int j) const;

    void add_to_position(int i, int j, T *data, int length);

    const vector<vector<T>> get_vector() const;
};

template<typename T>
Matrix2D<T>::Matrix2D(int di, int dj) : di_(di), dj_(dj) {
    if (di == 0 || dj == 0)
        throw length_error("Matrix2D constructor has 0 size");
    data_ = unique_ptr<T[]>(new T[di * dj]);
    fill_n(data_.get(), di_ * dj_, 0);
}

template<typename T>
Matrix2D<T>::Matrix2D(const Matrix2D<T> &m) : di_(m.di_), dj_(m.dj_), not_initialized(m.not_initialized) {
    if (di_ == 0 || dj_ == 0)
        throw length_error("Matrix2D dimensions has 0 size");
    data_ = unique_ptr<T[]>(new T[di_ * dj_]);
    std::copy(m.data_.get(), m.data_.get() + di_ * dj_, data_.get());
}

template<typename T>
Matrix2D<T>::Matrix2D() {}

template<typename T>
Matrix2D<T>::~Matrix2D() {}

template<typename T>
Matrix2D<T> &Matrix2D<T>::operator=(const Matrix2D<T> &m) {
    di_ = m.di_;
    dj_ = m.dj_;
    if (di_ == 0 || dj_ == 0)
        throw length_error("Matrix2D dimensions has 0 size");
    data_ = unique_ptr<T[]>(new T[di_ * dj_]);
    std::copy(m.data_.get(), m.data_.get() + di_ * dj_, data_.get());
    return *this;
}

template<typename T>
Matrix2D<T> &Matrix2D<T>::operator+=(const Matrix2D<T> &m) {

    if (m.di_ != di_ || m.dj_ != dj_)
        throw length_error("Matrixes of different sizes cannot be added.");

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
    if (i >= di_ || j >= dj_) {
        string error_message = "Matrix3D subscript out of bounds.\ni=" +
                               to_string(i) + " (>=" + to_string(di_) + "?)\nj=" +
                               to_string(j) + " (>" + to_string(dj_) + "?)";
        std::throw_with_nested(std::length_error(error_message.c_str()));
    }
    return data_[dj_ * i + j];
}

template<typename T>
T Matrix2D<T>::operator()(int i, int j) const {
    if (i >= di_ || j >= dj_) {
        string error_message = "Matrix3D subscript out of bounds.\ni=" +
                               to_string(i) + " (>=" + to_string(di_) + "?)\nj=" +
                               to_string(j) + " (>" + to_string(dj_) + "?)";
        std::throw_with_nested(std::length_error(error_message.c_str()));
    }
    return data_[dj_ * i + j];
}

template<typename T>
void Matrix2D<T>::print(int i) const {
    for (int j = 0; j < dj_; ++j) {
        printf("%0.2f ", data_[dj_ * i + j]);
    }
    printf("\n");
}

template<typename T>
void Matrix2D<T>::setData(T *data, int length) {
    if (length != di_ * dj_) {
        string er = "Size of data does not match that of matrix: " +
                    to_string(length) + " vs. " + to_string(di_*dj_);
        throw_with_nested(invalid_argument(er.c_str()));
    }
    memcpy(data_.get(), data, length * sizeof(T));
}

template<typename T>
void Matrix2D<T>::setPartialData(int i, T *data, int length) {
    if (i >= di_ + length)
        throw length_error("Matrix3D subscript out of bounds or negative");
    memcpy(data_.get() + i * dj_, data, length * sizeof(T));
}

template<typename T>
T *Matrix2D<T>::getPointerToElement(int i, int j) const {
    return data_.get() + i * dj_ + j;
}

template<typename T>
void Matrix2D<T>::reset(T value) {
    fill_n(data_.get(), di_ * dj_, value);
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
void Matrix2D<T>::add_to_position(int i, int j, T *data,
                                  int length) {
    int pos0 = i * dj_ + j;
    for (int p = 0; p < length; ++p) {
        data_[pos0 + p] += data[p];
    }
}

template<typename T>
const vector<vector<T>> Matrix2D<T>::get_vector() const {
    vector<vector<T>> vector_matrix;
    for (int i = 0; i < di_; ++i) {
        vector<T> row;
        for (int j = 0; j < dj_; ++j) {
            row.push_back(data_[dj_ * i + j]);
        }
        vector_matrix.push_back(row);
    }
    return vector_matrix;
}

template<typename T>
class Matrix3D {
private:
    int di_ = NON_INITIALIZED, dj_ = NON_INITIALIZED, dk_ = NON_INITIALIZED;
    unique_ptr<T[]> data_;
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

    void add_to_position(int i, int j, int k, T* data, int length);

    void setPartialData(int i, int j, T *data, int length);

    T* getPointerToElement(int i, int j, int k) const;

    void reset(T value);

    void print(int i) const;

    void setData(T *data, int length);

    int get_i() const;

    int get_j() const;

    int get_k() const;

    bool empty() const;
};

template<typename T>
Matrix3D<T>::Matrix3D(int di, int dj, int dk) : di_(di), dj_(dj), dk_(dk)
{
    if (di == 0 || dj == 0 || dk == 0)
        throw length_error("Matrix3D dimensions has 0 size");
    data_ = unique_ptr<T[]>(new T[di * dj * dk]);
    fill_n(data_.get(), di_ * dj_ * dk_, 0);
}

template<typename T>
Matrix3D<T>::Matrix3D(const Matrix3D<T> &m) : di_(m.di_), dj_(m.dj_), dk_(m.dk_) {
    if (di_ == 0 || dj_ == 0 || dk_ == 0)
        throw length_error("Matrix3D dimensions has 0 size");
    data_ = unique_ptr<T[]>(new T[di_ * dj_ * dk_]);
    std::copy(m.data_.get(), m.data_.get() + di_ * dj_ * dk_, data_.get());
}

template<typename T>
Matrix3D<T>::Matrix3D() {}

template<typename T>
Matrix3D<T>::~Matrix3D() {}

template<typename T>
Matrix3D<T> &Matrix3D<T>::operator=(const Matrix3D<T> &m) {
    di_ = m.di_;
    dj_ = m.dj_;
    dk_ = m.dk_;
    if (di_ == 0 || dj_ == 0 || dk_ == 0)
        throw length_error("Matrix3D dimensions has 0 size");
    data_ = unique_ptr<T[]>(new T[di_ * dj_ * dk_]);
    std::copy(m.data_.get(), m.data_.get() + di_ * dj_ * dk_, data_.get());
    return *this;
}

template<typename T>
Matrix3D<T> &Matrix3D<T>::operator+=(const Matrix3D<T> &m) {

    if (m.di_ != di_ || m.dj_ != dj_ || m.dk_ != dk_)
        throw length_error("Matrixes of different sizes cannot be added.");

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
    if (i >= di_ || j >= dj_ || k >= dk_) {
        string error_message = "Matrix3D subscript out of bounds.\ni=" +
                to_string(i) + " (>=" + to_string(di_) + "?)\nj=" +
                to_string(j) + " (>" + to_string(dj_) + "?)\nk=" +
                to_string(k) + " (>=" + to_string(dk_) + "?)";
        std::throw_with_nested(std::length_error(error_message.c_str()));
    }
    return data_[dj_ * dk_ * i + dk_ * j + k];
}

template<typename T>
T Matrix3D<T>::operator()(int i, int j, int k) const {
    if (i >= di_ || j >= dj_ || k >= dk_) {
        string error_message = "Matrix3D subscript out of bounds.\ni=" +
                to_string(i) + " (>=" + to_string(di_) + "?)\nj=" +
                to_string(j) + " (>" + to_string(dj_) + "?)\nk=" +
                to_string(k) + " (>=" + to_string(dk_) + "?)";
        std::throw_with_nested(std::length_error(error_message.c_str()));
    }
    return data_[dj_ * dk_ * i + dk_ * j + k];
}

template<typename T>
void Matrix3D<T>::setData(T *data, int length) {
    if (length != di_*dj_*dk_) {
        string er = "Size of data does not match that of matrix: " +
                to_string(length) + " vs. " + to_string(di_*dj_*dk_);
        throw_with_nested(invalid_argument(er.c_str()));
    }
    memcpy(data_.get(), data, length* sizeof(T));
}

template<typename T>
void Matrix3D<T>::setPartialData(int i, int j, T *data, int length) {
    if (i >= di_ || j >= dj_ || i * j < 0)
        throw length_error("Matrix3D subscript out of bounds or negative");
    memcpy(data_.get() + i * dj_ * dk_ + j * dk_, data, length * sizeof(T));
}

template<typename T>
bool Matrix3D<T>::empty() const {
    return di_ == NON_INITIALIZED;
}

template<typename T>
void Matrix3D<T>::reset(T value) {
    fill_n(data_.get(), di_ * dj_ * dk_, value);
}

template<typename T>
void Matrix3D<T>::print(int i) const {
    for (int j = 0; j < dj_; ++j) {
        for (int k = 0; k < dk_; ++k) {
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
        throw invalid_argument("the first argument must be either one of chars 'i', 'j' or 'k.'");

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

template<typename T>
T *Matrix3D<T>::getPointerToElement(int i, int j, int k) const {
    return data_.get() + i * dj_ * dk_ + j * dk_ + k;
}

template<typename T>
void Matrix3D<T>::add_to_position(int i, int j, int k, T *data,
                                  int length) {
    int pos0 = i * dj_ * dk_ + j * dk_ + k;
    for (int p = 0; p < length; ++p) {
        data_[pos0 + p] += data[p];
    }
}


#endif //TRIANGLEMODEL_MATRICES_H
