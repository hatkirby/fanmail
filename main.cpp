#include "fanmail.h"
#include <iostream>

int main(int argc, char** argv)
{
  Magick::InitializeMagick(nullptr);

  std::random_device randomDevice;
  std::mt19937 rng(randomDevice());

  // We also have to seed the C-style RNG because librawr uses it.
  srand(time(NULL));
  rand(); rand(); rand(); rand();

  if (argc != 2)
  {
    std::cout << "usage: fanmail [configfile]" << std::endl;
    return -1;
  }

  std::string configfile(argv[1]);

  try
  {
    fanmail bot(configfile, rng);

    try
    {
      bot.run();
    } catch (const std::exception& ex)
    {
      std::cout << "Error running bot: " << ex.what() << std::endl;
    }
  } catch (const std::exception& ex)
  {
    std::cout << "Error initializing bot: " << ex.what() << std::endl;
  }
}
