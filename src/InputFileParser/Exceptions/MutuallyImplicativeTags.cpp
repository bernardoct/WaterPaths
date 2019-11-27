//
// Created by Bernardo on 11/26/2019.
//

#include "MutuallyImplicativeTags.h"

MutuallyImplicativeTags::MutuallyImplicativeTags(
        const string &mutually_implicative_params, const string &tag_name,
        int line_number) :
    error_message(("Error: system input file with inconsistent "
                   "Parameters\nParameters " + mutually_implicative_params +
    " must be all present or all missing from " + tag_name +
    " in line number " + to_string(line_number)).c_str()) {}

const char *MutuallyImplicativeTags::what() const noexcept {
    return error_message.what();
}

static_assert(std::is_nothrow_copy_constructible<MutuallyImplicativeTags>::value,
              "S must be nothrow copy constructible");

