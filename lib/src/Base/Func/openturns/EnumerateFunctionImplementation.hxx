//                                               -*- C++ -*-
/**
 *  @brief The bijective function to select polynomials in the orthogonal basis
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
#ifndef OPENTURNS_ENUMERATEFUNCTIONIMPLEMENTATION_HXX
#define OPENTURNS_ENUMERATEFUNCTIONIMPLEMENTATION_HXX

#include "openturns/PersistentObject.hxx"
#include "openturns/Indices.hxx"

BEGIN_NAMESPACE_OPENTURNS

// Forward declaration
class EnumerateFunction;

/**
 * @class EnumerateFunctionImplementation
 *
 * The bijective function to select polynomials in the orthogonal basis
 */

class OT_API EnumerateFunctionImplementation
  : public PersistentObject
{
  CLASSNAME
public:


  /** Parameter constructor */
  explicit EnumerateFunctionImplementation(const UnsignedInteger dimension = 1);

  /** Virtual constructor */
  EnumerateFunctionImplementation * clone() const override;

  /** String converter */
  String __repr__() const override;

  /** The bijective association between an integer and a set of indices */
  virtual Indices operator() (const UnsignedInteger index) const;

  /** The inverse of the association */
  virtual UnsignedInteger inverse(const Indices & indices) const;

  /** The cardinal of the given strata */
  virtual UnsignedInteger getStrataCardinal(const UnsignedInteger strataIndex) const;

  /** The cardinal of the cumulated strata above or equal to the given strate */
  virtual UnsignedInteger getStrataCumulatedCardinal(const UnsignedInteger strataIndex) const;

  /** The index of the strata of degree max <= maximumDegree */
  virtual UnsignedInteger getMaximumDegreeStrataIndex(const UnsignedInteger maximumDegree) const;

  /** The cardinal of indices of degree max <= maximumDegree */
  virtual UnsignedInteger getMaximumDegreeCardinal(const UnsignedInteger maximumDegree) const;

  /** Basis size from total degree */
  virtual UnsignedInteger getBasisSizeFromTotalDegree(const UnsignedInteger maximumDegree) const;

  /** The marginal enumerate function */
  virtual EnumerateFunction getMarginal(const Indices & indices) const;

  /** The marginal enumerate function */
  virtual EnumerateFunction getMarginal(const UnsignedInteger i) const;

  /** Dimension accessor */
  void setDimension(const UnsignedInteger dimension);
  UnsignedInteger getDimension() const;

  /** Upper bound accessor */
  virtual void setUpperBound(const Indices & upperBound);
  virtual Indices getUpperBound() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:

  // Bounds
  Indices upperBound_;
private:

  // The enumerate function maps N into N^dimension
  UnsignedInteger dimension_;


} ; /* class EnumerateFunctionImplementation */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_ENUMERATEFUNCTIONIMPLEMENTATION_HXX */
