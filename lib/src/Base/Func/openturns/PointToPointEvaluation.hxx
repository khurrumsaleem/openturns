//                                               -*- C++ -*-
/**
 *  @brief The class that implements the composition between
 *         functions implementations or a point to field function
 *         and a field to point function
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

#ifndef OPENTURNS_POINTTOPOINTEVALUATION_HXX
#define OPENTURNS_POINTTOPOINTEVALUATION_HXX

#include "openturns/EvaluationImplementation.hxx"
#include "openturns/Evaluation.hxx"
#include "openturns/PointToFieldFunction.hxx"
#include "openturns/FieldToPointFunction.hxx"
#include "openturns/Function.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class PointToPointEvaluation
 *
 * The class that implement the composition of two functions or a field to point and a point to field function.
 */
class OT_API PointToPointEvaluation
  : public EvaluationImplementation
{
  CLASSNAME
public:

  /** Default constructor */
  PointToPointEvaluation();

  /** Parameter constructor */
  PointToPointEvaluation(const Function & leftFunction,
                         const Function & rightFunction);

  /** parameter constructor */
  PointToPointEvaluation(const FieldToPointFunction & fieldToPoint,
                         const PointToFieldFunction & pointToField);

  /** Virtual constructor */
  PointToPointEvaluation * clone() const override;

  /** Comparison operator */
  using EvaluationImplementation::operator ==;
  Bool operator ==(const PointToPointEvaluation & other) const;

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;

  /** Operator () */
  using EvaluationImplementation::operator();
  Point operator() (const Point & inP) const override;

  Sample operator() (const Sample & inS) const override;

  /** Parameters value accessor */
  Point getParameter() const override;
  void setParameter(const Point & parameter) override;

  /** Parameters description accessor */
  Description getParameterDescription() const override;
  void setParameterDescription(const Description & description) override;

  /** Accessor for input point dimension */
  UnsignedInteger getInputDimension() const override;

  /** Accessor for output point dimension */
  UnsignedInteger getOutputDimension() const override;

  /** Get the i-th marginal function */
  Evaluation getMarginal(const UnsignedInteger i) const override;

  /** Get the function corresponding to indices components */
  Evaluation getMarginal(const Indices & indices) const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

  /** Composed implementation accessor */
  Function getLeftFunction() const;
  Function getRightFunction() const;
  PointToFieldFunction getPointToFieldFunction() const;
  FieldToPointFunction getFieldToPointFunction() const;

protected:


private:

  /** Flag to tell if it is a composition of functions or a
   * composition of point to field and field to point functions */
  Bool isFunctionComposition_;

  /** The function f in h = f o g */
  Function leftFunction_;

  /** The function g in h = f o g */
  Function rightFunction_;

  /** The field to point function */
  FieldToPointFunction fieldToPointFunction_;

  /** The point to field function */
  PointToFieldFunction pointToFieldFunction_;

}; /* class PointToPointEvaluation */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_POINTTOPOINTEVALUATION_HXX */
