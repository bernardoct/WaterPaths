//
// Created by Bernardo on 11/25/2019.
//

#include "MissingParameter.h"

MissingParameter::MissingParameter(const string &missing_param_name,
        const string &tag_name, const int line_number) :
        error_message(("Error: system input file missing parameter\nParameter " + missing_param_name +
                        " is missing in tag " + tag_name + ", line number " +
                        to_string(line_number)).c_str()) {}

const char *MissingParameter::what() const noexcept {
    return error_message.what();
}

static_assert(std::is_nothrow_copy_constructible<MissingParameter>::value,
              "S must be nothrow copy constructible");