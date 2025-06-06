//                                               -*- C++ -*-
/**
 *  @brief The test file of class Gamma for standard methods
 *
 *  Copyright 2005-2025 Airbus-EDF-IMACS-ONERA-Phimeca
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include "openturns/OT.hxx"
#include "openturns/OTtestcode.hxx"

using namespace OT;
using namespace OT::Test;

class TestObject : public Gamma
{
public:
  TestObject() : Gamma(1.5, 2.5, -0.5) {}
  virtual ~TestObject() {}
};


int main(int, char *[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);

  try
  {
    // Test basic functionnalities
    checkClassWithClassName<TestObject>();

    // Instantiate one distribution object
    Collection<Gamma> allDistributions(0);
    allDistributions.add(Gamma(1.5, 2.5, -0.5));
    allDistributions.add(Gamma(15.0, 2.5));
    for (UnsignedInteger n = 0; n < allDistributions.getSize(); ++n)
    {
      Gamma distribution(allDistributions[n]);
      fullprint << "Distribution " << distribution << std::endl;
      std::cout << "Distribution " << distribution << std::endl;

      // Is this distribution elliptical ?
      fullprint << "Elliptical = " << (distribution.isElliptical() ? "true" : "false") << std::endl;

      // Is this distribution continuous ?
      fullprint << "Continuous = " << (distribution.isContinuous() ? "true" : "false") << std::endl;

      // Test for realization of distribution
      Point oneRealization = distribution.getRealization();
      fullprint << "oneRealization=" << oneRealization << std::endl;

      // Test for sampling
      UnsignedInteger size = 10000;
      Sample oneSample = distribution.getSample( size );
      fullprint << "oneSample first=" << oneSample[0] << " last=" << oneSample[size - 1] << std::endl;
      fullprint << "mean=" << oneSample.computeMean() << std::endl;
      fullprint << "covariance=" << oneSample.computeCovariance() << std::endl;
      size = 100;
      for (UnsignedInteger i = 0; i < 2; ++i)
      {
        fullprint << "Kolmogorov test for the generator, sample size=" << size << " is " << (FittingTest::Kolmogorov(distribution.getSample(size), distribution).getBinaryQualityMeasure() ? "accepted" : "rejected") << std::endl;
        size *= 10;
      }

      // Define a point
      Point point( distribution.getDimension(), 1.0 );
      fullprint << "Point= " << point << std::endl;

      // Show PDF and CDF of point
      Scalar eps = 1e-5;
      Point DDF = distribution.computeDDF( point );
      fullprint << "ddf     =" << DDF << std::endl;
      Scalar LPDF = distribution.computeLogPDF( point );
      fullprint << "log pdf=" << LPDF << std::endl;
      Scalar PDF = distribution.computePDF( point );
      fullprint << "pdf     =" << PDF << std::endl;
      fullprint << "pdf (FD)=" << (distribution.computeCDF( point + Point(1, eps) ) - distribution.computeCDF( point  + Point(1, -eps) )) / (2.0 * eps) << std::endl;
      Scalar CDF = distribution.computeCDF( point );
      fullprint << "cdf=" << CDF << std::endl;
      Scalar CCDF = distribution.computeComplementaryCDF( point );
      fullprint << "ccdf=" << CCDF << std::endl;
      Scalar Survival = distribution.computeSurvivalFunction( point );
      fullprint << "survival=" << Survival << std::endl;
      Point InverseSurvival = distribution.computeInverseSurvivalFunction(0.95);
      fullprint << "Inverse survival=" << InverseSurvival << std::endl;
      fullprint << "Survival(inverse survival)=" << distribution.computeSurvivalFunction(InverseSurvival) << std::endl;
      Complex CF = distribution.computeCharacteristicFunction( point[0] );
      fullprint << "characteristic function=" << CF << std::endl;
      Complex LCF = distribution.computeLogCharacteristicFunction( point[0] );
      fullprint << "log characteristic function=" << LCF << std::endl;
      Point PDFgr = distribution.computePDFGradient( point );
      fullprint << "pdf gradient     =" << PDFgr << std::endl;
      Point PDFgrFD(3);
      PDFgrFD[0] = (Gamma(distribution.getK() + eps, distribution.getLambda(), distribution.getGamma()).computePDF(point) -
                    Gamma(distribution.getK() - eps, distribution.getLambda(), distribution.getGamma()).computePDF(point)) / (2.0 * eps);
      PDFgrFD[1] = (Gamma(distribution.getK(), distribution.getLambda() + eps, distribution.getGamma()).computePDF(point) -
                    Gamma(distribution.getK(), distribution.getLambda() - eps, distribution.getGamma()).computePDF(point)) / (2.0 * eps);
      PDFgrFD[2] = (Gamma(distribution.getK(), distribution.getLambda(), distribution.getGamma() + eps).computePDF(point) -
                    Gamma(distribution.getK(), distribution.getLambda(), distribution.getGamma() - eps).computePDF(point)) / (2.0 * eps);
      fullprint << "pdf gradient (FD)=" << PDFgrFD << std::endl;
      Point CDFgr = distribution.computeCDFGradient( point );
      fullprint << "cdf gradient     =" << CDFgr << std::endl;
      Point CDFgrFD(3);
      CDFgrFD[0] = (Gamma(distribution.getK() + eps, distribution.getLambda(), distribution.getGamma()).computeCDF(point) -
                    Gamma(distribution.getK() - eps, distribution.getLambda(), distribution.getGamma()).computeCDF(point)) / (2.0 * eps);
      CDFgrFD[1] = (Gamma(distribution.getK(), distribution.getLambda() + eps, distribution.getGamma()).computeCDF(point) -
                    Gamma(distribution.getK(), distribution.getLambda() - eps, distribution.getGamma()).computeCDF(point)) / (2.0 * eps);
      CDFgrFD[2] = (Gamma(distribution.getK(), distribution.getLambda(), distribution.getGamma() + eps).computeCDF(point) -
                    Gamma(distribution.getK(), distribution.getLambda(), distribution.getGamma() - eps).computeCDF(point)) / (2.0 * eps);
      fullprint << "cdf gradient (FD)=" << CDFgrFD << std::endl;
      Point quantile = distribution.computeQuantile( 0.95 );
      fullprint << "quantile=" << quantile << std::endl;
      fullprint << "cdf(quantile)=" << distribution.computeCDF(quantile) << std::endl;
      // Confidence regions
      Scalar threshold;
      Interval interval(distribution.computeMinimumVolumeIntervalWithMarginalProbability(0.95, threshold));
      if (distribution.getK() == 1.5)
      {
        assert_almost_equal(interval.getLowerBound(), {-0.49937}, 1e-4, 0.0);
        assert_almost_equal(interval.getUpperBound(), {1.06337}, 1e-4, 0.0);
      }
      else if (distribution.getK() == 15.0)
      {
        assert_almost_equal(interval.getLowerBound(), {3.1431}, 1e-4, 0.0);
        assert_almost_equal(interval.getUpperBound(), {9.09027}, 1e-4, 0.0);
      }
      fullprint << "threshold=" << threshold << std::endl;
      Scalar beta;
      LevelSet levelSet(distribution.computeMinimumVolumeLevelSetWithThreshold(0.95, beta));
      fullprint << "Minimum volume level set=" << levelSet << std::endl;
      fullprint << "beta=" << beta << std::endl;
      fullprint << "Bilateral confidence interval=" << distribution.computeBilateralConfidenceIntervalWithMarginalProbability(0.95, beta) << std::endl;
      fullprint << "beta=" << beta << std::endl;
      fullprint << "Unilateral confidence interval (lower tail)=" << distribution.computeUnilateralConfidenceIntervalWithMarginalProbability(0.95, false, beta) << std::endl;
      fullprint << "beta=" << beta << std::endl;
      fullprint << "Unilateral confidence interval (upper tail)=" << distribution.computeUnilateralConfidenceIntervalWithMarginalProbability(0.95, true, beta) << std::endl;
      fullprint << "beta=" << beta << std::endl;
      fullprint << "entropy=" << distribution.computeEntropy() << std::endl;
      fullprint << "entropy (MC)=" << -distribution.computeLogPDF(distribution.getSample(1000000)).computeMean()[0] << std::endl;
      Point mean = distribution.getMean();
      fullprint << "mean=" << mean << std::endl;
      CovarianceMatrix covariance = distribution.getCovariance();
      fullprint << "covariance=" << covariance << std::endl;
      CovarianceMatrix correlation = distribution.getCorrelation();
      fullprint << "correlation=" << correlation << std::endl;
      CovarianceMatrix spearman = distribution.getSpearmanCorrelation();
      fullprint << "spearman=" << spearman << std::endl;
      CovarianceMatrix kendall = distribution.getKendallTau();
      fullprint << "kendall=" << kendall << std::endl;
      Gamma::PointWithDescriptionCollection parameters = distribution.getParametersCollection();
      fullprint << "parameters=" << parameters << std::endl;
      fullprint << "Standard representative=" << distribution.getStandardRepresentative().__str__() << std::endl;

      Point standardDeviation = distribution.getStandardDeviation();
      fullprint << "standard deviation=" << standardDeviation << std::endl;
      Point skewness = distribution.getSkewness();
      fullprint << "skewness=" << skewness << std::endl;
      Point kurtosis = distribution.getKurtosis();
      fullprint << "kurtosis=" << kurtosis << std::endl;
    }
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}

