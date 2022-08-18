/*
 * Copyright 2014-2017 Richard Linsdale.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
package uk.theretiredprogrammer.racetrainingdemonstrator.core.api;

import uk.theretiredprogrammer.racetrainingdemonstrator.core.ScenarioElement;


/**
 * Abstract Class describing a Flow Model. A Flow Model represents variable
 * flows over a area.
 *
 * @author Richard Linsdale (richard at theretiredprogrammer.uk)
 */
abstract public class FlowElement extends DisplayableElement {
    
    final protected ScenarioElement scenario;

    /**
     * Constructor
     *
     * @param name the flow name - either wind or water
     * @param scenario the scenario to be applied
     */
    public FlowElement(String name, ScenarioElement scenario) {
        super(name);
        this.scenario = scenario;
    }

    /**
     * Get the flow at a point in the flowmodel at the current time.
     *
     * @param pos the point at which the flow is to be calculated
     * @return the flow
     */
    public abstract Flow getFlow(Location pos);

    /**
     * Get the mean value for the flow across the entire flow model.
     *
     * @return the mean flow
     */
    public abstract Flow getMeanFlow();
}
