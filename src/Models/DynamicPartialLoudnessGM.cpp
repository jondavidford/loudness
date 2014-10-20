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

#include "../cnpy/cnpy.h"
#include "../Support/AuditoryTools.h"
#include "../Modules/FrameGenerator.h"
#include "../Modules/Butter.h"
#include "../Modules/FIR.h"
#include "../Modules/IIR.h"
#include "../Modules/PowerSpectrum.h"
#include "../Modules/CompressSpectrum.h"
#include "../Modules/WeightSpectrum.h"
#include "../Modules/FastRoexBank.h"
#include "../Modules/RoexBankANSIS3407.h"
#include "../Modules/SpecificLoudnessGM.h"
#include "../Modules/SpecificPartialLoudnessGM.h"
#include "../Modules/IntegratedLoudnessGM.h"
#include "DynamicPartialLoudnessGM.h"

namespace loudness{

    DynamicPartialLoudnessGM::DynamicPartialLoudnessGM(string pathToFilterCoefs, int nTracks) :
        Model("DynamicPartialLoudnessGM", true),
        pathToFilterCoefs_(pathToFilterCoefs)
    {
        loadParameterSet(FASTER1);
    }
    

    DynamicPartialLoudnessGM::~DynamicPartialLoudnessGM()
    {
    }

    void DynamicPartialLoudnessGM::setTimeStep(Real timeStep)
    {
        timeStep_ = timeStep;
    }

    void DynamicPartialLoudnessGM::setHpf(bool hpf)
    {
        hpf_ = hpf;
    }
    void DynamicPartialLoudnessGM::setDiffuseField(bool diffuseField)
    {
        diffuseField_ = diffuseField;
    }
    void DynamicPartialLoudnessGM::setGoertzel(bool goertzel)
    {
        goertzel_ = goertzel;
    }
    void DynamicPartialLoudnessGM::setDiotic(bool diotic)
    {
        diotic_ = diotic;
    }
    void DynamicPartialLoudnessGM::setUniform(bool uniform)
    {
        uniform_ = uniform;
    }
    void DynamicPartialLoudnessGM::setInterpRoexBank(bool interpRoexBank)
    {
        interpRoexBank_ = interpRoexBank;
    }
    void DynamicPartialLoudnessGM::setFilterSpacing(Real filterSpacing)
    {
        filterSpacing_ = filterSpacing;
    }
    void DynamicPartialLoudnessGM::setCompressionCriterion(Real compressionCriterion)
    {
        compressionCriterion_ = compressionCriterion;
    }
    void DynamicPartialLoudnessGM::setAnsiBank(bool ansiBank)
    {
        ansiBank_ = ansiBank;
    }
    void DynamicPartialLoudnessGM::setPathToFilterCoefs(string pathToFilterCoefs)
    {
        pathToFilterCoefs_ = pathToFilterCoefs;
    }

    void DynamicPartialLoudnessGM::setFastBank(bool fastBank)
    {
        fastBank_ = fastBank;
    }

    void DynamicPartialLoudnessGM::setNTracks(int nTracks)
    {
        nTracks_ = nTracks
    }

    Real DynamicPartialLoudnessGM::getTimeStep() const
    {
        return timeStep_;
    }

    void DynamicPartialLoudnessGM::loadParameterSet(ParameterSet set)
    {
        //common to all
        setTimeStep(0.001);
        setHpf(true);
        setDiffuseField(false);
        setGoertzel(false);
        setDiotic(true);
        setUniform(true);
        setInterpRoexBank(false);
        setFilterSpacing(0.25);
        setCompressionCriterion(0.0);
        setFastBank(false);

        switch(set){
            case GM02:
                break;
            case FASTER1:
                setFastBank(true);
                setCompressionCriterion(0.3);
                break;
            default:
                setFastBank(true);
                setCompressionCriterion(0.3);
        }
    }

    bool DynamicPartialLoudnessGM::initializeInternal(const SignalBankBank &input)
    {
        /*
         * Outer-Middle ear filter 
         */  

        //if filter coefficients have not been provided
        //use spectral weighting to approximate outer and middle ear response
        bool weightSpectrum = false;
        if(pathToFilterCoefs_.empty())
        {
            LOUDNESS_WARNING(name_
                    << ": No filter coefficients, opting to weight power spectrum.");

            weightSpectrum = true;

            //should we use for HPF for low freqs? default is true
            if(hpf_)
            {
                LOUDNESS_DEBUG(name_ << ": Using HPF.");
                modules_.push_back(unique_ptr<Module> (new Butter(3, 0, 50.0)));
            }
        }
        else { //otherwise, load them

            //load numpy array holding the filter coefficients
            cnpy::NpyArray arr = cnpy::npy_load(pathToFilterCoefs_);
            Real *data = reinterpret_cast<Real*> (arr.data);

            //check if filter is IIR or FIR
            bool iir = false;
            if(arr.shape[0]==2)
                iir = true;

            //load the coefficients
            RealVec bCoefs, aCoefs;
            for(unsigned int i=0; i<arr.shape[1];i++)
            {
                bCoefs.push_back(data[i]);
                if(iir)
                    aCoefs.push_back(data[i+arr.shape[1]]);
            }
            
            //create module
            if(iir)
                modules_.push_back(unique_ptr<Module> 
                        (new IIR(bCoefs, aCoefs))); 
            else
                modules_.push_back(unique_ptr<Module>
                        (new FIR(bCoefs))); 

            //clean up
            delete [] data;
        }

        /*
         * Frame generator for spectrogram
         */
        int windowSize = round(0.064*input.getFs());
        int hopSize = round(timeStep_*input.getFs());
        modules_.push_back(unique_ptr<Module> 
                (new FrameGenerator(windowSize, hopSize)));

        /*
         * Multi-resolution spectrogram
         */
        RealVec bandFreqsHz {10, 80, 500, 1250, 2540, 4050, 15001};

        //window spec
        RealVec windowSizeSecs {0.064, 0.032, 0.016, 0.008, 0.004, 0.002};
        
        //create module
        modules_.push_back(unique_ptr<Module> 
                (new PowerSpectrum(bandFreqsHz, windowSizeSecs, uniform_))); 

        /*
         * Compression
         */
        if(compressionCriterion_ > 0)
            modules_.push_back(unique_ptr<Module>
                    (new CompressSpectrum(compressionCriterion_))); 

        /*
         * Spectral weighting if necessary
         */
        if(weightSpectrum)
        {
            OME::MiddleEarType middleEar = OME::ANSI;
            OME::OuterEarType outerEar = OME::ANSI_FREE;
            if(hpf_)
                middleEar = OME::ANSI_HPF;
            if(diffuseField_)
                outerEar = OME::ANSI_DIFFUSE;

            modules_.push_back(unique_ptr<Module> 
                    (new WeightSpectrum(middleEar, outerEar))); 
        }

        /*
         * Roex filters
         */
        if(fastBank_)
        {
            modules_.push_back(unique_ptr<Module>
                    (new FastRoexBank(filterSpacing_, interpRoexBank_)));
        }
        else
        {
            modules_.push_back(unique_ptr<Module> 
                    (new RoexBankANSIS3407(1.8, 38.9, filterSpacing_)));
        }
        
        /*
         * Specific loudness
         */
        modules_.push_back(unique_ptr<Module>
                (new SpecificLoudnessGM));

        /*
        * Loudness integration 
        */   
        modules_.push_back(unique_ptr<Module>
                (new IntegratedLoudnessGM(diotic_, true)));

        return 1;
    }

}

