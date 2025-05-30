#! /usr/bin/env python

import openturns as ot
import math as m

ot.TESTPREAMBLE()


def sobol(indices, a):
    value = 1.0
    for i in range(indices.getSize()):
        value *= 1.0 / (3.0 * (1.0 + a[indices[i]]) ** 2)
    return value


# Problem parameters
inputDimension = 3
outputDimension = 2

# Reference analytical values
meanTh_Sobol = 1.0
covTh_Sobol = 1.0
kappa = ot.Point(inputDimension)
a = 7.0
b = 0.1
# Create the gSobol function
inputVariables = ot.Description(inputDimension)
outputVariables = ot.Description(outputDimension)
formula = ot.Description(outputDimension)
formula[0] = "1.0"
for i in range(inputDimension):
    kappa[i] = 0.5 * i
    covTh_Sobol *= 1.0 + 1.0 / (3.0 * (1.0 + kappa[i]) ** 2)
    inputVariables[i] = "xi" + str(i)
    formula[0] = (
        formula[0]
        + " * ((abs(4.0 * xi"
        + str(i)
        + " - 2.0) + "
        + str(kappa[i])
        + ") / (1.0 + "
        + str(kappa[i])
        + "))"
    )
formula[1] = (
    "sin("
    + str(-m.pi)
    + " + 2 * "
    + str(m.pi)
    + " * xi0) + ("
    + str(a)
    + ") * (sin("
    + str(-m.pi)
    + " + 2 * "
    + str(m.pi)
    + " * xi1)) ^ 2 + ("
    + str(b)
    + ") * ("
    + str(-m.pi)
    + " + 2 * "
    + str(m.pi)
    + " * xi2)^4 * sin("
    + str(-m.pi)
    + " + 2 * "
    + str(m.pi)
    + " * xi0)"
)
covTh_Sobol -= 1
# Reference analytical values
meanTh_Ishigami = a / 2.0
covTh_Ishigami = b**2 * m.pi**8 / 18.0 + (b * m.pi**4) / 5.0 + a**2 / 8.0 + 1.0 / 2.0
sob_1_Ishigami = ot.Point(3)
sob_1_Ishigami[0] = (
    b * m.pi**4 / 5.0 + b**2 * m.pi**8 / 50.0 + 1.0 / 2.0
) / covTh_Ishigami
sob_1_Ishigami[1] = (a**2 / 8.0) / covTh_Ishigami
sob_1_Ishigami[2] = 0.0
sob_2_Ishigami = ot.Point(3)
sob_2_Ishigami[0] = 0.0
sob_2_Ishigami[1] = (b**2 * m.pi**8 / 18.0 - b**2 * m.pi**8 / 50.0) / covTh_Ishigami
sob_2_Ishigami[2] = 0.0
sob_3_Ishigami = ot.Point(1, 0.0)
# Multidimensional reference values
# Mean
meanTh = ot.Point(outputDimension)
meanTh[0] = meanTh_Sobol
meanTh[1] = meanTh_Ishigami
# Covariance
covTh = ot.CovarianceMatrix(outputDimension)
covTh[0, 0] = covTh_Sobol
covTh[1, 1] = covTh_Ishigami
# 1rst order Sobol
sob_1 = ot.Point(inputDimension * outputDimension)
indices = ot.Indices(1)
indices[0] = 0
sob_1[0] = sobol(indices, kappa) / covTh_Sobol
indices[0] = 1
sob_1[1] = sobol(indices, kappa) / covTh_Sobol
indices[0] = 2
sob_1[2] = sobol(indices, kappa) / covTh_Sobol
sob_1[3] = sob_1_Ishigami[0]
sob_1[4] = sob_1_Ishigami[1]
sob_1[5] = sob_1_Ishigami[2]
# 2nd order Sobol
sob_2 = ot.Point(inputDimension * outputDimension)
indices = ot.Indices(2)
indices[0] = 0
indices[1] = 1
sob_2[0] = sobol(indices, kappa) / covTh_Sobol
indices[1] = 2
sob_2[1] = sobol(indices, kappa) / covTh_Sobol
indices[0] = 1
indices[1] = 2
sob_2[2] = sobol(indices, kappa) / covTh_Sobol
sob_2[3] = sob_2_Ishigami[0]
sob_2[4] = sob_2_Ishigami[1]
sob_2[5] = sob_2_Ishigami[2]
# 3rd order Sobol
sob_3 = ot.Point(outputDimension)
indices = ot.Indices(3)
indices[0] = 0
indices[1] = 1
indices[2] = 2
sob_3[0] = sobol(indices, kappa) / covTh_Sobol
sob_3[1] = sob_3_Ishigami[0]
# 1rst order Total Sobol
sob_T1 = ot.Point(inputDimension * outputDimension)
sob_T1[0] = sob_1[0] + sob_2[0] + sob_2[1] + sob_3[0]
sob_T1[1] = sob_1[1] + sob_2[0] + sob_2[2] + sob_3[0]
sob_T1[2] = sob_1[2] + sob_2[1] + sob_2[2] + sob_3[0]
sob_T1[3] = sob_1[3] + sob_2[3] + sob_2[4] + sob_3[1]
sob_T1[4] = sob_1[4] + sob_2[3] + sob_2[5] + sob_3[1]
sob_T1[5] = sob_1[5] + sob_2[4] + sob_2[5] + sob_3[1]
sob_T2 = ot.Point(inputDimension * outputDimension)
sob_T2[0] = sob_2[0] + sob_3[0]
sob_T2[1] = sob_2[1] + sob_3[0]
sob_T2[2] = sob_2[2] + sob_3[0]
sob_T2[3] = sob_2[3] + sob_3[1]
sob_T2[4] = sob_2[4] + sob_3[1]
sob_T2[5] = sob_2[5] + sob_3[1]
# 3rd order Total Sobol
sob_T3 = ot.Point(sob_3)
model = ot.SymbolicFunction(inputVariables, formula)

# Create the input distribution
distribution = ot.JointDistribution([ot.Uniform(0.0, 1.0)] * inputDimension)

# Create the orthogonal basis
enumerateFunction = ot.LinearEnumerateFunction(inputDimension)
productBasis = ot.OrthogonalProductPolynomialFactory(
    [ot.LegendreFactory()] * inputDimension, enumerateFunction
)

# Create the adaptive strategy
# We can choose amongst several strategies
# First, the most efficient (but more complex!) strategy
listAdaptiveStrategy = list()
degree = 6
indexMax = enumerateFunction.getStrataCumulatedCardinal(degree)
basisDimension = enumerateFunction.getStrataCumulatedCardinal(degree // 2)
threshold = 1.0e-6
listAdaptiveStrategy.append(
    ot.CleaningStrategy(productBasis, indexMax, basisDimension, threshold)
)
# Second, the most used (and most basic!) strategy
listAdaptiveStrategy.append(
    ot.FixedStrategy(productBasis, enumerateFunction.getStrataCumulatedCardinal(degree))
)

for adaptiveStrategyIndex in range(len(listAdaptiveStrategy)):
    adaptiveStrategy = listAdaptiveStrategy[adaptiveStrategyIndex]
    # Create the projection strategy
    samplingSize = 250
    listExperiment = list()
    # LHS experiment
    listExperiment.append(ot.LHSExperiment(distribution, samplingSize))
    for experiment in listExperiment:
        ot.RandomGenerator.SetSeed(0)
        X = experiment.generate()
        Y = model(X)
        # Create the polynomial chaos algorithm
        maximumResidual = 1.0e-10
        algo = ot.FunctionalChaosAlgorithm(X, Y, distribution, adaptiveStrategy)
        algo.setMaximumResidual(maximumResidual)
        algo.run()

        # Examine the results
        result = algo.getResult()
        print("###################################")
        print(algo.getAdaptiveStrategy())
        print(algo.getProjectionStrategy())

        # Post-process the results
        vector = ot.FunctionalChaosRandomVector(result)
        sensitivity = ot.FunctionalChaosSobolIndices(result)
        for outputIndex in range(outputDimension):
            print("output=", outputIndex)
            mean = vector.getMean()[outputIndex]
            print(
                "mean= %.5f" % mean,
                "absolute error=%.5e" % abs(mean - meanTh[outputIndex]),
            )
            variance = vector.getCovariance()[outputIndex, outputIndex]
            print(
                "variance=%.5f" % variance,
                "absolute error=%.5e" % abs(variance - covTh[outputIndex, outputIndex]),
            )
            indices = ot.Indices(1)
            for i in range(inputDimension):
                indices[0] = i
                value = sensitivity.getSobolIndex(i, outputIndex)
                print("value= %.5g" % value)
                print(
                    "Sobol index ",
                    i,
                    " =%.5f" % value,
                    "absolute error=%.5e"
                    % abs(value - sob_1[i + inputDimension * outputIndex]),
                )
            indices = ot.Indices(2)
            k = 0
            for i in range(inputDimension):
                indices[0] = i
                for j in range(i + 1, inputDimension):
                    indices[1] = j
                    value = sensitivity.getSobolIndex(indices, outputIndex)
                    print(
                        "Sobol index ",
                        indices,
                        " =%.5f" % value,
                        "absolute error=%.5e"
                        % abs(value - sob_2[k + inputDimension * outputIndex]),
                    )
                    k += 1
            indices = ot.Indices([0, 1, 2])
            value = sensitivity.getSobolIndex(indices, outputIndex)
            print(
                "Sobol index ",
                indices,
                " =%.5f" % value,
                "absolute error=%.5e" % abs(value - sob_3[outputIndex]),
            )
            for i in range(inputDimension):
                value = sensitivity.getSobolTotalIndex(i, outputIndex)
                print(
                    "Sobol total index ",
                    i,
                    " =%.5f" % value,
                    "absolute error=%.5e"
                    % abs(value - sob_T1[i + inputDimension * outputIndex]),
                )
            indices = ot.Indices(2)
            k = 0
            for i in range(inputDimension):
                indices[0] = i
                for j in range(i + 1, inputDimension):
                    indices[1] = j
                    value = sensitivity.getSobolTotalIndex(indices, outputIndex)
                    print(
                        "Sobol total index ",
                        indices,
                        " =%.5f" % value,
                        "absolute error=%.5e"
                        % abs(value - sob_T2[k + inputDimension * outputIndex]),
                    )
                    k += 1
            indices = ot.Indices([0, 1, 2])
            value = sensitivity.getSobolTotalIndex(indices, outputIndex)
            print(
                "Sobol total index ",
                indices,
                " =%.5f" % value,
                "absolute error=%.5e" % abs(value - sob_T3[1]),
            )
