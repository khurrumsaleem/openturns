//                                               -*- C++ -*-
/**
 *  @brief Result of GEV time-varying likelihood
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
#include "openturns/TimeVaryingResult.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/GumbelMuSigma.hxx"
#include "openturns/Brent.hxx"
#include "openturns/Curve.hxx"
#include "openturns/Normal.hxx"
#include "openturns/GeneralizedExtremeValueValidation.hxx"
#include "openturns/GeneralizedExtremeValue.hxx"
#include "openturns/GeneralizedParetoValidation.hxx"
#include "openturns/GeneralizedPareto.hxx"


BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(TimeVaryingResult)

static const Factory<TimeVaryingResult> Factory_TimeVaryingResult;


TimeVaryingResult::TimeVaryingResult()
  : PersistentObject()
{
  // Nothing to do
}

TimeVaryingResult::TimeVaryingResult(const DistributionFactory & factory,
                                     const Sample & data,
                                     const Function & parameterFunction,
                                     const Sample & timeGrid,
                                     const Distribution & parameterDistribution,
                                     const LinearFunction & normalizationFunction,
                                     const Scalar logLikelihood)
  : PersistentObject()
  , factory_(factory)
  , data_(data)
  , parameterFunction_(parameterFunction)
  , timeGrid_(timeGrid)
  , parameterDistribution_(parameterDistribution)
  , normalizationFunction_(normalizationFunction)
  , logLikelihood_(logLikelihood)
{
  if (data.getDimension() != 1)
    throw InvalidArgumentException(HERE) << "TimeVaryingResult: the data should be of dimension 1 got " << data.getDimension();
  if (data.getSize() != timeGrid.getSize())
    throw InvalidArgumentException(HERE) << "TimeVaryingResult: the time grid size (" << timeGrid.getSize() << ") must match the data size (" << data.getSize() << ")";
  if (timeGrid.getDimension() != normalizationFunction.getInputDimension())
    throw InvalidArgumentException(HERE) << "TimeVaryingResult: the time grid dimension (" << timeGrid.getDimension() << ") must match the normalization function input dimension (" << normalizationFunction.getInputDimension() << ")";
  if (normalizationFunction.getInputDimension() != normalizationFunction.getOutputDimension())
    throw InvalidArgumentException(HERE) << "TimeVaryingResult: the normalization function must have the same input dimension (" << normalizationFunction.getInputDimension() << ") as output dimension (" << normalizationFunction.getOutputDimension() << ")";
  if (parameterDistribution.getDimension() != parameterFunction.getParameter().getDimension())
    throw InvalidArgumentException(HERE) << "TimeVaryingResult: the parameter distribution dimension (" << parameterDistribution.getDimension() << ") must match the parameter function parameter dimension (" << parameterFunction.getParameter().getDimension() << ")";
}

TimeVaryingResult * TimeVaryingResult::clone() const
{
  return new TimeVaryingResult(*this);
}

Point TimeVaryingResult::getOptimalParameter() const
{
  return parameterDistribution_.getMean();
}

void TimeVaryingResult::setParameterDistribution(const Distribution & parameterDistribution)
{
  parameterDistribution_ = parameterDistribution;
}

Distribution TimeVaryingResult::getParameterDistribution() const
{
  return parameterDistribution_;
}

void TimeVaryingResult::setLogLikelihood(const Scalar logLikelihood)
{
  logLikelihood_ = logLikelihood;
}

Scalar TimeVaryingResult::getLogLikelihood() const
{
  return logLikelihood_;
}

/* Draw parameter for all time values */
Graph TimeVaryingResult::drawParameterFunction(const UnsignedInteger parameterIndex) const
{
  const Scalar xMin = timeGrid_.getMin()[0];
  const Scalar xMax = timeGrid_.getMax()[0];
  Graph result(parameterFunction_.getMarginal(parameterIndex).draw(xMin, xMax));
  result.setTitle("Parameter function");
  return result;
}

class TimeVaryingResultQuantileEvaluation : public EvaluationImplementation
{
public:
  TimeVaryingResultQuantileEvaluation(const TimeVaryingResult & result, const Scalar p)
    : result_(result)
    , p_(p)
  {
    setInputDescription({"t"});
    setOutputDescription({"quantile(t)"});
  }

  TimeVaryingResultQuantileEvaluation * clone() const override
  {
    return new TimeVaryingResultQuantileEvaluation(*this);
  }

  Point operator()(const Point & inP) const override
  {
    const Scalar t = inP[0];
    return result_.getDistribution(t).computeQuantile(p_);
  }

  UnsignedInteger getInputDimension() const override
  {
    return 1;
  }

  UnsignedInteger getOutputDimension() const override
  {
    return 1;
  }

private:
  TimeVaryingResult result_;
  Scalar p_ = 0.0;
};

/* Draw quantile for all time values */
Graph TimeVaryingResult::drawQuantileFunction(const Scalar p) const
{
  const Scalar xMin = timeGrid_.getMin()[0];
  const Scalar xMax = timeGrid_.getMax()[0];

  Function quantileFunction(TimeVaryingResultQuantileEvaluation(*this, p));
  Graph result(quantileFunction.draw(xMin, xMax));
  result.setTitle("Quantile function");
  return result;
}

GridLayout TimeVaryingResult::drawDiagnosticPlot() const
{
  const UnsignedInteger size = timeGrid_.getSize();
  const Normal dummy(3);
  Distribution standard;
  const String distributionType = factory_.build().getName();
  String standardType;
  if (distributionType == "GeneralizedExtremeValue")
  {
    standard = GeneralizedExtremeValue(0.0, 1.0, 0.0);
    standardType = "Gumbel";
  }
  else if (distributionType == "GeneralizedPareto")
  {
    standard = GeneralizedPareto(1.0, 0.0, 0.0);
    standardType = "Exponential";
  }
  else
    throw InvalidArgumentException(HERE) << "TimeVaryingResult expected GEV or GPD, got" << distributionType;

  const DistributionFactoryResult factoryResult(standard, dummy);
  GridLayout grid;
  if (distributionType == "GeneralizedExtremeValue")
  {
    // see eq 6.6 in coles2001 paragraph 6.2.3 p110
    Sample zT(size, 1);
    for (UnsignedInteger i = 0; i < size; ++ i)
    {
      const Scalar t = timeGrid_(i, 0);
      const Point parameters(parameterFunction_(Point({t})));
      const Scalar mu = parameters[0];
      const Scalar sigma = parameters[1];
      const Scalar xi = parameters[2];
      zT(i, 0) = 1.0 / xi * std::log1p(xi * (data_(i, 0) - mu) / sigma);
    }
    const GeneralizedExtremeValueValidation validation(factoryResult, zT);
    grid = validation.drawDiagnosticPlot();
  }
  else if (distributionType == "GeneralizedPareto")
  {
    // see coles2001 paragraph 6.2.3 p111
    Sample yT(size, 1);
    for (UnsignedInteger i = 0; i < size; ++ i)
    {
      const Scalar t = timeGrid_(i, 0);
      const Point parameters(parameterFunction_(Point({t})));
      const Scalar sigma = parameters[0];
      const Scalar xi = parameters[1];
      const Scalar u = parameters[2];
      yT(i, 0) = 1.0 / xi * std::log1p(xi * (data_(i, 0) - u) / sigma);
    }
    const GeneralizedParetoValidation validation(factoryResult, yT);
    grid = validation.drawDiagnosticPlot();
  }

  // Now adapt the axes titles and the legend
  Graph ppPlot(grid.getGraph(0, 0));
  ppPlot.setYTitle(standardType + " probability");
  grid.setGraph(0, 0, ppPlot);
  Graph qqPlot(grid.getGraph(0, 1));
  qqPlot.setYTitle(standardType + " quantile");
  grid.setGraph(0, 1, qqPlot);
  Graph densityPlot(grid.getGraph(1, 1));
  Description legends(densityPlot.getLegends());
  legends[0] = standardType + " PDF";
  densityPlot.setLegends(legends);
  grid.setGraph(1, 1, densityPlot);
  return grid;
}

String TimeVaryingResult::__repr__() const
{
  return OSS() << PersistentObject::__repr__();
}

Function TimeVaryingResult::getParameterFunction() const
{
  return parameterFunction_;
}

Sample TimeVaryingResult::getTimeGrid() const
{
  return timeGrid_;
}

LinearFunction TimeVaryingResult::getNormalizationFunction() const
{
  return normalizationFunction_;
}

/* Accessor to the distribution at a given time */
Distribution TimeVaryingResult::getDistribution(const Scalar t) const
{
  const Point parameters(parameterFunction_(Point({t})));
  return factory_.build(parameters);
}

/* Method save() stores the object through the StorageManager */
void TimeVaryingResult::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute("factory_", factory_);
  adv.saveAttribute("data_", data_);
  adv.saveAttribute("parameterFunction_", parameterFunction_);
  adv.saveAttribute("timeGrid_", timeGrid_);
  adv.saveAttribute("parameterDistribution_", parameterDistribution_);
  adv.saveAttribute("normalizationFunction_", normalizationFunction_);
  adv.saveAttribute("logLikelihood_", logLikelihood_);
}

/* Method load() reloads the object from the StorageManager */
void TimeVaryingResult::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute("factory_", factory_);
  adv.loadAttribute("data_", data_);
  adv.loadAttribute("parameterFunction_", parameterFunction_);
  adv.loadAttribute("timeGrid_", timeGrid_);
  adv.loadAttribute("parameterDistribution_", parameterDistribution_);
  adv.loadAttribute("normalizationFunction_", normalizationFunction_);
  adv.loadAttribute("logLikelihood_", logLikelihood_);
}

END_NAMESPACE_OPENTURNS
