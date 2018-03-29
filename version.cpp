#include "version.h"

bool operator==(const version_info& lhs, const version_info& rhs) {
  if ((lhs._major == rhs._major)
    && (lhs._minor == rhs._minor)
    && (lhs._patch == rhs._patch))
    return true;
  return false;
}

bool operator!=(const version_info& lhs, const version_info& rhs) {
  return !(lhs == rhs);
}

bool operator<(const version_info& lhs, const version_info& rhs) {
  if (lhs._major < rhs._major)
    return true;
  if (lhs._minor < rhs._minor)
    return true;
  if (lhs._patch < rhs._patch)
    return true;
  return false;
}

bool operator>(const version_info& lhs, const version_info& rhs) {
  return rhs < lhs;
}

bool operator<=(const version_info& lhs, const version_info& rhs) {
  return !(lhs > rhs);
}

bool operator>=(const version_info& lhs, const version_info& rhs) {
  return !(lhs < rhs);
}

std::ostream& operator<<(std::ostream& out, const version_info& out_version) {
  return out << out_version._major << '.' << out_version._minor << '.' << out_version._patch;
}

version_info version()
{
  return version_info(PROJECT_VERSION_MAJOR,
                      PROJECT_VERSION_MINOR,
                      PROJECT_VERSION_PATCH);
}
