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

import org.openide.util.Lookup;
import uk.theretiredprogrammer.nbpcglibrary.common.LogBuilder;
import uk.theretiredprogrammer.racetrainingdemonstrator.core.ScenarioElement;

/**
 * The Abstract DisplayableElement Factory Class.
 *
 * @author Richard Linsdale (richard at theretiredprogrammer.uk)
 */
public abstract class FlowElementFactory {

    /**
     * createInstance an element
     *
     * @param instancename the objectname
     * @param scenario the scenario to be applied
     * @return the element
     */
    public abstract FlowElement createInstance(String instancename, ScenarioElement scenario);

    /**
     * createInstance the classrefname of this FlowElement class.
     *
     * @return the classrefname name
     */
    public abstract String getName();

    /**
     * Create a new Instance of the target class.
     *
     * @param classrefname the classrefname of the target class
     * @param instancename the name to be given to the instance
     * @param scenario the scenario to be applied
     * @return the new instance of the target class
     */
    public static FlowElement createInstance(String classrefname, String instancename, ScenarioElement scenario) {
        LogBuilder.writeLog("racetrainingdemonstrator", "BoatFactory", "createInstance", instancename, classrefname);
        for (FlowElementFactory factory : Lookup.getDefault().lookupAll(FlowElementFactory.class)) {
            if (factory.getName().equals(classrefname)) {
                return factory.createInstance(instancename, scenario);
            }
        }
        return null;
    }

}
