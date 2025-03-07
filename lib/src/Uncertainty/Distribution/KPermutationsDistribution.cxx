//                                               -*- C++ -*-
/**
 *  @brief The KPermutationsDistribution distribution
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
#include <cmath>
#include "openturns/Collection.hxx"
#include "openturns/KPermutationsDistribution.hxx"
#include "openturns/KPermutations.hxx"
#include "openturns/RandomGenerator.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/Exception.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

typedef Collection<UnsignedInteger>     UnsignedIntegerCollection;

CLASSNAMEINIT(KPermutationsDistribution)

static const Factory<KPermutationsDistribution> Factory_KPermutationsDistribution;

/* Default constructor */
KPermutationsDistribution::KPermutationsDistribution()
  : DistributionImplementation()
  , k_(0)
  , n_(0)
{
  setName("KPermutationsDistribution");
  setKN(1, 1);
}

/* Parameters constructor */
KPermutationsDistribution::KPermutationsDistribution(const UnsignedInteger k,
    const UnsignedInteger n)
  : DistributionImplementation()
  , k_(0)
  , n_(0)
{
  setName("KPermutationsDistribution");
  setKN(k, n);
}

/* Tell if the distribution is continuous */
Bool KPermutationsDistribution::isContinuous() const
{
  return false;
}

/* Tell if the distribution is discrete */
Bool KPermutationsDistribution::isDiscrete() const
{
  return true;
}

/* Tell if the distribution is integer valued */
Bool KPermutationsDistribution::isIntegral() const
{
  return true;
}

/* Comparison operator */
Bool KPermutationsDistribution::operator ==(const KPermutationsDistribution & other) const
{
  if (this == &other) return true;
  return (k_ == other.k_) && (n_ == other.n_);
}

Bool KPermutationsDistribution::equals(const DistributionImplementation & other) const
{
  const KPermutationsDistribution* p_other = dynamic_cast<const KPermutationsDistribution*>(&other);
  return p_other && (*this == *p_other);
}

/* String converter */
String KPermutationsDistribution::__repr__() const
{
  OSS oss;
  oss << "class=" << KPermutationsDistribution::GetClassName()
      << " name=" << getName()
      << " dimension=" << getDimension()
      << " k=" << k_
      << " n=" << n_;
  return oss;
}

String KPermutationsDistribution::__str__(const String & ) const
{
  OSS oss;
  oss << getClassName() << "(k = " << k_ << ", n = " << n_ << ")";
  return oss;
}

/* Virtual constructor */
KPermutationsDistribution * KPermutationsDistribution::clone() const
{
  return new KPermutationsDistribution(*this);
}

/* Compute the numerical range of the distribution given the parameters values */
void KPermutationsDistribution::computeRange()
{
  const Point lowerBound(k_, 0.0);
  const Point upperBound(k_, n_ - 1.0);
  const Interval::BoolCollection finiteLowerBound(k_, true);
  const Interval::BoolCollection finiteUpperBound(k_, true);
  setRange(Interval(lowerBound, upperBound, finiteLowerBound, finiteUpperBound));
}

/* Get one realization of the distribution */
Point KPermutationsDistribution::getRealization() const
{
  Point realization(k_);
  Indices buffer(n_);
  buffer.fill();
  for (UnsignedInteger i = 0; i < k_; ++i)
  {
    const UnsignedInteger index = i + RandomGenerator::IntegerGenerate(n_ - i);
    realization[i] = buffer[index];
    buffer[index] = buffer[i];
  }
  return realization;
}

/* Get the PDF of the distribution */
Scalar KPermutationsDistribution::computeLogPDF(const Point & point) const
{
  const UnsignedInteger dimension = getDimension();
  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getDimension();
  Indices x(k_);
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    const Scalar k = point[i];
    if ((k < -supportEpsilon_) || (k > n_ + supportEpsilon_)) return SpecFunc::LowestScalar;
    const UnsignedInteger ik = static_cast< UnsignedInteger > (round(k));
    if (std::abs(k - ik) > supportEpsilon_) return SpecFunc::LowestScalar;
    x[i] = ik;
  }
  if (!x.check(n_)) return SpecFunc::LowestScalar;
  return logPDFValue_;
}

Scalar KPermutationsDistribution::computePDF(const Point & point) const
{
  const Scalar logPDF = computeLogPDF(point);
  if (logPDF == SpecFunc::LowestScalar) return 0.0;
  return std::exp(logPDF);
}

/* Get the CDF of the distribution */
Scalar KPermutationsDistribution::computeCDF(const Point & point) const
{
  const UnsignedInteger dimension = getDimension();
  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getDimension();

  if (dimension == 1) return static_cast < Scalar >(k_) / n_;
  Point sortedPoint(dimension);
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    const Scalar x = point[i];
    if (x < -supportEpsilon_) return 0.0;
    sortedPoint[i] = std::min(n_ - 1.0, floor(x + supportEpsilon_));
  }
  std::sort(sortedPoint.begin(), sortedPoint.end());
  Scalar cdfValue = 1.0;
  for (UnsignedInteger i = 0; i < dimension; ++i) cdfValue *= (sortedPoint[i] + 1.0 - i) / (n_ - i);
  return cdfValue;
}

/* Compute the scalar quantile of the 1D KPermutationsDistribution distribution */
Scalar KPermutationsDistribution::computeScalarQuantile(const Scalar prob,
    const Bool tail) const
{
  const UnsignedInteger i = static_cast< UnsignedInteger >(ceil(prob * (n_ - 1.0)));
  return (tail ? n_ - 1.0 - i : i);
} // computeScalarQuantile

/* Compute the quantile of the KPermutationsDistribution distribution */
Point KPermutationsDistribution::computeQuantile(const Scalar prob,
    const Bool tail,
    Scalar & marginalProb) const
{
  const Scalar p = (tail ? 1.0 - prob : prob);
  if (p <= 0.0) return Point(k_, 0.0);
  if (p >= 1.0) return Point(k_, n_);
  UnsignedInteger iMin = 0;
  UnsignedInteger iMax = n_;
  while (iMax > iMin + 1)
  {
    const UnsignedInteger iMiddle = (iMax + iMin) / 2;
    const Scalar cdfMiddle = computeCDF(Point(k_, iMiddle));
    if (cdfMiddle < p)
    {
      iMin = iMiddle;
    }
    else
    {
      iMax = iMiddle;
    }
  } // while
  marginalProb = computeScalarQuantile(prob, tail);
  return Point(k_, iMax);
} // computeQuantile

/* Get the i-th marginal distribution */
Distribution KPermutationsDistribution::getMarginal(const UnsignedInteger i) const
{
  const UnsignedInteger dimension = getDimension();
  if (i >= dimension) throw InvalidArgumentException(HERE) << "The index of a marginal distribution must be in the range [0, dim-1]";
  KPermutationsDistribution marginal(1, n_);
  marginal.setDescription({getDescription()[i]});
  return marginal;
}

/* Get the distribution of the marginal distribution corresponding to indices dimensions */
Distribution KPermutationsDistribution::getMarginal(const Indices & indices) const
{
  const UnsignedInteger dimension = getDimension();
  if (!indices.check(dimension)) throw InvalidArgumentException(HERE) << "The indices of a marginal distribution must be in the range [0, dim-1] and must be different";
  // Special case for dimension 1
  if (dimension == 1) return clone();
  // General case
  const UnsignedInteger outputDimension = indices.getSize();
  KPermutationsDistribution marginal(outputDimension, n_);
  marginal.setDescription(getDescription().select(indices));
  return marginal;
}

/* Get the support of a discrete distribution that intersect a given interval */
Sample KPermutationsDistribution::getSupport(const Interval & interval) const
{
  if (interval.getDimension() != getDimension()) throw InvalidArgumentException(HERE) << "Error: the given interval has a dimension that does not match the distribution dimension.";
  // Convert int values into float
  const IndicesCollection intResult(KPermutations(k_, n_).generate());
  const UnsignedInteger size = intResult.getSize();
  if (size == 0) return Sample(0, getDimension());
  const Interval inter(interval.intersect(range_));
  // Common case: get the full support
  if (inter == range_)
  {
    Sample result(size, dimension_);
    for (UnsignedInteger i = 0; i < size; ++i)
      for (UnsignedInteger j = 0; j < dimension_; ++j)
        result(i, j) = intResult(i, j);
    return result;
  }
  Sample result(0, dimension_);
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    Point point(dimension_);
    for (UnsignedInteger j = 0; j < dimension_; ++j)
      point[j] = intResult(i, j);
    if (inter.contains(point))
      result.add(point);
  }
  return result;
}

/* Compute the mean of the distribution */
void KPermutationsDistribution::computeMean() const
{
  mean_ = Point(k_, 0.5 * (n_ - 1.0));
  isAlreadyComputedMean_ = true;
}

/* Compute the covariance of the distribution */
void KPermutationsDistribution::computeCovariance() const
{
  const Scalar var = (n_ * n_ - 1.0) / 12.0;
  const Scalar cov = -(n_ + 1.0) / 12.0;
  covariance_ = CovarianceMatrix(k_, Point(k_ * k_, cov));
  for (UnsignedInteger i = 0; i < k_; ++i) covariance_(i, i) = var;
  isAlreadyComputedCovariance_ = true;
}

/* Parameters value and description accessor */
Point KPermutationsDistribution::getParameter() const
{
  return {static_cast<Scalar>(k_), static_cast<Scalar>(n_)};
}

void KPermutationsDistribution::setParameter(const Point & parameter)
{
  if (parameter.getSize() != 2) throw InvalidArgumentException(HERE) << "KPermutations expected 2 parameters, got " << parameter.getSize();
  if (parameter[0] != std::round(parameter[0])) throw InvalidArgumentException(HERE) << "the KPermutations first parameter k must be an integer, got " << parameter[0];
  if (parameter[1] != std::round(parameter[1])) throw InvalidArgumentException(HERE) << "the KPermutations second parameter n must be an integer, got " << parameter[1];
  setKN(parameter[0], parameter[1]);
}

/* Parameters description accessor */
Description KPermutationsDistribution::getParameterDescription() const
{
  return {"k", "n"};
}

/* Parameters value and description accessor */
KPermutationsDistribution::PointWithDescriptionCollection KPermutationsDistribution::getParametersCollection() const
{
  const UnsignedInteger dimension = getDimension();
  PointWithDescriptionCollection parameters((dimension == 1 ? 1 : dimension + 1));
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    PointWithDescription point(1);
    point[0] = n_;
    Description description(1);
    description[0] = "n";
    point.setDescription(description);
    point.setName(getDescription()[i]);
    parameters[i] = point;
  }
  if (dimension > 1)
  {
    PointWithDescription point(2);
    Description description = {"k", "n"};
    point[0] = k_;
    point[1] = n_;
    point.setDescription(description);
    point.setName("dependence");
    parameters[dimension] = point;
  }
  return parameters;
}

/* K accessor */
void KPermutationsDistribution::setK(const UnsignedInteger k)
{
  if (k == 0) throw InvalidArgumentException(HERE) << "Error: k must be > 0.";
  if (k > n_) throw InvalidArgumentException(HERE) << "Error: k must be less or equal to n, here k=" << k << " and n=" << n_;
  if (k != k_)
  {
    k_ = k;
    logPDFValue_ = SpecFunc::LogGamma(n_ - k_ + 1) - SpecFunc::LogGamma(n_ + 1);
    setDimension(k);
    isAlreadyComputedMean_ = false;
    isAlreadyComputedCovariance_ = false;
    isAlreadyCreatedGeneratingFunction_ = false;
  }
}

/* K accessor */
UnsignedInteger KPermutationsDistribution::getK() const
{
  return k_;
}

/* N accessor */
void KPermutationsDistribution::setN(const UnsignedInteger n)
{
  if (n == 0) throw InvalidArgumentException(HERE) << "Error: n must be > 0.";
  if (n < k_) throw InvalidArgumentException(HERE) << "Error: n must be greater or equal to k, here n=" << n << " and k=" << k_;
  if (n != n_)
  {
    n_ = n;
    logPDFValue_ = SpecFunc::LogGamma(n_ - k_ + 1) - SpecFunc::LogGamma(n_ + 1);
    isAlreadyComputedMean_ = false;
    isAlreadyComputedCovariance_ = false;
    computeRange();
  }
}

UnsignedInteger KPermutationsDistribution::getN() const
{
  return n_;
}

/* K/N accessor */
void KPermutationsDistribution::setKN(const UnsignedInteger k,
                                      const UnsignedInteger n)
{
  if (k == 0) throw InvalidArgumentException(HERE) << "Error: k must be > 0.";
  if (n == 0) throw InvalidArgumentException(HERE) << "Error: n must be > 0.";
  if (k > n) throw InvalidArgumentException(HERE) << "Error: k must be less or equal to n, here k=" << k << " and n=" << n;
  k_ = k;
  setDimension(k);
  n_ = n;
  logPDFValue_ = SpecFunc::LogGamma(n_ - k_ + 1) - SpecFunc::LogGamma(n_ + 1);
  isAlreadyComputedMean_ = false;
  isAlreadyComputedCovariance_ = false;
  computeRange();
}

/* Method save() stores the object through the StorageManager */
void KPermutationsDistribution::save(Advocate & adv) const
{
  DistributionImplementation::save(adv);
  adv.saveAttribute( "k_", k_ );
  adv.saveAttribute( "n_", n_ );
  adv.saveAttribute( "logPDFValue_", logPDFValue_ );
}

/* Method load() reloads the object from the StorageManager */
void KPermutationsDistribution::load(Advocate & adv)
{
  DistributionImplementation::load(adv);
  adv.loadAttribute( "k_", k_ );
  adv.loadAttribute( "n_", n_ );
  adv.loadAttribute( "logPDFValue_", logPDFValue_ );
  computeRange();
}

END_NAMESPACE_OPENTURNS
