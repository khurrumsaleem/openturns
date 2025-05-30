//                                               -*- C++ -*-
/**
 *  @brief An implementation class for distribution-based random vectors
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
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/DeconditionedRandomVector.hxx"

BEGIN_NAMESPACE_OPENTURNS




CLASSNAMEINIT(DeconditionedRandomVector)

static const Factory<DeconditionedRandomVector> Factory_DeconditionedRandomVector;

/* Default constructor */
DeconditionedRandomVector::DeconditionedRandomVector()
  : RandomVectorImplementation()
{}

/* Default constructor */
DeconditionedRandomVector::DeconditionedRandomVector(const Distribution & distribution,
    const RandomVector & randomParameters)
  : RandomVectorImplementation(),
    distribution_(distribution),
    randomParameters_(randomParameters)
{
  // Check if the random parameters random vector has a dimension compatible with
  // the number of parameters of the distribution
  if (randomParameters.getDimension() != distribution.getParameterDimension()) throw InvalidArgumentException(HERE) << "Error: the random parameters dimension must be equal with the number of parameters of the distribution.";
  // Get the description from the underlying distribution
  setDescription(distribution.getDescription());
}

/* Virtual constructor */
DeconditionedRandomVector * DeconditionedRandomVector::clone() const
{
  return new DeconditionedRandomVector(*this);
}

/* String converter */
String DeconditionedRandomVector::__repr__() const
{
  OSS oss;
  oss << "class=" << DeconditionedRandomVector::GetClassName()
      << " distribution=" << distribution_
      << " random parameters=" << randomParameters_;
  return oss;
}



/* Here is the interface that all derived class must implement */


/* Dimension accessor */
UnsignedInteger DeconditionedRandomVector::getDimension() const
{
  return distribution_.getDimension();
}

/* Realization accessor */
Point DeconditionedRandomVector::getRealization() const
{
  Point parameters;
  return getRealization(parameters);
}

/* Realization accessor */
Point DeconditionedRandomVector::getRealization(Point & parameters) const
{
  parameters = randomParameters_.getRealization();
  distribution_.setParameter(parameters);
  return distribution_.getRealization();
}

/* Distribution accessor */
Distribution DeconditionedRandomVector::getDistribution() const
{
  return distribution_;
}

/* Random parameters accessor */
RandomVector DeconditionedRandomVector::getRandomParameters() const
{
  return randomParameters_;
}


Point DeconditionedRandomVector::getParameter() const
{
  Point parameter(distribution_.getParameter());
  parameter.add(randomParameters_.getParameter());
  return parameter;
}

void DeconditionedRandomVector::setParameter(const Point & parameter)
{
  const UnsignedInteger distributionParameterDimension = distribution_.getParameter().getDimension();
  const UnsignedInteger randomParametersParameterDimension = randomParameters_.getParameter().getDimension();
  if (parameter.getDimension() != (distributionParameterDimension + randomParametersParameterDimension))
    throw InvalidArgumentException(HERE) << "Wrong conditional random vector parameter size";
  Point distributionParameter(distributionParameterDimension);
  std::copy(parameter.begin(), parameter.begin() + distributionParameterDimension, distributionParameter.begin());
  distribution_.setParameter(distributionParameter);
  Point randomParametersParameter(randomParametersParameterDimension);
  std::copy(parameter.begin() + distributionParameterDimension, parameter.end(), randomParametersParameter.begin());
  randomParameters_.setParameter(randomParametersParameter);
}

Description DeconditionedRandomVector::getParameterDescription() const
{
  Description description(distribution_.getParameterDescription());
  description.add(randomParameters_.getParameterDescription());
  return description;
}

/* Method save() stores the object through the StorageManager */
void DeconditionedRandomVector::save(Advocate & adv) const
{
  RandomVectorImplementation::save(adv);
  adv.saveAttribute( "distribution_", distribution_ );
  adv.saveAttribute( "randomParameters_", randomParameters_ );
}

/* Method load() reloads the object from the StorageManager */
void DeconditionedRandomVector::load(Advocate & adv)
{
  RandomVectorImplementation::load(adv);
  adv.loadAttribute( "distribution_", distribution_ );
  adv.loadAttribute( "randomParameters_", randomParameters_ );
}

END_NAMESPACE_OPENTURNS
