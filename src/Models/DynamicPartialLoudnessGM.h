/*
 * Copyright (C) 2014 Dominic Ward <contactdominicward@gmail.com>
 *
 * This file is part of Loudness
 *
 * Loudness is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Loudness is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Loudness.  If not, see <http://www.gnu.org/licenses/>. 
 */

#ifndef DynamicPartialLoudnessGM_H
#define DynamicPartialLoudnessGM_H

#include "../Support/Model.h"

namespace loudness{

    /**
     * @class DynamicPartialLoudnessGM
     *
     * @brief Implements Glasberg and Moore's time-varying partial loudness model.
     *
     * At present, there are two parameter sets available:
     *
     * 1. GM02 - The specification used by Glasberg and Moore (2002).
     * 2. FASTER1 - Uses a compressed spectrum with a fast roex filterbank.
     *
     * The default is FASTER1.
     *
     * Use loadParameterSet() to select the model parameters.
     *
     * Note that input to the model must be a TrackBank containing 4 tracks.
     * The first 2 tracks are right and left channels of the target signal, respectively.
     * The third and fourth tracks are the right and left channels of the masking
     * signal, respectively.
     *
     * When using filter spacings greater than 0.1 Cams, the sampled excitation
     * pattern can be interpolated to approximate the high resolution pattern.
     * If you want this, use setInterpRoexBank(true).
     *
     * REFERENCES:
     *
     * Glasberg, B. R., & Moore, B. C. J. (1990). Derivation of Auditory Filter
     * Shapes From Notched-Noise Data. Hearing Research, 47, 103–138.
     *
     * Moore, B. C. J., Glasberg, B. R., & Baer, T. (1997). A Model for the
     * Prediction of Thresholds, Loudness, and Partial Loudness. Journal of the
     * Audio Engineering Society, 45(4), 224–240.
     *
     * Glasberg, B. R., & Moore, B. C. J. (2002). A Model of Loudness Applicable
     * to Time-Varying Sounds. Journal of the Audio Engineering Society, 50(5),
     * 331–342.
     *
     * Glasberg, B. R., & Moore, B. C. J. (2006). Prediction of Absolute
     * Thresholds and Equal-Loudness Contours Using a Modified Loudness Model.
     * The Journal of the Acoustical Society of America, 120(2), 585–588.
     *
     * ANSI S3.4-2007. Procedure for the Computation of Loudness of Steady
     * Sounds.
     */
    class DynamicPartialLoudnessGM : public Model
    {
        public:

            enum ParameterSet{
                GM02 = 0 /**< Glasberg and Moore 2002. */,
                FASTER1 = 1 /**< Compressed spectrum and fast roex filterbank. */
            };

            /**
             * @brief Constructs a model with a path to the '.npy' file holding
             * the pre-cochlear filter coefficients.
             *
             * If no path is given, the hybrid filter will
             * perform the outer and middle ear filtering.
             */
            DynamicPartialLoudnessGM(string pathToFilterCoefs = "");

            virtual ~DynamicPartialLoudnessGM();

            /**
             * @brief Loads a parameter set.
             */
            void loadParameterSet(ParameterSet set);

            void setTimeStep(Real timeStep);
            void setHpf(bool hpf);
            void setDiffuseField(bool diffuseField);
            void setGoertzel(bool goertzel);
            void setDiotic(bool diotic);
            void setUniform(bool uniform);
            void setInterpRoexBank(bool interpRoexBank);
            void setFilterSpacing(Real filterSpacing);
            void setCompressionCriterion(Real compressionCriterion);
            void setAnsiBank(bool ansiBank);
            void setPathToFilterCoefs(string pathToFilterCoefs);
            void setFastBank(bool fastBank);

            Real getTimeStep() const;
            
        private:
            virtual bool initializeInternal(const TrackBank &input);

            int outerEarType_;
            Real timeStep_, filterSpacing_, compressionCriterion_;
            bool ansiBank_, fastBank_, interpRoexBank_, uniform_, diotic_, goertzel_;
            bool hpf_, diffuseField_;
            string pathToFilterCoefs_;

    }; 
}

#endif

