#ifndef FANMAIL_H_11D8D668
#define FANMAIL_H_11D8D668

#include <random>
#include <string>
#include <memory>
#include <Magick++.h>
#include <twitter.h>
#include <rawr.h>
#include "designer.h"

class fanmail {
public:

  fanmail(
    std::string configFile,
    std::mt19937& rng);

  void run() const;

private:

  void sendTweet(Magick::Image image, std::string doc) const;

  std::mt19937& rng_;
  std::unique_ptr<twitter::auth> auth_;
  std::unique_ptr<twitter::client> client_;
  std::unique_ptr<designer> layout_;
  rawr kgramstats_;

};

#endif /* end of include guard: SAP_H_11D8D668 */
