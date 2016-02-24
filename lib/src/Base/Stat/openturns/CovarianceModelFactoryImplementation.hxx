//                                               -*- C++ -*-
/**
 *  @brief Top-level class for all spectral model factories
 *
 *  Copyright 2005-2016 Airbus-EDF-IMACS-Phimeca
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
 *  You should have received a copy of the GNU Lesser General Public
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#ifndef OPENTURNS_COVARIANCEMODELFACTORYIMPLEMENTATION_HXX
#define OPENTURNS_COVARIANCEMODELFACTORYIMPLEMENTATION_HXX

#include "openturns/CovarianceModel.hxx"
#include "openturns/ProcessSample.hxx"
#include "openturns/Field.hxx"
#include "openturns/NumericalPoint.hxx"
#include "openturns/PersistentObject.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class CovarianceModelFactoryImplementation
 */
class OT_API CovarianceModelFactoryImplementation
  : public PersistentObject
{
  CLASSNAME;
public:

  typedef Pointer<CovarianceModelFactoryImplementation>    Implementation;

  /** Default constructor */
  CovarianceModelFactoryImplementation();

  /** Virtual constructor */
  virtual CovarianceModelFactoryImplementation * clone() const;

  /** String converter */
  virtual String __repr__() const;

  /** String converter */
  virtual String __str__(const String & offset = "") const;

  /* Here is the interface that all derived class must implement */
  /** Build a spectral model based on a sample */
  virtual CovarianceModelImplementation::Implementation build(const ProcessSample & sample) const;

  /** Build a spectral model based on a Field */
  virtual CovarianceModelImplementation::Implementation build(const Field & timeSerie) const;

  /** Method save() stores the object through the StorageManager */
  virtual void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  virtual void load(Advocate & adv);

protected:

}; /* class CovarianceModelFactoryImplementation */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_SPECTRALMODELFACTORYIMPLEMENTATION_HXX */
