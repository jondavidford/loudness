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

#ifndef MODULE_H
#define MODULE_H

#include "TrackBank.h"

namespace loudness{

    /**
     * @class Module
     * 
     * @brief Abstract class for all modules.
     *
     * All modules are derived from Module. Derived modules must implement the
     * pure virtual functions initializeInternal(), processInternal() and
     * resetInternal(). 
     *
     * After calling a module's constructor with the appropriate input
     * arguments, the module can be initialised by passing a reference to the
     * input TrackBank to initialize(), which in turn calls
     * initializeInternal() where the bulk of the work takes place. Upon
     * successful initialisation, an output TrackBank is setup for storing the
     * processing result. If a module has a target module (see
     * setTargetModule()), then it is also initialised by passing the output
     * TrackBank to the input of the target module function
     * initializeInternal(). This allows the ins and outs of modules to be
     * connected and thus create a processing pipeline. 
     *
     * To process a TrackBank, pass a reference to the process() function.
     * This will call the module specific processInternal() function which will
     * pass the processed TrackBank (output) to it's target for further
     * processing (input). The input TrackBank passed to process() must have
     * the same number channels, number of samples and centre frequencies as the
     * one used to initialise the module. Some modules do not take an input
     * TrackBank but instead generate their own input. In this case,
     * initialize() and process() are called with no arguments. Note that
     * processInternal() is only called if the TrackBank trigger is 1 (which is
     * the default), otherwise the output bank will not be updated.
     *
     * @author Dominic Ward
     *
     * @sa TrackBank
     */
    class Module
    {
    public:
        Module(string name = "Module");
        virtual ~Module();

        /**
         * @brief Initialises a module with no input.
         *
         * This is used for modules which generate their own input for
         * processing. This is virtual to enable such modules to override the
         * default function (which does nothing).
         *
         * @return true if module has been initialised, false otherwise.
         */
        virtual bool initialize();

        /**
         * @brief Initialises a module with an input TrackBank.
         *
         * This calls initializeInternal() and checks for a target if
         * successfully initialised. If a target module exists, it too is
         * initialised.
         *
         * @param input The input TrackBank for processing.
         *
         * @return true if module has been initialised, false otherwise.
         */
        virtual bool initialize(const TrackBank &input);

        /**
         * @brief Processes a self-generated input.
         *
         * As with initialize() this function is virtual for overriding the
         * default function. This is useful for modules that generate their own
         * input.
         */
        virtual void process();

        /**
         * @brief Processes the input TrackBank.
         *
         * @param input The input TrackBank to be processed. Must be same
         * structure as the one used to initialise the module.
         */
        virtual void process(const TrackBank &input);

        /**
         * @brief Restores a module to intialisation state and clears the
         * contents of it's output TrackBank.
         */
        void reset();

        void resize(int nTracks);

        /**
         * @brief Adds a single target module to the object.  
         *
         * Once processed, the output TrackBank is passed as input to the
         * target module for processing. Note that the target module must
         * continue to exist for the lifetime of the aggregate object when
         * initialize(), process() or reset() are called.
         *
         * @param targetModule Pointer to the target module.
         */
        void setTargetModule(Module *targetModule);
        
        /**
         * @brief Removes the target module.
         */
        void removeTargetModule();

        /**
         * @brief Returns the module initialisation state.
         *
         * @return true if initialised, false otherwise.
         */
        bool isInitialized() const;

        /**
         * @brief Returns a pointer to the output TrackBank used for storing the
         * processing result.
         *
         * @return TrackBank pointer.
         */
        const TrackBank* getOutput() const;

        /**
         * @brief Returns the name of the module.
         *
         * @return String corresponding to the module name.
         */
        const string& getName() const;

    protected:
        //Pure virtual functions
        virtual bool initializeInternal(const TrackBank &input) = 0;
        virtual void processInternal(const TrackBank &input) = 0;
        virtual void resetInternal() = 0;
        //virtual void resizeInternal(int nTracks) = 0;

        //members
        bool initialized_;
        Module *targetModule_;
        TrackBank output_;
        string name_;
    };
}

#endif
