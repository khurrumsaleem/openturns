"""
Use case : cantilever beam
==========================
"""

import openturns as ot


class CantileverBeam:
    """
    Data class for the cantilever beam example.


    Attributes
    ----------

    dim : int
        The dimension of the problem, dim=4.

    E : :class:`~openturns.Beta`
        Young's modulus distribution
        Beta(0.9, 3.5, 65.0e9, 75.0e9)

    F : :class:`~openturns.LogNormal`
        Load distribution
        LogNormalMuSigma()([300.0, 30.0, 0.0])

    L : :class:`~openturns.Uniform`
        Length distribution
        Uniform(2.5, 2.6)

    II : :class:`~openturns.Beta`
        Moment of inertia distribution
        Beta(2.5, 4.0, 1.3e-7, 1.7e-7)

    model : :class:`~openturns.SymbolicFunction`
        The physical model of the cantilever beam.

    R : :class:`~openturns.CorrelationMatrix`
        Correlation matrix used to define the copula.

    copula : :class:`~openturns.NormalCopula`
        Copula of the model.

    distribution : :class:`~openturns.JointDistribution`
        The joint distribution of the parameters.

    independentDistribution : :class:`~openturns.JointDistribution`
        The joint distribution of the parameters with independent copula.

    data : :class:`~openturns.Sample`
        Sample of the input distribution

    Examples
    --------
    >>> from openturns.usecases import cantilever_beam
    >>> # Load the cantilever beam model
    >>> cb = cantilever_beam.CantileverBeam()
    """

    def __init__(self):
        self.dim = 4  # number of inputs
        # Young's modulus E
        self.E = ot.Beta(0.9, 3.5, 65.0e9, 75.0e9)  # in N/m^2
        self.E.setDescription("E")
        self.E.setName("Young modulus")

        # Load F
        self.F = ot.LogNormal()  # in N
        self.F.setParameter(ot.LogNormalMuSigma()([300.0, 30.0, 0.0]))
        self.F.setDescription("F")
        self.F.setName("Load")

        # Length L
        self.L = ot.Uniform(2.5, 2.6)  # in m
        self.L.setDescription("L")
        self.L.setName("Length")

        # Moment of inertia I
        self.II = ot.Beta(2.5, 4.0, 1.3e-7, 1.7e-7)  # in m^4
        self.II.setDescription("I")
        self.II.setName("Inertia")

        # physical model
        self.model = ot.SymbolicFunction(["E", "F", "L", "I"], ["F*L^3/(3*E*I)"])

        # correlation matrix
        self.R = ot.CorrelationMatrix(self.dim)
        self.R[2, 3] = -0.2
        self.copula = ot.NormalCopula(
            ot.NormalCopula.GetCorrelationFromSpearmanCorrelation(self.R)
        )
        self.distribution = ot.JointDistribution(
            [self.E, self.F, self.L, self.II], self.copula
        )

        # special case of an independent copula
        self.independentDistribution = ot.JointDistribution(
            [self.E, self.F, self.L, self.II]
        )

        self.data = self.distribution.getSample(100)
