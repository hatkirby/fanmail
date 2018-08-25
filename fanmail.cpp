#include "fanmail.h"
#include <yaml-cpp/yaml.h>
#include <thread>
#include <chrono>
#include <fstream>
#include <iostream>

fanmail::fanmail(
  std::string configFile,
  std::mt19937& rng) :
    rng_(rng)
{
  // Load the config file.
  YAML::Node config = YAML::LoadFile(configFile);

  // Set up the Twitter client.
  auth_ =
    std::unique_ptr<twitter::auth>(
      new twitter::auth(
        config["consumer_key"].as<std::string>(),
        config["consumer_secret"].as<std::string>(),
        config["access_key"].as<std::string>(),
        config["access_secret"].as<std::string>()));

  client_ =
    std::unique_ptr<twitter::client>(
      new twitter::client(*auth_));

  // Set up the layout designer.
  layout_ = std::unique_ptr<designer>(
    new designer(config["images"].as<std::string>()));

  // Set up librawr.
  std::ifstream infile(config["corpus"].as<std::string>());
  std::string corpus;
  std::string line;
  while (getline(infile, line))
  {
    if (line.back() == '\r')
    {
      line.pop_back();
    }

    corpus += line + "\n";
  }

  kgramstats_.addCorpus(corpus);
  kgramstats_.compile(3);
}

void fanmail::run() const
{
  for (;;)
  {
    std::cout << "Generating tweet..." << std::endl;

    try
    {
      // Design the comic strip.
      Magick::Image image = layout_->generate(rng_);

      // Write the tweet.
      std::string doc = kgramstats_.randomSentence(15);
      doc.resize(40);

      std::cout << doc << std::endl;

      // Send the tweet.
      std::cout << "Sending tweet..." << std::endl;

      sendTweet(std::move(image), std::move(doc));

      std::cout << "Tweeted!" << std::endl;

      // Wait.
      std::binomial_distribution<int> waitDist(35, 0.5);
      int waitTime = waitDist(rng_) + 1;

      std::cout << "Waiting for " << waitTime << " hours..." << std::endl;

      std::this_thread::sleep_for(std::chrono::hours(waitTime));
    } catch (const Magick::ErrorImage& ex)
    {
      std::cout << "Image error: " << ex.what() << std::endl;
    } catch (const twitter::twitter_error& ex)
    {
      std::cout << "Twitter error: " << ex.what() << std::endl;

      std::this_thread::sleep_for(std::chrono::hours(1));
    }

    std::cout << std::endl;
  }
}

void fanmail::sendTweet(Magick::Image image, std::string doc) const
{
  Magick::Blob outputimg;
  image.magick("png");
  image.write(&outputimg);

  long media_id = client_->uploadMedia("image/png",
    static_cast<const char*>(outputimg.data()), outputimg.length());

  client_->updateStatus(std::move(doc), {media_id});
}
