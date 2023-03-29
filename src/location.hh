#ifndef LOCATION_H_
#define LOCATION_H_

#include <algorithm>
#include <string>

struct Location {
  size_t sline, scol, eline, ecol;

  Location merge(Location &other) {
    return Location{
        std::min(this->sline, other.sline), std::min(this->scol, other.scol),
        std::max(this->eline, other.eline), std::max(this->ecol, other.ecol)};
  }

  std::string to_string() {
    return std::to_string(sline) + ":" + std::to_string(scol) + ":" +
           std::to_string(eline) + ":" + std::to_string(ecol);
  }
};

#endif // LOCATION_H_
