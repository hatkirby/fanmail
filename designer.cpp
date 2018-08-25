#include "designer.h"
#include <dirent.h>
#include <sstream>
#include <iostream>

designer::designer(
  std::string imagesDir) :
    colDist_ {0, 1, 100, 30, 5, 1, 1}
{
  DIR* imgdir;
  struct dirent* ent;
  if ((imgdir = opendir(imagesDir.c_str())) == nullptr)
  {
    throw std::invalid_argument("Couldn't find images");
  }

  int maxStripLen = 0;
  std::map<int, int> stripLength;

  while ((ent = readdir(imgdir)) != nullptr)
  {
    std::string dname(ent->d_name);
    std::stringstream nameParse;
    int stripNum;
    int panelNum;
    char sep;
    std::string ext;

    nameParse << dname;

    if (nameParse >> stripNum &&
        nameParse >> sep &&
        nameParse >> panelNum &&
        nameParse >> ext &&
        ext == ".png")
    {
      if (!stripLength.count(stripNum) || stripLength[stripNum] < panelNum)
      {
        stripLength[stripNum] = panelNum;
      }

      if (panelNum > maxStripLen)
      {
        maxStripLen = panelNum;
      }

      comics_[stripNum][panelNum] = imagesDir + "/" + dname;
    }
  }

  closedir(imgdir);

  std::vector<int> lenHist(maxStripLen, 0);
  for (const auto& mapping : stripLength)
  {
    lenHist[mapping.second]++;
  }

  lenDist_ =
    std::discrete_distribution<int>(
      std::begin(lenHist),
      std::end(lenHist));
}

Magick::Image designer::generate(std::mt19937& rng) const
{
  int numPanels = lenDist_(rng);
  std::vector<Magick::Image> panels;

  int numCols = std::min(colDist_(rng), numPanels);

  int curCol = 0;
  int numRows = 0;
  int curRowWidth = 0;
  int maxRowWidth = 0;

  std::map<int, int> rowHeight;
  int curRowHeight = 0;

  for (int i = 0; i < numPanels; i++)
  {
    if (curCol == 0)
    {
      numRows++;
    }

    std::uniform_int_distribution<int> stripPick(1, comics_.size());
    int stripNum = stripPick(rng);
    const auto& strip = comics_.at(stripNum);

    size_t low = i / static_cast<double>(numPanels) * strip.size();
    size_t high = (i+1) / static_cast<double>(numPanels) * strip.size();
    std::uniform_int_distribution<int> panelPick(
      low,
      std::min(high, strip.size() - 1));

    int panelNum = panelPick(rng) + 1;
    const std::string& panel = strip.at(panelNum);

    std::cout << panel << std::endl;

    Magick::Image curfile(panel);
    curfile.borderColor("black");
    curfile.backgroundColor("black");

    if (std::bernoulli_distribution(1.0 / 50.0)(rng))
    {
      std::normal_distribution<double> rotDist(0.0, 20.0);
      double rotAmt = rotDist(rng);

      curfile.rotate(rotAmt);
    }

    if (std::bernoulli_distribution(1.0 / 2.0)(rng))
    {
      std::geometric_distribution<unsigned int> borderDist(1.0 / 8.0);
      curfile.border(
        Magick::Geometry{
          borderDist(rng),
          borderDist(rng)});
    }

    curRowWidth += curfile.columns();

    if (curfile.rows() > curRowHeight)
    {
      curRowHeight = curfile.rows();
    }

    panels.emplace_back(std::move(curfile));

    curCol++;

    if (curCol >= numCols || (i == numPanels - 1))
    {
      if (curRowWidth > maxRowWidth)
      {
        maxRowWidth = curRowWidth;
      }

      rowHeight[numRows - 1] = curRowHeight;

      curCol = 0;
      curRowWidth = 0;
      curRowHeight = 0;
    }
  }

  int fileHeight = 0;
  for (const auto& mapping : rowHeight)
  {
    fileHeight += mapping.second;
  }

  Magick::Image result{
    Magick::Geometry(maxRowWidth, fileHeight),
    "black"};

  int curx = 0;
  int cury = 0;
  int thisCol = 0;
  int thisRow = 0;
  for (const Magick::Image& panel : panels)
  {
    if (thisCol == numCols)
    {
      thisCol = 0;
      cury += rowHeight.at(thisRow);
      thisRow++;
      curx = 0;
    }

    result.composite(panel, curx, cury);

    curx += panel.columns();

    thisCol++;
  }

  return result;
}
