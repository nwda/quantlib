

/*
 Copyright (C) 2000, 2001, 2002, 2003 RiskMap srl

 This file is part of QuantLib, a free-software/open-source library
 for financial quantitative analysts and developers - http://quantlib.org/

 QuantLib is free software: you can redistribute it and/or modify it under the
 terms of the QuantLib license.  You should have received a copy of the
 license along with this program; if not, please email ferdinando@ametrano.net
 The license is also available online at http://quantlib.org/html/license.html

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the license for more details.
*/
/*! \file mceverest.cpp
    \brief %Everest-type option pricer

    \fullpath
    ql/Pricers/%mceverest.cpp
*/

// $Id$

#include <ql/handle.hpp>
#include <ql/MonteCarlo/everestpathpricer.hpp>
#include <ql/MonteCarlo/mctypedefs.hpp>
#include <ql/Pricers/mceverest.hpp>


namespace QuantLib {

    namespace Pricers {

        using Math::Statistics;
        using Math::Matrix;
        using MonteCarlo::MultiPath;
        using MonteCarlo::GaussianMultiPathGenerator;
        using MonteCarlo::PathPricer_old;
        using MonteCarlo::MonteCarloModel;
        using MonteCarlo::EverestPathPricer_old;

        McEverest::McEverest(const Array& dividendYield,
            const Matrix& covariance,
            Rate riskFreeRate, Time residualTime,
            bool antitheticVariance, long seed) {

            Size  n = covariance.rows();
            QL_REQUIRE(covariance.columns() == n,
                "McEverest: covariance matrix not square");
            QL_REQUIRE(dividendYield.size() == n,
                "McEverest: dividendYield size does not match"
                " that of covariance matrix");
            QL_REQUIRE(residualTime > 0,
                "McEverest: residualTime must be positive");

            //! Initialize the path generator
            Array mu(riskFreeRate - dividendYield
                                    - 0.5 * covariance.diagonal());

            Handle<GaussianMultiPathGenerator> pathGenerator(
                new GaussianMultiPathGenerator(mu, covariance,
                TimeGrid(residualTime, 1), seed));

            //! Initialize the pricer on the path pricer
            Handle<PathPricer_old<MultiPath> > pathPricer(
                new EverestPathPricer_old(QL_EXP(-riskFreeRate*residualTime),
                antitheticVariance));

             //! Initialize the multi-factor Monte Carlo
            mcModel_ = Handle<MonteCarloModel<Statistics,
                GaussianMultiPathGenerator, PathPricer_old<MultiPath> > > (
                new MonteCarloModel<Statistics,
                GaussianMultiPathGenerator, PathPricer_old<MultiPath> > (
                pathGenerator, pathPricer, Statistics(), false));

        }

    }

}
