//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for Parametrized distributions
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
#ifndef OPENTURNS_PARAMETRIZEDDISTRIBUTION_HXX
#define OPENTURNS_PARAMETRIZEDDISTRIBUTION_HXX

#include "openturns/DistributionImplementation.hxx"
#include "openturns/DistributionParameters.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class ParametrizedDistribution
 *
 * A subclass for Parametrized distributions.
 */
class OT_API ParametrizedDistribution
  : public DistributionImplementation
{

  CLASSNAME
public:

  /** Default constructor */
  ParametrizedDistribution();

  /** Constructor with parameter */
  explicit ParametrizedDistribution(const DistributionParameters & distParam);

  /** Virtual constructor */
  ParametrizedDistribution * clone() const override;

  /** Comparison operator */
  using DistributionImplementation::operator ==;
  Bool operator ==(const ParametrizedDistribution & other) const;
protected:
  Bool equals(const DistributionImplementation & other) const override;
public:

  /** Get one realization of the distribution */
  Point getRealization() const override;

  /** Get the DDF of the distribution */
  using DistributionImplementation::computeDDF;
  Point computeDDF(const Point & point) const override;

  /** Get the PDF of the distribution */
  using DistributionImplementation::computePDF;
  Scalar computePDF(const Point & point) const override;
  using DistributionImplementation::computeLogPDF;
  Scalar computeLogPDF(const Point & point) const override;

  /** Get the CDF of the distribution */
  using DistributionImplementation::computeCDF;
  Scalar computeCDF(const Point & point) const override;

  /** Get the CDF of the distribution */
  using DistributionImplementation::computeComplementaryCDF;
  Scalar computeComplementaryCDF(const Point & point) const override;

  /** Compute the entropy of the distribution */
  Scalar computeEntropy() const override;

  /** Get the characteristic function of the distribution, i.e. phi(u) = E(exp(I*u*X)) */
  Complex computeCharacteristicFunction(const Scalar x) const override;
  Complex computeLogCharacteristicFunction(const Scalar x) const override;

  /** Get the PDFGradient of the distribution */
  using DistributionImplementation::computePDFGradient;
  Point computePDFGradient(const Point & point) const override;

  /** Get the CDFGradient of the distribution */
  using DistributionImplementation::computeCDFGradient;
  Point computeCDFGradient(const Point & point) const override;

  /** Get the quantile of the distribution */
  using DistributionImplementation::computeQuantile;
  Point computeQuantile(const Scalar prob, const Bool tail = false) const override;

  /** Get the product minimum volume interval containing a given probability of the distribution */
  Interval computeMinimumVolumeIntervalWithMarginalProbability(const Scalar prob, Scalar & marginalProbOut) const override;

  /** Get the product bilateral confidence interval containing a given probability of the distribution */
  Interval computeBilateralConfidenceIntervalWithMarginalProbability(const Scalar prob, Scalar & marginalProbOut) const override;

  /** Get the product unilateral confidence interval containing a given probability of the distribution */
  Interval computeUnilateralConfidenceIntervalWithMarginalProbability(const Scalar prob, const Bool tail, Scalar & marginalProbOut) const override;

  /** Get the minimum volume level set containing a given probability of the distribution */
  LevelSet computeMinimumVolumeLevelSetWithThreshold(const Scalar prob, Scalar & thresholdOut) const override;

  /** Parameters value accessors */
  void setParameter(const Point & parameter) override;
  Point getParameter() const override;

  /** Parameters description accessor */
  Description getParameterDescription() const override;

  /** Check if the distribution is elliptical */
  Bool isElliptical() const override;

  /** Check if the distribution is continuous */
  Bool isContinuous() const override;

  /** Check if the distribution is a copula */
  Bool isCopula() const override;

  /** Check if the distribution is discrete */
  Bool isDiscrete() const override;

  /** Tell if the distribution is integer valued */
  Bool isIntegral() const override;

  /** Get the roughness, i.e. the L2-norm of the PDF */
  Scalar getRoughness() const override;

  /** Get the mean of the distribution */
  Point getMean() const override;

  /** Get the standard deviation of the distribution */
  Point getStandardDeviation() const override;

  /** Get the skewness of the distribution */
  Point getSkewness() const override;

  /** Get the kurtosis of the distribution */
  Point getKurtosis() const override;

  /** Get the standard representative in the parametric family, associated with the standard moments */
  Distribution getStandardRepresentative() const override;

  /** Compute the PDF of Xi | X1, ..., Xi-1. x = Xi, y = (X1,...,Xi-1) */
  using DistributionImplementation::computeConditionalPDF;
  Scalar computeConditionalPDF(const Scalar x, const Point & y) const override;
  Point computeSequentialConditionalPDF(const Point & x) const override;

  /** Compute the CDF of Xi | X1, ..., Xi-1. x = Xi, y = (X1,...,Xi-1) */
  using DistributionImplementation::computeConditionalCDF;
  Scalar computeConditionalCDF(const Scalar x, const Point & y) const override;
  Point computeSequentialConditionalCDF(const Point & x) const override;

  /** Compute the quantile of Xi | X1, ..., Xi-1, i.e. x such that CDF(x|y) = q with x = Xi, y = (X1,...,Xi-1) */
  using DistributionImplementation::computeConditionalQuantile;
  Scalar computeConditionalQuantile(const Scalar q, const Point & y) const override;
  Point computeSequentialConditionalQuantile(const Point & q) const override;

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

  /** Distribution parameters accessor */
  DistributionParameters getDistributionParameters() const;

private:

  /** Compute the range of the distribution*/
  void computeRange() override;

  DistributionParameters distributionParameters_;
  Distribution distribution_;

}; /* class ParametrizedDistribution */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_PARAMETRIZEDDISTRIBUTION_HXX */
