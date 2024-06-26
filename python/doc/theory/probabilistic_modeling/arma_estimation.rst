.. _arma_estimation:

ARMA process estimation
-----------------------

From the order :math:`(p,q)` or a range of orders :math:`(p,q) \in Ind_p \times Ind_q`,
where :math:`Ind_p = [p_1, p_2]`
and :math:`Ind_q = [q_1, q_2]`, the methods aims to find the *best* model
:math:`ARMA(p,q)` that fits the data and estimate the
corresponding coefficients. The *best* model is considered with
respect to the :math:`AIC_c` criteria (corrected Akaike Information
Criterion), defined by:

.. math::

      AICc = -2\log L_w + 2(p + q + 1)\frac{m}{m - p - q - 2}

where :math:`m` is half the number of points of the time grid of the
process sample (if the data are a process sample) or in a block of the
time series (if the data are a time series).

Two other criteria are computed for each order :math:`(p,q)`:

-  the AIC criterion:

   .. math::

      AIC = -2\log L_w + 2(p + q + 1)

-  and the BIC criterion:

   .. math::

      BIC = -2\log L_w + 2(p + q + 1)\log(p + q + 1)

The *BIC* criterion leads to a model that gives a better prediction;
the *AIC* criterion selects the best model that fits the given data;
the :math:`AIC_c` criterion improves the previous one by penalizing a
too high order that would artificially fit to the data.
For each order :math:`(p,q)`, the estimation of the coefficients
:math:`(a_k)_{1\leq k\leq p}`, :math:`(b_k)_{1\leq k\leq q}` and the
variance :math:`\sigma^2` is done using the Whittle estimator which is
based on the maximization of the likelihood function in the frequency
domain.
The principle is detailed hereafter for the case of a time series: in
the case of a process sample, the estimator is similar except for the
periodogram which is computed differently.
We consider a time series associated to the time grid
:math:`(t_0, \hdots, t_{n-1})` and a particular order :math:`(p,q)`.
The spectral density function of the
:math:`ARMA(p,q)` process writes:

.. math::
  :label: arma_spectrum

    f(\lambda, \vect{\theta}, \sigma^2) = \frac{\sigma^2}{2 \pi} \frac{|1 + b_1 \exp(-i \lambda) + \ldots
    + b_q \exp(-i q \lambda)|^2}{|1 + a_1 \exp(-i \lambda) + \ldots + a_p \exp(-i p \lambda)|^2} =
    \frac{\sigma^2}{2 \pi} g(\lambda, \vect{\theta})

where :math:`\vect{\theta} = (a_1, a_2,...,a_p,b_1,...,b_q)` and
:math:`\lambda` is the frequency value.

The Whittle log-likelihood writes:

.. math::
  :label: arma_likelihood

    \log L_w(\vect{\theta}, \sigma^2) = - \sum_{j=1}^{m} \log f(\lambda_j,  \vect{\theta}, \sigma^2) - \frac{1}{2 \pi} \sum_{j=1}^{m} \frac{I(\lambda_j)}{f(\lambda_j,  \vect{\theta}, \sigma^2)}

where:

-  :math:`I` is the non parametric estimate of the spectral density,
   expressed in the Fourier space (frequencies in :math:`[0,2\pi]`
   instead of :math:`[-f_{max}, f_{max}]`). By default the Welch estimator is used.

-  :math:`\lambda_j` is the :math:`j-th` Fourier frequency,
   :math:`\lambda_j = \frac{2 \pi j}{n}`, :math:`j=1 \ldots m` with
   :math:`m` the largest integer :math:`\leq \frac{n-1}{2}`.

We estimate the :math:`(p+q+1)` scalar coefficients by maximizing the
log-likelihood function. The corresponding equations lead to the
following relation:

.. math::
  :label: optSigma

    \sigma^{2*} = \frac{1}{m} \sum_{j=1}^{m} \frac{I(\lambda_j)}{g(\lambda_j, \vect{\theta}^{*})}

where :math:`\vect{\theta}^{*}` maximizes:

.. math::
  :label: lik2

    \log L_w(\vect{\theta}) = m (\log(2 \pi) - 1) - m \log\left( \frac{1}{m} \sum_{j=1}^{m} \frac{I(\lambda_j)}{g(\lambda_j, \vect{\theta})}\right) - \sum_{j=1}^{m} g(\lambda_j, \vect{\theta})

The Whitle estimation requires that:

-  the determinant of the eigenvalues of the companion matrix
   associated to the polynomial :math:`1 + a_1X + \dots + a_pX^p` are
   outside the unit disc,, which guarantees the stationarity of the
   process;

-  the determinant of the eigenvalues of the companion matrix
   associated to the polynomial :math:`1 + ba_1X + \dots + b_qX^q` are
   outside the unit disc, which guarantees the invertibility of the
   process.

Multivariate estimation
~~~~~~~~~~~~~~~~~~~~~~~

Let :math:`(t_i, \vect{X}_i)_{0\leq i \leq n-1}` be a multivariate
time series of dimension :math:`d` generated by an ARMA process
where :math:`(p,q)` are supposed to
be known. We assume that the white noise :math:`\varepsilon` is
distributed according to the normal distribution with zero mean and
with covariance matrix
:math:`\mat{\Sigma}_{\varepsilon} = \sigma^2\mat{Q}` where
:math:`|\mat{Q}| = 1` .
The normality of the white noise implies the normality of the process.
If we note :math:`\vect{W} = (\vect{X}_0, \hdots, \vect{X}_{n-1})`,
then :math:`\vect{W}` is normal with zero mean. Its covariance matrix
writes
:math:`\mathbb{E}(\vect{W}\vect{W}^t)= \sigma^2 \Sigma_{\vect{W}}`
which depends on the coefficients :math:`(\mat{A}_k, \mat{B}_l)` for
:math:`k = 1,\ldots,p` and :math:`l = 1,\ldots, q` and on the matrix
:math:`\mat{Q}`.

The likelihood of :math:`\vect{W}` writes:

.. math::
  :label: likelihoodMV

    L(\vect{\beta}, \sigma^2 | \vect{W}) = (2 \pi \sigma^2) ^{-\frac{d n}{2}} |\Sigma_{w}|^{-\frac{1}{2}} \exp\left(- (2\sigma^2)^{-1}  \vect{W}^t \Sigma_{\vect{W}}^{-1}  \vect{W} \right)

where
:math:`\vect{\beta} = (\mat{A}_{k}, \mat{B}_{l}, \mat{Q}),\ k = 1,\ldots,p`,
:math:`l = 1,\ldots, q` and where :math:`|.|` denotes the determinant.

The difficulty arises from the great size (:math:`dn \times dn`) of
:math:`\Sigma_{\vect{W}}` which is a dense matrix in the general case.
[mauricio1995]_ proposes an efficient algorithm to evaluate the likelihood
function. The main point is to use a change of variable that leads to a
block-diagonal sparse covariance matrix.

The multivariate Whittle estimation requires that:

-  the determinant of the eigenvalues of the companion matrix
   associated to the polynomial
   :math:`\mat{I} + \mat{A}_1\mat{X} + \dots + \mat{A}_p\mat{X}^p` are
   outside the unit disc, which guarantees the stationarity of the
   process;

-  the determinant of the eigenvalues of the companion matrix
   associated to the polynomial
   :math:`\mat{I} + \mat{B}_1\mat{X} + \dots + \mat{B}_q\mat{X}^q` are
   outside the unit disc, which guarantees the invertibility of the
   process.

.. topic:: API:

    - See :class:`~openturns.WhittleFactory`
    - See :class:`~openturns.WelchFactory`
    - See :class:`~openturns.ARMA`

.. topic:: Examples:

    - See :doc:`/auto_data_analysis/estimate_stochastic_processes/plot_estimate_arma`
    - See :doc:`/auto_data_analysis/estimate_stochastic_processes/plot_estimate_multivariate_arma`
