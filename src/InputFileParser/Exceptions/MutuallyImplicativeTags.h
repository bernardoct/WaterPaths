//
// Created by Bernardo on 11/26/2019.
//

#ifndef TRIANGLEMODEL_MUTUALLYIMPLICATIVETAGS_H
#define TRIANGLEMODEL_MUTUALLYIMPLICATIVETAGS_H

#include <exception>
#include <string>
#include <stdexcept>

using namespace std;

class MutuallyImplicativeTags : virtual public exception {
private:
    runtime_error error_message;
public:
    explicit
    MutuallyImplicativeTags(const string &mutually_implicative_params, const string &tag_name, int line_number);

    ~MutuallyImplicativeTags() noexcept override = default;

    const char* what() const noexcept override;
};


#endif //TRIANGLEMODEL_MUTUALLYIMPLICATIVETAGS_H
