//
// Created by Bernardo on 11/25/2019.
//

#ifndef TRIANGLEMODEL_MISSINGPARAMETER_H
#define TRIANGLEMODEL_MISSINGPARAMETER_H

#include <exception>
#include <string>
#include <stdexcept>

using namespace std;

class MissingParameter : public exception {
private:
    runtime_error error_message;
public:

    MissingParameter(const string &missing_param_name, const string &tag_name, int line_number);

    ~MissingParameter() noexcept override = default;

    const char* what() const noexcept override;
};


#endif //TRIANGLEMODEL_MISSINGPARAMETER_H
