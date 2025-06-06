//                                               -*- C++ -*-
/**
 *  @brief Class for the Nataf transformation hessian for elliptical
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
#ifndef OPENTURNS_NATAFELLIPTICALDISTRIBUTIONHESSIAN_HXX
#define OPENTURNS_NATAFELLIPTICALDISTRIBUTIONHESSIAN_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/ConstantHessian.hxx"
#include "openturns/StorageManager.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class NatafEllipticalDistributionHessian
 *
 * This class offers an interface for the Nataf gradient for elliptical distributions
 */
class OT_API NatafEllipticalDistributionHessian
  : public ConstantHessian
{
  CLASSNAME
public:


  /** Default constructor */
  NatafEllipticalDistributionHessian();

  /** Parameter constructor */
  explicit NatafEllipticalDistributionHessian(const UnsignedInteger dimension);

  /** Virtual constructor */
  NatafEllipticalDistributionHessian * clone() const override;

  /** String converter */
  String __repr__() const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:


private:

}; /* NatafEllipticalDistributionHessian */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_NATAFELLIPTICALDISTRIBUTIONHESSIAN_HXX */
