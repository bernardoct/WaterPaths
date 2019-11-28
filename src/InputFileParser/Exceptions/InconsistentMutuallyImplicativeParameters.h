//
// Created by Bernardo on 11/26/2019.
//

#ifndef TRIANGLEMODEL_INCONSISTENTMUTUALLYIMPLICATIVEPARAMETERS_H
#define TRIANGLEMODEL_INCONSISTENTMUTUALLYIMPLICATIVEPARAMETERS_H

#include <exception>
#include <string>
#include <stdexcept>

using namespace std;

class InconsistentMutuallyImplicativeParameters : virtual public exception {
private:
    runtime_error error_message;
public:
    explicit
    InconsistentMutuallyImplicativeParameters(const string &mutually_implicative_params, const string &tag_name, int line_number);

    ~InconsistentMutuallyImplicativeParameters() noexcept override = default;

    const char* what() const noexcept override;
};


#endif //TRIANGLEMODEL_INCONSISTENTMUTUALLYIMPLICATIVEPARAMETERS_H
