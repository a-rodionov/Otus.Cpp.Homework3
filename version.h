#include "version_numbers.h"
#include <iostream>

namespace homework3 {

struct version_info{

  version_info()
    { version_info(0, 0, 0); }

  version_info(const int major,
               const int minor,
               const int patch)
    : _major(major), _minor(minor), _patch(patch) {}

  int _major;
  int _minor;
  int _patch;
};

bool operator==(const version_info& lhs, const version_info& rhs);
bool operator!=(const version_info& lhs, const version_info& rhs);
bool operator<(const version_info& lhs, const version_info& rhs);
bool operator>(const version_info& lhs, const version_info& rhs);
bool operator<=(const version_info& lhs, const version_info& rhs);
bool operator>=(const version_info& lhs, const version_info& rhs);

std::ostream& operator<<(std::ostream& out, const version_info& out_version);

version_info version();

}
