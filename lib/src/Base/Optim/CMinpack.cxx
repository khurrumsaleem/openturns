//                                               -*- C++ -*-
/**
 *  @brief CMinpack solver
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
#include "openturns/CMinpack.hxx"
#include "openturns/Point.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Log.hxx"
#include "openturns/SpecFunc.hxx"

#ifdef OPENTURNS_HAVE_CMINPACK
# include <cminpack.h>
#endif

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(CMinpack)

static const Factory<CMinpack> Factory_CMinpack;


/* Default constructor */
CMinpack::CMinpack()
  : OptimizationAlgorithmImplementation()
{
  // Nothing to do
}

CMinpack::CMinpack(const OptimizationProblem & problem)
  : OptimizationAlgorithmImplementation(problem)
{
  checkProblem(problem);
}

/* Virtual constructor */
CMinpack * CMinpack::clone() const
{
  return new CMinpack(*this);
}

/* Check whether this problem can be solved by this solver.  Must be overloaded by the actual optimisation algorithm */
void CMinpack::checkProblem(const OptimizationProblem & problem) const
{
#ifdef OPENTURNS_HAVE_CMINPACK
  if (!problem.hasResidualFunction())
    throw InvalidArgumentException(HERE) << getClassName() << " only supports least-square problems";

  if (!(problem.getResidualFunction().getInputDimension() <= problem.getResidualFunction().getOutputDimension()))
    throw InvalidArgumentException(HERE) << getClassName() << " does not support underdetermined least squares problems";

  if (problem.hasBounds())
  {
    const UnsignedInteger dimension = problem.getDimension();
    const Interval bounds(problem.getBounds());
    for (UnsignedInteger j = 0; j < dimension; ++ j)
    {
      if ((bounds.getFiniteLowerBound()[j] && !bounds.getFiniteUpperBound()[j])
          || (!bounds.getFiniteLowerBound()[j] && bounds.getFiniteUpperBound()[j]))
        throw InvalidArgumentException(HERE) << getClassName() << " only supports box bounds";
    }
  }

  if (problem.hasInequalityConstraint() || problem.hasEqualityConstraint())
    throw InvalidArgumentException(HERE) << getClassName() << " does not support constraints";

  if (!problem.isContinuous())
    throw InvalidArgumentException(HERE) << "Error: " << getClassName() << " does not support non continuous problems";

#else
  (void)problem;
  throw NotYetImplementedException(HERE) << "No CMinpack support";
#endif
}


void CMinpack::Transform(Point & x, int n, const Interval & bounds, Point & jacfac)
{
  // this transformation allows one to handle box constraints
  // http://apps.jcns.fz-juelich.de/doku/sc/lmfit:constraints

  // u = xmiddle + xwidth * tanh(x)
  jacfac = Point(n, 1.0);
  const Point xmin(bounds.getLowerBound());
  const Point xmax(bounds.getUpperBound());
  for (int j = 0; j < n; ++ j)
  {
    if (bounds.getFiniteLowerBound()[j])
    {
      const Scalar xmiddle = (xmin[j] + xmax[j]) * 0.5;
      const Scalar xwidth = (xmax[j] - xmin[j]) * 0.5;
      const Scalar th = std::tanh(x[j]);
      x[j] = xmiddle + th * xwidth;
      jacfac[j] = xwidth * (1.0 - th * th);
    }
  }
}


void CMinpack::InverseTransform(Point & x, int n, const Interval & bounds)
{
  // x = atanh((u - xmiddle / xwidth))
  const Point xmin(bounds.getLowerBound());
  const Point xmax(bounds.getUpperBound());
  for (int j = 0; j < n; ++ j)
  {
    if (bounds.getFiniteLowerBound()[j])
    {
      const Scalar xmiddle = (xmin[j] + xmax[j]) * 0.5;
      const Scalar xwidth = (xmax[j] - xmin[j]) * 0.5;
      Scalar v = (x[j] - xmiddle) / xwidth;
      // clip v inside ]-1;1[
      if (v <= -1.0)
        v = -1.0 + SpecFunc::Precision;
      if (v >= 1.0)
        v = 1.0 - SpecFunc::Precision;
      x[j] = std::atanh(v);
    }
  }
}

int CMinpack::ComputeObjectiveJacobian(void *p, int m, int n, const Scalar *x, Scalar *fvec, Scalar *fjac, int /*ldfjac*/, int iflag)
{
  CMinpack *algorithm = static_cast<CMinpack *>(p);
  if (!algorithm)
    throw InternalException(HERE) << "CMinpack p is null";

  Point inP(n);
  std::copy(x, x + n, inP.begin());
  Point jacfac;
  if (algorithm->getProblem().hasBounds())
  {
    Transform(inP, n, algorithm->getProblem().getBounds(), jacfac);
  }

  if (iflag == 1)
  {
    // evaluation
    Point outP;
    try
    {
      outP = algorithm->getProblem().getResidualFunction()(inP);

      // track input/outputs
      algorithm->evaluationInputHistory_.add(inP);
      algorithm->evaluationOutputHistory_.add(Point(1, 0.5 * outP.normSquare()));

      // update result
      algorithm->result_.setCallsNumber(algorithm->evaluationInputHistory_.getSize());
      algorithm->result_.store(inP, Point(1, 0.5 * outP.normSquare()), 0.0, 0.0, 0.0, 0.0);
    }
    catch (const std::exception & exc)
    {
      LOGWARN(OSS() << "CMinpack failed to evaluate residual for x=" << inP.__str__() << " msg=" << exc.what());
      outP = Point(algorithm->getProblem().getResidualFunction().getOutputDimension(), std::sqrt(SpecFunc::MaxScalar));
    }
    std::copy(outP.begin(), outP.end(), fvec);
  }
  else if (iflag == 2)
  {
    // gradient
    Matrix jacobian;
    try
    {
      jacobian = algorithm->getProblem().getResidualFunction().gradient(inP).transpose();
    }
    catch (const std::exception & exc)
    {
      LOGWARN(OSS() << "CMinpack failed to evaluate residual gradient for x=" << inP.__str__() << " msg=" << exc.what());
    }
    if (algorithm->getProblem().hasBounds())
    {
      for (int j = 0; j < n; ++ j)
        for (int i = 0; i < m; ++ i)
          jacobian(i, j) *= jacfac[j];
    }
    std::copy(jacobian.data(), jacobian.data() + m * n, fjac);
  }

  std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();
  const Scalar timeDuration = std::chrono::duration<Scalar>(t1 - algorithm->t0_).count();
  if ((algorithm->getMaximumTimeDuration() > 0.0) && (timeDuration > algorithm->getMaximumTimeDuration()))
    return -1;

  // callbacks
  if (algorithm->progressCallback_.first)
  {
    algorithm->progressCallback_.first((100.0 * algorithm->evaluationInputHistory_.getSize()) / algorithm->getMaximumCallsNumber(), algorithm->progressCallback_.second);
  }
  if (algorithm->stopCallback_.first && algorithm->stopCallback_.first(algorithm->stopCallback_.second))
  {
    LOGWARN("CMinpack was stopped by user");
    return -1;
  }
  return 0;
}


/* Performs the actual computation by calling the CMinpack library
 */
void CMinpack::run()
{
#ifdef OPENTURNS_HAVE_CMINPACK
  const UnsignedInteger dimension = getProblem().getDimension();
  Point startingPoint(getStartingPoint());
  if (startingPoint.getDimension() != dimension)
    throw InvalidArgumentException(HERE) << "Invalid starting point dimension (" << startingPoint.getDimension() << "), expected " << dimension;

  const Interval bounds(getProblem().getBounds());
  if (getProblem().hasBounds() && !bounds.contains(startingPoint))
    throw InvalidArgumentException(HERE) << "Starting point is not inside bounds x=" << startingPoint.__str__() << " bounds=" << bounds;

  // initialize history
  evaluationInputHistory_ = Sample(0, dimension);
  evaluationOutputHistory_ = Sample(0, 1);
  result_ = OptimizationResult(getProblem());

  Point x(startingPoint);
  const int m = getProblem().getResidualFunction().getOutputDimension();
  const int n = getProblem().getResidualFunction().getInputDimension();

  if (!(n <= m))
    throw InvalidArgumentException(HERE) << "CMinpack does not support underdetermined least squares problems";

  int info = 0;

  /* Parameters designated as input parameters must be specified on
    entry to LMDER and are not changed on exit, while parameters
    designated as output parameters need not be specified on entry
    and are set to appropriate values on exit from LMDER.
    FCN is the name of the user-supplied subroutine which calculate
      the functions and the Jacobian.  FCN must be declared in an
      EXTERNAL statement in the user calling program, and should be
      written as follows.
      SUBROUTINE FCN(M,N,X,FVEC,FJAC,LDFJAC,IFLAG)
      INTEGER M,N,LDFJAC,IFLAG
      DOUBLE PRECISION X(N),FVEC(M),FJAC(LDFJAC,N)
      ----------
      IF IFLAG = 1 CALCULATE THE FUNCTIONS AT X AND
      RETURN THIS VECTOR IN FVEC.  DO NOT ALTER FJAC.
      IF IFLAG = 2 CALCULATE THE JACOBIAN AT X AND
      RETURN THIS MATRIX IN FJAC.  DO NOT ALTER FVEC.
      ----------
      RETURN
      END

                                                              Page
      The value of IFLAG should not be changed by FCN unless the
      user wants to terminate execution of LMDER.  In this case set
      IFLAG to a negative integer.
    M is a positive integer input variable set to the number of
      functions.
    N is a positive integer input variable set to the number of
      variables.  N must not exceed M.
    X is an array of length N.  On input X must contain an initial
      estimate of the solution vector.  On output X contains the
      final estimate of the solution vector.
    FVEC is an output array of length M which contains the function
      evaluated at the output X.
    FJAC is an output M by N array.  The upper N by N submatrix of
      FJAC contains an upper triangular matrix R with diagonal ele-
      ments of nonincreasing magnitude such that
              T     T           T
            P *(JAC *JAC)*P = R *R,
      where P is a permutation matrix and JAC is the final calcu-
      lated Jacobian.  Column j of P is column IPVT(j) (see below)
      of the identity matrix.  The lower trapezoidal part of FJAC
      contains information generated during the computation of R.
    LDFJAC is a positive integer input variable not less than M
      which specifies the leading dimension of the array FJAC.
    FTOL is a nonnegative input variable.  Termination occurs when
      both the actual and predicted relative reductions in the sum
      of squares are at most FTOL.  Therefore, FTOL measures the
      relative error desired in the sum of squares.  Section 4 con-
      tains more details about FTOL.
    XTOL is a nonnegative input variable.  Termination occurs when
      the relative error between two consecutive iterates is at most
      XTOL.  Therefore, XTOL measures the relative error desired in
      the approximate solution.  Section 4 contains more details
      about XTOL.
    GTOL is a nonnegative input variable.  Termination occurs when
      the cosine of the angle between FVEC and any column of the
      Jacobian is at most GTOL in absolute value.  Therefore, GTOL
      measures the orthogonality desired between the function vector
      and the columns of the Jacobian.  Section 4 contains more
      details about GTOL.
    MAXFEV is a positive integer input variable.  Termination occur
      when the number of calls to FCN with IFLAG = 1 has reached
      MAXFEV.

                                                              Page
    DIAG is an array of length N.  If MODE = 1 (see below), DIAG is
      internally set.  If MODE = 2, DIAG must contain positive
      entries that serve as multiplicative scale factors for the
      variables.
    MODE is an integer input variable.  If MODE = 1, the variables
      will be scaled internally.  If MODE = 2, the scaling is speci-
      fied by the input DIAG.  Other values of MODE are equivalent
      to MODE = 1.
    FACTOR is a positive input variable used in determining the ini-
      tial step bound.  This bound is set to the product of FACTOR
      and the Euclidean norm of DIAG*X if nonzero, or else to FACTO
      itself.  In most cases FACTOR should lie in the interval
      (.1,100.).  100. is a generally recommended value.
    NPRINT is an integer input variable that enables controlled
      printing of iterates if it is positive.  In this case, FCN is
      called with IFLAG = 0 at the beginning of the first iteration
      and every NPRINT iterations thereafter and immediately prior
      to return, with X, FVEC, and FJAC available for printing.
      FVEC and FJAC should not be altered.  If NPRINT is not posi-
      tive, no special calls of FCN with IFLAG = 0 are made.
    INFO is an integer output variable.  If the user has terminated
      execution, INFO is set to the (negative) value of IFLAG.  See
      description of FCN.  Otherwise, INFO is set as follows.
      INFO = 0  Improper input parameters.
      INFO = 1  Both actual and predicted relative reductions in th
                sum of squares are at most FTOL.
      INFO = 2  Relative error between two consecutive iterates is
                at most XTOL.jacfac
      INFO = 3  Conditions for INFO = 1 and INFO = 2 both hold.
      INFO = 4  The cosine of the angle between FVEC and any column
                of the Jacobian is at most GTOL in absolute value.
      INFO = 5  Number of calls to FCN with IFLAG = 1 has reached
                MAXFEV.
      INFO = 6  FTOL is too small.  No further reduction in the sum
                of squares is possible.
      INFO = 7  XTOL is too small.  No further improvement in the
                approximate solution X is possible.
      INFO = 8  GTOL is too small.  FVEC is orthogonal to the
                columns of the Jacobian to machine precision.
      Sections 4 and 5 contain more details about INFO.

                                                              Page
    NFEV is an integer output variable set to the number of calls t
      FCN with IFLAG = 1.
    NJEV is an integer output variable set to the number of calls t
      FCN with IFLAG = 2.
    IPVT is an integer output array of length N.  IPVT defines a
      permutation matrix P such that JAC*P = Q*R, where JAC is the
      final calculated Jacobian, Q is orthogonal (not stored), and
      is upper triangular with diagonal elements of nonincreasing
      magnitude.  Column j of P is column IPVT(j) of the identity
      matrix.
    QTF is an output array of length N which contains the first N
      elements of thCMinpack::e vector (Q transpose)*FVEC.
    WA1, WA2, and WA3 are work arrays of length N.
    WA4 is a work array of length M. */
  Point fvec(m);
  Point fjac(m * n);
  Point diag(n);
  int nfev = 0;
  int njev = 0;
  std::vector<int> ipvt(n);
  const int ldfjac = m;
  const Scalar ftol = getMaximumResidualError();
  const Scalar xtol = getMaximumAbsoluteError();
  const Scalar gtol = getMaximumConstraintError();
  const int maxfev = getMaximumCallsNumber();
  const int mode = 1;
  const Scalar factor = 100.0;
  const int nprint = 0;
  Point qtf(n);
  Point wa1(n);
  Point wa2(n);
  Point wa3(n);
  Point wa4(m);
  if (getProblem().hasBounds())
  {
    InverseTransform(x, n, bounds);
  }
  t0_ = std::chrono::steady_clock::now();

  info = lmder(&CMinpack::ComputeObjectiveJacobian, this, m, n, &x[0], &fvec[0], &fjac[0], ldfjac, ftol, xtol, gtol,
               maxfev, &diag[0], mode, factor, nprint, &nfev, &njev,
               &ipvt[0], &qtf[0], &wa1[0], &wa2[0], &wa3[0], &wa4[0]);

  setResultFromEvaluationHistory(evaluationInputHistory_, evaluationOutputHistory_);

  std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();
  const Scalar timeDuration = std::chrono::duration<Scalar>(t1 - t0_).count();
  result_.setTimeDuration(timeDuration);

  switch (info)
  {
    case -1:
      // user stop
      result_.setStatus(OptimizationResult::INTERRUPTION);
      result_.setStatusMessage("user stop");
      break;
    case 0:
      result_.setStatusMessage("improper input parameters");
      throw InvalidArgumentException(HERE) << "CMinpack: " << result_.getStatusMessage();
    case 1:
      result_.setStatusMessage("ftol termination condition is satisfied");
      break;
    case 2:
      result_.setStatusMessage("xtol termination condition is satisfied");
      break;
    case 3:
      result_.setStatusMessage("Both ftol and xtol termination conditions are satisfied");
      break;
    case 4:
      result_.setStatusMessage("gtol termination condition is satisfied");
      break;
    case 5:
      result_.setStatus(OptimizationResult::MAXIMUMCALLS);
      result_.setStatusMessage("maximum function evaluations exceeded");
      break;
    case 6:
      result_.setStatusMessage("ftol is too small");
      throw InvalidArgumentException(HERE) << "CMinpack: " << result_.getStatusMessage();
    case 7:
      result_.setStatusMessage("xtol is too small");
      throw InvalidArgumentException(HERE) << "CMinpack: " << result_.getStatusMessage();
    case 8:
      result_.setStatusMessage("gtol is too small");
      throw InvalidArgumentException(HERE) << "CMinpack: " << result_.getStatusMessage();
    default:
      result_.setStatusMessage("Unknown");
      throw NotYetImplementedException(HERE) << "CMinpack: unknown status code:" << info;
  }
  LOGDEBUG(OSS() << "CMinpack status: " << result_.getStatusMessage());
#else
  throw NotYetImplementedException(HERE) << "No CMinpack support";
#endif
}

/* String converter */
String CMinpack::__repr__() const
{
  OSS oss;
  oss << "class=" << getClassName()
      << " " << OptimizationAlgorithmImplementation::__repr__();
  return oss;
}

/* String converter */
String CMinpack::__str__(const String & ) const
{
  OSS oss(false);
  oss << "class=" << getClassName();
  return oss;
}

/* Method save() stores the object through the StorageManager */
void CMinpack::save(Advocate & adv) const
{
  OptimizationAlgorithmImplementation::save(adv);
}

/* Method load() reloads the object from the StorageManager */
void CMinpack::load(Advocate & adv)
{
  OptimizationAlgorithmImplementation::load(adv);
}

END_NAMESPACE_OPENTURNS

