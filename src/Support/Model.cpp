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

#include "Model.h"

namespace loudness{

    Model::Model(string name, bool dynamicModel) :
        name_(name),
        dynamicModel_(dynamicModel)
    {
        LOUDNESS_DEBUG(name_ << ": Constructed.");
    }

    Model::~Model() {}

    bool Model::initialize(const TrackBank &input)
    {
        if(!initializeInternal(input))
        {
            LOUDNESS_ERROR(name_ <<
                    ": Not initialised!");
            return 0;
        }
        else
        {
            LOUDNESS_DEBUG(name_ << ": initialised.");

            //set up the chain
            nModules_ = (int)modules_.size();
            for(int i=0; i<nModules_-1; i++)
                modules_[i]->setTargetModule(modules_[i+1].get());
            //initialise all
            modules_[0]->initialize(input);

            LOUDNESS_DEBUG(name_ 
                    << ": Targets set and all modules initialised.");

            initialized_ = 1;
            return 1;
        }
    }

    void Model::process(const TrackBank &input)
    {
        if(initialized_)
            modules_[0]->process(input);
        else
            LOUDNESS_WARNING(name_ << ": Not initialised!");
    }

    void Model::reset()
    {
        modules_[0]->reset();
    }

    void Model::resize(int nTracks)
    {
        modules_[0]->resize(nTracks);
    }

    const TrackBank* Model::getModuleOutput(int module) const
    {
        if (module<nModules_)
            return modules_[module]->getOutput();
        else
            return 0;
    }

    /*
     * Decided not to return const ref because of bounds checking.
     * Just return copy instead.
     */
    string Model::getModuleName(int module) const
    {
        if(module < (int)modules_.size())
            return modules_[module] -> getName();
        else
        {
            LOUDNESS_ERROR(name_ << ": index out of bounds.");
            return "";
        }
    }

    const string& Model::getName() const
    {
        return name_;
    }

    bool Model::isInitialized() const
    {
        return initialized_;
    }

    bool Model::isDynamicModel() const
    {
        return dynamicModel_;
    }

    int Model::getNModules() const
    {
        return nModules_;
    }            
}







