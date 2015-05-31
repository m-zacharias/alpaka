/**
* \file
* Copyright 2015 Benjamin Worpitz
*
* This file is part of alpaka.
*
* alpaka is free software: you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* alpaka is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with alpaka.
* If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include <alpaka/traits/Rand.hpp>           // CreateNormalReal, ...

#include <alpaka/devs/cuda/Dev.hpp>         // DevCuda

#include <alpaka/core/Common.hpp>           // ALPAKA_FCT_ACC_CUDA_ONLY
#include <alpaka/core/Align.hpp>            // ALPAKA_ALIGN
#include <alpaka/core/Cuda.hpp>             // ALPAKA_CUDA_RT_CHECK

#include <curand_kernel.h>                  // curand_init, ...

#include <type_traits>                      // std::enable_if

namespace alpaka
{
    namespace devs
    {
        namespace cuda
        {
            namespace rand
            {
                namespace generator
                {
                    namespace detail
                    {
                        //#############################################################################
                        //! The CUDA Xor random number generator.
                        //#############################################################################
                        class Xor
                        {
                        public:
                            //-----------------------------------------------------------------------------
                            //! Constructor.
                            //-----------------------------------------------------------------------------
                            ALPAKA_FCT_ACC_CUDA_ONLY Xor(
                                std::uint32_t const & seed,
                                std::uint32_t const & subsequence = 0,
                                std::uint32_t const & offset = 0)
                            {
                                curand_init(
                                    seed,
                                    subsequence,
                                    offset,
                                    &m_State);
                            }

                        public:
                            ALPAKA_ALIGN(curandStateXORWOW_t, m_State);
                        };
                    }
                }
                namespace distribution
                {
                    namespace detail
                    {
                        //#############################################################################
                        //! The CUDA random number floating point normal distribution.
                        //#############################################################################
                        template<
                            typename T>
                        class NormalReal;

                        //#############################################################################
                        //! The CUDA random number float normal distribution.
                        //#############################################################################
                        template<>
                        class NormalReal<
                            float>
                        {
                        public:
                            //-----------------------------------------------------------------------------
                            //! Constructor.
                            //-----------------------------------------------------------------------------
                            ALPAKA_FCT_ACC_CUDA_ONLY NormalReal() = default;

                            //-----------------------------------------------------------------------------
                            //! Call operator.
                            //-----------------------------------------------------------------------------
                            template<
                                typename TGenerator>
                            ALPAKA_FCT_ACC_CUDA_ONLY auto operator()(
                                TGenerator const & generator)
                            -> float
                            {
                                return curand_normal(generator.m_State);
                            }
                        };
                        //#############################################################################
                        //! The CUDA random number float normal distribution.
                        //#############################################################################
                        template<>
                        class NormalReal<
                            double>
                        {
                        public:
                            //-----------------------------------------------------------------------------
                            //! Constructor.
                            //-----------------------------------------------------------------------------
                            ALPAKA_FCT_ACC_CUDA_ONLY NormalReal() = default;

                            //-----------------------------------------------------------------------------
                            //! Call operator.
                            //-----------------------------------------------------------------------------
                            template<
                                typename TGenerator>
                            ALPAKA_FCT_ACC_CUDA_ONLY auto operator()(
                                TGenerator const & generator)
                            -> double
                            {
                                return curand_normal_double(generator.m_State);
                            }
                        };

                        //#############################################################################
                        //! The CUDA random number floating point uniform distribution.
                        //#############################################################################
                        template<
                            typename T>
                        class UniformReal;

                        //#############################################################################
                        //! The CUDA random number float uniform distribution.
                        //#############################################################################
                        template<>
                        class UniformReal<
                            float>
                        {
                        public:
                            //-----------------------------------------------------------------------------
                            //! Constructor.
                            //-----------------------------------------------------------------------------
                            ALPAKA_FCT_ACC_CUDA_ONLY UniformReal() = default;

                            //-----------------------------------------------------------------------------
                            //! Call operator.
                            //-----------------------------------------------------------------------------
                            template<
                                typename TGenerator>
                            ALPAKA_FCT_ACC_CUDA_ONLY auto operator()(
                                TGenerator const & generator)
                            -> float
                            {
                                // (0.f, 1.0f]
                                float const fUniformRand(curand_uniform(generator.m_State));
                                // NOTE: (1.0f - curand_uniform) does not work, because curand_uniform seems to return denormalized floats around 0.f.
                                // [0.f, 1.0f)
                                return fUniformRand * static_cast<float>( fUniformRand != 1.0f );
                            }
                        };
                        //#############################################################################
                        //! The CUDA random number float uniform distribution.
                        //#############################################################################
                        template<>
                        class UniformReal<
                            double>
                        {
                        public:
                            //-----------------------------------------------------------------------------
                            //! Constructor.
                            //-----------------------------------------------------------------------------
                            ALPAKA_FCT_ACC_CUDA_ONLY UniformReal() = default;

                            //-----------------------------------------------------------------------------
                            //! Call operator.
                            //-----------------------------------------------------------------------------
                            template<
                                typename TGenerator>
                            ALPAKA_FCT_ACC_CUDA_ONLY auto operator()(
                                TGenerator const & generator)
                            -> double
                            {
                                // (0.f, 1.0f]
                                double const fUniformRand(curand_uniform_double(generator.m_State));
                                // NOTE: (1.0f - curand_uniform_double) does not work, because curand_uniform_double seems to return denormalized floats around 0.f.
                                // [0.f, 1.0f)
                                return fUniformRand * static_cast<double>( fUniformRand != 1.0f );
                            }
                        };

                        //#############################################################################
                        //! The CUDA random number integer uniform distribution.
                        //#############################################################################
                        template<
                            typename T>
                        class UniformUint;

                        //#############################################################################
                        //! The CUDA random number unsigned integer uniform distribution.
                        //#############################################################################
                        template<>
                        class UniformUint<
                            unsigned int>
                        {
                        public:
                            //-----------------------------------------------------------------------------
                            //! Constructor.
                            //-----------------------------------------------------------------------------
                            ALPAKA_FCT_ACC_CUDA_ONLY UniformUint() = default;

                            //-----------------------------------------------------------------------------
                            //! Call operator.
                            //-----------------------------------------------------------------------------
                            template<
                                typename TGenerator>
                            ALPAKA_FCT_ACC_CUDA_ONLY auto operator()(
                                TGenerator const & generator)
                            -> unsigned int
                            {
                                return curand(generator.m_State);
                            }
                        };
                    }
                }
            }
        }
    }

    namespace traits
    {
        namespace rand
        {
            namespace distribution
            {
                //#############################################################################
                //! The CUDA random number float normal distribution get trait specialization.
                //#############################################################################
                template<
                    typename TAcc,
                    typename T>
                struct CreateNormalReal<
                    TAcc,
                    T,
                    typename std::enable_if<
                        std::is_same<
                            alpaka::dev::DevT<TAcc>,
                            alpaka::devs::cuda::DevCuda>::value
                        && std::is_floating_point<T>::value>::type>
                {
                    ALPAKA_FCT_ACC_CUDA_ONLY static auto createNormalReal(
                        TAcc const & acc)
                    -> alpaka::devs::cuda::rand::distribution::detail::NormalReal<T>
                    {
                        return alpaka::devs::cuda::rand::distribution::detail::NormalReal<T>();
                    }
                };
                //#############################################################################
                //! The CUDA random number float uniform distribution get trait specialization.
                //#############################################################################
                template<
                    typename TAcc,
                    typename T>
                struct CreateUniformReal<
                    TAcc,
                    T,
                    typename std::enable_if<
                        std::is_same<
                            alpaka::dev::DevT<TAcc>,
                            alpaka::devs::cuda::DevCuda>::value
                        && std::is_floating_point<T>::value>::type>
                {
                    ALPAKA_FCT_ACC_CUDA_ONLY static auto createUniformReal(
                        TAcc const & acc)
                    -> alpaka::devs::cuda::rand::distribution::detail::UniformReal<T>
                    {
                        return alpaka::devs::cuda::rand::distribution::detail::UniformReal<T>();
                    }
                };
                //#############################################################################
                //! The CUDA random number integer uniform distribution get trait specialization.
                //#############################################################################
                template<
                    typename TAcc,
                    typename T>
                struct CreateUniformUint<
                    TAcc,
                    T,
                    typename std::enable_if<
                        std::is_same<
                            alpaka::dev::DevT<TAcc>,
                            alpaka::devs::cuda::DevCuda>::value
                        && std::is_integral<T>::value>::type>
                {
                    ALPAKA_FCT_ACC_CUDA_ONLY static auto createUniformUint(
                        TAcc const & acc)
                    -> alpaka::devs::cuda::rand::distribution::detail::UniformUint<T>
                    {
                        return alpaka::devs::cuda::rand::distribution::detail::UniformUint<T>();
                    }
                };
            }

            namespace generator
            {
                //#############################################################################
                //! The CUDA random number default generator get trait specialization.
                //#############################################################################
                template<
                    typename TAcc>
                struct CreateDefault<
                    TAcc,
                    typename std::enable_if<
                        std::is_same<
                            alpaka::dev::DevT<TAcc>,
                            alpaka::devs::cuda::DevCuda>::value>::type>
                {
                    ALPAKA_FCT_ACC_CUDA_ONLY static auto createDefault(
                        TAcc const & acc,
                        std::uint32_t const & seed,
                        std::uint32_t const & subsequence)
                    -> alpaka::devs::cuda::rand::generator::detail::Xor
                    {
                        return alpaka::devs::cuda::rand::generator::detail::Xor(
                            seed,
                            subsequence);
                    }
                };
            }
        }
    }
}