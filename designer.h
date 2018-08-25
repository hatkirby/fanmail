#ifndef DESIGNER_H_863F77D3
#define DESIGNER_H_863F77D3

#include <random>
#include <string>
#include <Magick++.h>

class designer {
public:

  designer(std::string imagesPath);

  Magick::Image generate(std::mt19937& rng) const;

private:

  mutable std::discrete_distribution<int> colDist_;
  mutable std::discrete_distribution<int> lenDist_;
  std::map<int, std::map<int, std::string>> comics_;
};

#endif /* end of include guard: DESIGNER_H_863F77D3 */
